/*
   File:    buzsurface.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 28, 2017
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

#include "buzsurface.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzSurface"
#include <asupport.h>

struct _BuzSurfacePrivate {
	cairo_surface_t *surface;
	short version;
};

G_DEFINE_TYPE_WITH_CODE(BuzSurface, buz_surface, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzSurface)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_surface_class_init(BuzSurfaceClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_surface_init(BuzSurface *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzSurface *instance = BUZ_SURFACE(object);
	BuzSurfacePrivate *priv = buz_surface_get_instance_private(instance);
	if (priv->surface) {
		cairo_surface_destroy(priv->surface);
		priv->surface = NULL;
	}
	G_OBJECT_CLASS(buz_surface_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_surface_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzSurface *buz_surface_new(cairo_surface_t *surface, short version) {
	BuzSurface *result = g_object_new(BUZ_TYPE_SURFACE, NULL);
	BuzSurfacePrivate *priv = buz_surface_get_instance_private(result);
	a_object_construct((AObject *) result);
	priv->surface = surface;
	priv->version = version;
	return result;
}

short buz_surface_get_version(BuzSurface *surface) {
	BuzSurfacePrivate *priv = buz_surface_get_instance_private(surface);
	return priv->version;
}

cairo_surface_t *buz_surface_get_cairo_surface(BuzSurface *surface) {
	if (surface==NULL) {
		return NULL;
	}
	BuzSurfacePrivate *priv = buz_surface_get_instance_private(surface);
	return priv->surface;
}

