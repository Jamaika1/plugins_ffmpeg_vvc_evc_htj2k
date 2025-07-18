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

#include <string.h>

#include "gdbusauthmechanismexternal.h"
#include "gcredentials.h"
#include "gdbuserror.h"
#include "gioenumtypes.h"

#include "../glib/glibintl.h"

#ifdef G_OS_WIN32
#include "gwin32sid.h"
#endif

struct _GDBusAuthMechanismExternalPrivate
{
  gboolean is_client;
  gboolean is_server;
  GDBusAuthMechanismState state;
  gboolean empty_data_sent;
};

static gint                     mechanism_get_priority              (void);
static const gchar             *mechanism_get_name                  (void);

static gboolean                 mechanism_is_supported              (GDBusAuthMechanism   *mechanism);
static gchar                   *mechanism_encode_data               (GDBusAuthMechanism   *mechanism,
                                                                     const gchar          *data,
                                                                     gsize                 data_len,
                                                                     gsize                *out_data_len);
static gchar                   *mechanism_decode_data               (GDBusAuthMechanism   *mechanism,
                                                                     const gchar          *data,
                                                                     gsize                 data_len,
                                                                     gsize                *out_data_len);
static GDBusAuthMechanismState  mechanism_server_get_state          (GDBusAuthMechanism   *mechanism);
static void                     mechanism_server_initiate           (GDBusAuthMechanism   *mechanism,
                                                                     const gchar          *initial_response,
                                                                     gsize                 initial_response_len);
static void                     mechanism_server_data_receive       (GDBusAuthMechanism   *mechanism,
                                                                     const gchar          *data,
                                                                     gsize                 data_len);
static gchar                   *mechanism_server_data_send          (GDBusAuthMechanism   *mechanism,
                                                                     gsize                *out_data_len);
static gchar                   *mechanism_server_get_reject_reason  (GDBusAuthMechanism   *mechanism);
static void                     mechanism_server_shutdown           (GDBusAuthMechanism   *mechanism);
static GDBusAuthMechanismState  mechanism_client_get_state          (GDBusAuthMechanism   *mechanism);
static gchar                   *mechanism_client_initiate           (GDBusAuthMechanism   *mechanism,
                                                                     GDBusConnectionFlags  conn_flags,
                                                                     gsize                *out_initial_response_len);
static void                     mechanism_client_data_receive       (GDBusAuthMechanism   *mechanism,
                                                                     const gchar          *data,
                                                                     gsize                 data_len);
static gchar                   *mechanism_client_data_send          (GDBusAuthMechanism   *mechanism,
                                                                     gsize                *out_data_len);
static void                     mechanism_client_shutdown           (GDBusAuthMechanism   *mechanism);

/* ---------------------------------------------------------------------------------------------------- */

G_DEFINE_TYPE_WITH_PRIVATE (GDBusAuthMechanismExternal, _g_dbus_auth_mechanism_external, G_TYPE_DBUS_AUTH_MECHANISM)

/* ---------------------------------------------------------------------------------------------------- */

static void
_g_dbus_auth_mechanism_external_finalize (GObject *object)
{
  //GDBusAuthMechanismExternal *mechanism = G_DBUS_AUTH_MECHANISM_EXTERNAL (object);

  if (G_OBJECT_CLASS (_g_dbus_auth_mechanism_external_parent_class)->finalize != NULL)
    G_OBJECT_CLASS (_g_dbus_auth_mechanism_external_parent_class)->finalize (object);
}

static void
_g_dbus_auth_mechanism_external_class_init (GDBusAuthMechanismExternalClass *klass)
{
  GObjectClass *gobject_class;
  GDBusAuthMechanismClass *mechanism_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = _g_dbus_auth_mechanism_external_finalize;

  mechanism_class = G_DBUS_AUTH_MECHANISM_CLASS (klass);
  mechanism_class->get_name                  = mechanism_get_name;
  mechanism_class->get_priority              = mechanism_get_priority;
  mechanism_class->is_supported              = mechanism_is_supported;
  mechanism_class->encode_data               = mechanism_encode_data;
  mechanism_class->decode_data               = mechanism_decode_data;
  mechanism_class->server_get_state          = mechanism_server_get_state;
  mechanism_class->server_initiate           = mechanism_server_initiate;
  mechanism_class->server_data_receive       = mechanism_server_data_receive;
  mechanism_class->server_data_send          = mechanism_server_data_send;
  mechanism_class->server_get_reject_reason  = mechanism_server_get_reject_reason;
  mechanism_class->server_shutdown           = mechanism_server_shutdown;
  mechanism_class->client_get_state          = mechanism_client_get_state;
  mechanism_class->client_initiate           = mechanism_client_initiate;
  mechanism_class->client_data_receive       = mechanism_client_data_receive;
  mechanism_class->client_data_send          = mechanism_client_data_send;
  mechanism_class->client_shutdown           = mechanism_client_shutdown;
}

