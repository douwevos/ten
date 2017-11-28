/*
   File:    buztextlinelayout.h
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

#ifndef LAYOUT_BUZTEXTLINELAYOUT_H_
#define LAYOUT_BUZTEXTLINELAYOUT_H_

#include <gtk/gtk.h>
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_TEXT_LINE_LAYOUT              (buz_text_line_layout_get_type())
#define BUZ_TEXT_LINE_LAYOUT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_text_line_layout_get_type(), BuzTextLineLayout))
#define BUZ_TEXT_LINE_LAYOUT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_TEXT_LINE_LAYOUT, BuzTextLineLayoutClass))
#define BUZ_IS_TEXT_LINE_LAYOUT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_TEXT_LINE_LAYOUT))
#define BUZ_IS_TEXT_LINE_LAYOUT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_TEXT_LINE_LAYOUT))
#define BUZ_TEXT_LINE_LAYOUT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_TEXT_LINE_LAYOUT, BuzTextLineLayoutClass))


typedef struct _BuzTextLineLayout               BuzTextLineLayout;
typedef struct _BuzTextLineLayoutPrivate        BuzTextLineLayoutPrivate;
typedef struct _BuzTextLineLayoutClass          BuzTextLineLayoutClass;


struct _BuzTextLineLayout {
	AObject parent;
};

struct _BuzTextLineLayoutClass {
	AObjectClass parent_class;
};

struct _BuzSize {
	int width, height;
};

typedef struct _BuzSize BuzSize;


GType buz_text_line_layout_get_type();

BuzTextLineLayout *buz_text_line_layout_new(PangoLayoutLine *layout_line);

gboolean buz_text_line_layout_same_row(BuzTextLineLayout *line_layout, BuzTextLineLayout *other_line_layout);

const BuzSize buz_text_line_layout_get_size(BuzTextLineLayout *line_layout);

int buz_text_line_layout_view_x_at(BuzTextLineLayout *line_layout, int byte_index);

int buz_text_line_layout_show(BuzTextLineLayout *line_layout, cairo_t *cr, int view_left, int view_width);

G_END_DECLS

#endif /* LAYOUT_BUZTEXTLINELAYOUT_H_ */
