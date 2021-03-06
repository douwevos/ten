/*
   File:    buzrevision.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 19, 2017
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

#include "buzrevision.h"
#include <aaltobjectprivate.h>

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzRevision"
#include <asupport.h>

struct _AContext {
	AAltObjectContext parent;
	BuzContentShady *content;
	AArrayShady *cursors;
	BuzEnrichmentData *enrichment_data;
	int enriched_count;
};

struct _AEditContext {
	AAltObjectEditor parent;
};

typedef struct _AContext AContext;
typedef struct _AEditContext AEditContext;

G_DEFINE_TYPE(BuzRevision, buz_revision, A_TYPE_ALT_OBJECT);

static void l_dispose(GObject *object);
static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to);
static void l_a_anchor_content(AAltObject *object);
static gboolean l_equal(const AObject *object_a, const AObject *object_b);

static void buz_revision_class_init(BuzRevisionClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;

	AObjectClass *o_class = A_OBJECT_CLASS(clazz);
	o_class->equal = l_equal;

	AAltObjectClass *a_class = A_ALT_OBJECT_CLASS(clazz);
	a_class->context_size = sizeof(AContext);
	a_class->editor_size = sizeof(AEditContext);
	a_class->clone_context_for_mutable = TRUE;
	a_class->cloneContext = l_a_clone;
	a_class->anchorContent = l_a_anchor_content;
}

static void buz_revision_init(BuzRevision *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	if (context!=NULL && (a_alt_object_editor_get_original_context(priv->editor)!=priv->context)) {
		a_deref(context->content);
		a_deref(context->cursors);
	}
	G_OBJECT_CLASS(buz_revision_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

BuzRevision *buz_revision_new() {
	BuzRevision *result = g_object_new(BUZ_TYPE_REVISION, NULL);
	a_monitor_set_active_for(result);
	a_alt_object_construct((AAltObject *) result, TRUE);
	AContext *context = (AContext *) a_alt_object_private(result)->context;
	context->content = buz_content_new();

	context->cursors = a_array_new();
	BuzCursor *cursor = buz_cursor_new(0,0);
	a_array_add((AArray *) context->cursors, cursor);
	a_unref(cursor);
	return result;
}

static BuzContent *l_get_mutable_content(AContext *context) {
	if (!a_alt_object_is_mutable(context->content)) {
		BuzContent *content = buz_content_mutable(context->content);
		a_unref(context->content);
		context->content = content;
	}
	a_log_debug("context->content=%O", context->content);
	return (BuzContent *) context->content;
}

void buz_revision_insert(BuzRevision *revision, AStringShady *text) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	BuzContent *content = l_get_mutable_content(context);
	AIterator *iter = a_array_iterator(context->cursors);
	BuzCursorShady *cursor = NULL;
	while(a_iterator_next(iter, (gconstpointer *) &cursor)) {
		buz_content_insert(content, text, cursor);
	}
	a_unref(iter);
}

void buz_revision_set_page_at(BuzRevision *revision, BuzPageShady *page, int page_index) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	BuzContent *content = l_get_mutable_content(context);
	buz_content_set_page_at(content, page, page_index);
}

AArrayShady *buz_revision_get_cursors(BuzRevisionShady *revision) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	return context->cursors;
}

BuzContentShady *buz_revision_get_content(BuzRevisionShady *revision) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	return context->content;
}


int buz_revision_page_count(BuzRevisionShady *revision) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	return buz_content_page_count(context->content);
}

AIterator *buz_revision_page_iterator(BuzRevisionShady *revision) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	return buz_content_page_iterator(context->content);
}


void buz_revision_enrichment_remap(BuzRevisionAnchored *revision, BuzEnrichmentDataMapAnchored *old_map, BuzEnrichmentDataMapAnchored *new_map, BuzEnrichmentAction action, int index) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	BuzEnrichmentData *enrichment_data = context->enrichment_data;
	a_log_debug("remap:enrichment_data=%p", enrichment_data);
	if (enrichment_data) {
		buz_enrichment_data_remap(enrichment_data, old_map, new_map, action, index);
	}
	buz_content_enrichment_remap(context->content, old_map, new_map, action, index);
}

void buz_revision_enrich(BuzRevisionAnchored *revision, BuzEnrichmentDataMapAnchored *enrichment_map) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	if (context->enriched_count==0) {
		if (context->enrichment_data==NULL) {
			context->enrichment_data = buz_enrichment_data_new(enrichment_map);
		}
		buz_content_enrich(context->content, enrichment_map);
	}
	context->enriched_count++;
}

void buz_revision_impoverish(BuzRevisionAnchored *revision) {
	AContext *context = (AContext *) a_alt_object_private(revision)->context;
	if (context->enriched_count==1) {
		buz_content_impoverish(context->content);
		a_deref(context->enrichment_data);
	}
	context->enriched_count--;
}

A_ALT_C(BuzRevision, buz_revision)

static void l_a_clone(const AAltObjectContext *context_from, AAltObjectContext *context_to) {
	const AContext *from = (const AContext *) context_from;
	AContext *to = (AContext *) context_to;
	to->content = a_mutable_ref(from->content);
	to->cursors = a_mutable_ref(from->cursors);
	to->enriched_count = 0;
	to->enrichment_data = a_ref(from->enrichment_data);
}

static void l_a_anchor_content(AAltObject *object) {
	AAltObjectPrivate *priv = a_alt_object_private(object);
	AContext *context = (AContext *) priv->context;
	a_log_debug("pre_anchor:content=%o",context->content);
	context->content = buz_content_anchor(context->content);
	a_log_debug("post_anchor:content=%o",context->content);
	a_log_error("pre_anchor:cursors=%o, %o",context->cursors, a_array_first(context->cursors));

	context->cursors = a_array_anchor(context->cursors);
	a_log_error("post_anchor:cursors=%o, %o",context->cursors, a_array_first(context->cursors));
}

static gboolean l_equal(const AObject *object_a, const AObject *object_b) {
	a_log_debug("testing equality:a=%p, b=%p", object_a, object_b);
	if (object_a == object_b) {
		return TRUE;
	}
	if ((object_a==NULL) || (object_b==NULL)) {
		return FALSE;
	}


	AContext *context_a = (AContext *) a_alt_object_private(object_a)->context;
	AContext *context_b = (AContext *) a_alt_object_private(object_b)->context;
	if (context_a == context_b) {
		a_log_debug("context same");
		return TRUE;
	}

	a_log_error("testint content:a=%O, b=%O", context_a->content, context_b->content);

	return buz_content_equal(context_a->content, context_b->content)
			&& a_array_equal(context_a->cursors, context_b->cursors);
}
