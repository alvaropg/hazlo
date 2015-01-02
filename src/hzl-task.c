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

#include <uuid.h>

#include "hzl-task.h"

struct _HzlTaskPrivate {
        gchar *uuid;
        gchar *list_uuid;
        gchar *text;
};

static void hzl_task_set_property (GObject      *gobject,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec);
static void hzl_task_get_property (GObject      *gobject,
                                   guint         property_id,
                                   GValue       *value,
                                   GParamSpec   *pspec);
static void hzl_task_dispose      (GObject *gobject);
static void hzl_task_finalize     (GObject *gobject);

enum
{
        PROP_0,

        HZL_TASK_PROP_UUID,
        HZL_TASK_PROP_LIST_UUID,
        HZL_TASK_PROP_TEXT,

        N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (HzlTask, hzl_task, GOM_TYPE_RESOURCE)


static void
hzl_task_class_init (HzlTaskClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        GomResourceClass *resource_class;

        gobject_class->dispose = hzl_task_dispose;
        gobject_class->finalize = hzl_task_finalize;
        gobject_class->set_property = hzl_task_set_property;
        gobject_class->get_property = hzl_task_get_property;

        obj_properties[HZL_TASK_PROP_UUID] = g_param_spec_string ("uuid",
                                                                  "UUID",
                                                                  "Task Universal Unique Identifier",
                                                                  NULL,
                                                                  G_PARAM_READABLE);

        obj_properties[HZL_TASK_PROP_LIST_UUID] = g_param_spec_string ("list_uuid",
                                                                       "List UUID",
                                                                       "List Universal Unique Identifier which the task is into",
                                                                       "",
                                                                       G_PARAM_READWRITE);

        obj_properties[HZL_TASK_PROP_TEXT] = g_param_spec_string ("text",
                                                                  "Text",
                                                                  "The task text content",
                                                                  "",
                                                                  G_PARAM_READWRITE);

        g_object_class_install_properties (gobject_class,
                                           N_PROPERTIES,
                                           obj_properties);

        resource_class = GOM_RESOURCE_CLASS (klass);
        gom_resource_class_set_table (resource_class, "tasks");
        gom_resource_class_set_primary_key (resource_class, "uuid");
}

static void
hzl_task_init (HzlTask *self)
{
        uuid_t uuid;

        self->priv = hzl_task_get_instance_private (self);

        uuid_generate (uuid);
        self->priv->uuid = g_new0(gchar, 36);
        uuid_unparse (uuid, self->priv->uuid);
}

static void
hzl_task_set_property (GObject      *gobject,
                       guint         property_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
        HzlTask *self = HZL_TASK (gobject);

        switch (property_id) {
        case HZL_TASK_PROP_UUID:
                if (self->priv->uuid)
                        g_free (self->priv->uuid);
                self->priv->uuid = g_value_dup_string (value);
                break;
        case HZL_TASK_PROP_LIST_UUID:
                if (self->priv->list_uuid)
                        g_free (self->priv->list_uuid);
                self->priv->list_uuid = g_value_dup_string (value);
                break;
        case HZL_TASK_PROP_TEXT:
                if (self->priv->text)
                        g_free (self->priv->text);
                self->priv->text = g_value_dup_string (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        }
}

static void
hzl_task_get_property (GObject    *gobject,
                       guint       property_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
        HzlTask *self = HZL_TASK (gobject);

        switch (property_id) {
        case HZL_TASK_PROP_UUID:
                g_value_set_string (value, self->priv->uuid);
                break;
        case HZL_TASK_PROP_LIST_UUID:
                g_value_set_string (value, self->priv->list_uuid);
                break;                
        case HZL_TASK_PROP_TEXT:
                g_value_set_string (value, self->priv->text);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        }

}

static void
hzl_task_dispose (GObject *gobject)
{
        HzlTask *self = HZL_TASK (gobject);

        G_OBJECT_CLASS (hzl_task_parent_class)->dispose (gobject);
}

static void
hzl_task_finalize (GObject *gobject)
{
        HzlTask *self = HZL_TASK (gobject);

        g_clear_pointer (&self->priv->uuid, g_free);
        g_clear_pointer (&self->priv->list_uuid, g_free);
        g_clear_pointer (&self->priv->text, g_free);

        G_OBJECT_CLASS (hzl_task_parent_class)->finalize (gobject);
}

HzlTask*
hzl_task_new (const gchar *text)
{
        HzlTask *task;

        task = HZL_TASK (g_object_new (HZL_TYPE_TASK,
                                       "text", text,
                                       NULL));

        return task;
}

void
hzl_task_set_text (HzlTask *self, const gchar *text)
{
        g_return_if_fail (HZL_IS_TASK (self));

        g_object_set (G_TASK (self),
                      "text", text,
                      NULL);
}

const gchar*
hzl_task_get_text (HzlTask *self)
{
        g_return_val_if_fail (HZL_IS_TASK (self), NULL);

        return self->priv->text;
}
