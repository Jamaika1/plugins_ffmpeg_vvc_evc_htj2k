/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright © 2010 Red Hat, Inc
 * Copyright © 2015 Collabora, Ltd.
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
 */

#include "config.h"
#include "../glib/glib.h"

#include "gdtlsconnection.h"
#include "gcancellable.h"
#include "gioenumtypes.h"
#include "gsocket.h"
#include "gtlsbackend.h"
#include "gtlscertificate.h"
#include "gtlsconnection.h"
#include "gdtlsclientconnection.h"
#include "gtlsdatabase.h"
#include "gtlsinteraction.h"
#include "../glib/glibintl.h"
#include "gmarshal-internal.h"

/**
 * GDtlsConnection:
 *
 * `GDtlsConnection` is the base DTLS connection class type, which wraps
 * a [iface@Gio.DatagramBased] and provides DTLS encryption on top of it. Its
 * subclasses, [iface@Gio.DtlsClientConnection] and
 * [iface@Gio.DtlsServerConnection], implement client-side and server-side DTLS,
 * respectively.
 *
 * For TLS support, see [class@Gio.TlsConnection].
 *
 * As DTLS is datagram based, `GDtlsConnection` implements
 * [iface@Gio.DatagramBased], presenting a datagram-socket-like API for the
 * encrypted connection. This operates over a base datagram connection, which is
 * also a `GDatagramBased` ([property@Gio.DtlsConnection:base-socket]).
 *
 * To close a DTLS connection, use [method@Gio.DtlsConnection.close].
 *
 * Neither [iface@Gio.DtlsServerConnection] or [iface@Gio.DtlsClientConnection]
 * set the peer address on their base [iface@Gio.DatagramBased] if it is a
 * [class@Gio.Socket] — it is up to the caller to do that if they wish. If they
 * do not, and [method@Gio.Socket.close] is called on the base socket, the
 * `GDtlsConnection` will not raise a `G_IO_ERROR_NOT_CONNECTED` error on
 * further I/O.
 *
 * Since: 2.48
 */
G_DEFINE_INTERFACE (GDtlsConnection, g_dtls_connection, G_TYPE_DATAGRAM_BASED)

enum {
  ACCEPT_CERTIFICATE,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
  PROP_BASE_SOCKET = 1,
  PROP_REQUIRE_CLOSE_NOTIFY,
  PROP_REHANDSHAKE_MODE,
  PROP_DATABASE,
  PROP_INTERACTION,
  PROP_CERTIFICATE,
  PROP_PEER_CERTIFICATE,
  PROP_PEER_CERTIFICATE_ERRORS,
  PROP_PROTOCOL_VERSION,
  PROP_CIPHERSUITE_NAME,
};

