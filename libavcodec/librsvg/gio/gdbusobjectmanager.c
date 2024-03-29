/* GDBus - GLib D-Bus Library
 *
 * Copyright (C) 2008-2010 Red Hat, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#include "config.h"

#include "gdbusobject.h"
#include "gdbusobjectmanager.h"
#include "gdbusinterface.h"
#include "gdbusutils.h"

#include "../glib/glibintl.h"
#include "gmarshal-internal.h"

/**
 * GDBusObjectManager:
 *
 * The `GDBusObjectManager` type is the base type for service- and
 * client-side implementations of the standardized
 * [`org.freedesktop.DBus.ObjectManager`](http://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-objectmanager)
 * interface.
 *
 * See [class@Gio.DBusObjectManagerClient] for the client-side implementation
 * and [class@Gio.DBusObjectManagerServer] for the service-side implementation.
 */

typedef GDBusObjectManagerIface GDBusObjectManagerInterface;
G_DEFINE_INTERFACE (GDBusObjectManager, g_dbus_object_manager, G_TYPE_OBJECT)

enum {
  OBJECT_ADDED,
  OBJECT_REMOVED,
  INTERFACE_ADDED,
  INTERFACE_REMOVED,
  N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
g_dbus_object_manager_default_init (GDBusObjectManagerIface *iface)
{
  /**
   * GDBusObjectManager::object-added:
   * @manager: The #GDBusObjectManager emitting the signal.
   * @object: The #GDBusObject that was added.
   *
   * Emitted when @object is added to @manager.
   *
   * Since: 2.30
   */
  signals[OBJECT_ADDED] =
    g_signal_new (I_("object-added"),
                  G_TYPE_FROM_INTERFACE (iface),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GDBusObjectManagerIface, object_added),
                  NULL,
                  NULL,
                  NULL,
                  G_TYPE_NONE,
                  1,
                  G_TYPE_DBUS_OBJECT);

  /**
   * GDBusObjectManager::object-removed:
   * @manager: The #GDBusObjectManager emitting the signal.
   * @object: The #GDBusObject that was removed.
   *
   * Emitted when @object is removed from @manager.
   *
   * Since: 2.30
   */
  signals[OBJECT_REMOVED] =
    g_signal_new (I_("object-removed"),
                  G_TYPE_FROM_INTERFACE (iface),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GDBusObjectManagerIface, object_removed),
                  NULL,
                  NULL,
                  NULL,
                  G_TYPE_NONE,
                  1,
                  G_TYPE_DBUS_OBJECT);

  /**
   * GDBusObjectManager::interface-added:
   * @manager: The #GDBusObjectManager emitting the signal.
   * @object: The #GDBusObject on which an interface was added.
   * @interface: The #GDBusInterface that was added.
   *
   * Emitted when @interface is added to @object.
   *
   * This signal exists purely as a convenience to avoid having to
   * connect signals to all objects managed by @manager.
   *
   * Since: 2.30
   */
  signals[INTERFACE_ADDED] =
    g_signal_new (I_("interface-added"),
                  G_TYPE_FROM_INTERFACE (iface),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GDBusObjectManagerIface, interface_added),
                  NULL,
                  NULL,
                  _g_cclosure_marshal_VOID__OBJECT_OBJECT,
                  G_TYPE_NONE,
                  2,
                  G_TYPE_DBUS_OBJECT,
                  G_TYPE_DBUS_INTERFACE);
  g_signal_set_va_marshaller (signals[INTERFACE_ADDED],
                              G_TYPE_FROM_INTERFACE (iface),
                              _g_cclosure_marshal_VOID__OBJECT_OBJECTv);

  /**
   * GDBusObjectManager::interface-removed:
   * @manager: The #GDBusObjectManager emitting the signal.
   * @object: The #GDBusObject on which an interface was removed.
   * @interface: The #GDBusInterface that was removed.
   *
   * Emitted when @interface has been removed from @object.
   *
   * This signal exists purely as a convenience to avoid having to
   * connect signals to all objects managed by @manager.
   *
   * Since: 2.30
   */
  signals[INTERFACE_REMOVED] =
    g_signal_new (I_("interface-removed"),
                  G_TYPE_FROM_INTERFACE (iface),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GDBusObjectManagerIface, interface_removed),
                  NULL,
                  NULL,
                  _g_cclosure_marshal_VOID__OBJECT_OBJECT,
                  G_TYPE_NONE,
                  2,
                  G_TYPE_DBUS_OBJECT,
                  G_TYPE_DBUS_INTERFACE);
  g_signal_set_va_marshaller (signals[INTERFACE_REMOVED],
                              G_TYPE_FROM_INTERFACE (iface),
                              _g_cclosure_marshal_VOID__OBJECT_OBJECTv);

}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_object_manager_get_object_path:
 * @manager: A #GDBusObjectManager.
 *
 * Gets the object path that @manager is for.
 *
 * Returns: A string owned by @manager. Do not free.
 *
 * Since: 2.30
 */
const gchar *
g_dbus_object_manager_get_object_path (GDBusObjectManager *manager)
{
  GDBusObjectManagerIface *iface = G_DBUS_OBJECT_MANAGER_GET_IFACE (manager);
  return iface->get_object_path (manager);
}

/**
 * g_dbus_object_manager_get_objects:
 * @manager: A #GDBusObjectManager.
 *
 * Gets all #GDBusObject objects known to @manager.
 *
 * Returns: (transfer full) (element-type GDBusObject): A list of
 *   #GDBusObject objects. The returned list should be freed with
 *   g_list_free() after each element has been freed with
 *   g_object_unref().
 *
 * Since: 2.30
 */
GList *
g_dbus_object_manager_get_objects (GDBusObjectManager *manager)
{
  GDBusObjectManagerIface *iface = G_DBUS_OBJECT_MANAGER_GET_IFACE (manager);
  return iface->get_objects (manager);
}

/**
 * g_dbus_object_manager_get_object:
 * @manager: A #GDBusObjectManager.
 * @object_path: Object path to look up.
 *
 * Gets the #GDBusObject at @object_path, if any.
 *
 * Returns: (transfer full) (nullable): A #GDBusObject or %NULL. Free with
 *   g_object_unref().
 *
 * Since: 2.30
 */
GDBusObject *
g_dbus_object_manager_get_object (GDBusObjectManager *manager,
                                  const gchar        *object_path)
{
  GDBusObjectManagerIface *iface = G_DBUS_OBJECT_MANAGER_GET_IFACE (manager);
  g_return_val_if_fail (g_variant_is_object_path (object_path), NULL);
  return iface->get_object (manager, object_path);
}

/**
 * g_dbus_object_manager_get_interface:
 * @manager: A #GDBusObjectManager.
 * @object_path: Object path to look up.
 * @interface_name: D-Bus interface name to look up.
 *
 * Gets the interface proxy for @interface_name at @object_path, if
 * any.
 *
 * Returns: (transfer full) (nullable): A #GDBusInterface instance or %NULL. Free
 *   with g_object_unref().
 *
 * Since: 2.30
 */
GDBusInterface *
g_dbus_object_manager_get_interface (GDBusObjectManager *manager,
                                     const gchar        *object_path,
                                     const gchar        *interface_name)
{
  GDBusObjectManagerIface *iface = G_DBUS_OBJECT_MANAGER_GET_IFACE (manager);
  g_return_val_if_fail (g_variant_is_object_path (object_path), NULL);
  g_return_val_if_fail (g_dbus_is_interface_name (interface_name), NULL);
  return iface->get_interface (manager, object_path, interface_name);
}
