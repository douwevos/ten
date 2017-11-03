/*
   File:    aatomicinteger.c
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

#include "aatomicinteger.h"
#include "../astring.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AAtomicInteger"
#include "../asupport.h"

struct _AAtomicIntegerPrivate {
	int value;
};

G_DEFINE_TYPE_WITH_CODE(AAtomicInteger, a_atomic_integer, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AAtomicInteger)
);

static void l_as_string(const AObject *object, struct _AString *out);

static void a_atomic_integer_class_init(AAtomicIntegerClass *clazz) {
	A_OBJECT_CLASS(clazz)->asString = l_as_string;
}

static void a_atomic_integer_init(AAtomicInteger *instance) {
}


AAtomicInteger *a_atomic_integer_new() {
	AAtomicInteger *result = g_object_new(A_TYPE_ATOMIC_INTEGER, NULL);
	a_object_construct((AObject *) result);
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(result);
	__atomic_store_n(&(priv->value), 0, __ATOMIC_SEQ_CST);
	return result;
}


AAtomicInteger *a_atomic_integer_new_val(int initial) {
	AAtomicInteger *result = g_object_new(A_TYPE_ATOMIC_INTEGER, NULL);
	a_object_construct((AObject *) result);
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(result);
	__atomic_store_n(&(priv->value), initial, __ATOMIC_SEQ_CST);
	return result;
}



gboolean a_atomic_integer_compare_set(AAtomicInteger *int_atomic, int expect, int set) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	return __atomic_compare_exchange_n(&(priv->value), &expect, set, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

int a_atomic_integer_get(AAtomicInteger *int_atomic) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	return __atomic_load_n(&(priv->value), __ATOMIC_SEQ_CST);
}

void a_atomic_integer_set(AAtomicInteger *int_atomic, int newVal) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	__atomic_store_n(&(priv->value), newVal, __ATOMIC_SEQ_CST);
}

int a_atomic_integer_add(AAtomicInteger *int_atomic, int delta) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	return __atomic_add_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

int a_atomic_integer_decrement(AAtomicInteger *int_atomic) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	const int delta = 1;
	return __atomic_sub_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

int a_atomic_integer_increment(AAtomicInteger *int_atomic) {
	AAtomicIntegerPrivate *priv = a_atomic_integer_get_instance_private(int_atomic);
	const int delta = 1;
	return __atomic_add_fetch(&(priv->value), delta, __ATOMIC_SEQ_CST);
}

static void l_as_string(const AObject *object, struct _AString *out) {
	a_string_format(out, "%s[%d]", g_type_name_from_instance((GTypeInstance *) object), a_atomic_integer_get((AAtomicInteger *) object));
}
