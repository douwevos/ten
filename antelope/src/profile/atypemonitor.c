/*
   File:    atypemonitor.c
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

#include "atypemonitor.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef A_REFERENCES_MONITOR

ATypeMonitorEntry *cat_ref_entry_array = NULL;
int cat_ref_entry_count = 0;
static GMutex * volatile cat_ref_entry_mutex = NULL;
static GCond * volatile cat_ref_entry_condition;


ATypeMonitorEntry *a_type_monitor_get_entry(GType type, unsigned int size_of_instance, ATypeMonitorCounterType counter_type) {

	if (cat_ref_entry_mutex==NULL) {
		cat_ref_entry_mutex = g_malloc(sizeof(GMutex));
		g_mutex_init(cat_ref_entry_mutex);

		cat_ref_entry_condition = g_malloc(sizeof(GCond));
		g_cond_init(cat_ref_entry_condition);
	}
	g_mutex_lock(cat_ref_entry_mutex);


	ATypeMonitorEntry *result = NULL;
	if (cat_ref_entry_array==NULL) {
		cat_ref_entry_array = g_malloc(sizeof(ATypeMonitorEntry)*1);
		cat_ref_entry_count = 1;
		result = cat_ref_entry_array;
		result->type = type;
		result->create_count = 0;
		result->destroy_count = 0;
		result->ignore_count = 0;
		result->size_of_instance = size_of_instance;
	} else {
		int idx;
		for(idx=0; idx<cat_ref_entry_count; idx++) {
			ATypeMonitorEntry *test_entry = cat_ref_entry_array+idx;
			if (test_entry->type == type) {
				result = test_entry;
				break;
			}
		}

		if (result==NULL) {
			cat_ref_entry_count++;
			cat_ref_entry_array = g_realloc(cat_ref_entry_array, sizeof(ATypeMonitorEntry)*cat_ref_entry_count);
			result = cat_ref_entry_array+cat_ref_entry_count-1;
			result->type = type;
			result->create_count = 0;
			result->destroy_count = 0;
			result->ignore_count = 0;
			result->size_of_instance = size_of_instance;
		}
	}

	switch(counter_type) {
		case CAT_CT_CREATE : { result->create_count++; } break;
		case CAT_CT_DESTROY : { result->destroy_count++; } break;
		case CAT_CT_IGNORE : { result->ignore_count++; } break;
	}
	g_mutex_unlock(cat_ref_entry_mutex);
	return result;
}


static int cat_ref_entry_sort_cmp(const void *ptr_a, const void *ptr_b) {
	const ATypeMonitorEntry *entry_a = (const ATypeMonitorEntry *) ptr_a;
	const ATypeMonitorEntry *entry_b = (const ATypeMonitorEntry *) ptr_b;
	const char *name_a = g_type_name((entry_a)->type);
	const char *name_b = g_type_name((entry_b)->type);
	return strcmp(name_a, name_b);
}

void a_type_monitor_entry_report() {
	printf("entry-report\n");
	int idx;

	qsort(cat_ref_entry_array, cat_ref_entry_count, sizeof(ATypeMonitorEntry), cat_ref_entry_sort_cmp);

	unsigned long long total_lost = 0;
	unsigned long long total_all = 0;

	for(idx=0; idx<cat_ref_entry_count; idx++) {
		ATypeMonitorEntry *test_entry = cat_ref_entry_array+idx;

		if (test_entry->create_count != test_entry->destroy_count+test_entry->ignore_count) {
			unsigned long long lost = test_entry->create_count - (test_entry->destroy_count + test_entry->ignore_count);
			lost = lost*test_entry->size_of_instance;
			printf("\033[1;31m%d/%d/%d :: %s [%lld]\033[0m\n", test_entry->create_count, test_entry->destroy_count, test_entry->ignore_count, g_type_name(test_entry->type), lost);
			if (strcmp(g_type_name(test_entry->type), "CatFixedStringStatic")!=0) {
				total_lost += lost;
			}
		} else {
			printf("%d/%d/%d :: %s\n", test_entry->create_count, test_entry->destroy_count, test_entry->ignore_count, g_type_name(test_entry->type));
		}
		total_all += test_entry->create_count;
	}
		printf("----------------------------------------------------\n");
		int per = ((total_lost*10000)/total_all);
		if (total_lost==0) {
			printf("\033[1;32mTotal created: %lld\033[0m\n", total_all);
		} else {
			printf("\033[1;31mTotal lost: %lld (%d.%02d) - created: %lld\033[0m\n", total_lost, per/100, per%100, total_all);
		}
}

#endif
