/*
   File:    ahashmap.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 14, 2017
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

#include "ahashmap.h"
#include "../aaltobjectprivate.h"
#include "aarray.h"
#include <math.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AHashMap"
#include "../asupport.h"




#define LOAD_FACTOR           0.75
#define LOAD_FACTOR_INCREASE  13

struct _AContext {
	AAltObjectContext parent;
	GHashFunc hash_func;
	GEqualFunc equal_func;
	int size;
	int bucket_count;
	AArrayShady **buckets;
};

struct _AEditContext {
	AAltObjectEditor parent;
	int bucket_count;
	AArrayShady **buckets;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;


typedef struct _AHashMapIter	      AHashMapIter;
typedef struct _AHashMapIterClass	  AHashMapIterClass;

struct _AHashMapIter {
	GObject parent;
	AHashMap *hash_map;
	AContext *hash_map_context;
	int bucket_idx;
	AArrayShady *current_bucket;
	int current_bucket_index;
};

GType a_hash_map_iter_get_type(void);
#define A_TYPE_HASH_MAP_ITER            (a_hash_map_iter_get_type())


G_DEFINE_TYPE(AHashMap, a_hash_map, A_TYPE_ALT_OBJECT)

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_anchor_content(AAltObject *object);

static void a_hash_map_class_init(AHashMapClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	a_hash_map_get_instance_private(NULL);

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_anchor_content;
}

static void a_hash_map_init(AHashMap *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	G_OBJECT_CLASS(a_hash_map_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(a_hash_map_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


AHashMap *a_hash_map_new(GHashFunc hash_func, GEqualFunc equal_func) {
	AHashMap *result = g_object_new(A_TYPE_HASH_MAP, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AAltObjectPrivate *priv = a_alt_object_private(result);
	AContext *context = (AContext *) priv->context;
	context->hash_func = hash_func;
	context->equal_func = equal_func;
	context->size = 0;
	context->bucket_count = 0;
	context->buckets = NULL;
	AEditContext *editor = (AEditContext *) priv->editor;
	editor->buckets = NULL;
	editor->bucket_count = 0;
	return result;
}

static gconstpointer l_get_from_bucket_list(GEqualFunc equal_func, AArrayShady *bucket, gconstpointer key, int *index) {
	gconstpointer skey;
	gconstpointer svalue;
	gconstpointer result = NULL;
	AIterator *iterator = a_array_iterator(bucket);
	int cnt = 0;
	if (index) {
		*index =  -1;
	}

	while(a_iterator_next(iterator, &skey)) {
		a_iterator_next(iterator, &svalue);
		if (equal_func(key, skey)) {
			result = svalue;
			if (index) {
				*index = cnt;
			}
			break;
		}
		cnt+=2;
	}
	a_unref(iterator);
	return result;
}

static AArrayShady **l_create_buckets_list(int count) {
	if (count == 0) {
		return NULL;
	}
	return (AArrayShady **) g_malloc0(sizeof(AArrayShady *)*count);
}

static AArrayShady **l_clone_original_buckets(AHashMap *map) {
	AAltObjectPrivate *priv = a_alt_object_private(map);
	AEditContext *editor = (AEditContext *) priv->editor;
	int count = editor->bucket_count;
	a_log_detail("count = %d", count);
	AArrayShady **result = l_create_buckets_list(count);
	AArrayShady **out = result;
	AArrayShady **in = editor->buckets;
	for(count--; count>=0; count--) {
		a_log_trace("*in=%o", *in);
		*out = a_ref(*in);
		out++;
		in++;
	}
	a_log_detail("result=%p", result);
	return result;
}

static void l_rehash(AHashMap *map, int new_bucket_count, gboolean force) {
	AAltObjectPrivate *priv = a_alt_object_private(map);
	AContext *context = (AContext *) priv->context;
	if (new_bucket_count<LOAD_FACTOR_INCREASE) {
		new_bucket_count = LOAD_FACTOR_INCREASE;
	}
	a_log_debug("new_bucket_count=%d, bucket_count=%d", new_bucket_count, context->bucket_count);
	if (new_bucket_count==context->bucket_count && !force) {
		return;
	}

	AArrayShady **bucket_list = l_create_buckets_list(new_bucket_count);
	GHashFunc hash_func = context->hash_func;

	unsigned int hash, new_bucket_index;
	int bucket_index;
	gconstpointer key;
	gconstpointer value;
	AArray *new_bucket;
	for(bucket_index=0; bucket_index<context->bucket_count; bucket_index++) {
		AArrayShady *bucket = context->buckets[bucket_index];
		if (bucket) {
			AIterator *bucket_iter = a_array_iterator(bucket);
			while(a_iterator_next(bucket_iter, &key)) {
				a_iterator_next(bucket_iter, &value);
				hash = hash_func(key);
				new_bucket_index = hash%new_bucket_count;
				new_bucket = (AArray *) bucket_list[new_bucket_index];
				if (new_bucket == NULL) {
					new_bucket = a_array_new();
					bucket_list[new_bucket_index] = new_bucket;
				}
				a_array_add(new_bucket, key);
				a_array_add(new_bucket, value);
			}
			a_deref(bucket_iter);
		}
	}

	/* clear current buckets*/
	AEditContext *editor = (AEditContext *) priv->editor;
	if (editor && editor->buckets) {
		 if (editor->buckets==context->buckets) {
			 context->buckets = NULL;
			 context->bucket_count = 0;
		 }
	}
	bucket_index = context->bucket_count;
	for(bucket_index--; bucket_index>=0; bucket_index--) {
		a_deref(context->buckets[bucket_index]);
	}
	a_free_ptr(context->buckets);

	/* store new buckets and update modification count */
	context->buckets = bucket_list;
	context->bucket_count = new_bucket_count;
}


