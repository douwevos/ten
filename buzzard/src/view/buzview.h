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


GType buz_view_get_type();

void buz_view_construct(BuzView *view, BuzDocument *document);

BuzView *buz_view_new(BuzDocument *document);

G_END_DECLS

#endif /* VIEW_BUZVIEW_H_ */
