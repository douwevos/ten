/*
   File:    buzpageview.c
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

#include "buzpageview.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "BuzPageView"
#include <asupport.h>

struct _BuzPageViewPrivate {
	int height;
};

G_DEFINE_TYPE_WITH_CODE(BuzPageView, buz_page_view, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzPageView)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_page_view_class_init(BuzPageViewClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_page_view_init(BuzPageView *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzPageView *instance = BUZ_PAGE_VIEW(object);
//	BuzPageViewPrivate *priv = buz_page_view_get_instance_private(instance);
	G_OBJECT_CLASS(buz_page_view_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_page_view_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzPageView *buz_page_view_new() {
	BuzPageView *result = g_object_new(BUZ_TYPE_PAGE_VIEW, NULL);
	BuzPageViewPrivate *priv = buz_page_view_get_instance_private(result);
	a_object_construct((AObject *) result);
	return result;
}

void buz_page_view_set_height(BuzPageView *view, int height) {
	BuzPageViewPrivate *priv = buz_page_view_get_instance_private(view);
	priv->height = height;
}

int buz_page_view_get_height(BuzPageView *view) {
	BuzPageViewPrivate *priv = buz_page_view_get_instance_private(view);
	return priv->height;
}
