/*
   File:    aaltobject.c
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

#include "aaltobjectprivate.h"
#include "asupport.h"
#include "astring.h"

G_DEFINE_TYPE_WITH_CODE(AAltObject, a_alt_object, A_TYPE_OBJECT,
		G_ADD_PRIVATE(AAltObject)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static void l_as_string(const AObject *object, struct _AString *out);

static void a_alt_object_class_init(AAltObjectClass *clazz) {
	clazz->clone_context_for_mutable = FALSE;
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	AObjectClass *a_class = A_OBJECT_CLASS(clazz);
	a_class->asString = l_as_string;
}

static void a_alt_object_init(AAltObject *instance) {
}

static void l_dispose(GObject *object) {
	AAltObjectPrivate *priv = a_alt_object_get_instance_private((AAltObject *) object);
	if (priv->editor) {
		if (priv->editor->origin) {
			AAltObjectPrivate *orgpriv = a_alt_object_get_instance_private((AAltObject *) priv->editor->origin);
			if (priv->context==orgpriv->context) {
				priv->context = NULL;
			}
			a_unref(priv->editor->origin);
			priv->editor->origin = NULL;
		}
	}

	G_OBJECT_CLASS(a_alt_object_parent_class)->dispose(object);
}

static void l_finalize(GObject *object) {
	AAltObjectClass *a_cls = A_ALT_OBJECT_GET_CLASS(object);
	AAltObject *alt_object = (AAltObject *) object;
	AAltObjectPrivate *priv = a_alt_object_get_instance_private(alt_object);
	const AAltObjectContext *orig_context = a_alt_object_editor_get_original_context((AAltObjectEditor *) priv->editor);
	if (priv->context && orig_context!=priv->context) {

		if (g_atomic_int_dec_and_test(&(priv->context->ref_count))) {
			g_slice_free1(a_cls->context_size, priv->context);
//			printf("a_alt_object_anchor: free context %p, orig_context=%p, editor=%p\n", priv->context, orig_context, priv->editor);
			priv->context = NULL;
		}
	}

	if (priv->editor) {
		g_slice_free1(a_cls->editor_size, priv->editor);
		priv->editor = NULL;
	}

	G_OBJECT_CLASS(a_alt_object_parent_class)->finalize(object);
}


void a_alt_object_construct(AAltObject *object, gboolean mutable) {
	AAltObjectPrivate *priv = a_alt_object_get_instance_private(object);
	AAltObjectClass *a_cls = A_ALT_OBJECT_GET_CLASS(object);
	priv->context = g_slice_alloc0(a_cls->context_size);
	if (mutable) {
		priv->editor = g_slice_alloc0(a_cls->editor_size);
	}
	priv->context->ref_count = 1;
	a_object_construct((AObject *) object);
}

AAltObjectPrivate *a_alt_object_private(gconstpointer object) {
	return a_alt_object_get_instance_private(A_ALT_OBJECT(object));
}


static AAltObjectContext *l_clone(AAltObjectClass *a_cls, const AAltObjectContext *context) {
	AAltObjectContext *result = g_slice_alloc0(a_cls->context_size);
	result->ref_count = 1;
	if (a_cls->cloneContext) {
		a_cls->cloneContext(context, result);
	}
	return result;
}

const AAltObjectContext *a_alt_object_editor_get_original_context(const AAltObjectEditor *editor) {
	if (editor!=NULL && editor->origin!=NULL) {
		AAltObjectPrivate *priv = a_alt_object_get_instance_private((AAltObject *) editor->origin);
		return priv->context;
	}
	return NULL;
}


static void l_as_string(const AObject *object, struct _AString *out) {
	const char *iname = g_type_name_from_instance((GTypeInstance *) object);
	AAltObjectPrivate *priv = a_alt_object_get_instance_private((AAltObject *) object);
	a_string_format(out, "%s<%s:%p>[%p]", iname, priv->editor ? "mut" : "anc", priv->context, object);
}

gboolean a_alt_object_is_mutable(gconstpointer object) {
	gboolean result = FALSE;
	if (A_IS_ALT_OBJECT(object)) {
		AAltObjectPrivate *priv = a_alt_object_get_instance_private(A_ALT_OBJECT(object));
		result = priv->editor!=NULL;
	}
	return result;
}


gconstpointer a_alt_object_anchor(gconstpointer object) {
	if (object==NULL) {
		return NULL;
	}
	AAltObject *a_obj = A_ALT_OBJECT(object);
	AAltObjectPrivate *priv = a_alt_object_get_instance_private(a_obj);
	AAltObjectEditor *editor = g_atomic_pointer_get(&(priv->editor));
	/* the fastest: the object is already anchored we just return it */
	if (editor==NULL) {
#ifdef A_ALT_OBJECT_DEBUG
		printf("object:%p(%s), editor is NULL\n", object, g_type_name_from_instance(object));
#endif
		// was already anchored.
		return object;
	}

	/* if the context of the object is the same as the context of the original then just return the original */
	AAltObjectContext *orign_context = NULL;
	if (editor->origin) {
		AAltObjectPrivate *orign_priv = a_alt_object_get_instance_private((AAltObject *) editor->origin);
		orign_context = orign_priv->context;

		if (priv->context == orign_context) {
			void *result = a_ref((gpointer) editor->origin);
#ifdef A_ALT_OBJECT_DEBUG
			printf("a_alt_object_anchor(%d):object=%p, priv->context=%p, orign_context=%p\n", __LINE__, object, priv->context, orign_context);
#endif
			a_unref((gpointer) object);
			return result;
		}
#ifdef A_ALT_OBJECT_DEBUG
	} else {
		printf("object:%p(%s), no origin\n", object, g_type_name_from_instance(object));
#endif
	}


	/* anchor children */
	AAltObjectClass *a_class = A_ALT_OBJECT_GET_CLASS(a_obj);
	if (a_class->anchorContent) {
#ifdef A_ALT_OBJECT_DEBUG
		printf("object:%p(%s), anchoring content\n", object, g_type_name_from_instance(object));
#endif
		a_class->anchorContent(a_obj);
	}

	if (editor->origin) {
		AObjectClass *o_class = A_OBJECT_GET_CLASS(a_obj);

		/* if content of the context is the same as the content of the original the just return the original */
		if (o_class->equal(A_OBJECT(a_obj),  A_OBJECT(editor->origin))) {
			void *result = a_ref((gpointer) editor->origin);
//			printf("a_alt_object_anchor(%d):object=%p, result=%p, editor->origin=%p\n", __LINE__, object, result, editor->origin);
			a_unref((gpointer) object);
			return result;
		}
	}

	if (((GObject *) object)->ref_count==1) {
		/* there is only one reference, this must be the only editor. We just convert it to anchored and return it */
		if (editor->origin) {
			a_unref(priv->editor->origin);
			priv->editor->origin = NULL;
		}
		g_slice_free1(a_class->editor_size, priv->editor);
		priv->editor = NULL;
		return object;
	}

	/* We create a new anchored object. The current editable object is modified so that the origin (if any) is changes to the new anchored object */
	GTypeInstance *objinst = (GTypeInstance *) object;

	GType type = objinst->g_class->g_type;

	AAltObject *result = g_object_new(type, NULL);
	AAltObjectPrivate *rpriv = a_alt_object_get_instance_private(result);
	rpriv->editor = NULL;
	rpriv->context = priv->context;
	a_object_construct((AObject *) result);

	if (editor->origin) {
		a_unref((gpointer) editor->origin);
	}
	editor->origin = a_ref(result);

