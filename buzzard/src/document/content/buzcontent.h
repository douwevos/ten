/*
   File:    buzcontent.h
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

#ifndef DOCUMENT_BUZCONTENT_H_
#define DOCUMENT_BUZCONTENT_H_

#include "buzcursor.h"
#include "buzpage.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_CONTENT              (buz_content_get_type())
#define BUZ_CONTENT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_content_get_type(), BuzContent))
#define BUZ_CONTENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_CONTENT, BuzContentClass))
#define BUZ_IS_CONTENT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_CONTENT))
#define BUZ_IS_CONTENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_CONTENT))
#define BUZ_CONTENT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_CONTENT, BuzContentClass))


typedef const struct _BuzContent         BuzContentAnchored;
typedef const struct _BuzContent         BuzContentShady;
typedef struct _BuzContent               BuzContent;
typedef struct _BuzContentPrivate        BuzContentPrivate;
typedef struct _BuzContentClass          BuzContentClass;


struct _BuzContent {
	AAltObject parent;
};

struct _BuzContentClass {
	AAltObjectClass parent_class;
};


typedef struct _BuzRowLocation BuzRowLocation;
struct _BuzRowLocation {
	int page_index;
	int page_row_index;
};

GType buz_content_get_type();

BuzContent *buz_content_new();

int buz_content_page_count(BuzContentShady *content);
AIterator *buz_content_page_iterator(BuzContentShady *content);

BuzRowLocation buz_content_page_at_row(BuzContentShady *content, long long row);

BuzPageShady *buz_content_page_at(BuzContentShady *content, int page_index);

void buz_content_set_page_at(BuzContent *content, BuzPageShady *page, int page_index);

void buz_content_insert(BuzContent *content, AStringShady *txt, BuzCursorShady *cursor);

void buz_content_enrichment_remap(BuzContent *content, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index);

void buz_content_enrich(BuzContentAnchored *content, BuzEnrichmentDataMapAnchored *enrichment_map);
void buz_content_impoverish(BuzContentAnchored *content);

A_ALT_HEADERS(BuzContent, buz_content);

G_END_DECLS

#endif /* DOCUMENT_BUZCONTENT_H_ */
