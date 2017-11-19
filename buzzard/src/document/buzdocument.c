/*
   File:    buzdocument.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 19, 2017
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

#include "buzdocument.h"
#include "buzenrichmentdatamap.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzDocument"
#include <asupport.h>

struct _BuzDocumentPrivate {
	AAtomicReference *revision_reference;
	AWeakList *listeners;
	BuzEnrichmentDataMapAnchored *enrichment_map;
	gboolean large_mode;
};

G_DEFINE_TYPE_WITH_CODE(BuzDocument, buz_document, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzDocument)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_document_class_init(BuzDocumentClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_document_init(BuzDocument *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzDocument *instance = BUZ_DOCUMENT(object);
	BuzDocumentPrivate *priv = buz_document_get_instance_private(instance);
	a_deref(priv->listeners);
	a_deref(priv->revision_reference);
	a_deref(priv->enrichment_map);
	G_OBJECT_CLASS(buz_document_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_document_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


void buz_document_construct(BuzDocument *document) {
	a_monitor_set_active_for(document);
	a_object_construct(A_OBJECT(document));
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	priv->listeners = a_weak_list_new();
	BuzRevision *initial_revision = buz_revision_new();
	BuzRevisionAnchored *revision = buz_revision_anchor(initial_revision);
	priv->revision_reference = a_atomic_reference_new_val((GObject *) revision);
	a_log_debug("constructing");
	priv->enrichment_map = buz_enrichment_data_map_new();
	a_unref(revision);
}

BuzDocument *buz_document_new() {
	BuzDocument *result = g_object_new(BUZ_TYPE_DOCUMENT, NULL);
	buz_document_construct(result);
	return result;
}

void buz_document_set_large_mode(BuzDocument *document, gboolean large_mode) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	priv->large_mode = large_mode;
}

BuzRevisionAnchored *buz_document_get_revision_ref(BuzDocument *document) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	return (BuzRevisionAnchored *) a_atomic_reference_get(priv->revision_reference);
}


void buz_document_post_revision(BuzDocument *document, BuzRevisionShady *revision) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	BuzRevisionAnchored *anchored = buz_revision_anchor(revision);
	a_log_debug("anchored=%p", anchored);
	BuzRevisionAnchored *last_revision = (BuzRevisionAnchored *) a_atomic_reference_get_unsafe(priv->revision_reference);
	if (last_revision!=anchored) {
		buz_revision_enrich(anchored, priv->enrichment_map);
		buz_revision_impoverish(last_revision);
		a_atomic_reference_set(priv->revision_reference, (GObject *) anchored);

		AIterator *iter = a_weak_list_iterator(priv->listeners);
		BuzDocumentListener *listener = NULL;
		while(a_iterator_next(iter, (gconstpointer *) &listener)) {
			a_log_debug("listener:%O", listener);
			buz_document_listener_new_revision(listener, anchored);
		}
		a_unref(iter);
	}
	a_unref(anchored);
}


void buz_document_add_listener(BuzDocument *document, BuzDocumentListener *listener) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	a_weak_list_append_once(priv->listeners, (GObject *) listener);
}

void buz_document_remove_listener(BuzDocument *document, BuzDocumentListener *listener) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	a_weak_list_remove(priv->listeners, (GObject *) listener);
}

void buz_document_post_enrichment_slot_notify(BuzDocument *document, BuzRevision *a_revision, AStringAnchored *slot_key, gpointer user_data) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	/* notify listeners */
	AIterator *iter = a_weak_list_iterator(priv->listeners);
	BuzDocumentListener *listener = NULL;
	while(a_iterator_next(iter, (gconstpointer *) &listener)) {
		BuzDocumentListenerInterface *iface = BUZ_DOCUMENT_LISTENER_GET_INTERFACE(listener);
		if (iface->onEnrichmentSlotNotify) {
			iface->onEnrichmentSlotNotify(listener, a_revision, slot_key, user_data);
		}
	}
	a_deref(iter);
}

int buz_document_register_enrichment_slot(BuzDocument *document, AStringAnchored *slot_key) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	BuzEnrichmentDataMap *e_map = buz_enrichment_data_map_mutable(priv->enrichment_map);
	int index = buz_enrichment_data_map_add_slot(e_map, slot_key);
	if (index>=0) {
		a_log_debug("e_map=%O", e_map);

		BuzEnrichmentDataMapAnchored *map_anchored = buz_enrichment_data_map_anchor(e_map);
//		a_log_debug("slot added: rev=%p, slot_key=%o, index=%d, old-map=%p, map=%o", a_atomic_reference_get_unsafe(priv->a_revision_ref), slot_key, index, priv->enrichment_map, e_map);
		BuzRevisionAnchored *rev = (BuzRevisionAnchored *) a_atomic_reference_get_unsafe(priv->revision_reference);
		buz_revision_enrichment_remap(rev, priv->enrichment_map, map_anchored, BUZ_ENRICHMENT_ADD, index);
		a_swap_ref(priv->enrichment_map, map_anchored);

		/* notify listeners */
		AIterator *iter = a_weak_list_iterator(priv->listeners);
		BuzDocumentListener *listener = NULL;
		while(a_iterator_next(iter, (gconstpointer *) &listener)) {
			BuzDocumentListenerInterface *iface = BUZ_DOCUMENT_LISTENER_GET_INTERFACE(listener);
			if (iface->onSlotRegistered) {
				iface->onSlotRegistered(listener, rev, map_anchored, slot_key, index);
			}
		}
		a_deref(map_anchored);
		a_deref(iter);
	} else {
		a_deref(e_map);
	}
	return index;
}

void buz_document_unregister_enrichment_slot(BuzDocument *document, AStringAnchored *slot_key) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	BuzEnrichmentDataMap *e_map = buz_enrichment_data_map_mutable(priv->enrichment_map);
	int index = buz_enrichment_data_map_remove_slot(e_map, slot_key);
	if (index>=0) {
		e_map = buz_enrichment_data_map_anchor(e_map);
		BuzRevision *a_rev = (BuzRevision *) a_atomic_reference_get_unsafe(priv->revision_reference);
		buz_revision_enrichment_remap(a_rev, priv->enrichment_map, e_map, -1, index);
		a_swap_ref(priv->enrichment_map, e_map);
	}
	a_deref(e_map);
}
