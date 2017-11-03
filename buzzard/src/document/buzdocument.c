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

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzDocument"
#include <asupport.h>

struct _BuzDocumentPrivate {
	AAtomicReference *revision_reference;
	AWeakList *listeners;
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
	a_unref(revision);
}

BuzDocument *buz_document_new() {
	BuzDocument *result = g_object_new(BUZ_TYPE_DOCUMENT, NULL);
	buz_document_construct(result);
	return result;
}


BuzRevisionAnchored *buz_document_get_revision_ref(BuzDocument *document) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	return (BuzRevisionAnchored *) a_atomic_reference_get(priv->revision_reference);
}


void buz_document_post_revision(BuzDocument *document, BuzRevisionShady *revision) {
	BuzDocumentPrivate *priv = buz_document_get_instance_private(document);
	BuzRevisionAnchored *anchored = buz_revision_anchor(revision);
	a_log_debug("anchored=%p", anchored);
	a_atomic_reference_set(priv->revision_reference, (GObject *) anchored);

	AIterator *iter = a_weak_list_iterator(priv->listeners);
	BuzDocumentListener *listener = NULL;
	while(a_iterator_next(iter, (gconstpointer *) &listener)) {
		a_log_debug("listener:%O", listener);
		buz_document_listener_new_revision(listener, anchored);
	}
	a_unref(iter);
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
