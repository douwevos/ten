/*
   File:    buzpageview.h
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

#ifndef VIEW_BUZPAGEVIEW_H_
#define VIEW_BUZPAGEVIEW_H_

#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_PAGE_VIEW              (buz_page_view_get_type())
#define BUZ_PAGE_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_page_view_get_type(), BuzPageView))
#define BUZ_PAGE_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_PAGE_VIEW, BuzPageViewClass))
#define BUZ_IS_PAGE_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_PAGE_VIEW))
#define BUZ_IS_PAGE_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_PAGE_VIEW))
#define BUZ_PAGE_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_PAGE_VIEW, BuzPageViewClass))


typedef struct _BuzPageView               BuzPageView;
typedef struct _BuzPageViewPrivate        BuzPageViewPrivate;
typedef struct _BuzPageViewClass          BuzPageViewClass;


struct _BuzPageView {
	AObject parent;
};

struct _BuzPageViewClass {
	AObjectClass parent_class;
};


GType buz_page_view_get_type();

BuzPageView *buz_page_view_new();

void buz_page_view_set_height(BuzPageView *view, int height);
int buz_page_view_get_height(BuzPageView *view);

G_END_DECLS

#endif /* VIEW_BUZPAGEVIEW_H_ */
