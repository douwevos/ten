/*
   File:    buzlineview.h
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

#ifndef VIEW_BUZLINEVIEW_H_
#define VIEW_BUZLINEVIEW_H_

#include "../layout/buztextlayout.h"
#include "../layout/buzlayoutcontext.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_LINE_VIEW              (buz_line_view_get_type())
#define BUZ_LINE_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_line_view_get_type(), BuzLineView))
#define BUZ_LINE_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_LINE_VIEW, BuzLineViewClass))
#define BUZ_IS_LINE_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_LINE_VIEW))
#define BUZ_IS_LINE_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_LINE_VIEW))
#define BUZ_LINE_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_LINE_VIEW, BuzLineViewClass))


typedef struct _BuzLineView               BuzLineView;
typedef struct _BuzLineViewPrivate        BuzLineViewPrivate;
typedef struct _BuzLineViewClass          BuzLineViewClass;

struct _BuzLineView {
	AObject parent;
};

struct _BuzLineViewClass {
	AObjectClass parent_class;
};


GType buz_line_view_get_type();

BuzLineView *buz_line_view_new();

void buz_line_view_update(BuzLineView *line_view, BuzLayoutContext *layout_context, AStringAnchored *text);

BuzTextLayout *buz_line_view_get_text_layout(BuzLineView *line_view);

G_END_DECLS

#endif /* VIEW_BUZLINEVIEW_H_ */
