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

#include "hzl-application.h"
#include "hzl-application-window.h"

static void hzl_application_activate   (GApplication         *application);
static void hzl_application_startup   (GApplication         *application);

struct _HzlApplicationPrivate {
        GtkWidget *window;
        guint32 activation_timestamp;
};

G_DEFINE_TYPE_WITH_CODE (HzlApplication, hzl_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE (HzlApplication));

static void
hzl_application_class_init (HzlApplicationClass *klass)
{
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        application_class->activate = hzl_application_activate;
        application_class->startup = hzl_application_startup;
}

static void
hzl_application_init (HzlApplication *self)
{
        HzlApplicationPrivate *priv = hzl_application_get_instance_private(self);

        self->priv = priv;

        priv->window = NULL;
        priv->activation_timestamp = GDK_CURRENT_TIME;
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
        G_APPLICATION_CLASS (hzl_application_parent_class)->startup (application);
}

GtkApplication*
hzl_application_new (void)
{
        return GTK_APPLICATION (g_object_new (HZL_TYPE_APPLICATION, NULL));
}
