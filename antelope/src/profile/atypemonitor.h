/*
   File:    atypemonitor.h
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

#ifndef SRC_PROFILE_ATYPEMONITOR_H_
#define SRC_PROFILE_ATYPEMONITOR_H_

#ifdef A_REFERENCES_MONITOR

#include <glib-object.h>

typedef struct _ATypeMonitorEntry ATypeMonitorEntry;

struct _ATypeMonitorEntry {
	GType type;
	int create_count;
	int destroy_count;
	int ignore_count;
	unsigned int size_of_instance;
};

typedef enum _ATypeMonitorCounterType ATypeMonitorCounterType;

enum _ATypeMonitorCounterType {
	CAT_CT_CREATE,
	CAT_CT_DESTROY,
	CAT_CT_IGNORE
};

ATypeMonitorEntry *a_type_monitor_get_entry(GType type, unsigned int size_of_instance, ATypeMonitorCounterType counter_type);

void a_type_monitor_entry_report();

#else

#define a_type_monitor_entry_report()

#endif

#endif /* SRC_PROFILE_ATYPEMONITOR_H_ */