//	printf("a_alt_object_anchor(%d):object=%p, result=%p, editor->origin=%p, context=%p\n", __LINE__, object, result, editor->origin, rpriv->context);

	a_unref(object);

	return result;
}


gpointer a_alt_object_mutable(gconstpointer object) {
	if (object==NULL) {
		return NULL;
	}
	AAltObject *a_obj = A_ALT_OBJECT(object);
	AAltObjectPrivate *priv = a_alt_object_get_instance_private(a_obj);

	GTypeInstance *objinst = (GTypeInstance *) object;
	GType type = objinst->g_class->g_type;

//	printf("###################### ast.type=%ld, type=%ld\n", a_string_get_type(), type);

	AAltObject *result = g_object_new(type, NULL);
	AAltObjectPrivate *rpriv = a_alt_object_get_instance_private(result);
	AAltObjectClass *a_cls = A_ALT_OBJECT_GET_CLASS(object);
	AAltObjectEditor *editor = g_slice_alloc0(a_cls->editor_size);
	rpriv->editor = editor;

	AAltObjectEditor *old_editor = g_atomic_pointer_get(&(priv->editor));
	if (old_editor) {
		/* the source object is still mutable */
		const AAltObjectContext *old_editor_context = a_alt_object_editor_get_original_context((AAltObjectEditor *) old_editor);
		if ((old_editor_context==priv->context) && !a_cls->clone_context_for_mutable) {
			/* the source object is mutable but has not changed since it became mutable. We can reference the original context */
			editor->origin = a_ref(old_editor->origin);
			rpriv->context = priv->context;
			g_atomic_int_inc(&(rpriv->context->ref_count));
		} else {
			rpriv->context = l_clone(a_cls, priv->context);
		}
	} else {
		/* the source object is anchored */
		editor->origin = a_ref((gpointer) object);
		if (a_cls->clone_context_for_mutable) {
			rpriv->context = l_clone(a_cls, priv->context);
		} else {
			rpriv->context = priv->context;
			g_atomic_int_inc(&(rpriv->context->ref_count));
		}

	}

//	printf("a_alt_object_mutable(%d):object=%p, context=%p\n", __LINE__, object, rpriv->context);

//	printf("###################### name=%s\n", g_type_name_from_instance(result));

	a_object_construct((AObject *) result);
	return result;
}

gpointer a_mutable_ref(gconstpointer object) {
	if (A_IS_ALT_OBJECT(object)) {
		return a_alt_object_mutable(object);
	}
	return a_ref(object);
}

