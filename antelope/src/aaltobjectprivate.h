/*
   File:    aobjectprivate.h
   Project: antelope
   Author:  Douwe Vos
   Date:    Oct 8, 2017
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

#ifndef SRC_AALTOBJECTPRIVATE_H_
#define SRC_AALTOBJECTPRIVATE_H_

#include "aaltobject.h"

struct _AAltObjectPrivate {
	AAltObjectContext *context;
	AAltObjectEditor *editor;
};

AAltObjectPrivate *a_alt_object_private(gconstpointer object);

#endif /* SRC_AALTOBJECTPRIVATE_H_ */
