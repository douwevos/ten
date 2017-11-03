/*
   File:    amapiterator.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 13, 2017
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

#ifndef SRC_AMAPITERATOR_H_
#define SRC_AMAPITERATOR_H_

#include <glib-object.h>

#define A_TYPE_MAP_ITERATOR                 (a_map_iterator_get_type ())
#define A_MAP_ITERATOR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), A_TYPE_MAP_ITERATOR, AMapIterator))
#define A_IS_MAP_ITERATOR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_MAP_ITERATOR))
#define A_MAP_ITERATOR_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), A_TYPE_MAP_ITERATOR, AMapIteratorInterface))


typedef struct _AMapIterator               AMapIterator; /* dummy object */
typedef struct _AMapIteratorInterface      AMapIteratorInterface;

struct _AMapIteratorInterface {
  GTypeInterface parent_iface;
  gboolean (*next) (AMapIterator *self, gconstpointer *key, gconstpointer *value);
};

GType a_map_iterator_get_type(void);

gboolean a_map_iterator_next(AMapIterator *self, gconstpointer *key, gconstpointer *value);

#endif /* SRC_AITERATOR_H_ */
