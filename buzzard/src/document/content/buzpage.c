/*
   File:    buzpage.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 20, 2017
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

#include "buzpage.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzPage"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	AArrayShady *rows;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzPage, buz_page, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void buz_page_class_init(BuzPageClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->clone_context_for_mutable = TRUE;
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_a_anchor_content;
}

static void buz_page_init(BuzPage *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		a_deref(context->rows);
	}
	G_OBJECT_CLASS(buz_page_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzPage *buz_page_new() {
	BuzPage *result = g_object_new(BUZ_TYPE_PAGE, NULL);
	a_monitor_set_active_for(result);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AAltObjectPrivate *priv = a_alt_object_private(result);
	AContext *context = (AContext *) priv->context;
	context->rows = a_array_new();
	BuzRow *row = buz_row_new();
	a_array_add((AArray *) context->rows, row);
	a_unref(row);
	return result;
}

int buz_page_row_count(BuzPageShady *page) {
	AAltObjectPrivate *priv = a_alt_object_private(page);
	AContext *context = (AContext *) priv->context;
	return a_array_size(context->rows);
}

BuzRowShady *buz_page_row_at(BuzPageShady *page, int row) {
	AAltObjectPrivate *priv = a_alt_object_private(page);
	AContext *context = (AContext *) priv->context;
	return (BuzRowShady *) a_array_at(context->rows, row);
}

BuzRow *buz_page_editable_row_at(BuzPage *page, int row) {
	AAltObjectPrivate *priv = a_alt_object_private(page);
	AContext *context = (AContext *) priv->context;
	BuzRowShady *row_shady = a_array_at(context->rows, row);
	if (a_alt_object_is_mutable(row_shady)) {
		return (BuzRow *) row_shady;
	}
	BuzRow *result = buz_row_mutable(row_shady);
	a_array_set_at((AArray *) context->rows, result, row);
	a_unref(result);
	return result;
}

void buz_page_set_rows(BuzPage *page, AArray *row_list) {
	AAltObjectPrivate *priv = a_alt_object_private(page);
	AContext *context = (AContext *) priv->context;
	a_array_remove_all((AArray *) context->rows);
	a_array_add_all((AArray *) context->rows, row_list);
}

void buz_page_debug(BuzPage *page) {
	a_log_on_debug({
		AAltObjectPrivate *priv = a_alt_object_private(page);
		AContext *context = (AContext *) priv->context;

		AIterator *rowsiter = a_array_iterator(context->rows);
		BuzRowShady *row = NULL;
		while(a_iterator_next(rowsiter, (gconstpointer *) (&row))) {
			a_log_print("DUMP", "#row:%O", row);
		}
		a_unref(rowsiter);
	});
}


A_ALT_C(BuzPage, buz_page);

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->rows = a_mutable_ref(from->rows);
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	a_log_debug("pre_anchor:%p, rows=%o",object, context->rows);
	buz_page_debug((BuzPage *) object);
	context->rows = a_array_anchor(context->rows);
	a_log_debug("post_anchor:%p, rows=%o",object, context->rows);
	buz_page_debug((BuzPage *) object);
}


static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

	AContext *context_a = (AContext *) a_alt_object_private((AObject *) object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private((AObject *) object_b)->context;
	if (context_a == context_b) {
		return TRUE;
	}

	return a_array_equal(context_a->rows, context_b->rows);
}

