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

static void hzl_application_window_class_init (HzlApplicationWindowClass *klass);
static void hzl_application_window_init       (HzlApplicationWindow *self);

struct _HzlApplicationWindowPrivate {
        GtkWidget *header_bar;
        GtkWidget *stack;
};

G_DEFINE_TYPE_WITH_PRIVATE (HzlApplicationWindow, hzl_application_window, GTK_TYPE_APPLICATION_WINDOW);

static void
hzl_application_window_class_init (__attribute__ ((unused)) HzlApplicationWindowClass *klass)
{
}

static void
hzl_application_window_init (HzlApplicationWindow *self)
{
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
}

GtkWidget*
hzl_application_window_new (GtkApplication *application)
{
        return GTK_WIDGET (g_object_new (HZL_TYPE_APPLICATION_WINDOW,
                                         "application", application,
                                         "title", _(PACKAGE_NAME),
                                         "window-position", GTK_WIN_POS_CENTER,
                                         NULL));
}
