/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Hazlo - Simple to-do list manager
 * Copyright © 2014 Álvaro Peña <alvaropg@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "config.h"

#include <glib/gi18n.h>

#include "hzl-application-window.h"
#include "hzl-task.h"
#include "hzl-tasks-list.h"

static void hzl_application_window_class_init (HzlApplicationWindowClass *klass);
static void hzl_application_window_init       (HzlApplicationWindow *self);
static void hzl_application_window_dispose    (GObject *gobject);

static void hzl_application_window_task_entry_activate_cb  (GtkEntry              *entry,
                                                            gpointer               user_data);
static void hzl_application_window_task_entry_focus_in_cb  (GtkWidget             *widget,
                                                            GdkEvent              *event,
                                                            gpointer               user_data);
static void hzl_application_window_task_entry_focus_out_cb (GtkWidget             *widget,
                                                            GdkEvent              *event,
                                                            gpointer               user_data);
static void hzl_application_window_show_tasks_list_cb      (GObject               *source_object,
                                                            GAsyncResult          *async_result,
                                                            gpointer               user_data);
static void hzl_application_window_task_done_cb            (GtkCellRendererToggle *cell_renderer,
                                                            gchar                 *path_string,
                                                            gpointer               user_data);

struct _HzlApplicationWindowPrivate {
        GtkWidget *header_bar;
        GtkWidget *stack;
        GtkListStore *tasks_store;
        HzlTasksList *current_list;
};

G_DEFINE_TYPE_WITH_PRIVATE (HzlApplicationWindow, hzl_application_window, GTK_TYPE_APPLICATION_WINDOW);

enum
{
        DONE_COLUMN,
        TEXT_COLUMN,
        OBJECT_COLUMN,
        N_COLUMNS
};

GdkRGBA task_entry_color = { 0.5, 0.5, 0.5, 1 };

static void
hzl_application_window_class_init (__attribute__ ((unused)) HzlApplicationWindowClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->dispose = hzl_application_window_dispose;
}

static void
hzl_application_window_init (HzlApplicationWindow *self)
{
        GtkWidget *scrolled_window;
        GtkWidget *tree_view;
        GtkWidget *task_entry;
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        GdkGeometry geometry = {
                320,
                400,
                400,
                600,
                320,
                400
        };

        self->priv = hzl_application_window_get_instance_private (self);

        gtk_window_set_geometry_hints (GTK_WINDOW (self),
                                       NULL,
                                       &geometry,
                                       GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE | GDK_HINT_BASE_SIZE);

        self->priv->header_bar = gtk_header_bar_new ();
        gtk_header_bar_set_title (GTK_HEADER_BAR (self->priv->header_bar), _("Hazlo"));
        gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (self->priv->header_bar), TRUE);
        gtk_window_set_titlebar (GTK_WINDOW (self), self->priv->header_bar);
        gtk_widget_show (self->priv->header_bar);

        self->priv->stack = gtk_stack_new ();
        gtk_container_add (GTK_CONTAINER (self), self->priv->stack);
        gtk_widget_show (self->priv->stack);

        task_entry = gtk_entry_new ();
        gtk_widget_override_color (task_entry, GTK_STATE_FLAG_NORMAL, &task_entry_color);
        gtk_entry_set_text (GTK_ENTRY (task_entry), _("Write here your task"));
        gtk_header_bar_pack_start (GTK_HEADER_BAR (self->priv->header_bar), task_entry);
        g_signal_connect (G_OBJECT (task_entry), "focus-in-event", G_CALLBACK (hzl_application_window_task_entry_focus_in_cb), self);
        g_signal_connect (G_OBJECT (task_entry), "focus-out-event", G_CALLBACK (hzl_application_window_task_entry_focus_out_cb), self);
        g_signal_connect (G_OBJECT (task_entry), "activate", G_CALLBACK (hzl_application_window_task_entry_activate_cb), self);
        gtk_widget_show (task_entry);

        self->priv->tasks_store = gtk_list_store_new (N_COLUMNS,
                                                      G_TYPE_BOOLEAN,
                                                      G_TYPE_STRING,
                                                      G_TYPE_OBJECT);
        tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (self->priv->tasks_store));
        gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree_view), FALSE);
        /* Done column */
        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("Done",
                                                           renderer,
                                                           "active", DONE_COLUMN,
                                                           NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);
        g_signal_connect (G_OBJECT (renderer), "toggled", G_CALLBACK (hzl_application_window_task_done_cb), self);

        /* Text column */
        renderer = gtk_cell_renderer_text_new ();
        column = gtk_tree_view_column_new_with_attributes ("Text",
                                                           renderer,
                                                           "text", TEXT_COLUMN,
                                                           NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

        scrolled_window = gtk_scrolled_window_new (NULL, NULL);
        gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
        gtk_widget_show (scrolled_window);
        gtk_container_add (GTK_CONTAINER (self->priv->stack), scrolled_window);
        gtk_widget_show (tree_view);
}

static void
hzl_application_window_dispose (GObject *gobject)
{
        HzlApplicationWindow *self = HZL_APPLICATION_WINDOW (gobject);

        g_clear_object (&self->priv->current_list);

        G_OBJECT_CLASS (hzl_application_window_parent_class)->dispose (gobject);
}

static void
hzl_application_window_task_entry_focus_in_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
        gtk_widget_override_color (widget, GTK_STATE_FLAG_NORMAL, NULL);
        gtk_entry_set_text (GTK_ENTRY (widget), "");
}

static void
hzl_application_window_task_entry_focus_out_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
        gtk_widget_override_color (widget, GTK_STATE_FLAG_NORMAL, &task_entry_color);
        gtk_entry_set_text (GTK_ENTRY (widget), _("Write here your task"));
}

