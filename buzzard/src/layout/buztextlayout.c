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

void buz_text_layout_move_cursor(BuzTextLayout *layout, BuzCursor *cursor, BuzCursorMove cursor_move, int last_x_cursor) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	last_x_cursor = last_x_cursor*PANGO_SCALE;
	gboolean limit_cursor = TRUE;
	int x_sub = buz_cursor_x_sub(cursor);
	int x_bytes = buz_cursor_x_bytes(cursor);
	int x_bytes_next = x_bytes;
	switch(cursor_move) {
		case BUZ_CURSOR_MOVE_TO_END_OF_LINE : {
			int sub_line;
			pango_layout_index_to_line_x(priv->pango_layout, x_bytes_next, FALSE, &sub_line, NULL);
			PangoLayoutLine *layout_line = pango_layout_get_line_readonly(priv->pango_layout, sub_line);
			x_bytes_next = layout_line->start_index+layout_line->length;
			x_sub = 0;
		} break;
		case BUZ_CURSOR_MOVE_TO_BEGIN_OF_LINE : {
			int sub_line;
			pango_layout_index_to_line_x(priv->pango_layout, x_bytes_next, FALSE, &sub_line, NULL);
			PangoLayoutLine *layout_line = pango_layout_get_line_readonly(priv->pango_layout, sub_line);
			x_bytes_next = layout_line->start_index;
			x_sub = 0;
		} break;
		case BUZ_CURSOR_MOVE_ONE_CHAR_LEFT : {
			int trailing;
			pango_layout_move_cursor_visually(priv->pango_layout, TRUE, x_bytes, 0, -1, &x_bytes_next, &trailing);
			if (x_bytes_next==x_bytes) {
				x_bytes_next--;
			}
			if (x_bytes_next<0) {
				if (limit_cursor) {
					x_bytes_next = 0;
				} else {
					x_bytes_next = 0;
				}
			}
		} break;
		case BUZ_CURSOR_MOVE_ONE_CHAR_RIGHT : {
			int trailing;
			pango_layout_move_cursor_visually(priv->pango_layout, TRUE, x_bytes, 0, 1, &x_bytes_next, &trailing);
			if (x_bytes_next==x_bytes) {
				x_bytes_next++;
			}
			if (x_bytes_next>a_string_length(priv->text)) {
				if (limit_cursor) {
					x_bytes_next = a_string_length(priv->text);
				} else {
					x_bytes_next = a_string_length(priv->text);
					x_sub++;
				}
			}
		} break;
		case BUZ_CURSOR_MOVE_ONE_LINE_UP : {
			int sub_line;
			pango_layout_index_to_line_x(priv->pango_layout, x_bytes_next, FALSE, &sub_line, NULL);
			if (sub_line>0) {
				sub_line--;
				PangoLayoutLine *layout_line = pango_layout_get_line_readonly(priv->pango_layout, sub_line);


				int trailing;
				x_sub = 0;
				if (!pango_layout_line_x_to_index(layout_line, last_x_cursor, &x_bytes_next, &trailing)) {
					x_bytes_next = layout_line->start_index+layout_line->length;
					if (!limit_cursor) {
						int xpp = 0;
						pango_layout_line_index_to_x(layout_line, x_bytes_next, TRUE, &xpp);
						xpp = last_x_cursor - xpp;
//						x_sub = (int) round(xpp/view_ctx.ps_space_width);
//						if (x_sub<0) {
//							x_sub = 0;
//						}
					}
				} else {
					x_bytes_next += trailing;
				}

			}

		} break;
		case BUZ_CURSOR_MOVE_ONE_LINE_DOWN : {
			int sub_line;
			pango_layout_index_to_line_x(priv->pango_layout, x_bytes_next, FALSE, &sub_line, NULL);
			if (sub_line<pango_layout_get_line_count(priv->pango_layout)-1) {
				sub_line++;
				PangoLayoutLine *layout_line = pango_layout_get_line_readonly(priv->pango_layout, sub_line);


				int trailing;
				x_sub = 0;
				if (!pango_layout_line_x_to_index(layout_line, last_x_cursor, &x_bytes_next, &trailing)) {
					x_bytes_next = layout_line->start_index+layout_line->length;
					if (!limit_cursor) {
						int xpp = 0;
						pango_layout_line_index_to_x(layout_line, x_bytes_next, TRUE, &xpp);
						xpp = last_x_cursor - xpp;
//						x_sub = (int) round(xpp/view_ctx.ps_space_width);
//						if (x_sub<0) {
//							x_sub = 0;
//						}
					}
				} else {
					x_bytes_next += trailing;
				}

			}

		} break;
		case BUZ_CURSOR_MOVE_TO_LAST_X_CURSOR : {
			int sub_line;
			pango_layout_index_to_line_x(priv->pango_layout, x_bytes_next, FALSE, &sub_line, NULL);
			PangoLayoutLine *layout_line = pango_layout_get_line_readonly(priv->pango_layout, sub_line);
			int trailing;
			x_sub = 0;
			if (!pango_layout_line_x_to_index(layout_line, last_x_cursor, &x_bytes_next, &trailing)) {
				x_bytes_next = layout_line->start_index+layout_line->length;
				if (!limit_cursor) {
					int xpp = 0;
					pango_layout_line_index_to_x(layout_line, x_bytes_next, TRUE, &xpp);
					xpp = last_x_cursor - xpp;
//						x_sub = (int) round(xpp/view_ctx.ps_space_width);
//						if (x_sub<0) {
//							x_sub = 0;
//						}
				}
			} else {
				x_bytes_next += trailing;
			}

		} break;
	}

	buz_cursor_set_x(cursor, x_bytes_next, x_sub);
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

BuzTextLineLayout *buz_text_layout_line_at_x_byte(BuzTextLayout *layout, int x_byte) {
	BuzTextLayoutPrivate *priv = buz_text_layout_get_instance_private(layout);
	int sub_line;
	pango_layout_index_to_line_x(priv->pango_layout, x_byte, FALSE, &sub_line, NULL);
	return (BuzTextLineLayout *) a_array_at(priv->lines, sub_line);
}
