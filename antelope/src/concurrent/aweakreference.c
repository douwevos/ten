/*
   File:    aweakreference.c
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


#include "aweakreference.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AWeakReference"
#include "../asupport.h"

struct _AWeakReferencePrivate {
	GWeakRef ref;
};

G_DEFINE_TYPE_WITH_CODE(AWeakReference, a_weak_reference, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AWeakReference)
);

static void l_dispose(GObject *object);

static void a_weak_reference_class_init(AWeakReferenceClass *clazz) {

	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
}

static void a_weak_reference_init(AWeakReference *instance) {
}


static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AWeakReference *instance = A_WEAK_REFERENCE(object);
	AWeakReferencePrivate *priv = a_weak_reference_get_instance_private(instance);
	g_weak_ref_clear(&priv->ref);
	G_OBJECT_CLASS(a_weak_reference_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}


AWeakReference *a_weak_reference_new(GObject *value) {
	AWeakReference *result = g_object_new(A_TYPE_WEAK_REFERENCE, NULL);
	a_object_construct((AObject *) result);
	AWeakReferencePrivate *priv = a_weak_reference_get_instance_private(result);
	g_weak_ref_init(&priv->ref, value);
	return result;
}

GObject *a_weak_reference_get(AWeakReference *reference) {
	AWeakReferencePrivate *priv = a_weak_reference_get_instance_private(reference);
	GObject *result = g_weak_ref_get(&priv->ref);
	return a_ref_fake(result);
}

void a_weak_reference_set(AWeakReference *reference, GObject *new_value) {
	AWeakReferencePrivate *priv = a_weak_reference_get_instance_private(reference);
	g_weak_ref_set(&priv->ref, (gpointer) new_value);
}
