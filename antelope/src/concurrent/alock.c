/*
   File:    alock.c
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 15, 2017
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

#include "alock.h"

#include <sys/time.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "ALock"
#include "../asupport.h"

G_DEFINE_TYPE(ALock, a_lock, A_TYPE_OBJECT)

static void l_dispose(GObject *object);

static void a_lock_class_init(ALockClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
}

static void a_lock_init(ALock *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	ALock *instance = A_LOCK(object);
//	if (instance->mutex) {
//#ifdef A_REF_MONITORING
//		if (g_mutex_trylock(instance->mutex)) {
//			cat_stacktrace_print();
//		}
//#endif
		g_mutex_clear(&(instance->mutex));
//		instance->mutex = NULL;
//	}

//	if (instance->condition) {
		g_cond_clear(&(instance->condition));
//	}
	a_log_detail("end-dispose:%p", object);
}


ALock *a_lock_new() {
	ALock *result = g_object_new(A_TYPE_LOCK, NULL);
	a_object_construct((AObject *) result);
	g_mutex_init(&(result->mutex));
	g_cond_init(&(result->condition));
	return result;
}


void a_lock_lock(ALock *lock) {
//	int64_t start = cat_date_current_time();
//	while(TRUE) {
//		gboolean could_lock = g_mutex_trylock(lock->mutex);
//		if (could_lock) {
//			return;
//		}
//		int64_t end = cat_date_current_time();
//		if ((start-end)>10000) {
//			cat_stacktrace_print();
//			sleep(3);
////			a_lock_wait_timed(lock, 2500);
//		} else {
//			sleep(1);
////			a_lock_wait_timed(lock, 250);
//		}
//	}

	g_mutex_lock(&(lock->mutex));
}

void a_lock_unlock(ALock *lock) {
	g_mutex_unlock(&(lock->mutex));
}

void a_lock_wait(ALock *lock) {
	g_cond_wait(&(lock->condition), &(lock->mutex));
}

gboolean a_lock_wait_timed(ALock *lock, int64_t ms) {
	gint64 end_time = g_get_monotonic_time () + (ms * G_TIME_SPAN_SECOND)/1000l;
	return g_cond_wait_until(&(lock->condition), &(lock->mutex), end_time);
}

void a_lock_notify(ALock *lock) {
	g_cond_signal(&(lock->condition));
}

void a_lock_notify_all(ALock *lock) {
	g_cond_broadcast(&(lock->condition));
}
