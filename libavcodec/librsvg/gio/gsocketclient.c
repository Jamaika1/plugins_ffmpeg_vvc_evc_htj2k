/*  GIO - GLib Input, Output and Streaming Library
 *
 * Copyright © 2008, 2009 codethink
 * Copyright © 2009 Red Hat, Inc
 * Copyright © 2018 Igalia S.L.
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
 * Authors: Ryan Lortie <desrt@desrt.ca>
 *          Alexander Larsson <alexl@redhat.com>
 */

#include "config.h"
#include "gsocketclient.h"

#ifndef G_OS_WIN32
#include <netinet/in.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "gioenumtypes.h"
#include "gsocketaddressenumerator.h"
#include "gsocketconnectable.h"
#include "gsocketconnection.h"
#include "gioprivate.h"
#include "gproxyaddressenumerator.h"
#include "gproxyaddress.h"
#include "gtask.h"
#include "gcancellable.h"
#include "gioerror.h"
#include "gsocket.h"
#include "gnetworkaddress.h"
#include "gnetworking.h"
#include "gnetworkservice.h"
#include "gproxy.h"
#include "gproxyresolver.h"
#include "gsocketaddress.h"
#include "gtcpconnection.h"
#include "gtcpwrapperconnection.h"
#include "gtlscertificate.h"
#include "gtlsclientconnection.h"
#include "ginetaddress.h"
#include "../glib/glibintl.h"
#include "gmarshal-internal.h"

/* As recommended by RFC 8305 this is the time it waits
 * on a connection before starting another concurrent attempt.
 *
 * See https://datatracker.ietf.org/doc/html/rfc8305#section-8
 */
#define HAPPY_EYEBALLS_CONNECTION_ATTEMPT_DELAY_MS 250

/**
 * GSocketClient:
 *
 * `GSocketClient` is a lightweight high-level utility class for connecting to
 * a network host using a connection oriented socket type.
 *
 * You create a `GSocketClient` object, set any options you want, and then
 * call a sync or async connect operation, which returns a
 * [class@Gio.SocketConnection] subclass on success.
 *
 * The type of the [class@Gio.SocketConnection] object returned depends on the
 * type of the underlying socket that is in use. For instance, for a TCP/IP
 * connection it will be a [class@Gio.TcpConnection].
 *
 * As `GSocketClient` is a lightweight object, you don't need to cache it. You
 * can just create a new one any time you need one.
 *
 * Since: 2.22
 */


enum
{
  EVENT,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum
{
  PROP_NONE,
  PROP_FAMILY,
  PROP_TYPE,
  PROP_PROTOCOL,
  PROP_LOCAL_ADDRESS,
  PROP_TIMEOUT,
  PROP_ENABLE_PROXY,
  PROP_TLS,
  PROP_TLS_VALIDATION_FLAGS,
  PROP_PROXY_RESOLVER
};

struct _GSocketClientPrivate
{
  GSocketFamily family;
  GSocketType type;
  GSocketProtocol protocol;
  GSocketAddress *local_address;
  guint timeout;
  gboolean enable_proxy;
  GHashTable *app_proxies;
  gboolean tls;
  GTlsCertificateFlags tls_validation_flags;
  GProxyResolver *proxy_resolver;
};

G_DEFINE_TYPE_WITH_PRIVATE (GSocketClient, g_socket_client, G_TYPE_OBJECT)

static GSocket *
create_socket (GSocketClient  *client,
	       GSocketAddress *dest_address,
	       GError        **error)
{
  GSocketFamily family;
  GSocket *socket;

  family = client->priv->family;
  if (family == G_SOCKET_FAMILY_INVALID &&
      client->priv->local_address != NULL)
    family = g_socket_address_get_family (client->priv->local_address);
  if (family == G_SOCKET_FAMILY_INVALID)
    family = g_socket_address_get_family (dest_address);

  socket = g_socket_new (family,
			 client->priv->type,
			 client->priv->protocol,
			 error);
  if (socket == NULL)
    return NULL;

  if (client->priv->local_address)
    {
#ifdef IP_BIND_ADDRESS_NO_PORT
      g_socket_set_option (socket, IPPROTO_IP, IP_BIND_ADDRESS_NO_PORT, 1, NULL);
#endif

      if (!g_socket_bind (socket,
			  client->priv->local_address,
			  FALSE,
			  error))
	{
	  g_object_unref (socket);
	  return NULL;
	}
    }

  if (client->priv->timeout)
    g_socket_set_timeout (socket, client->priv->timeout);

  return socket;
}

static gboolean
can_use_proxy (GSocketClient *client)
{
  GSocketClientPrivate *priv = client->priv;

  return priv->enable_proxy
          && priv->type == G_SOCKET_TYPE_STREAM;
}

static void
clarify_connect_error (GError             *error,
		       GSocketConnectable *connectable,
		       GSocketAddress     *address)
{
  const char *name;
  char *tmp_name = NULL;

  if (G_IS_PROXY_ADDRESS (address))
    {
      name = tmp_name = g_inet_address_to_string (g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (address)));

      g_prefix_error (&error, _("Could not connect to proxy server %s: "), name);
    }
  else
    {
      if (G_IS_NETWORK_ADDRESS (connectable))
	name = g_network_address_get_hostname (G_NETWORK_ADDRESS (connectable));
      else if (G_IS_NETWORK_SERVICE (connectable))
	name = g_network_service_get_domain (G_NETWORK_SERVICE (connectable));
      else if (G_IS_INET_SOCKET_ADDRESS (connectable))
	name = tmp_name = g_inet_address_to_string (g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (connectable)));
      else
	name = NULL;

      if (name)
	g_prefix_error (&error, _("Could not connect to %s: "), name);
      else
	g_prefix_error (&error, _("Could not connect: "));
    }

  g_free (tmp_name);
}

static void
g_socket_client_init (GSocketClient *client)
{
  client->priv = g_socket_client_get_instance_private (client);
  client->priv->type = G_SOCKET_TYPE_STREAM;
  client->priv->app_proxies = g_hash_table_new_full (g_str_hash,
						     g_str_equal,
						     g_free,
						     NULL);
}

/**
 * g_socket_client_new:
 *
 * Creates a new #GSocketClient with the default options.
 *
 * Returns: a #GSocketClient.
 *     Free the returned object with g_object_unref().
 *
 * Since: 2.22
 */
GSocketClient *
g_socket_client_new (void)
{
  return g_object_new (G_TYPE_SOCKET_CLIENT, NULL);
}

static void
g_socket_client_finalize (GObject *object)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  g_clear_object (&client->priv->local_address);
  g_clear_object (&client->priv->proxy_resolver);

  G_OBJECT_CLASS (g_socket_client_parent_class)->finalize (object);

  g_hash_table_unref (client->priv->app_proxies);
}

