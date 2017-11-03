/*
   File:    cattestcontext.c
   Project: Natpad
   Author:  Douwe Vos
   Date:    Aug 19, 2010
   e-mail:  dmvos2000(at)yahoo.com

   Copyright (C) 2008 Douwe Vos.

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

#include "atestcontext.h"

#include "../src/astring.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../src/aref.h"

void a_test_context_start_sub(ATestContext *context, char *sub_name) {
	context->sub_fail_count = 0;
	context->sub_total_count = 0;
	context->sub_name = sub_name;
}

void a_test_context_stop_sub(ATestContext *context) {
	printf("total-tests:%4d :: failed-tests:%4d >> %s\n", context->sub_total_count, context->sub_fail_count, context->sub_name);
	context->total_fail_count += context->sub_fail_count;
	context->total_count += context->sub_total_count;
}


void a_test_context_test_for_true(ATestContext *context, gboolean val, const char *msg_if_not) {
	context->sub_total_count++;
	if (!val) {
		context->sub_fail_count++;
		if (msg_if_not) {
			printf("%s\n", msg_if_not);
		}
	}
}



void **a_test_context_load_names(ATestContext *context, int *count) {
	char *cwd = getcwd(NULL, 0);
	AString *e_path = a_string_new_with(cwd);
	a_free_ptr(cwd);
	a_string_append_chars(e_path, "/caterpillar/src-ut/etc/names.txt");
	FILE *file = fopen(a_string_chars(e_path), "r" );
	void **result = NULL;
	if (file != NULL) {
		result = g_malloc0(sizeof(AString *)*5000);
		int cn = 0;
		char line[300];
		while(fgets(line, sizeof(line), file)!= NULL) {
			int len = strlen(line);
			result[cn] = a_string_new_with_len(line, len-1);
			cn++;
		}
		fclose(file);
		*count = cn;
	} else {
		fprintf(stderr, "could not open file %s\n", a_string_chars(e_path));
	}
	a_deref(e_path);
	return result;
}



void a_test_context_free_names(ATestContext *context, void **names) {
	AString **name_list = (AString **) names;
	int idx;
	for(idx=0; idx<5000; idx++) {
		a_deref(name_list[idx]);
	}
	a_free_ptr(names);
}
