/*
   File:    buzmaterializedpage.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Nov 11, 2017
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

#include "buzmaterializedpage.h"
#include "buzpageprivate.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzMaterializedPage"
#include <asupport.h>

struct _AContext {
	struct _BuzPageContext parent;
	AArrayShady *rows;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzMaterializedPage, buz_materialized_page, BUZ_TYPE_PAGE);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
//static void l_to_string(const AObject *object, AString *out);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static int l_row_count(BuzPageShady *page);
static void l_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index);
static void l_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map);
static void l_impoverish(BuzPageAnchored *page);

static void buz_materialized_page_class_init(BuzMaterializedPageClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;
//	o_class->toString = l_to_string;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->anchorContent = l_a_anchor_content;
	a_class->cloneContext = l_a_clone;

	BuzPageClass *p_class = BUZ_PAGE_CLASS(clazz);
	p_class->rowCount = l_row_count;
	p_class->enrichmentRemap = l_enrichment_remap;
	p_class->enrich = l_enrich;
	p_class->impoverish = l_impoverish;
}

static void buz_materialized_page_init(BuzMaterializedPage *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		a_deref(context->rows);
	}

	G_OBJECT_CLASS(buz_materialized_page_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzMaterializedPage *buz_materialized_page_new() {
	BuzMaterializedPage *result = g_object_new(BUZ_TYPE_MATERIALIZED_PAGE, NULL);
	buz_page_construct((BuzPage *) result);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->rows = a_array_new();
	BuzRow *row = buz_row_new();
	a_array_add((AArray *) context->rows, row);
	a_unref(row);
	return result;
}

BuzRowShady *buz_materialized_page_row_at(BuzMaterializedPageShady *page, int row) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	return (BuzRowShady *) a_array_at(context->rows, row);
}

BuzRow *buz_materialized_page_editable_row_at(BuzMaterializedPage *page, int row) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	BuzRowShady *row_shady = a_array_at(context->rows, row);
	if (a_alt_object_is_mutable(row_shady)) {
		return (BuzRow *) row_shady;
	}
	BuzRow *result = buz_row_mutable(row_shady);
	a_array_set_at((AArray *) context->rows, result, row);
	a_unref(result);
	return result;
}

void buz_materialized_page_set_rows(BuzMaterializedPage *page, AArray *row_list) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	a_array_remove_all((AArray *) context->rows);
	a_array_add_all((AArray *) context->rows, row_list);
}

static int l_row_count(BuzPageShady *page) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	return a_array_size(context->rows);
}


static void l_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	BUZ_PAGE_CLASS(buz_materialized_page_parent_class)->enrichmentRemap(page, old_map, new_map, action, index);

	AContext *context = (AContext *) a_alt_object_private(page)->context;
	if (context->parent.enrichment_data) {
		AIterator *rowsiter = a_array_iterator(context->rows);
		BuzRowShady *row = NULL;
		while(a_iterator_next(rowsiter, (gconstpointer *) (&row))) {
			buz_row_enrichment_remap(row, old_map, new_map, action, index);
		}
		a_unref(rowsiter);
	}
}

static void l_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map) {
	BUZ_PAGE_CLASS(buz_materialized_page_parent_class)->enrich(page, enrichment_map);

	AContext *context = (AContext *) a_alt_object_private(page)->context;
	if (context->parent.enriched_count==1) {
		AIterator *rowsiter = a_array_iterator(context->rows);
		BuzRowShady *row = NULL;
		while(a_iterator_next(rowsiter, (gconstpointer *) (&row))) {
			buz_row_enrich(row, enrichment_map, context->parent.lock);
		}
		a_unref(rowsiter);
	}
}

static void l_impoverish(BuzPageAnchored *page) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	if (context->parent.enriched_count==1) {
		AIterator *rowsiter = a_array_iterator(context->rows);
		BuzRowShady *row = NULL;
		while(a_iterator_next(rowsiter, (gconstpointer *) (&row))) {
			buz_row_impoverish(row);
		}
		a_unref(rowsiter);
	}

	BUZ_PAGE_CLASS(buz_materialized_page_parent_class)->impoverish(page);
}


A_ALT_C(BuzMaterializedPage, buz_materialized_page)

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	A_ALT_OBJECT_CLASS(buz_materialized_page_parent_class)->cloneContext(context_from, context_to);
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->rows = a_mutable_ref(from->rows);
}

//static void l_a_anchor_content(AAltObject *object) {
//	AAltObjectPrivate *priv = a_alt_object_private(object);
//	AContext *context = (AContext *) priv->context;
//}

static void l_a_anchor_content(AAltObject *object) {
	A_ALT_OBJECT_CLASS(buz_materialized_page_parent_class)->anchorContent(object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	a_log_debug("pre_anchor:%p, rows=%o",object, context->rows);
//	buz_page_debug((BuzPage *) object);
	context->rows = a_array_anchor(context->rows);
	a_log_debug("post_anchor:%p, rows=%o",object, context->rows);
//	buz_page_debug((BuzPage *) object);
}


static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

	if (!A_OBJECT_CLASS(buz_materialized_page_parent_class)->equal(object_a, object_b)) {
		return FALSE;
	}

	AContext *context_a = (AContext *) a_alt_object_private(object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(object_b)->context;
	if (context_a == context_b) {
		return TRUE;
	}

	return a_array_equal(context_a->rows, context_b->rows);
}

//static void l_to_string(const AObject *object, AString *out) {
//	const char *name = g_type_instance_name((GTypeInstance *) object);
//	a_string_format(out, "%s", name);
//}

















