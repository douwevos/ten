/*
   File:    buzpagematerializer.h
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

#ifndef DOCUMENT_BUZPAGEMATERIALIZER_H_
#define DOCUMENT_BUZPAGEMATERIALIZER_H_

#include "content/buzmaterializedpage.h"
#include <glib-object.h>

#define BUZ_TYPE_PAGE_MATERIALIZER                 (buz_page_materializer_get_type ())
#define BUZ_PAGE_MATERIALIZER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUZ_TYPE_PAGE_MATERIALIZER, BuzPageMaterializer))
#define BUZ_IS_PAGE_MATERIALIZER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_PAGE_MATERIALIZER))
#define BUZ_PAGE_MATERIALIZER_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), BUZ_TYPE_PAGE_MATERIALIZER, BuzPageMaterializerInterface))


typedef struct _BuzPageMaterializer               BuzPageMaterializer; /* dummy object */
typedef struct _BuzPageMaterializerInterface      BuzPageMaterializerInterface;

struct _BuzPageMaterializerInterface {
  GTypeInterface parent_iface;
  BuzMaterializedPage *(*materialize) (BuzPageMaterializer *self, const char *data, long long size, long long offset);
};

GType buz_page_materializer_get_type(void);

BuzMaterializedPage *(*materialize) (BuzPageMaterializer *self, const char *data, long long size, long long offset);

#endif /* DOCUMENT_BUZPAGEMATERIALIZER_H_ */