static void
g_dtls_connection_default_init (GDtlsConnectionInterface *iface)
{
  /**
   * GDtlsConnection:base-socket:
   *
   * The #GDatagramBased that the connection wraps. Note that this may be any
   * implementation of #GDatagramBased, not just a #GSocket.
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("base-socket", NULL, NULL,
                                                            G_TYPE_DATAGRAM_BASED,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT_ONLY |
                                                            G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:database: (nullable)
   *
   * The certificate database to use when verifying this TLS connection.
   * If no certificate database is set, then the default database will be
   * used. See g_tls_backend_get_default_database().
   *
   * When using a non-default database, #GDtlsConnection must fall back to using
   * the #GTlsDatabase to perform certificate verification using
   * g_tls_database_verify_chain(), which means certificate verification will
   * not be able to make use of TLS session context. This may be less secure.
   * For example, if you create your own #GTlsDatabase that just wraps the
   * default #GTlsDatabase, you might expect that you have not changed anything,
   * but this is not true because you may have altered the behavior of
   * #GDtlsConnection by causing it to use g_tls_database_verify_chain(). See the
   * documentation of g_tls_database_verify_chain() for more details on specific
   * security checks that may not be performed. Accordingly, setting a
   * non-default database is discouraged except for specialty applications with
   * unusual security requirements.
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("database", NULL, NULL,
                                                            G_TYPE_TLS_DATABASE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:interaction: (nullable)
   *
   * A #GTlsInteraction object to be used when the connection or certificate
   * database need to interact with the user. This will be used to prompt the
   * user for passwords where necessary.
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("interaction", NULL, NULL,
                                                            G_TYPE_TLS_INTERACTION,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:require-close-notify:
   *
   * Whether or not proper TLS close notification is required.
   * See g_dtls_connection_set_require_close_notify().
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_boolean ("require-close-notify", NULL, NULL,
                                                             TRUE,
                                                             G_PARAM_READWRITE |
                                                             G_PARAM_CONSTRUCT |
                                                             G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:rehandshake-mode:
   *
   * The rehandshaking mode. See
   * g_dtls_connection_set_rehandshake_mode().
   *
   * Since: 2.48
   *
   * Deprecated: 2.60: The rehandshake mode is ignored.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_enum ("rehandshake-mode", NULL, NULL,
                                                          G_TYPE_TLS_REHANDSHAKE_MODE,
                                                          G_TLS_REHANDSHAKE_NEVER,
                                                          G_PARAM_READWRITE |
                                                          G_PARAM_CONSTRUCT |
                                                          G_PARAM_STATIC_STRINGS |
                                                          G_PARAM_DEPRECATED));
  /**
   * GDtlsConnection:certificate:
   *
   * The connection's certificate; see
   * g_dtls_connection_set_certificate().
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("certificate", NULL, NULL,
                                                            G_TYPE_TLS_CERTIFICATE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:peer-certificate: (nullable)
   *
   * The connection's peer's certificate, after the TLS handshake has
   * completed or failed. Note in particular that this is not yet set
   * during the emission of #GDtlsConnection::accept-certificate.
   *
   * (You can watch for a #GObject::notify signal on this property to
   * detect when a handshake has occurred.)
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("peer-certificate", NULL, NULL,
                                                            G_TYPE_TLS_CERTIFICATE,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:peer-certificate-errors:
   *
   * The errors noticed while verifying
   * #GDtlsConnection:peer-certificate. Normally this should be 0, but
   * it may not be if #GDtlsClientConnection:validation-flags is not
   * %G_TLS_CERTIFICATE_VALIDATE_ALL, or if
   * #GDtlsConnection::accept-certificate overrode the default
   * behavior.
   *
   * GLib guarantees that if certificate verification fails, at least
   * one error will be set, but it does not guarantee that all possible
   * errors will be set. Accordingly, you may not safely decide to
   * ignore any particular type of error. For example, it would be
   * incorrect to mask %G_TLS_CERTIFICATE_EXPIRED if you want to allow
   * expired certificates, because this could potentially be the only
   * error flag set even if other problems exist with the certificate.
   *
   * Since: 2.48
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_flags ("peer-certificate-errors", NULL, NULL,
                                                           G_TYPE_TLS_CERTIFICATE_FLAGS,
                                                           0,
                                                           G_PARAM_READABLE |
                                                           G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:advertised-protocols: (nullable)
   *
   * The list of application-layer protocols that the connection
   * advertises that it is willing to speak. See
   * g_dtls_connection_set_advertised_protocols().
   *
   * Since: 2.60
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_boxed ("advertised-protocols", NULL, NULL,
                                                           G_TYPE_STRV,
                                                           G_PARAM_READWRITE |
                                                           G_PARAM_STATIC_STRINGS));
  /**
   * GDtlsConnection:negotiated-protocol:
   *
   * The application-layer protocol negotiated during the TLS
   * handshake. See g_dtls_connection_get_negotiated_protocol().
   *
   * Since: 2.60
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("negotiated-protocol", NULL, NULL,
                                                            NULL,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_STRINGS));

  /**
   * GDtlsConnection:protocol-version:
   *
   * The DTLS protocol version in use. See g_dtls_connection_get_protocol_version().
   *
   * Since: 2.70
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_enum ("protocol-version", NULL, NULL,
                                                          G_TYPE_TLS_PROTOCOL_VERSION,
                                                          G_TLS_PROTOCOL_VERSION_UNKNOWN,
                                                          G_PARAM_READABLE |
                                                          G_PARAM_STATIC_STRINGS));

  /**
   * GDtlsConnection:ciphersuite-name: (nullable)
   *
   * The name of the DTLS ciphersuite in use. See g_dtls_connection_get_ciphersuite_name().
   *
   * Since: 2.70
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("ciphersuite-name", NULL, NULL,
                                                            NULL,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_STRINGS));

  /**
   * GDtlsConnection::accept-certificate:
   * @conn: a #GDtlsConnection
   * @peer_cert: the peer's #GTlsCertificate
   * @errors: the problems with @peer_cert.
   *
   * Emitted during the TLS handshake after the peer certificate has
   * been received. You can examine @peer_cert's certification path by
   * calling g_tls_certificate_get_issuer() on it.
   *
   * For a client-side connection, @peer_cert is the server's
   * certificate, and the signal will only be emitted if the
   * certificate was not acceptable according to @conn's
   * #GDtlsClientConnection:validation_flags. If you would like the
   * certificate to be accepted despite @errors, return %TRUE from the
   * signal handler. Otherwise, if no handler accepts the certificate,
   * the handshake will fail with %G_TLS_ERROR_BAD_CERTIFICATE.
   *
   * GLib guarantees that if certificate verification fails, this signal
   * will be emitted with at least one error will be set in @errors, but
   * it does not guarantee that all possible errors will be set.
   * Accordingly, you may not safely decide to ignore any particular
   * type of error. For example, it would be incorrect to ignore
   * %G_TLS_CERTIFICATE_EXPIRED if you want to allow expired
   * certificates, because this could potentially be the only error flag
   * set even if other problems exist with the certificate.
   *
   * For a server-side connection, @peer_cert is the certificate
   * presented by the client, if this was requested via the server's
   * #GDtlsServerConnection:authentication_mode. On the server side,
   * the signal is always emitted when the client presents a
   * certificate, and the certificate will only be accepted if a
   * handler returns %TRUE.
   *
   * Note that if this signal is emitted as part of asynchronous I/O
   * in the main thread, then you should not attempt to interact with
   * the user before returning from the signal handler. If you want to
   * let the user decide whether or not to accept the certificate, you
   * would have to return %FALSE from the signal handler on the first
   * attempt, and then after the connection attempt returns a
   * %G_TLS_ERROR_BAD_CERTIFICATE, you can interact with the user, and
   * if the user decides to accept the certificate, remember that fact,
   * create a new connection, and return %TRUE from the signal handler
   * the next time.
   *
   * If you are doing I/O in another thread, you do not
   * need to worry about this, and can simply block in the signal
   * handler until the UI thread returns an answer.
   *
   * Returns: %TRUE to accept @peer_cert (which will also
   * immediately end the signal emission). %FALSE to allow the signal
   * emission to continue, which will cause the handshake to fail if
   * no one else overrides it.
   *
   * Since: 2.48
   */
  signals[ACCEPT_CERTIFICATE] =
    g_signal_new (I_("accept-certificate"),
                  G_TYPE_DTLS_CONNECTION,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GDtlsConnectionInterface, accept_certificate),
                  g_signal_accumulator_true_handled, NULL,
                  _g_cclosure_marshal_BOOLEAN__OBJECT_FLAGS,
                  G_TYPE_BOOLEAN, 2,
                  G_TYPE_TLS_CERTIFICATE,
                  G_TYPE_TLS_CERTIFICATE_FLAGS);
  g_signal_set_va_marshaller (signals[ACCEPT_CERTIFICATE],
                              G_TYPE_FROM_INTERFACE (iface),
                              _g_cclosure_marshal_BOOLEAN__OBJECT_FLAGSv);
}

