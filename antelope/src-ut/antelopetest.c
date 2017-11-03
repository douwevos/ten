/*
   File:    antelopetest.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 8, 2017
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

#include "../src/collections/aarray.h"
#include "../src/astring.h"
#include "../src/profile/amonitor.h"
#include "aarrayut.h"
//#include "abitarrayut.h"
#include "ahashmaput.h"
#include "alockut.h"
#include "astringut.h"
#include "atestcontext.h"
#include <stdio.h>

#define A_LOG_LEVEL A_LOG_ALL
#include "../src/asupport.h"

static void
log_handler (const gchar   *log_domain,
             GLogLevelFlags log_level,
             const gchar   *message,
             gpointer       user_data)
{
	a_stacktrace_print();
  g_log_default_handler (log_domain, log_level, message, user_data);

  g_on_error_query ("antelope");
}


void dump_a(gpointer data, gpointer *user_data) {
	a_log_debug("item:%o", data);
}

int main(int argc, char **argv) {
	g_log_set_handler("GLib-GObject", G_LOG_LEVEL_CRITICAL, (GLogFunc) log_handler, NULL);
	g_log_set_handler("GLib-GObject", G_LOG_LEVEL_WARNING, (GLogFunc) log_handler, NULL);
	g_log_set_handler (NULL, G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL,
	                     log_handler, NULL);
	printf("starting antelope-test\n");

//	GObject *obj = a_array_new();
//
//
//
//	g_object_ref(array);
//
//	AArrayAnchored *acnhored = a_array_anchor(array);
//
//	AArrayAnchored *anchored = a_array_anchor(acnhored);
//
//
//	a_array_add(array, obj);
//
//
//	AArray *m_array = a_array_mutable(anchored);
//
	AString *text = a_string_new();
	a_string_append_chars_len(text, "hello", 5);
//	const char *t = a_string_chars(text);
//
//	printf("t=%s\n", t);
//
//	a_string_append_chars(text, " douwe", 6);
//	printf("t=%s\n", a_string_chars(text));
//
//
//	g_object_ref(text);
	AStringAnchored *tt = a_string_anchor(text);
//	a_unref(text);
//	printf("t=%s\n", a_string_chars(tt));
//
//	a_string_append_chars(text, " douwe", 6);
//	printf("t=%s\n", a_string_chars(text));
//
//	printf("t=%s\n", a_string_chars(tt));

	printf("creating mutable:tt=%p\n", tt);
	AString *t_mut = a_string_mutable(tt);
	printf("created mutable:%p\n", t_mut);


	a_string_append_chars_len(t_mut, "abc", 3);

	a_string_remove_range(t_mut, a_string_length(t_mut)-3, 3);

	printf("anchoring\n");

	AStringAnchored *t_anc = a_string_anchor(t_mut);

//	a_log_debug("nneeee :: %s", t_mut);



	if (t_anc==tt) {
		printf("same\n");
	} else {
		printf("no same\n");
	}

	AString *buf = a_string_new();
//	a_string_append_char(buf, 'h');
	a_string_format(buf, "test %p", argv);
	printf("wat: %s\n", a_string_chars(buf));

	a_log_debug("test:");

	AArray *array = a_array_new();
	a_array_add(array, buf);
	a_array_add(array, t_anc);

	a_array_foreach(array, dump_a, NULL);

	printf("tt=%p\n", tt);

	a_unref(tt);
	a_unref(t_anc);
	a_unref(buf);


	AIterator *iter = a_array_iterator(array);
	gpointer objptr = NULL;
	while(a_iterator_next(iter, &objptr)) {
		a_log_debug("mmmmm :%p", objptr);
	}
	a_unref(iter);
	a_unref(array);


	printf("starting caterpillar-test\n");
	ATestContext context;
	test_string(&context);
//	test_regexp(&context);
//	test_bit_array(&context);
	test_array(&context);
	test_lock(&context);



	a_type_monitor_entry_report();
	a_monitor_report();

	return 0;
}