static void
g_socket_client_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  switch (prop_id)
    {
      case PROP_FAMILY:
	g_value_set_enum (value, client->priv->family);
	break;

      case PROP_TYPE:
	g_value_set_enum (value, client->priv->type);
	break;

      case PROP_PROTOCOL:
	g_value_set_enum (value, client->priv->protocol);
	break;

      case PROP_LOCAL_ADDRESS:
	g_value_set_object (value, client->priv->local_address);
	break;

      case PROP_TIMEOUT:
	g_value_set_uint (value, client->priv->timeout);
	break;

      case PROP_ENABLE_PROXY:
	g_value_set_boolean (value, client->priv->enable_proxy);
	break;

      case PROP_TLS:
	g_value_set_boolean (value, g_socket_client_get_tls (client));
	break;

      case PROP_TLS_VALIDATION_FLAGS:
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	g_value_set_flags (value, g_socket_client_get_tls_validation_flags (client));
G_GNUC_END_IGNORE_DEPRECATIONS
	break;

      case PROP_PROXY_RESOLVER:
	g_value_set_object (value, g_socket_client_get_proxy_resolver (client));
	break;

      default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
g_socket_client_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  switch (prop_id)
    {
    case PROP_FAMILY:
      g_socket_client_set_family (client, g_value_get_enum (value));
      break;

    case PROP_TYPE:
      g_socket_client_set_socket_type (client, g_value_get_enum (value));
      break;

    case PROP_PROTOCOL:
      g_socket_client_set_protocol (client, g_value_get_enum (value));
      break;

    case PROP_LOCAL_ADDRESS:
      g_socket_client_set_local_address (client, g_value_get_object (value));
      break;

    case PROP_TIMEOUT:
      g_socket_client_set_timeout (client, g_value_get_uint (value));
      break;

    case PROP_ENABLE_PROXY:
      g_socket_client_set_enable_proxy (client, g_value_get_boolean (value));
      break;

    case PROP_TLS:
      g_socket_client_set_tls (client, g_value_get_boolean (value));
      break;

    case PROP_TLS_VALIDATION_FLAGS:
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      g_socket_client_set_tls_validation_flags (client, g_value_get_flags (value));
G_GNUC_END_IGNORE_DEPRECATIONS
      break;

    case PROP_PROXY_RESOLVER:
      g_socket_client_set_proxy_resolver (client, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

/**
 * g_socket_client_get_family:
 * @client: a #GSocketClient.
 *
 * Gets the socket family of the socket client.
 *
 * See g_socket_client_set_family() for details.
 *
 * Returns: a #GSocketFamily
 *
 * Since: 2.22
 */
GSocketFamily
g_socket_client_get_family (GSocketClient *client)
{
  return client->priv->family;
}

/**
 * g_socket_client_set_family:
 * @client: a #GSocketClient.
 * @family: a #GSocketFamily
 *
 * Sets the socket family of the socket client.
 * If this is set to something other than %G_SOCKET_FAMILY_INVALID
 * then the sockets created by this object will be of the specified
 * family.
 *
 * This might be useful for instance if you want to force the local
 * connection to be an ipv4 socket, even though the address might
 * be an ipv6 mapped to ipv4 address.
 *
 * Since: 2.22
 */
void
g_socket_client_set_family (GSocketClient *client,
			    GSocketFamily  family)
{
  if (client->priv->family == family)
    return;

  client->priv->family = family;
  g_object_notify (G_OBJECT (client), "family");
}

/**
 * g_socket_client_get_socket_type:
 * @client: a #GSocketClient.
 *
 * Gets the socket type of the socket client.
 *
 * See g_socket_client_set_socket_type() for details.
 *
 * Returns: a #GSocketFamily
 *
 * Since: 2.22
 */
GSocketType
g_socket_client_get_socket_type (GSocketClient *client)
{
  return client->priv->type;
}

/**
 * g_socket_client_set_socket_type:
 * @client: a #GSocketClient.
 * @type: a #GSocketType
 *
 * Sets the socket type of the socket client.
 * The sockets created by this object will be of the specified
 * type.
 *
 * It doesn't make sense to specify a type of %G_SOCKET_TYPE_DATAGRAM,
 * as GSocketClient is used for connection oriented services.
 *
 * Since: 2.22
 */
void
g_socket_client_set_socket_type (GSocketClient *client,
				 GSocketType    type)
{
  if (client->priv->type == type)
    return;

  client->priv->type = type;
  g_object_notify (G_OBJECT (client), "type");
}

/**
 * g_socket_client_get_protocol:
 * @client: a #GSocketClient
 *
 * Gets the protocol name type of the socket client.
 *
 * See g_socket_client_set_protocol() for details.
 *
 * Returns: a #GSocketProtocol
 *
 * Since: 2.22
 */
GSocketProtocol
g_socket_client_get_protocol (GSocketClient *client)
{
  return client->priv->protocol;
}

/**
 * g_socket_client_set_protocol:
 * @client: a #GSocketClient.
 * @protocol: a #GSocketProtocol
 *
 * Sets the protocol of the socket client.
 * The sockets created by this object will use of the specified
 * protocol.
 *
 * If @protocol is %G_SOCKET_PROTOCOL_DEFAULT that means to use the default
 * protocol for the socket family and type.
 *
 * Since: 2.22
 */
void
g_socket_client_set_protocol (GSocketClient   *client,
			      GSocketProtocol  protocol)
{
  if (client->priv->protocol == protocol)
    return;

  client->priv->protocol = protocol;
  g_object_notify (G_OBJECT (client), "protocol");
}

/**
 * g_socket_client_get_local_address:
 * @client: a #GSocketClient.
 *
 * Gets the local address of the socket client.
 *
 * See g_socket_client_set_local_address() for details.
 *
 * Returns: (nullable) (transfer none): a #GSocketAddress or %NULL. Do not free.
 *
 * Since: 2.22
 */
GSocketAddress *
g_socket_client_get_local_address (GSocketClient *client)
{
  return client->priv->local_address;
}

/**
 * g_socket_client_set_local_address:
 * @client: a #GSocketClient.
 * @address: (nullable): a #GSocketAddress, or %NULL
 *
 * Sets the local address of the socket client.
 * The sockets created by this object will bound to the
 * specified address (if not %NULL) before connecting.
 *
 * This is useful if you want to ensure that the local
 * side of the connection is on a specific port, or on
 * a specific interface.
 *
 * Since: 2.22
 */
void
g_socket_client_set_local_address (GSocketClient  *client,
				   GSocketAddress *address)
{
  if (address)
    g_object_ref (address);

  if (client->priv->local_address)
    {
      g_object_unref (client->priv->local_address);
    }
  client->priv->local_address = address;
  g_object_notify (G_OBJECT (client), "local-address");
}

/**
 * g_socket_client_get_timeout:
 * @client: a #GSocketClient
 *
 * Gets the I/O timeout time for sockets created by @client.
 *
 * See g_socket_client_set_timeout() for details.
 *
 * Returns: the timeout in seconds
 *
 * Since: 2.26
 */
guint
g_socket_client_get_timeout (GSocketClient *client)
{
  return client->priv->timeout;
}


/**
 * g_socket_client_set_timeout:
 * @client: a #GSocketClient.
 * @timeout: the timeout
 *
 * Sets the I/O timeout for sockets created by @client. @timeout is a
 * time in seconds, or 0 for no timeout (the default).
 *
 * The timeout value affects the initial connection attempt as well,
 * so setting this may cause calls to g_socket_client_connect(), etc,
 * to fail with %G_IO_ERROR_TIMED_OUT.
 *
 * Since: 2.26
 */
void
g_socket_client_set_timeout (GSocketClient *client,
			     guint          timeout)
{
  if (client->priv->timeout == timeout)
    return;

  client->priv->timeout = timeout;
  g_object_notify (G_OBJECT (client), "timeout");
}

/**
 * g_socket_client_get_enable_proxy:
 * @client: a #GSocketClient.
 *
 * Gets the proxy enable state; see g_socket_client_set_enable_proxy()
 *
 * Returns: whether proxying is enabled
 *
 * Since: 2.26
 */
gboolean
g_socket_client_get_enable_proxy (GSocketClient *client)
{
  return client->priv->enable_proxy;
}

/**
 * g_socket_client_set_enable_proxy:
 * @client: a #GSocketClient.
 * @enable: whether to enable proxies
 *
 * Sets whether or not @client attempts to make connections via a
 * proxy server. When enabled (the default), #GSocketClient will use a
 * #GProxyResolver to determine if a proxy protocol such as SOCKS is
 * needed, and automatically do the necessary proxy negotiation.
 *
 * See also g_socket_client_set_proxy_resolver().
 *
 * Since: 2.26
 */
void
g_socket_client_set_enable_proxy (GSocketClient *client,
				  gboolean       enable)
{
  enable = !!enable;
  if (client->priv->enable_proxy == enable)
    return;

  client->priv->enable_proxy = enable;
  g_object_notify (G_OBJECT (client), "enable-proxy");
}

/**
 * g_socket_client_get_tls:
 * @client: a #GSocketClient.
 *
 * Gets whether @client creates TLS connections. See
 * g_socket_client_set_tls() for details.
 *
 * Returns: whether @client uses TLS
 *
 * Since: 2.28
 */
gboolean
g_socket_client_get_tls (GSocketClient *client)
{
  return client->priv->tls;
}

/**
 * g_socket_client_set_tls:
 * @client: a #GSocketClient.
 * @tls: whether to use TLS
 *
 * Sets whether @client creates TLS (aka SSL) connections. If @tls is
 * %TRUE, @client will wrap its connections in a #GTlsClientConnection
 * and perform a TLS handshake when connecting.
 *
 * Note that since #GSocketClient must return a #GSocketConnection,
 * but #GTlsClientConnection is not a #GSocketConnection, this
 * actually wraps the resulting #GTlsClientConnection in a
 * #GTcpWrapperConnection when returning it. You can use
 * g_tcp_wrapper_connection_get_base_io_stream() on the return value
 * to extract the #GTlsClientConnection.
 *
 * If you need to modify the behavior of the TLS handshake (eg, by
 * setting a client-side certificate to use, or connecting to the
 * #GTlsConnection::accept-certificate signal), you can connect to
 * @client's #GSocketClient::event signal and wait for it to be
 * emitted with %G_SOCKET_CLIENT_TLS_HANDSHAKING, which will give you
 * a chance to see the #GTlsClientConnection before the handshake
 * starts.
 *
 * Since: 2.28
 */
void
g_socket_client_set_tls (GSocketClient *client,
			 gboolean       tls)
{
  tls = !!tls;
  if (tls == client->priv->tls)
    return;

  client->priv->tls = tls;
  g_object_notify (G_OBJECT (client), "tls");
}

/**
 * g_socket_client_get_tls_validation_flags:
 * @client: a #GSocketClient.
 *
 * Gets the TLS validation flags used creating TLS connections via
 * @client.
 *
 * This function does not work as originally designed and is impossible
 * to use correctly. See #GSocketClient:tls-validation-flags for more
 * information.
 *
 * Returns: the TLS validation flags
 *
 * Since: 2.28
 *
 * Deprecated: 2.72: Do not attempt to ignore validation errors.
 */
GTlsCertificateFlags
g_socket_client_get_tls_validation_flags (GSocketClient *client)
{
  return client->priv->tls_validation_flags;
}

/**
 * g_socket_client_set_tls_validation_flags:
 * @client: a #GSocketClient.
 * @flags: the validation flags
 *
 * Sets the TLS validation flags used when creating TLS connections
 * via @client. The default value is %G_TLS_CERTIFICATE_VALIDATE_ALL.
 *
 * This function does not work as originally designed and is impossible
 * to use correctly. See #GSocketClient:tls-validation-flags for more
 * information.
 *
 * Since: 2.28
 *
 * Deprecated: 2.72: Do not attempt to ignore validation errors.
 */
void
g_socket_client_set_tls_validation_flags (GSocketClient        *client,
					  GTlsCertificateFlags  flags)
{
  if (client->priv->tls_validation_flags != flags)
    {
      client->priv->tls_validation_flags = flags;
      g_object_notify (G_OBJECT (client), "tls-validation-flags");
    }
}

/**
 * g_socket_client_get_proxy_resolver:
 * @client: a #GSocketClient.
 *
 * Gets the #GProxyResolver being used by @client. Normally, this will
 * be the resolver returned by g_proxy_resolver_get_default(), but you
 * can override it with g_socket_client_set_proxy_resolver().
 *
 * Returns: (transfer none): The #GProxyResolver being used by
 *   @client.
 *
 * Since: 2.36
 */
GProxyResolver *
g_socket_client_get_proxy_resolver (GSocketClient *client)
{
  if (client->priv->proxy_resolver)
    return client->priv->proxy_resolver;
  else
    return g_proxy_resolver_get_default ();
}

/**
 * g_socket_client_set_proxy_resolver:
 * @client: a #GSocketClient.
 * @proxy_resolver: (nullable): a #GProxyResolver, or %NULL for the
 *   default.
 *
 * Overrides the #GProxyResolver used by @client. You can call this if
 * you want to use specific proxies, rather than using the system
 * default proxy settings.
 *
 * Note that whether or not the proxy resolver is actually used
 * depends on the setting of #GSocketClient:enable-proxy, which is not
 * changed by this function (but which is %TRUE by default)
 *
 * Since: 2.36
 */
void
g_socket_client_set_proxy_resolver (GSocketClient  *client,
                                    GProxyResolver *proxy_resolver)
{
  /* We have to be careful to avoid calling
   * g_proxy_resolver_get_default() until we're sure we need it,
   * because trying to load the default proxy resolver module will
   * break some test programs that aren't expecting it (eg,
   * tests/gsettings).
   */

  if (client->priv->proxy_resolver)
    g_object_unref (client->priv->proxy_resolver);

  client->priv->proxy_resolver = proxy_resolver;

  if (client->priv->proxy_resolver)
    g_object_ref (client->priv->proxy_resolver);
}

static void
g_socket_client_class_init (GSocketClientClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->finalize = g_socket_client_finalize;
  gobject_class->set_property = g_socket_client_set_property;
  gobject_class->get_property = g_socket_client_get_property;

  /**
   * GSocketClient::event:
   * @client: the #GSocketClient
   * @event: the event that is occurring
   * @connectable: the #GSocketConnectable that @event is occurring on
   * @connection: (nullable): the current representation of the connection
   *
   * Emitted when @client's activity on @connectable changes state.
   * Among other things, this can be used to provide progress
   * information about a network connection in the UI. The meanings of
   * the different @event values are as follows:
   *
   * - %G_SOCKET_CLIENT_RESOLVING: @client is about to look up @connectable
   *   in DNS. @connection will be %NULL.
   *
   * - %G_SOCKET_CLIENT_RESOLVED:  @client has successfully resolved
   *   @connectable in DNS. @connection will be %NULL.
   *
   * - %G_SOCKET_CLIENT_CONNECTING: @client is about to make a connection
   *   to a remote host; either a proxy server or the destination server
   *   itself. @connection is the #GSocketConnection, which is not yet
   *   connected.  Since GLib 2.40, you can access the remote
   *   address via g_socket_connection_get_remote_address().
   *
   * - %G_SOCKET_CLIENT_CONNECTED: @client has successfully connected
   *   to a remote host. @connection is the connected #GSocketConnection.
   *
   * - %G_SOCKET_CLIENT_PROXY_NEGOTIATING: @client is about to negotiate
   *   with a proxy to get it to connect to @connectable. @connection is
   *   the #GSocketConnection to the proxy server.
   *
   * - %G_SOCKET_CLIENT_PROXY_NEGOTIATED: @client has negotiated a
   *   connection to @connectable through a proxy server. @connection is
   *   the stream returned from g_proxy_connect(), which may or may not
   *   be a #GSocketConnection.
   *
   * - %G_SOCKET_CLIENT_TLS_HANDSHAKING: @client is about to begin a TLS
   *   handshake. @connection is a #GTlsClientConnection.
   *
   * - %G_SOCKET_CLIENT_TLS_HANDSHAKED: @client has successfully completed
   *   the TLS handshake. @connection is a #GTlsClientConnection.
   *
   * - %G_SOCKET_CLIENT_COMPLETE: @client has either successfully connected
   *   to @connectable (in which case @connection is the #GSocketConnection
   *   that it will be returning to the caller) or has failed (in which
   *   case @connection is %NULL and the client is about to return an error).
   *
   * Each event except %G_SOCKET_CLIENT_COMPLETE may be emitted
   * multiple times (or not at all) for a given connectable (in
   * particular, if @client ends up attempting to connect to more than
   * one address). However, if @client emits the #GSocketClient::event
   * signal at all for a given connectable, then it will always emit
   * it with %G_SOCKET_CLIENT_COMPLETE when it is done.
   *
   * Note that there may be additional #GSocketClientEvent values in
   * the future; unrecognized @event values should be ignored.
   *
   * Since: 2.32
   */
  signals[EVENT] =
    g_signal_new (I_("event"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GSocketClientClass, event),
		  NULL, NULL,
		  _g_cclosure_marshal_VOID__ENUM_OBJECT_OBJECT,
		  G_TYPE_NONE, 3,
		  G_TYPE_SOCKET_CLIENT_EVENT,
		  G_TYPE_SOCKET_CONNECTABLE,
		  G_TYPE_IO_STREAM);
  g_signal_set_va_marshaller (signals[EVENT],
                              G_TYPE_FROM_CLASS (class),
                              _g_cclosure_marshal_VOID__ENUM_OBJECT_OBJECTv);

  /**
   * GSocketClient:family:
   *
   * The address family to use for socket construction.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_FAMILY,
				   g_param_spec_enum ("family", NULL, NULL,
						      G_TYPE_SOCKET_FAMILY,
						      G_SOCKET_FAMILY_INVALID,
						      G_PARAM_CONSTRUCT |
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:type:
   *
   * The type to use for socket construction.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_TYPE,
				   g_param_spec_enum ("type", NULL, NULL,
						      G_TYPE_SOCKET_TYPE,
						      G_SOCKET_TYPE_STREAM,
						      G_PARAM_CONSTRUCT |
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:protocol:
   *
   * The protocol to use for socket construction, or `0` for default.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_PROTOCOL,
				   g_param_spec_enum ("protocol", NULL, NULL,
						      G_TYPE_SOCKET_PROTOCOL,
						      G_SOCKET_PROTOCOL_DEFAULT,
						      G_PARAM_CONSTRUCT |
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:local-address:
   *
   * The local address constructed sockets will be bound to.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_LOCAL_ADDRESS,
				   g_param_spec_object ("local-address", NULL, NULL,
							G_TYPE_SOCKET_ADDRESS,
							G_PARAM_CONSTRUCT |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:timeout:
   *
   * The I/O timeout for sockets, in seconds, or `0` for none.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_TIMEOUT,
				   g_param_spec_uint ("timeout", NULL, NULL,
						      0, G_MAXUINT, 0,
						      G_PARAM_CONSTRUCT |
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:enable-proxy:
   *
   * Enable proxy support.
   *
   * Since: 2.22
   */
   g_object_class_install_property (gobject_class, PROP_ENABLE_PROXY,
				    g_param_spec_boolean ("enable-proxy", NULL, NULL,
							  TRUE,
							  G_PARAM_CONSTRUCT |
							  G_PARAM_READWRITE |
							  G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:tls:
   *
   * Whether to create TLS connections.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class, PROP_TLS,
				   g_param_spec_boolean ("tls", NULL, NULL,
							 FALSE,
							 G_PARAM_CONSTRUCT |
							 G_PARAM_READWRITE |
							 G_PARAM_STATIC_STRINGS));

  /**
   * GSocketClient:tls-validation-flags:
   *
   * The TLS validation flags used when creating TLS connections. The
   * default value is %G_TLS_CERTIFICATE_VALIDATE_ALL.
   *
   * GLib guarantees that if certificate verification fails, at least one
   * flag will be set, but it does not guarantee that all possible flags
   * will be set. Accordingly, you may not safely decide to ignore any
   * particular type of error. For example, it would be incorrect to mask
   * %G_TLS_CERTIFICATE_EXPIRED if you want to allow expired certificates,
   * because this could potentially be the only error flag set even if
   * other problems exist with the certificate. Therefore, there is no
   * safe way to use this property. This is not a horrible problem,
   * though, because you should not be attempting to ignore validation
   * errors anyway. If you really must ignore TLS certificate errors,
   * connect to the #GSocketClient::event signal, wait for it to be
   * emitted with %G_SOCKET_CLIENT_TLS_HANDSHAKING, and use that to
   * connect to #GTlsConnection::accept-certificate.
   *
   * Deprecated: 2.72: Do not attempt to ignore validation errors.
   */
  g_object_class_install_property (gobject_class, PROP_TLS_VALIDATION_FLAGS,
				   g_param_spec_flags ("tls-validation-flags", NULL, NULL,
						       G_TYPE_TLS_CERTIFICATE_FLAGS,
						       G_TLS_CERTIFICATE_VALIDATE_ALL,
						       G_PARAM_CONSTRUCT |
						       G_PARAM_READWRITE |
						       G_PARAM_STATIC_STRINGS |
						       G_PARAM_DEPRECATED));

  /**
   * GSocketClient:proxy-resolver:
   *
   * The proxy resolver to use
   *
   * Since: 2.36
   */
  g_object_class_install_property (gobject_class, PROP_PROXY_RESOLVER,
                                   g_param_spec_object ("proxy-resolver", NULL, NULL,
                                                        G_TYPE_PROXY_RESOLVER,
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));
}

static void
g_socket_client_emit_event (GSocketClient       *client,
			    GSocketClientEvent   event,
			    GSocketConnectable  *connectable,
			    GIOStream           *connection)
{
  g_signal_emit (client, signals[EVENT], 0,
		 event, connectable, connection);
}

/* Originally, GSocketClient returned whatever error occurred last. Turns
 * out this doesn't work well in practice. Consider the following case:
 * DNS returns an IPv4 and IPv6 address. First we'll connect() to the
 * IPv4 address, and say that succeeds, but TLS is enabled and the TLS
 * handshake fails. Then we try the IPv6 address and receive ENETUNREACH
 * because IPv6 isn't supported. We wind up returning NETWORK_UNREACHABLE
 * even though the address can be pinged and a TLS error would be more
 * appropriate. So instead, we now try to return the error corresponding
 * to the latest attempted GSocketClientEvent in the connection process.
 * TLS errors take precedence over proxy errors, which take precedence
 * over connect() errors, which take precedence over DNS errors.
 *
 * Note that the example above considers a sync codepath, but this is an
 * issue for the async codepath too, where events and errors may occur
 * in confusing orders.
 */
typedef struct
{
  GError *tmp_error;
  GError *best_error;
  GSocketClientEvent best_error_event;
} SocketClientErrorInfo;

static SocketClientErrorInfo *
socket_client_error_info_new (void)
{
  return g_new0 (SocketClientErrorInfo, 1);
}

static void
socket_client_error_info_free (SocketClientErrorInfo *info)
{
  g_assert (info->tmp_error == NULL);
  g_clear_error (&info->best_error);
  g_free (info);
}

static void
consider_tmp_error (SocketClientErrorInfo *info,
                    GSocketClientEvent     event)
{
  if (info->tmp_error == NULL)
    return;

  /* If we ever add more GSocketClientEvents in the future, then we'll
   * no longer be able to use >= for this comparison, because future
   * events will compare greater than G_SOCKET_CLIENT_COMPLETE. Until
   * then, this is convenient. Note G_SOCKET_CLIENT_RESOLVING is 0 so we
   * need to use >= here or those errors would never be set. That means
   * if we get two errors on the same GSocketClientEvent, we wind up
   * preferring the last one, which is fine.
   */
  g_assert (event <= G_SOCKET_CLIENT_COMPLETE);
  if (event >= info->best_error_event)
    {
      g_clear_error (&info->best_error);
      info->best_error = info->tmp_error;
      info->tmp_error = NULL;
      info->best_error_event = event;
    }
  else
    {
      g_clear_error (&info->tmp_error);
    }
}

/**
 * g_socket_client_connect:
 * @client: a #GSocketClient.
 * @connectable: a #GSocketConnectable specifying the remote address.
 * @cancellable: (nullable): optional #GCancellable object, %NULL to ignore.
 * @error: #GError for error reporting, or %NULL to ignore.
 *
 * Tries to resolve the @connectable and make a network connection to it.
 *
 * Upon a successful connection, a new #GSocketConnection is constructed
 * and returned.  The caller owns this new object and must drop their
 * reference to it when finished with it.
 *
 * The type of the #GSocketConnection object returned depends on the type of
 * the underlying socket that is used. For instance, for a TCP/IP connection
 * it will be a #GTcpConnection.
 *
 * The socket created will be the same family as the address that the
 * @connectable resolves to, unless family is set with g_socket_client_set_family()
 * or indirectly via g_socket_client_set_local_address(). The socket type
 * defaults to %G_SOCKET_TYPE_STREAM but can be set with
 * g_socket_client_set_socket_type().
 *
 * If a local address is specified with g_socket_client_set_local_address() the
 * socket will be bound to this address before connecting.
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.22
 */
GSocketConnection *
g_socket_client_connect (GSocketClient       *client,
			 GSocketConnectable  *connectable,
			 GCancellable        *cancellable,
			 GError             **error)
{
  GIOStream *connection = NULL;
  GSocketAddressEnumerator *enumerator = NULL;
  SocketClientErrorInfo *error_info;
  gboolean ever_resolved = FALSE;

  error_info = socket_client_error_info_new ();

  if (can_use_proxy (client))
    {
      enumerator = g_socket_connectable_proxy_enumerate (connectable);
      if (client->priv->proxy_resolver &&
          G_IS_PROXY_ADDRESS_ENUMERATOR (enumerator))
        {
          g_object_set (G_OBJECT (enumerator),
                        "proxy-resolver", client->priv->proxy_resolver,
                        NULL);
        }
    }
  else
    enumerator = g_socket_connectable_enumerate (connectable);

  while (connection == NULL)
    {
      GSocketAddress *address = NULL;
      gboolean application_proxy = FALSE;
      GSocket *socket;
      gboolean using_proxy;

      if (g_cancellable_is_cancelled (cancellable))
	{
	  g_clear_error (&error_info->best_error);
	  g_cancellable_set_error_if_cancelled (cancellable, &error_info->best_error);
	  break;
	}

      if (!ever_resolved)
	{
	  g_socket_client_emit_event (client, G_SOCKET_CLIENT_RESOLVING,
				      connectable, NULL);
	}
      address = g_socket_address_enumerator_next (enumerator, cancellable,
						  &error_info->tmp_error);
      consider_tmp_error (error_info, G_SOCKET_CLIENT_RESOLVING);
      if (!ever_resolved)
	{
	  g_socket_client_emit_event (client, G_SOCKET_CLIENT_RESOLVED,
				      connectable, NULL);
	  ever_resolved = TRUE;
	}

      if (address == NULL)
	{
          /* Enumeration is finished. */
          g_assert (&error_info->best_error != NULL);
	  break;
	}

      using_proxy = (G_IS_PROXY_ADDRESS (address) &&
		     client->priv->enable_proxy);

      socket = create_socket (client, address, &error_info->tmp_error);
      consider_tmp_error (error_info, G_SOCKET_CLIENT_CONNECTING);
      if (socket == NULL)
	{
	  g_object_unref (address);
	  continue;
	}

      connection = (GIOStream *)g_socket_connection_factory_create_connection (socket);
      g_socket_connection_set_cached_remote_address ((GSocketConnection*)connection, address);
      g_socket_client_emit_event (client, G_SOCKET_CLIENT_CONNECTING, connectable, connection);

      if (g_socket_connection_connect (G_SOCKET_CONNECTION (connection),
				       address, cancellable, &error_info->tmp_error))
	{
          g_socket_connection_set_cached_remote_address ((GSocketConnection*)connection, NULL);
	  g_socket_client_emit_event (client, G_SOCKET_CLIENT_CONNECTED, connectable, connection);
	}
      else
	{
	  clarify_connect_error (error_info->tmp_error, connectable, address);
          consider_tmp_error (error_info, G_SOCKET_CLIENT_CONNECTING);
	  g_object_unref (connection);
	  connection = NULL;
	}

      if (connection && using_proxy)
	{
	  GProxyAddress *proxy_addr = G_PROXY_ADDRESS (address);
	  const gchar *protocol;
	  GProxy *proxy;

	  protocol = g_proxy_address_get_protocol (proxy_addr);

          /* The connection should not be anything else then TCP Connection,
           * but let's put a safety guard in case
	   */
          if (!G_IS_TCP_CONNECTION (connection))
            {
              g_critical ("Trying to proxy over non-TCP connection, this is "
                          "most likely a bug in GLib IO library.");

              g_set_error_literal (&error_info->tmp_error,
                  G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                  _("Proxying over a non-TCP connection is not supported."));
              consider_tmp_error (error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);

	      g_object_unref (connection);
	      connection = NULL;
            }
	  else if (g_hash_table_contains (client->priv->app_proxies, protocol))
	    {
	      application_proxy = TRUE;
	    }
          else if ((proxy = g_proxy_get_default_for_protocol (protocol)))
	    {
	      GIOStream *proxy_connection;

	      g_socket_client_emit_event (client, G_SOCKET_CLIENT_PROXY_NEGOTIATING, connectable, connection);
	      proxy_connection = g_proxy_connect (proxy,
						  connection,
						  proxy_addr,
						  cancellable,
						  &error_info->tmp_error);
	      consider_tmp_error (error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);

	      g_object_unref (connection);
	      connection = proxy_connection;
	      g_object_unref (proxy);

	      if (connection)
		g_socket_client_emit_event (client, G_SOCKET_CLIENT_PROXY_NEGOTIATED, connectable, connection);
	    }
	  else
	    {
	      g_set_error (&error_info->tmp_error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
			   _("Proxy protocol “%s” is not supported."),
			   protocol);
	      consider_tmp_error (error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);
	      g_object_unref (connection);
	      connection = NULL;
	    }
	}

      if (!application_proxy && connection && client->priv->tls)
	{
	  GIOStream *tlsconn;

	  tlsconn = g_tls_client_connection_new (connection, connectable, &error_info->tmp_error);
	  g_object_unref (connection);
	  connection = tlsconn;

	  if (tlsconn)
	    {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	      g_tls_client_connection_set_validation_flags (G_TLS_CLIENT_CONNECTION (tlsconn),
                                                            client->priv->tls_validation_flags);
G_GNUC_END_IGNORE_DEPRECATIONS
	      g_socket_client_emit_event (client, G_SOCKET_CLIENT_TLS_HANDSHAKING, connectable, connection);
	      if (g_tls_connection_handshake (G_TLS_CONNECTION (tlsconn),
					      cancellable, &error_info->tmp_error))
		{
		  g_socket_client_emit_event (client, G_SOCKET_CLIENT_TLS_HANDSHAKED, connectable, connection);
		}
	      else
		{
		  consider_tmp_error (error_info, G_SOCKET_CLIENT_TLS_HANDSHAKING);
		  g_object_unref (tlsconn);
		  connection = NULL;
		}
	    }
          else
            {
              consider_tmp_error (error_info, G_SOCKET_CLIENT_TLS_HANDSHAKING);
            }
	}

      if (connection && !G_IS_SOCKET_CONNECTION (connection))
	{
	  GSocketConnection *wrapper_connection;

	  wrapper_connection = g_tcp_wrapper_connection_new (connection, socket);
	  g_object_unref (connection);
	  connection = (GIOStream *)wrapper_connection;
	}

      g_object_unref (socket);
      g_object_unref (address);
    }
  g_object_unref (enumerator);

  if (!connection)
    g_propagate_error (error, g_steal_pointer (&error_info->best_error));
  socket_client_error_info_free (error_info);

  g_socket_client_emit_event (client, G_SOCKET_CLIENT_COMPLETE, connectable, connection);
  return G_SOCKET_CONNECTION (connection);
}

/**
 * g_socket_client_connect_to_host:
 * @client: a #GSocketClient
 * @host_and_port: the name and optionally port of the host to connect to
 * @default_port: the default port to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a pointer to a #GError, or %NULL
 *
 * This is a helper function for g_socket_client_connect().
 *
 * Attempts to create a TCP connection to the named host.
 *
 * @host_and_port may be in any of a number of recognized formats; an IPv6
 * address, an IPv4 address, or a domain name (in which case a DNS
 * lookup is performed).  Quoting with [] is supported for all address
 * types.  A port override may be specified in the usual way with a
 * colon.  Ports may be given as decimal numbers or symbolic names (in
 * which case an /etc/services lookup is performed).
 *
 * If no port override is given in @host_and_port then @default_port will be
 * used as the port number to connect to.
 *
 * In general, @host_and_port is expected to be provided by the user (allowing
 * them to give the hostname, and a port override if necessary) and
 * @default_port is expected to be provided by the application.
 *
 * In the case that an IP address is given, a single connection
 * attempt is made.  In the case that a name is given, multiple
 * connection attempts may be made, in turn and according to the
 * number of address records in DNS, until a connection succeeds.
 *
 * Upon a successful connection, a new #GSocketConnection is constructed
 * and returned.  The caller owns this new object and must drop their
 * reference to it when finished with it.
 *
 * In the event of any failure (DNS error, service not found, no hosts
 * connectable) %NULL is returned and @error (if non-%NULL) is set
 * accordingly.
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.22
 */
GSocketConnection *
g_socket_client_connect_to_host (GSocketClient  *client,
				 const gchar    *host_and_port,
				 guint16         default_port,
				 GCancellable   *cancellable,
				 GError        **error)
{
  GSocketConnectable *connectable;
  GSocketConnection *connection;

  connectable = g_network_address_parse (host_and_port, default_port, error);
  if (connectable == NULL)
    return NULL;

  connection = g_socket_client_connect (client, connectable,
					cancellable, error);
  g_object_unref (connectable);

  return connection;
}

/**
 * g_socket_client_connect_to_service:
 * @client: a #GSocketConnection
 * @domain: a domain name
 * @service: the name of the service to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a pointer to a #GError, or %NULL
 *
 * Attempts to create a TCP connection to a service.
 *
 * This call looks up the SRV record for @service at @domain for the
 * "tcp" protocol.  It then attempts to connect, in turn, to each of
 * the hosts providing the service until either a connection succeeds
 * or there are no hosts remaining.
 *
 * Upon a successful connection, a new #GSocketConnection is constructed
 * and returned.  The caller owns this new object and must drop their
 * reference to it when finished with it.
 *
 * In the event of any failure (DNS error, service not found, no hosts
 * connectable) %NULL is returned and @error (if non-%NULL) is set
 * accordingly.
 *
 * Returns: (transfer full): a #GSocketConnection if successful, or %NULL on error
 */
GSocketConnection *
g_socket_client_connect_to_service (GSocketClient  *client,
				    const gchar    *domain,
				    const gchar    *service,
				    GCancellable   *cancellable,
				    GError        **error)
{
  GSocketConnectable *connectable;
  GSocketConnection *connection;

  connectable = g_network_service_new (service, "tcp", domain);
  connection = g_socket_client_connect (client, connectable,
					cancellable, error);
  g_object_unref (connectable);

  return connection;
}

/**
 * g_socket_client_connect_to_uri:
 * @client: a #GSocketClient
 * @uri: A network URI
 * @default_port: the default port to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a pointer to a #GError, or %NULL
 *
 * This is a helper function for g_socket_client_connect().
 *
 * Attempts to create a TCP connection with a network URI.
 *
 * @uri may be any valid URI containing an "authority" (hostname/port)
 * component. If a port is not specified in the URI, @default_port
 * will be used. TLS will be negotiated if #GSocketClient:tls is %TRUE.
 * (#GSocketClient does not know to automatically assume TLS for
 * certain URI schemes.)
 *
 * Using this rather than g_socket_client_connect() or
 * g_socket_client_connect_to_host() allows #GSocketClient to
 * determine when to use application-specific proxy protocols.
 *
 * Upon a successful connection, a new #GSocketConnection is constructed
 * and returned.  The caller owns this new object and must drop their
 * reference to it when finished with it.
 *
 * In the event of any failure (DNS error, service not found, no hosts
 * connectable) %NULL is returned and @error (if non-%NULL) is set
 * accordingly.
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.26
 */
GSocketConnection *
g_socket_client_connect_to_uri (GSocketClient  *client,
				const gchar    *uri,
				guint16         default_port,
				GCancellable   *cancellable,
				GError        **error)
{
  GSocketConnectable *connectable;
  GSocketConnection *connection;

  connectable = g_network_address_parse_uri (uri, default_port, error);
  if (connectable == NULL)
    return NULL;

  connection = g_socket_client_connect (client, connectable,
					cancellable, error);
  g_object_unref (connectable);

  return connection;
}

typedef struct
{
  GTask *task; /* unowned */
  GSocketClient *client;

  GSocketConnectable *connectable;
  GSocketAddressEnumerator *enumerator;
  GCancellable *enumeration_cancellable;
  GCancellable *enumeration_parent_cancellable;  /* (nullable) (owned) */
  gulong enumeration_cancelled_id;

  GSList *connection_attempts;  /* (element-type ConnectionAttempt) (owned) */
  GSList *successful_connections;
  SocketClientErrorInfo *error_info;

  /* Number of times g_socket_address_enumerator_next_async() has successfully
   * returned an address. */
  guint n_addresses_enumerated;

  gboolean enumeration_completed;
  gboolean connection_in_progress;
  gboolean completed;
} GSocketClientAsyncConnectData;

static void connection_attempt_unref (gpointer attempt);

static void
g_socket_client_async_connect_data_free (GSocketClientAsyncConnectData *data)
{
  data->task = NULL;
  g_clear_object (&data->connectable);
  g_clear_object (&data->enumerator);

  g_cancellable_disconnect (data->enumeration_parent_cancellable, data->enumeration_cancelled_id);
  g_clear_object (&data->enumeration_parent_cancellable);
  data->enumeration_cancelled_id = 0;
  g_clear_object (&data->enumeration_cancellable);

  g_slist_free_full (data->connection_attempts, connection_attempt_unref);
  g_slist_free_full (data->successful_connections, connection_attempt_unref);

  g_clear_pointer (&data->error_info, socket_client_error_info_free);

  g_slice_free (GSocketClientAsyncConnectData, data);
}

typedef struct
{
  GSocketAddress *address;
  GSocket *socket;
  GIOStream *connection;
  GProxyAddress *proxy_addr;
  GSocketClientAsyncConnectData *data; /* unowned */
  GSource *delay_timeout_source;  /* (owned) */
  gboolean delay_reached;
  GCancellable *cancellable;
  GCancellable *task_cancellable;  /* (owned); this is equal to g_task_get_cancellable (ConnectionAttempt.data->task), but with a longer lifetime */
  gulong cancelled_id;
  grefcount ref;
} ConnectionAttempt;

static ConnectionAttempt *
connection_attempt_new (void)
{
  ConnectionAttempt *attempt = g_new0 (ConnectionAttempt, 1);
  g_ref_count_init (&attempt->ref);
  return attempt;
}

static ConnectionAttempt *
connection_attempt_ref (ConnectionAttempt *attempt)
{
  g_ref_count_inc (&attempt->ref);
  return attempt;
}

static void
connection_attempt_unref (gpointer pointer)
{
  ConnectionAttempt *attempt = pointer;
  if (g_ref_count_dec (&attempt->ref))
    {
      g_clear_object (&attempt->address);
      g_clear_object (&attempt->socket);
      g_clear_object (&attempt->connection);
      g_cancellable_disconnect (attempt->task_cancellable, attempt->cancelled_id);
      g_clear_object (&attempt->task_cancellable);
      attempt->cancelled_id = 0;
      g_clear_object (&attempt->cancellable);
      g_clear_object (&attempt->proxy_addr);
      if (attempt->delay_timeout_source)
        {
          g_source_destroy (attempt->delay_timeout_source);
          g_source_unref (attempt->delay_timeout_source);
        }
      g_free (attempt);
    }
}

static void
connection_attempt_remove (ConnectionAttempt *attempt)
{
  GSList *attempt_link = g_slist_find (attempt->data->connection_attempts, attempt);
  if (attempt_link != NULL)
    {
      attempt->data->connection_attempts = g_slist_delete_link (attempt->data->connection_attempts, attempt_link);
      connection_attempt_unref (attempt);
    }
}

static void
cancel_all_attempts (GSocketClientAsyncConnectData *data)
{
  GSList *l;

  for (l = data->connection_attempts; l; l = g_slist_next (l))
    {
      ConnectionAttempt *attempt_entry = l->data;
      g_cancellable_cancel (attempt_entry->cancellable);
      connection_attempt_unref (attempt_entry);
    }
  g_slist_free (data->connection_attempts);
  data->connection_attempts = NULL;

  g_slist_free_full (data->successful_connections, connection_attempt_unref);
  data->successful_connections = NULL;

  g_cancellable_cancel (data->enumeration_cancellable);
}

static void
g_socket_client_async_connect_complete (ConnectionAttempt *attempt)
{
  GSocketClientAsyncConnectData *data = attempt->data;
  GError *error = NULL;
  g_assert (attempt->connection);
  g_assert (!data->completed);

  if (!G_IS_SOCKET_CONNECTION (attempt->connection))
    {
      GSocketConnection *wrapper_connection;

      wrapper_connection = g_tcp_wrapper_connection_new (attempt->connection, attempt->socket);
      g_object_unref (attempt->connection);
      attempt->connection = (GIOStream *)wrapper_connection;
    }

  data->completed = TRUE;
  cancel_all_attempts (data);

  if (g_cancellable_set_error_if_cancelled (g_task_get_cancellable (data->task), &error))
    {
      g_debug ("GSocketClient: Connection cancelled!");
      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_COMPLETE, data->connectable, NULL);
      g_task_return_error (data->task, g_steal_pointer (&error));
    }
  else
    {
      g_debug ("GSocketClient: Connection successful!");
      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_COMPLETE, data->connectable, attempt->connection);
      g_task_return_pointer (data->task, g_steal_pointer (&attempt->connection), g_object_unref);
    }

  connection_attempt_unref (attempt);
  g_object_unref (data->task);
}


static void
g_socket_client_enumerator_callback (GObject      *object,
				     GAsyncResult *result,
				     gpointer      user_data);

static void
enumerator_next_async (GSocketClientAsyncConnectData *data,
                       gboolean                       add_task_ref)
{
  /* Each enumeration takes a ref. This arg just avoids repeated unrefs when
     an enumeration starts another enumeration */
  if (add_task_ref)
    g_object_ref (data->task);

  if (data->n_addresses_enumerated == 0)
    g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_RESOLVING, data->connectable, NULL);
  g_debug ("GSocketClient: Starting new address enumeration");
  g_socket_address_enumerator_next_async (data->enumerator,
					  data->enumeration_cancellable,
					  g_socket_client_enumerator_callback,
					  data);
}

static void try_next_connection_or_finish (GSocketClientAsyncConnectData *, gboolean);

static void
g_socket_client_tls_handshake_callback (GObject      *object,
					GAsyncResult *result,
					gpointer      user_data)
{
  ConnectionAttempt *attempt = user_data;
  GSocketClientAsyncConnectData *data = attempt->data;

  if (g_tls_connection_handshake_finish (G_TLS_CONNECTION (object),
					 result,
					 &data->error_info->tmp_error))
    {
      g_object_unref (attempt->connection);
      attempt->connection = G_IO_STREAM (object);

      g_debug ("GSocketClient: TLS handshake succeeded");
      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_TLS_HANDSHAKED, data->connectable, attempt->connection);
      g_socket_client_async_connect_complete (attempt);
    }
  else
    {
      g_object_unref (object);
      connection_attempt_unref (attempt);

      g_debug ("GSocketClient: TLS handshake failed: %s", data->error_info->tmp_error->message);
      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_TLS_HANDSHAKING);
      try_next_connection_or_finish (data, TRUE);
    }
}

