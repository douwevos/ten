/*
   File:    buzeditor.c
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

#include "buzeditor.h"
#include "view/buzeditorview.h"
#include "view/buzview.h"

#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzEditor"
#include <asupport.h>

enum {
  PROP_0,
  PROP_HADJUSTMENT,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY,
};

struct _BuzEditorPrivate {
	GtkAdjustment *hadjustment;
	GtkAdjustment *vadjustment;
	int hscroll_policy : 1;
	int vscroll_policy : 1;

	BuzDocument *document;
	BuzEditorView *editor_view;
};

G_DEFINE_TYPE_WITH_CODE(BuzEditor, buz_editor, GTK_TYPE_WIDGET,
		G_ADD_PRIVATE(BuzEditor)
		G_IMPLEMENT_INTERFACE(GTK_TYPE_SCROLLABLE, NULL)
);

static void l_dispose(GObject *object);
static void l_finalize(GObject *object);
static void l_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void l_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void l_widget_realize(GtkWidget *widget);
static void l_widget_unrealize(GtkWidget *widget);
static void l_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static gboolean l_widget_draw(GtkWidget *widget, cairo_t *cr);
static gboolean l_widget_focus(GtkWidget *widget, GtkDirectionType direction);

static void buz_editor_class_init(BuzEditorClass *clazz) {
	GObjectClass *object_class = G_OBJECT_CLASS(clazz);
	object_class->dispose = l_dispose;
	object_class->finalize = l_finalize;
	object_class->set_property = l_set_property;
	object_class->get_property = l_get_property;

	/* GtkScrollable interface */
	g_object_class_override_property (object_class, PROP_HADJUSTMENT,    "hadjustment");
	g_object_class_override_property (object_class, PROP_VADJUSTMENT,    "vadjustment");
	g_object_class_override_property (object_class, PROP_HSCROLL_POLICY, "hscroll-policy");
	g_object_class_override_property (object_class, PROP_VSCROLL_POLICY, "vscroll-policy");

	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(clazz);
	widget_class->realize = l_widget_realize;
	widget_class->unrealize = l_widget_unrealize;
	widget_class->size_allocate = l_widget_size_allocate;
	widget_class->draw = l_widget_draw;
	widget_class->focus = l_widget_focus;
}

static void buz_editor_init(BuzEditor *instance) {
}

static void l_dispose(GObject *object) {
	a_log_detail("dispose:%p", object);
	BuzEditor *instance = BUZ_EDITOR(object);
	BuzEditorPrivate *priv = buz_editor_get_instance_private(instance);
	a_deref(priv->editor_view);
	a_deref(priv->document);
	G_OBJECT_CLASS(buz_editor_parent_class)->dispose(object);
	a_log_detail("disposed:%p", object);
}

static void l_finalize(GObject *object) {
	a_log_detail("finalize:%p", object);
	G_OBJECT_CLASS(buz_editor_parent_class)->finalize(object);
	a_log_detail("finalized:%p", object);
}

static gboolean l_key_press_event(GtkWidget *gwidget, GdkEventKey *eev, gpointer data);

void buz_editor_construct(BuzEditor *editor) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);
	a_announce(editor);
	priv->editor_view = NULL;
	priv->document = NULL;
	GtkWidget *widget = GTK_WIDGET(editor);

	gtk_widget_set_events(widget, gtk_widget_get_events(widget)
	                                             | GDK_BUTTON_PRESS_MASK
	                                             | GDK_BUTTON2_MASK
	                                             | GDK_BUTTON3_MASK
	                                             | GDK_BUTTON4_MASK
	                                             | GDK_BUTTON5_MASK
	                                             | GDK_BUTTON_RELEASE_MASK
	                                             | GDK_POINTER_MOTION_MASK
	                                             | GDK_POINTER_MOTION_HINT_MASK
	                                             | GDK_BUTTON_RELEASE_MASK
	                                             | GDK_KEY_PRESS_MASK
	                                             | GDK_SCROLL_MASK
	                                             );

	g_signal_connect(editor, "key-press-event", G_CALLBACK(l_key_press_event), editor);

	gtk_widget_set_app_paintable(widget, TRUE);
	gtk_widget_set_can_focus(widget, TRUE);
	gtk_widget_show(widget);
}

