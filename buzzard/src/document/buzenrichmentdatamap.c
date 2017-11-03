/*
   File:    buzenrichmentdatamap.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 25, 2017
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

#include "buzenrichmentdatamap.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzEnrichmentDataMap"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	AArrayShady *keys;
	AArrayShady *nullified_data;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzEnrichmentDataMap, buz_enrichment_data_map, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
//static void l_to_string(const AObject *object, AString *out);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void buz_enrichment_data_map_class_init(BuzEnrichmentDataMapClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;
//	o_class->toString = l_to_string;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->clone_context_for_mutable = TRUE;
	a_class->anchorContent = l_a_anchor_content;
	a_class->cloneContext = l_a_clone;
}

static void buz_enrichment_data_map_init(BuzEnrichmentDataMap *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AContext *context = (AContext *) a_alt_object_private(object)->context;
	if (context) {
		a_unref(context->keys);
		a_unref(context->nullified_data);
	}
	G_OBJECT_CLASS(buz_enrichment_data_map_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzEnrichmentDataMap *buz_enrichment_data_map_new() {
	BuzEnrichmentDataMap *result = g_object_new(BUZ_TYPE_ENRICHMENT_DATA_MAP, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->keys = a_array_new();
	context->nullified_data = a_array_new();
	return result;
}

AArrayAnchored *buz_enrichment_data_map_get_default_nullified_data(BuzEnrichmentDataMapAnchored *enrichment_map) {
	AContext *context = (AContext *) a_alt_object_private(enrichment_map)->context;
	if (a_alt_object_is_mutable(context->nullified_data)) {
		a_log_error("still mutable");
		return NULL;
	}
	return (AArrayAnchored *) context->nullified_data;

}

int buz_enrichment_data_map_count(BuzEnrichmentDataMapShady *enrichment_map) {
	AContext *context = (AContext *) a_alt_object_private(enrichment_map)->context;
	return a_array_size(context->keys);
}

int buz_enrichment_data_map_find_slot_index(BuzEnrichmentDataMapShady *enrichment_map, AStringAnchored *slot_key, int guess_index) {
	AContext *context = (AContext *) a_alt_object_private(enrichment_map)->context;
	return a_array_find_index(context->keys, slot_key, guess_index);

}

int buz_enrichment_data_map_add_slot(BuzEnrichmentDataMap *enrichment_map, AStringAnchored *slot_key) {
	AContext *context = (AContext *) a_alt_object_private(enrichment_map)->context;
	int result = a_array_find_index(context->keys, slot_key, -1);
	if (result<0) {
		result = a_array_size(context->keys);
		a_array_add((AArray *) context->keys, slot_key);
		a_array_add((AArray *) context->nullified_data, NULL);
	}
	return result;
}

int buz_enrichment_data_map_remove_slot(BuzEnrichmentDataMap *enrichment_map, AStringAnchored *slot_key) {
	AContext *context = (AContext *) a_alt_object_private(enrichment_map)->context;
	int result = a_array_find_index(context->keys, slot_key, -1);
	if (result>=0) {
		a_array_remove((AArray *) context->keys, result, NULL);
		a_array_remove_first((AArray *) context->nullified_data, NULL);
	}
	return result;
}

A_ALT_C(BuzEnrichmentDataMap, buz_enrichment_data_map)

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->keys = a_mutable_ref(from->keys);
	to->nullified_data = a_mutable_ref(from->nullified_data);
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	context->keys = a_array_anchor(context->keys);
	context->nullified_data = a_array_anchor(context->nullified_data);
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
	return a_array_equal(context_a->keys, context_b->keys);
}

//static void l_to_string(const AObject *object, AString *out) {
//	const char *name = g_type_instance_name((GTypeInstance *) object);
//	a_string_format(out, "%s", name);
//}


