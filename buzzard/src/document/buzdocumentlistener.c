/*
   File:    buzdocumentlistener.c
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

#include "buzdocumentlistener.h"

G_DEFINE_INTERFACE(BuzDocumentListener, buz_document_listener, G_TYPE_OBJECT);

static void buz_document_listener_default_init (BuzDocumentListenerInterface *iface) {
	iface->newRevision = NULL;
}

void buz_document_listener_new_revision(BuzDocumentListener *self, BuzRevisionAnchored *revision) {
	BUZ_DOCUMENT_LISTENER_GET_INTERFACE(self)->newRevision(self, revision);
}
