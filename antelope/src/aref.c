/*
   File:    aref.c
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

#include <glib-object.h>
#include "astacktrace.h"
#include "asupport.h"
#include "adate.h"


#ifndef A_REFERENCES_MONITOR

gpointer a_ref(gconstpointer ptr) {
	if (ptr==NULL) {
		return NULL;
	}
	return g_object_ref(ptr);
}

void a_unref(gconstpointer ptr) {
	if (ptr) {
		g_object_unref(ptr);
	}
}

void a_swap_ref_intern(gconstpointer *field, gconstpointer value) {
	gpointer old = *field;
	if (old == value) {
		return;
	}
	*field = value==NULL ? NULL : g_object_ref(value);
	if (old) {
		g_object_unref(old);
	}
}

#endif
