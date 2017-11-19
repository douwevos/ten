/*
   File:    buzdocumentlistener.h
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

#ifndef DOCUMENT_BUZDOCUMENTLISTENER_H_
#define DOCUMENT_BUZDOCUMENTLISTENER_H_

#include "buzenrichmentdatamap.h"
#include "content/buzrevision.h"
#include <glib-object.h>

#define BUZ_TYPE_DOCUMENT_LISTENER                 (buz_document_listener_get_type ())
#define BUZ_DOCUMENT_LISTENER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUZ_TYPE_DOCUMENT_LISTENER, BuzDocumentListener))
#define BUZ_IS_DOCUMENT_LISTENER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_DOCUMENT_LISTENER))
#define BUZ_DOCUMENT_LISTENER_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), BUZ_TYPE_DOCUMENT_LISTENER, BuzDocumentListenerInterface))


typedef struct _BuzDocumentListener               BuzDocumentListener; /* dummy object */
typedef struct _BuzDocumentListenerInterface      BuzDocumentListenerInterface;

struct _BuzDocumentListenerInterface {
  GTypeInterface parent_iface;
  void (*newRevision) (BuzDocumentListener *self, BuzRevisionAnchored *revision);
  void (*onEnrichmentSlotNotify)(BuzDocumentListener *self, BuzRevision *a_revision, AStringAnchored *slot_key, gpointer user_data);
  void (*onSlotRegistered) (BuzDocumentListener *self, BuzRevisionAnchored *rev, BuzEnrichmentDataMap *enrichment_map, AStringAnchored *slot_key, int index);
};

GType buz_document_listener_get_type(void);

void buz_document_listener_new_revision(BuzDocumentListener *self, BuzRevisionAnchored *revision);

#endif /* DOCUMENT_BUZDOCUMENTLISTENER_H_ */