static void
g_socket_client_tls_handshake (ConnectionAttempt *attempt)
{
  GSocketClientAsyncConnectData *data = attempt->data;
  GIOStream *tlsconn;

  if (!data->client->priv->tls)
    {
      g_socket_client_async_connect_complete (attempt);
      return;
    }

  g_debug ("GSocketClient: Starting TLS handshake");
  tlsconn = g_tls_client_connection_new (attempt->connection,
					 data->connectable,
					 &data->error_info->tmp_error);
  if (tlsconn)
    {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      g_tls_client_connection_set_validation_flags (G_TLS_CLIENT_CONNECTION (tlsconn),
                                                    data->client->priv->tls_validation_flags);
G_GNUC_END_IGNORE_DEPRECATIONS
      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_TLS_HANDSHAKING, data->connectable, G_IO_STREAM (tlsconn));

      /* This operation will time out if the underlying #GSocket times out on
       * any part of the TLS handshake. It does not have a higher-level
       * timeout. */
      g_tls_connection_handshake_async (G_TLS_CONNECTION (tlsconn),
					G_PRIORITY_DEFAULT,
					g_task_get_cancellable (data->task),
					g_socket_client_tls_handshake_callback,
					attempt);
    }
  else
    {
      connection_attempt_unref (attempt);

      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_TLS_HANDSHAKING);
      try_next_connection_or_finish (data, TRUE);
    }
}

