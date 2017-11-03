/*
   File:    buzenrichmentdatamap.h
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 25, 2017
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

#ifndef DOCUMENT_BUZENRICHMENTDATAMAP_H_
#define DOCUMENT_BUZENRICHMENTDATAMAP_H_

#include <antelope.h>

G_BEGIN_DECLS

#define BUZ_TYPE_ENRICHMENT_DATA_MAP              (buz_enrichment_data_map_get_type())
#define BUZ_ENRICHMENT_DATA_MAP(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), buz_enrichment_data_map_get_type(), BuzEnrichmentDataMap))
#define BUZ_ENRICHMENT_DATA_MAP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), BUZ_TYPE_ENRICHMENT_DATA_MAP, BuzEnrichmentDataMapClass))
#define BUZ_IS_ENRICHMENT_DATA_MAP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUZ_TYPE_ENRICHMENT_DATA_MAP))
#define BUZ_IS_ENRICHMENT_DATA_MAP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), BUZ_TYPE_ENRICHMENT_DATA_MAP))
#define BUZ_ENRICHMENT_DATA_MAP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), BUZ_TYPE_ENRICHMENT_DATA_MAP, BuzEnrichmentDataMapClass))


typedef const struct _BuzEnrichmentDataMap         BuzEnrichmentDataMapAnchored;
typedef const struct _BuzEnrichmentDataMap         BuzEnrichmentDataMapShady;
typedef struct _BuzEnrichmentDataMap               BuzEnrichmentDataMap;
typedef struct _BuzEnrichmentDataMapPrivate        BuzEnrichmentDataMapPrivate;
typedef struct _BuzEnrichmentDataMapClass          BuzEnrichmentDataMapClass;


struct _BuzEnrichmentDataMap {
	AAltObject parent;
};

struct _BuzEnrichmentDataMapClass {
	AAltObjectClass parent_class;
};


GType buz_enrichment_data_map_get_type();

BuzEnrichmentDataMap *buz_enrichment_data_map_new();

AArrayAnchored *buz_enrichment_data_map_get_default_nullified_data(BuzEnrichmentDataMapAnchored *enrichment_map);

int buz_enrichment_data_map_count(BuzEnrichmentDataMapShady *enrichment_map);
int buz_enrichment_data_map_find_slot_index(BuzEnrichmentDataMapShady *enrichment_map, AStringAnchored *slot_key, int guess_index);

int buz_enrichment_data_map_add_slot(BuzEnrichmentDataMap *enrichment_map, AStringAnchored *slot_key);
int buz_enrichment_data_map_remove_slot(BuzEnrichmentDataMap *enrichment_map, AStringAnchored *slot_key);

A_ALT_HEADERS(BuzEnrichmentDataMap, buz_enrichment_data_map)

G_END_DECLS

#endif /* DOCUMENT_BUZENRICHMENTDATAMAP_H_ */
