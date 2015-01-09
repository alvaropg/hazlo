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

#ifndef __HZL_TASK_LIST_ROW_H__
#define __HZL_TASK_LIST_ROW_H__

#include <gtk/gtk.h>

#include "hzl-task.h"

G_BEGIN_DECLS

#define HZL_TYPE_TASK_LIST_ROW             (hzl_task_list_row_get_type())
#define HZL_TASK_LIST_ROW(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),HZL_TYPE_TASK_LIST_ROW,HzlTaskListRow))
#define HZL_TASK_LIST_ROW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),HZL_TYPE_TASK_LIST_ROW,HzlTaskListRowClass))
#define HZL_IS_TASK_LIST_ROW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),HZL_TYPE_TASK_LIST_ROW))
#define HZL_IS_TASK_LIST_ROW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),HZL_TYPE_TASK_LIST_ROW))
#define HZL_TASK_LIST_ROW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),HZL_TYPE_TASK_LIST_ROW,HzlTaskListRowClass))

typedef struct _HzlTaskListRow        HzlTaskListRow;
typedef struct _HzlTaskListRowClass   HzlTaskListRowClass;
typedef struct _HzlTaskListRowPrivate HzlTaskListRowPrivate;

struct _HzlTaskListRow {
        GtkListBoxRow parent;

        HzlTaskListRowPrivate *priv;
};

struct _HzlTaskListRowClass {
        GtkListBoxRowClass parent_class;
};

GType hzl_task_list_row_get_type (void) G_GNUC_CONST;

HzlTaskListRow* hzl_task_list_row_new      (HzlTask *task);
void            hzl_task_list_row_set_task (HzlTaskListRow *self, HzlTask *task);
const HzlTask*  hzl_task_list_row_get_task (HzlTaskListRow *self);

G_END_DECLS

#endif /* __HZL_TASK_LIST_ROW_H__ */
