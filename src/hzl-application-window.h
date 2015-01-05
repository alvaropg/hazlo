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

#ifndef __HZL_APPLICATION_WINDOW_H__
#define __HZL_APPLICATION_WINDOW_H__

#include <gtk/gtk.h>

#include "hzl-tasks-list.h"

G_BEGIN_DECLS

#define HZL_TYPE_APPLICATION_WINDOW             (hzl_application_window_get_type())
#define HZL_APPLICATION_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),HZL_TYPE_APPLICATION_WINDOW,HzlApplicationWindow))
#define HZL_APPLICATION_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),HZL_TYPE_APPLICATION_WINDOW,HzlApplicationWindowClass))
#define HZL_IS_APPLICATION_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),HZL_TYPE_APPLICATION_WINDOW))
#define HZL_IS_APPLICATION_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),HZL_TYPE_APPLICATION_WINDOW))
#define HZL_APPLICATION_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),HZL_TYPE_APPLICATION_WINDOW,HzlApplicationWindowClass))

typedef struct _HzlApplicationWindow        HzlApplicationWindow;
typedef struct _HzlApplicationWindowClass   HzlApplicationWindowClass;
typedef struct _HzlApplicationWindowPrivate HzlApplicationWindowPrivate;

struct _HzlApplicationWindow {
        GtkApplicationWindow parent;
        HzlApplicationWindowPrivate *priv;
};

struct _HzlApplicationWindowClass {
        GtkApplicationWindowClass parent_class;
};

GType        hzl_application_window_get_type  (void) G_GNUC_CONST;

GtkWidget*   hzl_application_window_new       (GtkApplication *application);

void         hzl_application_window_show_tasks_list (HzlApplicationWindow *self, HzlTasksList *list);

G_END_DECLS

#endif /* __HZL_APPLICATION_WINDOW_H__ */
