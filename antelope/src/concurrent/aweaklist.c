/*
   File:    aweaklist.c
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


#include "aweaklist.h"

#include "alock.h"
#include "aweakreference.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AWeakList"
#include "../asupport.h"

struct _AWeakListPrivate {
	ALock *lock;
	AArray *weak_list;
};

G_DEFINE_TYPE_WITH_CODE(AWeakList, a_weak_list, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AWeakList)
);

static void l_dispose(GObject *object);

static void a_weak_list_class_init(AWeakListClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
}

static void a_weak_list_init(AWeakList *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AWeakList *instance = A_WEAK_LIST(object);
	AWeakListPrivate *priv = a_weak_list_get_instance_private(instance);
	a_deref(priv->weak_list);
	a_deref(priv->lock);
	G_OBJECT_CLASS(a_weak_list_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

AWeakList *a_weak_list_new() {
	AWeakList *result = g_object_new(A_TYPE_WEAK_LIST, NULL);
	a_object_construct((AObject *) result);
	AWeakListPrivate *priv = a_weak_list_get_instance_private(result);
	priv->lock = a_lock_new();
	priv->weak_list = a_array_new();
	return result;
}

int a_weak_list_size(AWeakList *weak_list) {
	int result = 0;
	AWeakListPrivate *priv = a_weak_list_get_instance_private(weak_list);
	a_lock_lock(priv->lock);
	result = a_array_size(priv->weak_list);
	a_lock_unlock(priv->lock);
	return result;
}

void a_weak_list_append(AWeakList *weak_list, GObject *ref) {
	if (ref==NULL) {
		return;
	}
	AWeakListPrivate *priv = a_weak_list_get_instance_private(weak_list);
	a_lock_lock(priv->lock);
	AWeakReference *wref = a_weak_reference_new(ref);
	a_array_add(priv->weak_list, (GObject *) wref);
	a_deref(wref);
	a_lock_unlock(priv->lock);
}

gboolean a_weak_list_append_once(AWeakList *weak_list, GObject *ref) {
	if (ref==NULL) {
		return FALSE;
	}
	AWeakListPrivate *priv = a_weak_list_get_instance_private(weak_list);
	gboolean result = TRUE;
	a_lock_lock(priv->lock);

	int idx;
	for(idx=a_array_size(priv->weak_list)-1; idx>=0; idx--) {
		AWeakReference *weak_ref = (AWeakReference *) a_array_at(priv->weak_list, idx);
		GObject *strong_ref = a_weak_reference_get(weak_ref);
		if (strong_ref) {
			a_unref(strong_ref);
			if (strong_ref==ref) {
				result = FALSE;
				break;
			}
		} else {
			a_array_remove(priv->weak_list, idx, NULL);
		}
	}

	if (result) {
		AWeakReference *wref = a_weak_reference_new(ref);
		a_array_add(priv->weak_list, (GObject *) wref);
		a_deref(wref);
	}
	a_lock_unlock(priv->lock);
	return result;
}


gboolean a_weak_list_remove(AWeakList *weak_list, GObject *ref) {
	gboolean result = FALSE;
	AWeakListPrivate *priv = a_weak_list_get_instance_private(weak_list);
	a_lock_lock(priv->lock);
	int idx;
	for(idx=a_array_size(priv->weak_list)-1; idx>=0; idx--) {
		AWeakReference *weak_ref = (AWeakReference *) a_array_at(priv->weak_list, idx);
		GObject *strong_ref = a_weak_reference_get(weak_ref);
		if (strong_ref) {
			a_unref(strong_ref);
			if (strong_ref==ref) {
				a_array_remove(priv->weak_list, idx, NULL);
				result = TRUE;
				break;
			}
		} else {
			a_array_remove(priv->weak_list, idx, NULL);
		}
	}
	a_lock_unlock(priv->lock);
	return result;
}


AArray *a_weak_list_enlist(AWeakList *weak_list, AArray *enlist_to) {
	AWeakListPrivate *priv = a_weak_list_get_instance_private(weak_list);
	if (enlist_to==NULL) {
		enlist_to = a_array_new();
	}
	a_lock_lock(priv->lock);
	int idx;
	for(idx=0; idx<a_array_size(priv->weak_list); idx++) {
		AWeakReference *weak_ref = (AWeakReference *) a_array_at(priv->weak_list, idx);
		GObject *strong_ref = a_weak_reference_get(weak_ref);
		if (strong_ref) {
			a_array_add(enlist_to, strong_ref);
			a_unref(strong_ref);
		} else {
			a_array_remove(priv->weak_list, idx, NULL);
			idx--;
		}
	}
	a_lock_unlock(priv->lock);
	return enlist_to;
}


AIterator *a_weak_list_iterator(AWeakList *weak_list) {
	AArray *strong_list = a_weak_list_enlist(weak_list, NULL);
	AIterator *result = a_array_iterator(strong_list);
	a_deref(strong_list);
	return result;
}
