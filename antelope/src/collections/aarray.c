/*
   File:    aarray.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 8, 2017
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

#include "aarray.h"
#include "../aaltobjectprivate.h"
#include <string.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AArray"
#include "../asupport.h"

#define A_TYPE_ARRAY_ITER            (a_array_iter_get_type())

typedef struct _AArrayIter	      AArrayIter;
typedef struct _AArrayIterClass	  AArrayIterClass;

struct _AArrayIter {
	GObject parent;
	AArray *array;
	gconstpointer *data;
	int size;
	int offset;
};

GType a_array_iter_get_type(void);

struct _AContext {
	AAltObjectContext parent;
	gconstpointer *data;
	int size;
	int data_size;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(AArray, a_array, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_anchor_content(AAltObject *object);

static void a_array_class_init(AArrayClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_anchor_content;
}

static void a_array_init(AArray *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObject *instance = A_ALT_OBJECT(object);
	AAltObjectPrivate *priv = a_alt_object_private(instance);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		int idx = context->size-1;
		context->size = 0;
		while(idx>=0) {
			a_deref(context->data[idx]);
			idx--;
		}
	}
	G_OBJECT_CLASS(a_array_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	AAltObject *instance = A_ALT_OBJECT(object);
	AAltObjectPrivate *priv = a_alt_object_private(instance);
	AContext *context = (AContext *) priv->context;
	if (context) {
		a_free_ptr(context->data);
	}

	G_OBJECT_CLASS(a_array_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


AArray *a_array_new() {
	AArray *result = g_object_new(A_TYPE_ARRAY, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	return result;
}

static AContext *l_ensure_size(AArray *array, int minimal_size, gboolean copy) {
//	cat_log_debug("minimal_size=%d", minimal_size);
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (a_alt_object_editor_get_original_context(priv->editor) == priv->context) {

		AAltObjectClass *a_cls = A_ALT_OBJECT_GET_CLASS(array);
		AContext *context_copy = (AContext *) g_malloc0(a_cls->context_size);
		context_copy->parent.ref_count = 1;
		context_copy->size = context->size;
		context_copy->data_size = context->size;

		minimal_size += 16;
		if (minimal_size>context_copy->data_size) {
			context_copy->data_size = minimal_size;
		}
		context_copy->data = g_malloc(context_copy->data_size*sizeof(GObject*));
		if (copy) {
			int idx;
			for(idx=context->size-1; idx>=0; idx--) {
				context_copy->data[idx] = a_ref(context->data[idx]);
			}
		}
		priv->context = (AAltObjectContext *) context_copy;
		return context_copy;
	}

	if (context->data_size>=minimal_size) {
		return NULL;
	}

	minimal_size += 16;
	if (context->data_size==0) {
		context->data_size = minimal_size;
		context->data = g_malloc(context->data_size*sizeof(GObject*));
	} else {
		context->data_size += minimal_size;
		context->data = g_realloc(context->data, context->data_size*sizeof(GObject*));
	}
	return NULL;
}

void a_array_add(AArray *array, gconstpointer element) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	l_ensure_size(array, context->size+1, TRUE);
	context = (AContext *) priv->context;
	context->data[context->size] = a_ref(element);
	context->size++;
}

void a_array_add_at(AArray *array, gconstpointer element, int index) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (index<0) {
		index = 0;
	}
	if (index>=context->size) {
		a_array_add(array, element);
		return;
	}
	AContext *context_copy = l_ensure_size(array, context->size+1, FALSE);
	if (context_copy) {
		int in_index = 0;
		int out_index = 0;
		while(in_index<context->size) {
			if (in_index==index) {
				out_index++;
			}
			context_copy->data[out_index] = a_ref(context->data[in_index]);
			in_index++;
			out_index++;
		}
		context = context_copy;
	} else {
		memmove(context->data+index+1, context->data+index, (context->size-index)*sizeof(void *));
	}
	context->data[index] = a_ref(element);
	context->size++;
}

void a_array_add_all(AArray *array,  AArrayShady *list_to_add) {
	if (list_to_add==NULL) {
		return;
	}
	AAltObjectPrivate *l2a_priv = a_alt_object_private(list_to_add);
	AContext *l2a_context = (AContext *) l2a_priv->context;
	if (l2a_context->size==0) {
		return;
	}
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	l_ensure_size(array, context->size+l2a_context->size, TRUE);
	gconstpointer *out = context->data+context->size;
	gconstpointer *in = l2a_context->data;
	int idx;
	for(idx=l2a_context->size; idx>0; idx--) {
		*out = a_ref(*in);
		out++;
		in++;
	}
	context->size += l2a_context->size;
}

gboolean a_array_set_at(AArray *array, gconstpointer element, int index) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if ((index<0) || (index>=context->size)) {
		return FALSE;
	}
	AContext *context_copy = l_ensure_size(array, context->size, TRUE);
	if (context_copy) {
		context = context_copy;
	}
	a_swap_ref(context->data[index], element);
//	a_swap_ref_intern(context->data+index, element, "AArray", 226, (!(0)));
	return TRUE;
}

gboolean a_array_remove(AArray *array, int index, gconstpointer *removed_ref) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (removed_ref) {
		*removed_ref = NULL;
	}
	if (index<0 || index>=context->size) {
		return FALSE;
	}


	AContext *context_copy = l_ensure_size(array, context->size, FALSE);
	if (context_copy) {
		int in_index = 0;
		int out_index = 0;
		while(in_index<context->size) {
			if (in_index==index) {
				if (removed_ref) {
					*removed_ref = context->data[in_index];
				} else {
					a_unref(context->data[in_index]);
				}
				continue;
			} else {
				context_copy->data[out_index] = a_ref(context->data[in_index]);
				out_index++;
			}
			in_index++;
		}
		context = context_copy;
	} else {
		if (removed_ref) {
			*removed_ref = context->data[index];
		} else {
			a_unref(context->data[index]);
		}
		memmove(context->data+index, context->data+index+1, (context->size-(index+1))*sizeof(void *));
	}
	context->size--;
	return TRUE;
}

gboolean a_array_remove_first(AArray *array, gconstpointer *removed_ref) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (context->size>0) {
		return a_array_remove(array, 0, removed_ref);
	}
	if (removed_ref) {
		*removed_ref = NULL;
	}
	return FALSE;
}

gboolean a_array_remove_last(AArray *array, gconstpointer *removed_ref) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (context->size>0) {
		return a_array_remove(array, context->size-1, removed_ref);
	}
	if (removed_ref) {
		*removed_ref = NULL;
	}
	return FALSE;
}


void a_array_remove_range(AArray *array, int offset, int length) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;

	if ((length<=0) || (offset+length<=0) || offset>=context->size) {
		return;
	}

	if (offset<0) {
		length+=offset;
		offset = 0;
	}

	if (offset+length>context->size) {
		length = context->size-offset;
		if (length<=0) {
			return;
		}
	}

	int new_size = context->size-length;
	AContext *context_copy = l_ensure_size(array, new_size, FALSE);
	if (context_copy) {

		context_copy->size = new_size;
		int out_idx=0;
		int in_idx;
		for(in_idx=0; in_idx<offset; in_idx++) {
			context_copy->data[out_idx] = a_ref(context->data[in_idx]);
			out_idx++;
		}
		int old_size = context->size;
		for(in_idx=offset+length; in_idx<old_size; in_idx++) {
			context_copy->data[out_idx] = a_ref(context->data[in_idx]);
			out_idx++;
		}

	} else {
		int end_off = offset+length;
		int in_idx;
		for(in_idx=offset; in_idx<end_off; in_idx++) {
			a_unref(context->data[in_idx]);
		}
		memmove(context->data+offset, context->data+end_off, (context->size-(end_off))*sizeof(void *));
	}
	context->size = new_size;
}

void a_array_remove_all(AArray *array) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (context->size==0) {
		return;
	}
	AContext *context_copy = l_ensure_size(array, 0, FALSE);
	if (context_copy) {
		context = context_copy;
	}
	int in_idx;
	for(in_idx=context->size-1; in_idx>=0; in_idx--) {
		a_deref(context->data[in_idx]);
	}
	context->size = 0;
}


int a_array_size(AArrayShady *array) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	return context->size;
}

gconstpointer a_array_at(AArrayShady *array, int index) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (index>=0 && index<context->size) {
		return context->data[index];
	}
	return NULL;
}

gconstpointer a_array_first(AArrayShady *array) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (context->size>0) {
		return context->data[0];
	}
	return NULL;
}

gconstpointer a_array_last(AArrayShady *array) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (context->size>0) {
		return context->data[context->size-1];
	}
	return NULL;
}

int a_array_find_index(AArrayShady *array, gconstpointer search_item, int hint_index) {
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (hint_index>=0 && hint_index<context->size) {
		if (context->data[hint_index]==search_item) {
			return hint_index;
		}
	}
	int idx=0;
	for(idx=0; idx<context->size; idx++) {
		if (context->data[idx]==search_item) {
			return idx;
		}
	}
	return -1;
}

int a_array_find_index_ext(AArrayShady *array, gconstpointer search_item, int hint_index, GEqualFunc equal_func) {
	g_return_val_if_fail(equal_func!=NULL, -1);
	AAltObjectPrivate *priv = a_alt_object_private(array);
	AContext *context = (AContext *) priv->context;
	if (hint_index>=0 && hint_index<context->size) {
		if (equal_func(context->data[hint_index], search_item)) {
			return hint_index;
		}
	}
	int idx=0;
	for(idx=0; idx<context->size; idx++) {
		if (equal_func(context->data[idx], search_item)) {
			return idx;
		}
	}
	return -1;
}

void a_array_foreach(AArrayShady *array, GFunc iter_func, gpointer user_data) {
	AAltObjectPrivate *priv = a_alt_object_private((AArray *) array);
	AContext *context = (AContext *) priv->context;
	int idx = context->size;
	gconstpointer *ptr = context->data;
	while(idx>0) {
		iter_func((gpointer) *ptr, user_data);
		ptr++;
		idx--;
	}
}

AIterator *a_array_iterator(AArrayShady *array) {
	AAltObjectPrivate *priv = a_alt_object_private((AArray *) array);
	AContext *context = (AContext *) priv->context;
	AArrayIter *result = g_object_new(A_TYPE_ARRAY_ITER, NULL);
	result->array = a_ref(array);
	result->data = context->data;
	result->offset = 0;
	result->size = context->size;
	return (AIterator *) result;
}

A_ALT_C(AArray, a_array)

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a == NULL) || (object_b == NULL)) {
		return FALSE;
	}

	AAltObjectPrivate *priv_a = a_alt_object_private(object_a);
	AContext *context_a = (AContext *) priv_a->context;
	AAltObjectPrivate *priv_b = a_alt_object_private(object_b);
	AContext *context_b = (AContext *) priv_b->context;
	if (context_a == context_b) {
		return TRUE;
	}
	if (context_a->size != context_b->size) {
		return FALSE;
	}

	/* we assume the memcmp function to be extremely fast */
	if (memcmp(context_a->data, context_b->data, sizeof(void *) * context_a->size)==0) {
		return TRUE;
	}
	int idx=0;
	for(idx=context_a->size-1; idx>=0; idx--) {
		gconstpointer *data_a = (gconstpointer *) context_a->data[idx];
		gconstpointer *data_b = (gconstpointer *) context_b->data[idx];
		if (data_a==data_b) {
			continue;
		}
		if (A_IS_OBJECT(data_a) && A_IS_OBJECT(data_b)) {
			if (a_object_equal((AObject *) data_a, (AObject *) data_b)) {
				continue;
			}
		}
		return FALSE;
	}
	return TRUE;
}

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->data_size = from->size;
	to->size = from->size;
	to->data = g_malloc(sizeof(GObject *) * from->size);
	int idx = from->size-1;
	for(;idx>=0; idx--) {
		gconstpointer obj = from->data[idx];
		to->data[idx] = a_ref(obj);
	}
}

