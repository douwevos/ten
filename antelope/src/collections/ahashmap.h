/*
   File:    ahashmap.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 14, 2017
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

#ifndef SRC_COLLECTIONS_AHASHMAP_H_
#define SRC_COLLECTIONS_AHASHMAP_H_

#include "../aaltobject.h"
#include "amapiterator.h"

G_BEGIN_DECLS

#define A_TYPE_HASH_MAP              (a_hash_map_get_type())
#define A_HASH_MAP(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_hash_map_get_type(), AHashMap))
#define A_HASH_MAP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_HASH_MAP, AHashMapClass))
#define A_IS_HASH_MAP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_HASH_MAP))
#define A_IS_HASH_MAP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_HASH_MAP))
#define A_HASH_MAP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_HASH_MAP, AHashMapClass))


typedef const struct _AHashMap         AHashMapAnchored;
typedef const struct _AHashMap         AHashMapShady;
typedef struct _AHashMap               AHashMap;
typedef struct _AHashMapClass          AHashMapClass;


struct _AHashMap {
	AAltObject parent;
};

struct _AHashMapClass {
	AAltObjectClass parent_class;
};


GType a_hash_map_get_type();

AHashMap *a_hash_map_new(GHashFunc hash_func, GEqualFunc equal_func);

gboolean a_hash_map_put(AHashMap *map, gconstpointer key, gconstpointer value);
gconstpointer a_hash_map_get(AHashMapShady *map, gconstpointer key);

gboolean a_hash_map_equal(AHashMapShady *map_a, AHashMapShady *map_b, GEqualFunc equal_func_for_value);

G_END_DECLS

#endif /* SRC_COLLECTIONS_AHASHMAP_H_ */
