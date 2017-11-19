/*
   File:    buzeditorview.h
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

#ifndef VIEW_BUZEDITORVIEW_H_
#define VIEW_BUZEDITORVIEW_H_

#include "../document/buzdocument.h"
#include <antelope.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BUZ_TYPE_EDITOR_VIEW              (buz_editor_view_get_type())
#define BUZ_EDITOR_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_editor_view_get_type(), BuzEditorView))
#define BUZ_EDITOR_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_EDITOR_VIEW, BuzEditorViewClass))
#define BUZ_IS_EDITOR_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_EDITOR_VIEW))
#define BUZ_IS_EDITOR_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_EDITOR_VIEW))
#define BUZ_EDITOR_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_EDITOR_VIEW, BuzEditorViewClass))


typedef struct _BuzEditorView               BuzEditorView;
typedef struct _BuzEditorViewPrivate        BuzEditorViewPrivate;
typedef struct _BuzEditorViewClass          BuzEditorViewClass;


struct _BuzEditorView {
	AObject parent;
};

struct _BuzEditorViewClass {
	AObjectClass parent_class;
};


GType buz_editor_view_get_type();

BuzEditorView *buz_editor_view_new(BuzDocument *document, GtkWidget *widget);

gboolean buz_editor_view_key_pressed(BuzEditorView *editor_view, GdkEventKey *event);

void buz_editor_view_set_view_size(BuzEditorView *editor_view, int width, int height);

void buz_editor_view_draw(BuzEditorView *editor_view, cairo_t *cr);



G_END_DECLS

#endif /* VIEW_BUZEDITORVIEW_H_ */
