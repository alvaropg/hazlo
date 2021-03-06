/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Hzl - Photos selection application for GNOME
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

#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gi18n.h>
#include <gom/gom.h>

#include <config.h>

#include "hzl-application.h"
#include "hzl-application-window.h"
#include "hzl-utils.h"

/* Hazlo resources */
#include "hzl-tasks-list.h"
#include "hzl-task.h"

static void hzl_application_constructed (GObject *object);
static void hzl_application_dispose     (GObject *object);
static void hzl_application_finalize    (GObject *object);
static void hzl_application_activate    (GApplication *application);
static void hzl_application_startup     (GApplication *application);

static void hzl_application_quit_activated (GSimpleAction *action,
                                            GVariant      *parameter,
                                            gpointer       user_data);

static void hzl_application_window_destroyed_cb (GtkWidget *window,
                                                 gpointer   user_data);

static void hzl_application_adapter_async_open_cb  (GObject      *source_object,
                                                    GAsyncResult *async_result,
                                                    gpointer      user_data);
static void hzl_application_adapter_async_close_cb (GObject      *source_object,
                                                    GAsyncResult *async_result,
                                                    gpointer      user_data);

struct _HzlApplicationPrivate {
        GtkWidget *window;
        guint32 activation_timestamp;
        GomAdapter *adapter;
        gchar *db_uri;
        GomRepository *repository;
        HzlTasksList *inbox_list;
};

G_DEFINE_TYPE_WITH_CODE (HzlApplication, hzl_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE (HzlApplication));

static GActionEntry app_entries[] =
{
        { "quit", hzl_application_quit_activated, NULL, NULL, NULL }
};

static void
hzl_application_class_init (HzlApplicationClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        object_class->constructed = hzl_application_constructed;
        object_class->dispose = hzl_application_dispose;
        object_class->finalize = hzl_application_finalize;

        application_class->activate = hzl_application_activate;
        application_class->startup = hzl_application_startup;
}

static void
hzl_application_init (HzlApplication *self)
{
        GError *error = NULL;

        self->priv = hzl_application_get_instance_private(self);

        self->priv->window = NULL;
        self->priv->activation_timestamp = GDK_CURRENT_TIME;
        self->priv->adapter = NULL;
        self->priv->db_uri = g_build_filename (g_get_user_data_dir (), "hazlo.db", NULL);;
}

static void
hzl_application_constructed (GObject *object)
{
        g_application_set_application_id (G_APPLICATION (object), "org.gnome." PACKAGE_NAME);
        g_set_prgname ("Hazlo");
        g_set_application_name ("Hazlo");

        G_OBJECT_CLASS (hzl_application_parent_class)->constructed (object);
}

static void
hzl_application_dispose (GObject *object)
{
        HzlApplication *self = HZL_APPLICATION (object);

        g_clear_object (&self->priv->repository);
        g_clear_object (&self->priv->adapter);

        G_OBJECT_CLASS (hzl_application_parent_class)->dispose (object);
}

static void
hzl_application_finalize (GObject *object)
{
        HzlApplication *self = HZL_APPLICATION (object);

        g_clear_pointer (&self->priv->db_uri, g_free);

        G_OBJECT_CLASS (hzl_application_parent_class)->finalize (object);
}

static void
hzl_application_activate (GApplication *application)
{
        HzlApplication *self = HZL_APPLICATION (application);

        if (self->priv->adapter == NULL) {
                self->priv->adapter = gom_adapter_new ();
                gom_adapter_open_async (self->priv->adapter, self->priv->db_uri, hzl_application_adapter_async_open_cb, self);
        }

        if (self->priv->window == NULL) {
                self->priv->window = hzl_application_window_new (GTK_APPLICATION (self));
                g_signal_connect (G_OBJECT (self->priv->window), "destroy", G_CALLBACK (hzl_application_window_destroyed_cb), self);
        }

        gtk_window_present_with_time (GTK_WINDOW (self->priv->window), self->priv->activation_timestamp);
        self->priv->activation_timestamp = GDK_CURRENT_TIME;
}

