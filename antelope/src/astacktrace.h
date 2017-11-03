/*
   File:    astacktrace.h
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

#ifndef SRC_ASTACKTRACE_H_
#define SRC_ASTACKTRACE_H_

#define _GNU_SOURCE
#define __USE_GNU

#include <glib-object.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <bfd.h>
#include <dlfcn.h>
#include <link.h>

typedef struct _AStacktracer AStacktracer;
typedef struct _AStackBfd    AStackBfd;


struct _AStackBfd {
	struct _AStackBfd *next;
	char *of_file_name;
	bfd *abfd;
	asymbol **symbol_table;
	long symbol_count;
};


struct _AStacktracer {
	AStackBfd *first_stack_bfd;
};



typedef struct _AStacktraceEntry AStacktraceEntry;
typedef struct _AStacktrace      AStacktrace;


struct _AStacktraceEntry {
	void *trace_address;

	char *of_file_name;
	void *of_base_address;

	char *src_file_name;
	char *src_function;
	unsigned long src_line_nr;
};

struct _AStacktrace {
	AStacktraceEntry *entry_array;
	int entry_count;
};



typedef struct _AStackBfdAndEntry AStackBfdAndEntry;

struct _AStackBfdAndEntry {
	AStackBfd *stack_bfd;
	AStacktraceEntry *trace_entry;
};



AStacktracer *a_stacktracer_instance();

AStacktrace *a_stacktracer_create_trace(AStacktracer *tracer, int start, int length);

void a_stacktrace_free(AStacktrace *trace);

void a_stacktrace_dump(AStacktrace *trace);

gboolean a_stacktrace_filter(AStacktrace *trace, const char *flt_filename, const char *flt_function, gboolean first_only);

void a_stacktrace_print();

#endif /* SRC_ASTACKTRACE_H_ */
