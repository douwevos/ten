/*
   File:    astringut.c
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

#include "astringut.h"
#include "../src/astring.h"
#include "../src/collections/ahashmap.h"

#define A_LOG_LEVEL A_LOG_WARN
#include "../src/asupport.h"

AStringConstant test_a_string = a_string_constant_declare("Test me");

static void _test_simple(ATestContext *context) {
	AStringAnchored *a_test_static = a_string_constant_get(test_a_string);
	AString *a_test_local = a_string_new_with("Test me");
	a_test_context_test_for_true(context, a_string_equal(a_test_local, a_test_static), "equal-test");
	a_deref(a_test_local);
	a_test_local = a_string_new_with("Wrong");
	a_test_context_test_for_true(context, !a_string_equal(a_test_local, a_test_static), "equal-test-inv");
	a_deref(a_test_local);

	AStringAnchored *a_test_const = a_string_fast("Test me");
	a_test_context_test_for_true(context, a_string_equal(a_test_const, a_test_static), "equal-test");
	a_deref(a_test_const);

}

static void _test_format(ATestContext *context) {
	AString *a_test_local = a_string_new_with("Test me");
	AHashMap *dummy_map = a_hash_map_new(NULL, NULL);
	AString *e_formated = a_string_new();
	a_string_format(e_formated, "test it, context=%o, map=%o, say-1000=%d, pointer=%p", a_test_local, dummy_map, 1000, dummy_map);


	AString *e_manual = a_string_new_with("test it, context=");
	a_string_append_string(e_manual, a_test_local);
	a_string_append_chars(e_manual, ", map=");
	A_OBJECT_GET_CLASS(dummy_map)->asString((AObject*) dummy_map, e_manual);
	a_string_append_chars(e_manual, ", say-1000=");
	a_string_append_decimal(e_manual, 1000);
	a_string_append_chars(e_manual, ", pointer=");
	a_string_append_chars_len(e_manual, "0x", 3);
	a_string_append_hexadecimal(e_manual, (unsigned long long int) dummy_map, -1, FALSE);

	a_test_context_test_for_true(context, a_string_equal(e_manual, e_formated), "formatting");

	a_deref(e_manual);
	a_deref(e_formated);
	a_deref(a_test_local);
	a_deref(dummy_map);
}


void test_string(ATestContext *context) {
	a_test_context_start_sub(context, __FILE__);
	_test_simple(context);
	_test_format(context);
	a_test_context_stop_sub(context);
}