static void l_anchor_content(AAltObject *object) {
	a_log_debug("anchoring object:%o", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	int idx = context->size;
	gconstpointer *ptr = context->data;
	while(idx>0) {
		gconstpointer item = *ptr;
		if (A_IS_ALT_OBJECT(item)) {
			a_log_debug("anchoring item:%p", item);
			*ptr = (gpointer) a_alt_object_anchor(item);
			a_log_debug("anchored item:%p into %p", item, *ptr);
		}
		ptr++;
		idx--;
	}
}




#define A_ARRAY_ITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_array_iter_get_type(), AArrayIter))
#define A_ARRAY_ITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_ARRAY_ITER, AArrayIterClass))
#define A_IS_ARRAY_ITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_ARRAY_ITER))
#define A_IS_ARRAY_ITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_ARRAY_ITER))
#define A_ARRAY_ITER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_ARRAY_ITER, AArrayIterClass))


struct _AArrayIterClass {
	GObjectClass parent_class;
};


static gboolean l_iter_next(AIterator *self, gconstpointer *next_element) {
	AArrayIter *this = (AArrayIter *) self;
//	a_log_trace("_iter_next:this->offset=%d", this->offset);
	if (this->offset>=this->size) {
		return FALSE;
	}
	*next_element = this->data[this->offset++];
	return TRUE;
}

static void l_iterator_interface_init(AIteratorInterface *iface) {
	iface->next = l_iter_next;
}


G_DEFINE_TYPE_WITH_CODE(AArrayIter, a_array_iter, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(A_TYPE_ITERATOR, l_iterator_interface_init)
);

static void l_dispose_iter(GObject *object);


static void a_array_iter_class_init(AArrayIterClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose_iter;
}

static void a_array_iter_init(AArrayIter *array_iter) {
}

static void l_dispose_iter(GObject *object) {
	AArrayIter *array_iter = A_ARRAY_ITER(object);
	a_unref(array_iter->array);
	G_OBJECT_CLASS(a_array_iter_parent_class)->dispose(object);

}
