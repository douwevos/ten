/*
   File:    buzcursor.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 21, 2017
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

#include "buzcursor.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzCursor"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	long long row;
	int column;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzCursor, buz_cursor, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);
static void l_as_string(const AObject *object, struct _AString *out);

static void buz_cursor_class_init(BuzCursorClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;
	o_class->asString = l_as_string;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->cloneContext = l_a_clone;
}

static void buz_cursor_init(BuzCursor *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	G_OBJECT_CLASS(buz_cursor_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzCursor *buz_cursor_new(long long row, int column) {
	BuzCursor *result = g_object_new(BUZ_TYPE_CURSOR, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->row = row;
	context->column = column;
	return result;
}

long long buz_cursor_get_row(BuzCursorShady *cursor) {
	AContext *context = (AContext *) a_alt_object_private(cursor)->context;
	return context->row;
}

int buz_cursor_get_column(BuzCursorShady *cursor) {
	AContext *context = (AContext *) a_alt_object_private(cursor)->context;
	return context->column;
}


BuzCursorAnchored *buz_cursor_anchor(BuzCursorShady *cursor) {
	return (BuzCursorAnchored *) a_alt_object_anchor(cursor);
}

BuzCursor *buz_cursor_mutable(BuzCursorShady *cursor) {
	return (BuzCursor *) a_alt_object_mutable(cursor);
}


static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->column = from->column;
	to->row = from->row;
}

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

	AContext *context_a = (AContext *) a_alt_object_private(object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(object_b)->context;
	if (context_a == context_b) {
		return TRUE;
	}

	return (context_a->column==context_b->column) && (context_a->row==context_b->row);
}

static void l_as_string(const AObject *object, struct _AString *out) {
	AContext *context = (AContext *) a_alt_object_private(object)->context;
	const char *name = g_type_name_from_instance((GTypeInstance *) object);
	a_string_format(out, "%s[col=%d, row=%d]", name, context->column, context->row);
}
