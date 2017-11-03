/*
   File:    amonitor.c
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

#include "amonitor.h"
#include "../astacktrace.h"
#include "../adate.h"
#include "../astring.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef A_REFERENCES_MONITOR

static GMutex * volatile mutex = NULL;
static GCond * volatile condition;

static GList *monitor_list = NULL;
static gboolean skip_type_monitor = FALSE;

static __thread gboolean block = FALSE;

static const char *monitor_by_type_name;

enum _MonitorRefAction {
	A_MRA_INCREASE,
	A_MRA_DECREASE,
	A_MRA_NO_CHANGE,
	A_MRA_EOL,
	A_MRA_EOL_NOTIFY,
	A_MRA_MONITOR_START
};

typedef enum _MonitorRefAction MonitorRefAction;

struct _MonitorInfo {
	gconstpointer obj;
	AString *stringified;
	const char *type_name;
	int ref_count;
	MonitorRefAction action;
#ifdef A_STACKTRACE_ENABLED
    AStacktrace *trace;
#endif
    int64_t time_of_occurrence;
    void *thread_pointer;
};

typedef struct _MonitorInfo MonitorInfo;


static void cat_ref_report_entry(GPtrArray *collected, AString *stringified, gboolean do_print) {
	int idx = 0;
	int pseudo_ref_count = 0;

	while(idx<collected->len) {
		MonitorInfo *main_info = (MonitorInfo *) collected->pdata[idx];
		if (idx==0) {
			if (do_print) {
				if (stringified) {
					printf("monitor-report for %p %s\n", main_info->obj, a_string_chars(stringified));
				} else {
					printf("monitor-report for %p\n", main_info->obj);
				}
			}
			pseudo_ref_count = 1;
		} else {
			switch(main_info->action) {
				case A_MRA_DECREASE : {
					pseudo_ref_count--;
				} break;
				case A_MRA_INCREASE : {
					pseudo_ref_count++;
				} break;
				default :
					break;
			}
		}
		if (do_print) {
			const char *description = NULL;
			switch(main_info->action) {
				case A_MRA_INCREASE : description = "INCREASE"; break;
				case A_MRA_DECREASE : description = "DECREASE"; break;
				case A_MRA_EOL : description = "END-OF-LIFE"; break;
				case A_MRA_EOL_NOTIFY : description = "END-OF-LIFE NOTIFY"; break;
				case A_MRA_MONITOR_START : description = "MONITOR-START"; break;
				case A_MRA_NO_CHANGE : description = "NO-CHANGE"; break;
				default : description = "<nil>"; break;
			}
			printf("[%d] :: time=%ld ref-count(sampled:%d calculated=%d) thread:%p %s\n", idx, main_info->time_of_occurrence, main_info->ref_count, pseudo_ref_count, main_info->thread_pointer, description);
#ifdef A_STACKTRACE_ENABLED
			a_stacktrace_dump(main_info->trace);
#endif
			printf("\n");
		}

#ifdef A_STACKTRACE_ENABLED
		if (main_info->trace) {
			a_stacktrace_free(main_info->trace);
			main_info->trace = NULL;
		}
#endif
		if (main_info->stringified) {
			g_object_unref(main_info->stringified);
			main_info->stringified = NULL;
		}
		g_free(main_info);
		idx++;
	}
	if (idx>0 && do_print) {
		printf("----------------------------------------------------\n");
	}
}

static void l_ref_report_impl(void *obj2report) {
	GPtrArray *collected = g_ptr_array_new();
	GList *iter;
	gboolean keep_running = TRUE;
	gboolean uncleared_object;
	while(keep_running) {
		iter = monitor_list;
		uncleared_object = TRUE;
		if (iter) {
			g_ptr_array_set_size(collected, 0);
			MonitorInfo *main_info = (MonitorInfo *) iter->data;

			AString *stringified = main_info->stringified;
			g_ptr_array_add(collected, main_info);
			monitor_list = g_list_remove_link(monitor_list, iter);
			if (main_info->ref_count!=0) {
				iter = monitor_list;
				gboolean is_trailing = FALSE;
				int last_ref_count = main_info->ref_count;
				while(iter) {
					MonitorInfo *iter_info = (MonitorInfo *) iter->data;
					if (iter_info->obj == main_info->obj) {
						GList *iter_2_del = iter;
						iter = iter->next;
						if (iter_info->action == A_MRA_MONITOR_START && ((last_ref_count!=iter_info->ref_count) || (is_trailing))) {
							break;
						}
						if (is_trailing && iter_info->ref_count>0) {
							break;
						}
						last_ref_count = iter_info->ref_count;

						monitor_list = g_list_remove_link(monitor_list, iter_2_del);
						g_ptr_array_add(collected, iter_info);
						if (iter_info->stringified) {
							stringified = iter_info->stringified;
						}

						if ((iter_info->ref_count==0) || (iter_info->action==A_MRA_EOL) || (iter_info->action==A_MRA_EOL_NOTIFY)) {
							is_trailing = TRUE;
							uncleared_object = FALSE;
						}
					} else {
						iter = iter->next;
					}
				}
			} else {
				uncleared_object = FALSE;
			}


			if (obj2report) {
				if (obj2report==main_info->obj) {
					cat_ref_report_entry(collected, stringified, TRUE);
				}
			} else {
//			clean_obj = FALSE;
#ifdef MONITOR_CLAZZ_INVERT
				clean_obj = !clean_obj;
#endif
				cat_ref_report_entry(collected, stringified, uncleared_object);
			}
		} else {
			keep_running = FALSE;
		}
	}
}

void a_monitor_report() {
	skip_type_monitor = TRUE;
	printf("monitor-report\n");
	l_ref_report_impl(NULL);
}



static int l_ref_cnt(gconstpointer obj) {
	if (obj) {
		if (G_IS_OBJECT(obj)) {
			return G_OBJECT(obj)->ref_count;
		} else {
			return -1000;
		}
	}
	return -1;
}

static void l_monitor(gconstpointer obj, MonitorRefAction action) {
	if (obj==NULL || skip_type_monitor) {
		return;
	}

	const char *type_name = g_type_name_from_instance((GTypeInstance *) obj);

	if (monitor_by_type_name==NULL || (strcmp(type_name, monitor_by_type_name)!=0)) {
		if (g_object_get_data((GObject *) obj, "a_ref_monitor")==NULL) {
			return;

		}
	}


	if (mutex==NULL) {
		mutex = g_malloc(sizeof(GMutex));
		g_mutex_init(mutex);
		condition = g_malloc(sizeof(GCond));
		g_cond_init(condition);
	}
	g_mutex_lock(mutex);

//			printf("check i :: %d %d\n ", (int) mon_type , g_type_check_instance_is_a((GTypeInstance *) obj, mon_type));
//			printf("%s >> %s[%d] %s(%p, ref-count=%d)\n", stxt, clazz_or_filename, line_nr, MONITOR_CLAZZ, obj, _ref_cnt(obj));
	MonitorInfo *mon_info = g_new(MonitorInfo, 1);
	mon_info->thread_pointer = g_thread_self();
	mon_info->time_of_occurrence = a_timestamp();
	mon_info->obj = obj;
	mon_info->type_name = type_name;
	mon_info->stringified = NULL;
	GObject *object = G_OBJECT(obj);
	int ref_count = object->ref_count;
	switch(action) {
		case A_MRA_INCREASE : ref_count++; break;
		case A_MRA_EOL_NOTIFY :
//			mon_info->stringified = a_string_new();
//			a_string_format(mon_info->stringified, "%O", object);
			ref_count--; break;
		case A_MRA_DECREASE : ref_count--; break;
		case A_MRA_MONITOR_START :
		case A_MRA_EOL :
			if (ref_count>0) {
				gboolean old_block = block;
				block = TRUE;
				mon_info->stringified = a_string_new();
				a_string_format(mon_info->stringified, "%o", object);
				block = old_block;
			}
			break;
		default : break;
	}

	mon_info->ref_count = ref_count;
	mon_info->action = action;

	monitor_list = g_list_append(monitor_list, mon_info);

#ifdef A_STACKTRACE_ENABLED
	mon_info->trace = a_stacktracer_create_trace(NULL, -1, -1);
	a_stacktrace_filter(mon_info->trace, "amonitor.c", NULL, TRUE);
	a_stacktrace_filter(mon_info->trace, "amonitor.c", NULL, TRUE);
//	a_stacktrace_filter(mon_info->trace, "aobject.c", NULL, TRUE);
//	a_stacktrace_filter(mon_info->trace, "aaltobject.c", NULL, TRUE);
	a_stacktrace_filter(mon_info->trace, NULL, "_start", TRUE);
	a_stacktrace_filter(mon_info->trace, "libc-start.c", NULL, TRUE);
#endif
	g_mutex_unlock(mutex);

	int rc = l_ref_cnt(obj);
	if ((rc%5000)==1000) {
		printf("%s(%p, ref-count=%d) TO HIGH!\n", type_name, obj, l_ref_cnt(obj));
	}
}


gpointer a_monitor_ref(gconstpointer obj) {
	if (obj) {
		if (block) {
			return g_object_ref((gpointer) obj);
		}
		if (G_IS_OBJECT(obj)) {
			l_monitor(obj, A_MRA_INCREASE);
			g_object_ref((gpointer) obj);
		} else {
			printf("can not ref, not an object obj=%p\n", obj);
			a_stacktrace_print();
		}
	}
	return (gpointer) obj;
}

void a_monitor_unref(gconstpointer obj) {
	if (obj) {
		gpointer in_obj = (gpointer) obj;
		if (block) {
			g_object_unref((gpointer) obj);
			return;
		}
		if (G_IS_OBJECT(in_obj)) {
			l_monitor(obj, A_MRA_DECREASE);
			g_object_unref(in_obj);
		} else {
			printf("WARN: ptr:%p can not unref, not an object\n", obj);
//			cat_ref_report_impl(obj);
#ifdef A_STACKTRACE_ENABLED
			a_stacktrace_print();
#endif
			printf("WARN: instance-name=%s\n", g_type_name_from_instance((GTypeInstance *) obj));
		}
	}
}

void a_monitor_swap_ref(gconstpointer *field, gconstpointer value) {
	gconstpointer oldvalue = *field;
	if (oldvalue==value) {
		return;
	}
	if (block) {
		g_object_ref((gpointer) value);
		*field = value;
		g_object_unref((gpointer) oldvalue);
	} else {
		if (G_IS_OBJECT(value)) {
			l_monitor(value, A_MRA_INCREASE);
			g_object_ref((gpointer) value);
		}
		*field = value;
		if (G_IS_OBJECT(oldvalue)) {
			l_monitor(oldvalue, A_MRA_DECREASE);
			g_object_unref((gpointer) oldvalue);
		}
	}
}


static void l_week_notify(gpointer data, GObject *ptr_of_obj) {
	GType tp = (GType) data;
	if (!skip_type_monitor) {
		a_type_monitor_get_entry(tp, sizeof(ptr_of_obj), CAT_CT_DESTROY);
	}

	if (ptr_of_obj) {
		l_monitor(ptr_of_obj, A_MRA_EOL_NOTIFY);
	}
}

void a_monitor_ref_created(gconstpointer obj) {
	if (block==TRUE) {
		return;
	}
	if (obj) {
		l_monitor(obj, A_MRA_MONITOR_START);
		GObject *gobj = G_OBJECT(obj);

		if (!skip_type_monitor) {
			a_type_monitor_get_entry(gobj->g_type_instance.g_class->g_type, sizeof(*obj), CAT_CT_CREATE);
			g_object_weak_ref(gobj, (GWeakNotify) l_week_notify, (void *) (gobj->g_type_instance.g_class->g_type));
		}
	}
}

void a_monitor_ref_destroyed(gconstpointer obj) {
	if (block==TRUE) {
		return;
	}
	if (obj) {
		l_monitor(obj, A_MRA_EOL);
	}
}

void a_monitor_set_active_for(gconstpointer obj) {
	if (block==TRUE) {
		return;
	}
	if (g_object_get_data((GObject *) obj, "a_ref_monitor")==NULL) {
		g_object_set_data((GObject *) obj, "a_ref_monitor", "a_ref_monitor");
		l_monitor(obj, A_MRA_MONITOR_START);
	}
}

void a_monitor_set_for_type_name(const char *name) {
	monitor_by_type_name = name;
}


#endif
