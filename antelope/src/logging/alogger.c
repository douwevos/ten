/*
   File:    alogger.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 9, 2017
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

#include "alogger.h"
#include <stdio.h>
#include "../astring.h"
#include "../aref.h"
#include "../adate.h"

static int64_t a_log_start_time;

__thread gboolean cat_log_block = FALSE;

void a_logger_print(const char *level, const char *file_or_class, int line_nr, const char *text, ...) {
	AString *buf = a_string_new();

	if (a_log_start_time==0) {
		a_log_start_time = a_timestamp();
	}
	gint64 diftime = a_timestamp() - a_log_start_time;

	a_string_format(buf, "%ld %s %s(%d): ", diftime, level, file_or_class, line_nr);
	va_list args;
	va_start(args, text);
	a_string_vformat(buf, text, args);
	va_end(args);
	printf("%s", a_string_chars(buf));
	a_unref(buf);
}
