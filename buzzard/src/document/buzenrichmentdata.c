/*
   File:    buzenrichmentdata.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 26, 2017
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

#include "buzenrichmentdata.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzEnrichmentData"
#include <asupport.h>

struct _BuzEnrichmentDataPrivate {
	ALock *lock;
	BuzEnrichmentDataMapAnchored *enrichment_map;
	AArray *data;
};

G_DEFINE_TYPE_WITH_CODE(BuzEnrichmentData, buz_enrichment_data, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzEnrichmentData)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_enrichment_data_class_init(BuzEnrichmentDataClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_enrichment_data_init(BuzEnrichmentData *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzEnrichmentData *instance = BUZ_ENRICHMENT_DATA(object);
//	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private(instance);
	G_OBJECT_CLASS(buz_enrichment_data_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_enrichment_data_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzEnrichmentData *buz_enrichment_data_new(BuzEnrichmentDataMapAnchored *enrichment_map) {
	BuzEnrichmentData *result = g_object_new(BUZ_TYPE_ENRICHMENT_DATA, NULL);
	a_object_construct((AObject *) result);
	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private(result);
	priv->enrichment_map = a_ref(enrichment_map);
	priv->data = NULL;
	priv->lock = a_lock_new();
	return result;
}

BuzEnrichmentData *buz_enrichment_data_new_lock(BuzEnrichmentDataMapAnchored *enrichment_map, ALock *lock) {
	BuzEnrichmentData *result = g_object_new(BUZ_TYPE_ENRICHMENT_DATA, NULL);
	a_object_construct((AObject *) result);
	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private(result);
	priv->enrichment_map = a_ref(enrichment_map);
	priv->data = NULL;
	priv->lock = a_ref(lock);
	return result;
}

static void l_ensure_data(BuzEnrichmentDataPrivate *priv) {
	if (priv->data==NULL) {
		AArrayAnchored *nullified_data = buz_enrichment_data_map_get_default_nullified_data(priv->enrichment_map);
		priv->data = a_array_mutable(nullified_data);
	}
}


ALock *buz_enrichment_data_get_lock(BuzEnrichmentData *enrichment_data) {
	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private(enrichment_data);
	return priv->lock;
}

void buz_enrichment_data_remap(BuzEnrichmentData *enrichment_data, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMap *new_map, int add_idx, int rem_idx) {
	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private(enrichment_data);
	// TODO old_map is not the current map... we need a manual synchronize here ... for now log a fatal msgs
	a_lock_lock(priv->lock);
	a_log_debug("enrichment_data=%p, new-map=%p, add_idx=%d", enrichment_data, new_map, add_idx);
	if (priv->enrichment_map != old_map) {
		a_log_fatal("need to implement this");
	}

	a_swap_ref(priv->enrichment_map, new_map);
	if (priv->data) {
		if (add_idx>=0) {
			a_array_add_at(priv->data, NULL, add_idx);
		}
		if (rem_idx>=0) {
			a_array_remove(priv->data, rem_idx, NULL);
		}
	}
	a_lock_unlock(priv->lock);
}

BuzEnrichmentDataMapAnchored *buz_enrichment_data_get_map(const BuzEnrichmentData *enrichment_data) {
	const BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private((BuzEnrichmentData *) enrichment_data);
	a_lock_lock(priv->lock);
	BuzEnrichmentDataMapAnchored *result = priv->enrichment_map;
	a_lock_unlock(priv->lock);
	return result;
}

GObject *buz_enrichment_data_get_slot_content_ref(const BuzEnrichmentData *enrichment_data, int slot_index, AStringAnchored *slot_key) {
	GObject *result = NULL;
	const BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private((BuzEnrichmentData *) enrichment_data);
	a_lock_lock(priv->lock);
	if (priv->data) {
		if (slot_key) {
			slot_index = buz_enrichment_data_map_find_slot_index(priv->enrichment_map, slot_key, slot_index);
		}
		if (slot_index>=0) {
			result = (GObject *) a_array_at(priv->data, slot_index);
			a_ref(result);
		}
	}
	a_lock_unlock(priv->lock);
	return result;
}

void buz_enrichment_data_set_slot_content(const BuzEnrichmentData *enrichment_data, int slot_index, AStringAnchored *slot_key, GObject *content) {
	BuzEnrichmentDataPrivate *priv = buz_enrichment_data_get_instance_private((BuzEnrichmentData *) enrichment_data);
	a_lock_lock(priv->lock);
	if (slot_key) {
		slot_index = buz_enrichment_data_map_find_slot_index(priv->enrichment_map, slot_key, slot_index);
	}
	a_log_debug("map=%p, slot_index=%d, slot_key=%o, content=%o", priv->enrichment_map, slot_index, slot_key, content);
	if (slot_index>=0) {
		l_ensure_data(priv);
		a_array_set_at(priv->data, content, slot_index);
	}
	a_lock_unlock(priv->lock);

}
