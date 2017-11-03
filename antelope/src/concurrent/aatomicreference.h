/*
   File:    catatomicreference.h
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

#ifndef AATOMICREFERENCE_H_
#define AATOMICREFERENCE_H_

#include "../aobject.h"

G_BEGIN_DECLS

#define A_TYPE_ATOMIC_REFERENCE              (a_atomic_reference_get_type())
#define A_ATOMIC_REFERENCE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_atomic_reference_get_type(), AAtomicReference))
#define A_ATOMIC_REFERENCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ATOMIC_REFERENCE, AAtomicReferenceClass))
#define A_IS_ATOMIC_REFERENCE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ATOMIC_REFERENCE))
#define A_IS_ATOMIC_REFERENCE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ATOMIC_REFERENCE))
#define A_ATOMIC_REFERENCE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ATOMIC_REFERENCE, AAtomicReferenceClass))

typedef struct _AAtomicReference               AAtomicReference;
typedef struct _AAtomicReferencePrivate        AAtomicReferencePrivate;
typedef struct _AAtomicReferenceClass          AAtomicReferenceClass;

struct _AAtomicReference {
	AObject parent;
	AAtomicReferencePrivate *priv;
};

struct _AAtomicReferenceClass {
	AObjectClass parent_class;
};


GType a_atomic_reference_get_type();


AAtomicReference *a_atomic_reference_new();

AAtomicReference *a_atomic_reference_new_val(GObject *initial);

gboolean a_atomic_reference_compare_set(AAtomicReference *atomic_ref, GObject *expect, GObject *set);

/*
 * a_atomic_reference_get:
 * @atomic_ref: a #AAtomicReference
 *
 * Returns a reference to the current set #GObject. Please note that when the function returns the content of the
 * atomic reference might already have been changed. This is also the reason why the reference count of the returned
 * object is increased and needs to be dereferenced by the caller.
 *
 * Return value: A reference to the object stored in this atomic-reference.
 */
GObject *a_atomic_reference_get(AAtomicReference *atomic_ref);

/**
 * a_atomic_reference_get_unsafe:
 * @atomic_ref: a #AAtomicReference
 *
 * Returns the current set #GObject. Please note that the function returns the content of the atomic reference without
 * obeying any locks. This is useful when the caller is sure that this function is called from the same thread as where
 * the value is set and that the value is only set by this thread only (in other words there is a single writer thread
 * and multiple reader threads).
 *
 * Return value: The object stored in this atomic-reference.
 */
GObject *a_atomic_reference_get_unsafe(AAtomicReference *atomic_ref);


gboolean a_atomic_reference_set(AAtomicReference *atomic_ref, GObject *newVal);

G_END_DECLS

#endif /* AATOMICREFERENCE_H_ */
