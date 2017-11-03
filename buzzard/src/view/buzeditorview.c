/*
   File:    buzeditorview.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 21, 2017
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

#include "buzeditorview.h"
#include "buzview.h"
#include "../layout/buzlayoutcontext.h"
#include "../layout/buztextlayout.h"
#include "../document/content/buzrevision.h"
#include "../document/buzdocumentlistener.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzEditorView"
#include <asupport.h>

struct _BuzEditorViewPrivate {
	BuzDocument *document;
	GtkWidget *widget;
	BuzView *view;
};

static void l_document_listener_iface_init(BuzDocumentListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE(BuzEditorView, buz_editor_view, A_TYPE_OBJECT,
		G_ADD_PRIVATE(BuzEditorView)
		G_IMPLEMENT_INTERFACE(BUZ_TYPE_DOCUMENT_LISTENER, l_document_listener_iface_init)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);

static void buz_editor_view_class_init(BuzEditorViewClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
}

static void buz_editor_view_init(BuzEditorView *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzEditorView *instance = BUZ_EDITOR_VIEW(object);
	BuzEditorViewPrivate *priv = buz_editor_view_get_instance_private(instance);
	a_deref(priv->document);
	a_deref(priv->view);
	G_OBJECT_CLASS(buz_editor_view_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_editor_view_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}


BuzEditorView *buz_editor_view_new(BuzDocument *document, GtkWidget *widget) {
	BuzEditorView *result = g_object_new(BUZ_TYPE_EDITOR_VIEW, NULL);
	BuzEditorViewPrivate *priv = buz_editor_view_get_instance_private(result);
	a_object_construct((AObject *) result);
	priv->document = a_ref(document);
	priv->widget = widget;
	priv->view = NULL;
	buz_document_add_listener(document, BUZ_DOCUMENT_LISTENER(result));
	return result;
}


gboolean buz_editor_view_key_pressed(BuzEditorView *editor_view, GdkEventKey *event) {
	BuzEditorViewPrivate *priv = buz_editor_view_get_instance_private(editor_view);
	BuzRevisionAnchored *revision_anc = buz_document_get_revision_ref(priv->document);
	a_log_debug("hitting key:%s, revision-in=%o", event->string, revision_anc);
	BuzRevision *revision = buz_revision_mutable(revision_anc);
	AStringAnchored *key_string = a_string_fast(event->string);
	buz_revision_insert(revision, key_string);
	a_log_debug("hitting key:%s, revision-out=%o", event->string, revision);
	a_unref(key_string);
	buz_document_post_revision(priv->document, revision);
	a_unref(revision_anc);
	return FALSE;
}

void buz_editor_view_draw(BuzEditorView *editor_view, cairo_t *cr) {
	BuzEditorViewPrivate *priv = buz_editor_view_get_instance_private(editor_view);
	BuzRevisionAnchored *revision = buz_document_get_revision_ref(priv->document);
	AIterator *page_iter = buz_revision_page_iterator(revision);
	a_unref(revision);
	BuzPageShady *page = NULL;

	PangoContext *pango_context = gtk_widget_create_pango_context(priv->widget);
//	pango_context_new();
	PangoFontDescription *font_descr = pango_font_description_from_string("Ubuntu Mono 13");
	pango_context_set_font_description(pango_context, font_descr);
//	PangoFont *font = pango_context_load_font(pango_context, font_descr);
	pango_context_changed(pango_context);

//	PangoContext *pango_context = gtk_widget_get_pango_context(priv->widget);
	BuzLayoutContext *layout_context = buz_layout_context_new(pango_context);
	a_log_debug("do_draw:revision=%o", revision);

	double view_y = 0;
	while(a_iterator_next(page_iter, (gconstpointer *) &page)) {
		int page_row_count = buz_page_row_count(page);
		int page_row_idx;
		a_log_debug("page=%o, row_count=%d", page, page_row_count);
		for(page_row_idx=0; page_row_idx<page_row_count; page_row_idx++) {
			BuzRowShady *buzrow = buz_page_row_at(page, page_row_idx);
//			a_log_debug("buzrow=%o", buzrow);
			AStringShady *text = buz_row_text(buzrow);
			BuzTextLayout *text_layout = buz_text_layout_new(layout_context);
			buz_text_layout_set_text(text_layout, text);
//			PangoLayout  *pango_layout = gtk_widget_create_pango_layout(priv->widget, a_string_chars(text));
			cairo_move_to(cr, 0, view_y);
//			PangoRectangle inkt_rect;
//			pango_layout_get_pixel_extents(pango_layout, &inkt_rect, NULL);
			buz_text_layout_show(text_layout, cr);
//			pango_cairo_show_layout(cr, pango_layout);
//			a_unref(pango_layout);
			view_y += buz_text_layout_get_height(text_layout);
			a_unref(text_layout);
		}
	}
	a_unref(page_iter);
	a_unref(layout_context);
}

static void l_document_new_revision(BuzDocumentListener *listener, BuzRevisionAnchored *revision) {
	BuzEditorView *editor_view = BUZ_EDITOR_VIEW(listener);
	BuzEditorViewPrivate *priv = buz_editor_view_get_instance_private(editor_view);
	gtk_widget_queue_draw(priv->widget);
}

static void l_document_listener_iface_init(BuzDocumentListenerInterface *iface) {
	iface->newRevision = l_document_new_revision;
}