static void l_rehash_check_len(AHashMap *map, int size) {
	float nice_bucket_count = (float) size * LOAD_FACTOR;
	int wanted_bckt_cnt = ceilf(nice_bucket_count);
	wanted_bckt_cnt = (wanted_bckt_cnt-1+LOAD_FACTOR_INCREASE)/LOAD_FACTOR_INCREASE;
	wanted_bckt_cnt *= LOAD_FACTOR_INCREASE;
	l_rehash(map, wanted_bckt_cnt, FALSE);
}

static void l_rehash_check(AHashMap *map) {
	AAltObjectPrivate *priv = a_alt_object_private(map);
	AContext *context = (AContext *) priv->context;
	l_rehash_check_len(map, context->size);
}


gboolean a_hash_map_put(AHashMap *map, gconstpointer key, gconstpointer value) {
	AAltObjectPrivate *priv = a_alt_object_private(map);
	AContext *context = (AContext *) priv->context;

	unsigned int hash = context->hash_func(key);
	a_log_debug("map=%p, bucket_count=%d, hash=%d, key=%o", map, context->bucket_count, hash, key);
	a_ref(key);
	a_ref(value);
	if (context->bucket_count==0) {
		int new_bucket_count = LOAD_FACTOR_INCREASE;
		context->bucket_count = new_bucket_count;
		context->buckets = l_create_buckets_list(new_bucket_count);
	}
	unsigned int bucket_index = hash % context->bucket_count;
	a_log_detail("map=%p, bucket_index=%d", e_map, bucket_index);

	AEditContext *editor = (AEditContext *) priv->editor;

	if (editor && editor->buckets) {
		 if (editor->buckets==context->buckets) {
			 context->buckets = l_clone_original_buckets(map);
		 }
	}
	AArrayShady *bai = context->buckets[bucket_index];
	if (bai == NULL) {
		context->buckets[bucket_index] = a_array_new();
	} else if (!a_alt_object_is_mutable(bai)) {
		context->buckets[bucket_index] = a_array_mutable(bai);
		a_deref(bai);
	}

	AArray *e_bai = (AArray *) context->buckets[bucket_index];
	a_log_debug("%p, e_bucket=%p", map, e_bai);

	int key_index = -1;
	gconstpointer old_value = e_bai==NULL ? NULL : l_get_from_bucket_list(context->equal_func, e_bai, key, &key_index);
	a_log_trace("map=%p, old_value=%p, value=%p", map, old_value, value);
	if (old_value == value) {
		a_deref(key);
		a_deref(value);
		return FALSE;
	}

	a_log_trace("map=%p, key_index=%d, buckets=%p", map, key_index, context->buckets);
	if (key_index>=0) {
		a_array_set_at(e_bai, value, key_index+1);
	} else {
		a_array_add(e_bai, key);
		a_array_add(e_bai, value);
		context->size++;
	}
	a_unref(key);
	a_unref(value);
	l_rehash_check(map);
	a_log_trace("buckets=%p", context->buckets);
	return TRUE;
}


