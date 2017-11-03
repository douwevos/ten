/*
   File:    buzsurface.h
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

#ifndef LAYOUT_BUZSURFACE_H_
#define LAYOUT_BUZSURFACE_H_

#include <gtk/gtk.h>
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_SURFACE              (buz_surface_get_type())
#define BUZ_SURFACE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_surface_get_type(), BuzSurface))
#define BUZ_SURFACE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_SURFACE, BuzSurfaceClass))
#define BUZ_IS_SURFACE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_SURFACE))
#define BUZ_IS_SURFACE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_SURFACE))
#define BUZ_SURFACE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_SURFACE, BuzSurfaceClass))


typedef struct _BuzSurface               BuzSurface;
typedef struct _BuzSurfacePrivate        BuzSurfacePrivate;
typedef struct _BuzSurfaceClass          BuzSurfaceClass;


struct _BuzSurface {
	AObject parent;
};

struct _BuzSurfaceClass {
	AObjectClass parent_class;
};


GType buz_surface_get_type();

BuzSurface *buz_surface_new(cairo_surface_t *surface, short version);

short buz_surface_get_version(BuzSurface *surface);

cairo_surface_t *buz_surface_get_cairo_surface(BuzSurface *surface);

G_END_DECLS

#endif /* LAYOUT_BUZSURFACE_H_ */