static void
g_socket_client_proxy_connect_callback (GObject      *object,
					GAsyncResult *result,
					gpointer      user_data)
{
  ConnectionAttempt *attempt = user_data;
  GSocketClientAsyncConnectData *data = attempt->data;

  g_object_unref (attempt->connection);
  attempt->connection = g_proxy_connect_finish (G_PROXY (object),
                                                result,
                                                &data->error_info->tmp_error);
  if (attempt->connection)
    {
      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_PROXY_NEGOTIATED, data->connectable, attempt->connection);
      g_socket_client_tls_handshake (attempt);
    }
  else
    {
      connection_attempt_unref (attempt);

      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);
      try_next_connection_or_finish (data, TRUE);
    }
}

static void
complete_connection_with_error (GSocketClientAsyncConnectData *data,
                                GError                        *error)
{
  g_debug ("GSocketClient: Connection failed: %s", error->message);
  g_assert (!data->completed);

  g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_COMPLETE, data->connectable, NULL);
  data->completed = TRUE;
  cancel_all_attempts (data);
  g_task_return_error (data->task, error);
}

static gboolean
task_completed_or_cancelled (GSocketClientAsyncConnectData *data)
{
  GTask *task = data->task;
  GCancellable *cancellable = g_task_get_cancellable (task);
  GError *error = NULL;

  if (data->completed)
    return TRUE;
  else if (g_cancellable_set_error_if_cancelled (cancellable, &error))
    {
      complete_connection_with_error (data, g_steal_pointer (&error));
      return TRUE;
    }
  else
    return FALSE;
}

