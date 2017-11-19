/*
   File:    buzlineview.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Nov 12, 2017
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

#include "buzlineview.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzLineView"
#include <asupport.h>

struct _BuzLineViewPrivate {
	BuzTextLayout *text_layout;
};

G_DEFINE_TYPE_WITH_CODE(BuzLineView, buz_line_view, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzLineView)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_line_view_class_init(BuzLineViewClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_line_view_init(BuzLineView *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzLineView *instance = BUZ_LINE_VIEW(object);
//	BuzLineViewPrivate *priv = buz_line_view_get_instance_private(instance);
	G_OBJECT_CLASS(buz_line_view_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_line_view_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzLineView *buz_line_view_new() {
	BuzLineView *result = g_object_new(BUZ_TYPE_LINE_VIEW, NULL);
	BuzLineViewPrivate *priv = buz_line_view_get_instance_private(result);
	a_object_construct((AObject *) result);
	return result;
}

void buz_line_view_update(BuzLineView *line_view, BuzLayoutContext *layout_context, AStringAnchored *text) {
	BuzLineViewPrivate *priv = buz_line_view_get_instance_private(line_view);

	if (priv->text_layout!=NULL) {
		if (buz_text_layout_get_layout_context(priv->text_layout)!=layout_context) {
			a_unref(priv->text_layout);
			priv->text_layout = buz_text_layout_new(layout_context);
		}
	} else {
		priv->text_layout = buz_text_layout_new(layout_context);
	}
	buz_text_layout_set_text(priv->text_layout, text);
}

BuzTextLayout *buz_line_view_get_text_layout(BuzLineView *line_view) {
	BuzLineViewPrivate *priv = buz_line_view_get_instance_private(line_view);
	return priv->text_layout;
}

