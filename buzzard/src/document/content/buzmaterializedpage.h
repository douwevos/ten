/*
   File:    buzmaterializedpage.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Nov 11, 2017
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

#ifndef DOCUMENT_CONTENT_BUZMATERIALIZEDPAGE_H_
#define DOCUMENT_CONTENT_BUZMATERIALIZEDPAGE_H_

#include "buzpage.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_MATERIALIZED_PAGE              (buz_materialized_page_get_type())
#define BUZ_MATERIALIZED_PAGE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_materialized_page_get_type(), BuzMaterializedPage))
#define BUZ_MATERIALIZED_PAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_MATERIALIZED_PAGE, BuzMaterializedPageClass))
#define BUZ_IS_MATERIALIZED_PAGE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_MATERIALIZED_PAGE))
#define BUZ_IS_MATERIALIZED_PAGE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_MATERIALIZED_PAGE))
#define BUZ_MATERIALIZED_PAGE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_MATERIALIZED_PAGE, BuzMaterializedPageClass))


typedef const struct _BuzMaterializedPage         BuzMaterializedPageAnchored;
typedef const struct _BuzMaterializedPage         BuzMaterializedPageShady;
typedef struct _BuzMaterializedPage               BuzMaterializedPage;
typedef struct _BuzMaterializedPagePrivate        BuzMaterializedPagePrivate;
typedef struct _BuzMaterializedPageClass          BuzMaterializedPageClass;


struct _BuzMaterializedPage {
	BuzPage parent;
};

struct _BuzMaterializedPageClass {
	BuzPageClass parent_class;
};


GType buz_materialized_page_get_type();

BuzMaterializedPage *buz_materialized_page_new();

BuzRowShady *buz_materialized_page_row_at(BuzMaterializedPageShady *page, int row);

BuzRow *buz_materialized_page_editable_row_at(BuzMaterializedPage *page, int row);

void buz_materialized_page_set_rows(BuzMaterializedPage *page, AArray *row_list);


A_ALT_HEADERS(BuzMaterializedPage, buz_materialized_page)

G_END_DECLS

#endif /* DOCUMENT_CONTENT_BUZMATERIALIZEDPAGE_H_ */