BuzEditor *buz_editor_new(BuzDocument *document) {
	BuzEditor *result = g_object_new(BUZ_TYPE_EDITOR, NULL);
	BuzEditorPrivate *priv = buz_editor_get_instance_private(result);
	buz_editor_construct(result);
	priv->document = a_ref(document);
	return result;
}



static void l_set_hadjustment_values(BuzEditor *editor) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);
	GtkAllocation allocation;
	gtk_widget_get_allocation(GTK_WIDGET(editor), &allocation);
	int view_width = allocation.width;

	gdouble old_value = gtk_adjustment_get_value(priv->hadjustment);
	gdouble new_upper = view_width * 2;
//  gdouble new_upper = MAX (screen_width, priv->width);
	g_object_set(priv->hadjustment, "lower", 0.0, "upper", new_upper,
			"page-size", (gdouble) view_width,
			"step-increment", view_width * 0.1, "page-increment", view_width * 0.9,
			NULL);

	gdouble new_value = CLAMP(old_value, 0, new_upper - view_width);
	if (new_value != old_value) {
		gtk_adjustment_set_value (priv->hadjustment, new_value);
	}
}



static void l_set_vadjustment_values(BuzEditor *editor) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);
	GtkAllocation allocation;
	gtk_widget_get_allocation(GTK_WIDGET(editor), &allocation);
	int view_height = allocation.height;

	gdouble old_value = gtk_adjustment_get_value(priv->vadjustment);
	gdouble new_upper = gtk_adjustment_get_upper(priv->vadjustment);
	if (priv->editor_view) {
		BuzView *view = buz_editor_view_get_view(priv->editor_view);
		long long layout_height = buz_view_get_layout_height(view);
//		long long int layout_height = cha_document_view_get_layout_height(priv->document_view);
		new_upper = MAX(layout_height, view_height);
	}
	a_log_error("view_height=%d, new_upper=%d", view_height, (int) new_upper);
//  gdouble new_upper = MAX (screen_width, priv->width);
	g_object_set(priv->vadjustment, "lower", 0.0, "upper", new_upper,
			"page-size", (gdouble) view_height,
			"step-increment", view_height * 0.1, "page-increment", view_height * 0.9,
			NULL);

	gdouble new_value = CLAMP(old_value, 0, new_upper - view_height);
	a_log_error("new_value=%d, old_value=%d", (int) new_value, (int) old_value);
	if (new_value != old_value) {
		gtk_adjustment_set_value (priv->vadjustment, new_value);
	}
}



static void l_hadjustment_value_changed(GtkAdjustment *adjustment, BuzEditor *editor) {
//	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);
//	glong aval = (glong) gtk_adjustment_get_value(adjustment);
//	glong old = cha_document_view_set_view_x(priv->document_view, aval);
//    glong dx = old - aval;
//	if (priv->depreacated_scrolling) {
//		l_deprecated_scroll(editor, dx, 0);
//	} else {
//		GdkWindow *window = gtk_widget_get_window((GtkWidget *) editor);
//		cha_document_view_invalidate_lines(priv->document_view);
//		gdk_window_scroll(window, dx, 0);
//		gdk_window_process_updates(window, TRUE);
//	}
}


static void l_vadjustment_value_changed(GtkAdjustment *adjustment, BuzEditor *editor) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);
	glong aval = (glong) gtk_adjustment_get_value(adjustment);

//	int s = (int) (aval % priv->font_height);
//	if (s>priv->font_height/2) {
//		s -= priv->font_height;
//	}
//	aval = aval - s;

	glong old = buz_editor_view_set_view_y(priv->editor_view, aval);
