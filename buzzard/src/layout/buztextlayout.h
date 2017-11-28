/*
   File:    buztextlayout.h
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

#ifndef LAYOUT_BUZTEXTLAYOUT_H_
#define LAYOUT_BUZTEXTLAYOUT_H_

#include "buztextlinelayout.h"
#include "buzlayoutcontext.h"
#include "../document/content/buzcursor.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_TEXT_LAYOUT              (buz_text_layout_get_type())
#define BUZ_TEXT_LAYOUT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_text_layout_get_type(), BuzTextLayout))
#define BUZ_TEXT_LAYOUT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_TEXT_LAYOUT, BuzTextLayoutClass))
#define BUZ_IS_TEXT_LAYOUT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_TEXT_LAYOUT))
#define BUZ_IS_TEXT_LAYOUT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_TEXT_LAYOUT))
#define BUZ_TEXT_LAYOUT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_TEXT_LAYOUT, BuzTextLayoutClass))

typedef enum _BuzCursorMove BuzCursorMove;

enum _BuzCursorMove {
	BUZ_CURSOR_MOVE_ONE_CHAR_RIGHT,
	BUZ_CURSOR_MOVE_ONE_CHAR_LEFT,
	BUZ_CURSOR_MOVE_TO_END_OF_LINE,
	BUZ_CURSOR_MOVE_TO_BEGIN_OF_LINE,
	BUZ_CURSOR_MOVE_ONE_LINE_UP,
	BUZ_CURSOR_MOVE_ONE_LINE_DOWN,
	BUZ_CURSOR_MOVE_TO_LAST_X_CURSOR,
};


typedef struct _BuzTextLayout               BuzTextLayout;
typedef struct _BuzTextLayoutPrivate        BuzTextLayoutPrivate;
typedef struct _BuzTextLayoutClass          BuzTextLayoutClass;


struct _BuzTextLayout {
	AObject parent;
};

struct _BuzTextLayoutClass {
	AObjectClass parent_class;
};


GType buz_text_layout_get_type();

BuzTextLayout *buz_text_layout_new(BuzLayoutContext *context);

BuzLayoutContext *buz_text_layout_get_layout_context(BuzTextLayout *layout);

void buz_text_layout_set_text(BuzTextLayout *layout, AStringShady *text);

void buz_text_layout_move_cursor(BuzTextLayout *layout, BuzCursor *cursor, BuzCursorMove cursor_move, int last_x_cursor);

void buz_text_layout_show(BuzTextLayout *layout, cairo_t *cairo);

double buz_text_layout_get_height(BuzTextLayout *layout);

const BuzSize buz_text_layout_get_size(BuzTextLayout *layout);

int buz_text_layout_line_count(BuzTextLayout *layout);

BuzTextLineLayout *buz_text_layout_line_at(BuzTextLayout *layout, int index);
BuzTextLineLayout *buz_text_layout_line_at_x_byte(BuzTextLayout *layout, int x_byte);

G_END_DECLS

#endif /* LAYOUT_BUZTEXTLAYOUT_H_ */
