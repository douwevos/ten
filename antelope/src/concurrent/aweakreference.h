/*
   File:    aweakreference.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 15, 2017
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

#ifndef AWEAKREFERENCE_H_
#define AWEAKREFERENCE_H_

#include "../aobject.h"

G_BEGIN_DECLS

#define A_TYPE_WEAK_REFERENCE              (a_weak_reference_get_type())
#define A_WEAK_REFERENCE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_weak_reference_get_type(), AWeakReference))
#define A_WEAK_REFERENCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_WEAK_REFERENCE, AWeakReferenceClass))
#define A_IS_WEAK_REFERENCE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_WEAK_REFERENCE))
#define A_IS_WEAK_REFERENCE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_WEAK_REFERENCE))
#define A_WEAK_REFERENCE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_WEAK_REFERENCE, AWeakReferenceClass))


typedef struct _AWeakReference               AWeakReference;
typedef struct _AWeakReferencePrivate        AWeakReferencePrivate;
typedef struct _AWeakReferenceClass          AWeakReferenceClass;


struct _AWeakReference {
	AObject parent;
};

struct _AWeakReferenceClass {
	AObjectClass parent_class;
};


GType a_weak_reference_get_type();

/**
 * Creates a new weak-reference to the given value.
 */
AWeakReference *a_weak_reference_new(GObject *value);

/**
 * Returns a strong reference to the internally stored value or NULL if the value was already unreferenced.
 */
GObject *a_weak_reference_get(AWeakReference *reference);


void a_weak_reference_set(AWeakReference *reference, GObject *new_value);

G_END_DECLS

#endif /* AWEAKREFERENCE_H_ */
