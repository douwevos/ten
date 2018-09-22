/*
   File:    buzrawpage.c
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

#include "buzrawpage.h"
#include "buzpageprivate.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzRawPage"
#include <asupport.h>

struct _AContext {
	struct _BuzPageContext parent;
	goffset start;
	goffset end;
	int estimated_row_count;

	AArray *materialized_list;
};

typedef struct _AContext AContext;

G_DEFINE_TYPE(BuzRawPage, buz_raw_page, BUZ_TYPE_PAGE);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static int l_row_count(BuzPageShady *page);


static void buz_raw_page_class_init(BuzRawPageClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	BuzPageClass *page_class = BUZ_PAGE_CLASS(clazz);
	page_class->rowCount = l_row_count;
}

static void buz_raw_page_init(BuzRawPage *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzRawPage *instance = BUZ_RAW_PAGE(object);
//	BuzRawPagePrivate *priv = buz_raw_page_get_instance_private(instance);
	G_OBJECT_CLASS(buz_raw_page_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_raw_page_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzRawPage *buz_raw_page_new(goffset start, goffset end) {
	BuzRawPage *result = g_object_new(BUZ_TYPE_RAW_PAGE, NULL);
	BuzRawPagePrivate *priv = buz_raw_page_get_instance_private(result);
	buz_page_construct((BuzPage *) result);
	long long rc = (end-start)/30;
	if (rc>0x7FFFFFFF) {
		rc = 0x7FFFFFFF;
	}
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->estimated_row_count = (int) rc;
	return result;
}

static int l_row_count(BuzPageShady *page) {
	AContext *context = (AContext *) a_alt_object_private(page)->context;
	return context->estimated_row_count;
}
