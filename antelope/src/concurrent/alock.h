/*
   File:    alock.h
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

#ifndef ALOCK_H_
#define ALOCK_H_

#include "../aobject.h"
#include <stdint.h>

G_BEGIN_DECLS

#define A_TYPE_LOCK            (a_lock_get_type())
#define A_LOCK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_lock_get_type(), ALock))
#define A_LOCK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_LOCK, ALockClass))
#define A_IS_LOCK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_LOCK))
#define A_IS_LOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_LOCK))
#define A_LOCK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_LOCK, ALockClass))


typedef struct _ALock	   	   ALock;
typedef struct _ALockClass   ALockClass;


struct _ALock {
	AObject parent;
	GMutex mutex;
	GCond condition;
};

struct _ALockClass {
	AObjectClass parent_class;
};


GType a_lock_get_type();

ALock *a_lock_new();


void a_lock_lock(ALock *lock);
void a_lock_unlock(ALock *lock);

void a_lock_wait(ALock *lock);
gboolean a_lock_wait_timed(ALock *lock, int64_t ms);

void a_lock_notify(ALock *lock);
void a_lock_notify_all(ALock *lock);

G_END_DECLS

#endif /* ALOCK_H_ */
