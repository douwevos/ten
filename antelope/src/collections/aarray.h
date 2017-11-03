/*
   File:    aarray.h
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

#ifndef SRC_AARRAY_H_
#define SRC_AARRAY_H_

#include "../aaltobject.h"
#include "aiterator.h"

#define A_TYPE_ARRAY              (a_array_get_type())
#define A_ARRAY(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_array_get_type(), AArray))
#define A_ARRAY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ARRAY, AArrayClass))
#define A_IS_ARRAY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ARRAY))
#define A_IS_ARRAY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ARRAY))
#define A_ARRAY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ARRAY, AArrayClass))

typedef const struct _AArray    AArrayAnchored;
typedef const struct _AArray    AArrayShady;
typedef struct _AArray          AArray;
typedef struct _AArrayClass     AArrayClass;

struct _AArray {
	AAltObject parent;
};

struct _AArrayClass {
	AAltObjectClass parent_class;
};

GType a_array_get_type();

AArray *a_array_new();

void a_array_add(AArray *array, gconstpointer element);
void a_array_add_at(AArray *array, gconstpointer element, int index);
void a_array_add_all(AArray *array,  AArrayShady *list_to_add);
gboolean a_array_set_at(AArray *array, gconstpointer element, int index);

gboolean a_array_remove(AArray *array, int index, gconstpointer *removed_ref);
gboolean a_array_remove_first(AArray *array, gconstpointer *removed_ref);
gboolean a_array_remove_last(AArray *array, gconstpointer *removed_ref);
void a_array_remove_range(AArray *array, int offset, int length);
void a_array_remove_all(AArray *array);

int a_array_size(AArrayShady *array);
gconstpointer a_array_at(AArrayShady *array, int index);
gconstpointer a_array_first(AArrayShady *array);
gconstpointer a_array_last(AArrayShady *array);

int a_array_find_index(AArrayShady *array, gconstpointer search_item, int hint_index);
int a_array_find_index_ext(AArrayShady *array, gconstpointer search_item, int hint_index, GEqualFunc eq_func);

void a_array_foreach(AArrayShady *array, GFunc iter_func, gpointer user_data);
AIterator *a_array_iterator(AArrayShady *array);

A_ALT_HEADERS(AArray, a_array);

#endif /* SRC_AARRAY_H_ */
