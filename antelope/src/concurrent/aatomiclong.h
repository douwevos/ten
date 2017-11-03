/*
   File:    catatomiclong.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 15, 2017
   Web:     http://www.natpad.net/
   e-mail:  dmvos2000(at)yahoo.com

   Copyright (C) 2013 Douwe Vos.

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

#ifndef AATOMICLONG_H_
#define AATOMICLONG_H_

#include "../aobject.h"

G_BEGIN_DECLS

#define A_TYPE_ATOMIC_LONG              (a_atomic_long_get_type())
#define A_ATOMIC_LONG(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_atomic_long_get_type(), AAtomicLong))
#define A_ATOMIC_LONG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ATOMIC_LONG, AAtomicLongClass))
#define A_IS_ATOMIC_LONG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ATOMIC_LONG))
#define A_IS_ATOMIC_LONG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ATOMIC_LONG))
#define A_ATOMIC_LONG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ATOMIC_LONG, AAtomicLongClass))
#define A_ATOMIC_LONG_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), A_TYPE_ATOMIC_LONG, AAtomicLongPrivate))


typedef struct _AAtomicLong               AAtomicLong;
typedef struct _AAtomicLongPrivate        AAtomicLongPrivate;
typedef struct _AAtomicLongClass          AAtomicLongClass;


struct _AAtomicLong {
	AObject parent;
};

struct _AAtomicLongClass {
	AObjectClass parent_class;
};


GType a_atomic_long_get_type();


AAtomicLong *a_atomic_long_new();

AAtomicLong *a_atomic_long_new_val(long long initial);

gboolean a_atomic_long_compare_set(AAtomicLong *long_atomic, long long expect, long long set);

long long a_atomic_long_get(AAtomicLong *long_atomic);

void a_atomic_long_set(AAtomicLong *long_atomic, long long newVal);

long long a_atomic_long_add(AAtomicLong *long_atomic, long long delta);

long long a_atomic_long_decrement(AAtomicLong *long_atomic);

long long a_atomic_long_increment(AAtomicLong *long_atomic);

G_END_DECLS

#endif /* AATOMICLONG_H_ */
