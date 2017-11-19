/*
   File:    aobject.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 10, 2017
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

#include "aobject.h"
#include "astring.h"
#include "profile/amonitor.h"

#define A_LOG_LEVEL A_LOG_ALL
#define A_LOG_CLASS "AObject"
#include "asupport.h"

G_DEFINE_TYPE(AObject, a_object, G_TYPE_OBJECT);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static void l_as_string(const AObject *object, struct _AString *out);

static void a_object_class_init(AObjectClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	clazz->asString = l_as_string;
}

static void a_object_init(AObject *instance) {
}

static void l_dispose(GObject *object) {
//	a_log_detail("dispose:%p", object);
//	AObject *instance = A_OBJECT(object);
//	AObjectPrivate *priv = a_object_get_instance_private(instance);
	G_OBJECT_CLASS(a_object_parent_class)->dispose(object);
//	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
//	a_log_detail("finalize:%p", object);
#ifdef A_REFERENCES_MONITORED
	a_monitor_ref_destroyed(object);
#endif
	G_OBJECT_CLASS(a_object_parent_class)->finalize(object);
//	a_log_detail("finalized:%p", object);
}


void a_object_construct(AObject *object) {
#ifdef A_REFERENCES_MONITORED
	a_monitor_ref_created(object);
#endif
}


static void l_as_string(const AObject *object, struct _AString *out) {
	const char *iname = g_type_name_from_instance((GTypeInstance *) object);
	a_string_format(out, "%s[%p]", iname, object);
}


gboolean a_object_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a==object_b) {
		return TRUE;
	}
	if (object_a==NULL || object_b==NULL) {
		return TRUE;
	}
	return A_OBJECT_GET_CLASS(object_a)->equal(object_a, object_b);
}


