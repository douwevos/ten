/*
   File:    aatomiclong.c
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

#include "aatomiclong.h"
#include "../astring.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AAtomicLong"
#include "../asupport.h"

struct _AAtomicLongPrivate {
	long long value;
};

G_DEFINE_TYPE_WITH_CODE(AAtomicLong, a_atomic_long, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AAtomicLong)
);

static void l_as_string(const AObject *object, struct _AString *out);

static void a_atomic_long_class_init(AAtomicLongClass *clazz) {
	A_OBJECT_CLASS(clazz)->asString = l_as_string;
}

static void a_atomic_long_init(AAtomicLong *instance) {
}

AAtomicLong *a_atomic_long_new() {
	AAtomicLong *result = g_object_new(A_TYPE_ATOMIC_LONG, NULL);
	a_object_construct((AObject *) result);
	AAtomicLongPrivate *priv = a_atomic_long_get_instance_private(result);
	__atomic_store_n(&(priv->value), 0, __ATOMIC_SEQ_CST);
	return result;
}


AAtomicLong *a_atomic_long_new_val(long long initial) {
	AAtomicLong *result = g_object_new(A_TYPE_ATOMIC_LONG, NULL);
	a_object_construct((AObject *) result);
	AAtomicLongPrivate *priv = a_atomic_long_get_instance_private(result);
	__atomic_store_n(&(priv->value), initial, __ATOMIC_SEQ_CST);
	return result;
}

gboolean a_atomic_long_compare_set(AAtomicLong *long_atomic, long long expect, long long set) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	return __atomic_compare_exchange_n(&(priv->value), &expect, set, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

long long a_atomic_long_get(AAtomicLong *long_atomic) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	return __atomic_load_n(&(priv->value), __ATOMIC_SEQ_CST);
}

void a_atomic_long_set(AAtomicLong *long_atomic, long long newVal) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	__atomic_store_n(&(priv->value), newVal, __ATOMIC_SEQ_CST);
}

long long a_atomic_long_add(AAtomicLong *long_atomic, long long delta) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	return __atomic_add_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

long long a_atomic_long_decrement(AAtomicLong *long_atomic) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	const long long delta = 1;
	return __atomic_sub_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

long long a_atomic_long_increment(AAtomicLong *long_atomic) {
	AAtomicLongPrivate *priv = A_ATOMIC_LONG_GET_PRIVATE(long_atomic);
	const long long delta = 1;
	return __atomic_add_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

static void l_as_string(const AObject *object, struct _AString *out) {
	a_string_format(out, "%s[%ld]", g_type_name_from_instance((GTypeInstance *) object), a_atomic_long_get((AAtomicLong *) object));
}
