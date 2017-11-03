/*
   File:    alockut.c
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

#include "alockut.h"

#include "../src/concurrent/alock.h"

#define A_LOG_LEVEL A_LOG_WARN
#include "../src/asupport.h"

void test_lock(ATestContext *context) {
	ALock *lock = a_lock_new();

	a_lock_lock(lock);

	a_lock_wait_timed(lock, 2000l);

	a_lock_unlock(lock);
	a_deref(lock);
}
