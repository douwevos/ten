/*
   File:    cattestcontext.c
   Project: Caterpillar
   Author:  Douwe Vos
   Date:    Aug 19, 2010
   e-mail:  dmvos2000(at)yahoo.com

   Copyright (C) 2010 Douwe Vos.

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


#ifndef A_TEST_CONTEXT_H_
#define A_TEST_CONTEXT_H_

#include <glib-object.h>

struct _ATestContext {
	int sub_total_count;
	int sub_fail_count;
	char *sub_name;
	int total_count;
	int total_fail_count;
};

typedef struct _ATestContext ATestContext;


void a_test_context_start_sub(ATestContext *context, char *sub_name);
void a_test_context_stop_sub(ATestContext *context);


void a_test_context_test_for_true(ATestContext *context, gboolean val, const char *msg_if_not);

void **a_test_context_load_names(ATestContext *context, int *count);

void a_test_context_free_names(ATestContext *context, void **names);

#endif
