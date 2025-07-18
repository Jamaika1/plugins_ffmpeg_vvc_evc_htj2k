/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright (C) 2010 Collabora, Ltd.
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
 * Author: Nicolas Dufresne <nicolas.dufresne@collabora.co.uk>
 */

#include "config.h"

#include "gproxyresolver.h"

#include "../glib/glib.h"
#include "../glib/glibintl.h"

#include "gasyncresult.h"
#include "gcancellable.h"
#include "gtask.h"
#include "giomodule.h"
#include "gioerror.h"
#include "giomodule-priv.h"
#include "gnetworkingprivate.h"

/**
 * GProxyResolver:
 *
 * `GProxyResolver` provides synchronous and asynchronous network proxy
 * resolution. `GProxyResolver` is used within [class@Gio.SocketClient] through
 * the method [method@Gio.SocketConnectable.proxy_enumerate].
 *
 * Implementations of `GProxyResolver` based on
 * [libproxy](https://github.com/libproxy/libproxy) and GNOME settings can be
 * found in [glib-networking](https://gitlab.gnome.org/GNOME/glib-networking).
 * GIO comes with an implementation for use inside Flatpak portals.
 *
 * Since: 2.26
 */

/**
 * GProxyResolverInterface:
 * @g_iface: The parent interface.
 * @is_supported: the virtual function pointer for g_proxy_resolver_is_supported()
 * @lookup: the virtual function pointer for g_proxy_resolver_lookup()
 * @lookup_async: the virtual function pointer for
 *  g_proxy_resolver_lookup_async()
 * @lookup_finish: the virtual function pointer for
 *  g_proxy_resolver_lookup_finish()
 *
 * The virtual function table for #GProxyResolver.
 */

G_DEFINE_INTERFACE (GProxyResolver, g_proxy_resolver, G_TYPE_OBJECT)

static void
g_proxy_resolver_default_init (GProxyResolverInterface *iface)
{
}

static GProxyResolver *proxy_resolver_default_singleton = NULL;  /* (owned) (atomic) */

/**
 * g_proxy_resolver_get_default:
 *
 * Gets the default #GProxyResolver for the system.
 *
 * Returns: (not nullable) (transfer none): the default #GProxyResolver, which
 *     will be a dummy object if no proxy resolver is available
 *
 * Since: 2.26
 */
GProxyResolver *
g_proxy_resolver_get_default (void)
{
  if (g_once_init_enter_pointer (&proxy_resolver_default_singleton))
    {
      GProxyResolver *singleton;

      singleton = _g_io_module_get_default (G_PROXY_RESOLVER_EXTENSION_POINT_NAME,
                                            "GIO_USE_PROXY_RESOLVER",
                                            (GIOModuleVerifyFunc) g_proxy_resolver_is_supported);

      g_once_init_leave_pointer (&proxy_resolver_default_singleton, singleton);
    }

  return proxy_resolver_default_singleton;
}

/**
 * g_proxy_resolver_is_supported:
 * @resolver: a #GProxyResolver
 *
 * Checks if @resolver can be used on this system. (This is used
 * internally; g_proxy_resolver_get_default() will only return a proxy
 * resolver that returns %TRUE for this method.)
 *
 * Returns: %TRUE if @resolver is supported.
 *
 * Since: 2.26
 */
gboolean
g_proxy_resolver_is_supported (GProxyResolver *resolver)
{
  GProxyResolverInterface *iface;

  g_return_val_if_fail (G_IS_PROXY_RESOLVER (resolver), FALSE);

  iface = G_PROXY_RESOLVER_GET_IFACE (resolver);

  return (* iface->is_supported) (resolver);
}

