/*
   File:    aiterator.c
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

#include "aiterator.h"

G_DEFINE_INTERFACE(AIterator, a_iterator, G_TYPE_OBJECT);

static void a_iterator_default_init (AIteratorInterface *iface) {
	iface->next = NULL;
}

gboolean a_iterator_next(AIterator *self, gconstpointer *value) {
	return A_ITERATOR_GET_INTERFACE(self)->next(self, value);
}
