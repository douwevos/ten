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
#include "../document/buzdocumentlistener.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzView"
#include <asupport.h>

struct _BuzViewPrivate {
	BuzDocument *document;
	AStringAnchored *view_slot_key;
	BuzRevisionAnchored *revision;
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
	a_deref(priv->view_slot_key);
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
	AString *slot_key = a_string_new();
	a_string_format(slot_key, "View-%lx", (long long) view);
	priv->view_slot_key = a_string_anchor(slot_key);
	buz_document_add_listener(document, (BuzDocumentListener *) view);
}


BuzView *buz_view_new(BuzDocument *document) {
	BuzView *result = g_object_new(BUZ_TYPE_VIEW, NULL);
	buz_view_construct(result, document);
	return result;
}


static void l_document_new_revision(BuzDocumentListener *listener, BuzRevisionAnchored *revision) {
	BuzView *instance = BUZ_VIEW(listener);
	BuzViewPrivate *priv = buz_view_get_instance_private(instance);
	a_swap_ref(priv->revision , revision);

}

static void l_document_listener_iface_init(BuzDocumentListenerInterface *iface) {
	iface->newRevision = l_document_new_revision;
}
