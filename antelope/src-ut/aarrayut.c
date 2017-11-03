/*
   File:    aarrayut.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 16, 2017
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

#include "aarrayut.h"

#include "../src/collections/aarray.h"
#include "../src/astring.h"

#define A_LOG_LEVEL A_LOG_WARN
#include "../src/asupport.h"
//
//
//static void _test_boolean(ATestContext *context, const char *test, gboolean val_a, gboolean val_b) {
//	if (val_a!=val_b) {
//		context->sub_fail_count++;
//		printf("fail !!\n");
////		cat_log_warn("NO MATCH ! %s", test);
//	}
//	context->sub_total_count++;
//}
//
static AStringAnchored **_get_10_strings() {
	AStringAnchored **strings = (AStringAnchored **) g_malloc(sizeof(AString *)*10);
	strings[0] = a_string_anchor(a_string_new_with("zero"));
	strings[1] = a_string_anchor(a_string_new_with("one"));
	strings[2] = a_string_anchor(a_string_new_with("two"));
	strings[3] = a_string_anchor(a_string_new_with("three"));
	strings[4] = a_string_anchor(a_string_new_with("four"));
	strings[5] = a_string_anchor(a_string_new_with("five"));
	strings[6] = a_string_anchor(a_string_new_with("six"));
	strings[7] = a_string_anchor(a_string_new_with("seven"));
	strings[8] = a_string_anchor(a_string_new_with("eight"));
	strings[9] = a_string_anchor(a_string_new_with("nine"));
	return strings;
}

static void _unget_10_strings(AStringAnchored **strings) {
	a_deref(strings[0]);
	a_deref(strings[1]);
	a_deref(strings[2]);
	a_deref(strings[3]);
	a_deref(strings[4]);
	a_deref(strings[5]);
	a_deref(strings[6]);
	a_deref(strings[7]);
	a_deref(strings[8]);
	a_deref(strings[9]);
	g_free(strings);
}
//
//
//static gboolean _ref_count_strings(CatFixedString **strings, int rc[10]) {
//	int idx;
//	for(idx=0; idx<9; idx++) {
//		if (G_OBJECT(strings[idx])->ref_count!=rc[idx]) {
////			cat_log_warn("G_OBJECT(strings[%d])->ref_count=%d  != rc[%d]=%d", idx, G_OBJECT(strings[idx])->ref_count, idx, rc[idx]);
//			return FALSE;
//		}
//	}
//	return TRUE;
//}
//
static void _test_append(ATestContext *context) {
	AStringAnchored **strings = _get_10_strings();
	AArray *e_array = a_array_new();
	a_array_add(e_array, strings[2]);
	a_array_add(e_array, strings[2]);
	a_array_add(e_array, strings[4]);
	a_array_add(e_array, strings[6]);
	a_array_add(e_array, strings[2]);

	a_array_remove_range(e_array, 2, 1);

	AArrayAnchored *a_array = a_array_anchor(e_array);

	e_array = a_array_mutable(a_array);
	a_deref(a_array);
	a_array_remove_range(e_array, 2, 1);
//	a_array_insert(e_array, G_OBJECT(strings[3]), 3);
	a_array_add(e_array, G_OBJECT(strings[3]));
	a_array = a_array_anchor(e_array);
	a_deref(a_array);

	_unget_10_strings(strings);
}
//
//static void _test_append_2(ATestContext *context) {
//	CatFixedString **strings = _get_10_strings();
//	CatFixedArray *fixed_array = cat_fixed_array_new();
//	fixed_array = cat_fixed_array_append(fixed_array, G_OBJECT(strings[2]));
//	fixed_array = cat_fixed_array_append(fixed_array, G_OBJECT(strings[2]));
//	CatFixedArray *snapshot = cat_ref_sink_ptr(fixed_array);
//	fixed_array = cat_fixed_array_append(fixed_array, G_OBJECT(strings[4]));
//	fixed_array = cat_fixed_array_append(fixed_array, G_OBJECT(strings[6]));
//	fixed_array = cat_fixed_array_append(fixed_array, G_OBJECT(strings[2]));
//	cat_ref_sink_ptr(fixed_array);
//	int refs[] = {1,1,6,1,2,1,2,1,1,1};
//	_test_boolean(context, "append-ref-count", _ref_count_strings(strings, refs), TRUE);
//	a_deref(fixed_array);
//
//	int refsnap[] = {1,1,3,1,1,1,1,1,1,1};
//	_test_boolean(context, "append-ref-count-snap", _ref_count_strings(strings, refsnap), TRUE);
//
//	a_deref(snapshot);
//
//	int unrefs[] = {1,1,1,1,1,1,1,1,1,1};
//	_test_boolean(context, "append-unref-count", _ref_count_strings(strings, unrefs), TRUE);
//	_unget_10_strings(strings);
//}
//
//
//
void test_array(ATestContext *context) {
	a_test_context_start_sub(context, __FILE__);
	_test_append(context);
//	_test_append_2(context);
	a_test_context_stop_sub(context);
}
