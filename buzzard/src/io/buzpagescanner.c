/*
   File:    buzpagescanner.c
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

#include "buzpagescanner.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzPageScanner"
#include <asupport.h>

struct _BuzPageScannerPrivate {
	char *contents;
	gsize contents_length;
};

G_DEFINE_TYPE_WITH_CODE(BuzPageScanner, buz_page_scanner, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzPageScanner)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_page_scanner_class_init(BuzPageScannerClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_page_scanner_init(BuzPageScanner *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzPageScanner *instance = BUZ_PAGE_SCANNER(object);
//	BuzPageScannerPrivate *priv = buz_page_scanner_get_instance_private(instance);
	G_OBJECT_CLASS(buz_page_scanner_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_page_scanner_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzPageScanner *buz_page_scanner_new(char *contents, gsize contents_length) {
	BuzPageScanner *result = g_object_new(BUZ_TYPE_PAGE_SCANNER, NULL);
	BuzPageScannerPrivate *priv = buz_page_scanner_get_instance_private(result);
	a_object_construct((AObject *) result);
	priv->contents = contents;
	priv->contents_length = contents_length;
	return result;
}


BuzPage *buz_page_scanner_scan(BuzPageScanner *page_scanner) {
	BuzPageScannerPrivate *priv = buz_page_scanner_get_instance_private(page_scanner);
	AArray *row_list = a_array_new();


	char *text = priv->contents;
	char *end = text + priv->contents_length;

	char *off_last = text;
	gboolean keep_running = TRUE;

	char *off = text;
	while(keep_running && off<end) {
		if (a_array_size(row_list)>3) {
			break;
		}

		char ch = *off;
		if (ch==0xa) {
//			ChaLineEnd le = CHA_LINE_END_LF;
			char *next_off = off+1;
			if (off+1<end) {
				ch = *(off+1);
				if (ch==0xd) {
//					le = CHA_LINE_END_LFCR;
					next_off = off+2;
				} else {
				}
			}
			BuzRow *row = buz_row_new();
			AString *rowtext = buz_row_editable_text(row);
			a_string_append_chars_len(rowtext, off_last, (int) (off-off_last));
			a_log_debug("created rowtext=%o, length=%d", rowtext, (int) (off-off_last));
			a_array_add(row_list, row);
			a_unref(row);
//			keep_running = cha_scanned_line(off_last, off, le, data);
			off = next_off;
			off_last = next_off;
		} else if (ch==0xd) {
//			ChaLineEnd le = CHA_LINE_END_CR;
			char *next_off = off+1;
			if (off+1<end) {
				ch = *(off+1);
				if (ch==0xa) {
//					le = CHA_LINE_END_CRLF;
					next_off = off+2;
				} else {
				}
			}
			BuzRow *row = buz_row_new();
			AString *rowtext = buz_row_editable_text(row);
			a_string_append_chars_len(rowtext, off_last, (int) (off-off_last));
			a_log_debug("created rowtext=%o, length=%d", rowtext, (int) (off-off_last));
			a_array_add(row_list, row);
			a_unref(row);
//			keep_running = cha_scanned_line(off_last, off, le, data);
			off = next_off;
			off_last = next_off;
		} else {
			off++;
		}
	}

//	if (keep_running && off_last<end) {
//		cha_scanned_line(off_last, end, CHA_LINE_END_NONE, data);
//	}

	BuzPage *result = buz_page_new();
	buz_page_set_rows(result, row_list);
	a_log_debug("row_list_size=%d", a_array_size(row_list));
	a_unref(row_list);

	return result;
}
