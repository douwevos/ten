/*
   File:    buztextlinelayout.c
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

#include "buztextlinelayout.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzTextLineLayout"
#include <asupport.h>

struct _BuzTextLineLayoutPrivate {
	PangoLayoutLine *layout_line;
	BuzSize size;
};

G_DEFINE_TYPE_WITH_CODE(BuzTextLineLayout, buz_text_line_layout, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzTextLineLayout)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_text_line_layout_class_init(BuzTextLineLayoutClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_text_line_layout_init(BuzTextLineLayout *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzTextLineLayout *instance = BUZ_TEXT_LINE_LAYOUT(object);
//	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(instance);
	G_OBJECT_CLASS(buz_text_line_layout_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_text_line_layout_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzTextLineLayout *buz_text_line_layout_new(PangoLayoutLine *layout_line) {
	BuzTextLineLayout *result = g_object_new(BUZ_TYPE_TEXT_LINE_LAYOUT, NULL);
	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(result);
	a_object_construct((AObject *) result);
	a_ref(layout_line->layout);
	priv->layout_line = layout_line;
	PangoRectangle ink_rect;
	pango_layout_line_get_extents(priv->layout_line, NULL, &ink_rect);
	priv->size.height = (ink_rect.height+PANGO_SCALE-1)/PANGO_SCALE;
	priv->size.width = (ink_rect.width+PANGO_SCALE-1)/PANGO_SCALE;
	return result;
}

gboolean buz_text_line_layout_same_row(BuzTextLineLayout *line_layout, BuzTextLineLayout *other_line_layout) {
	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(line_layout);
	BuzTextLineLayoutPrivate *opriv = buz_text_line_layout_get_instance_private(other_line_layout);
	return priv->layout_line->layout==opriv->layout_line->layout;
}


const BuzSize buz_text_line_layout_get_size(BuzTextLineLayout *line_layout) {
	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(line_layout);
	return priv->size;
}

int buz_text_line_layout_view_x_at(BuzTextLineLayout *line_layout, int byte_index) {
	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(line_layout);

	int cursor_sub_line_idx = 0;
	int cursor_xpos = 0;
	pango_layout_index_to_line_x(priv->layout_line->layout, byte_index, FALSE, &cursor_sub_line_idx, &cursor_xpos);

	PangoLayoutLine *ll = pango_layout_get_line_readonly(priv->layout_line->layout, cursor_sub_line_idx);
	if (ll==priv->layout_line) {
		return cursor_xpos;
	}
	return -1;
}


int buz_text_line_layout_show(BuzTextLineLayout *line_layout, cairo_t *cr) {
	BuzTextLineLayoutPrivate *priv = buz_text_line_layout_get_instance_private(line_layout);
	int baseline = (pango_layout_get_baseline(priv->layout_line->layout))/PANGO_SCALE;
	cairo_rel_move_to(cr,0, baseline);
	pango_cairo_show_layout_line(cr, priv->layout_line);
	return priv->size.height;
}
