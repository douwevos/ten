/*
   File:    buzrow.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 22, 2017
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

#ifndef DOCUMENT_BUZROW_H_
#define DOCUMENT_BUZROW_H_

#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_ROW              (buz_row_get_type())
#define BUZ_ROW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_row_get_type(), BuzRow))
#define BUZ_ROW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_ROW, BuzRowClass))
#define BUZ_IS_ROW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_ROW))
#define BUZ_IS_ROW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_ROW))
#define BUZ_ROW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_ROW, BuzRowClass))


typedef const struct _BuzRow         BuzRowAnchored;
typedef const struct _BuzRow         BuzRowShady;
typedef struct _BuzRow               BuzRow;
typedef struct _BuzRowPrivate        BuzRowPrivate;
typedef struct _BuzRowClass          BuzRowClass;


struct _BuzRow {
	AAltObject parent;
};

struct _BuzRowClass {
	AAltObjectClass parent_class;
};


GType buz_row_get_type();

BuzRow *buz_row_new();

AStringShady *buz_row_text(BuzRowShady *row);
AString *buz_row_editable_text(BuzRow *row);

BuzRowAnchored *buz_row_anchor(BuzRowShady *row);
BuzRow *buz_row_mutable(BuzRowShady *row);

G_END_DECLS

#endif /* DOCUMENT_BUZROW_H_ */
