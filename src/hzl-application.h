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

#ifndef __HZL_APPLICATION_H__
#define __HZL_APPLICATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HZL_TYPE_APPLICATION             (hzl_application_get_type())
#define HZL_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),HZL_TYPE_APPLICATION,HzlApplication))
#define HZL_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),HZL_TYPE_APPLICATION,HzlApplicationClass))
#define HZL_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),HZL_TYPE_APPLICATION))
#define HZL_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),HZL_TYPE_APPLICATION))
#define HZL_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),HZL_TYPE_APPLICATION,HzlApplicationClass))

typedef struct _HzlApplication        HzlApplication;
typedef struct _HzlApplicationClass   HzlApplicationClass;
typedef struct _HzlApplicationPrivate HzlApplicationPrivate;

struct _HzlApplication {
        GtkApplication parent;
        HzlApplicationPrivate *priv;
};

struct _HzlApplicationClass {
        GtkApplicationClass parent_class;
};

GType hzl_application_get_type (void) G_GNUC_CONST;

GtkApplication* hzl_application_new (void);

G_END_DECLS

#endif /* __HZL_APPLICATION_H__ */