gconstpointer a_hash_map_get(AHashMapShady *map, gconstpointer key) {
	AAltObjectPrivate *priv = a_alt_object_private(map);
	AContext *context = (AContext *) priv->context;

	unsigned int hash = context->hash_func(key);
	unsigned int bucket_index = hash % context->bucket_count;
	AArrayShady *bucket = context->buckets[bucket_index];
	a_log_debug("%p, bucket=%p", map, bucket);

	gconstpointer result = NULL;
	if (bucket) {
		result = l_get_from_bucket_list(context->equal_func, bucket, key, NULL);
	}
	return result;
}

gboolean a_hash_map_equal(AHashMapShady *map_a, AHashMapShady *map_b, GEqualFunc equal_func_for_value) {
	if (map_a==map_b) {
		return TRUE;
	}
	if (map_a==NULL || map_b==NULL) {
		return FALSE;
	}
	AContext *context_a = (AContext *) a_alt_object_private(map_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(map_b)->context;
	if (context_a==context_b) {
		return TRUE;
	}
	if (context_a->size!=context_b->size) {
		return FALSE;
	}
	if (context_a->bucket_count==context_b->bucket_count && context_a->buckets==context_b->buckets) {
		return TRUE;
	}

	gboolean result = TRUE;
	int bucket_index;
	gconstpointer value;
	gconstpointer other_val;
	gconstpointer key;
	for(bucket_index=0; bucket_index<context_a->bucket_count && result; bucket_index++) {
		AArrayShady *bucket = (AArrayShady *) context_a->buckets[bucket_index];
		if (bucket) {
			if (a_array_size(bucket)!=0) {
				if (context_a->bucket_count==context_b->bucket_count) {
					AArrayShady *obucket = (AArrayShady *) context_b->buckets[bucket_index];
					if (obucket==bucket) {
						/* the two buckets are 100% identical */
						continue;
					}
				}
				AIterator *src_iter = a_array_iterator(bucket);
				while(a_iterator_next(src_iter, &key) && result) {
					a_iterator_next(src_iter, &value);
					other_val = a_hash_map_get(map_b, key);
					if (other_val==value) {
						continue;
					} else if (equal_func_for_value==NULL) {
						if (A_IS_OBJECT(value) && A_IS_OBJECT(other_val)) {
							result = a_object_equal((AObject *) value, (AObject *) other_val);
						} else {
							result = FALSE;
						}
					} else if (!equal_func_for_value(value, other_val)) {
						result = FALSE;
					}
				}
				a_unref(src_iter);
			}
		}
	}
	return result;
}


static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	return a_hash_map_equal((AHashMapShady *) object_a, (AHashMapShady *) object_b, NULL);
}


