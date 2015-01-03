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
        GResource *resource;
        GomAdapter *adapter;
        gchar *db_uri;
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
        self->priv->resource = g_resource_load (hzl_utils_get_resource_filename (), &error);
        if (error == NULL) {
                g_resources_register (self->priv->resource);
        } else {
                g_error (_("Error loading resources file: %s"), error->message);
        }
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

        if (self->priv->resource) {
                g_resources_unregister (self->priv->resource);
                g_resource_unref (self->priv->resource);
                self->priv->resource = NULL;
        }

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
        GtkBuilder *builder;

        G_APPLICATION_CLASS (hzl_application_parent_class)->startup (application);

        g_action_map_add_action_entries (G_ACTION_MAP (application),
                                         app_entries, G_N_ELEMENTS (app_entries),
                                         application);

        builder = gtk_builder_new_from_resource ("/org/gnome/hazlo/app-menu.ui");
        gtk_application_set_app_menu (GTK_APPLICATION (application), G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu")));
        g_object_unref (builder);
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
        gboolean finish_result;
        HzlApplication *self = HZL_APPLICATION (user_data);
        GError *error = NULL;

        finish_result = gom_adapter_open_finish (self->priv->adapter, async_result, &error);
        if (finish_result == FALSE) {
                g_error ("Error opening connection with database: %s\n", error->message);
        }
}

static void
hzl_application_adapter_async_close_cb (__attribute__ ((unused)) GObject *source_object,
                                        GAsyncResult *async_result,
                                        gpointer user_data)
{
        HzlApplication *self = HZL_APPLICATION (user_data);
        gboolean finish_result;
        GError *error = NULL;

        finish_result = gom_adapter_close_finish (self->priv->adapter, async_result, &error);
        if (finish_result == FALSE) {
                g_warning ("Error closing database: %s\n", error->message);
        }

        g_application_release (G_APPLICATION (self));
}

GtkApplication*
hzl_application_new (void)
{
        return GTK_APPLICATION (g_object_new (HZL_TYPE_APPLICATION, NULL));
}