/**
 * g_dtls_connection_set_database:
 * @conn: a #GDtlsConnection
 * @database: (nullable): a #GTlsDatabase
 *
 * Sets the certificate database that is used to verify peer certificates.
 * This is set to the default database by default. See
 * g_tls_backend_get_default_database(). If set to %NULL, then
 * peer certificate validation will always set the
 * %G_TLS_CERTIFICATE_UNKNOWN_CA error (meaning
 * #GDtlsConnection::accept-certificate will always be emitted on
 * client-side connections, unless that bit is not set in
 * #GDtlsClientConnection:validation-flags).
 *
 * There are nonintuitive security implications when using a non-default
 * database. See #GDtlsConnection:database for details.
 *
 * Since: 2.48
 */
void
g_dtls_connection_set_database (GDtlsConnection *conn,
                                GTlsDatabase    *database)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));
  g_return_if_fail (database == NULL || G_IS_TLS_DATABASE (database));

  g_object_set (G_OBJECT (conn),
                "database", database,
                NULL);
}

/**
 * g_dtls_connection_get_database:
 * @conn: a #GDtlsConnection
 *
 * Gets the certificate database that @conn uses to verify
 * peer certificates. See g_dtls_connection_set_database().
 *
 * Returns: (transfer none) (nullable): the certificate database that @conn uses or %NULL
 *
 * Since: 2.48
 */
GTlsDatabase*
g_dtls_connection_get_database (GDtlsConnection *conn)
{
  GTlsDatabase *database = NULL;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), NULL);

  g_object_get (G_OBJECT (conn),
                "database", &database,
                NULL);
  if (database)
    g_object_unref (database);
  return database;
}

/**
 * g_dtls_connection_set_certificate:
 * @conn: a #GDtlsConnection
 * @certificate: the certificate to use for @conn
 *
 * This sets the certificate that @conn will present to its peer
 * during the TLS handshake. For a #GDtlsServerConnection, it is
 * mandatory to set this, and that will normally be done at construct
 * time.
 *
 * For a #GDtlsClientConnection, this is optional. If a handshake fails
 * with %G_TLS_ERROR_CERTIFICATE_REQUIRED, that means that the server
 * requires a certificate, and if you try connecting again, you should
 * call this method first. You can call
 * g_dtls_client_connection_get_accepted_cas() on the failed connection
 * to get a list of Certificate Authorities that the server will
 * accept certificates from.
 *
 * (It is also possible that a server will allow the connection with
 * or without a certificate; in that case, if you don't provide a
 * certificate, you can tell that the server requested one by the fact
 * that g_dtls_client_connection_get_accepted_cas() will return
 * non-%NULL.)
 *
 * Since: 2.48
 */
