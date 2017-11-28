/*
   File:    buzcursor.h
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

#ifndef DOCUMENT_BUZCURSOR_H_
#define DOCUMENT_BUZCURSOR_H_

#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_CURSOR              (buz_cursor_get_type())
#define BUZ_CURSOR(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_cursor_get_type(), BuzCursor))
#define BUZ_CURSOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_CURSOR, BuzCursorClass))
#define BUZ_IS_CURSOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_CURSOR))
#define BUZ_IS_CURSOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_CURSOR))
#define BUZ_CURSOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_CURSOR, BuzCursorClass))


typedef const struct _BuzCursor         BuzCursorAnchored;
typedef const struct _BuzCursor         BuzCursorShady;
typedef struct _BuzCursor               BuzCursor;
typedef struct _BuzCursorPrivate        BuzCursorPrivate;
typedef struct _BuzCursorClass          BuzCursorClass;


struct _BuzCursor {
	AAltObject parent;
};

struct _BuzCursorClass {
	AAltObjectClass parent_class;
};


GType buz_cursor_get_type();

BuzCursor *buz_cursor_new(long long row, int column);

long long buz_cursor_row(BuzCursorShady *cursor);
int buz_cursor_x_bytes(BuzCursorShady *cursor);
int buz_cursor_x_sub(BuzCursorShady *cursor);

void buz_cursor_set_x(BuzCursor *cursor, int x_bytes, int x_sub);

void buz_cursor_set_row(BuzCursor *cursor, long long row);

A_ALT_HEADERS(BuzCursor, buz_cursor);

G_END_DECLS

#endif /* DOCUMENT_BUZCURSOR_H_ */
