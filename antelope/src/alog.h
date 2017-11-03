/*
   File:    alog.h
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

#include "logging/alogger.h"
#include <stdio.h>

#define A_LOG_ALL     0
#define A_LOG_TRACE   10
#define A_LOG_DETAIL  20
#define A_LOG_DEBUG   30
#define A_LOG_VERBOSE 40
#define A_LOG_INFO    50
#define A_LOG_WARN    60
#define A_LOG_ERROR   70
#define A_LOG_FATAL   80

#ifdef A_LOG_CLASS
#define a_log_print(level, format, args...) \
{ \
	a_logger_print(level, A_LOG_CLASS, __LINE__, format "\n", ##args); \
}
#else
#define a_log_print(level, format, args...) \
{ \
	a_logger_print(level, __FILE__, __LINE__, format "\n", ##args); \
}
#endif


#ifndef A_LOG_LEVEL
#define A_LOG_LEVEL A_LOG_ALL
#endif


#if A_LOG_LEVEL<=A_LOG_TRACE
#define a_log_trace(format, args...) a_log_print("TRACE", format, ##args)
#define a_log_on_trace(code) code
#else
#define a_log_trace(format, args...)
#define a_log_on_trace(code)
#endif

#if A_LOG_LEVEL<=A_LOG_DETAIL
#define a_log_detail(format, args...) a_log_print("DTAIL", format, ##args)
#define a_log_on_detail(code) code
#else
#define a_log_detail(format, args...)
#define a_log_on_detail(code)
#endif


#if A_LOG_LEVEL<=A_LOG_DEBUG
#define a_log_debug(format, args...) a_log_print("DEBUG", format, ##args)
#define a_log_on_debug(code) code
#else
#define a_log_debug(format, args...)
#define a_log_on_debug(code)
#endif

#if A_LOG_LEVEL<=A_LOG_INFO
#define a_log_info(format, args...) a_log_print("INFO", format, ##args)
#define a_log_on_info(code) code
#else
#define a_log_info(format, args...)
#define a_log_on_info(code)
#endif

#if A_LOG_LEVEL<=A_LOG_WARN
#define a_log_warn(format, args...) a_log_print("WARN", format, ##args)
#define a_log_on_warn(code) code
#else
#define a_log_warn(format, args...)
#define a_log_on_warn(code)
#endif


#if A_LOG_LEVEL<=A_LOG_ERROR
#define a_log_error(format, args...) a_log_print("ERROR", format, ##args)
#define a_log_on_error(code) code
#else
#define a_log_error(format, args...)
#define a_log_on_error(code)
#endif

#if A_LOG_LEVEL<=A_LOG_FATAL
#define a_log_fatal(format, args...) a_log_print("FATAL", format, ##args)
#define a_log_on_fatal(code) code
#else
#define a_log_fatal(format, args...)
#define a_log_on_fatal(code)
#endif
