/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Hazlo
 * Copyright © 2015 Álvaro Peña <alvaropg@gmail.com>
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

#ifndef __HZL_TASKS_LIST_H__
#define __HZL_TASKS_LIST_H__

#include <gom/gom.h>

G_BEGIN_DECLS

#define HZL_TYPE_TASKS_LIST             (hzl_tasks_list_get_type())
#define HZL_TASKS_LIST(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),HZL_TYPE_TASKS_LIST,HzlTasksList))
#define HZL_TASKS_LIST_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),HZL_TYPE_TASKS_LIST,HzlTasksListClass))
#define HZL_IS_TASKS_LIST(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),HZL_TYPE_TASKS_LIST))
#define HZL_IS_TASKS_LIST_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),HZL_TYPE_TASKS_LIST))
#define HZL_TASKS_LIST_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),HZL_TYPE_TASKS_LIST,HzlTasksListClass))

typedef struct _HzlTasksList        HzlTasksList;
typedef struct _HzlTasksListClass   HzlTasksListClass;
typedef struct _HzlTasksListPrivate HzlTasksListPrivate;

struct _HzlTasksList {
        GomResource parent;

        HzlTasksListPrivate *priv;
};

struct _HzlTasksListClass {
        GomResourceClass parent_class;
};

GType hzl_tasks_list_get_type (void) G_GNUC_CONST;

HzlTasksList* hzl_tasks_list_new (const gchar *name);
void          hzl_tasks_list_set_name (HzlTasksList *self, const gchar *name);
const gchar*  hzl_tasks_list_get_name (HzlTasksList *self);

G_END_DECLS

#endif /* __HZL_TASKS_LIST_H__ */
