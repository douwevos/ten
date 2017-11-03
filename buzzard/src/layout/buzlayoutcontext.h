/*
   File:    buzlayoutcontext.h
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

#ifndef LAYOUT_BUZLAYOUTCONTEXT_H_
#define LAYOUT_BUZLAYOUTCONTEXT_H_

#include <antelope.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BUZ_TYPE_LAYOUT_CONTEXT              (buz_layout_context_get_type())
#define BUZ_LAYOUT_CONTEXT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_layout_context_get_type(), BuzLayoutContext))
#define BUZ_LAYOUT_CONTEXT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_LAYOUT_CONTEXT, BuzLayoutContextClass))
#define BUZ_IS_LAYOUT_CONTEXT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_LAYOUT_CONTEXT))
#define BUZ_IS_LAYOUT_CONTEXT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_LAYOUT_CONTEXT))
#define BUZ_LAYOUT_CONTEXT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_LAYOUT_CONTEXT, BuzLayoutContextClass))


typedef struct _BuzLayoutContext               BuzLayoutContext;
typedef struct _BuzLayoutContextPrivate        BuzLayoutContextPrivate;
typedef struct _BuzLayoutContextClass          BuzLayoutContextClass;


struct _BuzLayoutContext {
	AObject parent;
};

struct _BuzLayoutContextClass {
	AObjectClass parent_class;
};


GType buz_layout_context_get_type();

BuzLayoutContext *buz_layout_context_new(PangoContext *context);

PangoContext *buz_layout_context_get_pango_context(BuzLayoutContext *context);


G_END_DECLS

#endif /* LAYOUT_BUZLAYOUTCONTEXT_H_ */
