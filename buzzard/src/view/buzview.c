/*
   File:    buzview.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 21, 2017
   e-mail:  dmvos2000(at)yahoo.com

   Copyright (C) 2017 Douwe Vos.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "buzview.h"
#include "buzpageview.h"
#include "buzlineview.h"
#include "../document/buzdocumentlistener.h"
#include "../document/content/buzmaterializedpage.h"
#include "../layout/buztextlayout.h"
#include "../layout/buztextlinelayout.h"
#include <gtk/gtk.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzView"
#include <asupport.h>

struct _BuzViewPrivate {
	BuzDocument *document;
	BuzEnrichmentSlot view_slot;
	BuzRevisionAnchored *revision;
	int view_config_id;
	BuzLayoutContext *layout_context;
	BuzViewDimensions view_dimensions;

	long long layout_height;
	int layout_width;

	long long first_line_row;
	long long first_line_view_y;
	AArray *lines_in_view;
};

static void l_document_listener_iface_init(BuzDocumentListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE(BuzView, buz_view, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzView)
		G_IMPLEMENT_INTERFACE(BUZ_TYPE_DOCUMENT_LISTENER, l_document_listener_iface_init)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_view_class_init(BuzViewClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_view_init(BuzView *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzView *instance = BUZ_VIEW(object);
	BuzViewPrivate *priv = buz_view_get_instance_private(instance);
	a_deref(priv->document);
	a_deref(priv->revision);
	a_deref(priv->view_slot.slot_key);
	G_OBJECT_CLASS(buz_view_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_view_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}

void buz_view_construct(BuzView *view, BuzDocument *document) {
	a_object_construct((AObject *) view);
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	priv->document = a_ref(document);
	priv->revision = NULL;
	priv->view_config_id = 0;
	AString *slot_key = a_string_new();
	a_string_format(slot_key, "View-%lx", (long long) view);
	priv->view_slot.slot_key = a_string_anchor(slot_key);
	priv->revision = buz_document_get_revision_ref(document);
	buz_document_add_listener(document, (BuzDocumentListener *) view);

	a_log_debug("slot-key = %O", priv->view_slot.slot_key);

	priv->view_slot.slot_index = buz_document_register_enrichment_slot(document, priv->view_slot.slot_key);

	priv->layout_context = NULL;
	priv->lines_in_view = NULL;
}


BuzView *buz_view_new(BuzDocument *document) {
	BuzView *result = g_object_new(BUZ_TYPE_VIEW, NULL);
	buz_view_construct(result, document);
	return result;
}

void buz_view_set_layout_context(BuzView *view, BuzLayoutContext *layout_context) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	if(priv->layout_context == layout_context) {
		return;
	}
	a_swap_ref(priv->layout_context, layout_context);
	priv->view_config_id++;

	// TODO this should invalidate all view data
}

void buz_view_set_top(BuzView *view, long long top) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	priv->view_dimensions.top = top;
}


void buz_view_set_view_size(BuzView *view, int width, int height) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	priv->view_dimensions.width = width;
	priv->view_dimensions.height = height;
}

const BuzViewDimensions buz_view_get_dimensions(BuzView *view) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	return priv->view_dimensions;
}

long long buz_view_get_layout_height(BuzView *view) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	return priv->layout_height;
}

int buz_view_get_layout_width(BuzView *view) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	return priv->layout_width;
}

BuzPageView *l_create_page_view(BuzView *view, BuzPageAnchored *page) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	BuzPageView *result = buz_page_view_new();
	int row_count = buz_page_row_count(page);
	int font_height = buz_layout_context_get_height(priv->layout_context);
	buz_page_view_set_height(result, row_count*font_height);
	return result;
}

void buz_view_update_lines(BuzView *view) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	if (priv->revision==NULL) {
		a_unref(priv->lines_in_view);
	}
	AIterator *page_iter = buz_revision_page_iterator(priv->revision);
	a_log_debug("page-count=%d", buz_revision_page_count(priv->revision));

	long long top = priv->view_dimensions.top;
	long long bottom = priv->view_dimensions.top + priv->view_dimensions.height;
	long long offset_y = 0;
	long long row_number = 0;

	AArray *lines_in_view = a_array_new();
	long long first_line_view_y = 0;
	long long first_line_row = 0;

	int max_width = 0;



	BuzPageAnchored *page = NULL;
	while(a_iterator_next(page_iter, (gconstpointer *) &page)) {
		BuzPageView *page_view = (BuzPageView *) buz_page_get_slot_content_ref(page, &priv->view_slot);
		if (page_view==NULL) {
			page_view = l_create_page_view(view, page);
			buz_page_set_slot_content(page, &priv->view_slot, page_view);
			a_unref(page_view);
		}



		int row_count = buz_page_row_count(page);
		int page_height = buz_page_view_get_height(page_view);
		a_log_debug("page_height=%d, page=%p, page_view=%p", page_height, page, page_view);
		long long offset_next_y = offset_y+page_height;
		if (offset_next_y>=top) {
			if (BUZ_IS_MATERIALIZED_PAGE(page)) {
				offset_next_y = offset_y;
				BuzMaterializedPageShady *mat_page = (BuzMaterializedPageShady *) page;
				a_log_debug("row_count=%d", row_count);
				int row_idx;
				for(row_idx=0; row_idx<row_count; row_idx++) {
					BuzRowShady *row = buz_materialized_page_row_at(mat_page, row_idx);
					BuzLineView *line_view = (BuzLineView *) buz_row_get_slot_content_ref(row, &priv->view_slot);
					if (line_view == NULL) {
						line_view = buz_line_view_new();
						buz_row_set_slot_content(row, &priv->view_slot, (GObject *) line_view);
					}
					AStringShady *text = buz_row_text(row);
					buz_line_view_update(line_view, priv->layout_context, text);
					BuzTextLayout *text_layout = buz_line_view_get_text_layout(line_view);

					const BuzSize line_size = buz_text_layout_get_size(text_layout);
					if ((top<offset_next_y+line_size.height) && (bottom>offset_next_y)) {
						long line_y = offset_next_y;
						int line_count = buz_text_layout_line_count(text_layout);
						int line_idx;
						for(line_idx=0; line_idx<line_count; line_idx++) {
							BuzTextLineLayout *line_layout = buz_text_layout_line_at(text_layout, line_idx);
							const BuzSize l_size = buz_text_line_layout_get_size(line_layout);
							if ((top<line_y+l_size.height) && (bottom>line_y)){
								if (a_array_size(lines_in_view)==0) {
									first_line_view_y = line_y;
									first_line_row = row_number+row_idx;
								}
								a_array_add(lines_in_view, line_layout);
							}
							line_y += l_size.height;
							if (max_width<l_size.width) {
								max_width = l_size.width;
							}
						}
					}

					a_log_debug("row[%d]=%d", row_idx, line_size.height, offset_next_y);
					offset_next_y += line_size.height;
				}
				int new_page_height = (int) offset_next_y-offset_y;
				buz_page_view_set_height(page_view, new_page_height);
			} else {
//				// TODO page is not materialized ... we need it to materialize
			}

		}
		offset_y = offset_next_y;
		row_number += row_count;
	}

	a_swap_ref(priv->lines_in_view, lines_in_view);
	a_unref(lines_in_view);
	priv->first_line_view_y = first_line_view_y;
	priv->first_line_row = first_line_row;

	a_log_debug("%p, layout_height=%ld, offset_y=%ld, revision=%p", view, priv->layout_height, offset_y, priv->revision);

	if ((max_width!=priv->layout_width) || (offset_y!=priv->layout_height)) {
		priv->layout_width = max_width;
		priv->layout_height = offset_y;
	}

}

AArray *buz_view_get_lines(BuzView *view, long long *first_line_y_view, long long *first_line_row) {
	BuzViewPrivate *priv = buz_view_get_instance_private(view);
	 if (first_line_y_view) {
		 *first_line_y_view = priv->first_line_view_y;
	 }
	 if (first_line_row) {
		 *first_line_row = priv->first_line_row;
	 }
	 return priv->lines_in_view;
}



static void l_document_new_revision(BuzDocumentListener *listener, BuzRevisionAnchored *revision) {
	BuzView *instance = BUZ_VIEW(listener);
	BuzViewPrivate *priv = buz_view_get_instance_private(instance);
	if (priv->revision==revision) {
		return;
	}
	BuzEnrichmentDataMapAnchored *enrichment_map = buz_document_get_enrichment_map(priv->document);
	if (revision) {
		buz_revision_enrich(revision, enrichment_map);
	}
	if (priv->revision) {
		buz_revision_impoverish(priv->revision);
	}
	a_swap_ref(priv->revision , revision);
}


static void l_document_listener_iface_init(BuzDocumentListenerInterface *iface) {
	iface->newRevision = l_document_new_revision;
}