static void
_g_dbus_auth_mechanism_external_init (GDBusAuthMechanismExternal *mechanism)
{
  mechanism->priv = _g_dbus_auth_mechanism_external_get_instance_private (mechanism);
}

/* ---------------------------------------------------------------------------------------------------- */

static gboolean
mechanism_is_supported (GDBusAuthMechanism *mechanism)
{
  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), FALSE);

#if defined(G_OS_WIN32)
  /* all that is required is current process SID */
  return TRUE;
#else
  /* This mechanism is only available if credentials has been exchanged */
  if (_g_dbus_auth_mechanism_get_credentials (mechanism) != NULL)
    return TRUE;
  else
    return FALSE;
#endif
}

static gint
mechanism_get_priority (void)
{
  /* We prefer EXTERNAL to most other mechanism (DBUS_COOKIE_SHA1 and ANONYMOUS) */
  return 100;
}

static const gchar *
mechanism_get_name (void)
{
  return "EXTERNAL";
}

static gchar *
mechanism_encode_data (GDBusAuthMechanism   *mechanism,
                       const gchar          *data,
                       gsize                 data_len,
                       gsize                *out_data_len)
{
  return NULL;
}


static gchar *
mechanism_decode_data (GDBusAuthMechanism   *mechanism,
                       const gchar          *data,
                       gsize                 data_len,
                       gsize                *out_data_len)
{
  return NULL;
}

/* ---------------------------------------------------------------------------------------------------- */

static GDBusAuthMechanismState
mechanism_server_get_state (GDBusAuthMechanism   *mechanism)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), G_DBUS_AUTH_MECHANISM_STATE_INVALID);
  g_return_val_if_fail (m->priv->is_server && !m->priv->is_client, G_DBUS_AUTH_MECHANISM_STATE_INVALID);

  return m->priv->state;
}

static gboolean
data_matches_credentials (const gchar  *data,
                          gsize         data_len,
                          GCredentials *credentials)
{
  gboolean match;

  match = FALSE;

  if (credentials == NULL)
    goto out;

#if defined(G_OS_UNIX)
  {
    gint64 alleged_uid;
    gchar *endp;

    /* If we were unable to find out the uid, then nothing
     * can possibly match it.  */
    if (g_credentials_get_unix_user (credentials, NULL) == (uid_t) -1)
      goto out;

    /* An empty authorization identity means we want to be
     * whatever identity the out-of-band credentials say we have
     * (RFC 4422 appendix A.1). This effectively matches any uid. */
    if (data == NULL || data_len == 0)
      {
        match = TRUE;
        goto out;
      }
    /* on UNIX, this is the uid as a string in base 10 */
    alleged_uid = g_ascii_strtoll (data, &endp, 10);
    if (*endp == '\0')
      {
        if (g_credentials_get_unix_user (credentials, NULL) == alleged_uid)
          {
            match = TRUE;
          }
      }
  }
#else
  /* TODO: Dont know how to compare credentials on this OS. Please implement. */
#endif

 out:
  return match;
}

static void
mechanism_server_initiate (GDBusAuthMechanism   *mechanism,
                           const gchar          *initial_response,
                           gsize                 initial_response_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism));
  g_return_if_fail (!m->priv->is_server && !m->priv->is_client);

  m->priv->is_server = TRUE;

  if (initial_response != NULL)
    {
      if (data_matches_credentials (initial_response,
                                    initial_response_len,
                                    _g_dbus_auth_mechanism_get_credentials (mechanism)))
        {
          m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_ACCEPTED;
        }
      else
        {
          m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_REJECTED;
        }
    }
  else
    {
      /* The initial-response optimization was not used, so we need to
       * send an empty challenge to prompt the client to respond. */
      m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_HAVE_DATA_TO_SEND;
    }
}

static void
mechanism_server_data_receive (GDBusAuthMechanism   *mechanism,
                               const gchar          *data,
                               gsize                 data_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism));
  g_return_if_fail (m->priv->is_server && !m->priv->is_client);
  g_return_if_fail (m->priv->state == G_DBUS_AUTH_MECHANISM_STATE_WAITING_FOR_DATA);

  if (data_matches_credentials (data,
                                data_len,
                                _g_dbus_auth_mechanism_get_credentials (mechanism)))
    {
      m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_ACCEPTED;
    }
  else
    {
      m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_REJECTED;
    }
}

static gchar *
mechanism_server_data_send (GDBusAuthMechanism   *mechanism,
                            gsize                *out_data_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), NULL);
  g_return_val_if_fail (m->priv->is_server && !m->priv->is_client, NULL);

  if (out_data_len)
    *out_data_len = 0;

  if (m->priv->empty_data_sent)
    {
      /* We have already sent an empty data response.
         Reject the connection.  */
      m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_REJECTED;
      return NULL;
    }

  m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_WAITING_FOR_DATA;
  m->priv->empty_data_sent = TRUE;

  return g_strdup ("");
}

