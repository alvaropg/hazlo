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

#include "hzl-task-list-row.h"

struct _HzlTaskListRowPrivate {
        HzlTask *task;

        GtkWidget *done_check;
        GtkWidget *text_label;
        GtkWidget *due_label;
        GtkWidget *note_image;
};

static void hzl_task_list_row_set_property (GObject      *object,
                                                guint         property_id,
                                                const GValue *value,
                                                GParamSpec   *pspec);
static void hzl_task_list_row_get_property (GObject      *object,
                                                guint         property_id,
                                                GValue       *value,
                                                GParamSpec   *pspec);
static void hzl_task_list_row_dispose      (GObject *gobject);
static void hzl_task_list_row_finalize     (GObject *gobject);

static void hzl_task_list_row_bind_task    (HzlTaskListRow *self);

enum
{
        PROP_0,

        HZL_TASK_LIST_ROW_PROP_TASK,

        N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (HzlTaskListRow, hzl_task_list_row, GTK_TYPE_LIST_BOX_ROW)


static void
hzl_task_list_row_class_init (HzlTaskListRowClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->dispose = hzl_task_list_row_dispose;
        gobject_class->finalize = hzl_task_list_row_finalize;
        gobject_class->set_property = hzl_task_list_row_set_property;
        gobject_class->get_property = hzl_task_list_row_get_property;

        obj_properties[HZL_TASK_LIST_ROW_PROP_TASK] = g_param_spec_object ("task",
                                                                           "Task",
                                                                           "The HzlTask",
                                                                           HZL_TYPE_TASK,
                                                                           G_PARAM_READWRITE);

        g_object_class_install_properties (gobject_class,
                                           N_PROPERTIES,
                                           obj_properties);

        gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gnome/hazlo/task-list-row.ui");
        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), HzlTaskListRow, done_check);
        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), HzlTaskListRow, text_label);
        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), HzlTaskListRow, due_label);
        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), HzlTaskListRow, note_image);
}

static void
hzl_task_list_row_init (HzlTaskListRow *self)
{
        self->priv = hzl_task_list_row_get_instance_private (self);

        gtk_widget_init_template (GTK_WIDGET (self));
}

static void
hzl_task_list_row_set_property (GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
        HzlTaskListRow *self = HZL_TASK_LIST_ROW (object);

        switch (property_id) {
        case HZL_TASK_LIST_ROW_PROP_TASK:
                g_clear_object (&self->priv->task);
                self->priv->task = g_value_get_object (value);
                g_object_ref (self->priv->task);
                hzl_task_list_row_bind_task (self);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        }
}

static void
hzl_task_list_row_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
        HzlTaskListRow *self = HZL_TASK_LIST_ROW (object);

        switch (property_id) {
        case HZL_TASK_LIST_ROW_PROP_TASK:
                g_value_set_object (value, self->priv->task);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        }

}

static void
hzl_task_list_row_dispose (GObject *gobject)
{
        HzlTaskListRow *self = HZL_TASK_LIST_ROW (gobject);

        g_clear_object (&self->priv->task);

        G_OBJECT_CLASS (hzl_task_list_row_parent_class)->dispose (gobject);
}

static void
hzl_task_list_row_finalize (GObject *gobject)
{
        HzlTaskListRow *self = HZL_TASK_LIST_ROW (gobject);

        G_OBJECT_CLASS (hzl_task_list_row_parent_class)->finalize (gobject);
}

static void
hzl_task_list_row_bind_task (HzlTaskListRow *self)
{
        gtk_label_set_text (GTK_LABEL (self->priv->text_label), hzl_task_get_text (self->priv->task));
        g_object_bind_property (self->priv->task, "text",
                                self->priv->text_label, "label",
                                G_BINDING_BIDIRECTIONAL);
}

HzlTaskListRow*
hzl_task_list_row_new (HzlTask *task)
{
        g_return_val_if_fail (HZL_IS_TASK (task), NULL);

        return HZL_TASK_LIST_ROW (g_object_new (HZL_TYPE_TASK_LIST_ROW,
                                                "task", task,
                                                NULL));
}

void
hzl_task_list_row_set_task (HzlTaskListRow *self, HzlTask *task)
{
        g_return_if_fail (HZL_IS_TASK_LIST_ROW (self));

        g_object_set (HZL_TASK_LIST_ROW (self),
                      "task", task,
                      NULL);
}

const HzlTask*
hzl_task_list_row_get_task (HzlTaskListRow *self)
{
        g_return_val_if_fail (HZL_IS_TASK_LIST_ROW (self), NULL);

        return self->priv->task;
}