void
g_dtls_connection_set_certificate (GDtlsConnection *conn,
                                   GTlsCertificate *certificate)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));
  g_return_if_fail (G_IS_TLS_CERTIFICATE (certificate));

  g_object_set (G_OBJECT (conn), "certificate", certificate, NULL);
}

/**
 * g_dtls_connection_get_certificate:
 * @conn: a #GDtlsConnection
 *
 * Gets @conn's certificate, as set by
 * g_dtls_connection_set_certificate().
 *
 * Returns: (transfer none) (nullable): @conn's certificate, or %NULL
 *
 * Since: 2.48
 */
GTlsCertificate *
g_dtls_connection_get_certificate (GDtlsConnection *conn)
{
  GTlsCertificate *certificate;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), NULL);

  g_object_get (G_OBJECT (conn), "certificate", &certificate, NULL);
  if (certificate)
    g_object_unref (certificate);

  return certificate;
}

/**
 * g_dtls_connection_set_interaction:
 * @conn: a connection
 * @interaction: (nullable): an interaction object, or %NULL
 *
 * Set the object that will be used to interact with the user. It will be used
 * for things like prompting the user for passwords.
 *
 * The @interaction argument will normally be a derived subclass of
 * #GTlsInteraction. %NULL can also be provided if no user interaction
 * should occur for this connection.
 *
 * Since: 2.48
 */
void
g_dtls_connection_set_interaction (GDtlsConnection *conn,
                                   GTlsInteraction *interaction)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));
  g_return_if_fail (interaction == NULL || G_IS_TLS_INTERACTION (interaction));

  g_object_set (G_OBJECT (conn), "interaction", interaction, NULL);
}

/**
 * g_dtls_connection_get_interaction:
 * @conn: a connection
 *
 * Get the object that will be used to interact with the user. It will be used
 * for things like prompting the user for passwords. If %NULL is returned, then
 * no user interaction will occur for this connection.
 *
 * Returns: (transfer none) (nullable): The interaction object.
 *
 * Since: 2.48
 */
GTlsInteraction *
g_dtls_connection_get_interaction (GDtlsConnection       *conn)
{
  GTlsInteraction *interaction = NULL;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), NULL);

  g_object_get (G_OBJECT (conn), "interaction", &interaction, NULL);
  if (interaction)
    g_object_unref (interaction);

  return interaction;
}

/**
 * g_dtls_connection_get_peer_certificate:
 * @conn: a #GDtlsConnection
 *
 * Gets @conn's peer's certificate after the handshake has completed
 * or failed. (It is not set during the emission of
 * #GDtlsConnection::accept-certificate.)
 *
 * Returns: (transfer none) (nullable): @conn's peer's certificate, or %NULL
 *
 * Since: 2.48
 */
GTlsCertificate *
g_dtls_connection_get_peer_certificate (GDtlsConnection *conn)
{
  GTlsCertificate *peer_certificate;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), NULL);

  g_object_get (G_OBJECT (conn), "peer-certificate", &peer_certificate, NULL);
  if (peer_certificate)
    g_object_unref (peer_certificate);

  return peer_certificate;
}

/**
 * g_dtls_connection_get_peer_certificate_errors:
 * @conn: a #GDtlsConnection
 *
 * Gets the errors associated with validating @conn's peer's
 * certificate, after the handshake has completed or failed. (It is
 * not set during the emission of #GDtlsConnection::accept-certificate.)
 *
 * Returns: @conn's peer's certificate errors
 *
 * Since: 2.48
 */
GTlsCertificateFlags
g_dtls_connection_get_peer_certificate_errors (GDtlsConnection *conn)
{
  GTlsCertificateFlags errors;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), 0);

  g_object_get (G_OBJECT (conn), "peer-certificate-errors", &errors, NULL);
  return errors;
}

/**
 * g_dtls_connection_set_require_close_notify:
 * @conn: a #GDtlsConnection
 * @require_close_notify: whether or not to require close notification
 *
 * Sets whether or not @conn expects a proper TLS close notification
 * before the connection is closed. If this is %TRUE (the default),
 * then @conn will expect to receive a TLS close notification from its
 * peer before the connection is closed, and will return a
 * %G_TLS_ERROR_EOF error if the connection is closed without proper
 * notification (since this may indicate a network error, or
 * man-in-the-middle attack).
 *
 * In some protocols, the application will know whether or not the
 * connection was closed cleanly based on application-level data
 * (because the application-level data includes a length field, or is
 * somehow self-delimiting); in this case, the close notify is
 * redundant and may be omitted. You
 * can use g_dtls_connection_set_require_close_notify() to tell @conn
 * to allow an "unannounced" connection close, in which case the close
 * will show up as a 0-length read, as in a non-TLS
 * #GDatagramBased, and it is up to the application to check that
 * the data has been fully received.
 *
 * Note that this only affects the behavior when the peer closes the
 * connection; when the application calls g_dtls_connection_close_async() on
 * @conn itself, this will send a close notification regardless of the
 * setting of this property. If you explicitly want to do an unclean
 * close, you can close @conn's #GDtlsConnection:base-socket rather
 * than closing @conn itself.
 *
 * Since: 2.48
 */