static gchar *
mechanism_server_get_reject_reason (GDBusAuthMechanism   *mechanism)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), NULL);
  g_return_val_if_fail (m->priv->is_server && !m->priv->is_client, NULL);
  g_return_val_if_fail (m->priv->state == G_DBUS_AUTH_MECHANISM_STATE_REJECTED, NULL);

  /* can never end up here because we are never in the REJECTED state */
  g_assert_not_reached ();

  return NULL;
}

static void
mechanism_server_shutdown (GDBusAuthMechanism   *mechanism)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism));
  g_return_if_fail (m->priv->is_server && !m->priv->is_client);

  m->priv->is_server = FALSE;
}

/* ---------------------------------------------------------------------------------------------------- */

static GDBusAuthMechanismState
mechanism_client_get_state (GDBusAuthMechanism   *mechanism)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), G_DBUS_AUTH_MECHANISM_STATE_INVALID);
  g_return_val_if_fail (m->priv->is_client && !m->priv->is_server, G_DBUS_AUTH_MECHANISM_STATE_INVALID);

  return m->priv->state;
}

static gchar *
mechanism_client_initiate (GDBusAuthMechanism   *mechanism,
                           GDBusConnectionFlags  conn_flags,
                           gsize                *out_initial_response_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);
  gchar *initial_response = NULL;

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), NULL);
  g_return_val_if_fail (!m->priv->is_server && !m->priv->is_client, NULL);

  m->priv->is_client = TRUE;
  m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_WAITING_FOR_DATA;

  *out_initial_response_len = 0;

  if (conn_flags & G_DBUS_CONNECTION_FLAGS_CROSS_NAMESPACE)
    {
      /* If backwards-compatibility with GDBus servers < 2.73.3 is not a
       * concern, we do not send an initial response, because there is
       * no way to express an empty authorization identity this way.
       * Instead, we'll reply to the server's first (empty) challenge
       * with an empty authorization identity in our first response.  */
    }
  else
    {
      /* Send the Unix uid or Windows SID as an initial response.
       * This is the only thing that is interoperable with GDBus < 2.73.3
       * servers (RHEL ≤ 9, Debian ≤ 11, Ubuntu ≤ 22.04, SUSE ≤ 15.5). */
#if defined(G_OS_UNIX)
      GCredentials *credentials;

      credentials = _g_dbus_auth_mechanism_get_credentials (mechanism);
      g_assert (credentials != NULL);

      initial_response = g_strdup_printf ("%" G_GINT64_FORMAT, (gint64) g_credentials_get_unix_user (credentials, NULL));
#elif defined(G_OS_WIN32)
      initial_response = _g_win32_current_process_sid_string (NULL);
#else
      /* GDBus < 2.73.3 servers can't have worked on this platform anyway,
       * so it isn't a regression to behave as though
       * G_DBUS_CONNECTION_FLAGS_CROSS_NAMESPACE had been set. */
      g_debug ("Unknown platform, cannot use initial response in EXTERNAL");
#endif
    }

  if (initial_response)
    {
      m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_ACCEPTED;
      *out_initial_response_len = strlen (initial_response);
    }
  return initial_response;
}

static void
mechanism_client_data_receive (GDBusAuthMechanism   *mechanism,
                               const gchar          *data,
                               gsize                 data_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism));
  g_return_if_fail (m->priv->is_client && !m->priv->is_server);
  g_return_if_fail (m->priv->state == G_DBUS_AUTH_MECHANISM_STATE_WAITING_FOR_DATA);

  /* The server sent us a challenge, which should normally
   * be empty.  We respond with our authorization identity.  */
  m->priv->state = G_DBUS_AUTH_MECHANISM_STATE_HAVE_DATA_TO_SEND;
}

static gchar *
mechanism_client_data_send (GDBusAuthMechanism   *mechanism,
                            gsize                *out_data_len)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_val_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism), NULL);
  g_return_val_if_fail (m->priv->is_client && !m->priv->is_server, NULL);
  g_return_val_if_fail (m->priv->state == G_DBUS_AUTH_MECHANISM_STATE_HAVE_DATA_TO_SEND, NULL);

  /* We respond to the server's challenge by sending our
   * authorization identity, which is the empty string, meaning
   * whoever the out-of-band credentials say we are.  */
  *out_data_len = 0;
  return g_strdup ("");
}

static void
mechanism_client_shutdown (GDBusAuthMechanism   *mechanism)
{
  GDBusAuthMechanismExternal *m = G_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism);

  g_return_if_fail (G_IS_DBUS_AUTH_MECHANISM_EXTERNAL (mechanism));
  g_return_if_fail (m->priv->is_client && !m->priv->is_server);

  m->priv->is_client = FALSE;
}

/* ---------------------------------------------------------------------------------------------------- */
