/*
   File:    buzdocumentloader.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 29, 2017
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

#include "buzdocumentloader.h"
#include "buzpagescanner.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzDocumentLoader"
#include <asupport.h>

struct _BuzDocumentLoaderPrivate {
	void *dummy;
};

G_DEFINE_TYPE_WITH_CODE(BuzDocumentLoader, buz_document_loader, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzDocumentLoader)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_document_loader_class_init(BuzDocumentLoaderClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_document_loader_init(BuzDocumentLoader *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
//	BuzDocumentLoader *instance = BUZ_DOCUMENT_LOADER(object);
//	BuzDocumentLoaderPrivate *priv = buz_document_loader_get_instance_private(instance);
	G_OBJECT_CLASS(buz_document_loader_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_document_loader_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzDocumentLoader *buz_document_loader_new() {
	BuzDocumentLoader *result = g_object_new(BUZ_TYPE_DOCUMENT_LOADER, NULL);
//	BuzDocumentLoaderPrivate *priv = buz_document_loader_get_instance_private(result);
	a_object_construct((AObject *) result);
	return result;
}

void buz_document_loader_load(BuzDocumentLoader *loader, BuzDocument *document, GFile *file) {
//	BuzDocumentLoaderPrivate *priv = buz_document_loader_get_instance_private(loader);
	a_log_info("start loading");

//	GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_SIZE, G_FILE_QUERY_INFO_NONE, NULL, NULL);
//	a_log_debug("info=%o", info);
//	if (info==NULL) {
//		return;
//	}
//	goffset file_length = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
//	a_unref(info);



	gsize contents_length = 0;
	char *contents = NULL;
	GError *error = NULL;
	if (g_file_load_contents(file, NULL, &contents, &contents_length, NULL, &error)) {

		BuzPageScanner *page_scanner = buz_page_scanner_new(contents, contents_length);

		BuzPage *page = buz_page_scanner_scan(page_scanner);

		BuzRevisionAnchored *revision_anc = buz_document_get_revision_ref(document);
		BuzRevision *revision = buz_revision_mutable(revision_anc);
		buz_revision_set_page_at(revision, page, 0);
		a_unref(page);
		buz_document_post_revision(document, revision);
		a_unref(page_scanner);
		a_unref(revision_anc);
	} else {
		a_log_error("error=%s", error->message);
	}


}