void
g_dtls_connection_set_require_close_notify (GDtlsConnection *conn,
                                            gboolean         require_close_notify)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));

  g_object_set (G_OBJECT (conn),
                "require-close-notify", require_close_notify,
                NULL);
}

/**
 * g_dtls_connection_get_require_close_notify:
 * @conn: a #GDtlsConnection
 *
 * Tests whether or not @conn expects a proper TLS close notification
 * when the connection is closed. See
 * g_dtls_connection_set_require_close_notify() for details.
 *
 * Returns: %TRUE if @conn requires a proper TLS close notification.
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_get_require_close_notify (GDtlsConnection *conn)
{
  gboolean require_close_notify;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), TRUE);

  g_object_get (G_OBJECT (conn),
                "require-close-notify", &require_close_notify,
                NULL);
  return require_close_notify;
}

/**
 * g_dtls_connection_set_rehandshake_mode:
 * @conn: a #GDtlsConnection
 * @mode: the rehandshaking mode
 *
 * Since GLib 2.64, changing the rehandshake mode is no longer supported
 * and will have no effect. With TLS 1.3, rehandshaking has been removed from
 * the TLS protocol, replaced by separate post-handshake authentication and
 * rekey operations.
 *
 * Since: 2.48
 *
 * Deprecated: 2.60. Changing the rehandshake mode is no longer
 *   required for compatibility. Also, rehandshaking has been removed
 *   from the TLS protocol in TLS 1.3.
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
void
g_dtls_connection_set_rehandshake_mode (GDtlsConnection     *conn,
                                        GTlsRehandshakeMode  mode)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));

  g_object_set (G_OBJECT (conn),
                "rehandshake-mode", G_TLS_REHANDSHAKE_SAFELY,
                NULL);
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_dtls_connection_get_rehandshake_mode:
 * @conn: a #GDtlsConnection
 *
 * Gets @conn rehandshaking mode. See
 * g_dtls_connection_set_rehandshake_mode() for details.
 *
 * Returns: %G_TLS_REHANDSHAKE_SAFELY
 *
 * Since: 2.48
 *
 * Deprecated: 2.64. Changing the rehandshake mode is no longer
 *   required for compatibility. Also, rehandshaking has been removed
 *   from the TLS protocol in TLS 1.3.
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GTlsRehandshakeMode
g_dtls_connection_get_rehandshake_mode (GDtlsConnection *conn)
{
  GTlsRehandshakeMode mode;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), G_TLS_REHANDSHAKE_SAFELY);

  /* Continue to call g_object_get(), even though the return value is
   * ignored, so that behavior doesn’t change for derived classes.
   */
  g_object_get (G_OBJECT (conn),
                "rehandshake-mode", &mode,
                NULL);
  return G_TLS_REHANDSHAKE_SAFELY;
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_dtls_connection_handshake:
 * @conn: a #GDtlsConnection
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Attempts a TLS handshake on @conn.
 *
 * On the client side, it is never necessary to call this method;
 * although the connection needs to perform a handshake after
 * connecting, #GDtlsConnection will handle this for you automatically
 * when you try to send or receive data on the connection. You can call
 * g_dtls_connection_handshake() manually if you want to know whether
 * the initial handshake succeeded or failed (as opposed to just
 * immediately trying to use @conn to read or write, in which case,
 * if it fails, it may not be possible to tell if it failed before
 * or after completing the handshake), but beware that servers may reject
 * client authentication after the handshake has completed, so a
 * successful handshake does not indicate the connection will be usable.
 *
 * Likewise, on the server side, although a handshake is necessary at
 * the beginning of the communication, you do not need to call this
 * function explicitly unless you want clearer error reporting.
 *
 * Previously, calling g_dtls_connection_handshake() after the initial
 * handshake would trigger a rehandshake; however, this usage was
 * deprecated in GLib 2.60 because rehandshaking was removed from the
 * TLS protocol in TLS 1.3. Since GLib 2.64, calling this function after
 * the initial handshake will no longer do anything.
 *
 * #GDtlsConnection::accept_certificate may be emitted during the
 * handshake.
 *
 * Returns: success or failure
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_handshake (GDtlsConnection  *conn,
                             GCancellable     *cancellable,
                             GError          **error)
{
  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);

  return G_DTLS_CONNECTION_GET_INTERFACE (conn)->handshake (conn, cancellable,
                                                            error);
}

/**
 * g_dtls_connection_handshake_async:
 * @conn: a #GDtlsConnection
 * @io_priority: the [I/O priority](iface.AsyncResult.html#io-priority) of the request
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: callback to call when the handshake is complete
 * @user_data: the data to pass to the callback function
 *
 * Asynchronously performs a TLS handshake on @conn. See
 * g_dtls_connection_handshake() for more information.
 *
 * Since: 2.48
 */
