/*
   File:    buzpage.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 20, 2017
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

#ifndef DOCUMENT_BUZPAGE_H_
#define DOCUMENT_BUZPAGE_H_

#include "buzrow.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_PAGE              (buz_page_get_type())
#define BUZ_PAGE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_page_get_type(), BuzPage))
#define BUZ_PAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_PAGE, BuzPageClass))
#define BUZ_IS_PAGE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_PAGE))
#define BUZ_IS_PAGE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_PAGE))
#define BUZ_PAGE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_PAGE, BuzPageClass))


typedef const struct _BuzPage         BuzPageShady;
typedef const struct _BuzPage         BuzPageAnchored;
typedef struct _BuzPage               BuzPage;
typedef struct _BuzPagePrivate        BuzPagePrivate;
typedef struct _BuzPageClass          BuzPageClass;


struct _BuzPage {
	AAltObject parent;
};

struct _BuzPageClass {
	AAltObjectClass parent_class;
	int (*rowCount)(BuzPageShady *page);
	void (*enrichmentRemap)(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index);
	void (*enrich)(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map);
	void (*impoverish)(BuzPageAnchored *page);
};


GType buz_page_get_type();

void buz_page_construct(BuzPage *page);

int buz_page_row_count(BuzPageShady *page);

void buz_page_enrichment_remap(BuzPageShady *page, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index);

void buz_page_enrich(BuzPageAnchored *page, BuzEnrichmentDataMapAnchored *enrichment_map);
void buz_page_impoverish(BuzPageAnchored *page);

GObject *buz_page_get_slot_content_ref(BuzPageAnchored *page, BuzEnrichmentSlot *slot);
void buz_page_set_slot_content(BuzPageAnchored *page, BuzEnrichmentSlot *slot, GObject *content);


A_ALT_HEADERS(BuzPage, buz_page);

G_END_DECLS

#endif /* DOCUMENT_BUZPAGE_H_ */
