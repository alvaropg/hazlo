/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Hzl - Photos selection application for GNOME
 * Copyright © 2014 Álvaro Peña <alvaropg@gmail.com>
 *
 * APP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * APP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with APP. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HZL_TASK_H__
#define __HZL_TASK_H__

#include <gom/gom.h>

G_BEGIN_DECLS

#define HZL_TYPE_TASK             (hzl_task_get_type())
#define HZL_TASK(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),HZL_TYPE_TASK,HzlTask))
#define HZL_TASK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),HZL_TYPE_TASK,HzlTaskClass))
#define HZL_IS_TASK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),HZL_TYPE_TASK))
#define HZL_IS_TASK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),HZL_TYPE_TASK))
#define HZL_TASK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),HZL_TYPE_TASK,HzlTaskClass))

typedef struct _HzlTask        HzlTask;
typedef struct _HzlTaskClass   HzlTaskClass;
typedef struct _HzlTaskPrivate HzlTaskPrivate;

struct _HzlTask {
        GomResource parent;

        HzlTaskPrivate *priv;
};

struct _HzlTaskClass {
        GomResourceClass parent_class;
};

GType hzl_task_get_type (void) G_GNUC_CONST;

HzlTask*     hzl_task_new (gchar *text);
void         hzl_task_set_text (HzlTask *self, gchar *text);
const gchar* hzl_task_get_text (HzlTask *self);

G_END_DECLS

#endif /* __HZL_TASK_H__ */
