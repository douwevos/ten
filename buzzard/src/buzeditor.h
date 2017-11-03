/*
   File:    buzeditor.h
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

#ifndef BUZEDITOR_H_
#define BUZEDITOR_H_

#include "document/buzdocument.h"
#include <gtk/gtk.h>
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_EDITOR              (buz_editor_get_type())
#define BUZ_EDITOR(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_editor_get_type(), BuzEditor))
#define BUZ_EDITOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_EDITOR, BuzEditorClass))
#define BUZ_IS_EDITOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_EDITOR))
#define BUZ_IS_EDITOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_EDITOR))
#define BUZ_EDITOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_EDITOR, BuzEditorClass))


typedef struct _BuzEditor               BuzEditor;
typedef struct _BuzEditorPrivate        BuzEditorPrivate;
typedef struct _BuzEditorClass          BuzEditorClass;


struct _BuzEditor {
	GtkWidget parent;
};

struct _BuzEditorClass {
	GtkWidgetClass parent_class;
};


GType buz_editor_get_type();

BuzEditor *buz_editor_new(BuzDocument *document);

G_END_DECLS

#endif /* BUZEDITOR_H_ */
