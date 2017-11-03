/*
   File:    buzdocumentloader.h
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

#ifndef DOCUMENT_BUZDOCUMENTLOADER_H_
#define DOCUMENT_BUZDOCUMENTLOADER_H_

#include "../document/buzdocument.h"
#include <antelope.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define BUZ_TYPE_DOCUMENT_LOADER              (buz_document_loader_get_type())
#define BUZ_DOCUMENT_LOADER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_document_loader_get_type(), BuzDocumentLoader))
#define BUZ_DOCUMENT_LOADER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_DOCUMENT_LOADER, BuzDocumentLoaderClass))
#define BUZ_IS_DOCUMENT_LOADER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_DOCUMENT_LOADER))
#define BUZ_IS_DOCUMENT_LOADER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_DOCUMENT_LOADER))
#define BUZ_DOCUMENT_LOADER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_DOCUMENT_LOADER, BuzDocumentLoaderClass))


typedef struct _BuzDocumentLoader               BuzDocumentLoader;
typedef struct _BuzDocumentLoaderPrivate        BuzDocumentLoaderPrivate;
typedef struct _BuzDocumentLoaderClass          BuzDocumentLoaderClass;


struct _BuzDocumentLoader {
	AObject parent;
};

struct _BuzDocumentLoaderClass {
	AObjectClass parent_class;
};


GType buz_document_loader_get_type();

BuzDocumentLoader *buz_document_loader_new();

void buz_document_loader_load(BuzDocumentLoader *loader, BuzDocument *document, GFile *file);

G_END_DECLS

#endif /* DOCUMENT_BUZDOCUMENTLOADER_H_ */
