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

#include <config.h>

#include "hzl-application.h"
#include "hzl-application-window.h"
#include "hzl-utils.h"

static void hzl_application_constructed (GObject *object);
static void hzl_application_dispose     (GObject *object);
static void hzl_application_activate    (GApplication *application);
static void hzl_application_startup     (GApplication *application);

struct _HzlApplicationPrivate {
        GtkWidget *window;
        guint32 activation_timestamp;
        GResource *resource;
};

G_DEFINE_TYPE_WITH_CODE (HzlApplication, hzl_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE (HzlApplication));

static void
hzl_application_class_init (HzlApplicationClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        object_class->constructed = hzl_application_constructed;
        object_class->dispose = hzl_application_dispose;

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
}

static void
hzl_application_constructed (GObject *object)
{
        g_application_set_application_id (G_APPLICATION (object), "org.gnome." PACKAGE_NAME);

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
hzl_application_activate (GApplication *application)
{
        HzlApplication *self = HZL_APPLICATION (application);

        if (self->priv->window == NULL) {
                self->priv->window = hzl_application_window_new (GTK_APPLICATION (self));
        }

        gtk_window_present_with_time (GTK_WINDOW (self->priv->window), self->priv->activation_timestamp);
        self->priv->activation_timestamp = GDK_CURRENT_TIME;

        /* Perhaps this would be useful */
        /* hzl_application_populate_inbox (self); */
}

static void
hzl_application_startup (GApplication *application)
{
        GtkBuilder *builder;

        G_APPLICATION_CLASS (hzl_application_parent_class)->startup (application);

        builder = gtk_builder_new_from_resource ("/org/gnome/hazlo/app-menu.ui");
        gtk_application_set_app_menu (GTK_APPLICATION (application), G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu")));
}

GtkApplication*
hzl_application_new (void)
{
        return GTK_APPLICATION (g_object_new (HZL_TYPE_APPLICATION,
                                              NULL));
}