/* Returns %TRUE if it’s popped a connection of data->successful_connections and
 * successfully started the next ongoing async operation for that connection. */
static gboolean
try_next_successful_connection (GSocketClientAsyncConnectData *data)
{
  ConnectionAttempt *attempt;
  const gchar *protocol;
  GProxy *proxy;

  if (data->connection_in_progress)
    return FALSE;

  g_assert (data->successful_connections != NULL);
  attempt = data->successful_connections->data;
  g_assert (attempt != NULL);
  data->successful_connections = g_slist_remove (data->successful_connections, attempt);
  data->connection_in_progress = TRUE;

  g_debug ("GSocketClient: Starting application layer connection");

  if (!attempt->proxy_addr)
    {
      g_socket_client_tls_handshake (g_steal_pointer (&attempt));
      return TRUE;
    }

  protocol = g_proxy_address_get_protocol (attempt->proxy_addr);

  /* The connection should not be anything other than TCP,
   * but let's put a safety guard in case
   */
  if (!G_IS_TCP_CONNECTION (attempt->connection))
    {
      g_critical ("Trying to proxy over non-TCP connection, this is "
          "most likely a bug in GLib IO library.");

      g_set_error_literal (&data->error_info->tmp_error,
          G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
          _("Proxying over a non-TCP connection is not supported."));
      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);
    }
  else if (g_hash_table_contains (data->client->priv->app_proxies, protocol))
    {
      /* Simply complete the connection, we don't want to do TLS handshake
       * as the application proxy handling may need proxy handshake first */
      g_socket_client_async_connect_complete (g_steal_pointer (&attempt));
      return TRUE;
    }
  else if ((proxy = g_proxy_get_default_for_protocol (protocol)))
    {
      GIOStream *connection = attempt->connection;
      GProxyAddress *proxy_addr = attempt->proxy_addr;

      g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_PROXY_NEGOTIATING, data->connectable, attempt->connection);
      g_debug ("GSocketClient: Starting proxy connection");

      /* FIXME: The #GProxy implementations do not have well-defined timeout
       * behaviour, so this operation may theoretically never complete or time
       * out. In practice, all #GProxy implementations use a #GSocket and a
       * default timeout on that will eventually be hit. But there is no
       * higher-level timeout. */
      g_proxy_connect_async (proxy,
                             connection,
                             proxy_addr,
                             g_task_get_cancellable (data->task),
                             g_socket_client_proxy_connect_callback,
                             g_steal_pointer (&attempt));
      g_object_unref (proxy);
      return TRUE;
    }
  else
    {
      g_set_error (&data->error_info->tmp_error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
          _("Proxy protocol “%s” is not supported."),
          protocol);
      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_PROXY_NEGOTIATING);
    }

  data->connection_in_progress = FALSE;
  g_clear_pointer (&attempt, connection_attempt_unref);
  return FALSE; /* All non-return paths are failures */
}