void
g_dtls_connection_handshake_async (GDtlsConnection      *conn,
                                   int                   io_priority,
                                   GCancellable         *cancellable,
                                   GAsyncReadyCallback   callback,
                                   gpointer              user_data)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));

  G_DTLS_CONNECTION_GET_INTERFACE (conn)->handshake_async (conn, io_priority,
                                                           cancellable,
                                                           callback, user_data);
}

/**
 * g_dtls_connection_handshake_finish:
 * @conn: a #GDtlsConnection
 * @result: a #GAsyncResult.
 * @error: a #GError pointer, or %NULL
 *
 * Finish an asynchronous TLS handshake operation. See
 * g_dtls_connection_handshake() for more information.
 *
 * Returns: %TRUE on success, %FALSE on failure, in which
 * case @error will be set.
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_handshake_finish (GDtlsConnection  *conn,
                                    GAsyncResult     *result,
                                    GError          **error)
{
  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);

  return G_DTLS_CONNECTION_GET_INTERFACE (conn)->handshake_finish (conn,
                                                                   result,
                                                                   error);
}

/**
 * g_dtls_connection_shutdown:
 * @conn: a #GDtlsConnection
 * @shutdown_read: %TRUE to stop reception of incoming datagrams
 * @shutdown_write: %TRUE to stop sending outgoing datagrams
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Shut down part or all of a DTLS connection.
 *
 * If @shutdown_read is %TRUE then the receiving side of the connection is shut
 * down, and further reading is disallowed. Subsequent calls to
 * g_datagram_based_receive_messages() will return %G_IO_ERROR_CLOSED.
 *
 * If @shutdown_write is %TRUE then the sending side of the connection is shut
 * down, and further writing is disallowed. Subsequent calls to
 * g_datagram_based_send_messages() will return %G_IO_ERROR_CLOSED.
 *
 * It is allowed for both @shutdown_read and @shutdown_write to be TRUE — this
 * is equivalent to calling g_dtls_connection_close().
 *
 * If @cancellable is cancelled, the #GDtlsConnection may be left
 * partially-closed and any pending untransmitted data may be lost. Call
 * g_dtls_connection_shutdown() again to complete closing the #GDtlsConnection.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_shutdown (GDtlsConnection  *conn,
                            gboolean          shutdown_read,
                            gboolean          shutdown_write,
                            GCancellable     *cancellable,
                            GError          **error)
{
  GDtlsConnectionInterface *iface;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable),
                        FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (!shutdown_read && !shutdown_write)
    return TRUE;

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  g_assert (iface->shutdown != NULL);

  return iface->shutdown (conn, shutdown_read, shutdown_write,
                          cancellable, error);
}

/**
 * g_dtls_connection_shutdown_async:
 * @conn: a #GDtlsConnection
 * @shutdown_read: %TRUE to stop reception of incoming datagrams
 * @shutdown_write: %TRUE to stop sending outgoing datagrams
 * @io_priority: the [I/O priority](iface.AsyncResult.html#io-priority) of the request
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: callback to call when the shutdown operation is complete
 * @user_data: the data to pass to the callback function
 *
 * Asynchronously shut down part or all of the DTLS connection. See
 * g_dtls_connection_shutdown() for more information.
 *
 * Since: 2.48
 */
void
g_dtls_connection_shutdown_async (GDtlsConnection      *conn,
                                  gboolean              shutdown_read,
                                  gboolean              shutdown_write,
                                  int                   io_priority,
                                  GCancellable         *cancellable,
                                  GAsyncReadyCallback   callback,
                                  gpointer              user_data)
{
  GDtlsConnectionInterface *iface;

  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  g_assert (iface->shutdown_async != NULL);

  iface->shutdown_async (conn, TRUE, TRUE, io_priority, cancellable,
                         callback, user_data);
}

