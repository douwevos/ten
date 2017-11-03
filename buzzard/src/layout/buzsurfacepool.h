/*
   File:    buzsurfacepool.h
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

#ifndef LAYOUT_BUZSURFACEPOOL_H_
#define LAYOUT_BUZSURFACEPOOL_H_

#include "buzsurface.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_SURFACE_POOL              (buz_surface_pool_get_type())
#define BUZ_SURFACE_POOL(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_surface_pool_get_type(), BuzSurfacePool))
#define BUZ_SURFACE_POOL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_SURFACE_POOL, BuzSurfacePoolClass))
#define BUZ_IS_SURFACE_POOL(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_SURFACE_POOL))
#define BUZ_IS_SURFACE_POOL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_SURFACE_POOL))
#define BUZ_SURFACE_POOL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_SURFACE_POOL, BuzSurfacePoolClass))


typedef struct _BuzSurfacePool               BuzSurfacePool;
typedef struct _BuzSurfacePoolPrivate        BuzSurfacePoolPrivate;
typedef struct _BuzSurfacePoolClass          BuzSurfacePoolClass;


struct _BuzSurfacePool {
	AObject parent;
};

struct _BuzSurfacePoolClass {
	AObjectClass parent_class;
};


GType buz_surface_pool_get_type();

BuzSurfacePool *buz_surface_pool_new();

void buz_surface_pool_set_width(BuzSurfacePool *pool, int width);
void buz_surface_pool_set_height(BuzSurfacePool *pool, int height);
void buz_surface_pool_set_window(BuzSurfacePool *pool, GdkWindow *window);


BuzSurface *buz_surface_pool_obtain(BuzSurfacePool *pool);
void buz_surface_pool_release(BuzSurfacePool *pool, BuzSurface *surface);

G_END_DECLS

#endif /* LAYOUT_BUZSURFACEPOOL_H_ */
