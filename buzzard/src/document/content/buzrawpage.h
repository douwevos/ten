/*
   File:    buzrawpage.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Dec 5, 2017
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

#ifndef DOCUMENT_CONTENT_BUZRAWPAGE_H_
#define DOCUMENT_CONTENT_BUZRAWPAGE_H_

#include "buzpage.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_RAW_PAGE              (buz_raw_page_get_type())
#define BUZ_RAW_PAGE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_raw_page_get_type(), BuzRawPage))
#define BUZ_RAW_PAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_RAW_PAGE, BuzRawPageClass))
#define BUZ_IS_RAW_PAGE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_RAW_PAGE))
#define BUZ_IS_RAW_PAGE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_RAW_PAGE))
#define BUZ_RAW_PAGE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_RAW_PAGE, BuzRawPageClass))


typedef struct _BuzRawPage               BuzRawPage;
typedef struct _BuzRawPagePrivate        BuzRawPagePrivate;
typedef struct _BuzRawPageClass          BuzRawPageClass;


struct _BuzRawPage {
	BuzPage parent;
};

struct _BuzRawPageClass {
	BuzPageClass parent_class;
};


GType buz_raw_page_get_type();

BuzRawPage *buz_raw_page_new();

G_END_DECLS

#endif /* DOCUMENT_CONTENT_BUZRAWPAGE_H_ */