/**
 * g_dtls_connection_shutdown_finish:
 * @conn: a #GDtlsConnection
 * @result: a #GAsyncResult
 * @error: a #GError pointer, or %NULL
 *
 * Finish an asynchronous TLS shutdown operation. See
 * g_dtls_connection_shutdown() for more information.
 *
 * Returns: %TRUE on success, %FALSE on failure, in which
 * case @error will be set
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_shutdown_finish (GDtlsConnection  *conn,
                                   GAsyncResult     *result,
                                   GError          **error)
{
  GDtlsConnectionInterface *iface;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  g_assert (iface->shutdown_finish != NULL);

  return iface->shutdown_finish (conn, result, error);
}

/**
 * g_dtls_connection_close:
 * @conn: a #GDtlsConnection
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Close the DTLS connection. This is equivalent to calling
 * g_dtls_connection_shutdown() to shut down both sides of the connection.
 *
 * Closing a #GDtlsConnection waits for all buffered but untransmitted data to
 * be sent before it completes. It then sends a `close_notify` DTLS alert to the
 * peer and may wait for a `close_notify` to be received from the peer. It does
 * not close the underlying #GDtlsConnection:base-socket; that must be closed
 * separately.
 *
 * Once @conn is closed, all other operations will return %G_IO_ERROR_CLOSED.
 * Closing a #GDtlsConnection multiple times will not return an error.
 *
 * #GDtlsConnections will be automatically closed when the last reference is
 * dropped, but you might want to call this function to make sure resources are
 * released as early as possible.
 *
 * If @cancellable is cancelled, the #GDtlsConnection may be left
 * partially-closed and any pending untransmitted data may be lost. Call
 * g_dtls_connection_close() again to complete closing the #GDtlsConnection.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_close (GDtlsConnection  *conn,
                         GCancellable     *cancellable,
                         GError          **error)
{
  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable),
                        FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return G_DTLS_CONNECTION_GET_INTERFACE (conn)->shutdown (conn, TRUE, TRUE,
                                                           cancellable, error);
}

/**
 * g_dtls_connection_close_async:
 * @conn: a #GDtlsConnection
 * @io_priority: the [I/O priority](iface.AsyncResult.html#io-priority) of the request
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @callback: callback to call when the close operation is complete
 * @user_data: the data to pass to the callback function
 *
 * Asynchronously close the DTLS connection. See g_dtls_connection_close() for
 * more information.
 *
 * Since: 2.48
 */
void
g_dtls_connection_close_async (GDtlsConnection      *conn,
                               int                   io_priority,
                               GCancellable         *cancellable,
                               GAsyncReadyCallback   callback,
                               gpointer              user_data)
{
  g_return_if_fail (G_IS_DTLS_CONNECTION (conn));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  G_DTLS_CONNECTION_GET_INTERFACE (conn)->shutdown_async (conn, TRUE, TRUE,
                                                          io_priority,
                                                          cancellable,
                                                          callback, user_data);
}

/**
 * g_dtls_connection_close_finish:
 * @conn: a #GDtlsConnection
 * @result: a #GAsyncResult
 * @error: a #GError pointer, or %NULL
 *
 * Finish an asynchronous TLS close operation. See g_dtls_connection_close()
 * for more information.
 *
 * Returns: %TRUE on success, %FALSE on failure, in which
 * case @error will be set
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_close_finish (GDtlsConnection  *conn,
                                GAsyncResult     *result,
                                GError          **error)
{
  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return G_DTLS_CONNECTION_GET_INTERFACE (conn)->shutdown_finish (conn, result,
                                                                  error);
}

/**
 * g_dtls_connection_emit_accept_certificate:
 * @conn: a #GDtlsConnection
 * @peer_cert: the peer's #GTlsCertificate
 * @errors: the problems with @peer_cert
 *
 * Used by #GDtlsConnection implementations to emit the
 * #GDtlsConnection::accept-certificate signal.
 *
 * Returns: %TRUE if one of the signal handlers has returned
 *     %TRUE to accept @peer_cert
 *
 * Since: 2.48
 */
gboolean
g_dtls_connection_emit_accept_certificate (GDtlsConnection      *conn,
                                           GTlsCertificate      *peer_cert,
                                           GTlsCertificateFlags  errors)
{
  gboolean accept = FALSE;

  g_signal_emit (conn, signals[ACCEPT_CERTIFICATE], 0,
                 peer_cert, errors, &accept);
  return accept;
}

/**
 * g_dtls_connection_set_advertised_protocols:
 * @conn: a #GDtlsConnection
 * @protocols: (array zero-terminated=1) (nullable): a %NULL-terminated
 *   array of ALPN protocol names (eg, "http/1.1", "h2"), or %NULL
 *
 * Sets the list of application-layer protocols to advertise that the
 * caller is willing to speak on this connection. The
 * Application-Layer Protocol Negotiation (ALPN) extension will be
 * used to negotiate a compatible protocol with the peer; use
 * g_dtls_connection_get_negotiated_protocol() to find the negotiated
 * protocol after the handshake.  Specifying %NULL for the the value
 * of @protocols will disable ALPN negotiation.
 *
 * See [IANA TLS ALPN Protocol IDs](https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml#alpn-protocol-ids)
 * for a list of registered protocol IDs.
 *
 * Since: 2.60
 */
void
g_dtls_connection_set_advertised_protocols (GDtlsConnection     *conn,
                                            const gchar * const *protocols)
{
  GDtlsConnectionInterface *iface;

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  if (iface->set_advertised_protocols == NULL)
    return;

  iface->set_advertised_protocols (conn, protocols);
}

