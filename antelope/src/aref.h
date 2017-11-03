/*
   File:    aref.h
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

#include "profile/amonitor.h"
#include <glib-object.h>

#ifndef SRC_AREF_H_
#define SRC_AREF_H_

#define a_free_ptr(ptr) { gpointer val = (gpointer) ptr; ptr = NULL; if (val) { g_free(val); } }

#ifndef A_REFERENCES_MONITOR

gpointer a_ref(gconstpointer ptr);
#define a_ref_fake(ptr) ptr
void a_unref(gconstpointer ptr);
void a_swap_ref_intern(gconstpointer *field, gconstpointer value);
#define a_swap_ref(field, value) a_swap_ref_intern((gconstpointer *) (&field), value);
#define a_announce(value)

#else

#define a_ref(ptr) a_monitor_ref(ptr)
#define a_ref_fake(ptr) ptr
#define a_unref(ptr) a_monitor_unref(ptr)
#define a_swap_ref(field, value) a_monitor_swap_ref((gconstpointer *) (&field), value)
#define a_announce(value) a_monitor_ref_created(value)


#endif

#define a_deref(obj) { gpointer val = (gpointer) obj; obj=NULL; a_unref(val); }

#endif /* SRC_AREF_H_ */
