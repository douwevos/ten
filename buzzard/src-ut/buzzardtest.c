/*
   File:    buzzardtest.c
   Project: buzzard
   Author:  Douwe Vos
   Date:    Oct 18, 2017
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

#include <gtk/gtk.h>
#include <antelope.h>
#include "../src/buzeditor.h"
#include "../src/document/buzdocument.h"
#include "../src/io/buzdocumentloader.h"
//#include "../src/io/chadocumentmanager.h"


#define A_LOG_LEVEL A_LOG_WARN
#define A_LOG_CLASS "BuzzardTest"
#include <asupport.h>

static gboolean l_close_window(GtkWidget *widget, GdkEvent *eev, gpointer main_ptr) {
	gtk_main_quit();
	return FALSE;
}


static void my_g_log_default_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {
	a_log_fatal("%s(level:%d) %s", log_domain, log_level, message);
	a_stacktrace_print();


	a_monitor_report();
}

int main(int argc, char **argv) {

	a_monitor_set_for_type_name("AString");

	g_log_set_handler("GLib-GObject", G_LOG_LEVEL_CRITICAL, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler("GLib-GObject", G_LOG_LEVEL_WARNING, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler("Gdk", G_LOG_LEVEL_CRITICAL, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler("Gdk", G_LOG_LEVEL_WARNING, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler("Gtk", G_LOG_LEVEL_CRITICAL, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_CRITICAL, (GLogFunc) my_g_log_default_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_WARNING, (GLogFunc) my_g_log_default_handler, NULL);

	gtk_init(&argc, &argv);

//	ChaEditorService *editor_service = cha_editor_service_new();

	GtkWidget *top_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(top_window), "delete-event", G_CALLBACK(l_close_window), NULL);
	gtk_widget_set_size_request(top_window, 512, 368);


	GtkScrolledWindow *scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add((GtkContainer *) top_window, (GtkWidget *) scrolled_window);
//
//	ChaDocumentManager *document_manager = cha_document_manager_new();
//
//	ChaDocument *document = cha_document_new(document_manager);
	BuzDocument *document = buz_document_new();
	BuzEditor *buz_editor = buz_editor_new(document);
	gtk_container_add((GtkContainer *) scrolled_window, (GtkWidget *) buz_editor);


	GFile *file = g_file_new_for_path("/home/superman/cpp-workspace/natpad/chameleon/src/layout/chadocumentview.c");
//	buz_document_open(document, file);

	BuzDocumentLoader *loader = buz_document_loader_new();
	buz_document_loader_load(loader, document, file);
	a_unref(loader);
	//	cha_document_manager_load(document_manager, document, file);
	a_unref(document);


	gtk_widget_set_visible(top_window, TRUE);
	gtk_widget_show_all(top_window);
	gtk_widget_grab_focus((GtkWidget *) buz_editor);
	gtk_main();

	a_monitor_report();
	a_type_monitor_entry_report();

	return 0;
}