/**
 * g_dtls_connection_get_negotiated_protocol:
 * @conn: a #GDtlsConnection
 *
 * Gets the name of the application-layer protocol negotiated during
 * the handshake.
 *
 * If the peer did not use the ALPN extension, or did not advertise a
 * protocol that matched one of @conn's protocols, or the TLS backend
 * does not support ALPN, then this will be %NULL. See
 * g_dtls_connection_set_advertised_protocols().
 *
 * Returns: (nullable): the negotiated protocol, or %NULL
 *
 * Since: 2.60
 */
const gchar *
g_dtls_connection_get_negotiated_protocol (GDtlsConnection *conn)
{
  GDtlsConnectionInterface *iface;

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  if (iface->get_negotiated_protocol == NULL)
    return NULL;

  return iface->get_negotiated_protocol (conn);
}

/**
 * g_dtls_connection_get_channel_binding_data:
 * @conn: a #GDtlsConnection
 * @type: #GTlsChannelBindingType type of data to fetch
 * @data: (out caller-allocates) (optional) (transfer none): #GByteArray is
 *        filled with the binding data, or %NULL
 * @error: a #GError pointer, or %NULL
 *
 * Query the TLS backend for TLS channel binding data of @type for @conn.
 *
 * This call retrieves TLS channel binding data as specified in RFC
 * [5056](https://tools.ietf.org/html/rfc5056), RFC
 * [5929](https://tools.ietf.org/html/rfc5929), and related RFCs.  The
 * binding data is returned in @data.  The @data is resized by the callee
 * using #GByteArray buffer management and will be freed when the @data
 * is destroyed by g_byte_array_unref(). If @data is %NULL, it will only
 * check whether TLS backend is able to fetch the data (e.g. whether @type
 * is supported by the TLS backend). It does not guarantee that the data
 * will be available though.  That could happen if TLS connection does not
 * support @type or the binding data is not available yet due to additional
 * negotiation or input required.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 2.66
 */
gboolean
g_dtls_connection_get_channel_binding_data (GDtlsConnection         *conn,
                                            GTlsChannelBindingType   type,
                                            GByteArray              *data,
                                            GError                 **error)
{
  GDtlsConnectionInterface *iface;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  iface = G_DTLS_CONNECTION_GET_INTERFACE (conn);
  if (iface->get_binding_data == NULL)
    {
      g_set_error_literal (error, G_TLS_CHANNEL_BINDING_ERROR,
          G_TLS_CHANNEL_BINDING_ERROR_NOT_IMPLEMENTED,
          _("TLS backend does not implement TLS binding retrieval"));
      return FALSE;
    }

  return iface->get_binding_data (conn, type, data, error);
}

/**
 * g_dtls_connection_get_protocol_version:
 * @conn: a #GDTlsConnection
 *
 * Returns the current DTLS protocol version, which may be
 * %G_TLS_PROTOCOL_VERSION_UNKNOWN if the connection has not handshaked, or
 * has been closed, or if the TLS backend has implemented a protocol version
 * that is not a recognized #GTlsProtocolVersion.
 *
 * Returns: The current DTLS protocol version
 *
 * Since: 2.70
 */
GTlsProtocolVersion
g_dtls_connection_get_protocol_version (GDtlsConnection *conn)
{
  GTlsProtocolVersion protocol_version;
  GEnumClass *enum_class;
  GEnumValue *enum_value;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), G_TLS_PROTOCOL_VERSION_UNKNOWN);

  g_object_get (G_OBJECT (conn),
                "protocol-version", &protocol_version,
                NULL);

  /* Convert unknown values to G_TLS_PROTOCOL_VERSION_UNKNOWN. */
  enum_class = g_type_class_peek_static (G_TYPE_TLS_PROTOCOL_VERSION);
  enum_value = g_enum_get_value (enum_class, protocol_version);
  return enum_value ? protocol_version : G_TLS_PROTOCOL_VERSION_UNKNOWN;
}

/**
 * g_dtls_connection_get_ciphersuite_name:
 * @conn: a #GDTlsConnection
 *
 * Returns the name of the current DTLS ciphersuite, or %NULL if the
 * connection has not handshaked or has been closed. Beware that the TLS
 * backend may use any of multiple different naming conventions, because
 * OpenSSL and GnuTLS have their own ciphersuite naming conventions that
 * are different from each other and different from the standard, IANA-
 * registered ciphersuite names. The ciphersuite name is intended to be
 * displayed to the user for informative purposes only, and parsing it
 * is not recommended.
 *
 * Returns: (nullable): The name of the current DTLS ciphersuite, or %NULL
 *
 * Since: 2.70
 */
gchar *
g_dtls_connection_get_ciphersuite_name (GDtlsConnection *conn)
{
  gchar *ciphersuite_name;

  g_return_val_if_fail (G_IS_DTLS_CONNECTION (conn), NULL);

  g_object_get (G_OBJECT (conn),
                "ciphersuite-name", &ciphersuite_name,
                NULL);

  return g_steal_pointer (&ciphersuite_name);
}
