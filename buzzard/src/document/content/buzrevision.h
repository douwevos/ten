/*
   File:    buzrevision.h
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

#ifndef DOCUMENT_BUZREVISION_H_
#define DOCUMENT_BUZREVISION_H_

#include "buzcontent.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_REVISION              (buz_revision_get_type())
#define BUZ_REVISION(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_revision_get_type(), BuzRevision))
#define BUZ_REVISION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_REVISION, BuzRevisionClass))
#define BUZ_IS_REVISION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_REVISION))
#define BUZ_IS_REVISION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_REVISION))
#define BUZ_REVISION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_REVISION, BuzRevisionClass))


typedef const struct _BuzRevision         BuzRevisionAnchored;
typedef const struct _BuzRevision         BuzRevisionShady;
typedef struct _BuzRevision               BuzRevision;
typedef struct _BuzRevisionPrivate        BuzRevisionPrivate;
typedef struct _BuzRevisionClass          BuzRevisionClass;


struct _BuzRevision {
	AAltObject parent;
};

struct _BuzRevisionClass {
	AAltObjectClass parent_class;
};


GType buz_revision_get_type();

BuzRevision *buz_revision_new();

void buz_revision_insert(BuzRevision *revision, AStringShady *text);

void buz_revision_set_page_at(BuzRevision *revision, BuzPageShady *page, int page_index);

AArrayShady *buz_revision_get_cursors(BuzRevisionShady *revision);
BuzContentShady *buz_revision_get_content(BuzRevisionShady *revision);

int buz_revision_page_count(BuzRevisionShady *revision);
AIterator *buz_revision_page_iterator(BuzRevisionShady *revision);

void buz_revision_enrichment_remap(BuzRevisionAnchored *revision, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_anchored, BuzEnrichmentAction action, int index);

void buz_revision_enrich(BuzRevisionAnchored *revision, BuzEnrichmentDataMapAnchored *enrichment_map);
void buz_revision_impoverish(BuzRevisionAnchored *revision);

A_ALT_HEADERS(BuzRevision, buz_revision);

G_END_DECLS

#endif /* DOCUMENT_BUZREVISION_H_ */