static void
hzl_application_window_task_entry_activate_cb (GtkEntry *entry, gpointer user_data)
{
        HzlApplicationWindow *self = HZL_APPLICATION_WINDOW (user_data);
        HzlTask *new_task;
        const gchar *text_entry;
        GomRepository *repository;
        GError *error = NULL;

        text_entry = gtk_entry_get_text (entry);
        if (gtk_entry_get_text_length (entry) == 0)
                return;

        g_object_get (G_OBJECT (self->priv->current_list),
                      "repository", &repository,
                      NULL);
        new_task = g_object_new (HZL_TYPE_TASK,
                                 "repository", repository,
                                 "text", text_entry,
                                 "list_uuid", hzl_tasks_list_get_uuid (self->priv->current_list),
                                 NULL);
        gom_resource_save_sync (GOM_RESOURCE (new_task), &error);
        if (error != NULL)
                g_warning ("Error saving task: %s\n", error->message);
        else {
                GtkTreeIter iter;

                gtk_list_store_append (self->priv->tasks_store, &iter);
                gtk_list_store_set (self->priv->tasks_store, &iter,
                                    DONE_COLUMN, FALSE,
                                    TEXT_COLUMN, text_entry,
                                    OBJECT_COLUMN, new_task,
                                    -1);
                gtk_entry_set_text (entry, "");
        }
}

static void
hzl_application_window_show_tasks_list_cb (GObject *source_object, GAsyncResult *async_result, gpointer user_data)
{
        GError *error = NULL;
        GomResourceGroup *resource_group;
        HzlApplicationWindow *self;
        guint tasks_count;
        guint i;

        self = HZL_APPLICATION_WINDOW (user_data);
        resource_group = gom_repository_find_finish (GOM_REPOSITORY (source_object), async_result, &error);
        if (error != NULL) {
                g_warning ("Error getting the tasks: (%d) %s\n", error->code, error->message);
                return;
        }

        tasks_count = gom_resource_group_get_count (resource_group);
        gom_resource_group_fetch_sync (resource_group, 0, tasks_count, &error);
        if (error != NULL) {
                g_object_unref (resource_group);
                g_warning ("Error fetching all tasks in list: %s\n", error->message);
                return;
        }
        for (i = 0; i < tasks_count; i++) {
                GtkTreeIter iter;
                HzlTask *task;

                task = HZL_TASK (gom_resource_group_get_index (resource_group, i));
                if (HZL_IS_TASK (task)) {
                        gtk_list_store_append (self->priv->tasks_store, &iter);
                        gtk_list_store_set (self->priv->tasks_store, &iter,
                                            DONE_COLUMN, hzl_task_is_completed (task),
                                            TEXT_COLUMN, hzl_task_get_text (task),
                                            OBJECT_COLUMN, task,
                                            -1);
                }
        }

        g_object_unref (resource_group);
}

static void
hzl_application_window_task_done_cb (GtkCellRendererToggle *cell_renderer,
                                     gchar                 *path_string,
                                     gpointer               user_data)
{
        GtkTreeIter iter;
        HzlApplicationWindow *self = HZL_APPLICATION_WINDOW (user_data);
        GValue object_value = { 0, };
        HzlTask *task;
        GError *error = NULL;

        gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (self->priv->tasks_store), &iter, path_string);
        gtk_tree_model_get_value (GTK_TREE_MODEL (self->priv->tasks_store), &iter, OBJECT_COLUMN, &object_value);
        task = HZL_TASK (g_value_get_object (&object_value));
        if (hzl_task_is_completed (task))
                hzl_task_unmark_completed (task);
        else
                hzl_task_mark_completed_now (task);
        gom_resource_save_sync (GOM_RESOURCE (task), &error);
        if (error == NULL)
                gtk_list_store_set (self->priv->tasks_store, &iter,
                                    DONE_COLUMN, hzl_task_is_completed (task),
                                    -1);
        else
                g_warning ("Error chaning task %s completiong status: %s\n",
                           hzl_task_get_uuid (task),
                           error->message);

        g_value_unset (&object_value);
}

GtkWidget*
hzl_application_window_new (GtkApplication *application)
{
        return GTK_WIDGET (g_object_new (HZL_TYPE_APPLICATION_WINDOW,
                                         "application", application,
                                         "title", _("Hazlo"),
                                         "window-position", GTK_WIN_POS_CENTER,
                                         NULL));
}

void
hzl_application_window_show_tasks_list (HzlApplicationWindow *self, HzlTasksList *list)
{
        GomFilter *filter;
        GValue value = { 0, };
        GomRepository *repository;

        g_return_if_fail (HZL_IS_APPLICATION_WINDOW (self));
        g_return_if_fail (HZL_IS_TASKS_LIST (list));

        g_object_get (G_OBJECT (list), "repository", &repository, NULL);
        g_return_if_fail (GOM_IS_REPOSITORY (repository));

        gtk_list_store_clear (self->priv->tasks_store);

        g_value_init (&value, G_TYPE_STRING);
        g_value_set_string (&value, hzl_tasks_list_get_uuid (list));
        filter = gom_filter_new_eq (HZL_TYPE_TASK, "list_uuid", &value);
        g_value_unset (&value);

        if (self->priv->current_list)
                g_object_unref (self->priv->current_list);
        self->priv->current_list = list;
        g_object_ref (self->priv->current_list);

        gom_repository_find_async (repository, HZL_TYPE_TASK, filter, hzl_application_window_show_tasks_list_cb, self);
        g_object_unref (filter);
}
