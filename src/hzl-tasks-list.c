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

#include <uuid.h>

#include "hzl-tasks-list.h"

struct _HzlTasksListPrivate {
        gchar *uuid;
        gchar *name;
};

static void hzl_tasks_list_set_property (GObject      *gobject,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec);
static void hzl_tasks_list_get_property (GObject      *gobject,
                                   guint         property_id,
                                   GValue       *value,
                                   GParamSpec   *pspec);
static void hzl_tasks_list_dispose      (GObject *gobject);
static void hzl_tasks_list_finalize     (GObject *gobject);

enum
{
        PROP_0,

        HZL_TASKS_LIST_PROP_UUID,
        HZL_TASKS_LIST_PROP_NAME,

        N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (HzlTasksList, hzl_tasks_list, GOM_TYPE_RESOURCE)


static void
hzl_tasks_list_class_init (HzlTasksListClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        GomResourceClass *resource_class;

        gobject_class->dispose = hzl_tasks_list_dispose;
        gobject_class->finalize = hzl_tasks_list_finalize;
        gobject_class->set_property = hzl_tasks_list_set_property;
        gobject_class->get_property = hzl_tasks_list_get_property;

        obj_properties[HZL_TASKS_LIST_PROP_UUID] = g_param_spec_string ("uuid",
                                                                              "UUID",
                                                                              "Task Universal Unique Identifier",
                                                                              NULL,
                                                                              G_PARAM_READWRITE);

        obj_properties[HZL_TASKS_LIST_PROP_NAME] = g_param_spec_string ("name",
                                                                              "Name",
                                                                              "The tasks list name",
                                                                              "",
                                                                              G_PARAM_READWRITE);

        g_object_class_install_properties (gobject_class,
                                           N_PROPERTIES,
                                           obj_properties);

        resource_class = GOM_RESOURCE_CLASS (klass);
        gom_resource_class_set_table (resource_class, "tasks_list");
        gom_resource_class_set_primary_key (resource_class, "uuid");
}

static void
hzl_tasks_list_init (HzlTasksList *self)
{
        uuid_t uuid;

        self->priv = hzl_tasks_list_get_instance_private (self);

        uuid_generate (uuid);
        self->priv->uuid = g_new0(gchar, 36);
        uuid_unparse (uuid, self->priv->uuid);
}

static void
hzl_tasks_list_set_property (GObject      *gobject,
                       guint         property_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
        HzlTasksList *self = HZL_TASKS_LIST (gobject);

        switch (property_id) {
        case HZL_TASKS_LIST_PROP_UUID:
                if (self->priv->uuid)
                        g_free (self->priv->uuid);
                self->priv->uuid = g_value_dup_string (value);
                break;
        case HZL_TASKS_LIST_PROP_NAME:
                if (self->priv->name)
                        g_free (self->priv->name);
                self->priv->name = g_value_dup_string (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        }
}

static void
hzl_tasks_list_get_property (GObject    *gobject,
                       guint       property_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
        HzlTasksList *self = HZL_TASKS_LIST (gobject);

        switch (property_id) {
        case HZL_TASKS_LIST_PROP_UUID:
                g_value_set_string (value, self->priv->uuid);
                break;
        case HZL_TASKS_LIST_PROP_NAME:
                g_value_set_string (value, self->priv->name);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        }

}

static void
hzl_tasks_list_dispose (GObject *gobject)
{
        HzlTasksList *self = HZL_TASKS_LIST (gobject);

        G_OBJECT_CLASS (hzl_tasks_list_parent_class)->dispose (gobject);
}

static void
hzl_tasks_list_finalize (GObject *gobject)
{
        HzlTasksList *self = HZL_TASKS_LIST (gobject);

        g_clear_pointer (&self->priv->uuid, g_free);
        g_clear_pointer (&self->priv->name, g_free);

        G_OBJECT_CLASS (hzl_tasks_list_parent_class)->finalize (gobject);
}

HzlTasksList*
hzl_tasks_list_new (const gchar *name)
{
        HzlTasksList *task;

        task = HZL_TASKS_LIST (g_object_new (HZL_TYPE_TASKS_LIST,
                                             "name", name,
                                             NULL));

        return task;
}

void
hzl_tasks_list_set_name (HzlTasksList *self, const gchar *name)
{
        g_return_if_fail (HZL_IS_TASKS_LIST (self));

        g_object_set (HZL_TASKS_LIST (self),
                      "name", name,
                      NULL);
}

const gchar*
hzl_tasks_list_get_name (HzlTasksList *self)
{
        g_return_val_if_fail (HZL_IS_TASKS_LIST (self), NULL);

        return self->priv->name;
}
