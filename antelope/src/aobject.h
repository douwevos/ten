/*
   File:    aobject.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 10, 2017
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

#ifndef SRC_AOBJECT_H_
#define SRC_AOBJECT_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define A_TYPE_OBJECT              (a_object_get_type())
#define A_OBJECT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_object_get_type(), AObject))
#define A_OBJECT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_OBJECT, AObjectClass))
#define A_IS_OBJECT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_OBJECT))
#define A_IS_OBJECT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_OBJECT))
#define A_OBJECT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_OBJECT, AObjectClass))


typedef struct _AObject               AObject;
typedef struct _AObjectPrivate        AObjectPrivate;
typedef struct _AObjectClass          AObjectClass;

struct _AString;

struct _AObject {
	GObject parent;
};

struct _AObjectClass {
	GObjectClass parent_class;
	void (*asString)(const AObject *object, struct _AString *out);
	gboolean (*equal)(const AObject *object_a, const AObject *object_b);
};

GType a_object_get_type();

void a_object_construct(AObject *object);

gboolean a_object_equal(const AObject *object_a, const AObject *object_b);

G_END_DECLS

#endif /* SRC_AOBJECT_H_ */
