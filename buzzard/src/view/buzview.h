/*
   File:    buzview.h
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

#ifndef VIEW_BUZVIEW_H_
#define VIEW_BUZVIEW_H_

#include "../document/buzdocument.h"
#include "../layout/buzlayoutcontext.h"
#include "../document/content/buzrevision.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_VIEW              (buz_view_get_type())
#define BUZ_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_view_get_type(), BuzView))
#define BUZ_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_VIEW, BuzViewClass))
#define BUZ_IS_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_VIEW))
#define BUZ_IS_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_VIEW))
#define BUZ_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_VIEW, BuzViewClass))


typedef struct _BuzView               BuzView;
typedef struct _BuzViewPrivate        BuzViewPrivate;
typedef struct _BuzViewClass          BuzViewClass;


struct _BuzView {
	AObject parent;
};

struct _BuzViewClass {
	AObjectClass parent_class;
};

typedef struct _BuzViewDimensions BuzViewDimensions;

struct _BuzViewDimensions {
	long long top;
	int height;
	int left, width;
};

GType buz_view_get_type();

void buz_view_construct(BuzView *view, BuzDocument *document);

BuzView *buz_view_new(BuzDocument *document);
void buz_view_set_layout_context(BuzView *view, BuzLayoutContext *layout_context);

//void buz_view_move_cursor_right(BuzView *view, gboolean per_word);
//void buz_view_move_cursor_left(BuzView *view, gboolean per_word);

int buz_view_move_cursor_left_or_right(BuzView *view, gboolean move_right, gboolean per_word);
void buz_view_move_cursor_up_or_down(BuzView *view, int view_x_cursor, gboolean down);


void buz_view_set_top(BuzView *view, long long top);

void buz_view_set_view_size(BuzView *view, int width, int height);

const BuzViewDimensions buz_view_get_dimensions(BuzView *view);

long long buz_view_get_layout_height(BuzView *view);
int buz_view_get_layout_width(BuzView *view);

void buz_view_update_lines(BuzView *view);

AArray *buz_view_get_lines(BuzView *view, long long *first_line_y_view, long long *first_line_row);

G_END_DECLS

#endif /* VIEW_BUZVIEW_H_ */
