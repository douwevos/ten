/*
   File:    buzrow.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 22, 2017
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

#include "buzrow.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzRow"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	AStringShady *text;
	BuzEnrichmentData *enrichment_data;
	int enriched_count;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzRow, buz_row, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void buz_row_class_init(BuzRowClass *clazz) {
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

static void buz_row_init(BuzRow *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		a_deref(context->text);
	}

	G_OBJECT_CLASS(buz_row_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzRow *buz_row_new() {
	BuzRow *result = g_object_new(BUZ_TYPE_ROW, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->text = a_string_new();
	context->enrichment_data = NULL;
	return result;
}

AStringShady *buz_row_text(BuzRowShady *row) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	return context->text;
}

AString *buz_row_editable_text(BuzRow *row) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	if (!a_alt_object_is_mutable(context->text)) {
		AString *text = a_string_mutable(context->text);
		a_swap_ref(context->text, text);
		a_unref(text);
	}
	return (AString *) context->text;
}

void buz_row_enrichment_remap(BuzRowShady *row, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	if (context->enrichment_data) {
		buz_enrichment_data_remap(context->enrichment_data, old_map, new_map, action, index);
	}
}

void buz_row_enrich(BuzRowAnchored *row, BuzEnrichmentDataMapAnchored *enrichment_map, ALock *lock) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	a_log_debug("enriching row:%o, context->enriched_count=%d", row, context->enriched_count);
	if (context->enriched_count==0) {
		if (context->enrichment_data==NULL) {
			context->enrichment_data = buz_enrichment_data_new_lock(enrichment_map, lock);
		}
	}
	context->enriched_count++;
}

void buz_row_impoverish(BuzRowAnchored *row) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	if (context->enriched_count==1) {
		a_deref(context->enrichment_data);
	}
	context->enriched_count--;
}

GObject *buz_row_get_slot_content_ref(BuzRowAnchored *row, BuzEnrichmentSlot *slot) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	a_log_debug("get-slot_content:%O, slot=%p", row, slot);
	return buz_enrichment_data_get_slot_content_ref(context->enrichment_data, slot);
}

void buz_row_set_slot_content(BuzRowAnchored *row, BuzEnrichmentSlot *slot, GObject *content) {
	AContext *context = (AContext *) a_alt_object_private(row)->context;
	buz_enrichment_data_set_slot_content(context->enrichment_data, slot, content);
}

BuzRowAnchored *buz_row_anchor(BuzRowShady *row) {
	return (BuzRowAnchored *) a_alt_object_anchor(row);
}

BuzRow *buz_row_mutable(BuzRowShady *row) {
	return (BuzRow *) a_alt_object_mutable(row);
}


static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->text = a_mutable_ref(from->text);
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	a_log_debug("pre_anchor:text=%o",context->text);
	context->text = a_string_anchor(context->text);
	a_log_debug("post_anchor:text=%o",context->text);
}

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

	AContext *context_a = (AContext *) a_alt_object_private(object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(object_b)->context;
	if (context_a == context_b) {
		return TRUE;
	}
	return a_string_equal(context_a->text, context_b->text);
}

