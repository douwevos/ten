/*
   File:    buzcontent.c
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

#include "buzcontent.h"
#include "buzmaterializedpage.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzContent"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	AArrayShady *pages;
	BuzEnrichmentData *volatile enrichment_data;
	int enriched_count;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzContent, buz_content, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void buz_content_class_init(BuzContentClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->clone_context_for_mutable = TRUE;
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_a_anchor_content;
}

static void buz_content_init(BuzContent *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		a_deref(context->pages);
	}
	G_OBJECT_CLASS(buz_content_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzContent *buz_content_new() {
	BuzContent *result = g_object_new(BUZ_TYPE_CONTENT, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->pages = a_array_new();
	BuzMaterializedPage *page = buz_materialized_page_new();
	a_array_add((AArray *) context->pages, page);
	a_unref(page);
	context->enrichment_data = NULL;
	context->enriched_count = 0;
	return result;
}

int buz_content_page_count(BuzContentShady *content) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	return a_array_size(context->pages);
}

AIterator *buz_content_page_iterator(BuzContentShady *content) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	return a_array_iterator(context->pages);
}

BuzRowLocation buz_content_page_at_row(BuzContentShady *content, long long row) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	AIterator *iter = a_array_iterator(context->pages);
	long long offset = 0L;
	BuzPageShady *page = NULL;
	BuzRowLocation result;
	result.page_index=-1;
	int page_index=0;
	while(a_iterator_next(iter, (gconstpointer *) (&page))) {
		int row_count = buz_page_row_count(page);
		long long page_offset_end = offset + row_count;
		if (row>=offset && row<page_offset_end) {
			result.page_index = page_index;
			break;
		}
		offset = page_offset_end;
		page_index++;
	}
	a_unref(iter);
	if (result.page_index>=0) {
		result.page_row_index = (int) (row-offset);
	}
	return result;
}

void buz_content_set_page_at(BuzContent *content, BuzPageShady *page, int page_index) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	a_log_debug("set page:content=%o, pages=%o", content, context->pages);
	a_array_set_at((AArray *) context->pages, page, page_index);
	a_log_debug("page set:content=%o, pages=%o", content, context->pages);
}


BuzMaterializedPage *buz_content_editable_page_at(BuzContent *content, int page_index) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	BuzPageShady *in_list = (BuzPageShady *) a_array_at(context->pages, page_index);
	if (BUZ_IS_MATERIALIZED_PAGE(in_list) && a_alt_object_is_mutable(in_list)) {
		return (BuzMaterializedPage *) in_list;
	}
	if (!BUZ_IS_MATERIALIZED_PAGE(in_list)) {
		// TODO
	}
	BuzMaterializedPage *result = buz_materialized_page_mutable((BuzMaterializedPageShady *) in_list);
	a_array_set_at((AArray *) context->pages, result, page_index);
	a_unref(result);
	return result;
}

void buz_content_insert(BuzContent *content, AStringShady *txt, BuzCursorShady *cursor) {
//	AContext *context = (AContext *) a_alt_object_private(content)->context;
	long long row = buz_cursor_get_row(cursor);
	BuzRowLocation row_location = buz_content_page_at_row(content, row);
	a_log_debug("insert text:%O at cursor:%O, page_idx=%d", txt, cursor, row_location.page_index);
	if (row_location.page_index>=0) {
		BuzMaterializedPage *page = buz_content_editable_page_at(content, row_location.page_index);
		BuzRow *row = buz_materialized_page_editable_row_at(page, row_location.page_row_index);
		a_log_debug("page=%O, row:%O", page, row);
//		int column = buz_cursor_get_column(cursor);
		AString *text = buz_row_editable_text(row);
		a_string_append_string(text, txt);
		a_log_debug("row.text:%O", text);

	}
}

void buz_content_enrichment_remap(BuzContent *content, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	if (context->enrichment_data) {
		buz_enrichment_data_remap(context->enrichment_data, old_map, new_map, action, index);
		if (context->pages) {
			AIterator *iter = a_array_iterator(context->pages);
			BuzPageShady *page = NULL;
			while(a_iterator_next(iter, (gconstpointer *) (&page))) {
				buz_page_enrichment_remap(page, old_map, new_map, action, index);
			}
			a_unref(iter);
		}
	}
}

void buz_content_enrich(BuzContentAnchored *content, BuzEnrichmentDataMapAnchored *enrichment_map) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	if (context->enriched_count==0) {
		context->enrichment_data = buz_enrichment_data_new(enrichment_map);
		AIterator *iter = a_array_iterator(context->pages);
		BuzPageShady *page = NULL;
		while(a_iterator_next(iter, (gconstpointer *) (&page))) {
			buz_page_enrich(page, enrichment_map);
		}
		a_unref(iter);
	}
	context->enriched_count++;
}

void buz_content_impoverish(BuzContentAnchored *content) {
	AContext *context = (AContext *) a_alt_object_private(content)->context;
	if (context->enriched_count==0) {
		a_deref(context->enrichment_data);
		AIterator *iter = a_array_iterator(context->pages);
		BuzPageShady *page = NULL;
		while(a_iterator_next(iter, (gconstpointer *) (&page))) {
			buz_page_impoverish(page);
		}
		a_unref(iter);
	}
	context->enriched_count--;
}

A_ALT_C(BuzContent, buz_content);

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->pages = a_mutable_ref(from->pages);
	to->enriched_count = 0;
	to->enrichment_data = NULL;
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	a_log_debug("pre_anchor:content=%o",context->pages);
	context->pages = a_array_anchor(context->pages);
	a_log_debug("post_anchor:content=%o",context->pages);
}

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	a_log_debug("testing same:obj_a=%p, obj_b=%p", object_a, object_b);
	if (object_a == object_b) {
		return TRUE;
	}
	a_log_debug("testing for null:obj_a=%p, obj_b=%p", object_a, object_b);
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}


	AContext *context_a = (AContext *) a_alt_object_private(object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(object_b)->context;
	a_log_debug("testing for context:context_a=%p, context_b=%p", context_a, context_b);
	if (context_a == context_b) {
		return TRUE;
	}

	a_log_debug("testing pages:pages_a=%p, pages_b=%p", context_a->pages, context_b->pages);
	return a_array_equal(context_a->pages, context_b->pages);

}
