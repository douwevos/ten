/*
   File:    aweaklist.h
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

#ifndef AWEAKLIST_H_
#define AWEAKLIST_H_

#include "../collections/aarray.h"
#include "../collections/aiterator.h"
#include "../aobject.h"

G_BEGIN_DECLS

#define A_TYPE_WEAK_LIST              (a_weak_list_get_type())
#define A_WEAK_LIST(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_weak_list_get_type(), AWeakList))
#define A_WEAK_LIST_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_WEAK_LIST, AWeakListClass))
#define A_IS_WEAK_LIST(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_WEAK_LIST))
#define A_IS_WEAK_LIST_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_WEAK_LIST))
#define A_WEAK_LIST_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_WEAK_LIST, AWeakListClass))


typedef struct _AWeakList               AWeakList;
typedef struct _AWeakListPrivate        AWeakListPrivate;
typedef struct _AWeakListClass          AWeakListClass;


struct _AWeakList {
	AObject parent;
};

struct _AWeakListClass {
	AObjectClass parent_class;
};

GType a_weak_list_get_type();

/**
 * Creates a new empty AWeakList instance. A AWeakList allows the user to create a reference list of
 * weak-referenced objects.
 */
AWeakList *a_weak_list_new();

/**
 * Returns the number of weak-references in the list. The number is the raw number of elements in the internally used
 * map. This internally used map might contain already released references and this method does not traverse through
 * the list to check this.
 */
int a_weak_list_size(AWeakList *weak_list);

/**
 * Appends the given ref to this AWeakList. If the ref is NULL then nothing is added.
 */
void a_weak_list_append(AWeakList *weak_list, GObject *ref);

/**
 * Appends the given ref to this AWeakList but only if it was not already present in the list. If the ref is NULL
 * then nothing is added. The functions returns true if the entry was added and false otherwise.
 */
gboolean a_weak_list_append_once(AWeakList *weak_list, GObject *ref);


/**
 * Removes the first weak reference to the given ref searching the list backwards. The method returns TRUE if the ref
 * was present in the list and was removed, otherwise it will return FALSE.
 */
gboolean a_weak_list_remove(AWeakList *weak_list, GObject *ref);

/**
 * Enlist the references in the given array enlist_to. If enlist_to is NULL a new array will be created. The enlisted
 * will hold a strong reference through the CatArray.
 */
AArray *a_weak_list_enlist(AWeakList *weak_list, AArray *enlist_to);

/**
 * Creates an iterator of the references in the AWeakList. Once the iterator is created any changes to the list will
 * not interfere with the iterator.
 */
AIterator *a_weak_list_iterator(AWeakList *weak_list);


G_END_DECLS

#endif /* AWEAKLIST_H_ */
