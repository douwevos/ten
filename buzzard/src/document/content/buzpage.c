/*
   File:    buzpage.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 20, 2017
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

#include "buzpageprivate.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzPage"
#include <asupport.h>

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzPage, buz_page, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);
static void l_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index);
static void l_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map);
static void l_impoverish(BuzPageAnchored *page);

static void buz_page_class_init(BuzPageClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(BuzPageContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->clone_context_for_mutable = TRUE;
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_a_anchor_content;

	clazz->enrichmentRemap = l_enrichment_remap;
	clazz->enrich = l_enrich;
	clazz->impoverish = l_impoverish;
}

static void buz_page_init(BuzPage *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	BuzPageContext *context = (BuzPageContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
	}
	G_OBJECT_CLASS(buz_page_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

void buz_page_construct(BuzPage *page) {
	a_alt_object_construct((AAltObject *) page, TRUE);
	AAltObjectPrivate *priv = a_alt_object_private(page);
	BuzPageContext *context = (BuzPageContext *) priv->context;
	context->enrichment_data = NULL;
	context->lock = a_lock_new();
}

//
//void buz_page_debug(BuzPage *page) {
//	a_log_on_debug({
//		AAltObjectPrivate *priv = a_alt_object_private(page);
//		BuzPageContext *context = (BuzPageContext *) priv->context;
//
//		AIterator *rowsiter = a_array_iterator(context->rows);
//		BuzRowShady *row = NULL;
//		while(a_iterator_next(rowsiter, (gconstpointer *) (&row))) {
//			a_log_print("DUMP", "#row:%O", row);
//		}
//		a_unref(rowsiter);
//	});
//}

int buz_page_row_count(BuzPageShady *page) {
	return BUZ_PAGE_GET_CLASS(page)->rowCount(page);
}


void buz_page_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	BUZ_PAGE_GET_CLASS(page)->enrichmentRemap(page, old_map, new_map, action, index);
}

static void l_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	BuzPageContext *context = (BuzPageContext *) a_alt_object_private(page)->context;
	if (context->enrichment_data) {
		buz_enrichment_data_remap(context->enrichment_data, old_map, new_map, action, index);
	}
}

void buz_page_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map) {
	BUZ_PAGE_GET_CLASS(page)->enrich(page, enrichment_map);
}

static void l_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map) {
	BuzPageContext *context = (BuzPageContext *) a_alt_object_private(page)->context;
	if (context->enriched_count==0) {
		if (context->enrichment_data==NULL) {
			context->enrichment_data = buz_enrichment_data_new(enrichment_map);
		}
	}
	context->enriched_count++;
}

void buz_page_impoverish(BuzPageAnchored *page) {
	BUZ_PAGE_GET_CLASS(page)->impoverish(page);
}

static void l_impoverish(BuzPageAnchored *page) {
	BuzPageContext *context = (BuzPageContext *) a_alt_object_private(page)->context;
	if (context->enriched_count==1) {
		a_deref(context->enrichment_data);
	}
	context->enriched_count--;
}

GObject *buz_page_get_slot_content_ref(BuzPageAnchored *page, BuzEnrichmentSlot *slot) {
	BuzPageContext *context = (BuzPageContext *) a_alt_object_private(page)->context;
	return buz_enrichment_data_get_slot_content_ref(context->enrichment_data, slot);
}

void buz_page_set_slot_content(BuzPageAnchored *page, BuzEnrichmentSlot *slot, GObject *content) {
	BuzPageContext *context = (BuzPageContext *) a_alt_object_private(page)->context;
	buz_enrichment_data_set_slot_content(context->enrichment_data, slot, content);
}


A_ALT_C(BuzPage, buz_page);

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const BuzPageContext *from = (const BuzPageContext *) context_from;
	BuzPageContext *to = (BuzPageContext *) context_to;
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	BuzPageContext *context = (BuzPageContext *) priv->context;
}


static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

//	BuzPageContext *context_a = (BuzPageContext *) a_alt_object_private((AObject *) object_a)->context;
//	BuzPageContext *context_b = (BuzPageContext *) a_alt_object_private((AObject *) object_b)->context;
//	if (context_a == context_b) {
//		return TRUE;
//	}

	return TRUE;
}