static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	AContext *context_a = (AContext *) context_from;
	AContext *context_b = (AContext *) context_to;

	context_b->equal_func = context_a->equal_func;
	context_b->hash_func = context_a->hash_func;
	context_b->size = context_a->size;
	context_b->bucket_count = context_a->bucket_count;
	if (context_a->bucket_count==0) {
		context_b->buckets = NULL;
		return;
	}
	context_b->buckets = l_create_buckets_list(context_b->bucket_count);

	int bucket_index;
	for(bucket_index=0; bucket_index<context_a->bucket_count; bucket_index++) {
		AArrayShady *bucket = context_a->buckets[bucket_index];
		AArray *new_bucket = NULL;
		if (bucket) {
			if (a_alt_object_is_mutable(bucket)) {
				new_bucket = a_array_new();
				a_array_add_all(new_bucket, bucket);
			} else {
				new_bucket = a_ref(bucket);
			}
		}
		context_b->buckets[bucket_index] = new_bucket;
	}
}

static void l_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	AEditContext *editor = (AEditContext *) priv->editor;

	a_log_debug("editor.buckets=%p, priv->buckets=%p", editor->buckets, context->buckets);
	if (editor->buckets!=context->buckets) {
		int bucket_index = context->bucket_count;
		for(bucket_index--; bucket_index>=0; bucket_index--) {
			AArrayShady *bucket = context->buckets[bucket_index];
			if (bucket) {
				if (!a_alt_object_is_mutable(bucket)) {
					context->buckets[bucket_index] = a_array_anchor(bucket);
				}
			}
		}
	}
}


#define A_HASH_MAP_ITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_hash_map_iter_get_type(), AHashMapIter))
#define A_HASH_MAP_ITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_HASH_MAP_ITER, AHashMapIterClass))
#define A_IS_HASH_MAP_ITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_HASH_MAP_ITER))
#define A_IS_HASH_MAP_ITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_HASH_MAP_ITER))
#define A_HASH_MAP_ITER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_HASH_MAP_ITER, AHashMapIterClass))


struct _AHashMapIterClass {
	GObjectClass parent_class;
};


static gboolean l_iter_next(AMapIterator *self, gconstpointer *okey, gconstpointer *ovalue) {
	AHashMapIter *this = (AHashMapIter *) self;
	if (this->current_bucket==NULL) {
		if (this->bucket_idx>=this->hash_map_context->bucket_count) {
			if (ovalue) {
				*ovalue = NULL;
			}
			if (okey) {
				*okey = NULL;
			}
			return FALSE;
		}
		while(this->current_bucket == NULL || a_array_size(this->current_bucket)==0) {
			this->bucket_idx++;
			if (this->bucket_idx>=this->hash_map_context->bucket_count) {
				if (ovalue) {
					*ovalue = NULL;
				}
				if (okey) {
					*okey = NULL;
				}
				return FALSE;
			}
			this->current_bucket = this->hash_map_context->buckets[this->bucket_idx];
		}
		this->current_bucket_index = 0;
	}
	gconstpointer key = a_array_at(this->current_bucket, this->current_bucket_index++);
	gconstpointer value = a_array_at(this->current_bucket, this->current_bucket_index++);
	if (this->current_bucket_index>=a_array_size(this->current_bucket)) {
		this->current_bucket = NULL;
	}
	if (ovalue) {
		*ovalue = value;
	}
	if (okey) {
		*okey = key;
	}
	return TRUE;
}

static void l_map_iterator_interface_init(AMapIteratorInterface *iface) {
	iface->next = l_iter_next;
}

G_DEFINE_TYPE_WITH_CODE(AHashMapIter, a_hash_map_iter, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(A_TYPE_MAP_ITERATOR, l_map_iterator_interface_init)
);

static void l_dispose_iter(GObject *object);


static void a_hash_map_iter_class_init(AHashMapIterClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose_iter;
}

static void a_hash_map_iter_init(AHashMapIter *array_iter) {
}

static void l_dispose_iter(GObject *object) {
	AHashMapIter *map_iter = A_HASH_MAP_ITER(object);
	a_deref(map_iter->hash_map);
	G_OBJECT_CLASS(a_hash_map_iter_parent_class)->dispose(object);

}
