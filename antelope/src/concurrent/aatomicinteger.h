/*
   File:    aatomicinteger.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 15, 2017
   Web:     http://www.natpad.net/
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

#ifndef AATOMICINTEGER_H_
#define AATOMICINTEGER_H_

#include "../aobject.h"

G_BEGIN_DECLS

#define A_TYPE_ATOMIC_INTEGER              (a_atomic_integer_get_type())
#define A_ATOMIC_INTEGER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_atomic_integer_get_type(), AAtomicInteger))
#define A_ATOMIC_INTEGER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ATOMIC_INTEGER, AAtomicIntegerClass))
#define A_IS_ATOMIC_INTEGER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ATOMIC_INTEGER))
#define A_IS_ATOMIC_INTEGER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ATOMIC_INTEGER))
#define A_ATOMIC_INTEGER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ATOMIC_INTEGER, AAtomicIntegerClass))
#define A_ATOMIC_INTEGER_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), A_TYPE_ATOMIC_INTEGER, AAtomicIntegerPrivate))


typedef struct _AAtomicInteger               AAtomicInteger;
typedef struct _AAtomicIntegerPrivate        AAtomicIntegerPrivate;
typedef struct _AAtomicIntegerClass          AAtomicIntegerClass;


struct _AAtomicInteger {
	AObject parent;
};

struct _AAtomicIntegerClass {
	AObjectClass parent_class;
};

GType a_atomic_integer_get_type();

AAtomicInteger *a_atomic_integer_new();

AAtomicInteger *a_atomic_integer_new_val(int initial);

gboolean a_atomic_integer_compare_set(AAtomicInteger *int_atomic, int expect, int set);

int a_atomic_integer_get(AAtomicInteger *int_atomic);

void a_atomic_integer_set(AAtomicInteger *int_atomic, int newVal);

int a_atomic_integer_add(AAtomicInteger *int_atomic, int delta);

int a_atomic_integer_decrement(AAtomicInteger *int_atomic);

int a_atomic_integer_increment(AAtomicInteger *int_atomic);

G_END_DECLS

#endif /* AATOMICINTEGER_H_ */
