/*
   File:    buzpagescanner.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 29, 2017
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

#ifndef IO_BUZPAGESCANNER_H_
#define IO_BUZPAGESCANNER_H_

#include "../document/content/buzmaterializedpage.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_PAGE_SCANNER              (buz_page_scanner_get_type())
#define BUZ_PAGE_SCANNER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_page_scanner_get_type(), BuzPageScanner))
#define BUZ_PAGE_SCANNER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_PAGE_SCANNER, BuzPageScannerClass))
#define BUZ_IS_PAGE_SCANNER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_PAGE_SCANNER))
#define BUZ_IS_PAGE_SCANNER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_PAGE_SCANNER))
#define BUZ_PAGE_SCANNER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_PAGE_SCANNER, BuzPageScannerClass))


typedef struct _BuzPageScanner               BuzPageScanner;
typedef struct _BuzPageScannerPrivate        BuzPageScannerPrivate;
typedef struct _BuzPageScannerClass          BuzPageScannerClass;


struct _BuzPageScanner {
	AObject parent;
};

struct _BuzPageScannerClass {
	AObjectClass parent_class;
};


GType buz_page_scanner_get_type();

BuzPageScanner *buz_page_scanner_new();

BuzMaterializedPage *buz_page_scanner_scan(BuzPageScanner *page_scanner);

G_END_DECLS

#endif /* IO_BUZPAGESCANNER_H_ */
