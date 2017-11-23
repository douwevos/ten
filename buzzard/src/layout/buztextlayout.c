/*
   File:    buztextlayout.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 26, 2017
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

#include "buztextlayout.h"


#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzTextLayout"
#include <asupport.h>

struct _BuzTextLayoutPrivate {
	BuzLayoutContext *context;
	AStringAnchored *text;
	PangoLayout *pango_layout;

	AArray *lines;
	BuzSize size;
};

G_DEFINE_TYPE_WITH_CODE(BuzTextLayout, buz_text_layout, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzTextLayout)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_text_layout_class_init(BuzTextLayoutClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_text_layout_init(BuzTextLayout *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzTextLayout *instance = BUZ_TEXT_LAYOUT(object);
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(instance);
	a_unref(priv->context);
	a_unref(priv->pango_layout);
	a_unref(priv->text);
	G_OBJECT_CLASS(buz_text_layout_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_text_layout_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzTextLayout *buz_text_layout_new(BuzLayoutContext *context) {
	BuzTextLayout *result = g_object_new(BUZ_TYPE_TEXT_LAYOUT, NULL);
	a_object_construct((AObject *) result);
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(result);
	priv->text = NULL;
	priv->context = a_ref(context);
	PangoContext *pango_context = buz_layout_context_get_pango_context(context);
	priv->pango_layout = pango_layout_new(pango_context);
	priv->lines = a_array_new();
	return result;
}


BuzLayoutContext *buz_text_layout_get_layout_context(BuzTextLayout *layout) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	return priv->context;
}

void buz_text_layout_set_text(BuzTextLayout *layout, AStringShady *text) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	if (priv->text == text) {
		return;
	}
	priv->text = a_string_anchor(a_ref(text));
	if (a_string_length(priv->text)==0) {
		pango_layout_set_text(priv->pango_layout, " ", 1);
	} else {
		pango_layout_set_text(priv->pango_layout, a_string_chars(priv->text), a_string_length(priv->text));
	}
	priv->size.height = 0;
	priv->size.width = 0;
	a_array_remove_all(priv->lines);
	int line_count = pango_layout_get_line_count(priv->pango_layout);
	int line_idx;
	for(line_idx=0; line_idx<line_count; line_idx++) {
		BuzTextLineLayout *line_layout = buz_text_line_layout_new(pango_layout_get_line_readonly(priv->pango_layout, line_idx));
		const BuzSize ll_size = buz_text_line_layout_get_size(line_layout);
		priv->size.height += ll_size.height;
		if (ll_size.width>priv->size.width) {
			priv->size.width = ll_size.width;
		}
		a_array_add(priv->lines, line_layout);
		a_unref(line_layout);
	}
}

void buz_text_layout_show(BuzTextLayout *layout, cairo_t *cairo) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	pango_cairo_show_layout(cairo, priv->pango_layout);
}

double buz_text_layout_get_height(BuzTextLayout *layout) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	return priv->size.height;
}

const BuzSize buz_text_layout_get_size(BuzTextLayout *layout) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	return priv->size;
}


int buz_text_layout_line_count(BuzTextLayout *layout) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	return pango_layout_get_line_count(priv->pango_layout);
}

BuzTextLineLayout *buz_text_layout_line_at(BuzTextLayout *layout, int index) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	return (BuzTextLineLayout *) a_array_at(priv->lines, index);
}
