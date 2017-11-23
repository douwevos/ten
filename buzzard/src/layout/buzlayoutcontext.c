/*
   File:    buzlayoutcontext.c
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

#include "buzlayoutcontext.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzLayoutContext"
#include <asupport.h>

struct _BuzLayoutContextPrivate {
	PangoContext *pango_context;
	int font_height;
};

G_DEFINE_TYPE_WITH_CODE(BuzLayoutContext, buz_layout_context, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzLayoutContext)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_layout_context_class_init(BuzLayoutContextClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_layout_context_init(BuzLayoutContext *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzLayoutContext *instance = BUZ_LAYOUT_CONTEXT(object);
	BuzLayoutContextPrivate *priv = buz_layout_context_get_instance_private(instance);
	a_unref(priv->pango_context);
	G_OBJECT_CLASS(buz_layout_context_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_layout_context_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzLayoutContext *buz_layout_context_new(PangoContext *pango_context) {
	BuzLayoutContext *result = g_object_new(BUZ_TYPE_LAYOUT_CONTEXT, NULL);
	a_object_construct((AObject *) result);
	BuzLayoutContextPrivate *priv = buz_layout_context_get_instance_private(result);
	priv->pango_context = a_ref(pango_context);

	PangoLayout *layout = pango_layout_new(pango_context);
	pango_layout_set_text(layout, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 50);
	PangoRectangle inkt_rect;
	pango_layout_get_extents(layout, NULL, &inkt_rect);
	priv->font_height = ((inkt_rect.height+PANGO_SCALE-1)/PANGO_SCALE);

	return result;
}

PangoContext *buz_layout_context_get_pango_context(BuzLayoutContext *context) {
	BuzLayoutContextPrivate *priv = buz_layout_context_get_instance_private(context);
	return priv->pango_context;
}


int buz_layout_context_get_height(BuzLayoutContext *context) {
	BuzLayoutContextPrivate *priv = buz_layout_context_get_instance_private(context);
	return priv->font_height;
}