static void
try_next_connection_or_finish (GSocketClientAsyncConnectData *data,
                               gboolean                       end_current_connection)
{
  if (end_current_connection)
    data->connection_in_progress = FALSE;

  if (data->connection_in_progress)
    return;

  /* Try to pop and make progress on the next successful_connection. */
  while (data->successful_connections)
    {
      if (try_next_successful_connection (data))
        return;
    }

  /* If there are no more successful_connections which we can make progress on,
   * try the next address enumeration. */
  if (!data->enumeration_completed)
    {
      enumerator_next_async (data, FALSE);
      return;
    }

  complete_connection_with_error (data, g_steal_pointer (&data->error_info->best_error));
  g_object_unref (data->task);
}

static void
g_socket_client_connected_callback (GObject      *source,
				    GAsyncResult *result,
				    gpointer      user_data)
{
  ConnectionAttempt *attempt = g_steal_pointer (&user_data);
  GSocketClientAsyncConnectData *data = attempt->data;

  if (task_completed_or_cancelled (data) || g_cancellable_is_cancelled (attempt->cancellable))
    {
      connection_attempt_remove (attempt);
      connection_attempt_unref (attempt);
      g_object_unref (data->task);
      return;
    }

  if (attempt->delay_timeout_source)
    {
      g_source_destroy (attempt->delay_timeout_source);
      g_clear_pointer (&attempt->delay_timeout_source, g_source_unref);
    }

  if (!g_socket_connection_connect_finish (G_SOCKET_CONNECTION (source),
					   result, &data->error_info->tmp_error))
    {
      if (!g_cancellable_is_cancelled (attempt->cancellable))
        {
          g_debug ("GSocketClient: Connection attempt failed: %s", data->error_info->tmp_error->message);
          clarify_connect_error (data->error_info->tmp_error, data->connectable, attempt->address);
          consider_tmp_error (data->error_info, G_SOCKET_CLIENT_CONNECTING);
          connection_attempt_remove (attempt);
          connection_attempt_unref (attempt);
          try_next_connection_or_finish (data, FALSE);
        }
      else /* Silently ignore cancelled attempts */
        {
          g_clear_error (&data->error_info->tmp_error);
          g_object_unref (data->task);
          connection_attempt_unref (attempt);
        }

      return;
    }

  g_socket_connection_set_cached_remote_address ((GSocketConnection*)attempt->connection, NULL);
  g_debug ("GSocketClient: TCP connection successful");
  g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_CONNECTED, data->connectable, attempt->connection);

  /* wrong, but backward compatible */
  g_socket_set_blocking (attempt->socket, TRUE);

  /* This ends the parallel "happy eyeballs" portion of connecting.
     Now that we have a successful tcp connection we will attempt to connect
     at the TLS/Proxy layer. If those layers fail we will move on to the next
     connection.
   */
  connection_attempt_remove (attempt);
  data->successful_connections = g_slist_append (data->successful_connections, g_steal_pointer (&attempt));
  try_next_connection_or_finish (data, FALSE);
}