/**
 * g_proxy_resolver_lookup:
 * @resolver: a #GProxyResolver
 * @uri: a URI representing the destination to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Looks into the system proxy configuration to determine what proxy,
 * if any, to use to connect to @uri. The returned proxy URIs are of
 * the form `<protocol>://[user[:password]@]host[:port]` or
 * `direct://`, where `<protocol>` could be http, rtsp, socks
 * or other proxying protocol.
 *
 * If you don't know what network protocol is being used on the
 * socket, you should use `none` as the URI protocol.
 * In this case, the resolver might still return a generic proxy type
 * (such as SOCKS), but would not return protocol-specific proxy types
 * (such as http).
 *
 * `direct://` is used when no proxy is needed.
 * Direct connection should not be attempted unless it is part of the
 * returned array of proxies.
 *
 * Returns: (transfer full) (array zero-terminated=1): A
 *               NULL-terminated array of proxy URIs. Must be freed
 *               with g_strfreev().
 *
 * Since: 2.26
 */
gchar **
g_proxy_resolver_lookup (GProxyResolver  *resolver,
			 const gchar     *uri,
			 GCancellable    *cancellable,
			 GError         **error)
{
  GProxyResolverInterface *iface;
  gchar **proxy_uris;

  g_return_val_if_fail (G_IS_PROXY_RESOLVER (resolver), NULL);
  g_return_val_if_fail (uri != NULL, NULL);

  if (!g_uri_is_valid (uri, G_URI_FLAGS_NONE, NULL))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                   "Invalid URI ‘%s’", uri);
      return NULL;
    }

  iface = G_PROXY_RESOLVER_GET_IFACE (resolver);

  proxy_uris = (* iface->lookup) (resolver, uri, cancellable, error);
  if (proxy_uris == NULL && error != NULL)
    g_assert (*error != NULL);
  return proxy_uris;
}

/**
 * g_proxy_resolver_lookup_async:
 * @resolver: a #GProxyResolver
 * @uri: a URI representing the destination to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: (scope async): callback to call after resolution completes
 * @user_data: data for @callback
 *
 * Asynchronous lookup of proxy. See g_proxy_resolver_lookup() for more
 * details.
 *
 * Since: 2.26
 */
void
g_proxy_resolver_lookup_async (GProxyResolver      *resolver,
			       const gchar         *uri,
			       GCancellable        *cancellable,
			       GAsyncReadyCallback  callback,
			       gpointer             user_data)
{
  GProxyResolverInterface *iface;
  GError *error = NULL;

  g_return_if_fail (G_IS_PROXY_RESOLVER (resolver));
  g_return_if_fail (uri != NULL);

  if (!g_uri_is_valid (uri, G_URI_FLAGS_NONE, NULL))
    {
      g_set_error (&error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                   "Invalid URI ‘%s’", uri);
      g_task_report_error (resolver, callback, user_data,
                           g_proxy_resolver_lookup_async,
                           g_steal_pointer (&error));
      return;
    }

  iface = G_PROXY_RESOLVER_GET_IFACE (resolver);

  (* iface->lookup_async) (resolver, uri, cancellable, callback, user_data);
}

/**
 * g_proxy_resolver_lookup_finish:
 * @resolver: a #GProxyResolver
 * @result: the result passed to your #GAsyncReadyCallback
 * @error: return location for a #GError, or %NULL
 *
 * Call this function to obtain the array of proxy URIs when
 * g_proxy_resolver_lookup_async() is complete. See
 * g_proxy_resolver_lookup() for more details.
 *
 * Returns: (transfer full) (array zero-terminated=1): A
 *               NULL-terminated array of proxy URIs. Must be freed
 *               with g_strfreev().
 *
 * Since: 2.26
 */
gchar **
g_proxy_resolver_lookup_finish (GProxyResolver     *resolver,
				GAsyncResult       *result,
				GError            **error)
{
  GProxyResolverInterface *iface;
  gchar **proxy_uris;

  g_return_val_if_fail (G_IS_PROXY_RESOLVER (resolver), NULL);

  if (g_async_result_is_tagged (result, g_proxy_resolver_lookup_async))
    return g_task_propagate_pointer (G_TASK (result), error);

  iface = G_PROXY_RESOLVER_GET_IFACE (resolver);

  proxy_uris = (* iface->lookup_finish) (resolver, result, error);
  if (proxy_uris == NULL && error != NULL)
    g_assert (*error != NULL);
  return proxy_uris;
}