static void
hzl_application_startup (GApplication *application)
{
        GtkCssProvider *style_provider;

        G_APPLICATION_CLASS (hzl_application_parent_class)->startup (application);

        g_action_map_add_action_entries (G_ACTION_MAP (application),
                                         app_entries, G_N_ELEMENTS (app_entries),
                                         application);

        style_provider = gtk_css_provider_new ();
        gtk_css_provider_load_from_resource (style_provider, "/org/gnome/hazlo/css/hazlo.css");
        gtk_style_context_add_provider_for_screen (gdk_screen_get_default (), GTK_STYLE_PROVIDER (style_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_object_unref (style_provider);
}

static void
hzl_application_quit_activated (__attribute__ ((unused)) GSimpleAction *action,
                                __attribute__ ((unused)) GVariant      *parameter,
                                gpointer       user_data)
{
        gtk_widget_destroy (HZL_APPLICATION (user_data)->priv->window);
}

static void
hzl_application_window_destroyed_cb (__attribute__ ((unused)) GtkWidget *window,
                                     gpointer user_data)
{
        HzlApplication *self = HZL_APPLICATION (user_data);

        g_application_hold (G_APPLICATION (self));
        gom_adapter_close_async (self->priv->adapter, hzl_application_adapter_async_close_cb, self);
}

static void
hzl_application_adapter_async_open_cb (__attribute__ ((unused)) GObject *source_object,
                                       GAsyncResult *async_result,
                                       gpointer user_data)
{
        HzlApplication *self = HZL_APPLICATION (user_data);
        GError *error = NULL;
        GList *object_types = NULL;
        GomFilter *filter;
        GValue value = { 0, };
        gchar *inbox_name;

        gom_adapter_open_finish (self->priv->adapter, async_result, &error);
        if (error != NULL) {
                g_error ("Error opening connection with database: %s\n", error->message);
        }

        /* 1. Ensure tables */
        self->priv->repository = gom_repository_new (self->priv->adapter);

        object_types = g_list_append (object_types, GINT_TO_POINTER (HZL_TYPE_TASKS_LIST));
        object_types = g_list_append (object_types, GINT_TO_POINTER (HZL_TYPE_TASK));
        gom_repository_automatic_migrate_sync (self->priv->repository, 1, object_types, &error);
        if (error != NULL) {
                g_error ("Error populating DB resources: %s\n", error->message);
        }

        /* 2. Getting the "Inbox" list */
        g_value_init (&value, G_TYPE_STRING);
        /* Translators: this is the default tasks list name. This list is created by default and it can not be removed. */
        inbox_name = g_strdup ("hazlo-inbox");
        g_value_set_string (&value, inbox_name);
        filter = gom_filter_new_eq (HZL_TYPE_TASKS_LIST, "uuid", &value);
        g_value_unset (&value);
        self->priv->inbox_list = HZL_TASKS_LIST (gom_repository_find_one_sync (self->priv->repository,
                                                                   HZL_TYPE_TASKS_LIST,
                                                                   filter,
                                                                   &error));
        /* At this moment (3 Jan 2015, GOM version 0.2.1) the "not found error" is the number 3 */
        if (error != NULL) {
                HzlTask *start_task;

                if (error->code != 3)
                        g_error ("Error finding 'Inbox' tasks list: (%d) %s\n", error->code, error->message);

                /* Create the Inbox list */
                g_clear_error (&error);
                self->priv->inbox_list = g_object_new (HZL_TYPE_TASKS_LIST,
                                                       "repository", self->priv->repository,
                                                       "uuid", "hazlo-inbox",
                                                       "name", inbox_name,
                                                       NULL);
                gom_resource_save_sync (GOM_RESOURCE (self->priv->inbox_list), &error);
                if (error != NULL)
                        g_error ("Error creating default 'Inbox' tasks list: %s\n", error->message);

                start_task = g_object_new (HZL_TYPE_TASK,
                                           "repository", self->priv->repository,
                                           "text", _("This is your first task!"),
                                           "list_uuid", hzl_tasks_list_get_uuid (self->priv->inbox_list),
                                           NULL);
                gom_resource_save_sync (GOM_RESOURCE (start_task), &error);
                if (error != NULL)
                        g_warning ("Error creating default task: %s\n", error->message);
                g_clear_error (&error);
        }

        /* 3. populate "main" list content */
        hzl_application_window_show_tasks_list (HZL_APPLICATION_WINDOW (self->priv->window), self->priv->inbox_list);

        g_clear_pointer (&inbox_name, g_free);
        g_object_unref (filter);
}

static void
hzl_application_adapter_async_close_cb (__attribute__ ((unused)) GObject *source_object,
                                        GAsyncResult *async_result,
                                        gpointer user_data)
{
        HzlApplication *self = HZL_APPLICATION (user_data);
        GError *error = NULL;

        gom_adapter_close_finish (self->priv->adapter, async_result, &error);
        if (error != NULL) {
                g_warning ("Error closing database: %s\n", error->message);
        }

        g_application_release (G_APPLICATION (self));
}

GtkApplication*
hzl_application_new (void)
{
        return GTK_APPLICATION (g_object_new (HZL_TYPE_APPLICATION, NULL));
}