static gboolean
on_connection_attempt_delay_reached (gpointer data)
{
  ConnectionAttempt *attempt = data;

  g_assert (!attempt->delay_reached);
  attempt->delay_reached = TRUE;

  if (!attempt->data->enumeration_completed)
    {
      g_debug ("GSocketClient: Connection attempt delay reached, trying another enumeration");
      enumerator_next_async (attempt->data, TRUE);
    }

  g_clear_pointer (&attempt->delay_timeout_source, g_source_unref);
  return G_SOURCE_REMOVE;
}

static void
on_connection_cancelled (GCancellable *cancellable,
                         gpointer      data)
{
  GCancellable *linked_cancellable = G_CANCELLABLE (data);

  g_cancellable_cancel (linked_cancellable);
}

static void
g_socket_client_enumerator_callback (GObject      *object,
				     GAsyncResult *result,
				     gpointer      user_data)
{
  GSocketClientAsyncConnectData *data = user_data;
  GSocketAddress *address = NULL;
  GSocket *socket;
  ConnectionAttempt *attempt;

  if (task_completed_or_cancelled (data))
    {
      g_object_unref (data->task);
      return;
    }

  address = g_socket_address_enumerator_next_finish (data->enumerator,
						     result, &data->error_info->tmp_error);
  if (address == NULL)
    {
      if (G_UNLIKELY (data->enumeration_completed))
        return;

      data->enumeration_completed = TRUE;
      g_debug ("GSocketClient: Address enumeration completed (out of addresses)");

      /* As per API docs: We only care about error if it's the first call,
         after that the enumerator is done.

         Note that we don't care about cancellation errors because
         task_completed_or_cancelled() above should handle that.

         If this fails and nothing is in progress then we will complete task here.
       */
      if ((data->n_addresses_enumerated > 0 && !data->connection_attempts && !data->connection_in_progress) ||
          data->n_addresses_enumerated == 0)
        {
          g_debug ("GSocketClient: Address enumeration failed: %s",
                   data->error_info->tmp_error ? data->error_info->tmp_error->message : NULL);
          consider_tmp_error (data->error_info, G_SOCKET_CLIENT_RESOLVING);
          g_assert (data->error_info->best_error);
          complete_connection_with_error (data, g_steal_pointer (&data->error_info->best_error));
        }

      /* Enumeration should never trigger again, drop our ref */
      g_object_unref (data->task);
      return;
    }

  g_debug ("GSocketClient: Address enumeration succeeded");
  if (data->n_addresses_enumerated == 0)
    g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_RESOLVED,
                                data->connectable, NULL);

  data->n_addresses_enumerated++;

  socket = create_socket (data->client, address, &data->error_info->tmp_error);
  if (socket == NULL)
    {
      g_object_unref (address);
      consider_tmp_error (data->error_info, G_SOCKET_CLIENT_CONNECTING);
      enumerator_next_async (data, FALSE);
      return;
    }

  attempt = connection_attempt_new ();
  attempt->data = data;
  attempt->socket = socket;
  attempt->address = address;
  attempt->cancellable = g_cancellable_new ();
  attempt->connection = (GIOStream *)g_socket_connection_factory_create_connection (socket);
  attempt->delay_timeout_source = g_timeout_source_new (HAPPY_EYEBALLS_CONNECTION_ATTEMPT_DELAY_MS);

  g_debug ("%s: starting connection attempt %p for GSocketClientAsyncConnectData %p",
           G_STRFUNC, attempt, data);

  if (G_IS_PROXY_ADDRESS (address) && data->client->priv->enable_proxy)
    attempt->proxy_addr = g_object_ref (G_PROXY_ADDRESS (address));

  g_source_set_callback (attempt->delay_timeout_source, on_connection_attempt_delay_reached, attempt, NULL);
  g_source_attach (attempt->delay_timeout_source, g_task_get_context (data->task));
  data->connection_attempts = g_slist_append (data->connection_attempts, connection_attempt_ref (attempt));

  if (g_task_get_cancellable (data->task))
    {
      attempt->task_cancellable = g_object_ref (g_task_get_cancellable (data->task));
      attempt->cancelled_id =
          g_cancellable_connect (attempt->task_cancellable, G_CALLBACK (on_connection_cancelled),
                                 g_object_ref (attempt->cancellable), g_object_unref);
    }

  g_socket_connection_set_cached_remote_address ((GSocketConnection *)attempt->connection, address);
  g_debug ("GSocketClient: Starting TCP connection attempt");
  g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_CONNECTING, data->connectable, attempt->connection);

  /* If client->priv->timeout is set, this async operation will time out after
   * then. Otherwise it will continue until the kernel timeouts for a
   * non-blocking connect() call (if any) are hit. */
  g_socket_connection_connect_async (G_SOCKET_CONNECTION (attempt->connection),
				     address,
				     attempt->cancellable,
				     g_socket_client_connected_callback, attempt  /* transfer full */);
}

/**
 * g_socket_client_connect_async:
 * @client: a #GSocketClient
 * @connectable: a #GSocketConnectable specifying the remote address.
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: (scope async): a #GAsyncReadyCallback
 * @user_data: user data for the callback
 *
 * This is the asynchronous version of g_socket_client_connect().
 *
 * You may wish to prefer the asynchronous version even in synchronous
 * command line programs because, since 2.60, it implements
 * [RFC 8305](https://tools.ietf.org/html/rfc8305) "Happy Eyeballs"
 * recommendations to work around long connection timeouts in networks
 * where IPv6 is broken by performing an IPv4 connection simultaneously
 * without waiting for IPv6 to time out, which is not supported by the
 * synchronous call. (This is not an API guarantee, and may change in
 * the future.)
 *
 * When the operation is finished @callback will be
 * called. You can then call g_socket_client_connect_finish() to get
 * the result of the operation.
 *
 * Since: 2.22
 */