//    glong dy = old - aval;
//    a_log_debug("scroll-v:%d, %d", (int) dy, aval);
//
//    if (priv->depreacated_scrolling) {
//    	l_deprecated_scroll(editor, 0, dy);
//    } else {
//		GdkWindow *window = gtk_widget_get_window((GtkWidget *) editor);
//		cha_document_view_set_in_scroll(priv->document_view, TRUE);
//		if (!cha_document_view_check_cache_boundaries(priv->document_view)) {
//			cha_document_view_invalidate_lines(priv->document_view);
//		}
//		gdk_window_scroll(window, 0, dy);
//		gdk_window_process_updates(window, TRUE);
//		cha_document_view_set_in_scroll(priv->document_view, FALSE);
//    }
}


static void l_set_hadjustment(BuzEditor *editor, GtkAdjustment *adjustment) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);

	if (adjustment && priv->hadjustment == adjustment) {
	    return;
	}

	if (priv->hadjustment != NULL) {
		g_signal_handlers_disconnect_by_func(priv->hadjustment, l_hadjustment_value_changed, editor);
		g_object_unref(priv->hadjustment);
	}

	if (adjustment == NULL) {
	    adjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	g_signal_connect(adjustment, "value-changed", G_CALLBACK(l_hadjustment_value_changed), editor);
	priv->hadjustment = a_ref(adjustment);
	if (priv->editor_view) {
		buz_editor_view_set_adjustments(priv->editor_view, priv->hadjustment, priv->vadjustment);
	}
	l_set_hadjustment_values(editor);
	g_object_notify(G_OBJECT(editor), "hadjustment");
}


static void l_set_vadjustment(BuzEditor *editor, GtkAdjustment *adjustment) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);

	if (adjustment && priv->vadjustment == adjustment) {
	    return;
	}

	if (priv->vadjustment != NULL) {
		g_signal_handlers_disconnect_by_func(priv->vadjustment, l_vadjustment_value_changed, editor);
		g_object_unref(priv->vadjustment);
	}

	if (adjustment == NULL) {
	    adjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	g_signal_connect(adjustment, "value-changed", G_CALLBACK(l_vadjustment_value_changed), editor);
	priv->vadjustment = a_ref(adjustment);
	if (priv->editor_view) {
		buz_editor_view_set_adjustments(priv->editor_view, priv->hadjustment, priv->vadjustment);
	}
	l_set_vadjustment_values(editor);
	g_object_notify(G_OBJECT(editor), "vadjustment");
}



static void l_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
	BuzEditor *editor = BUZ_EDITOR(object);
	BuzEditorPrivate *priv = buz_editor_get_instance_private(editor);

	switch (prop_id) {
		case PROP_HADJUSTMENT:
			l_set_hadjustment(editor, g_value_get_object(value));
			break;
		case PROP_VADJUSTMENT:
			l_set_vadjustment(editor, g_value_get_object(value));
			break;
		case PROP_HSCROLL_POLICY:
			if (priv->hscroll_policy != g_value_get_enum(value)) {
				priv->hscroll_policy = g_value_get_enum(value);
				gtk_widget_queue_resize(GTK_WIDGET(editor));
				g_object_notify_by_pspec(object, pspec);
			} break;
    	case PROP_VSCROLL_POLICY:
    		if (priv->vscroll_policy != g_value_get_enum(value)) {
    			priv->vscroll_policy = g_value_get_enum(value);
    			gtk_widget_queue_resize(GTK_WIDGET(editor));
    			g_object_notify_by_pspec (object, pspec);
    		} break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


static void l_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(object));

	switch(prop_id) {
		case PROP_HADJUSTMENT:
			g_value_set_object(value, priv->hadjustment);
			break;
		case PROP_VADJUSTMENT:
			g_value_set_object(value, priv->vadjustment);
			break;
		case PROP_HSCROLL_POLICY:
			g_value_set_enum(value, priv->hscroll_policy);
			break;
		case PROP_VSCROLL_POLICY:
			g_value_set_enum(value, priv->vscroll_policy);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


static gboolean l_key_press_event(GtkWidget *gwidget, GdkEventKey *eev, gpointer data) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(data));
	return buz_editor_view_key_pressed(priv->editor_view, eev);
}


