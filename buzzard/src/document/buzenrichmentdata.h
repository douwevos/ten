/*
   File:    buzenrichmentdata.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 26, 2017
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

#ifndef DOCUMENT_BUZENRICHMENTDATA_H_
#define DOCUMENT_BUZENRICHMENTDATA_H_

#include "buzenrichmentdatamap.h"
#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_ENRICHMENT_DATA              (buz_enrichment_data_get_type())
#define BUZ_ENRICHMENT_DATA(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_enrichment_data_get_type(), BuzEnrichmentData))
#define BUZ_ENRICHMENT_DATA_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_ENRICHMENT_DATA, BuzEnrichmentDataClass))
#define BUZ_IS_ENRICHMENT_DATA(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_ENRICHMENT_DATA))
#define BUZ_IS_ENRICHMENT_DATA_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_ENRICHMENT_DATA))
#define BUZ_ENRICHMENT_DATA_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_ENRICHMENT_DATA, BuzEnrichmentDataClass))


typedef struct _BuzEnrichmentData               BuzEnrichmentData;
typedef struct _BuzEnrichmentDataPrivate        BuzEnrichmentDataPrivate;
typedef struct _BuzEnrichmentDataClass          BuzEnrichmentDataClass;


struct _BuzEnrichmentData {
	AObject parent;
};

struct _BuzEnrichmentDataClass {
	AObjectClass parent_class;
};


GType buz_enrichment_data_get_type();

BuzEnrichmentData *buz_enrichment_data_new(BuzEnrichmentDataMapAnchored *a_enrichment_map);
BuzEnrichmentData *buz_enrichment_data_new_lock(BuzEnrichmentDataMapAnchored *a_enrichment_map, ALock *lock);

ALock *buz_enrichment_data_get_lock(BuzEnrichmentData *enrichment_data);

void buz_enrichment_data_remap(BuzEnrichmentData *enrichment_data, BuzEnrichmentDataMapAnchored *a_old_map, BuzEnrichmentDataMap *a_new_map, int add_idx, int rem_idx);

BuzEnrichmentDataMapAnchored *buz_enrichment_data_get_map(const BuzEnrichmentData *enrichment_data);

GObject *buz_enrichment_data_get_slot_content_ref(const BuzEnrichmentData *enrichment_data, int slot_index, AStringAnchored *slot_key);
void buz_enrichment_data_set_slot_content(const BuzEnrichmentData *enrichment_data, int slot_index, AStringAnchored *slot_key, GObject *content);

G_END_DECLS

#endif /* DOCUMENT_BUZENRICHMENTDATA_H_ */
