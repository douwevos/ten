/*
   File:    buzdocument.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 19, 2017
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

#ifndef DOCUMENT_BUZDOCUMENT_H_
#define DOCUMENT_BUZDOCUMENT_H_

#include "buzdocumentlistener.h"
#include "content/buzrevision.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_DOCUMENT              (buz_document_get_type())
#define BUZ_DOCUMENT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_document_get_type(), BuzDocument))
#define BUZ_DOCUMENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_DOCUMENT, BuzDocumentClass))
#define BUZ_IS_DOCUMENT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_DOCUMENT))
#define BUZ_IS_DOCUMENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_DOCUMENT))
#define BUZ_DOCUMENT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_DOCUMENT, BuzDocumentClass))


typedef struct _BuzDocument               BuzDocument;
typedef struct _BuzDocumentPrivate        BuzDocumentPrivate;
typedef struct _BuzDocumentClass          BuzDocumentClass;


struct _BuzDocument {
	AObject parent;
};

struct _BuzDocumentClass {
	AObjectClass parent_class;
};


GType buz_document_get_type();

void buz_document_construct(BuzDocument *document);

BuzDocument *buz_document_new();

BuzRevisionAnchored *buz_document_get_revision_ref(BuzDocument *document);

void buz_document_post_revision(BuzDocument *document, BuzRevisionShady *c_revision);


void buz_document_add_listener(BuzDocument *document, BuzDocumentListener *listener);
void buz_document_remove_listener(BuzDocument *document, BuzDocumentListener *listener);

G_END_DECLS

#endif /* DOCUMENT_BUZDOCUMENT_H_ */
