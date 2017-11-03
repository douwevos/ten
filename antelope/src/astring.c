/*
   File:    astring.c
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

#include "astring.h"
#include "aaltobjectprivate.h"
#include <string.h>
#include "profile/amonitor.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "AString"
#include "asupport.h"

struct _AContext {
	AAltObjectContext parent;
	gchar *data;
	int data_size;
	int size;
	unsigned int hash;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(AString, a_string, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void a_string_class_init(AStringClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;


	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->cloneContext = l_a_clone;
}

static void a_string_init(AString *instance) {
}

static void l_dispose(GObject *object) {
//	a_log_detail("dispose:%p", object);
//	AString *instance = A_STRING(object);
//	AStringPrivate *priv = a_string_get_instance_private(instance);
	G_OBJECT_CLASS(a_string_parent_class)->dispose(object);
//	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) object);
	AContext *context = (AContext *) priv->context;
	if (context) {
		a_log_detail("context=%p, data=%p", context, context->data);
		if (context->data_size>=0) {
			a_free_ptr(context->data);
		}
	}

	G_OBJECT_CLASS(a_string_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


AString *a_string_new() {
	AString *result = g_object_new(A_TYPE_STRING, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	return result;
}

AString *a_string_new_with(const char *txt) {
	AString *result = g_object_new(A_TYPE_STRING, NULL);
	a_monitor_set_active_for(result);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) result);
	AContext *context = (AContext *) priv->context;
	if (txt) {
		context->data = strdup(txt);
		context->size = strlen(txt);
		context->data_size = context->size;
	}
	return result;
}


AString *a_string_new_with_len(const char *txt, int length) {
	AString *result = g_object_new(A_TYPE_STRING, NULL);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) result);
	AContext *context = (AContext *) priv->context;
	context->data = strndup(txt, length);
	context->size = length;
	context->data_size = context->size;
	return result;
}

AStringAnchored *a_string_fast(const char *txt) {
	AString *result = g_object_new(A_TYPE_STRING, NULL);
	a_alt_object_construct((AAltObject *) result, FALSE);
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) result);
	AContext *context = (AContext *) priv->context;
	context->hash = 0;
	if (txt) {
		context->data = (char *) txt;
		context->size = strlen(txt);
		context->data_size = -1;
	}
	return result;
}

AStringAnchored *a_string_constant_get_declared(AStringConstant *string_constant) {
	AStringAnchored *result = string_constant->s;
	if (result == NULL) {
		result = a_string_fast(string_constant->txt);
		string_constant->s = result;
	}
	guint *d = (guint *) &(result->parent.parent.parent.ref_count);
	*d = 1000000;
//	result->parent.parent.parent.ref_count = 1000000;
	return result;
}

const char *a_string_chars(AStringShady *string) {
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) string);
	AContext *context = (AContext *) priv->context;
	return context->data ? context->data : "";
}

char a_string_char_at(AStringShady *string, int index) {
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) string);
	AContext *context = (AContext *) priv->context;
	return context->data[index];
}

int a_string_length(AStringShady *string) {
	AAltObjectPrivate *priv = a_alt_object_private((AAltObject *) string);
	AContext *context = (AContext *) priv->context;
	return context->size;
}



//
//int a_string_wo_hash(AStringShady *string) {
//	AAltObjectPrivate *priv = a_alt_object_private(string);
//	AContext *context = (AContext *) priv->context;
//	return context->hash;
//	const CatStringWoPrivate *priv = cat_string_wo_get_instance_private((CatStringWo *) string_wo);
//	if (!cat_wo_is_anchored((CatWo *) string_wo)) {
//		cat_log_info("WARN:hashing on a non anchored string is normally not a good idea");
//		return l_hash(priv->data, priv->size);
//	}
//	return priv->hash;
//}


static AContext *l_ensure_capicity(AString *string, int requested_size) {
	AAltObjectPrivate *priv = a_alt_object_private(string);
	requested_size = ((requested_size+115)/116)*116;
	AContext *context = (AContext *) priv->context;
	AEditContext *editor = (AEditContext *) priv->editor;
	AContext *ocontext = (AContext *) a_alt_object_editor_get_original_context((AAltObjectEditor *) editor);
	if (context == ocontext) {

		int n = requested_size<context->size ? requested_size : context->size;

		AAltObjectClass *a_cls = A_ALT_OBJECT_GET_CLASS(string);
		AContext *context_copy = (AContext *) g_malloc0(a_cls->context_size);
		context_copy->parent.ref_count = 1;
		context_copy->size = context->size;
		context_copy->data_size = requested_size;
		context_copy->data = g_malloc(requested_size+1);

		memcpy(context_copy->data, context->data, n);
		priv->context = (AAltObjectContext *) context_copy;
		context = context_copy;
		context->data_size = requested_size;
		context->data[context->size] = 0;
		return context_copy;
	} else  if (context->data_size<requested_size) {
		context->data = g_realloc(context->data, requested_size+1);
		context->data_size = requested_size;
	}
	return NULL;
}

void a_string_append_string(AString *string, AStringShady *to_add) {
	AAltObjectPrivate *priv = a_alt_object_private(to_add);
	AContext *context = (AContext *) priv->context;
	a_string_append_chars_len(string, context->data, context->size);
}

void a_string_append_char(AString *string, char ch) {
	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;

	int new_length = context->size + 1;
	AContext *new_context = l_ensure_capicity(string, new_length);
	if (new_context) {
		memcpy(new_context->data, context->data, context->size);
		context = new_context;
	}

	context = (AContext *) priv->context;
	context->data[new_length-1] = ch;
	context->data[new_length] = (gchar) 0;
	context->size = new_length;
}

void a_string_append_chars(AString *string, const char *txt) {
	a_string_append_chars_len(string, txt, strlen(txt));
}

void a_string_append_chars_len(AString *string, const char *txt, int length) {
	if (length<=0) {
		return;
	}
	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;

	int new_length = context->size + length;
	AContext *new_context = l_ensure_capicity(string, new_length);
	if (new_context) {
		memcpy(new_context->data, context->data, context->size);
		context = new_context;
	}

	context = (AContext *) priv->context;
	memcpy(context->data+context->size, txt, length);
	context->data[new_length] = (gchar) 0;
	context->size = new_length;
}

void a_string_remove_range(AString *string, int offset, int length) {
	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;

	if (length<=0) {
		return;
	}

	if (offset<0) {
		offset = 0;
	} else if (offset>=context->size) {
		return;
	}

	if (offset+length>context->size) {
		length = context->size-offset;
	}

	AContext *new_context = l_ensure_capicity(string, length);
	if (new_context) {
		memcpy(new_context->data, context->data, offset);
		memcpy(new_context->data+offset, context->data+offset+length, new_context->size-(offset+length));
		context = new_context;
	} else {
		memmove(context->data+offset, context->data+offset+length, context->size-(offset+length));
	}

	context->size -= length;
	context->data[context->size] = 0;
}


void a_string_append_decimal(AString *string, long long val) {
	int idx=0;
	gboolean sign = val<0;
	if (sign) {
		if (val==0x8000000000000000) {
			a_string_append_chars_len(string, "-9223372036854775808", 20);
			return;
		}
		val = -val;
	}
	char buf[60];
	long rest;
	while(idx<59) {
		rest = val % 10l;
		val = val/10l;
		buf[idx] = '0'+rest;
		idx++;
		if (val==0) {
			break;
		}
	}
	if (sign) {
		buf[idx++] = '-';
	}


	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;

	int new_length = context->size + idx;
	AContext *new_context = l_ensure_capicity(string, new_length);
	if (new_context) {
		memcpy(new_context->data, context->data, context->size);
		context = new_context;
	}

	gchar *new_data = context->data;
	int out_idx = context->size;
	for(idx--; idx>=0; idx--) {
		new_data[out_idx++] = buf[idx];
	}
	new_data[new_length] = (gchar) 0;
	context->size = new_length;
}


static const char *l_txt_hex = "0123456789ABCDEF";

void a_string_append_hexadecimal(AString *string, unsigned long long val, int digits, gboolean prefix_zero) {
	int idx=0;
	char buf[17];
	while(val>0) {
		buf[idx] = l_txt_hex[val & 0xf];
		val = val>>4;
		idx++;
	}
	if (idx==0) {
		buf[0] = '0';
		idx++;
	}

	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;

	int new_length = context->size;
	if (digits>0) {
		new_length += digits;
	} else {
		new_length += idx;
	}
	AContext *new_context = l_ensure_capicity(string, new_length);
	if (new_context) {
		memcpy(new_context->data, context->data, context->size);
		context = new_context;
	}

	gchar *new_data = context->data;
	int out_idx = context->size;
	if (digits>0) {
		char pch = prefix_zero ? '0' : ' ';
		while(digits>idx) {
			new_data[out_idx++] = pch;
			digits--;
		}
		if (digits<idx) {
			idx=digits;
		}
	}
	for(idx--; idx>=0; idx--) {
		new_data[out_idx++] = buf[idx];
	}
	new_data[new_length] = 0;
	context->size = new_length;
}


void a_string_format(AString *string, const char *format, ...) {
	va_list args;
	va_start(args, format);
	a_string_vformat(string, format, args);
	va_end(args);
}

#define FM_NONE         0
#define FM_START        1	// '%'
#define FM_ESCAPE       2	// '\'
#define FM_START_ZERO   3	// '%0'
#define FM_LONG         4	// '%l'


void a_string_vformat(AString *string, const char *format, va_list args) {
	gboolean keep_running = TRUE;
	int mode = FM_NONE;
	int cidx=0;
	int count = 0;
	gboolean prepend0 = FALSE;
	while(keep_running) {
		char ch = format[cidx];
		switch(mode) {
			case FM_NONE : {
				prepend0 = FALSE;
				count = 0;
				if(ch==0) {
					keep_running = FALSE;
				} else if (ch=='%') {
					mode = FM_START;
					count = 0;
					prepend0 = FALSE;
				} else if (ch=='\\') {
					mode = FM_ESCAPE;
				} else {
					a_string_append_char(string, ch);
				}
			} break;
			case FM_START : {
				if(ch==0) {
					a_string_append_char(string, '%');
					keep_running = FALSE;
				} else if (ch=='%') {
					a_string_append_char(string, '%');
				} else if (ch=='0') {
					if (count == 0) {
						prepend0 = TRUE;
					}
					count = count*10;
				} else if (ch>='1' && ch<='9') {
					count = (count*10) + (ch-'0');
				} else if (ch=='d') {
					int val = va_arg(args, int);
					a_string_append_decimal(string, val);
					mode = FM_NONE;
				} else if (ch=='f') {
					double val = va_arg(args, double);
					char ft[100];
					sprintf(ft, "%f", val);
					a_string_append_chars(string, ft);
					mode = FM_NONE;
				} else if (ch=='l') {
					mode = FM_LONG;
				} else if (ch=='s') {
					const char *val = va_arg(args, const char *);
					a_string_append_chars(string, val);
					mode = FM_NONE;
				} else if (ch=='o' || ch=='O') {
					GObject *val = va_arg(args, GObject *);
					if (val==NULL) {
						a_string_append_chars_len(string, "<nil>", 5);
					} else if (ch=='O' && A_IS_STRING(val)) {
						a_string_append_chars_len(string, a_string_chars((AString *) val), a_string_length((AString *) val));
					} else if (A_IS_OBJECT(val)) {
						A_OBJECT_GET_CLASS((AObject *) val)->asString((const AObject *) val, string);
					} else {
						a_string_append_chars(string, g_type_name_from_instance((GTypeInstance *) val));
						a_string_append_chars_len(string, "[0x", 3);
						a_string_append_hexadecimal(string, (unsigned long long) val, 0, FALSE);
						a_string_append_chars_len(string, "]", 1);
					}
					mode = FM_NONE;
				} else if (ch=='p') {
					void *val = va_arg(args, void *);
					a_string_append_chars_len(string, "0x", 2);
					a_string_append_hexadecimal(string, (unsigned long long) val, 0, FALSE);
					mode = FM_NONE;
				} else if (ch=='x') {
					int val = va_arg(args, int);
					a_string_append_hexadecimal(string, (unsigned long long) val, count, prepend0);
					mode = FM_NONE;
				} else if (ch=='c') {
					char val = (char) va_arg(args, int);
					a_string_append_char(string, val);
					mode = FM_NONE;
				} else {
					a_string_append_char(string, '%');
					a_string_append_char(string, ch);
					mode = FM_NONE;
				}
			} break;

			case FM_ESCAPE : {
				if(ch==0) {
					a_string_append_char(string, '\\');
					keep_running = FALSE;
				} else if (ch=='%') {
					a_string_append_char(string, '%');
					mode = FM_NONE;
				} else if (ch=='n') {
					a_string_append_char(string, '\n');
					mode = FM_NONE;
				} else if (ch=='t') {
					a_string_append_char(string, '\t');
					mode = FM_NONE;
				} else {
					a_string_append_char(string, '\\');
					a_string_append_char(string, ch);
					mode = FM_NONE;
				}
			} break;

			case FM_START_ZERO : {
				if(ch==0) {
					a_string_append_chars_len(string, "%0", 2);
					keep_running = FALSE;
				} else {
					a_string_append_chars_len(string, "%0", 2);
					mode = FM_NONE;
				}
			} break;

			case FM_LONG : {
				if(ch==0) {
					a_string_append_chars_len(string, "%l", 2);
					keep_running = FALSE;
				} else if (ch=='d') {
					long long val = va_arg(args, long long);
					a_string_append_decimal(string, val);
					mode = FM_NONE;
				} else if (ch=='f') {
					double val = va_arg(args, double);
					char ft[150];
					sprintf(ft, "%lf", val);
					a_string_append_chars(string, ft);
					mode = FM_NONE;
				} else {
					a_string_append_chars_len(string, "%l", 2);
					mode = FM_NONE;
				}
			} break;


		}
		cidx++;
	}
}

static guint l_hash(const gchar *data, int size) {
	unsigned int code = 0;
	int n = size>255 ? 255 : size;
	const gchar *p = data;

	for(; n; n--) {
		code = (code * 71) + *p;
		p++;
	}
	return code;
}

guint a_string_hash(AStringShady *string) {
	AAltObjectPrivate *priv = a_alt_object_private(string);
	AContext *context = (AContext *) priv->context;
	if (priv->editor) {
		return l_hash(context->data, context->data_size);
	}
	if (context->hash==0) {
		guint hash = l_hash(context->data, context->data_size);
		if (hash == 0) {
			hash++;
		}
		context->hash = hash;
	}
	return context->hash;
}

gboolean a_string_equal(AStringShady *string_a, AStringShady *string_b) {
	if (string_a==string_b) {
		return TRUE;
	} else if (string_a==NULL && string_b==NULL) {
		return FALSE;
	}
	return A_OBJECT_GET_CLASS(string_a)->equal((AObject *) string_a, (AObject *) string_b);
}



AStringAnchored *a_string_anchor(AStringShady *string) {
	AStringAnchored *result = (AStringAnchored *) a_alt_object_anchor(string);
	return result;
}

AString *a_string_mutable(AStringShady *string) {
	AString *result = (AString *) a_alt_object_mutable(string);
	return result;
}

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->data_size = from->size;
	to->size = from->size;
	to->data = g_malloc(sizeof(GObject *) * from->size+1);
	memcpy(to->data, from->data, from->size);
	to->data[to->size] = 0;
}

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}

	AAltObjectPrivate *priv_a = a_alt_object_private((AObject *) object_a);
	AContext *context_a = (AContext *) priv_a->context;
	AAltObjectPrivate *priv_b = a_alt_object_private((AObject *) object_b);
	AContext *context_b = (AContext *) priv_b->context;
	if (context_a == context_b) {
		return TRUE;
	}
	if (context_a->size!=context_b->size) {
		return FALSE;
	}
	return memcmp(context_a->data, context_b->data, context_a->size)==0;
}
