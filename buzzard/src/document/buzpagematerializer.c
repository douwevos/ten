/*
   File:    buzpagematerializer.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Nov 29, 2017
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

#include "buzpagematerializer.h"

G_DEFINE_INTERFACE(BuzPageMaterializer, buz_page_materializer, G_TYPE_OBJECT);

static void buz_page_materializer_default_init (BuzPageMaterializerInterface *iface) {
	iface->materialize = NULL;
}

BuzMaterializedPage *buz_page_materializer_materialize(BuzPageMaterializer *self, const char *data, long long size, long long offset) {
	return BUZ_PAGE_MATERIALIZER_GET_INTERFACE(self)->materialize(self, data, size, offset);
}