void
g_socket_client_connect_async (GSocketClient       *client,
			       GSocketConnectable  *connectable,
			       GCancellable        *cancellable,
			       GAsyncReadyCallback  callback,
			       gpointer             user_data)
{
  GSocketClientAsyncConnectData *data;

  g_return_if_fail (G_IS_SOCKET_CLIENT (client));

  data = g_slice_new0 (GSocketClientAsyncConnectData);
  data->client = client;
  data->connectable = g_object_ref (connectable);
  data->error_info = socket_client_error_info_new ();

  if (can_use_proxy (client))
    {
      data->enumerator = g_socket_connectable_proxy_enumerate (connectable);
      if (client->priv->proxy_resolver &&
          G_IS_PROXY_ADDRESS_ENUMERATOR (data->enumerator))
        {
          g_object_set (G_OBJECT (data->enumerator),
                        "proxy-resolver", client->priv->proxy_resolver,
                        NULL);
        }
    }
  else
    data->enumerator = g_socket_connectable_enumerate (connectable);

  /* This function tries to match the behavior of g_socket_client_connect ()
     which is simple enough but much of it is done in parallel to be as responsive
     as possible as per Happy Eyeballs (RFC 8305). This complicates flow quite a
     bit but we can describe it in 3 sections:

     Firstly we have address enumeration (DNS):
       - This may be triggered multiple times by enumerator_next_async().
       - It also has its own cancellable (data->enumeration_cancellable).
       - Enumeration is done lazily because GNetworkAddressAddressEnumerator
         also does work in parallel and may lazily add new addresses.
       - If the first enumeration errors then the task errors. Otherwise all enumerations
         will potentially be used (until task or enumeration is cancelled).

      Then we start attempting connections (TCP):
        - Each connection is independent and kept in a ConnectionAttempt object.
          - They each hold a ref on the main task and have their own cancellable.
        - Multiple attempts may happen in parallel as per Happy Eyeballs.
        - Upon failure or attempt delays being reached more connection attempts are made.
          - If no connections succeed the task errors.
        - Upon success they are kept in a list of successful connections.

      Lastly we connect at the application layer (TLS, Proxies):
        - These are done in serial.
          - The reasoning here is that Happy Eyeballs is about making bad connections responsive
            at the IP/TCP layers. Issues at the application layer are generally not due to
            connectivity issues but rather misconfiguration.
        - Upon failure it will try the next TCP connection until it runs out and
          the task errors.
        - Upon success it cancels everything remaining (enumeration and connections)
          and returns the connection.
  */

  data->task = g_task_new (client, cancellable, callback, user_data);
  g_task_set_check_cancellable (data->task, FALSE); /* We handle this manually */
  g_task_set_source_tag (data->task, g_socket_client_connect_async);
  g_task_set_task_data (data->task, data, (GDestroyNotify)g_socket_client_async_connect_data_free);

  data->enumeration_cancellable = g_cancellable_new ();
  if (cancellable)
    {
      data->enumeration_parent_cancellable = g_object_ref (cancellable);
      data->enumeration_cancelled_id =
          g_cancellable_connect (cancellable, G_CALLBACK (on_connection_cancelled),
                                 g_object_ref (data->enumeration_cancellable), g_object_unref);
    }

  g_debug ("%s: starting new g_socket_client_connect_async() with GTask %p "
           "and GSocketClientAsyncConnectData %p",
           G_STRFUNC, data->task, data);

  enumerator_next_async (data, FALSE);
}

/**
 * g_socket_client_connect_to_host_async:
 * @client: a #GSocketClient
 * @host_and_port: the name and optionally the port of the host to connect to
 * @default_port: the default port to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: (scope async): a #GAsyncReadyCallback
 * @user_data: user data for the callback
 *
 * This is the asynchronous version of g_socket_client_connect_to_host().
 *
 * When the operation is finished @callback will be
 * called. You can then call g_socket_client_connect_to_host_finish() to get
 * the result of the operation.
 *
 * Since: 2.22
 */
void
g_socket_client_connect_to_host_async (GSocketClient        *client,
				       const gchar          *host_and_port,
				       guint16               default_port,
				       GCancellable         *cancellable,
				       GAsyncReadyCallback   callback,
				       gpointer              user_data)
{
  GSocketConnectable *connectable;
  GError *error;

  error = NULL;
  connectable = g_network_address_parse (host_and_port, default_port,
					 &error);
  if (connectable == NULL)
    {
      g_task_report_error (client, callback, user_data,
                           g_socket_client_connect_to_host_async,
                           error);
    }
  else
    {
      g_socket_client_connect_async (client,
				     connectable, cancellable,
				     callback, user_data);
      g_object_unref (connectable);
    }
}

/**
 * g_socket_client_connect_to_service_async:
 * @client: a #GSocketClient
 * @domain: a domain name
 * @service: the name of the service to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: (scope async): a #GAsyncReadyCallback
 * @user_data: user data for the callback
 *
 * This is the asynchronous version of
 * g_socket_client_connect_to_service().
 *
 * Since: 2.22
 */
void
g_socket_client_connect_to_service_async (GSocketClient       *client,
					  const gchar         *domain,
					  const gchar         *service,
					  GCancellable        *cancellable,
					  GAsyncReadyCallback  callback,
					  gpointer             user_data)
{
  GSocketConnectable *connectable;

  connectable = g_network_service_new (service, "tcp", domain);
  g_socket_client_connect_async (client,
				 connectable, cancellable,
				 callback, user_data);
  g_object_unref (connectable);
}

/**
 * g_socket_client_connect_to_uri_async:
 * @client: a #GSocketClient
 * @uri: a network uri
 * @default_port: the default port to connect to
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: (scope async): a #GAsyncReadyCallback
 * @user_data: user data for the callback
 *
 * This is the asynchronous version of g_socket_client_connect_to_uri().
 *
 * When the operation is finished @callback will be
 * called. You can then call g_socket_client_connect_to_uri_finish() to get
 * the result of the operation.
 *
 * Since: 2.26
 */
void
g_socket_client_connect_to_uri_async (GSocketClient        *client,
				      const gchar          *uri,
				      guint16               default_port,
				      GCancellable         *cancellable,
				      GAsyncReadyCallback   callback,
				      gpointer              user_data)
{
  GSocketConnectable *connectable;
  GError *error;

  error = NULL;
  connectable = g_network_address_parse_uri (uri, default_port, &error);
  if (connectable == NULL)
    {
      g_task_report_error (client, callback, user_data,
                           g_socket_client_connect_to_uri_async,
                           error);
    }
  else
    {
      g_debug("g_socket_client_connect_to_uri_async");
      g_socket_client_connect_async (client,
				     connectable, cancellable,
				     callback, user_data);
      g_object_unref (connectable);
    }
}


/**
 * g_socket_client_connect_finish:
 * @client: a #GSocketClient.
 * @result: a #GAsyncResult.
 * @error: a #GError location to store the error occurring, or %NULL to
 * ignore.
 *
 * Finishes an async connect operation. See g_socket_client_connect_async()
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.22
 */
GSocketConnection *
g_socket_client_connect_finish (GSocketClient  *client,
				GAsyncResult   *result,
				GError        **error)
{
  g_return_val_if_fail (g_task_is_valid (result, client), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * g_socket_client_connect_to_host_finish:
 * @client: a #GSocketClient.
 * @result: a #GAsyncResult.
 * @error: a #GError location to store the error occurring, or %NULL to
 * ignore.
 *
 * Finishes an async connect operation. See g_socket_client_connect_to_host_async()
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.22
 */
GSocketConnection *
g_socket_client_connect_to_host_finish (GSocketClient  *client,
					GAsyncResult   *result,
					GError        **error)
{
  return g_socket_client_connect_finish (client, result, error);
}

/**
 * g_socket_client_connect_to_service_finish:
 * @client: a #GSocketClient.
 * @result: a #GAsyncResult.
 * @error: a #GError location to store the error occurring, or %NULL to
 * ignore.
 *
 * Finishes an async connect operation. See g_socket_client_connect_to_service_async()
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.22
 */
GSocketConnection *
g_socket_client_connect_to_service_finish (GSocketClient  *client,
					   GAsyncResult   *result,
					   GError        **error)
{
  return g_socket_client_connect_finish (client, result, error);
}

/**
 * g_socket_client_connect_to_uri_finish:
 * @client: a #GSocketClient.
 * @result: a #GAsyncResult.
 * @error: a #GError location to store the error occurring, or %NULL to
 * ignore.
 *
 * Finishes an async connect operation. See g_socket_client_connect_to_uri_async()
 *
 * Returns: (transfer full): a #GSocketConnection on success, %NULL on error.
 *
 * Since: 2.26
 */
GSocketConnection *
g_socket_client_connect_to_uri_finish (GSocketClient  *client,
				       GAsyncResult   *result,
				       GError        **error)
{
  return g_socket_client_connect_finish (client, result, error);
}

/**
 * g_socket_client_add_application_proxy:
 * @client: a #GSocketClient
 * @protocol: The proxy protocol
 *
 * Enable proxy protocols to be handled by the application. When the
 * indicated proxy protocol is returned by the #GProxyResolver,
 * #GSocketClient will consider this protocol as supported but will
 * not try to find a #GProxy instance to handle handshaking. The
 * application must check for this case by calling
 * g_socket_connection_get_remote_address() on the returned
 * #GSocketConnection, and seeing if it's a #GProxyAddress of the
 * appropriate type, to determine whether or not it needs to handle
 * the proxy handshaking itself.
 *
 * This should be used for proxy protocols that are dialects of
 * another protocol such as HTTP proxy. It also allows cohabitation of
 * proxy protocols that are reused between protocols. A good example
 * is HTTP. It can be used to proxy HTTP, FTP and Gopher and can also
 * be use as generic socket proxy through the HTTP CONNECT method.
 *
 * When the proxy is detected as being an application proxy, TLS handshake
 * will be skipped. This is required to let the application do the proxy
 * specific handshake.
 */
void
g_socket_client_add_application_proxy (GSocketClient *client,
			               const gchar   *protocol)
{
  g_hash_table_add (client->priv->app_proxies, g_strdup (protocol));
}
