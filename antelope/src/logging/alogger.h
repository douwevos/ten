/*
   File:    alogger.h
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

#ifndef SRC_LOGGING_ALOGGER_H_
#define SRC_LOGGING_ALOGGER_H_

#include <glib-object.h>

void a_logger_print(const char *level, const char *file_or_class, int line_nr, const char *text, ...);

#endif /* SRC_LOGGING_ALOGGER_H_ */
