/*
   File:    buzsurfacepool.c
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

#include "buzsurfacepool.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzSurfacePool"
#include <asupport.h>

struct _BuzSurfacePoolPrivate {
	AArray *pool;
	int width;
	int height;
	GdkWindow *window;
	short version;
};

G_DEFINE_TYPE_WITH_CODE(BuzSurfacePool, buz_surface_pool, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzSurfacePool)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_surface_pool_class_init(BuzSurfacePoolClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_surface_pool_init(BuzSurfacePool *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzSurfacePool *instance = BUZ_SURFACE_POOL(object);
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(instance);
	a_deref(priv->pool);
	a_deref(priv->window);
	G_OBJECT_CLASS(buz_surface_pool_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_surface_pool_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzSurfacePool *buz_surface_pool_new() {
	BuzSurfacePool *result = g_object_new(BUZ_TYPE_SURFACE_POOL, NULL);
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(result);
	a_object_construct((AObject *) result);
	priv->pool = a_array_new();
	priv->width = 1;
	priv->height = 1;
	priv->window = NULL;
	priv->version = 0;
	return result;
}


void buz_surface_pool_set_width(BuzSurfacePool *pool, int width) {
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(pool);
	if (priv->width==width) {
		return;
	}
	priv->width = width;
	priv->version++;
	a_array_remove_all(priv->pool);
}

void buz_surface_pool_set_height(BuzSurfacePool *pool, int height) {
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(pool);
	if (priv->height==height) {
		return;
	}
	priv->height = height;
	priv->version++;
	a_array_remove_all(priv->pool);
}

void buz_surface_pool_set_window(BuzSurfacePool *pool, GdkWindow *window) {
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(pool);
	if (priv->window==window) {
		return;
	}
	a_swap_ref(priv->window, window);
	priv->version++;
	a_array_remove_all(priv->pool);
}


BuzSurface *buz_surface_pool_obtain(BuzSurfacePool *pool) {
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(pool);
	BuzSurface *result = NULL;
	if (a_array_size(priv->pool)>0) {
		a_array_remove_last(priv->pool, (gconstpointer *) &result);
	} else {
		cairo_surface_t *cairo_surface = NULL;
		if (priv->window==NULL) {
			cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, priv->width, priv->height);
		} else {
			cairo_surface = gdk_window_create_similar_surface(priv->window, CAIRO_CONTENT_COLOR, priv->width, priv->height);
		}
		result = buz_surface_new(cairo_surface, priv->version);
		a_log_debug("new surface created");
	}
	return result;
}


void buz_surface_pool_release(BuzSurfacePool *pool, BuzSurface *surface) {
	BuzSurfacePoolPrivate *priv = buz_surface_pool_get_instance_private(pool);
	if (buz_surface_get_version(surface)==priv->version) {
		a_array_add(priv->pool, surface);
	}
}
