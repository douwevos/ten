/*
   File:    aobject.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 8, 2017
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

#ifndef SRC_AALTOBJECT_H_
#define SRC_AALTOBJECT_H_

#include "aobject.h"

G_BEGIN_DECLS

#define A_ALT_HEADERS(tp, pre) \
tp *pre##_mutable(tp##Shady *instance); \
tp##Anchored *pre##_anchor(tp##Shady *instance); \
gboolean pre##_equal(tp##Shady *instance_a, tp##Shady *instance_b); \


#define A_ALT_C(tp, pre) \
tp *pre##_mutable(tp##Shady *instance) { \
	return (tp *) a_alt_object_mutable(instance); \
} \
tp##Anchored *pre##_anchor(tp##Shady *instance) { \
	return (tp##Anchored *) a_alt_object_anchor(instance); \
} \
gboolean pre##_equal(tp##Shady *instance_a, tp##Shady *instance_b) { \
	if (instance_a==instance_b) { \
		return TRUE; \
	} \
	if ((instance_a==NULL) || (instance_b==NULL)) { \
		return FALSE; \
	} \
	return A_OBJECT_GET_CLASS(instance_a)->equal((const AObject *) instance_a, (const AObject *) instance_b); \
} \

#define A_ALT_C_MONITOR(tp, pre) \
tp *pre##_mutable(tp##Shady *instance) { \
	tp *result = (tp *) a_alt_object_mutable(instance); \
	if (result!=instance) { \
		a_monitor_set_active_for(result); \
	} \
	return result; \
} \
tp##Anchored *pre##_anchor(tp##Shady *instance) { \
	tp##Anchored *result = (tp##Anchored *) a_alt_object_anchor(instance); \
	if (result!=instance) { \
		a_monitor_set_active_for(result); \
	} \
	return result; \
} \
gboolean pre##_equal(tp##Shady *instance_a, tp##Shady *instance_b) { \
	if (instance_a==instance_b) { \
		return TRUE; \
	} \
	if ((instance_a==NULL) || (instance_b==NULL)) { \
		return FALSE; \
	} \
	return A_OBJECT_GET_CLASS(instance_a)->equal(instance_a, instance_b); \
} \


#define A_TYPE_ALT_OBJECT              (a_alt_object_get_type())
#define A_ALT_OBJECT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_alt_object_get_type(), AAltObject))
#define A_ALT_OBJECT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ALT_OBJECT, AAltObjectClass))
#define A_IS_ALT_OBJECT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ALT_OBJECT))
#define A_IS_ALT_OBJECT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ALT_OBJECT))
#define A_ALT_OBJECT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ALT_OBJECT, AAltObjectClass))


typedef struct _AAltObject               AAltObject;
typedef struct _AAltObjectPrivate        AAltObjectPrivate;
typedef struct _AAltObjectClass          AAltObjectClass;
typedef struct _AAltObjectContext        AAltObjectContext;
typedef struct _AAltObjectEditor         AAltObjectEditor;


struct _AAltObject {
	AObject parent;
};


struct _AAltObjectContext {
	volatile guint ref_count;
};


struct _AAltObjectEditor {
	const AAltObject *origin;
};


struct _AAltObjectClass {
	AObjectClass parent_class;
	int context_size;
	int editor_size;
	gboolean clone_context_for_mutable;
	void (*cloneContext)(const AAltObjectContext *context_from, AAltObjectContext *context_to);
	void (*anchorContent)(AAltObject *object);
};

GType a_alt_object_get_type();

void a_alt_object_construct(AAltObject *object, gboolean mutable);

gboolean a_alt_object_is_mutable(gconstpointer object);

gconstpointer a_alt_object_anchor(gconstpointer object);

/* Either creates an editable version of the given object or increases the number of editors.
 */
void *a_alt_object_mutable(gconstpointer object);

const AAltObjectContext *a_alt_object_editor_get_original_context(const AAltObjectEditor *origin);

gpointer a_mutable_ref(gconstpointer object);

G_END_DECLS

#endif /* SRC_AALTOBJECT_H_ */
