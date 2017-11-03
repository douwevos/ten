/*
   File:    aiterator.h
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

#ifndef SRC_AITERATOR_H_
#define SRC_AITERATOR_H_

#include <glib-object.h>

#define A_TYPE_ITERATOR                 (a_iterator_get_type ())
#define A_ITERATOR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), A_TYPE_ITERATOR, AIterator))
#define A_IS_ITERATOR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ITERATOR))
#define A_ITERATOR_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), A_TYPE_ITERATOR, AIteratorInterface))


typedef struct _AIterator               AIterator; /* dummy object */
typedef struct _AIteratorInterface      AIteratorInterface;

struct _AIteratorInterface {
  GTypeInterface parent_iface;
  gboolean (*next) (AIterator *self, gconstpointer *next_element);
};

GType a_iterator_get_type(void);

gboolean a_iterator_next(AIterator *self, gconstpointer *next_element);

#endif /* SRC_AITERATOR_H_ */
