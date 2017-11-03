/*
   File:    catatomicreference.c
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

#include "aatomicreference.h"
#include "../astring.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AAtomicReference"
#include "../asupport.h"

struct _AAtomicReferencePrivate {
	void volatile *value;
	volatile int lock;
};

G_DEFINE_TYPE_WITH_CODE(AAtomicReference, a_atomic_reference, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AAtomicReference)
);

static void l_as_string(const AObject *object, struct _AString *out);
static void l_dispose(GObject *object);

static void a_atomic_reference_class_init(AAtomicReferenceClass *clazz) {
	AObjectClass *a_class = A_OBJECT_CLASS(clazz);
	a_class->asString = l_as_string;

	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
}

static void a_atomic_reference_init(AAtomicReference *instance) {
	AAtomicReferencePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(instance, A_TYPE_ATOMIC_REFERENCE, AAtomicReferencePrivate);
	instance->priv = priv;
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAtomicReference *instance = A_ATOMIC_REFERENCE(object);
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(instance);
	a_deref(priv->value);
	G_OBJECT_CLASS(a_atomic_reference_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}


AAtomicReference *a_atomic_reference_new() {
	AAtomicReference *result = g_object_new(A_TYPE_ATOMIC_REFERENCE, NULL);
	a_object_construct((AObject *) result);
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(result);
	__atomic_store_n(&(priv->lock), 0, __ATOMIC_SEQ_CST);
	__atomic_store_n((void **) &(priv->value), NULL, __ATOMIC_SEQ_CST);
	return result;
}

AAtomicReference *a_atomic_reference_new_val(GObject *initial) {
	AAtomicReference *result = g_object_new(A_TYPE_ATOMIC_REFERENCE, NULL);
	a_object_construct((AObject *) result);
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(result);
	__atomic_store_n(&(priv->lock), 0, __ATOMIC_SEQ_CST);
	__atomic_store_n((void **) &(priv->value), initial, __ATOMIC_SEQ_CST);
	a_ref(initial);
	return result;
}

gboolean a_atomic_reference_compare_set(AAtomicReference *ref_atomic, GObject *expect, GObject *set) {
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(ref_atomic);
	// set or increase write lock
	a_log_trace("ref_atomic=%o, compare=%o, set=%o", ref_atomic, expect, set);
	while(TRUE) {
		int lock_val = __atomic_load_n(&(priv->lock), __ATOMIC_SEQ_CST);
		a_log_trace("lock_val=%d", lock_val);
		if (lock_val==0) {
			if (__atomic_compare_exchange_n(&(priv->lock), &lock_val, lock_val-1, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
				break;
			}
		}
	}
	gboolean result = __atomic_compare_exchange_n((void **) &(priv->value), &expect, set, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	if (result) {
		a_ref(set);
		a_deref(expect);
	}
	__atomic_add_fetch(&(priv->lock), 1, __ATOMIC_SEQ_CST);
	a_log_debug("result=%d", result);
	return result;
}

GObject *a_atomic_reference_get(AAtomicReference *ref_atomic) {
	a_log_trace("ref_atomic=%o", ref_atomic);
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(ref_atomic);
	// set or increase read lock
	while(TRUE) {
		int lock_val = __atomic_load_n(&(priv->lock), __ATOMIC_SEQ_CST);
		a_log_trace("lock_val=%d", lock_val);
		if (lock_val>=0) {
			if (__atomic_compare_exchange_n(&(priv->lock), &lock_val, lock_val+1, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
				break;
			}
		}
	}
	void *result = __atomic_load_n((void **) &(priv->value), __ATOMIC_SEQ_CST);
	a_ref(result);


	a_log_on_debug( int new_lock = )
	__atomic_sub_fetch(&(priv->lock), 1, __ATOMIC_SEQ_CST);

	a_log_debug("result=%o, new_lock=%d", result, new_lock);
	return (GObject *) result;
}

GObject *a_atomic_reference_get_unsafe(AAtomicReference *ref_atomic) {
	a_log_trace("ref_atomic=%o", ref_atomic);
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(ref_atomic);
	void *result = __atomic_load_n((void **) &(priv->value), __ATOMIC_SEQ_CST);
	a_log_debug("result=%o, new_lock=%d", result);
	return (GObject *) result;
}

gboolean a_atomic_reference_set(AAtomicReference *ref_atomic, GObject *newVal) {
	AAtomicReferencePrivate *priv = a_atomic_reference_get_instance_private(ref_atomic);
	gboolean ret = FALSE;
	// set or increase write lock
	a_log_trace("ref_atomic=%o, set=%o", ref_atomic, newVal);
	while(TRUE) {
		int lock_val = __atomic_load_n(&(priv->lock), __ATOMIC_SEQ_CST);
		a_log_trace("lock_val=%d", lock_val);
		if (lock_val==0) {
			if (__atomic_compare_exchange_n(&(priv->lock), &lock_val, lock_val-1, FALSE, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
				break;
			}
		}
	}
	GObject *result = __atomic_exchange_n((void **) &(priv->value), (newVal), __ATOMIC_SEQ_CST);
	a_log_debug("result=%p, value=%p", result, priv->value);
	if (result!=newVal) {
		a_ref(newVal);
		a_deref(result);
		ret = TRUE;
	}
	a_log_trace("unlocking");
	__atomic_add_fetch(&(priv->lock), 1, __ATOMIC_SEQ_CST);
	a_log_trace("unlocked");
	return ret;
}

static void l_as_string(const AObject *object, struct _AString *out) {
	a_string_format(out, "%s[%p]", g_type_name_from_instance((GTypeInstance *) object), a_atomic_reference_get_unsafe((AAtomicReference *) object));
}
