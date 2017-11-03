/*
   File:    astring.h
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

#ifndef SRC_ASTRING_H_
#define SRC_ASTRING_H_

#include "aaltobject.h"

G_BEGIN_DECLS

#define A_TYPE_STRING              (a_string_get_type())
#define A_STRING(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), a_string_get_type(), AString))
#define A_STRING_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), A_TYPE_STRING, AStringClass))
#define A_IS_STRING(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), A_TYPE_STRING))
#define A_IS_STRING_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), A_TYPE_STRING))
#define A_STRING_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), A_TYPE_STRING, AStringClass))


typedef const struct _AString         AStringAnchored;
typedef const struct _AString         AStringShady;
typedef struct _AString               AString;
typedef struct _AStringClass          AStringClass;


struct _AString {
	AAltObject parent;
};

struct _AStringClass {
	AAltObjectClass parent_class;
};



typedef struct _AStringConstant AStringConstant;

struct _AStringConstant {
	const char *txt;
	AStringAnchored * volatile s;
};

#define a_string_constant_declare(t) { t, NULL }
#define a_string_constant_get(f) a_string_constant_get_declared(&f)


GType a_string_get_type();

AString *a_string_new();
AString *a_string_new_with(const char *txt);
AString *a_string_new_with_len(const char *txt, int length);

AStringAnchored *a_string_fast(const char *txt);
AStringAnchored *a_string_constant_get_declared(AStringConstant *string_constant);


const char *a_string_chars(AStringShady *string);
char a_string_char_at(AStringShady *string, int index);
int a_string_length(AStringShady *string);

void a_string_append_string(AString *string, AStringShady *to_add);
void a_string_append_char(AString *string, char ch);
void a_string_append_chars(AString *string, const char *txt);
void a_string_append_chars_len(AString *string, const char *txt, int length);
void a_string_remove_range(AString *string, int offset, int length);

void a_string_append_decimal(AString *string, long long val);
void a_string_append_hexadecimal(AString *string, unsigned long long val, int digits, gboolean prefix_zero);

void a_string_format(AString *string, const char *format, ...);
void a_string_vformat(AString *string, const char *format, va_list args);

guint a_string_hash(AStringShady *string);
gboolean a_string_equal(AStringShady *string_a, AStringShady *string_b);

AStringAnchored *a_string_anchor(AStringShady *string);
AString *a_string_mutable(AStringShady *string);

G_END_DECLS

#endif /* SRC_ASTRING_H_ */