static void l_widget_realize(GtkWidget *widget) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(widget));

	gtk_widget_set_realized(widget, TRUE);

	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);

	GdkWindowAttr attributes;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = allocation.x;
	attributes.y = allocation.y;
	attributes.width = allocation.width;
	attributes.height = allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.event_mask = GDK_VISIBILITY_NOTIFY_MASK
			| GDK_EXPOSURE_MASK
			| gtk_widget_get_events(widget);

	gint attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

	GdkWindow *window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
	gtk_widget_set_window(widget, window);
	gtk_widget_register_window(widget, window);

//	priv->pango_context = gtk_widget_create_pango_context(widget);
//
	if (priv->editor_view==NULL) {
		BuzView *buz_view = buz_view_new(priv->document);
//		BuzEditorClass *editor_class = BUZ_EDITOR_GET_CLASS(widget);
//		if (editor_class->initializeDocumentView) {
//			priv->editor_view = editor_class->initializeDocumentView((BuzEditor *) widget, priv->document, priv->pango_context, widget);
//		} else {
			priv->editor_view = buz_editor_view_new(priv->document, widget);
//		}
//		if (priv->a_preferences) {
//			cha_document_view_set_preferences(priv->document_view, priv->a_preferences);
//		}
		buz_editor_view_set_adjustments(priv->editor_view, priv->hadjustment, priv->vadjustment);
		a_unref(buz_view);
	}
//
//	if (priv->cursor==NULL) {
//		priv->cursor = gdk_cursor_new_for_display(gdk_window_get_display(window), GDK_XTERM);
//	}
//
//	gdk_window_set_cursor(gtk_widget_get_window(widget), priv->cursor);
//	gtk_im_context_set_client_window(priv->im_context, window);
}

static void l_widget_unrealize(GtkWidget *widget) {
//	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(widget));
//	gtk_im_context_set_client_window(priv->im_context, NULL);
	GTK_WIDGET_CLASS(buz_editor_parent_class)->unrealize (widget);
}


static void l_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(widget));

	GtkAllocation cur_alloc;
	gtk_widget_get_allocation(widget, &cur_alloc);
	if (cur_alloc.x==allocation->x
			&& cur_alloc.y==allocation->y
			&& cur_alloc.width==allocation->width
						&& cur_alloc.height==allocation->height
						) {
		a_log_warn("breaking !!!");
		return;
	}
	gtk_widget_set_allocation(widget, allocation);

	if (gtk_widget_get_realized(widget)) {
		gdk_window_move_resize(gtk_widget_get_window(widget), allocation->x, allocation->y, allocation->width, allocation->height);
	}

	buz_editor_view_set_view_size(priv->editor_view, allocation->width, allocation->height);

	l_set_hadjustment_values((BuzEditor *) widget);
	l_set_vadjustment_values((BuzEditor *) widget);

//	cha_document_view_invalidate_lines(priv->document_view);
}


static gboolean l_widget_draw(GtkWidget *widget, cairo_t *cr) {
	BuzEditorPrivate *priv = buz_editor_get_instance_private(BUZ_EDITOR(widget));

	cairo_set_source_rgb(cr, 0,0,0);
//	cairo_move_to(cr, 10, 20);
//	cairo_line_to(cr, 100,500);
//	cairo_stroke(cr);
	buz_editor_view_draw(priv->editor_view, cr);
	return FALSE;
}

static gboolean l_widget_focus(GtkWidget *widget, GtkDirectionType direction) {
	gtk_widget_grab_focus(widget);
	return TRUE;
}
