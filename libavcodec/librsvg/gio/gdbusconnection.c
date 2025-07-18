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

/*
 * TODO for GDBus:
 *
 * - would be nice to expose GDBusAuthMechanism and an extension point
 *
 * - Need to rewrite GDBusAuth and rework GDBusAuthMechanism. In particular
 *   the mechanism VFuncs need to be able to set an error.
 *
 * - Need to document other mechanisms/sources for determining the D-Bus
 *   address of a well-known bus.
 *
 *   - e.g. on Win32 we need code like from here
 *
 *     http://cgit.freedesktop.org/~david/gdbus-standalone/tree/gdbus/gdbusaddress.c#n900
 *
 *     that was never copied over here because it originally was copy-paste
 *     from the GPLv2 / AFL 2.1 libdbus sources.
 *
 *   - on OS X we need to look in launchd for the address
 *
 *     https://bugs.freedesktop.org/show_bug.cgi?id=14259
 *
 *   - on X11 we need to look in a X11 property on the X server
 *     - (we can also just use dbus-launch(1) from the D-Bus
 *        distribution)
 *
 *   - (ideally) this requires D-Bus spec work because none of
 *     this has never really been specced out properly (except
 *     the X11 bits)
 *
 * - Related to the above, we also need to be able to launch a message bus
 *   instance.... Since we don't want to write our own bus daemon we should
 *   launch dbus-daemon(1) (thus: Win32 and OS X need to bundle it)
 *
 * - probably want a G_DBUS_NONCE_TCP_TMPDIR environment variable
 *   to specify where the nonce is stored. This will allow people to use
 *   G_DBUS_NONCE_TCP_TMPDIR=/mnt/secure.company.server/dbus-nonce-dir
 *   to easily achieve secure RPC via nonce-tcp.
 *
 * - need to expose an extension point for resolving D-Bus address and
 *   turning them into GIOStream objects. This will allow us to implement
 *   e.g. X11 D-Bus transports without dlopen()'ing or linking against
 *   libX11 from libgio.
 *   - see g_dbus_address_connect() in gdbusaddress.c
 *
 * - would be cute to use kernel-specific APIs to resolve fds for
 *   debug output when using G_DBUS_DEBUG=message, e.g. in addition to
 *
 *     fd 21: dev=8:1,mode=0100644,ino=1171231,uid=0,gid=0,rdev=0:0,size=234,atime=1273070640,mtime=1267126160,ctime=1267126160
 *
 *   maybe we can show more information about what fd 21 really is.
 *   Ryan suggests looking in /proc/self/fd for clues / symlinks!
 *   Initial experiments on Linux 2.6 suggests that the symlink looks
 *   like this:
 *
 *    3 -> /proc/18068/fd
 *
 *   e.g. not of much use.
 *
 *  - GDBus High-Level docs
 *    - Proxy: properties, signals...
 *    - Connection: IOStream based, ::close, connection setup steps
 *                  mainloop integration, threading
 *    - Differences from libdbus (extend "Migrating from")
 *      - the message handling thread
 *      - Using GVariant instead of GValue
 *    - Explain why the high-level API is a good thing and what
 *      kind of pitfalls it avoids
 *      - Export objects before claiming names
 *    - Talk about auto-starting services (cf. GBusNameWatcherFlags)
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "gdbusauth.h"
#include "gdbusutils.h"
#include "gdbusaddress.h"
#include "gdbusmessage.h"
#include "gdbusconnection.h"
#include "gdbuserror.h"
#include "gioenumtypes.h"
#include "gdbusintrospection.h"
#include "gdbusmethodinvocation.h"
#include "gdbusprivate.h"
#include "gdbusauthobserver.h"
#include "ginitable.h"
#include "gasyncinitable.h"
#include "giostream.h"
#include "gasyncresult.h"
#include "gtask.h"
#include "gmarshal-internal.h"

#ifdef G_OS_UNIX
#include "gunixconnection.h"
#include "gunixfdmessage.h"
#endif

#include "../glib/glibintl.h"

#define G_DBUS_CONNECTION_FLAGS_ALL                           \
  (G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT |            \
   G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER |            \
   G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS |   \
   G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION |           \
   G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING |         \
   G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_REQUIRE_SAME_USER | \
   G_DBUS_CONNECTION_FLAGS_CROSS_NAMESPACE)

/**
 * GDBusConnection:
 *
 * The `GDBusConnection` type is used for D-Bus connections to remote
 * peers such as a message buses.
 *
 * It is a low-level API that offers a lot of flexibility. For instance,
 * it lets you establish a connection over any transport that can by represented
 * as a [class@Gio.IOStream].
 *
 * This class is rarely used directly in D-Bus clients. If you are writing
 * a D-Bus client, it is often easier to use the [func@Gio.bus_own_name],
 * [func@Gio.bus_watch_name] or [func@Gio.DBusProxy.new_for_bus] APIs.
 *
 * As an exception to the usual GLib rule that a particular object must not
 * be used by two threads at the same time, `GDBusConnection`s methods may be
 * called from any thread. This is so that [func@Gio.bus_get] and
 * [func@Gio.bus_get_sync] can safely return the same `GDBusConnection` when
 * called from any thread.
 *
 * Most of the ways to obtain a `GDBusConnection` automatically initialize it
 * (i.e. connect to D-Bus): for instance, [func@Gio.DBusConnection.new] and
 * [func@Gio.bus_get], and the synchronous versions of those methods, give you
 * an initialized connection. Language bindings for GIO should use
 * [func@Gio.Initable.new] or [func@Gio.AsyncInitable.new_async], which also
 * initialize the connection.
 *
 * If you construct an uninitialized `GDBusConnection`, such as via
 * [ctor@GObject.Object.new], you must initialize it via [method@Gio.Initable.init] or
 * [method@Gio.AsyncInitable.init_async] before using its methods or properties.
 * Calling methods or accessing properties on a `GDBusConnection` that has not
 * completed initialization successfully is considered to be invalid, and leads
 * to undefined behaviour. In particular, if initialization fails with a
 * `GError`, the only valid thing you can do with that `GDBusConnection` is to
 * free it with [method@GObject.Object.unref].
 *
 * ## An example D-Bus server
 *
 * Here is an example for a D-Bus server:
 * [gdbus-example-server.c](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/tests/gdbus-example-server.c)
 *
 * ## An example for exporting a subtree
 *
 * Here is an example for exporting a subtree:
 * [gdbus-example-subtree.c](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/tests/gdbus-example-subtree.c)
 *
 * ## An example for file descriptor passing
 *
 * Here is an example for passing UNIX file descriptors:
 * [gdbus-unix-fd-client.c](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/tests/gdbus-example-unix-fd-client.c)
 *
 * ## An example for exporting a GObject
 *
 * Here is an example for exporting a #GObject:
 * [gdbus-example-export.c](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/tests/gdbus-example-export.c)
 *
 * Since: 2.26
 */

/* ---------------------------------------------------------------------------------------------------- */

typedef struct _GDBusConnectionClass GDBusConnectionClass;

/**
 * GDBusConnectionClass:
 * @closed: Signal class handler for the #GDBusConnection::closed signal.
 *
 * Class structure for #GDBusConnection.
 *
 * Since: 2.26
 */
struct _GDBusConnectionClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public >*/
  /* Signals */
  void (*closed) (GDBusConnection *connection,
                  gboolean         remote_peer_vanished,
                  GError          *error);
};

G_LOCK_DEFINE_STATIC (message_bus_lock);

static GWeakRef the_session_bus;
static GWeakRef the_system_bus;

/* Extra pseudo-member of GDBusSendMessageFlags.
 * Set by initable_init() to indicate that despite not being initialized yet,
 * enough of the only-valid-after-init members are set that we can send a
 * message, and we're being called from its thread, so no memory barrier is
 * required before accessing them.
 */
#define SEND_MESSAGE_FLAGS_INITIALIZING (1u << 31)

/* Same as SEND_MESSAGE_FLAGS_INITIALIZING, but in GDBusCallFlags */
#define CALL_FLAGS_INITIALIZING (1u << 31)

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  GDestroyNotify              callback;
  gpointer                    user_data;
} CallDestroyNotifyData;

static gboolean
call_destroy_notify_data_in_idle (gpointer user_data)
{
  CallDestroyNotifyData *data = user_data;
  data->callback (data->user_data);
  return FALSE;
}

static void
call_destroy_notify_data_free (CallDestroyNotifyData *data)
{
  g_free (data);
}

/*
 * call_destroy_notify: <internal>
 * @context: (nullable): A #GMainContext or %NULL.
 * @callback: (nullable): A #GDestroyNotify or %NULL.
 * @user_data: Data to pass to @callback.
 *
 * Schedules @callback to run in @context.
 */
static void
call_destroy_notify (GMainContext  *context,
                     GDestroyNotify callback,
                     gpointer       user_data)
{
  GSource *idle_source;
  CallDestroyNotifyData *data;

  if (callback == NULL)
    return;

  data = g_new0 (CallDestroyNotifyData, 1);
  data->callback = callback;
  data->user_data = user_data;

  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
  g_source_set_callback (idle_source,
                         call_destroy_notify_data_in_idle,
                         data,
                         (GDestroyNotify) call_destroy_notify_data_free);
  g_source_set_static_name (idle_source, "[gio] call_destroy_notify_data_in_idle");
  g_source_attach (idle_source, context);
  g_source_unref (idle_source);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  gatomicrefcount ref_count;
  /* All remaining fields are immutable after construction. */
  GDBusSignalCallback callback;
  gpointer user_data;
  GDestroyNotify user_data_free_func;
  guint id;
  GMainContext *context;
} SignalSubscriber;

static SignalSubscriber *
signal_subscriber_ref (SignalSubscriber *subscriber)
{
  g_atomic_ref_count_inc (&subscriber->ref_count);
  return subscriber;
}

static void
signal_subscriber_unref (SignalSubscriber *subscriber)
{
  if (g_atomic_ref_count_dec (&subscriber->ref_count))
    {
      /* Destroy the user data. It doesn’t matter which thread
       * signal_subscriber_unref() is called in (or whether it’s called with a
       * lock held), as call_destroy_notify() always defers to the next
       * #GMainContext iteration. */
      call_destroy_notify (subscriber->context,
                           subscriber->user_data_free_func,
                           subscriber->user_data);

      g_main_context_unref (subscriber->context);
      g_free (subscriber);
    }
}

typedef struct
{
  /*
   * 1 reference while waiting for GetNameOwner() to finish
   * 1 reference for each SignalData that points to this one as its
   *   shared_name_watcher
   */
  grefcount ref_count;

  gchar *owner;
  guint32 get_name_owner_serial;
} WatchedName;

static WatchedName *
watched_name_new (void)
{
  WatchedName *watched_name = g_new0 (WatchedName, 1);

  g_ref_count_init (&watched_name->ref_count);
  watched_name->owner = NULL;
  return g_steal_pointer (&watched_name);
}

typedef struct SignalData SignalData;

struct SignalData
{
  gchar *rule;
  gchar *sender;
  gchar *interface_name;
  gchar *member;
  gchar *object_path;
  gchar *arg0;
  GDBusSignalFlags flags;
  GPtrArray *subscribers;  /* (owned) (element-type SignalSubscriber) */

  /*
   * If the sender is a well-known name, this is an unowned SignalData
   * representing the NameOwnerChanged signal that tracks its owner.
   * NULL if sender is NULL.
   * NULL if sender is its own owner (a unique name or DBUS_SERVICE_DBUS).
   *
   * Invariants: if not NULL, then
   * shared_name_watcher->sender == DBUS_SERVICE_DBUS
   * shared_name_watcher->interface_name == DBUS_INTERFACE_DBUS
   * shared_name_watcher->member == "NameOwnerChanged"
   * shared_name_watcher->object_path == DBUS_PATH_DBUS
   * shared_name_watcher->arg0 == sender
   * shared_name_watcher->flags == NONE
   * shared_name_watcher->watched_name == NULL
   */
  SignalData *shared_name_watcher;

  /*
   * Non-NULL if this SignalData is another SignalData's shared_name_watcher.
   * One reference for each SignalData that has this one as its
   * shared_name_watcher.
   * Otherwise NULL.
   */
  WatchedName *watched_name;
};

static SignalData *
signal_data_new_take (gchar *rule,
                      gchar *sender,
                      gchar *interface_name,
                      gchar *member,
                      gchar *object_path,
                      gchar *arg0,
                      GDBusSignalFlags flags)
{
  SignalData *signal_data = g_new0 (SignalData, 1);

  signal_data->rule = rule;
  signal_data->sender = sender;
  signal_data->interface_name = interface_name;
  signal_data->member = member;
  signal_data->object_path = object_path;
  signal_data->arg0 = arg0;
  signal_data->flags = flags;
  signal_data->subscribers = g_ptr_array_new_with_free_func ((GDestroyNotify) signal_subscriber_unref);
  return g_steal_pointer (&signal_data);
}

static void
signal_data_free (SignalData *signal_data)
{
  /* The SignalData should not be freed while it still has subscribers */
  g_assert (signal_data->subscribers->len == 0);

  /* The SignalData should not be freed while it is watching for
   * NameOwnerChanged on behalf of another SignalData */
  g_assert (signal_data->watched_name == NULL);

  /* The SignalData should be detached from its name watcher, if any,
   * before it is freed */
  g_assert (signal_data->shared_name_watcher == NULL);

  g_free (signal_data->rule);
  g_free (signal_data->sender);
  g_free (signal_data->interface_name);
  g_free (signal_data->member);
  g_free (signal_data->object_path);
  g_free (signal_data->arg0);
  g_ptr_array_unref (signal_data->subscribers);

  g_free (signal_data);
}

/* ---------------------------------------------------------------------------------------------------- */

#ifdef G_OS_WIN32
#define CONNECTION_ENSURE_LOCK(obj) do { ; } while (FALSE)
#else
// TODO: for some reason this doesn't work on Windows
#define CONNECTION_ENSURE_LOCK(obj) do {                                \
    if (G_UNLIKELY (g_mutex_trylock(&(obj)->lock)))                     \
      {                                                                 \
        g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                             "CONNECTION_ENSURE_LOCK: GDBusConnection object lock is not locked"); \
      }                                                                 \
  } while (FALSE)
#endif

#define CONNECTION_LOCK(obj) do {                                       \
    g_mutex_lock (&(obj)->lock);                                        \
  } while (FALSE)

#define CONNECTION_UNLOCK(obj) do {                                     \
    g_mutex_unlock (&(obj)->lock);                                      \
  } while (FALSE)

/* Flags in connection->atomic_flags */
enum {
    FLAG_INITIALIZED = 1 << 0,
    FLAG_EXIT_ON_CLOSE = 1 << 1,
    FLAG_CLOSED = 1 << 2
};

struct _GDBusConnection
{
  /*< private >*/
  GObject parent_instance;

  /* ------------------------------------------------------------------------ */
  /* -- General object state ------------------------------------------------ */
  /* ------------------------------------------------------------------------ */

  /* General-purpose lock for most fields */
  GMutex lock;

  /* A lock used in the init() method of the GInitable interface - see comments
   * in initable_init() for why a separate lock is needed.
   *
   * If you need both @lock and @init_lock, you must take @init_lock first.
   */
  GMutex init_lock;

  /* Set (by loading the contents of /var/lib/dbus/machine-id) the first time
   * someone calls DBUS_INTERFACE_PEER.GetMachineId(). Protected by @lock.
   */
  gchar *machine_id;

  /* The underlying stream used for communication
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  GIOStream *stream;

  /* The object used for authentication (if any).
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  GDBusAuth *auth;

  /* Last serial used. Protected by @lock. */
  guint32 last_serial;

  /* The object used to send/receive messages.
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  GDBusWorker *worker;

  /* If connected to a message bus, this contains the unique name assigned to
   * us by the bus (e.g. ":1.42").
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  gchar *bus_unique_name;

  /* The GUID returned by the other side if we authenticated as a client or
   * the GUID to use if authenticating as a server.
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  gchar *guid;

  /* FLAG_INITIALIZED is set exactly when initable_init() has finished running.
   * Inspect @initialization_error to see whether it succeeded or failed.
   *
   * FLAG_EXIT_ON_CLOSE is the exit-on-close property.
   *
   * FLAG_CLOSED is the closed property. It may be read at any time, but
   * may only be written while holding @lock.
   */
  gint atomic_flags;  /* (atomic) */

  /* If the connection could not be established during initable_init(),
   * this GError will be set.
   * Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  GError *initialization_error;

  /* The result of g_main_context_ref_thread_default() when the object
   * was created (the GObject _init() function) - this is used for delivery
   * of the :closed GObject signal.
   *
   * Only set in the GObject init function, so no locks are needed.
   */
  GMainContext *main_context_at_construction;

  /* Read-only construct properties, no locks needed */
  gchar *address;
  GDBusConnectionFlags flags;

  /* Map used for managing method replies, protected by @lock */
  GHashTable *map_method_serial_to_task;  /* guint32 -> owned GTask* */
  GHashTable *map_method_serial_to_name_watcher;  /* guint32 -> unowned SignalData* */

  /* Maps used for managing signal subscription, protected by @lock */
  GHashTable *map_rule_to_signal_data;                      /* match rule (gchar*)    -> SignalData */
  GHashTable *map_id_to_signal_data;                        /* id (guint)             -> SignalData */
  GHashTable *map_sender_unique_name_to_signal_data_array;  /* unique sender (gchar*) -> GPtrArray* of SignalData */

  /* Maps used for managing exported objects and subtrees,
   * protected by @lock
   */
  GHashTable *map_object_path_to_eo;  /* gchar* -> ExportedObject* */
  GHashTable *map_id_to_ei;           /* guint  -> ExportedInterface* */
  GHashTable *map_object_path_to_es;  /* gchar* -> ExportedSubtree* */
  GHashTable *map_id_to_es;           /* guint  -> ExportedSubtree* */

  /* Map used for storing last used serials for each thread, protected by @lock */
  GHashTable *map_thread_to_last_serial;

  /* Structure used for message filters, protected by @lock */
  GPtrArray *filters;

  /* Capabilities negotiated during authentication
   * Read-only after initable_init(), so it may be read without holding a
   * lock, if you check for initialization first.
   */
  GDBusCapabilityFlags capabilities;

  /* Protected by @init_lock */
  GDBusAuthObserver *authentication_observer;

  /* Read-only after initable_init(), so it may be read if you either
   * hold @init_lock or check for initialization first.
   */
  GCredentials *credentials;

  /* set to TRUE when finalizing */
  gboolean finalizing;
};

typedef struct ExportedObject ExportedObject;
static void exported_object_free (ExportedObject *eo);

typedef struct ExportedSubtree ExportedSubtree;
static ExportedSubtree *exported_subtree_ref (ExportedSubtree *es);
static void exported_subtree_unref (ExportedSubtree *es);

enum
{
  CLOSED_SIGNAL,
  LAST_SIGNAL,
};

enum
{
  PROP_0,
  PROP_STREAM,
  PROP_ADDRESS,
  PROP_FLAGS,
  PROP_GUID,
  PROP_UNIQUE_NAME,
  PROP_CLOSED,
  PROP_EXIT_ON_CLOSE,
  PROP_CAPABILITY_FLAGS,
  PROP_AUTHENTICATION_OBSERVER,
};

static void distribute_signals (GDBusConnection  *connection,
                                GDBusMessage     *message);

static void distribute_method_call (GDBusConnection  *connection,
                                    GDBusMessage     *message);

static gboolean handle_generic_unlocked (GDBusConnection *connection,
                                         GDBusMessage    *message);


static void purge_all_signal_subscriptions (GDBusConnection *connection);
static void purge_all_filters (GDBusConnection *connection);

static void schedule_method_call (GDBusConnection            *connection,
                                  GDBusMessage               *message,
                                  guint                       registration_id,
                                  guint                       subtree_registration_id,
                                  const GDBusInterfaceInfo   *interface_info,
                                  const GDBusMethodInfo      *method_info,
                                  const GDBusPropertyInfo    *property_info,
                                  GVariant                   *parameters,
                                  const GDBusInterfaceVTable *vtable,
                                  GMainContext               *main_context,
                                  gpointer                    user_data);

#define _G_ENSURE_LOCK(name) do {                                       \
    if (G_UNLIKELY (G_TRYLOCK(name)))                                   \
      {                                                                 \
        g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                             "_G_ENSURE_LOCK: Lock '" #name "' is not locked"); \
      }                                                                 \
  } while (FALSE)                                                       \

static guint signals[LAST_SIGNAL] = { 0 };

static void initable_iface_init       (GInitableIface      *initable_iface);
static void async_initable_iface_init (GAsyncInitableIface *async_initable_iface);

G_DEFINE_TYPE_WITH_CODE (GDBusConnection, g_dbus_connection, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, initable_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, async_initable_iface_init)
                         );

/*
 * Check that all members of @connection that can only be accessed after
 * the connection is initialized can safely be accessed. If not,
 * log a critical warning. This function is a memory barrier.
 *
 * Returns: %TRUE if initialized
 */
static gboolean
check_initialized (GDBusConnection *connection)
{
  /* The access to @atomic_flags isn't conditional, so that this function
   * provides a memory barrier for thread-safety even if checks are disabled.
   * (If you don't want this stricter guarantee, you can call
   * g_return_if_fail (check_initialized (c)).)
   *
   * This isn't strictly necessary now that we've decided use of an
   * uninitialized GDBusConnection is undefined behaviour, but it seems
   * better to be as deterministic as is feasible.
   *
   * (Anything that could suffer a crash from seeing undefined values
   * must have a race condition - thread A initializes the connection while
   * thread B calls a method without initialization, hoping that thread A will
   * win the race - so its behaviour is undefined anyway.)
   */
  gint flags = g_atomic_int_get (&connection->atomic_flags);

  g_return_val_if_fail (flags & FLAG_INITIALIZED, FALSE);

  /* We can safely access this, due to the memory barrier above */
  g_return_val_if_fail (connection->initialization_error == NULL, FALSE);

  return TRUE;
}

typedef enum {
    MAY_BE_UNINITIALIZED = (1<<1)
} CheckUnclosedFlags;

/*
 * Check the same thing as check_initialized(), and also that the
 * connection is not closed. If the connection is uninitialized,
 * raise a critical warning (it's programmer error); if it's closed,
 * raise a recoverable GError (it's a runtime error).
 *
 * This function is a memory barrier.
 *
 * Returns: %TRUE if initialized and not closed
 */
static gboolean
check_unclosed (GDBusConnection     *connection,
                CheckUnclosedFlags   check,
                GError             **error)
{
  /* check_initialized() is effectively inlined, so we don't waste time
   * doing two memory barriers
   */
  gint flags = g_atomic_int_get (&connection->atomic_flags);

  if (!(check & MAY_BE_UNINITIALIZED))
    {
      g_return_val_if_fail (flags & FLAG_INITIALIZED, FALSE);
      g_return_val_if_fail (connection->initialization_error == NULL, FALSE);
    }

  if (flags & FLAG_CLOSED)
    {
      g_set_error_literal (error,
                           G_IO_ERROR,
                           G_IO_ERROR_CLOSED,
                           _("The connection is closed"));
      return FALSE;
    }

  return TRUE;
}

static GHashTable *alive_connections = NULL;

static void
g_dbus_connection_dispose (GObject *object)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (object);

  G_LOCK (message_bus_lock);
  CONNECTION_LOCK (connection);
  if (connection->worker != NULL)
    {
      _g_dbus_worker_stop (connection->worker);
      connection->worker = NULL;
      if (alive_connections != NULL)
        g_warn_if_fail (g_hash_table_remove (alive_connections, connection));
    }
  else
    {
      if (alive_connections != NULL)
        g_warn_if_fail (!g_hash_table_contains (alive_connections, connection));
    }
  CONNECTION_UNLOCK (connection);
  G_UNLOCK (message_bus_lock);

  if (G_OBJECT_CLASS (g_dbus_connection_parent_class)->dispose != NULL)
    G_OBJECT_CLASS (g_dbus_connection_parent_class)->dispose (object);
}

static void
g_dbus_connection_finalize (GObject *object)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (object);

  connection->finalizing = TRUE;

  purge_all_signal_subscriptions (connection);

  purge_all_filters (connection);
  g_ptr_array_unref (connection->filters);

  if (connection->authentication_observer != NULL)
    g_object_unref (connection->authentication_observer);

  if (connection->auth != NULL)
    g_object_unref (connection->auth);

  if (connection->credentials)
    g_object_unref (connection->credentials);

  if (connection->stream != NULL)
    {
      g_object_unref (connection->stream);
      connection->stream = NULL;
    }

  g_free (connection->address);

  g_free (connection->guid);
  g_free (connection->bus_unique_name);

  if (connection->initialization_error != NULL)
    g_error_free (connection->initialization_error);

  g_hash_table_unref (connection->map_method_serial_to_task);
  g_hash_table_unref (connection->map_method_serial_to_name_watcher);

  g_hash_table_unref (connection->map_rule_to_signal_data);
  g_hash_table_unref (connection->map_id_to_signal_data);
  g_hash_table_unref (connection->map_sender_unique_name_to_signal_data_array);

  g_hash_table_unref (connection->map_id_to_ei);
  g_hash_table_unref (connection->map_object_path_to_eo);
  g_hash_table_unref (connection->map_id_to_es);
  g_hash_table_unref (connection->map_object_path_to_es);

  g_hash_table_unref (connection->map_thread_to_last_serial);

  g_main_context_unref (connection->main_context_at_construction);

  g_free (connection->machine_id);

  g_mutex_clear (&connection->init_lock);
  g_mutex_clear (&connection->lock);

  G_OBJECT_CLASS (g_dbus_connection_parent_class)->finalize (object);
}

/* called in any user thread, with the connection's lock not held */
static void
g_dbus_connection_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (object);

  switch (prop_id)
    {
    case PROP_STREAM:
      g_value_set_object (value, g_dbus_connection_get_stream (connection));
      break;

    case PROP_GUID:
      g_value_set_string (value, g_dbus_connection_get_guid (connection));
      break;

    case PROP_UNIQUE_NAME:
      g_value_set_string (value, g_dbus_connection_get_unique_name (connection));
      break;

    case PROP_CLOSED:
      g_value_set_boolean (value, g_dbus_connection_is_closed (connection));
      break;

    case PROP_EXIT_ON_CLOSE:
      g_value_set_boolean (value, g_dbus_connection_get_exit_on_close (connection));
      break;

    case PROP_CAPABILITY_FLAGS:
      g_value_set_flags (value, g_dbus_connection_get_capabilities (connection));
      break;

    case PROP_FLAGS:
      g_value_set_flags (value, g_dbus_connection_get_flags (connection));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/* called in any user thread, with the connection's lock not held */
static void
g_dbus_connection_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (object);

  switch (prop_id)
    {
    case PROP_STREAM:
      connection->stream = g_value_dup_object (value);
      break;

    case PROP_GUID:
      connection->guid = g_value_dup_string (value);
      break;

    case PROP_ADDRESS:
      connection->address = g_value_dup_string (value);
      break;

    case PROP_FLAGS:
      connection->flags = g_value_get_flags (value);
      break;

    case PROP_EXIT_ON_CLOSE:
      g_dbus_connection_set_exit_on_close (connection, g_value_get_boolean (value));
      break;

    case PROP_AUTHENTICATION_OBSERVER:
      connection->authentication_observer = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/* Base-class implementation of GDBusConnection::closed.
 *
 * Called in a user thread, by the main context that was thread-default when
 * the object was constructed.
 */
static void
g_dbus_connection_real_closed (GDBusConnection *connection,
                               gboolean         remote_peer_vanished,
                               GError          *error)
{
  gint flags = g_atomic_int_get (&connection->atomic_flags);

  /* Because atomic int access is a memory barrier, we can safely read
   * initialization_error without a lock, as long as we do it afterwards.
   */
  if (remote_peer_vanished &&
      (flags & FLAG_EXIT_ON_CLOSE) != 0 &&
      (flags & FLAG_INITIALIZED) != 0 &&
      connection->initialization_error == NULL)
    {
      raise (SIGTERM);
    }
}

static void
g_dbus_connection_class_init (GDBusConnectionClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize     = g_dbus_connection_finalize;
  gobject_class->dispose      = g_dbus_connection_dispose;
  gobject_class->set_property = g_dbus_connection_set_property;
  gobject_class->get_property = g_dbus_connection_get_property;

  klass->closed = g_dbus_connection_real_closed;

  /**
   * GDBusConnection:stream:
   *
   * The underlying #GIOStream used for I/O.
   *
   * If this is passed on construction and is a #GSocketConnection,
   * then the corresponding #GSocket will be put into non-blocking mode.
   *
   * While the #GDBusConnection is active, it will interact with this
   * stream from a worker thread, so it is not safe to interact with
   * the stream directly.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_STREAM,
                                   g_param_spec_object ("stream", NULL, NULL,
                                                        G_TYPE_IO_STREAM,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:address:
   *
   * A D-Bus address specifying potential endpoints that can be used
   * when establishing the connection.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ADDRESS,
                                   g_param_spec_string ("address", NULL, NULL,
                                                        NULL,
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:flags:
   *
   * Flags from the #GDBusConnectionFlags enumeration.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_FLAGS,
                                   g_param_spec_flags ("flags", NULL, NULL,
                                                       G_TYPE_DBUS_CONNECTION_FLAGS,
                                                       G_DBUS_CONNECTION_FLAGS_NONE,
                                                       G_PARAM_READABLE |
                                                       G_PARAM_WRITABLE |
                                                       G_PARAM_CONSTRUCT_ONLY |
                                                       G_PARAM_STATIC_NAME |
                                                       G_PARAM_STATIC_BLURB |
                                                       G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:guid:
   *
   * The GUID of the peer performing the role of server when
   * authenticating.
   *
   * If you are constructing a #GDBusConnection and pass
   * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER in the
   * #GDBusConnection:flags property then you **must** also set this
   * property to a valid guid.
   *
   * If you are constructing a #GDBusConnection and pass
   * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT in the
   * #GDBusConnection:flags property you will be able to read the GUID
   * of the other peer here after the connection has been successfully
   * initialized.
   *
   * Note that the
   * [D-Bus specification](https://dbus.freedesktop.org/doc/dbus-specification.html#addresses)
   * uses the term ‘UUID’ to refer to this, whereas GLib consistently uses the
   * term ‘GUID’ for historical reasons.
   *
   * Despite its name, the format of #GDBusConnection:guid does not follow
   * [RFC 4122](https://datatracker.ietf.org/doc/html/rfc4122) or the Microsoft
   * GUID format.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_GUID,
                                   g_param_spec_string ("guid", NULL, NULL,
                                                        NULL,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:unique-name:
   *
   * The unique name as assigned by the message bus or %NULL if the
   * connection is not open or not a message bus connection.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_UNIQUE_NAME,
                                   g_param_spec_string ("unique-name", NULL, NULL,
                                                        NULL,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:closed:
   *
   * A boolean specifying whether the connection has been closed.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_CLOSED,
                                   g_param_spec_boolean ("closed", NULL, NULL,
                                                         FALSE,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_BLURB |
                                                         G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:exit-on-close:
   *
   * A boolean specifying whether the process will be terminated (by
   * calling `raise(SIGTERM)`) if the connection is closed by the
   * remote peer.
   *
   * Note that #GDBusConnection objects returned by g_bus_get_finish()
   * and g_bus_get_sync() will (usually) have this property set to %TRUE.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_EXIT_ON_CLOSE,
                                   g_param_spec_boolean ("exit-on-close", NULL, NULL,
                                                         FALSE,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_WRITABLE |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_BLURB |
                                                         G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:capabilities:
   *
   * Flags from the #GDBusCapabilityFlags enumeration
   * representing connection features negotiated with the other peer.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_CAPABILITY_FLAGS,
                                   g_param_spec_flags ("capabilities", NULL, NULL,
                                                       G_TYPE_DBUS_CAPABILITY_FLAGS,
                                                       G_DBUS_CAPABILITY_FLAGS_NONE,
                                                       G_PARAM_READABLE |
                                                       G_PARAM_STATIC_NAME |
                                                       G_PARAM_STATIC_BLURB |
                                                       G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection:authentication-observer:
   *
   * A #GDBusAuthObserver object to assist in the authentication process or %NULL.
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_AUTHENTICATION_OBSERVER,
                                   g_param_spec_object ("authentication-observer", NULL, NULL,
                                                        G_TYPE_DBUS_AUTH_OBSERVER,
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

  /**
   * GDBusConnection::closed:
   * @connection: the #GDBusConnection emitting the signal
   * @remote_peer_vanished: %TRUE if @connection is closed because the
   *     remote peer closed its end of the connection
   * @error: (nullable): a #GError with more details about the event or %NULL
   *
   * Emitted when the connection is closed.
   *
   * The cause of this event can be
   *
   * - If g_dbus_connection_close() is called. In this case
   *   @remote_peer_vanished is set to %FALSE and @error is %NULL.
   *
   * - If the remote peer closes the connection. In this case
   *   @remote_peer_vanished is set to %TRUE and @error is set.
   *
   * - If the remote peer sends invalid or malformed data. In this
   *   case @remote_peer_vanished is set to %FALSE and @error is set.
   *
   * Upon receiving this signal, you should give up your reference to
   * @connection. You are guaranteed that this signal is emitted only
   * once.
   *
   * Since: 2.26
   */
  signals[CLOSED_SIGNAL] = g_signal_new (I_("closed"),
                                         G_TYPE_DBUS_CONNECTION,
                                         G_SIGNAL_RUN_LAST,
                                         G_STRUCT_OFFSET (GDBusConnectionClass, closed),
                                         NULL,
                                         NULL,
                                         _g_cclosure_marshal_VOID__BOOLEAN_BOXED,
                                         G_TYPE_NONE,
                                         2,
                                         G_TYPE_BOOLEAN,
                                         G_TYPE_ERROR);
  g_signal_set_va_marshaller (signals[CLOSED_SIGNAL],
                              G_TYPE_FROM_CLASS (klass),
                              _g_cclosure_marshal_VOID__BOOLEAN_BOXEDv);
}

static void
g_dbus_connection_init (GDBusConnection *connection)
{
  g_mutex_init (&connection->lock);
  g_mutex_init (&connection->init_lock);

  connection->map_method_serial_to_task = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
  connection->map_method_serial_to_name_watcher = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

  connection->map_rule_to_signal_data = g_hash_table_new (g_str_hash,
                                                          g_str_equal);
  connection->map_id_to_signal_data = g_hash_table_new (g_direct_hash,
                                                        g_direct_equal);
  connection->map_sender_unique_name_to_signal_data_array = g_hash_table_new_full (g_str_hash,
                                                                                   g_str_equal,
                                                                                   g_free,
                                                                                   (GDestroyNotify) g_ptr_array_unref);

  connection->map_object_path_to_eo = g_hash_table_new_full (g_str_hash,
                                                             g_str_equal,
                                                             NULL,
                                                             (GDestroyNotify) exported_object_free);

  connection->map_id_to_ei = g_hash_table_new (g_direct_hash,
                                               g_direct_equal);

  connection->map_object_path_to_es = g_hash_table_new_full (g_str_hash,
                                                             g_str_equal,
                                                             NULL,
                                                             (GDestroyNotify) exported_subtree_unref);

  connection->map_id_to_es = g_hash_table_new (g_direct_hash,
                                               g_direct_equal);

  connection->map_thread_to_last_serial = g_hash_table_new (g_direct_hash,
                                                            g_direct_equal);

  connection->main_context_at_construction = g_main_context_ref_thread_default ();

  connection->filters = g_ptr_array_new ();
}

/**
 * g_dbus_connection_get_stream:
 * @connection: a #GDBusConnection
 *
 * Gets the underlying stream used for IO.
 *
 * While the #GDBusConnection is active, it will interact with this
 * stream from a worker thread, so it is not safe to interact with
 * the stream directly.
 *
 * Returns: (transfer none) (not nullable): the stream used for IO
 *
 * Since: 2.26
 */
GIOStream *
g_dbus_connection_get_stream (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return NULL;

  return connection->stream;
}

/**
 * g_dbus_connection_start_message_processing:
 * @connection: a #GDBusConnection
 *
 * If @connection was created with
 * %G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING, this method
 * starts processing messages. Does nothing on if @connection wasn't
 * created with this flag or if the method has already been called.
 *
 * Since: 2.26
 */
void
g_dbus_connection_start_message_processing (GDBusConnection *connection)
{
  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return;

  g_assert (connection->worker != NULL);
  _g_dbus_worker_unfreeze (connection->worker);
}

/**
 * g_dbus_connection_is_closed:
 * @connection: a #GDBusConnection
 *
 * Gets whether @connection is closed.
 *
 * Returns: %TRUE if the connection is closed, %FALSE otherwise
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_is_closed (GDBusConnection *connection)
{
  gint flags;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);

  flags = g_atomic_int_get (&connection->atomic_flags);

  return (flags & FLAG_CLOSED) ? TRUE : FALSE;
}

/**
 * g_dbus_connection_get_capabilities:
 * @connection: a #GDBusConnection
 *
 * Gets the capabilities negotiated with the remote peer
 *
 * Returns: zero or more flags from the #GDBusCapabilityFlags enumeration
 *
 * Since: 2.26
 */
GDBusCapabilityFlags
g_dbus_connection_get_capabilities (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), G_DBUS_CAPABILITY_FLAGS_NONE);

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return G_DBUS_CAPABILITY_FLAGS_NONE;

  return connection->capabilities;
}

/**
 * g_dbus_connection_get_flags:
 * @connection: a #GDBusConnection
 *
 * Gets the flags used to construct this connection
 *
 * Returns: zero or more flags from the #GDBusConnectionFlags enumeration
 *
 * Since: 2.60
 */
GDBusConnectionFlags
g_dbus_connection_get_flags (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), G_DBUS_CONNECTION_FLAGS_NONE);

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return G_DBUS_CONNECTION_FLAGS_NONE;

  return connection->flags;
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called in a temporary thread without holding locks. */
static void
flush_in_thread_func (GTask         *task,
                      gpointer       source_object,
                      gpointer       task_data,
                      GCancellable  *cancellable)
{
  GError *error = NULL;

  if (g_dbus_connection_flush_sync (source_object,
                                    cancellable,
                                    &error))
    g_task_return_boolean (task, TRUE);
  else
    g_task_return_error (task, error);
}

/**
 * g_dbus_connection_flush:
 * @connection: a #GDBusConnection
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: (nullable): a #GAsyncReadyCallback to call when the
 *     request is satisfied or %NULL if you don't care about the result
 * @user_data: The data to pass to @callback
 *
 * Asynchronously flushes @connection, that is, writes all queued
 * outgoing messages to the transport and then flushes the transport
 * (using g_output_stream_flush_async()). This is useful in programs
 * that want to emit a D-Bus signal and then exit immediately. Without
 * flushing the connection, there is no guarantee that the message has
 * been sent to the networking buffers in the OS kernel.
 *
 * This is an asynchronous method. When the operation is finished,
 * @callback will be invoked in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from. You can
 * then call g_dbus_connection_flush_finish() to get the result of the
 * operation. See g_dbus_connection_flush_sync() for the synchronous
 * version.
 *
 * Since: 2.26
 */
void
g_dbus_connection_flush (GDBusConnection     *connection,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data)
{
  GTask *task;

  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));

  task = g_task_new (connection, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_dbus_connection_flush);
  g_task_run_in_thread (task, flush_in_thread_func);
  g_object_unref (task);
}

/**
 * g_dbus_connection_flush_finish:
 * @connection: a #GDBusConnection
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed
 *     to g_dbus_connection_flush()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_flush().
 *
 * Returns: %TRUE if the operation succeeded, %FALSE if @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_flush_finish (GDBusConnection  *connection,
                                GAsyncResult     *res,
                                GError          **error)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (g_task_is_valid (res, connection), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return g_task_propagate_boolean (G_TASK (res), error);
}

/**
 * g_dbus_connection_flush_sync:
 * @connection: a #GDBusConnection
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously flushes @connection. The calling thread is blocked
 * until this is done. See g_dbus_connection_flush() for the
 * asynchronous version of this method and more details about what it
 * does.
 *
 * Returns: %TRUE if the operation succeeded, %FALSE if @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_flush_sync (GDBusConnection  *connection,
                              GCancellable     *cancellable,
                              GError          **error)
{
  gboolean ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  ret = FALSE;

  /* This is only a best-effort attempt to see whether the connection is
   * closed, so it doesn't need the lock. If the connection closes just
   * after this check, but before scheduling the flush operation, the
   * result will be more or less the same as if the connection closed while
   * the flush operation was pending - it'll fail with either CLOSED or
   * CANCELLED.
   */
  if (!check_unclosed (connection, 0, error))
    goto out;

  g_assert (connection->worker != NULL);

  ret = _g_dbus_worker_flush_sync (connection->worker,
                                   cancellable,
                                   error);

 out:
  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  GDBusConnection *connection;
  GError *error;
  gboolean remote_peer_vanished;
} EmitClosedData;

static void
emit_closed_data_free (EmitClosedData *data)
{
  g_object_unref (data->connection);
  if (data->error != NULL)
    g_error_free (data->error);
  g_free (data);
}

/* Called in a user thread that has acquired the main context that was
 * thread-default when the object was constructed
 */
static gboolean
emit_closed_in_idle (gpointer user_data)
{
  EmitClosedData *data = user_data;
  gboolean result;

  g_object_notify (G_OBJECT (data->connection), "closed");
  g_signal_emit (data->connection,
                 signals[CLOSED_SIGNAL],
                 0,
                 data->remote_peer_vanished,
                 data->error,
                 &result);
  return FALSE;
}

/* Can be called from any thread, must hold lock.
 * FLAG_CLOSED must already have been set.
 */
static void
schedule_closed_unlocked (GDBusConnection *connection,
                          gboolean         remote_peer_vanished,
                          GError          *error)
{
  GSource *idle_source;
  EmitClosedData *data;

  CONNECTION_ENSURE_LOCK (connection);

  data = g_new0 (EmitClosedData, 1);
  data->connection = g_object_ref (connection);
  data->remote_peer_vanished = remote_peer_vanished;
  data->error = error != NULL ? g_error_copy (error) : NULL;

  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
  g_source_set_callback (idle_source,
                         emit_closed_in_idle,
                         data,
                         (GDestroyNotify) emit_closed_data_free);
  g_source_set_static_name (idle_source, "[gio] emit_closed_in_idle");
  g_source_attach (idle_source, connection->main_context_at_construction);
  g_source_unref (idle_source);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_close:
 * @connection: a #GDBusConnection
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: (nullable): a #GAsyncReadyCallback to call when the request is
 *     satisfied or %NULL if you don't care about the result
 * @user_data: The data to pass to @callback
 *
 * Closes @connection. Note that this never causes the process to
 * exit (this might only happen if the other end of a shared message
 * bus connection disconnects, see #GDBusConnection:exit-on-close).
 *
 * Once the connection is closed, operations such as sending a message
 * will return with the error %G_IO_ERROR_CLOSED. Closing a connection
 * will not automatically flush the connection so queued messages may
 * be lost. Use g_dbus_connection_flush() if you need such guarantees.
 *
 * If @connection is already closed, this method fails with
 * %G_IO_ERROR_CLOSED.
 *
 * When @connection has been closed, the #GDBusConnection::closed
 * signal is emitted in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread that @connection was constructed in.
 *
 * This is an asynchronous method. When the operation is finished,
 * @callback will be invoked in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from. You can
 * then call g_dbus_connection_close_finish() to get the result of the
 * operation. See g_dbus_connection_close_sync() for the synchronous
 * version.
 *
 * Since: 2.26
 */
void
g_dbus_connection_close (GDBusConnection     *connection,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data)
{
  GTask *task;

  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return;

  g_assert (connection->worker != NULL);

  task = g_task_new (connection, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_dbus_connection_close);
  _g_dbus_worker_close (connection->worker, task);
  g_object_unref (task);
}

/**
 * g_dbus_connection_close_finish:
 * @connection: a #GDBusConnection
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed
 *     to g_dbus_connection_close()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_close().
 *
 * Returns: %TRUE if the operation succeeded, %FALSE if @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_close_finish (GDBusConnection  *connection,
                                GAsyncResult     *res,
                                GError          **error)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (g_task_is_valid (res, connection), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return g_task_propagate_boolean (G_TASK (res), error);
}

typedef struct {
    GMainLoop *loop;
    GAsyncResult *result;
} SyncCloseData;

/* Can be called by any thread, without the connection lock */
static void
sync_close_cb (GObject *source_object,
               GAsyncResult *res,
               gpointer user_data)
{
  SyncCloseData *data = user_data;

  data->result = g_object_ref (res);
  g_main_loop_quit (data->loop);
}

/**
 * g_dbus_connection_close_sync:
 * @connection: a #GDBusConnection
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously closes @connection. The calling thread is blocked
 * until this is done. See g_dbus_connection_close() for the
 * asynchronous version of this method and more details about what it
 * does.
 *
 * Returns: %TRUE if the operation succeeded, %FALSE if @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_close_sync (GDBusConnection  *connection,
                              GCancellable     *cancellable,
                              GError          **error)
{
  gboolean ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  ret = FALSE;

  if (check_unclosed (connection, 0, error))
    {
      GMainContext *context;
      SyncCloseData data;

      context = g_main_context_new ();
      g_main_context_push_thread_default (context);
      data.loop = g_main_loop_new (context, TRUE);
      data.result = NULL;

      g_dbus_connection_close (connection, cancellable, sync_close_cb, &data);
      g_main_loop_run (data.loop);
      ret = g_dbus_connection_close_finish (connection, data.result, error);

      g_object_unref (data.result);
      g_main_loop_unref (data.loop);
      g_main_context_pop_thread_default (context);
      g_main_context_unref (context);
    }

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_get_last_serial:
 * @connection: a #GDBusConnection
 *
 * Retrieves the last serial number assigned to a #GDBusMessage on
 * the current thread. This includes messages sent via both low-level
 * API such as g_dbus_connection_send_message() as well as
 * high-level API such as g_dbus_connection_emit_signal(),
 * g_dbus_connection_call() or g_dbus_proxy_call().
 *
 * Returns: the last used serial or zero when no message has been sent
 *     within the current thread
 *
 * Since: 2.34
 */
guint32
g_dbus_connection_get_last_serial (GDBusConnection *connection)
{
  guint32 ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), 0);

  CONNECTION_LOCK (connection);
  ret = GPOINTER_TO_UINT (g_hash_table_lookup (connection->map_thread_to_last_serial,
                                               g_thread_self ()));
  CONNECTION_UNLOCK (connection);

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

/* Can be called by any thread, with the connection lock held */
static gboolean
g_dbus_connection_send_message_unlocked (GDBusConnection   *connection,
                                         GDBusMessage      *message,
                                         GDBusSendMessageFlags flags,
                                         guint32           *out_serial,
                                         GError           **error)
{
  guchar *blob;
  gsize blob_size;
  guint32 serial_to_use;

  CONNECTION_ENSURE_LOCK (connection);

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (G_IS_DBUS_MESSAGE (message), FALSE);

  /* TODO: check all necessary headers are present */

  if (out_serial != NULL)
    *out_serial = 0;

  /* If we're in initable_init(), don't check for being initialized, to avoid
   * chicken-and-egg problems. initable_init() is responsible for setting up
   * our prerequisites (mainly connection->worker), and only calling us
   * from its own thread (so no memory barrier is needed).
   */
  if (!check_unclosed (connection,
                       (flags & SEND_MESSAGE_FLAGS_INITIALIZING) ? MAY_BE_UNINITIALIZED : 0,
                       error))
    return FALSE;

  blob = g_dbus_message_to_blob (message,
                                 &blob_size,
                                 connection->capabilities,
                                 error);
  if (blob == NULL)
    return FALSE;

  if (flags & G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL)
    {
      serial_to_use = g_dbus_message_get_serial (message);
    }
  else
    {
      /* The serial_to_use must not be zero, as per
       * https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-messages. */
      if (connection->last_serial == G_MAXUINT32)
        connection->last_serial = 1;
      else
        connection->last_serial++;

      serial_to_use = connection->last_serial;
    }

  g_assert (serial_to_use != 0);

  switch (blob[0])
    {
    case 'l':
      ((guint32 *) blob)[2] = GUINT32_TO_LE (serial_to_use);
      break;
    case 'B':
      ((guint32 *) blob)[2] = GUINT32_TO_BE (serial_to_use);
      break;
    default:
      g_assert_not_reached ();
      break;
    }

#if 0
  g_printerr ("Writing message of %" G_GSIZE_FORMAT " bytes (serial %d) on %p:\n",
              blob_size, serial_to_use, connection);
  g_printerr ("----\n");
  hexdump (blob, blob_size);
  g_printerr ("----\n");
#endif

  /* TODO: use connection->auth to encode the blob */

  if (out_serial != NULL)
    *out_serial = serial_to_use;

  /* store used serial for the current thread */
  /* TODO: watch the thread disposal and remove associated record
   *       from hashtable
   *  - see https://bugzilla.gnome.org/show_bug.cgi?id=676825#c7
   */
  g_hash_table_replace (connection->map_thread_to_last_serial,
                        g_thread_self (),
                        GUINT_TO_POINTER (serial_to_use));

  if (!(flags & G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL))
    g_dbus_message_set_serial (message, serial_to_use);

  g_dbus_message_lock (message);

  _g_dbus_worker_send_message (connection->worker,
                               message,
                               (gchar*) blob, /* transfer ownership */
                               blob_size);

  return TRUE;
}

/**
 * g_dbus_connection_send_message:
 * @connection: a #GDBusConnection
 * @message: a #GDBusMessage
 * @flags: flags affecting how the message is sent
 * @out_serial: (out) (optional): return location for serial number assigned
 *     to @message when sending it or %NULL
 * @error: Return location for error or %NULL
 *
 * Asynchronously sends @message to the peer represented by @connection.
 *
 * Unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag, the serial number
 * will be assigned by @connection and set on @message via
 * g_dbus_message_set_serial(). If @out_serial is not %NULL, then the
 * serial number used will be written to this location prior to
 * submitting the message to the underlying transport. While it has a `volatile`
 * qualifier, this is a historical artifact and the argument passed to it should
 * not be `volatile`.
 *
 * If @connection is closed then the operation will fail with
 * %G_IO_ERROR_CLOSED. If @message is not well-formed,
 * the operation fails with %G_IO_ERROR_INVALID_ARGUMENT.
 *
 * See this [server][class@Gio.DBusConnection#an-example-d-bus-server]
 * and [client][class@Gio.DBusConnection#an-example-for-file-descriptor-passing]
 * for an example of how to use this low-level API to send and receive
 * UNIX file descriptors.
 *
 * Note that @message must be unlocked, unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag.
 *
 * Returns: %TRUE if the message was well-formed and queued for
 *     transmission, %FALSE if @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_send_message (GDBusConnection        *connection,
                                GDBusMessage           *message,
                                GDBusSendMessageFlags   flags,
                                volatile guint32       *out_serial,
                                GError                **error)
{
  gboolean ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (G_IS_DBUS_MESSAGE (message), FALSE);
  g_return_val_if_fail ((flags & G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL) || !g_dbus_message_get_locked (message), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  CONNECTION_LOCK (connection);
  ret = g_dbus_connection_send_message_unlocked (connection, message, flags, (guint32 *) out_serial, error);
  CONNECTION_UNLOCK (connection);
  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  guint32 serial;

  gulong cancellable_handler_id;
  GSource *cancelled_idle_source;  /* (owned) (nullable) */

  GSource *timeout_source;  /* (owned) (nullable) */

  gboolean delivered;
} SendMessageData;

/* Can be called from any thread with or without lock held */
static void
send_message_data_free (SendMessageData *data)
{
  /* These should already have been cleared by send_message_with_reply_cleanup(). */
  g_assert (data->timeout_source == NULL);
  g_assert (data->cancellable_handler_id == 0);

  g_slice_free (SendMessageData, data);
}

/* ---------------------------------------------------------------------------------------------------- */

/* can be called from any thread with lock held; @task is (transfer none) */
static void
send_message_with_reply_cleanup (GTask *task, gboolean remove)
{
  GDBusConnection *connection = g_task_get_source_object (task);
  SendMessageData *data = g_task_get_task_data (task);

  CONNECTION_ENSURE_LOCK (connection);

  g_assert (!data->delivered);

  data->delivered = TRUE;

  if (data->timeout_source != NULL)
    {
      g_source_destroy (data->timeout_source);
      g_clear_pointer (&data->timeout_source, g_source_unref);
    }
  if (data->cancellable_handler_id > 0)
    {
      g_cancellable_disconnect (g_task_get_cancellable (task), data->cancellable_handler_id);
      data->cancellable_handler_id = 0;
    }
  if (data->cancelled_idle_source != NULL)
    {
      g_source_destroy (data->cancelled_idle_source);
      g_clear_pointer (&data->cancelled_idle_source, g_source_unref);
    }

  if (remove)
    {
      gboolean removed = g_hash_table_remove (connection->map_method_serial_to_task,
                                              GUINT_TO_POINTER (data->serial));
      g_warn_if_fail (removed);
    }
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called from GDBus worker thread with lock held; @task is (transfer none). */
static void
send_message_data_deliver_reply_unlocked (GTask           *task,
                                          GDBusMessage    *reply)
{
  SendMessageData *data = g_task_get_task_data (task);

  if (data->delivered)
    goto out;

  g_task_return_pointer (task, g_object_ref (reply), g_object_unref);

  send_message_with_reply_cleanup (task, TRUE);

 out:
  ;
}

/* Called from a user thread, lock is not held; @task is (transfer none) */
static void
send_message_data_deliver_error (GTask      *task,
                                 GQuark      domain,
                                 gint        code,
                                 const char *message)
{
  GDBusConnection *connection = g_task_get_source_object (task);
  SendMessageData *data = g_task_get_task_data (task);

  CONNECTION_LOCK (connection);
  if (data->delivered)
    {
      CONNECTION_UNLOCK (connection);
      return;
    }

  /* Hold a ref on @task as send_message_with_reply_cleanup() will remove it
   * from the task map and could end up dropping the last reference */
  g_object_ref (task);

  send_message_with_reply_cleanup (task, TRUE);
  CONNECTION_UNLOCK (connection);

  g_task_return_new_error_literal (task, domain, code, message);
  g_object_unref (task);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called from a user thread, lock is not held; @task is (transfer none) */
static gboolean
send_message_with_reply_cancelled_idle_cb (gpointer user_data)
{
  GTask *task = user_data;

  send_message_data_deliver_error (task, G_IO_ERROR, G_IO_ERROR_CANCELLED,
                                   _("Operation was cancelled"));
  return G_SOURCE_REMOVE;
}

/* Can be called from any thread with or without lock held */
static void
send_message_with_reply_cancelled_cb (GCancellable *cancellable,
                                      gpointer      user_data)
{
  GTask *task = user_data;
  SendMessageData *data = g_task_get_task_data (task);

  /* postpone cancellation to idle handler since we may be called directly
   * via g_cancellable_connect() (e.g. holding lock)
   */
  if (data->cancelled_idle_source != NULL)
    return;

  data->cancelled_idle_source = g_idle_source_new ();
  g_source_set_static_name (data->cancelled_idle_source, "[gio] send_message_with_reply_cancelled_idle_cb");
  g_task_attach_source (task, data->cancelled_idle_source, send_message_with_reply_cancelled_idle_cb);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called from a user thread, lock is not held; @task is (transfer none) */
static gboolean
send_message_with_reply_timeout_cb (gpointer user_data)
{
  GTask *task = user_data;

  send_message_data_deliver_error (task, G_IO_ERROR, G_IO_ERROR_TIMED_OUT,
                                   _("Timeout was reached"));
  return G_SOURCE_REMOVE;
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called from a user thread, connection's lock is held */
static void
g_dbus_connection_send_message_with_reply_unlocked (GDBusConnection     *connection,
                                                    GDBusMessage        *message,
                                                    GDBusSendMessageFlags flags,
                                                    gint                 timeout_msec,
                                                    guint32             *out_serial,
                                                    GCancellable        *cancellable,
                                                    GAsyncReadyCallback  callback,
                                                    gpointer             user_data)
{
  GTask *task;
  SendMessageData *data;
  GError *error = NULL;
  guint32 serial;

  if (out_serial == NULL)
    out_serial = &serial;
  *out_serial = 0;

  if (timeout_msec == -1)
    timeout_msec = 25 * 1000;

  data = g_slice_new0 (SendMessageData);
  task = g_task_new (connection, cancellable, callback, user_data);
  g_task_set_source_tag (task,
                         g_dbus_connection_send_message_with_reply_unlocked);
  g_task_set_task_data (task, data, (GDestroyNotify) send_message_data_free);

  if (g_task_return_error_if_cancelled (task))
    {
      g_object_unref (task);
      return;
    }

  if (!g_dbus_connection_send_message_unlocked (connection, message, flags, out_serial, &error))
    {
      g_task_return_error (task, error);
      g_object_unref (task);
      return;
    }
  data->serial = *out_serial;

  if (cancellable != NULL)
    {
      data->cancellable_handler_id = g_cancellable_connect (cancellable,
                                                            G_CALLBACK (send_message_with_reply_cancelled_cb),
                                                            g_object_ref (task),
                                                            g_object_unref);
    }

  if (timeout_msec != G_MAXINT)
    {
      data->timeout_source = g_timeout_source_new (timeout_msec);
      g_source_set_static_name (data->timeout_source, "[gio] send_message_with_reply_unlocked");
      g_task_attach_source (task, data->timeout_source,
                            (GSourceFunc) send_message_with_reply_timeout_cb);
    }

  g_hash_table_insert (connection->map_method_serial_to_task,
                       GUINT_TO_POINTER (*out_serial),
                       g_steal_pointer (&task));
}

/**
 * g_dbus_connection_send_message_with_reply:
 * @connection: a #GDBusConnection
 * @message: a #GDBusMessage
 * @flags: flags affecting how the message is sent
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @out_serial: (out) (optional): return location for serial number assigned
 *     to @message when sending it or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: (nullable): a #GAsyncReadyCallback to call when the request
 *     is satisfied or %NULL if you don't care about the result
 * @user_data: The data to pass to @callback
 *
 * Asynchronously sends @message to the peer represented by @connection.
 *
 * Unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag, the serial number
 * will be assigned by @connection and set on @message via
 * g_dbus_message_set_serial(). If @out_serial is not %NULL, then the
 * serial number used will be written to this location prior to
 * submitting the message to the underlying transport. While it has a `volatile`
 * qualifier, this is a historical artifact and the argument passed to it should
 * not be `volatile`.
 *
 * If @connection is closed then the operation will fail with
 * %G_IO_ERROR_CLOSED. If @cancellable is canceled, the operation will
 * fail with %G_IO_ERROR_CANCELLED. If @message is not well-formed,
 * the operation fails with %G_IO_ERROR_INVALID_ARGUMENT.
 *
 * This is an asynchronous method. When the operation is finished, @callback
 * will be invoked in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from. You can then call
 * g_dbus_connection_send_message_with_reply_finish() to get the result of the operation.
 * See g_dbus_connection_send_message_with_reply_sync() for the synchronous version.
 *
 * Note that @message must be unlocked, unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag.
 *
 * See this [server][class@Gio.DBusConnection#an-example-d-bus-server]
 * and [client][class@Gio.DBusConnection#an-example-for-file-descriptor-passing]
 * for an example of how to use this low-level API to send and receive
 * UNIX file descriptors.
 *
 * Since: 2.26
 */
void
g_dbus_connection_send_message_with_reply (GDBusConnection       *connection,
                                           GDBusMessage          *message,
                                           GDBusSendMessageFlags  flags,
                                           gint                   timeout_msec,
                                           volatile guint32      *out_serial,
                                           GCancellable          *cancellable,
                                           GAsyncReadyCallback    callback,
                                           gpointer               user_data)
{
  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));
  g_return_if_fail (G_IS_DBUS_MESSAGE (message));
  g_return_if_fail ((flags & G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL) || !g_dbus_message_get_locked (message));
  g_return_if_fail (timeout_msec >= 0 || timeout_msec == -1);

  CONNECTION_LOCK (connection);
  g_dbus_connection_send_message_with_reply_unlocked (connection,
                                                      message,
                                                      flags,
                                                      timeout_msec,
                                                      (guint32 *) out_serial,
                                                      cancellable,
                                                      callback,
                                                      user_data);
  CONNECTION_UNLOCK (connection);
}

/**
 * g_dbus_connection_send_message_with_reply_finish:
 * @connection: a #GDBusConnection
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed to
 *     g_dbus_connection_send_message_with_reply()
 * @error: teturn location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_send_message_with_reply().
 *
 * Note that @error is only set if a local in-process error
 * occurred. That is to say that the returned #GDBusMessage object may
 * be of type %G_DBUS_MESSAGE_TYPE_ERROR. Use
 * g_dbus_message_to_gerror() to transcode this to a #GError.
 *
 * See this [server][class@Gio.DBusConnection#an-example-d-bus-server]
 * and [client][class@Gio.DBusConnection#an-example-for-file-descriptor-passing]
 * for an example of how to use this low-level API to send and receive
 * UNIX file descriptors.
 *
 * Returns: (transfer full): a locked #GDBusMessage or %NULL if @error is set
 *
 * Since: 2.26
 */
GDBusMessage *
g_dbus_connection_send_message_with_reply_finish (GDBusConnection  *connection,
                                                  GAsyncResult     *res,
                                                  GError          **error)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  g_return_val_if_fail (g_task_is_valid (res, connection), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  return g_task_propagate_pointer (G_TASK (res), error);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  GAsyncResult *res;
  GMainContext *context;
  GMainLoop *loop;
} SendMessageSyncData;

/* Called from a user thread, lock is not held */
static void
send_message_with_reply_sync_cb (GDBusConnection *connection,
                                 GAsyncResult    *res,
                                 gpointer         user_data)
{
  SendMessageSyncData *data = user_data;
  data->res = g_object_ref (res);
  g_main_loop_quit (data->loop);
}

/**
 * g_dbus_connection_send_message_with_reply_sync:
 * @connection: a #GDBusConnection
 * @message: a #GDBusMessage
 * @flags: flags affecting how the message is sent.
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @out_serial: (out) (optional): return location for serial number
 *     assigned to @message when sending it or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously sends @message to the peer represented by @connection
 * and blocks the calling thread until a reply is received or the
 * timeout is reached. See g_dbus_connection_send_message_with_reply()
 * for the asynchronous version of this method.
 *
 * Unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag, the serial number
 * will be assigned by @connection and set on @message via
 * g_dbus_message_set_serial(). If @out_serial is not %NULL, then the
 * serial number used will be written to this location prior to
 * submitting the message to the underlying transport. While it has a `volatile`
 * qualifier, this is a historical artifact and the argument passed to it should
 * not be `volatile`.
 *
 * If @connection is closed then the operation will fail with
 * %G_IO_ERROR_CLOSED. If @cancellable is canceled, the operation will
 * fail with %G_IO_ERROR_CANCELLED. If @message is not well-formed,
 * the operation fails with %G_IO_ERROR_INVALID_ARGUMENT.
 *
 * Note that @error is only set if a local in-process error
 * occurred. That is to say that the returned #GDBusMessage object may
 * be of type %G_DBUS_MESSAGE_TYPE_ERROR. Use
 * g_dbus_message_to_gerror() to transcode this to a #GError.
 *
 * See this [server][class@Gio.DBusConnection#an-example-d-bus-server]
 * and [client][class@Gio.DBusConnection#an-example-for-file-descriptor-passing]
 * for an example of how to use this low-level API to send and receive
 * UNIX file descriptors.
 *
 * Note that @message must be unlocked, unless @flags contain the
 * %G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL flag.
 *
 * Returns: (transfer full): a locked #GDBusMessage that is the reply
 *     to @message or %NULL if @error is set
 *
 * Since: 2.26
 */
GDBusMessage *
g_dbus_connection_send_message_with_reply_sync (GDBusConnection        *connection,
                                                GDBusMessage           *message,
                                                GDBusSendMessageFlags   flags,
                                                gint                    timeout_msec,
                                                volatile guint32       *out_serial,
                                                GCancellable           *cancellable,
                                                GError                **error)
{
  SendMessageSyncData data;
  GDBusMessage *reply;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  g_return_val_if_fail (G_IS_DBUS_MESSAGE (message), NULL);
  g_return_val_if_fail ((flags & G_DBUS_SEND_MESSAGE_FLAGS_PRESERVE_SERIAL) || !g_dbus_message_get_locked (message), NULL);
  g_return_val_if_fail (timeout_msec >= 0 || timeout_msec == -1, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  data.res = NULL;
  data.context = g_main_context_new ();
  data.loop = g_main_loop_new (data.context, FALSE);

  g_main_context_push_thread_default (data.context);

  g_dbus_connection_send_message_with_reply (connection,
                                             message,
                                             flags,
                                             timeout_msec,
                                             out_serial,
                                             cancellable,
                                             (GAsyncReadyCallback) send_message_with_reply_sync_cb,
                                             &data);
  g_main_loop_run (data.loop);
  reply = g_dbus_connection_send_message_with_reply_finish (connection,
                                                            data.res,
                                                            error);

  g_main_context_pop_thread_default (data.context);

  g_main_context_unref (data.context);
  g_main_loop_unref (data.loop);
  if (data.res)
    g_object_unref (data.res);

  return reply;
}

/* ---------------------------------------------------------------------------------------------------- */

/*
 * Called in any thread.
 * Must hold the connection lock when calling this, unless
 * connection->finalizing is TRUE.
 */
static void
name_watcher_unref_watched_name (GDBusConnection *connection,
                                 SignalData *name_watcher)
{
  WatchedName *watched_name = name_watcher->watched_name;

  g_assert (watched_name != NULL);

  if (!g_ref_count_dec (&watched_name->ref_count))
    return;

  /* Removing watched_name from the name_watcher may result in
   * name_watcher being freed, so we must make sure name_watcher is no
   * longer in map_method_serial_to_name_watcher.
   *
   * If we stop watching the name while our GetNameOwner call was still
   * in-flight, then when the reply eventually arrives, we will not find
   * its serial number in the map and harmlessly ignore it as a result. */
  if (watched_name->get_name_owner_serial != 0)
    g_hash_table_remove (connection->map_method_serial_to_name_watcher,
                         GUINT_TO_POINTER (watched_name->get_name_owner_serial));

  name_watcher->watched_name = NULL;
  g_free (watched_name->owner);
  g_free (watched_name);
}

/* called in GDBusWorker thread with lock held */
static void
name_watcher_set_name_owner_unlocked (SignalData *name_watcher,
                                      const char *new_owner)
{
  if (new_owner != NULL && new_owner[0] == '\0')
    new_owner = NULL;

  g_assert (name_watcher->watched_name != NULL);
  g_set_str (&name_watcher->watched_name->owner, new_owner);
}

/* called in GDBusWorker thread with lock held */
static void
name_watcher_deliver_name_owner_changed_unlocked (SignalData *name_watcher,
                                                  GDBusMessage *message)
{
  GVariant *body;

  body = g_dbus_message_get_body (message);

  if (G_LIKELY (body != NULL && g_variant_is_of_type (body, G_VARIANT_TYPE ("(sss)"))))
    {
      const char *name;
      const char *new_owner;

      g_variant_get (body, "(&s&s&s)", &name, NULL, &new_owner);

      /* Our caller already checked this */
      g_assert (g_strcmp0 (name_watcher->arg0, name) == 0);

      /* FIXME: This should be validating that `new_owner` is a unique name,
       * but IBus’ implementation of a message bus is not compliant with the spec.
       * See https://gitlab.gnome.org/GNOME/glib/-/issues/3353 */
      if (G_LIKELY (new_owner[0] == '\0' || g_dbus_is_name (new_owner)))
        name_watcher_set_name_owner_unlocked (name_watcher, new_owner);
      else
        g_warning ("Received NameOwnerChanged signal with invalid owner \"%s\" for \"%s\"",
                   new_owner, name);
    }
  else
    {
      g_warning ("Received NameOwnerChanged signal with unexpected "
                 "signature %s",
                 body == NULL ? "()" : g_variant_get_type_string (body));

    }
}

/* called in GDBusWorker thread with lock held */
static void
name_watcher_deliver_get_name_owner_reply_unlocked (SignalData *name_watcher,
                                                    GDBusConnection *connection,
                                                    GDBusMessage *message)
{
  GDBusMessageType type;
  GVariant *body;
  WatchedName *watched_name;

  watched_name = name_watcher->watched_name;
  g_assert (watched_name != NULL);
  g_assert (watched_name->get_name_owner_serial != 0);

  type = g_dbus_message_get_message_type (message);
  body = g_dbus_message_get_body (message);

  if (type == G_DBUS_MESSAGE_TYPE_ERROR)
    {
      if (g_strcmp0 (g_dbus_message_get_error_name (message),
                     DBUS_ERROR_NAME_HAS_NO_OWNER))
        name_watcher_set_name_owner_unlocked (name_watcher, NULL);
      /* else it's something like NoReply or AccessDenied, which tells
       * us nothing - leave the owner set to whatever we most recently
       * learned from NameOwnerChanged, or NULL */
    }
  else if (type != G_DBUS_MESSAGE_TYPE_METHOD_RETURN)
    {
      g_warning ("Received GetNameOwner reply with unexpected type %d",
                 type);
    }
  else if (G_LIKELY (body != NULL && g_variant_is_of_type (body, G_VARIANT_TYPE ("(s)"))))
    {
      const char *new_owner;

      g_variant_get (body, "(&s)", &new_owner);

      /* FIXME: This should be validating that `new_owner` is a unique name,
       * but IBus’ implementation of a message bus is not compliant with the spec.
       * See https://gitlab.gnome.org/GNOME/glib/-/issues/3353 */
      if (G_LIKELY (g_dbus_is_name (new_owner)))
        name_watcher_set_name_owner_unlocked (name_watcher, new_owner);
      else
        g_warning ("Received GetNameOwner reply with invalid owner \"%s\" for \"%s\"",
                   new_owner, name_watcher->arg0);
    }
  else
    {
      g_warning ("Received GetNameOwner reply with unexpected signature %s",
                 body == NULL ? "()" : g_variant_get_type_string (body));
    }

  g_hash_table_remove (connection->map_method_serial_to_name_watcher,
                       GUINT_TO_POINTER (watched_name->get_name_owner_serial));
  watched_name->get_name_owner_serial = 0;
}

/* Called in a user thread, lock is held */
static void
name_watcher_call_get_name_owner_unlocked (GDBusConnection *connection,
                                           SignalData *name_watcher)
{
  GDBusMessage *message;
  GError *local_error = NULL;
  WatchedName *watched_name;

  g_assert (g_strcmp0 (name_watcher->sender, DBUS_SERVICE_DBUS) == 0);
  g_assert (g_strcmp0 (name_watcher->interface_name, DBUS_INTERFACE_DBUS) == 0);
  g_assert (g_strcmp0 (name_watcher->member, "NameOwnerChanged") == 0);
  g_assert (g_strcmp0 (name_watcher->object_path, DBUS_PATH_DBUS) == 0);
  /* arg0 of the NameOwnerChanged message is the well-known name whose owner
   * we are interested in */
  g_assert (g_dbus_is_name (name_watcher->arg0));
  g_assert (name_watcher->flags == G_DBUS_SIGNAL_FLAGS_NONE);

  watched_name = name_watcher->watched_name;
  g_assert (watched_name != NULL);
  g_assert (watched_name->owner == NULL);
  g_assert (watched_name->get_name_owner_serial == 0);
  g_assert (name_watcher->shared_name_watcher == NULL);

  message = g_dbus_message_new_method_call (DBUS_SERVICE_DBUS,
                                            DBUS_PATH_DBUS,
                                            DBUS_INTERFACE_DBUS,
                                            "GetNameOwner");
  g_dbus_message_set_body (message, g_variant_new ("(s)", name_watcher->arg0));

  if (g_dbus_connection_send_message_unlocked (connection, message,
                                               G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                               &watched_name->get_name_owner_serial,
                                               &local_error))
    {
      g_assert (watched_name->get_name_owner_serial != 0);
      g_hash_table_insert (connection->map_method_serial_to_name_watcher,
                           GUINT_TO_POINTER (watched_name->get_name_owner_serial),
                           name_watcher);
    }
  else
    {
      g_critical ("Error while sending GetNameOwner() message: %s",
                  local_error->message);
      g_clear_error (&local_error);
      g_assert (watched_name->get_name_owner_serial == 0);
    }

  g_object_unref (message);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  guint                       id;
  guint                       ref_count;
  GDBusMessageFilterFunction  filter_function;
  gpointer                    user_data;
  GDestroyNotify              user_data_free_func;
  GMainContext               *context;
} FilterData;

static void
filter_data_destroy (FilterData *filter, gboolean notify_sync)
{
  if (notify_sync)
    {
      if (filter->user_data_free_func != NULL)
        filter->user_data_free_func (filter->user_data);
    }
  else
    {
      call_destroy_notify (filter->context,
                           filter->user_data_free_func,
                           filter->user_data);
    }
  g_main_context_unref (filter->context);
  g_free (filter);
}

/* requires CONNECTION_LOCK */
static FilterData **
copy_filter_list (GPtrArray *filters)
{
  FilterData **copy;
  guint n;

  copy = g_new (FilterData *, filters->len + 1);
  for (n = 0; n < filters->len; n++)
    {
      copy[n] = filters->pdata[n];
      copy[n]->ref_count++;
    }
  copy[n] = NULL;

  return copy;
}

/* requires CONNECTION_LOCK */
static void
free_filter_list (FilterData **filters)
{
  guint n;

  for (n = 0; filters[n]; n++)
    {
      filters[n]->ref_count--;
      if (filters[n]->ref_count == 0)
        filter_data_destroy (filters[n], FALSE);
    }
  g_free (filters);
}

/* Called in GDBusWorker's thread - we must not block - with no lock held */
static void
on_worker_message_received (GDBusWorker  *worker,
                            GDBusMessage *message,
                            gpointer      user_data)
{
  GDBusConnection *connection;
  FilterData **filters;
  guint n;
  gboolean alive;

  G_LOCK (message_bus_lock);
  alive = g_hash_table_contains (alive_connections, user_data);
  if (!alive)
    {
      G_UNLOCK (message_bus_lock);
      return;
    }
  connection = G_DBUS_CONNECTION (user_data);
  g_object_ref (connection);
  G_UNLOCK (message_bus_lock);

  //g_debug ("in on_worker_message_received");

  g_object_ref (message);
  g_dbus_message_lock (message);

  //g_debug ("boo ref_count = %d %p %p", G_OBJECT (connection)->ref_count, connection, connection->worker);

  /* First collect the set of callback functions */
  CONNECTION_LOCK (connection);
  filters = copy_filter_list (connection->filters);
  CONNECTION_UNLOCK (connection);

  /* then call the filters in order (without holding the lock) */
  for (n = 0; filters[n]; n++)
    {
      message = filters[n]->filter_function (connection,
                                             message,
                                             TRUE,
                                             filters[n]->user_data);
      if (message == NULL)
        break;
      g_dbus_message_lock (message);
    }

  CONNECTION_LOCK (connection);
  free_filter_list (filters);
  CONNECTION_UNLOCK (connection);

  /* Standard dispatch unless the filter ate the message - no need to
   * do anything if the message was altered
   */
  if (message != NULL)
    {
      GDBusMessageType message_type;

      message_type = g_dbus_message_get_message_type (message);
      if (message_type == G_DBUS_MESSAGE_TYPE_METHOD_RETURN || message_type == G_DBUS_MESSAGE_TYPE_ERROR)
        {
          guint32 reply_serial;
          GTask *task;
          SignalData *name_watcher;

          reply_serial = g_dbus_message_get_reply_serial (message);
          CONNECTION_LOCK (connection);
          task = g_hash_table_lookup (connection->map_method_serial_to_task,
                                      GUINT_TO_POINTER (reply_serial));
          if (task != NULL)
            {
              /* This removes @task from @map_method_serial_to_task. */
              //g_debug ("delivering reply/error for serial %d for %p", reply_serial, connection);
              send_message_data_deliver_reply_unlocked (task, message);
            }
          else
            {
              //g_debug ("message reply/error for serial %d but no SendMessageData found for %p", reply_serial, connection);
            }

          name_watcher = g_hash_table_lookup (connection->map_method_serial_to_name_watcher,
                                              GUINT_TO_POINTER (reply_serial));

          if (name_watcher != NULL)
            {
              g_assert (name_watcher->watched_name != NULL);
              g_assert (name_watcher->watched_name->get_name_owner_serial == reply_serial);
              name_watcher_deliver_get_name_owner_reply_unlocked (name_watcher,
                                                                  connection,
                                                                  message);
            }

          CONNECTION_UNLOCK (connection);
        }
      else if (message_type == G_DBUS_MESSAGE_TYPE_SIGNAL)
        {
          CONNECTION_LOCK (connection);
          distribute_signals (connection, message);
          CONNECTION_UNLOCK (connection);
        }
      else if (message_type == G_DBUS_MESSAGE_TYPE_METHOD_CALL)
        {
          CONNECTION_LOCK (connection);
          distribute_method_call (connection, message);
          CONNECTION_UNLOCK (connection);
        }
    }

  if (message != NULL)
    g_object_unref (message);
  g_object_unref (connection);
}

/* Called in GDBusWorker's thread, lock is not held */
static GDBusMessage *
on_worker_message_about_to_be_sent (GDBusWorker  *worker,
                                    GDBusMessage *message,
                                    gpointer      user_data)
{
  GDBusConnection *connection;
  FilterData **filters;
  guint n;
  gboolean alive;

  G_LOCK (message_bus_lock);
  alive = g_hash_table_contains (alive_connections, user_data);
  if (!alive)
    {
      G_UNLOCK (message_bus_lock);
      return message;
    }
  connection = G_DBUS_CONNECTION (user_data);
  g_object_ref (connection);
  G_UNLOCK (message_bus_lock);

  //g_debug ("in on_worker_message_about_to_be_sent");

  /* First collect the set of callback functions */
  CONNECTION_LOCK (connection);
  filters = copy_filter_list (connection->filters);
  CONNECTION_UNLOCK (connection);

  /* then call the filters in order (without holding the lock) */
  for (n = 0; filters[n]; n++)
    {
      g_dbus_message_lock (message);
      message = filters[n]->filter_function (connection,
                                             message,
                                             FALSE,
                                             filters[n]->user_data);
      if (message == NULL)
        break;
    }

  CONNECTION_LOCK (connection);
  free_filter_list (filters);
  CONNECTION_UNLOCK (connection);

  g_object_unref (connection);

  return message;
}

/* called with connection lock held, in GDBusWorker thread
 * @key, @value and @user_data are (transfer none) */
static gboolean
cancel_method_on_close (gpointer key, gpointer value, gpointer user_data)
{
  GTask *task = value;
  SendMessageData *data = g_task_get_task_data (task);

  if (data->delivered)
    return FALSE;

  g_task_return_new_error_literal (task,
                                   G_IO_ERROR,
                                   G_IO_ERROR_CLOSED,
                                   _("The connection is closed"));

  /* Ask send_message_with_reply_cleanup not to remove the element from the
   * hash table - we're in the middle of a foreach; that would be unsafe.
   * Instead, return TRUE from this function so that it gets removed safely.
   */
  send_message_with_reply_cleanup (task, FALSE);
  return TRUE;
}

/* Called in GDBusWorker's thread - we must not block - without lock held */
static void
on_worker_closed (GDBusWorker *worker,
                  gboolean     remote_peer_vanished,
                  GError      *error,
                  gpointer     user_data)
{
  GDBusConnection *connection;
  gboolean alive;
  guint old_atomic_flags;

  G_LOCK (message_bus_lock);
  alive = g_hash_table_contains (alive_connections, user_data);
  if (!alive)
    {
      G_UNLOCK (message_bus_lock);
      return;
    }
  connection = G_DBUS_CONNECTION (user_data);
  g_object_ref (connection);
  G_UNLOCK (message_bus_lock);

  //g_debug ("in on_worker_closed: %s", error->message);

  CONNECTION_LOCK (connection);
  /* Even though this is atomic, we do it inside the lock to avoid breaking
   * assumptions in remove_match_rule(). We'd need the lock in a moment
   * anyway, so, no loss.
   */
  old_atomic_flags = g_atomic_int_or (&connection->atomic_flags, FLAG_CLOSED);

  if (!(old_atomic_flags & FLAG_CLOSED))
    {
      g_hash_table_foreach_remove (connection->map_method_serial_to_task, cancel_method_on_close, NULL);
      schedule_closed_unlocked (connection, remote_peer_vanished, error);
    }
  CONNECTION_UNLOCK (connection);

  g_object_unref (connection);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Determines the biggest set of capabilities we can support on this
 * connection.
 *
 * Called with the init_lock held.
 */
static GDBusCapabilityFlags
get_offered_capabilities_max (GDBusConnection *connection)
{
      GDBusCapabilityFlags ret;
      ret = G_DBUS_CAPABILITY_FLAGS_NONE;
#ifdef G_OS_UNIX
      if (G_IS_UNIX_CONNECTION (connection->stream))
        ret |= G_DBUS_CAPABILITY_FLAGS_UNIX_FD_PASSING;
#endif
      return ret;
}

/* Called in a user thread, lock is not held */
static gboolean
initable_init (GInitable     *initable,
               GCancellable  *cancellable,
               GError       **error)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (initable);
  gboolean ret;

  /* This method needs to be idempotent to work with the singleton
   * pattern. See the docs for g_initable_init(). We implement this by
   * locking.
   *
   * Unfortunately we can't use the main lock since the on_worker_*()
   * callbacks above needs the lock during initialization (for message
   * bus connections we do a synchronous Hello() call on the bus).
   */
  g_mutex_lock (&connection->init_lock);

  ret = FALSE;

  /* Make this a no-op if we're already initialized (successfully or
   * unsuccessfully)
   */
  if ((g_atomic_int_get (&connection->atomic_flags) & FLAG_INITIALIZED))
    {
      ret = (connection->initialization_error == NULL);
      goto out;
    }

  /* Because of init_lock, we can't get here twice in different threads */
  g_assert (connection->initialization_error == NULL);

  /* The user can pass multiple (but mutally exclusive) construct
   * properties:
   *
   *  - stream (of type GIOStream)
   *  - address (of type gchar*)
   *
   * At the end of the day we end up with a non-NULL GIOStream
   * object in connection->stream.
   */
  if (connection->address != NULL)
    {
      g_assert (connection->stream == NULL);

      if ((connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER) ||
          (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS) ||
          (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_REQUIRE_SAME_USER))
        {
          g_set_error_literal (&connection->initialization_error,
                               G_IO_ERROR,
                               G_IO_ERROR_INVALID_ARGUMENT,
                               _("Unsupported flags encountered when constructing a client-side connection"));
          goto out;
        }

      connection->stream = g_dbus_address_get_stream_sync (connection->address,
                                                           NULL, /* TODO: out_guid */
                                                           cancellable,
                                                           &connection->initialization_error);
      if (connection->stream == NULL)
        goto out;
    }
  else if (connection->stream != NULL)
    {
      /* nothing to do */
    }
  else
    {
      g_assert_not_reached ();
    }

  /* Authenticate the connection */
  if (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER)
    {
      g_assert (!(connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT));
      g_assert (connection->guid != NULL);
      connection->auth = _g_dbus_auth_new (connection->stream);
      if (!_g_dbus_auth_run_server (connection->auth,
                                    connection->authentication_observer,
                                    connection->guid,
                                    (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS),
                                    (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_REQUIRE_SAME_USER),
                                    get_offered_capabilities_max (connection),
                                    &connection->capabilities,
                                    &connection->credentials,
                                    cancellable,
                                    &connection->initialization_error))
        goto out;
    }
  else if (connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT)
    {
      g_assert (!(connection->flags & G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER));
      g_assert (connection->guid == NULL);
      connection->auth = _g_dbus_auth_new (connection->stream);
      connection->guid = _g_dbus_auth_run_client (connection->auth,
                                                  connection->authentication_observer,
                                                  connection->flags,
                                                  get_offered_capabilities_max (connection),
                                                  &connection->capabilities,
                                                  cancellable,
                                                  &connection->initialization_error);
      if (connection->guid == NULL)
        goto out;
    }

  if (connection->authentication_observer != NULL)
    {
      g_object_unref (connection->authentication_observer);
      connection->authentication_observer = NULL;
    }

  //g_output_stream_flush (G_SOCKET_CONNECTION (connection->stream)

  //g_debug ("haz unix fd passing powers: %d", connection->capabilities & G_DBUS_CAPABILITY_FLAGS_UNIX_FD_PASSING);

#ifdef G_OS_UNIX
  /* We want all IO operations to be non-blocking since they happen in
   * the worker thread which is shared by _all_ connections.
   */
  if (G_IS_SOCKET_CONNECTION (connection->stream))
    {
      g_socket_set_blocking (g_socket_connection_get_socket (G_SOCKET_CONNECTION (connection->stream)), FALSE);
    }
#endif

  G_LOCK (message_bus_lock);
  if (alive_connections == NULL)
    alive_connections = g_hash_table_new (g_direct_hash, g_direct_equal);
  g_hash_table_add (alive_connections, connection);
  G_UNLOCK (message_bus_lock);

  connection->worker = _g_dbus_worker_new (connection->stream,
                                           connection->capabilities,
                                           ((connection->flags & G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING) != 0),
                                           on_worker_message_received,
                                           on_worker_message_about_to_be_sent,
                                           on_worker_closed,
                                           connection);

  /* if a bus connection, call org.freedesktop.DBus.Hello - this is how we're getting a name */
  if (connection->flags & G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION)
    {
      GVariant *hello_result;

      /* we could lift this restriction by adding code in gdbusprivate.c */
      if (connection->flags & G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING)
        {
          g_set_error_literal (&connection->initialization_error,
                               G_IO_ERROR,
                               G_IO_ERROR_FAILED,
                               "Cannot use DELAY_MESSAGE_PROCESSING with MESSAGE_BUS_CONNECTION");
          goto out;
        }

      hello_result = g_dbus_connection_call_sync (connection,
                                                  DBUS_SERVICE_DBUS,
                                                  DBUS_PATH_DBUS,
                                                  DBUS_INTERFACE_DBUS,
                                                  "Hello",
                                                  NULL, /* parameters */
                                                  G_VARIANT_TYPE ("(s)"),
                                                  CALL_FLAGS_INITIALIZING,
                                                  -1,
                                                  NULL, /* TODO: cancellable */
                                                  &connection->initialization_error);
      if (hello_result == NULL)
        goto out;

      g_variant_get (hello_result, "(s)", &connection->bus_unique_name);
      g_variant_unref (hello_result);
      //g_debug ("unique name is '%s'", connection->bus_unique_name);
    }

  ret = TRUE;
 out:
  if (!ret)
    {
      g_assert (connection->initialization_error != NULL);
      g_propagate_error (error, g_error_copy (connection->initialization_error));
    }

  /* Don't cache canceled errors. Otherwise other concurrent users of the same connection
   * object will be canceled as well. */
  if (g_error_matches (connection->initialization_error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
      if (connection->worker != NULL)
        {
          _g_dbus_worker_stop (connection->worker);
          connection->worker = NULL;
          if (alive_connections != NULL)
            g_warn_if_fail (g_hash_table_remove (alive_connections, connection));
        }
      g_clear_error (&connection->initialization_error);
      g_clear_object (&connection->stream);
      g_clear_object (&connection->auth);
      g_clear_object (&connection->credentials);
      g_clear_pointer (&connection->guid, g_free);
      connection->capabilities = 0;
    }
  else
    g_atomic_int_or (&connection->atomic_flags, FLAG_INITIALIZED);
  g_mutex_unlock (&connection->init_lock);

  return ret;
}

static void
initable_iface_init (GInitableIface *initable_iface)
{
  initable_iface->init = initable_init;
}

/* ---------------------------------------------------------------------------------------------------- */

static void
async_initable_iface_init (GAsyncInitableIface *async_initable_iface)
{
  /* Use default */
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_new:
 * @stream: a #GIOStream
 * @guid: (nullable): the GUID to use if authenticating as a server or %NULL
 * @flags: flags describing how to make the connection
 * @observer: (nullable): a #GDBusAuthObserver or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: a #GAsyncReadyCallback to call when the request is satisfied
 * @user_data: the data to pass to @callback
 *
 * Asynchronously sets up a D-Bus connection for exchanging D-Bus messages
 * with the end represented by @stream.
 *
 * If @stream is a #GSocketConnection, then the corresponding #GSocket
 * will be put into non-blocking mode.
 *
 * The D-Bus connection will interact with @stream from a worker thread.
 * As a result, the caller should not interact with @stream after this
 * method has been called, except by calling g_object_unref() on it.
 *
 * If @observer is not %NULL it may be used to control the
 * authentication process.
 *
 * When the operation is finished, @callback will be invoked. You can
 * then call g_dbus_connection_new_finish() to get the result of the
 * operation.
 *
 * This is an asynchronous failable constructor. See
 * g_dbus_connection_new_sync() for the synchronous
 * version.
 *
 * Since: 2.26
 */
void
g_dbus_connection_new (GIOStream            *stream,
                       const gchar          *guid,
                       GDBusConnectionFlags  flags,
                       GDBusAuthObserver    *observer,
                       GCancellable         *cancellable,
                       GAsyncReadyCallback   callback,
                       gpointer              user_data)
{
  _g_dbus_initialize ();

  g_return_if_fail (G_IS_IO_STREAM (stream));
  g_return_if_fail ((flags & ~G_DBUS_CONNECTION_FLAGS_ALL) == 0);

  g_async_initable_new_async (G_TYPE_DBUS_CONNECTION,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "stream", stream,
                              "guid", guid,
                              "flags", flags,
                              "authentication-observer", observer,
                              NULL);
}

/**
 * g_dbus_connection_new_finish:
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback
 *     passed to g_dbus_connection_new().
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_new().
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set. Free
 *     with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_dbus_connection_new_finish (GAsyncResult  *res,
                              GError       **error)
{
  GObject *object;
  GObject *source_object;

  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  source_object = g_async_result_get_source_object (res);
  g_assert (source_object != NULL);
  object = g_async_initable_new_finish (G_ASYNC_INITABLE (source_object),
                                        res,
                                        error);
  g_object_unref (source_object);
  if (object != NULL)
    return G_DBUS_CONNECTION (object);
  else
    return NULL;
}

/**
 * g_dbus_connection_new_sync:
 * @stream: a #GIOStream
 * @guid: (nullable): the GUID to use if authenticating as a server or %NULL
 * @flags: flags describing how to make the connection
 * @observer: (nullable): a #GDBusAuthObserver or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously sets up a D-Bus connection for exchanging D-Bus messages
 * with the end represented by @stream.
 *
 * If @stream is a #GSocketConnection, then the corresponding #GSocket
 * will be put into non-blocking mode.
 *
 * The D-Bus connection will interact with @stream from a worker thread.
 * As a result, the caller should not interact with @stream after this
 * method has been called, except by calling g_object_unref() on it.
 *
 * If @observer is not %NULL it may be used to control the
 * authentication process.
 *
 * This is a synchronous failable constructor. See
 * g_dbus_connection_new() for the asynchronous version.
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set.
 *     Free with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_dbus_connection_new_sync (GIOStream             *stream,
                            const gchar           *guid,
                            GDBusConnectionFlags   flags,
                            GDBusAuthObserver     *observer,
                            GCancellable          *cancellable,
                            GError               **error)
{
  _g_dbus_initialize ();
  g_return_val_if_fail (G_IS_IO_STREAM (stream), NULL);
  g_return_val_if_fail ((flags & ~G_DBUS_CONNECTION_FLAGS_ALL) == 0, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);
  return g_initable_new (G_TYPE_DBUS_CONNECTION,
                         cancellable,
                         error,
                         "stream", stream,
                         "guid", guid,
                         "flags", flags,
                         "authentication-observer", observer,
                         NULL);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_new_for_address:
 * @address: a D-Bus address
 * @flags: flags describing how to make the connection
 * @observer: (nullable): a #GDBusAuthObserver or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: a #GAsyncReadyCallback to call when the request is satisfied
 * @user_data: the data to pass to @callback
 *
 * Asynchronously connects and sets up a D-Bus client connection for
 * exchanging D-Bus messages with an endpoint specified by @address
 * which must be in the
 * [D-Bus address format](https://dbus.freedesktop.org/doc/dbus-specification.html#addresses).
 *
 * This constructor can only be used to initiate client-side
 * connections - use g_dbus_connection_new() if you need to act as the
 * server. In particular, @flags cannot contain the
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER,
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS or
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_REQUIRE_SAME_USER flags.
 *
 * When the operation is finished, @callback will be invoked. You can
 * then call g_dbus_connection_new_for_address_finish() to get the result of
 * the operation.
 *
 * If @observer is not %NULL it may be used to control the
 * authentication process.
 *
 * This is an asynchronous failable constructor. See
 * g_dbus_connection_new_for_address_sync() for the synchronous
 * version.
 *
 * Since: 2.26
 */
void
g_dbus_connection_new_for_address (const gchar          *address,
                                   GDBusConnectionFlags  flags,
                                   GDBusAuthObserver    *observer,
                                   GCancellable         *cancellable,
                                   GAsyncReadyCallback   callback,
                                   gpointer              user_data)
{
  _g_dbus_initialize ();

  g_return_if_fail (address != NULL);
  g_return_if_fail ((flags & ~G_DBUS_CONNECTION_FLAGS_ALL) == 0);

  g_async_initable_new_async (G_TYPE_DBUS_CONNECTION,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "address", address,
                              "flags", flags,
                              "authentication-observer", observer,
                              NULL);
}

/**
 * g_dbus_connection_new_for_address_finish:
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed
 *     to g_dbus_connection_new()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_new_for_address().
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set.
 *     Free with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_dbus_connection_new_for_address_finish (GAsyncResult  *res,
                                          GError       **error)
{
  GObject *object;
  GObject *source_object;

  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  source_object = g_async_result_get_source_object (res);
  g_assert (source_object != NULL);
  object = g_async_initable_new_finish (G_ASYNC_INITABLE (source_object),
                                        res,
                                        error);
  g_object_unref (source_object);
  if (object != NULL)
    return G_DBUS_CONNECTION (object);
  else
    return NULL;
}

/**
 * g_dbus_connection_new_for_address_sync:
 * @address: a D-Bus address
 * @flags: flags describing how to make the connection
 * @observer: (nullable): a #GDBusAuthObserver or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously connects and sets up a D-Bus client connection for
 * exchanging D-Bus messages with an endpoint specified by @address
 * which must be in the
 * [D-Bus address format](https://dbus.freedesktop.org/doc/dbus-specification.html#addresses).
 *
 * This constructor can only be used to initiate client-side
 * connections - use g_dbus_connection_new_sync() if you need to act
 * as the server. In particular, @flags cannot contain the
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER,
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS or
 * %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_REQUIRE_SAME_USER flags.
 *
 * This is a synchronous failable constructor. See
 * g_dbus_connection_new_for_address() for the asynchronous version.
 *
 * If @observer is not %NULL it may be used to control the
 * authentication process.
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set.
 *     Free with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_dbus_connection_new_for_address_sync (const gchar           *address,
                                        GDBusConnectionFlags   flags,
                                        GDBusAuthObserver     *observer,
                                        GCancellable          *cancellable,
                                        GError               **error)
{
  _g_dbus_initialize ();

  g_return_val_if_fail (address != NULL, NULL);
  g_return_val_if_fail ((flags & ~G_DBUS_CONNECTION_FLAGS_ALL) == 0, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);
  return g_initable_new (G_TYPE_DBUS_CONNECTION,
                         cancellable,
                         error,
                         "address", address,
                         "flags", flags,
                         "authentication-observer", observer,
                         NULL);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_set_exit_on_close:
 * @connection: a #GDBusConnection
 * @exit_on_close: whether the process should be terminated
 *     when @connection is closed by the remote peer
 *
 * Sets whether the process should be terminated when @connection is
 * closed by the remote peer. See #GDBusConnection:exit-on-close for
 * more details.
 *
 * Note that this function should be used with care. Most modern UNIX
 * desktops tie the notion of a user session with the session bus, and expect
 * all of a user's applications to quit when their bus connection goes away.
 * If you are setting @exit_on_close to %FALSE for the shared session
 * bus connection, you should make sure that your application exits
 * when the user session ends.
 *
 * Since: 2.26
 */
void
g_dbus_connection_set_exit_on_close (GDBusConnection *connection,
                                     gboolean         exit_on_close)
{
  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));

  if (exit_on_close)
    g_atomic_int_or (&connection->atomic_flags, FLAG_EXIT_ON_CLOSE);
  else
    g_atomic_int_and (&connection->atomic_flags, ~FLAG_EXIT_ON_CLOSE);

}

/**
 * g_dbus_connection_get_exit_on_close:
 * @connection: a #GDBusConnection
 *
 * Gets whether the process is terminated when @connection is
 * closed by the remote peer. See
 * #GDBusConnection:exit-on-close for more details.
 *
 * Returns: whether the process is terminated when @connection is
 *     closed by the remote peer
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_get_exit_on_close (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);

  if (g_atomic_int_get (&connection->atomic_flags) & FLAG_EXIT_ON_CLOSE)
    return TRUE;
  else
    return FALSE;
}

/**
 * g_dbus_connection_get_guid:
 * @connection: a #GDBusConnection
 *
 * The GUID of the peer performing the role of server when
 * authenticating. See #GDBusConnection:guid for more details.
 *
 * Returns: (not nullable): The GUID. Do not free this string, it is owned by
 *     @connection.
 *
 * Since: 2.26
 */
const gchar *
g_dbus_connection_get_guid (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  return connection->guid;
}

/**
 * g_dbus_connection_get_unique_name:
 * @connection: a #GDBusConnection
 *
 * Gets the unique name of @connection as assigned by the message
 * bus. This can also be used to figure out if @connection is a
 * message bus connection.
 *
 * Returns: (nullable): the unique name or %NULL if @connection is not a message
 *     bus connection. Do not free this string, it is owned by
 *     @connection.
 *
 * Since: 2.26
 */
const gchar *
g_dbus_connection_get_unique_name (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return NULL;

  return connection->bus_unique_name;
}

/**
 * g_dbus_connection_get_peer_credentials:
 * @connection: a #GDBusConnection
 *
 * Gets the credentials of the authenticated peer. This will always
 * return %NULL unless @connection acted as a server
 * (e.g. %G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER was passed)
 * when set up and the client passed credentials as part of the
 * authentication process.
 *
 * In a message bus setup, the message bus is always the server and
 * each application is a client. So this method will always return
 * %NULL for message bus clients.
 *
 * Returns: (transfer none) (nullable): a #GCredentials or %NULL if not
 *     available. Do not free this object, it is owned by @connection.
 *
 * Since: 2.26
 */
GCredentials *
g_dbus_connection_get_peer_credentials (GDBusConnection *connection)
{
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);

  /* do not use g_return_val_if_fail(), we want the memory barrier */
  if (!check_initialized (connection))
    return NULL;

  return connection->credentials;
}

/* ---------------------------------------------------------------------------------------------------- */

static guint _global_filter_id = 1;  /* (atomic) */

/**
 * g_dbus_connection_add_filter:
 * @connection: a #GDBusConnection
 * @filter_function: a filter function
 * @user_data: user data to pass to @filter_function
 * @user_data_free_func: function to free @user_data with when filter
 *     is removed or %NULL
 *
 * Adds a message filter. Filters are handlers that are run on all
 * incoming and outgoing messages, prior to standard dispatch. Filters
 * are run in the order that they were added.  The same handler can be
 * added as a filter more than once, in which case it will be run more
 * than once.  Filters added during a filter callback won't be run on
 * the message being processed. Filter functions are allowed to modify
 * and even drop messages.
 *
 * Note that filters are run in a dedicated message handling thread so
 * they can't block and, generally, can't do anything but signal a
 * worker thread. Also note that filters are rarely needed - use API
 * such as g_dbus_connection_send_message_with_reply(),
 * g_dbus_connection_signal_subscribe() or g_dbus_connection_call() instead.
 *
 * If a filter consumes an incoming message the message is not
 * dispatched anywhere else - not even the standard dispatch machinery
 * (that API such as g_dbus_connection_signal_subscribe() and
 * g_dbus_connection_send_message_with_reply() relies on) will see the
 * message. Similarly, if a filter consumes an outgoing message, the
 * message will not be sent to the other peer.
 *
 * If @user_data_free_func is non-%NULL, it will be called (in the
 * thread-default main context of the thread you are calling this
 * method from) at some point after @user_data is no longer
 * needed. (It is not guaranteed to be called synchronously when the
 * filter is removed, and may be called after @connection has been
 * destroyed.)
 *
 * Returns: a filter identifier that can be used with
 *     g_dbus_connection_remove_filter()
 *
 * Since: 2.26
 */
guint
g_dbus_connection_add_filter (GDBusConnection            *connection,
                              GDBusMessageFilterFunction  filter_function,
                              gpointer                    user_data,
                              GDestroyNotify              user_data_free_func)
{
  FilterData *data;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), 0);
  g_return_val_if_fail (filter_function != NULL, 0);
  g_return_val_if_fail (check_initialized (connection), 0);

  CONNECTION_LOCK (connection);
  data = g_new0 (FilterData, 1);
  data->id = (guint) g_atomic_int_add (&_global_filter_id, 1); /* TODO: overflow etc. */
  data->ref_count = 1;
  data->filter_function = filter_function;
  data->user_data = user_data;
  data->user_data_free_func = user_data_free_func;
  data->context = g_main_context_ref_thread_default ();
  g_ptr_array_add (connection->filters, data);
  CONNECTION_UNLOCK (connection);

  return data->id;
}

/* only called from finalize(), removes all filters */
static void
purge_all_filters (GDBusConnection *connection)
{
  guint n;

  for (n = 0; n < connection->filters->len; n++)
    filter_data_destroy (connection->filters->pdata[n], FALSE);
}

/**
 * g_dbus_connection_remove_filter:
 * @connection: a #GDBusConnection
 * @filter_id: an identifier obtained from g_dbus_connection_add_filter()
 *
 * Removes a filter.
 *
 * Note that since filters run in a different thread, there is a race
 * condition where it is possible that the filter will be running even
 * after calling g_dbus_connection_remove_filter(), so you cannot just
 * free data that the filter might be using. Instead, you should pass
 * a #GDestroyNotify to g_dbus_connection_add_filter(), which will be
 * called when it is guaranteed that the data is no longer needed.
 *
 * Since: 2.26
 */
void
g_dbus_connection_remove_filter (GDBusConnection *connection,
                                 guint            filter_id)
{
  guint n;
  gboolean found;
  FilterData *to_destroy;

  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));
  g_return_if_fail (check_initialized (connection));

  CONNECTION_LOCK (connection);
  found = FALSE;
  to_destroy = NULL;
  for (n = 0; n < connection->filters->len; n++)
    {
      FilterData *data = connection->filters->pdata[n];
      if (data->id == filter_id)
        {
          found = TRUE;
          g_ptr_array_remove_index (connection->filters, n);
          data->ref_count--;
          if (data->ref_count == 0)
            to_destroy = data;
          break;
        }
    }
  CONNECTION_UNLOCK (connection);

  /* do free without holding lock */
  if (to_destroy != NULL)
    filter_data_destroy (to_destroy, TRUE);
  else if (!found)
    {
      g_warning ("g_dbus_connection_remove_filter: No filter found for filter_id %d", filter_id);
    }
}

/* ---------------------------------------------------------------------------------------------------- */

static gchar *
args_to_rule (const gchar      *sender,
              const gchar      *interface_name,
              const gchar      *member,
              const gchar      *object_path,
              const gchar      *arg0,
              GDBusSignalFlags  flags)
{
  GString *rule;

  rule = g_string_new ("type='signal'");
  if (flags & G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE)
    g_string_prepend_c (rule, '-');
  if (sender != NULL)
    g_string_append_printf (rule, ",sender='%s'", sender);
  if (interface_name != NULL)
    g_string_append_printf (rule, ",interface='%s'", interface_name);
  if (member != NULL)
    g_string_append_printf (rule, ",member='%s'", member);
  if (object_path != NULL)
    g_string_append_printf (rule, ",path='%s'", object_path);

  if (arg0 != NULL)
    {
      if (flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_PATH)
        g_string_append_printf (rule, ",arg0path='%s'", arg0);
      else if (flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_NAMESPACE)
        g_string_append_printf (rule, ",arg0namespace='%s'", arg0);
      else
        g_string_append_printf (rule, ",arg0='%s'", arg0);
    }

  return g_string_free (rule, FALSE);
}

static guint _global_subscriber_id = 1;  /* (atomic) */
static guint _global_registration_id = 1;  /* (atomic) */
static guint _global_subtree_registration_id = 1;  /* (atomic) */

/* ---------------------------------------------------------------------------------------------------- */

/* Called in a user thread, lock is held */
static void
add_match_rule (GDBusConnection *connection,
                const gchar     *match_rule)
{
  GError *error;
  GDBusMessage *message;

  if (match_rule[0] == '-')
    return;

  message = g_dbus_message_new_method_call (DBUS_SERVICE_DBUS,
                                            DBUS_PATH_DBUS,
                                            DBUS_INTERFACE_DBUS,
                                            "AddMatch");
  g_dbus_message_set_body (message, g_variant_new ("(s)", match_rule));
  error = NULL;
  if (!g_dbus_connection_send_message_unlocked (connection,
                                                message,
                                                G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                NULL,
                                                &error))
    {
      g_critical ("Error while sending AddMatch() message: %s", error->message);
      g_error_free (error);
    }
  g_object_unref (message);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called in a user thread, lock is held */
static void
remove_match_rule (GDBusConnection *connection,
                   const gchar     *match_rule)
{
  GError *error;
  GDBusMessage *message;

  if (match_rule[0] == '-')
    return;

  message = g_dbus_message_new_method_call (DBUS_SERVICE_DBUS,
                                            DBUS_PATH_DBUS,
                                            DBUS_INTERFACE_DBUS,
                                            "RemoveMatch");
  g_dbus_message_set_body (message, g_variant_new ("(s)", match_rule));

  error = NULL;
  if (!g_dbus_connection_send_message_unlocked (connection,
                                                message,
                                                G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                NULL,
                                                &error))
    {
      /* If we could get G_IO_ERROR_CLOSED here, it wouldn't be reasonable to
       * critical; but we're holding the lock, and our caller checked whether
       * we were already closed, so we can't get that error.
       */
      g_critical ("Error while sending RemoveMatch() message: %s", error->message);
      g_error_free (error);
    }
  g_object_unref (message);
}

/* ---------------------------------------------------------------------------------------------------- */

static gboolean
is_signal_data_for_name_lost_or_acquired (SignalData *signal_data)
{
  return g_strcmp0 (signal_data->sender, DBUS_SERVICE_DBUS) == 0 &&
         g_strcmp0 (signal_data->interface_name, DBUS_INTERFACE_DBUS) == 0 &&
         g_strcmp0 (signal_data->object_path, DBUS_PATH_DBUS) == 0 &&
         (g_strcmp0 (signal_data->member, "NameLost") == 0 ||
          g_strcmp0 (signal_data->member, "NameAcquired") == 0);
}

/* ---------------------------------------------------------------------------------------------------- */

/* called in any thread, connection lock is held */
static void
add_signal_data (GDBusConnection *connection,
                 SignalData      *signal_data,
                 const char      *sender_unique_name)
{
  GPtrArray *signal_data_array;

  g_hash_table_insert (connection->map_rule_to_signal_data,
                       signal_data->rule,
                       signal_data);

  /* Add the match rule to the bus...
   *
   * Avoid adding match rules for NameLost and NameAcquired messages - the bus will
   * always send such messages to us.
   */
  if (connection->flags & G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION)
    {
      if (!is_signal_data_for_name_lost_or_acquired (signal_data))
        add_match_rule (connection, signal_data->rule);
    }

  signal_data_array = g_hash_table_lookup (connection->map_sender_unique_name_to_signal_data_array,
                                           sender_unique_name);
  if (signal_data_array == NULL)
    {
      signal_data_array = g_ptr_array_new ();
      g_hash_table_insert (connection->map_sender_unique_name_to_signal_data_array,
                           g_strdup (sender_unique_name),
                           signal_data_array);
    }
  g_ptr_array_add (signal_data_array, signal_data);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_signal_subscribe:
 * @connection: a #GDBusConnection
 * @sender: (nullable): sender name to match on (unique or well-known name)
 *     or %NULL to listen from all senders
 * @interface_name: (nullable): D-Bus interface name to match on or %NULL to
 *     match on all interfaces
 * @member: (nullable): D-Bus signal name to match on or %NULL to match on
 *     all signals
 * @object_path: (nullable): object path to match on or %NULL to match on
 *     all object paths
 * @arg0: (nullable): contents of first string argument to match on or %NULL
 *     to match on all kinds of arguments
 * @flags: #GDBusSignalFlags describing how arg0 is used in subscribing to the
 *     signal
 * @callback: callback to invoke when there is a signal matching the requested data
 * @user_data: user data to pass to @callback
 * @user_data_free_func: (nullable): function to free @user_data with when
 *     subscription is removed or %NULL
 *
 * Subscribes to signals on @connection and invokes @callback whenever
 * the signal is received. Note that @callback will be invoked in the
 * thread-default main context (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from.
 *
 * If @connection is not a message bus connection, @sender must be
 * %NULL.
 *
 * If @sender is a well-known name note that @callback is invoked with
 * the unique name for the owner of @sender, not the well-known name
 * as one would expect. This is because the message bus rewrites the
 * name. As such, to avoid certain race conditions, users should be
 * tracking the name owner of the well-known name and use that when
 * processing the received signal.
 *
 * If one of %G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_NAMESPACE or
 * %G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_PATH are given, @arg0 is
 * interpreted as part of a namespace or path.  The first argument
 * of a signal is matched against that part as specified by D-Bus.
 *
 * If @user_data_free_func is non-%NULL, it will be called (in the
 * thread-default main context of the thread you are calling this
 * method from) at some point after @user_data is no longer
 * needed. (It is not guaranteed to be called synchronously when the
 * signal is unsubscribed from, and may be called after @connection
 * has been destroyed.)
 *
 * As @callback is potentially invoked in a different thread from where it’s
 * emitted, it’s possible for this to happen after
 * g_dbus_connection_signal_unsubscribe() has been called in another thread.
 * Due to this, @user_data should have a strong reference which is freed with
 * @user_data_free_func, rather than pointing to data whose lifecycle is tied
 * to the signal subscription. For example, if a #GObject is used to store the
 * subscription ID from g_dbus_connection_signal_subscribe(), a strong reference
 * to that #GObject must be passed to @user_data, and g_object_unref() passed to
 * @user_data_free_func. You are responsible for breaking the resulting
 * reference count cycle by explicitly unsubscribing from the signal when
 * dropping the last external reference to the #GObject. Alternatively, a weak
 * reference may be used.
 *
 * It is guaranteed that if you unsubscribe from a signal using
 * g_dbus_connection_signal_unsubscribe() from the same thread which made the
 * corresponding g_dbus_connection_signal_subscribe() call, @callback will not
 * be invoked after g_dbus_connection_signal_unsubscribe() returns.
 *
 * The returned subscription identifier is an opaque value which is guaranteed
 * to never be zero.
 *
 * This function can never fail.
 *
 * Returns: a subscription identifier that can be used with g_dbus_connection_signal_unsubscribe()
 *
 * Since: 2.26
 */
guint
g_dbus_connection_signal_subscribe (GDBusConnection     *connection,
                                    const gchar         *sender,
                                    const gchar         *interface_name,
                                    const gchar         *member,
                                    const gchar         *object_path,
                                    const gchar         *arg0,
                                    GDBusSignalFlags     flags,
                                    GDBusSignalCallback  callback,
                                    gpointer             user_data,
                                    GDestroyNotify       user_data_free_func)
{
  gchar *rule;
  SignalData *signal_data;
  SignalData *name_watcher = NULL;
  SignalSubscriber *subscriber;
  gboolean sender_is_its_own_owner;
  const gchar *sender_unique_name;

  /* Right now we abort if AddMatch() fails since it can only fail with the bus being in
   * an OOM condition. We might want to change that but that would involve making
   * g_dbus_connection_signal_subscribe() asynchronous and having the call sites
   * handle that. And there's really no sensible way of handling this short of retrying
   * to add the match rule... and then there's the little thing that, hey, maybe there's
   * a reason the bus in an OOM condition.
   *
   * Doable, but not really sure it's worth it...
   */

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), 0);
  g_return_val_if_fail (sender == NULL || (g_dbus_is_name (sender) && (connection->flags & G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION)), 0);
  g_return_val_if_fail (interface_name == NULL || g_dbus_is_interface_name (interface_name), 0);
  g_return_val_if_fail (member == NULL || g_dbus_is_member_name (member), 0);
  g_return_val_if_fail (object_path == NULL || g_variant_is_object_path (object_path), 0);
  g_return_val_if_fail (callback != NULL, 0);
  g_return_val_if_fail (check_initialized (connection), 0);
  g_return_val_if_fail (!((flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_PATH) && (flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_NAMESPACE)), 0);
  g_return_val_if_fail (!(arg0 == NULL && (flags & (G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_PATH | G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_NAMESPACE))), 0);

  CONNECTION_LOCK (connection);

  /* If G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE was specified, we will end up
   * with a '-' character to prefix the rule (which will otherwise be
   * normal).
   *
   * This allows us to hash the rule and do our lifecycle tracking in
   * the usual way, but the '-' prevents the match rule from ever
   * actually being send to the bus (either for add or remove).
   */
  rule = args_to_rule (sender, interface_name, member, object_path, arg0, flags);

  if (sender != NULL && (g_dbus_is_unique_name (sender) || g_strcmp0 (sender, DBUS_SERVICE_DBUS) == 0))
    sender_is_its_own_owner = TRUE;
  else
    sender_is_its_own_owner = FALSE;

  if (sender_is_its_own_owner)
    sender_unique_name = sender;
  else
    sender_unique_name = "";

  subscriber = g_new0 (SignalSubscriber, 1);
  subscriber->ref_count = 1;
  subscriber->callback = callback;
  subscriber->user_data = user_data;
  subscriber->user_data_free_func = user_data_free_func;
  subscriber->id = (guint) g_atomic_int_add (&_global_subscriber_id, 1); /* TODO: overflow etc. */
  subscriber->context = g_main_context_ref_thread_default ();

  /* see if we've already have this rule */
  signal_data = g_hash_table_lookup (connection->map_rule_to_signal_data, rule);
  if (signal_data != NULL)
    {
      g_ptr_array_add (signal_data->subscribers, subscriber);
      g_free (rule);
      goto out;
    }

  signal_data = signal_data_new_take (g_steal_pointer (&rule),
                                      g_strdup (sender),
                                      g_strdup (interface_name),
                                      g_strdup (member),
                                      g_strdup (object_path),
                                      g_strdup (arg0),
                                      flags);
  g_ptr_array_add (signal_data->subscribers, subscriber);

  /* If subscribing to a signal from a specific sender with a well-known
   * name, we must first subscribe to NameOwnerChanged signals for that
   * well-known name, so that we can match the current owner of the name
   * against the sender of each signal. */
  if (sender != NULL && !sender_is_its_own_owner)
    {
      gchar *name_owner_rule = NULL;

      /* We already checked that sender != NULL implies MESSAGE_BUS_CONNECTION */
      g_assert (connection->flags & G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION);

      name_owner_rule = args_to_rule (DBUS_SERVICE_DBUS,
                                      DBUS_INTERFACE_DBUS,
                                      "NameOwnerChanged",
                                      DBUS_PATH_DBUS,
                                      sender,
                                      G_DBUS_SIGNAL_FLAGS_NONE);
      name_watcher = g_hash_table_lookup (connection->map_rule_to_signal_data, name_owner_rule);

      if (name_watcher == NULL)
        {
          name_watcher = signal_data_new_take (g_steal_pointer (&name_owner_rule),
                                               g_strdup (DBUS_SERVICE_DBUS),
                                               g_strdup (DBUS_INTERFACE_DBUS),
                                               g_strdup ("NameOwnerChanged"),
                                               g_strdup (DBUS_PATH_DBUS),
                                               g_strdup (sender),
                                               G_DBUS_SIGNAL_FLAGS_NONE);
          add_signal_data (connection, name_watcher, DBUS_SERVICE_DBUS);
        }

      if (name_watcher->watched_name == NULL)
        {
          name_watcher->watched_name = watched_name_new ();
          name_watcher_call_get_name_owner_unlocked (connection, name_watcher);
        }
      else
        {
          g_ref_count_inc (&name_watcher->watched_name->ref_count);
        }

      signal_data->shared_name_watcher = name_watcher;

      g_clear_pointer (&name_owner_rule, g_free);
    }

  add_signal_data (connection, signal_data, sender_unique_name);

 out:
  g_hash_table_insert (connection->map_id_to_signal_data,
                       GUINT_TO_POINTER (subscriber->id),
                       signal_data);

  CONNECTION_UNLOCK (connection);

  return subscriber->id;
}

/* ---------------------------------------------------------------------------------------------------- */

/*
 * Called in any thread.
 * Must hold the connection lock when calling this, unless
 * connection->finalizing is TRUE.
 * May free signal_data, so do not dereference it after this.
 */
static void
remove_signal_data_if_unused (GDBusConnection *connection,
                              SignalData *signal_data)
{
  const gchar *sender_unique_name;
  GPtrArray *signal_data_array;

  /* Cannot remove while there are still subscribers */
  if (signal_data->subscribers->len != 0)
    return;

  /* Cannot remove while another SignalData is still using this one
   * as its shared_name_watcher, which holds watched_name->ref_count > 0 */
  if (signal_data->watched_name != NULL)
    return;

  /* Point of no return: we have committed to removing it */

  if (signal_data->sender != NULL && signal_data->shared_name_watcher == NULL)
    sender_unique_name = signal_data->sender;
  else
    sender_unique_name = "";

  g_warn_if_fail (g_hash_table_remove (connection->map_rule_to_signal_data, signal_data->rule));

  signal_data_array = g_hash_table_lookup (connection->map_sender_unique_name_to_signal_data_array,
                                           sender_unique_name);
  g_warn_if_fail (signal_data_array != NULL);
  g_warn_if_fail (g_ptr_array_remove (signal_data_array, signal_data));

  if (signal_data_array->len == 0)
    {
      g_warn_if_fail (g_hash_table_remove (connection->map_sender_unique_name_to_signal_data_array,
                                           sender_unique_name));
    }

  /* remove the match rule from the bus unless NameLost or NameAcquired (see subscribe()) */
  if ((connection->flags & G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION) &&
      !is_signal_data_for_name_lost_or_acquired (signal_data) &&
      !g_dbus_connection_is_closed (connection) &&
      !connection->finalizing)
    {
      /* The check for g_dbus_connection_is_closed() means that
       * sending the RemoveMatch message can't fail with
       * G_IO_ERROR_CLOSED, because we're holding the lock,
       * so on_worker_closed() can't happen between the check we just
       * did, and releasing the lock later.
       */
      remove_match_rule (connection, signal_data->rule);
    }

  if (signal_data->shared_name_watcher != NULL)
    {
      SignalData *name_watcher = g_steal_pointer (&signal_data->shared_name_watcher);

      name_watcher_unref_watched_name (connection, name_watcher);
      /* May free signal_data */
      remove_signal_data_if_unused (connection, name_watcher);
    }

  signal_data_free (signal_data);
}

/* called in any thread */
/* must hold lock when calling this (except if connection->finalizing is TRUE)
 * returns the number of removed subscribers */
static guint
unsubscribe_id_internal (GDBusConnection *connection,
                         guint            subscription_id)
{
  SignalData *signal_data;
  guint n;
  guint n_removed = 0;

  signal_data = g_hash_table_lookup (connection->map_id_to_signal_data,
                                     GUINT_TO_POINTER (subscription_id));
  if (signal_data == NULL)
    {
      /* Don't warn here, we may have thrown all subscriptions out when the connection was closed */
      goto out;
    }

  for (n = 0; n < signal_data->subscribers->len; n++)
    {
      SignalSubscriber *subscriber = signal_data->subscribers->pdata[n];

      if (subscriber->id != subscription_id)
        continue;

      /* It’s OK to rearrange the array order using the ‘fast’ #GPtrArray
       * removal functions, since we’re going to exit the loop below anyway — we
       * never move on to the next element. Secondly, subscription IDs are
       * guaranteed to be unique. */
      g_warn_if_fail (g_hash_table_remove (connection->map_id_to_signal_data,
                                           GUINT_TO_POINTER (subscription_id)));
      n_removed++;
      g_ptr_array_remove_index_fast (signal_data->subscribers, n);
      /* May free signal_data */
      remove_signal_data_if_unused (connection, signal_data);
      goto out;
    }

  g_assert_not_reached ();

 out:
  return n_removed;
}

/**
 * g_dbus_connection_signal_unsubscribe:
 * @connection: a #GDBusConnection
 * @subscription_id: a subscription id obtained from
 *     g_dbus_connection_signal_subscribe()
 *
 * Unsubscribes from signals.
 *
 * Note that there may still be D-Bus traffic to process (relating to this
 * signal subscription) in the current thread-default #GMainContext after this
 * function has returned. You should continue to iterate the #GMainContext
 * until the #GDestroyNotify function passed to
 * g_dbus_connection_signal_subscribe() is called, in order to avoid memory
 * leaks through callbacks queued on the #GMainContext after it’s stopped being
 * iterated.
 * Alternatively, any idle source with a priority lower than %G_PRIORITY_DEFAULT
 * that was scheduled after unsubscription, also indicates that all resources
 * of this subscription are released.
 *
 * Since: 2.26
 */
void
g_dbus_connection_signal_unsubscribe (GDBusConnection *connection,
                                      guint            subscription_id)
{
  guint n_subscribers_removed G_GNUC_UNUSED  /* when compiling with G_DISABLE_ASSERT */;

  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));
  g_return_if_fail (check_initialized (connection));

  CONNECTION_LOCK (connection);
  n_subscribers_removed = unsubscribe_id_internal (connection, subscription_id);
  CONNECTION_UNLOCK (connection);

  /* invariant */
  g_assert (n_subscribers_removed == 0 || n_subscribers_removed == 1);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  SignalSubscriber    *subscriber;  /* (owned) */
  GDBusMessage        *message;  /* (owned) */
  GDBusConnection     *connection;
  const gchar         *sender;  /* (nullable) for peer-to-peer connections */
  const gchar         *path;
  const gchar         *interface;
  const gchar         *member;
} SignalInstance;

/* called on delivery thread (e.g. where g_dbus_connection_signal_subscribe() was called) with
 * no locks held
 */
static gboolean
emit_signal_instance_in_idle_cb (gpointer data)
{
  SignalInstance *signal_instance = data;
  GVariant *parameters;
  gboolean has_subscription;

  parameters = g_dbus_message_get_body (signal_instance->message);
  if (parameters == NULL)
    {
      parameters = g_variant_new ("()");
      g_variant_ref_sink (parameters);
    }
  else
    {
      g_variant_ref_sink (parameters);
    }

#if 0
  g_print ("in emit_signal_instance_in_idle_cb (id=%d sender=%s path=%s interface=%s member=%s params=%s)\n",
           signal_instance->subscriber->id,
           signal_instance->sender,
           signal_instance->path,
           signal_instance->interface,
           signal_instance->member,
           g_variant_print (parameters, TRUE));
#endif

  /* Careful here, don't do the callback if we no longer has the subscription */
  CONNECTION_LOCK (signal_instance->connection);
  has_subscription = FALSE;
  if (g_hash_table_lookup (signal_instance->connection->map_id_to_signal_data,
                           GUINT_TO_POINTER (signal_instance->subscriber->id)) != NULL)
    has_subscription = TRUE;
  CONNECTION_UNLOCK (signal_instance->connection);

  if (has_subscription)
    signal_instance->subscriber->callback (signal_instance->connection,
                                           signal_instance->sender,
                                           signal_instance->path,
                                           signal_instance->interface,
                                           signal_instance->member,
                                           parameters,
                                           signal_instance->subscriber->user_data);

  g_variant_unref (parameters);

  return FALSE;
}

static void
signal_instance_free (SignalInstance *signal_instance)
{
  g_clear_object (&signal_instance->message);
  g_object_unref (signal_instance->connection);
  signal_subscriber_unref (signal_instance->subscriber);
  g_free (signal_instance);
}

static gboolean
namespace_rule_matches (const gchar *namespace,
                        const gchar *name)
{
  size_t len_namespace;
  size_t len_name;

  len_namespace = strlen (namespace);
  len_name = strlen (name);

  if (len_name < len_namespace)
    return FALSE;

  if (memcmp (namespace, name, len_namespace) != 0)
    return FALSE;

  return len_namespace == len_name || name[len_namespace] == '.';
}

static gboolean
path_rule_matches (const gchar *path_a,
                   const gchar *path_b)
{
  size_t len_a, len_b;

  len_a = strlen (path_a);
  len_b = strlen (path_b);

  if (len_a < len_b && (len_a == 0 || path_a[len_a - 1] != '/'))
    return FALSE;

  if (len_b < len_a && (len_b == 0 || path_b[len_b - 1] != '/'))
    return FALSE;

  return memcmp (path_a, path_b, MIN (len_a, len_b)) == 0;
}

/* called in GDBusWorker thread WITH lock held
 *
 * @sender is (nullable) for peer-to-peer connections */
static void
schedule_callbacks (GDBusConnection *connection,
                    GPtrArray       *signal_data_array,
                    GDBusMessage    *message,
                    const gchar     *sender)
{
  guint n, m;
  const gchar *interface;
  const gchar *member;
  const gchar *path;
  const gchar *arg0;
  const gchar *arg0_path;

  interface = NULL;
  member = NULL;
  path = NULL;
  arg0 = NULL;
  arg0_path = NULL;

  interface = g_dbus_message_get_interface (message);
  member = g_dbus_message_get_member (message);
  path = g_dbus_message_get_path (message);
  arg0 = g_dbus_message_get_arg0 (message);
  arg0_path = g_dbus_message_get_arg0_path (message);

  /* These two are mutually exclusive through the type system. */
  g_assert (arg0 == NULL || arg0_path == NULL);

#if 0
  g_print ("In schedule_callbacks:\n"
           "  sender    = '%s'\n"
           "  interface = '%s'\n"
           "  member    = '%s'\n"
           "  path      = '%s'\n"
           "  arg0      = '%s'\n",
           sender,
           interface,
           member,
           path,
           arg0);
#endif

  /* TODO: if this is slow, then we can change signal_data_array into
   *       map_object_path_to_signal_data_array or something.
   */
  for (n = 0; n < signal_data_array->len; n++)
    {
      SignalData *signal_data = signal_data_array->pdata[n];

      if (signal_data->interface_name != NULL && g_strcmp0 (signal_data->interface_name, interface) != 0)
        continue;

      if (signal_data->member != NULL && g_strcmp0 (signal_data->member, member) != 0)
        continue;

      if (signal_data->object_path != NULL && g_strcmp0 (signal_data->object_path, path) != 0)
        continue;

      if (signal_data->shared_name_watcher != NULL)
        {
          /* We want signals from a specified well-known name, which means
           * the signal's sender needs to be the unique name that currently
           * owns that well-known name, and we will have found this
           * SignalData in
           * connection->map_sender_unique_name_to_signal_data_array[""]. */
          const WatchedName *watched_name;
          const char *current_owner;

          g_assert (signal_data->sender != NULL);
          /* Invariant: We never need to watch for the owner of a unique
           * name, or for the owner of DBUS_SERVICE_DBUS, either of which
           * is always its own owner */
          g_assert (!g_dbus_is_unique_name (signal_data->sender));
          g_assert (g_strcmp0 (signal_data->sender, DBUS_SERVICE_DBUS) != 0);

          watched_name = signal_data->shared_name_watcher->watched_name;
          g_assert (watched_name != NULL);
          current_owner = watched_name->owner;

          /* Skip the signal if the actual sender is not known to own
           * the required name */
          if (current_owner == NULL || g_strcmp0 (current_owner, sender) != 0)
            continue;
        }
      else if (signal_data->sender != NULL)
        {
          /* We want signals from a unique name or o.fd.DBus... */
          g_assert (g_dbus_is_unique_name (signal_data->sender)
                    || g_str_equal (signal_data->sender, DBUS_SERVICE_DBUS));

          /* ... which means we must have found this SignalData in
           * connection->map_sender_unique_name_to_signal_data_array[signal_data->sender],
           * therefore we would only have found it if the signal's
           * actual sender matches the required signal_data->sender */
          g_assert (g_strcmp0 (signal_data->sender, sender) == 0);
        }
      /* else the sender is unspecified and we will accept anything */

      if (signal_data->arg0 != NULL)
        {
          if (signal_data->flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_NAMESPACE)
            {
              if (arg0 == NULL || !namespace_rule_matches (signal_data->arg0, arg0))
                continue;
            }
          else if (signal_data->flags & G_DBUS_SIGNAL_FLAGS_MATCH_ARG0_PATH)
            {
              if ((arg0 == NULL || !path_rule_matches (signal_data->arg0, arg0)) &&
                  (arg0_path == NULL || !path_rule_matches (signal_data->arg0, arg0_path)))
                continue;
            }
          else if (arg0 == NULL || !g_str_equal (signal_data->arg0, arg0))
            continue;
        }

      if (signal_data->watched_name != NULL)
        {
          /* Invariant: SignalData should only have a watched_name if it
           * represents the NameOwnerChanged signal */
          g_assert (g_strcmp0 (sender, DBUS_SERVICE_DBUS) == 0);
          g_assert (g_strcmp0 (interface, DBUS_INTERFACE_DBUS) == 0);
          g_assert (g_strcmp0 (path, DBUS_PATH_DBUS) == 0);
          g_assert (g_strcmp0 (member, "NameOwnerChanged") == 0);
          name_watcher_deliver_name_owner_changed_unlocked (signal_data, message);
        }

      for (m = 0; m < signal_data->subscribers->len; m++)
        {
          SignalSubscriber *subscriber = signal_data->subscribers->pdata[m];
          GSource *idle_source;
          SignalInstance *signal_instance;

          signal_instance = g_new0 (SignalInstance, 1);
          signal_instance->subscriber = signal_subscriber_ref (subscriber);
          signal_instance->message = g_object_ref (message);
          signal_instance->connection = g_object_ref (connection);
          signal_instance->sender = sender;
          signal_instance->path = path;
          signal_instance->interface = interface;
          signal_instance->member = member;

          idle_source = g_idle_source_new ();
          g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
          g_source_set_callback (idle_source,
                                 emit_signal_instance_in_idle_cb,
                                 signal_instance,
                                 (GDestroyNotify) signal_instance_free);
          g_source_set_static_name (idle_source, "[gio] emit_signal_instance_in_idle_cb");
          g_source_attach (idle_source, subscriber->context);
          g_source_unref (idle_source);
        }
    }
}

/* called in GDBusWorker thread with lock held */
static void
distribute_signals (GDBusConnection *connection,
                    GDBusMessage    *message)
{
  GPtrArray *signal_data_array;
  const gchar *sender, *interface, *member, *path;

  g_assert (g_dbus_message_get_message_type (message) == G_DBUS_MESSAGE_TYPE_SIGNAL);

  sender = g_dbus_message_get_sender (message);

  /* all three of these are required, but should have been validated already
   * by validate_headers() in gdbusmessage.c */
  interface = g_dbus_message_get_interface (message);
  member = g_dbus_message_get_member (message);
  path = g_dbus_message_get_path (message);

  g_assert (interface != NULL);
  g_assert (member != NULL);
  g_assert (path != NULL);

  if (G_UNLIKELY (_g_dbus_debug_signal ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Signal:\n"
               " <<<< RECEIVED SIGNAL %s.%s\n"
               "      on object %s\n"
               "      sent by name %s\n",
               interface, member, path,
               sender != NULL ? sender : "(none)");
      _g_dbus_debug_print_unlock ();
    }

  /* collect subscribers that match on sender */
  if (sender != NULL)
    {
      signal_data_array = g_hash_table_lookup (connection->map_sender_unique_name_to_signal_data_array, sender);
      if (signal_data_array != NULL)
        schedule_callbacks (connection, signal_data_array, message, sender);
    }

  /* collect subscribers not matching on sender, or matching a well-known name */
  signal_data_array = g_hash_table_lookup (connection->map_sender_unique_name_to_signal_data_array, "");
  if (signal_data_array != NULL)
    schedule_callbacks (connection, signal_data_array, message, sender);
}

/* ---------------------------------------------------------------------------------------------------- */

/* only called from finalize(), removes all subscriptions */
static void
purge_all_signal_subscriptions (GDBusConnection *connection)
{
  GHashTableIter iter;
  gpointer key;
  GArray *ids;
  guint n;

  ids = g_array_new (FALSE, FALSE, sizeof (guint));
  g_hash_table_iter_init (&iter, connection->map_id_to_signal_data);
  while (g_hash_table_iter_next (&iter, &key, NULL))
    {
      guint subscription_id = GPOINTER_TO_UINT (key);
      g_array_append_val (ids, subscription_id);
    }

  for (n = 0; n < ids->len; n++)
    {
      guint subscription_id = g_array_index (ids, guint, n);
      unsubscribe_id_internal (connection, subscription_id);
    }
  g_array_free (ids, TRUE);
}

/* ---------------------------------------------------------------------------------------------------- */

static GDBusInterfaceVTable *
_g_dbus_interface_vtable_copy (const GDBusInterfaceVTable *vtable)
{
  /* Don't waste memory by copying padding - remember to update this
   * when changing struct _GDBusInterfaceVTable in gdbusconnection.h
   */
  return g_memdup2 ((gconstpointer) vtable, 3 * sizeof (gpointer));
}

static void
_g_dbus_interface_vtable_free (GDBusInterfaceVTable *vtable)
{
  g_free (vtable);
}

/* ---------------------------------------------------------------------------------------------------- */

static GDBusSubtreeVTable *
_g_dbus_subtree_vtable_copy (const GDBusSubtreeVTable *vtable)
{
  /* Don't waste memory by copying padding - remember to update this
   * when changing struct _GDBusSubtreeVTable in gdbusconnection.h
   */
  return g_memdup2 ((gconstpointer) vtable, 3 * sizeof (gpointer));
}

static void
_g_dbus_subtree_vtable_free (GDBusSubtreeVTable *vtable)
{
  g_free (vtable);
}

/* ---------------------------------------------------------------------------------------------------- */

struct ExportedObject
{
  gchar *object_path;
  GDBusConnection *connection;

  /* maps gchar* -> ExportedInterface* */
  GHashTable *map_if_name_to_ei;
};

/* only called with lock held */
static void
exported_object_free (ExportedObject *eo)
{
  g_free (eo->object_path);
  g_hash_table_unref (eo->map_if_name_to_ei);
  g_free (eo);
}

typedef struct
{
  ExportedObject *eo;

  gint                        refcount;  /* (atomic) */

  guint                       id;
  gchar                      *interface_name;  /* (owned) */
  GDBusInterfaceVTable       *vtable;  /* (owned) */
  GDBusInterfaceInfo         *interface_info;  /* (owned) */

  GMainContext               *context;  /* (owned) */
  gpointer                    user_data;
  GDestroyNotify              user_data_free_func;
} ExportedInterface;

static ExportedInterface *
exported_interface_ref (ExportedInterface *ei)
{
  g_atomic_int_inc (&ei->refcount);

  return ei;
}

/* May be called with lock held */
static void
exported_interface_unref (ExportedInterface *ei)
{
  if (!g_atomic_int_dec_and_test (&ei->refcount))
    return;

  g_dbus_interface_info_cache_release (ei->interface_info);
  g_dbus_interface_info_unref ((GDBusInterfaceInfo *) ei->interface_info);

  /* All uses of ei->vtable from callbacks scheduled in idle functions must
   * have completed by this call_destroy_notify() call, as language bindings
   * may destroy function closures in this callback. */
  call_destroy_notify (ei->context,
                       ei->user_data_free_func,
                       ei->user_data);

  g_main_context_unref (ei->context);

  g_free (ei->interface_name);
  _g_dbus_interface_vtable_free (ei->vtable);
  g_free (ei);
}

struct ExportedSubtree
{
  gint                      refcount;  /* (atomic) */

  guint                     id;
  gchar                    *object_path;  /* (owned) */
  GDBusConnection          *connection;  /* (unowned) */
  GDBusSubtreeVTable       *vtable;  /* (owned) */
  GDBusSubtreeFlags         flags;

  GMainContext             *context;  /* (owned) */
  gpointer                  user_data;
  GDestroyNotify            user_data_free_func;
};

static ExportedSubtree *
exported_subtree_ref (ExportedSubtree *es)
{
  g_atomic_int_inc (&es->refcount);

  return es;
}

/* May be called with lock held */
static void
exported_subtree_unref (ExportedSubtree *es)
{
  if (!g_atomic_int_dec_and_test (&es->refcount))
    return;

  /* All uses of es->vtable from callbacks scheduled in idle functions must
   * have completed by this call_destroy_notify() call, as language bindings
   * may destroy function closures in this callback. */
  call_destroy_notify (es->context,
                       es->user_data_free_func,
                       es->user_data);

  g_main_context_unref (es->context);

  _g_dbus_subtree_vtable_free (es->vtable);
  g_free (es->object_path);
  g_free (es);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Convenience function to check if @registration_id (if not zero) or
 * @subtree_registration_id (if not zero) has been unregistered. If
 * so, returns %TRUE.
 *
 * If not, sets @out_ei and/or @out_es to a strong reference to the relevant
 * #ExportedInterface/#ExportedSubtree and returns %FALSE.
 *
 * May be called by any thread. Caller must *not* hold lock.
 */
static gboolean
has_object_been_unregistered (GDBusConnection    *connection,
                              guint               registration_id,
                              ExportedInterface **out_ei,
                              guint               subtree_registration_id,
                              ExportedSubtree   **out_es)
{
  gboolean ret;
  ExportedInterface *ei = NULL;
  gpointer es = NULL;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);

  ret = FALSE;

  CONNECTION_LOCK (connection);

  if (registration_id != 0)
    {
      ei = g_hash_table_lookup (connection->map_id_to_ei, GUINT_TO_POINTER (registration_id));
      if (ei == NULL)
        ret = TRUE;
      else if (out_ei != NULL)
        *out_ei = exported_interface_ref (ei);
    }
  if (subtree_registration_id != 0)
    {
      es = g_hash_table_lookup (connection->map_id_to_es, GUINT_TO_POINTER (subtree_registration_id));
      if (es == NULL)
        ret = TRUE;
      else if (out_es != NULL)
        *out_es = exported_subtree_ref (es);
    }

  CONNECTION_UNLOCK (connection);

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  GDBusConnection *connection;
  GDBusMessage *message;  /* (owned) */
  gpointer user_data;
  const gchar *property_name;
  const GDBusInterfaceVTable *vtable;
  GDBusInterfaceInfo *interface_info;
  const GDBusPropertyInfo *property_info;
  guint registration_id;
  guint subtree_registration_id;
} PropertyData;

static void
property_data_free (PropertyData *data)
{
  g_object_unref (data->connection);
  g_clear_object (&data->message);
  g_free (data);
}

/* called in thread where object was registered - no locks held */
static gboolean
invoke_get_property_in_idle_cb (gpointer _data)
{
  PropertyData *data = _data;
  GVariant *value;
  GError *error;
  GDBusMessage *reply;
  ExportedInterface *ei = NULL;
  ExportedSubtree *es = NULL;

  if (has_object_been_unregistered (data->connection,
                                    data->registration_id,
                                    &ei,
                                    data->subtree_registration_id,
                                    &es))
    {
      reply = g_dbus_message_new_method_error (data->message,
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               /* Translators: The first placeholder is a D-Bus interface,
                                                * the second is the path of an object. */
                                               _("No such interface “%s” on object at path %s"),
                                               DBUS_INTERFACE_PROPERTIES,
                                               g_dbus_message_get_path (data->message));
      g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      goto out;
    }

  error = NULL;
  value = data->vtable->get_property (data->connection,
                                      g_dbus_message_get_sender (data->message),
                                      g_dbus_message_get_path (data->message),
                                      data->interface_info->name,
                                      data->property_name,
                                      &error,
                                      data->user_data);


  if (value != NULL)
    {
      g_assert_no_error (error);

      g_variant_take_ref (value);
      reply = g_dbus_message_new_method_reply (data->message);
      g_dbus_message_set_body (reply, g_variant_new ("(v)", value));
      g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_variant_unref (value);
      g_object_unref (reply);
    }
  else
    {
      gchar *dbus_error_name;
      g_assert (error != NULL);
      dbus_error_name = g_dbus_error_encode_gerror (error);
      reply = g_dbus_message_new_method_error_literal (data->message,
                                                       dbus_error_name,
                                                       error->message);
      g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_free (dbus_error_name);
      g_error_free (error);
      g_object_unref (reply);
    }

 out:
  g_clear_pointer (&ei, exported_interface_unref);
  g_clear_pointer (&es, exported_subtree_unref);

  return FALSE;
}

/* called in thread where object was registered - no locks held */
static gboolean
invoke_set_property_in_idle_cb (gpointer _data)
{
  PropertyData *data = _data;
  GError *error;
  GDBusMessage *reply;
  GVariant *value;

  error = NULL;
  value = NULL;

  g_variant_get (g_dbus_message_get_body (data->message),
                 "(ssv)",
                 NULL,
                 NULL,
                 &value);

  if (!data->vtable->set_property (data->connection,
                                   g_dbus_message_get_sender (data->message),
                                   g_dbus_message_get_path (data->message),
                                   data->interface_info->name,
                                   data->property_name,
                                   value,
                                   &error,
                                   data->user_data))
    {
      gchar *dbus_error_name;
      g_assert (error != NULL);
      dbus_error_name = g_dbus_error_encode_gerror (error);
      reply = g_dbus_message_new_method_error_literal (data->message,
                                                       dbus_error_name,
                                                       error->message);
      g_free (dbus_error_name);
      g_error_free (error);
    }
  else
    {
      reply = g_dbus_message_new_method_reply (data->message);
    }

  g_assert (reply != NULL);
  g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);
  g_variant_unref (value);

  return FALSE;
}

/* called in any thread with connection's lock held */
static gboolean
validate_and_maybe_schedule_property_getset (GDBusConnection            *connection,
                                             GDBusMessage               *message,
                                             guint                       registration_id,
                                             guint                       subtree_registration_id,
                                             gboolean                    is_get,
                                             GDBusInterfaceInfo         *interface_info,
                                             const GDBusInterfaceVTable *vtable,
                                             GMainContext               *main_context,
                                             gpointer                    user_data)
{
  gboolean handled;
  const char *interface_name;
  const char *property_name;
  const GDBusPropertyInfo *property_info;
  GSource *idle_source;
  PropertyData *property_data;
  GDBusMessage *reply;

  handled = FALSE;

  if (is_get)
    g_variant_get (g_dbus_message_get_body (message),
                   "(&s&s)",
                   &interface_name,
                   &property_name);
  else
    g_variant_get (g_dbus_message_get_body (message),
                   "(&s&sv)",
                   &interface_name,
                   &property_name,
                   NULL);

  if (vtable == NULL)
    goto out;

  /* Check that the property exists - if not fail with DBUS_ERROR_INVALID_ARGS
   */
  property_info = NULL;

  /* TODO: the cost of this is O(n) - it might be worth caching the result */
  property_info = g_dbus_interface_info_lookup_property (interface_info, property_name);
  if (property_info == NULL)
    {
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("No such property “%s”"),
                                               property_name);
      g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }

  if (is_get && !(property_info->flags & G_DBUS_PROPERTY_INFO_FLAGS_READABLE))
    {
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("Property “%s” is not readable"),
                                               property_name);
      g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }
  else if (!is_get && !(property_info->flags & G_DBUS_PROPERTY_INFO_FLAGS_WRITABLE))
    {
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("Property “%s” is not writable"),
                                               property_name);
      g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }

  if (!is_get)
    {
      GVariant *value;

      /* Fail with DBUS_ERROR_INVALID_ARGS if the type
       * of the given value is wrong
       */
      g_variant_get_child (g_dbus_message_get_body (message), 2, "v", &value);
      if (g_strcmp0 (g_variant_get_type_string (value), property_info->signature) != 0)
        {
          reply = g_dbus_message_new_method_error (message,
                                                   DBUS_ERROR_INVALID_ARGS,
                                                   _("Error setting property “%s”: Expected type “%s” but got “%s”"),
                                                   property_name, property_info->signature,
                                                   g_variant_get_type_string (value));
          g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
          g_variant_unref (value);
          g_object_unref (reply);
          handled = TRUE;
          goto out;
        }

      g_variant_unref (value);
    }

  /* If the vtable pointer for get_property() resp. set_property() is
   * NULL then dispatch the call via the method_call() handler.
   */
  if (is_get)
    {
      if (vtable->get_property == NULL)
        {
          schedule_method_call (connection, message, registration_id, subtree_registration_id,
                                interface_info, NULL, property_info, g_dbus_message_get_body (message),
                                vtable, main_context, user_data);
          handled = TRUE;
          goto out;
        }
    }
  else
    {
      if (vtable->set_property == NULL)
        {
          schedule_method_call (connection, message, registration_id, subtree_registration_id,
                                interface_info, NULL, property_info, g_dbus_message_get_body (message),
                                vtable, main_context, user_data);
          handled = TRUE;
          goto out;
        }
    }

  /* ok, got the property info - call user code in an idle handler */
  property_data = g_new0 (PropertyData, 1);
  property_data->connection = g_object_ref (connection);
  property_data->message = g_object_ref (message);
  property_data->user_data = user_data;
  property_data->property_name = property_name;
  property_data->vtable = vtable;
  property_data->interface_info = interface_info;
  property_data->property_info = property_info;
  property_data->registration_id = registration_id;
  property_data->subtree_registration_id = subtree_registration_id;

  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
  g_source_set_callback (idle_source,
                         is_get ? invoke_get_property_in_idle_cb : invoke_set_property_in_idle_cb,
                         property_data,
                         (GDestroyNotify) property_data_free);
  if (is_get)
    g_source_set_static_name (idle_source, "[gio] invoke_get_property_in_idle_cb");
  else
    g_source_set_static_name (idle_source, "[gio] invoke_set_property_in_idle_cb");
  g_source_attach (idle_source, main_context);
  g_source_unref (idle_source);

  handled = TRUE;

 out:
  return handled;
}

/* called in GDBusWorker thread with connection's lock held */
static gboolean
handle_getset_property (GDBusConnection *connection,
                        ExportedObject  *eo,
                        GDBusMessage    *message,
                        gboolean         is_get)
{
  ExportedInterface *ei;
  gboolean handled;
  const char *interface_name;
  const char *property_name;

  handled = FALSE;

  if (is_get)
    g_variant_get (g_dbus_message_get_body (message),
                   "(&s&s)",
                   &interface_name,
                   &property_name);
  else
    g_variant_get (g_dbus_message_get_body (message),
                   "(&s&sv)",
                   &interface_name,
                   &property_name,
                   NULL);

  /* Fail with DBUS_ERROR_INVALID_ARGS if there is
   * no such interface registered
   */
  ei = g_hash_table_lookup (eo->map_if_name_to_ei, interface_name);
  if (ei == NULL)
    {
      GDBusMessage *reply;
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("No such interface “%s”"),
                                               interface_name);
      g_dbus_connection_send_message_unlocked (eo->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }

  handled = validate_and_maybe_schedule_property_getset (eo->connection,
                                                         message,
                                                         ei->id,
                                                         0,
                                                         is_get,
                                                         ei->interface_info,
                                                         ei->vtable,
                                                         ei->context,
                                                         ei->user_data);
 out:
  return handled;
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
  GDBusConnection *connection;
  GDBusMessage *message;  /* (owned) */
  gpointer user_data;
  const GDBusInterfaceVTable *vtable;
  GDBusInterfaceInfo *interface_info;
  guint registration_id;
  guint subtree_registration_id;
} PropertyGetAllData;

static void
property_get_all_data_free (PropertyGetAllData *data)
{
  g_object_unref (data->connection);
  g_clear_object (&data->message);
  g_free (data);
}

/* called in thread where object was registered - no locks held */
static gboolean
invoke_get_all_properties_in_idle_cb (gpointer _data)
{
  PropertyGetAllData *data = _data;
  GVariantBuilder builder;
  GDBusMessage *reply;
  guint n;
  ExportedInterface *ei = NULL;
  ExportedSubtree *es = NULL;

  if (has_object_been_unregistered (data->connection,
                                    data->registration_id,
                                    &ei,
                                    data->subtree_registration_id,
                                    &es))
    {
      reply = g_dbus_message_new_method_error (data->message,
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               _("No such interface “%s” on object at path %s"),
                                               DBUS_INTERFACE_PROPERTIES,
                                               g_dbus_message_get_path (data->message));
      g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      goto out;
    }

  /* TODO: Right now we never fail this call - we just omit values if
   *       a get_property() call is failing.
   *
   *       We could fail the whole call if just a single get_property() call
   *       returns an error. We need clarification in the D-Bus spec about this.
   */
  g_variant_builder_init_static (&builder, G_VARIANT_TYPE ("(a{sv})"));
  g_variant_builder_open (&builder, G_VARIANT_TYPE ("a{sv}"));
  for (n = 0; data->interface_info->properties != NULL && data->interface_info->properties[n] != NULL; n++)
    {
      const GDBusPropertyInfo *property_info = data->interface_info->properties[n];
      GVariant *value;

      if (!(property_info->flags & G_DBUS_PROPERTY_INFO_FLAGS_READABLE))
        continue;

      value = data->vtable->get_property (data->connection,
                                          g_dbus_message_get_sender (data->message),
                                          g_dbus_message_get_path (data->message),
                                          data->interface_info->name,
                                          property_info->name,
                                          NULL,
                                          data->user_data);

      if (value == NULL)
        continue;

      g_variant_take_ref (value);
      g_variant_builder_add (&builder,
                             "{sv}",
                             property_info->name,
                             value);
      g_variant_unref (value);
    }
  g_variant_builder_close (&builder);

  reply = g_dbus_message_new_method_reply (data->message);
  g_dbus_message_set_body (reply, g_variant_builder_end (&builder));
  g_dbus_connection_send_message (data->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);

 out:
  g_clear_pointer (&ei, exported_interface_unref);
  g_clear_pointer (&es, exported_subtree_unref);

  return FALSE;
}

static gboolean
interface_has_readable_properties (GDBusInterfaceInfo *interface_info)
{
  gint i;

  if (!interface_info->properties)
    return FALSE;

  for (i = 0; interface_info->properties[i]; i++)
    if (interface_info->properties[i]->flags & G_DBUS_PROPERTY_INFO_FLAGS_READABLE)
      return TRUE;

  return FALSE;
}

/* called in any thread with connection's lock held */
static gboolean
validate_and_maybe_schedule_property_get_all (GDBusConnection            *connection,
                                              GDBusMessage               *message,
                                              guint                       registration_id,
                                              guint                       subtree_registration_id,
                                              GDBusInterfaceInfo         *interface_info,
                                              const GDBusInterfaceVTable *vtable,
                                              GMainContext               *main_context,
                                              gpointer                    user_data)
{
  gboolean handled;
  GSource *idle_source;
  PropertyGetAllData *property_get_all_data;

  handled = FALSE;

  if (vtable == NULL)
    goto out;

  /* If the vtable pointer for get_property() is NULL but we have a
   * non-zero number of readable properties, then dispatch the call via
   * the method_call() handler.
   */
  if (vtable->get_property == NULL && interface_has_readable_properties (interface_info))
    {
      schedule_method_call (connection, message, registration_id, subtree_registration_id,
                            interface_info, NULL, NULL, g_dbus_message_get_body (message),
                            vtable, main_context, user_data);
      handled = TRUE;
      goto out;
    }

  /* ok, got the property info - call user in an idle handler */
  property_get_all_data = g_new0 (PropertyGetAllData, 1);
  property_get_all_data->connection = g_object_ref (connection);
  property_get_all_data->message = g_object_ref (message);
  property_get_all_data->user_data = user_data;
  property_get_all_data->vtable = vtable;
  property_get_all_data->interface_info = interface_info;
  property_get_all_data->registration_id = registration_id;
  property_get_all_data->subtree_registration_id = subtree_registration_id;

  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
  g_source_set_callback (idle_source,
                         invoke_get_all_properties_in_idle_cb,
                         property_get_all_data,
                         (GDestroyNotify) property_get_all_data_free);
  g_source_set_static_name (idle_source, "[gio] invoke_get_all_properties_in_idle_cb");
  g_source_attach (idle_source, main_context);
  g_source_unref (idle_source);

  handled = TRUE;

 out:
  return handled;
}

/* called in GDBusWorker thread with connection's lock held */
static gboolean
handle_get_all_properties (GDBusConnection *connection,
                           ExportedObject  *eo,
                           GDBusMessage    *message)
{
  ExportedInterface *ei;
  gboolean handled;
  const char *interface_name;

  handled = FALSE;

  g_variant_get (g_dbus_message_get_body (message),
                 "(&s)",
                 &interface_name);

  /* Fail with DBUS_ERROR_INVALID_ARGS if there is
   * no such interface registered
   */
  ei = g_hash_table_lookup (eo->map_if_name_to_ei, interface_name);
  if (ei == NULL)
    {
      GDBusMessage *reply;
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("No such interface “%s”"),
                                               interface_name);
      g_dbus_connection_send_message_unlocked (eo->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }

  handled = validate_and_maybe_schedule_property_get_all (eo->connection,
                                                          message,
                                                          ei->id,
                                                          0,
                                                          ei->interface_info,
                                                          ei->vtable,
                                                          ei->context,
                                                          ei->user_data);
 out:
  return handled;
}

/* ---------------------------------------------------------------------------------------------------- */

static const gchar introspect_header[] =
  "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
  "                      \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
  "<!-- GDBus " PACKAGE_VERSION " -->\n"
  "<node>\n";

static const gchar introspect_tail[] =
  "</node>\n";

static const gchar introspect_properties_interface[] =
  "  <interface name=\"" DBUS_INTERFACE_PROPERTIES "\">\n"
  "    <method name=\"Get\">\n"
  "      <arg type=\"s\" name=\"interface_name\" direction=\"in\"/>\n"
  "      <arg type=\"s\" name=\"property_name\" direction=\"in\"/>\n"
  "      <arg type=\"v\" name=\"value\" direction=\"out\"/>\n"
  "    </method>\n"
  "    <method name=\"GetAll\">\n"
  "      <arg type=\"s\" name=\"interface_name\" direction=\"in\"/>\n"
  "      <arg type=\"a{sv}\" name=\"properties\" direction=\"out\"/>\n"
  "    </method>\n"
  "    <method name=\"Set\">\n"
  "      <arg type=\"s\" name=\"interface_name\" direction=\"in\"/>\n"
  "      <arg type=\"s\" name=\"property_name\" direction=\"in\"/>\n"
  "      <arg type=\"v\" name=\"value\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <signal name=\"PropertiesChanged\">\n"
  "      <arg type=\"s\" name=\"interface_name\"/>\n"
  "      <arg type=\"a{sv}\" name=\"changed_properties\"/>\n"
  "      <arg type=\"as\" name=\"invalidated_properties\"/>\n"
  "    </signal>\n"
  "  </interface>\n";

static const gchar introspect_introspectable_interface[] =
  "  <interface name=\"" DBUS_INTERFACE_INTROSPECTABLE "\">\n"
  "    <method name=\"Introspect\">\n"
  "      <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
  "    </method>\n"
  "  </interface>\n"
  "  <interface name=\"" DBUS_INTERFACE_PEER "\">\n"
  "    <method name=\"Ping\"/>\n"
  "    <method name=\"GetMachineId\">\n"
  "      <arg type=\"s\" name=\"machine_uuid\" direction=\"out\"/>\n"
  "    </method>\n"
  "  </interface>\n";

static void
introspect_append_header (GString *s)
{
  g_string_append (s, introspect_header);
}

static void
maybe_add_path (const gchar *path, gsize path_len, const gchar *object_path, GHashTable *set)
{
  if (g_str_has_prefix (object_path, path) && strlen (object_path) > path_len && object_path[path_len-1] == '/')
    {
      const gchar *begin;
      const gchar *end;
      gchar *s;

      begin = object_path + path_len;
      end = strchr (begin, '/');
      if (end != NULL)
        s = g_strndup (begin, end - begin);
      else
        s = g_strdup (begin);

      if (!g_hash_table_contains (set, s))
        g_hash_table_add (set, s);
      else
        g_free (s);
    }
}

/* TODO: we want a nicer public interface for this */
/* called in any thread with connection's lock held */
static gchar **
g_dbus_connection_list_registered_unlocked (GDBusConnection *connection,
                                            const gchar     *path)
{
  GPtrArray *p;
  gchar **ret;
  GHashTableIter hash_iter;
  const gchar *object_path;
  gsize path_len;
  GHashTable *set;

  CONNECTION_ENSURE_LOCK (connection);

  path_len = strlen (path);
  if (path_len > 1)
    path_len++;

  set = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_iter_init (&hash_iter, connection->map_object_path_to_eo);
  while (g_hash_table_iter_next (&hash_iter, (gpointer) &object_path, NULL))
    maybe_add_path (path, path_len, object_path, set);

  g_hash_table_iter_init (&hash_iter, connection->map_object_path_to_es);
  while (g_hash_table_iter_next (&hash_iter, (gpointer) &object_path, NULL))
    maybe_add_path (path, path_len, object_path, set);

  p = g_hash_table_steal_all_keys (set);
  g_hash_table_unref (set);

  g_ptr_array_add (p, NULL);
  ret = (gchar **) g_ptr_array_free (p, FALSE);
  return ret;
}

/* called in any thread with connection's lock not held */
static gchar **
g_dbus_connection_list_registered (GDBusConnection *connection,
                                   const gchar     *path)
{
  gchar **ret;
  CONNECTION_LOCK (connection);
  ret = g_dbus_connection_list_registered_unlocked (connection, path);
  CONNECTION_UNLOCK (connection);
  return ret;
}

/* called in GDBusWorker thread with connection's lock held */
static gboolean
handle_introspect (GDBusConnection *connection,
                   ExportedObject  *eo,
                   GDBusMessage    *message)
{
  guint n;
  GString *s;
  GDBusMessage *reply;
  GHashTableIter hash_iter;
  ExportedInterface *ei;
  gchar **registered;

  /* first the header with the standard interfaces */
  s = g_string_sized_new (sizeof (introspect_header) +
                          sizeof (introspect_properties_interface) +
                          sizeof (introspect_introspectable_interface) +
                          sizeof (introspect_tail));
  introspect_append_header (s);
  if (!g_hash_table_lookup (eo->map_if_name_to_ei,
                            DBUS_INTERFACE_PROPERTIES))
    g_string_append (s, introspect_properties_interface);

  if (!g_hash_table_lookup (eo->map_if_name_to_ei,
                            DBUS_INTERFACE_INTROSPECTABLE))
    g_string_append (s, introspect_introspectable_interface);

  /* then include the registered interfaces */
  g_hash_table_iter_init (&hash_iter, eo->map_if_name_to_ei);
  while (g_hash_table_iter_next (&hash_iter, NULL, (gpointer) &ei))
    g_dbus_interface_info_generate_xml (ei->interface_info, 2, s);

  /* finally include nodes registered below us */
  registered = g_dbus_connection_list_registered_unlocked (connection, eo->object_path);
  for (n = 0; registered != NULL && registered[n] != NULL; n++)
    g_string_append_printf (s, "  <node name=\"%s\"/>\n", registered[n]);
  g_strfreev (registered);
  g_string_append (s, introspect_tail);

  reply = g_dbus_message_new_method_reply (message);
  g_dbus_message_set_body (reply, g_variant_new ("(s)", s->str));
  g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);
  g_string_free (s, TRUE);

  return TRUE;
}

/* called in thread where object was registered - no locks held */
static gboolean
call_in_idle_cb (gpointer user_data)
{
  GDBusMethodInvocation *invocation = G_DBUS_METHOD_INVOCATION (user_data);
  GDBusInterfaceVTable *vtable;
  guint registration_id;
  guint subtree_registration_id;
  ExportedInterface *ei = NULL;
  ExportedSubtree *es = NULL;

  registration_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (invocation), "g-dbus-registration-id"));
  subtree_registration_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (invocation), "g-dbus-subtree-registration-id"));

  if (has_object_been_unregistered (g_dbus_method_invocation_get_connection (invocation),
                                    registration_id,
                                    &ei,
                                    subtree_registration_id,
                                    &es))
    {
      GDBusMessage *reply;
      reply = g_dbus_message_new_method_error (g_dbus_method_invocation_get_message (invocation),
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               _("No such interface “%s” on object at path %s"),
                                               g_dbus_method_invocation_get_interface_name (invocation),
                                               g_dbus_method_invocation_get_object_path (invocation));
      g_dbus_connection_send_message (g_dbus_method_invocation_get_connection (invocation), reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      goto out;
    }

  vtable = g_object_get_data (G_OBJECT (invocation), "g-dbus-interface-vtable");
  g_assert (vtable != NULL && vtable->method_call != NULL);

  vtable->method_call (g_dbus_method_invocation_get_connection (invocation),
                       g_dbus_method_invocation_get_sender (invocation),
                       g_dbus_method_invocation_get_object_path (invocation),
                       g_dbus_method_invocation_get_interface_name (invocation),
                       g_dbus_method_invocation_get_method_name (invocation),
                       g_dbus_method_invocation_get_parameters (invocation),
                       g_object_ref (invocation),
                       g_dbus_method_invocation_get_user_data (invocation));

 out:
  g_clear_pointer (&ei, exported_interface_unref);
  g_clear_pointer (&es, exported_subtree_unref);

  return FALSE;
}

/* called in GDBusWorker thread with connection's lock held */
static void
schedule_method_call (GDBusConnection            *connection,
                      GDBusMessage               *message,
                      guint                       registration_id,
                      guint                       subtree_registration_id,
                      const GDBusInterfaceInfo   *interface_info,
                      const GDBusMethodInfo      *method_info,
                      const GDBusPropertyInfo    *property_info,
                      GVariant                   *parameters,
                      const GDBusInterfaceVTable *vtable,
                      GMainContext               *main_context,
                      gpointer                    user_data)
{
  GDBusMethodInvocation *invocation;
  GSource *idle_source;

  invocation = _g_dbus_method_invocation_new (g_dbus_message_get_sender (message),
                                              g_dbus_message_get_path (message),
                                              g_dbus_message_get_interface (message),
                                              g_dbus_message_get_member (message),
                                              method_info,
                                              property_info,
                                              connection,
                                              message,
                                              parameters,
                                              user_data);

  /* TODO: would be nicer with a real MethodData like we already
   * have PropertyData and PropertyGetAllData... */
  g_object_set_data (G_OBJECT (invocation), "g-dbus-interface-vtable", (gpointer) vtable);
  g_object_set_data (G_OBJECT (invocation), "g-dbus-registration-id", GUINT_TO_POINTER (registration_id));
  g_object_set_data (G_OBJECT (invocation), "g-dbus-subtree-registration-id", GUINT_TO_POINTER (subtree_registration_id));

  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_DEFAULT);
  g_source_set_callback (idle_source,
                         call_in_idle_cb,
                         g_steal_pointer (&invocation),
                         g_object_unref);
  g_source_set_static_name (idle_source, "[gio, " __FILE__ "] call_in_idle_cb");
  g_source_attach (idle_source, main_context);
  g_source_unref (idle_source);
}

/* called in GDBusWorker thread with connection's lock held */
static gboolean
validate_and_maybe_schedule_method_call (GDBusConnection            *connection,
                                         GDBusMessage               *message,
                                         guint                       registration_id,
                                         guint                       subtree_registration_id,
                                         GDBusInterfaceInfo         *interface_info,
                                         const GDBusInterfaceVTable *vtable,
                                         GMainContext               *main_context,
                                         gpointer                    user_data)
{
  GDBusMethodInfo *method_info;
  GDBusMessage *reply;
  GVariant *parameters;
  gboolean handled;
  GVariantType *in_type;

  handled = FALSE;

  /* TODO: the cost of this is O(n) - it might be worth caching the result */
  method_info = g_dbus_interface_info_lookup_method (interface_info, g_dbus_message_get_member (message));

  /* if the method doesn't exist, return the DBUS_ERROR_UNKNOWN_METHOD
   * error to the caller
   */
  if (method_info == NULL)
    {
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               _("No such method “%s”"),
                                               g_dbus_message_get_member (message));
      g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
      handled = TRUE;
      goto out;
    }

  parameters = g_dbus_message_get_body (message);
  if (parameters == NULL)
    {
      parameters = g_variant_new ("()");
      g_variant_ref_sink (parameters);
    }
  else
    {
      g_variant_ref (parameters);
    }

  /* Check that the incoming args are of the right type - if they are not, return
   * the DBUS_ERROR_INVALID_ARGS error to the caller
   */
  in_type = _g_dbus_compute_complete_signature (method_info->in_args);
  if (!g_variant_is_of_type (parameters, in_type))
    {
      gchar *type_string;

      type_string = g_variant_type_dup_string (in_type);

      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_INVALID_ARGS,
                                               _("Type of message, “%s”, does not match expected type “%s”"),
                                               g_variant_get_type_string (parameters),
                                               type_string);
      g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_variant_type_free (in_type);
      g_variant_unref (parameters);
      g_object_unref (reply);
      g_free (type_string);
      handled = TRUE;
      goto out;
    }
  g_variant_type_free (in_type);

  /* schedule the call in idle */
  schedule_method_call (connection, message, registration_id, subtree_registration_id,
                        interface_info, method_info, NULL, parameters,
                        vtable, main_context, user_data);
  g_variant_unref (parameters);
  handled = TRUE;

 out:
  return handled;
}

/* ---------------------------------------------------------------------------------------------------- */

/* called in GDBusWorker thread with connection's lock held */
static gboolean
obj_message_func (GDBusConnection *connection,
                  ExportedObject  *eo,
                  GDBusMessage    *message,
                  gboolean        *object_found)
{
  const gchar *interface_name;
  const gchar *member;
  const gchar *signature;
  gboolean handled;

  handled = FALSE;

  interface_name = g_dbus_message_get_interface (message);
  member = g_dbus_message_get_member (message);
  signature = g_dbus_message_get_signature (message);

  /* see if we have an interface for handling this call */
  if (interface_name != NULL)
    {
      ExportedInterface *ei;
      ei = g_hash_table_lookup (eo->map_if_name_to_ei, interface_name);
      if (ei != NULL)
        {
          /* we do - invoke the handler in idle in the right thread */

          /* handle no vtable or handler being present */
          if (ei->vtable == NULL || ei->vtable->method_call == NULL)
            goto out;

          handled = validate_and_maybe_schedule_method_call (connection,
                                                             message,
                                                             ei->id,
                                                             0,
                                                             ei->interface_info,
                                                             ei->vtable,
                                                             ei->context,
                                                             ei->user_data);
          goto out;
        }
      else
        {
          *object_found = TRUE;
        }
    }

  if (g_strcmp0 (interface_name, DBUS_INTERFACE_INTROSPECTABLE) == 0 &&
      g_strcmp0 (member, "Introspect") == 0 &&
      g_strcmp0 (signature, "") == 0)
    {
      handled = handle_introspect (connection, eo, message);
      goto out;
    }
  else if (g_strcmp0 (interface_name, DBUS_INTERFACE_PROPERTIES) == 0 &&
           g_strcmp0 (member, "Get") == 0 &&
           g_strcmp0 (signature, "ss") == 0)
    {
      handled = handle_getset_property (connection, eo, message, TRUE);
      goto out;
    }
  else if (g_strcmp0 (interface_name, DBUS_INTERFACE_PROPERTIES) == 0 &&
           g_strcmp0 (member, "Set") == 0 &&
           g_strcmp0 (signature, "ssv") == 0)
    {
      handled = handle_getset_property (connection, eo, message, FALSE);
      goto out;
    }
  else if (g_strcmp0 (interface_name, DBUS_INTERFACE_PROPERTIES) == 0 &&
           g_strcmp0 (member, "GetAll") == 0 &&
           g_strcmp0 (signature, "s") == 0)
    {
      handled = handle_get_all_properties (connection, eo, message);
      goto out;
    }

 out:
  return handled;
}

/**
 * g_dbus_connection_register_object:
 * @connection: a #GDBusConnection
 * @object_path: the object path to register at
 * @interface_info: introspection data for the interface
 * @vtable: (nullable): a #GDBusInterfaceVTable to call into or %NULL
 * @user_data: (nullable): data to pass to functions in @vtable
 * @user_data_free_func: function to call when the object path is unregistered
 * @error: return location for error or %NULL
 *
 * Registers callbacks for exported objects at @object_path with the
 * D-Bus interface that is described in @interface_info.
 *
 * Calls to functions in @vtable (and @user_data_free_func) will happen
 * in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from.
 *
 * Note that all #GVariant values passed to functions in @vtable will match
 * the signature given in @interface_info - if a remote caller passes
 * incorrect values, the `org.freedesktop.DBus.Error.InvalidArgs`
 * is returned to the remote caller.
 *
 * Additionally, if the remote caller attempts to invoke methods or
 * access properties not mentioned in @interface_info the
 * `org.freedesktop.DBus.Error.UnknownMethod` resp.
 * `org.freedesktop.DBus.Error.InvalidArgs` errors
 * are returned to the caller.
 *
 * It is considered a programming error if the
 * #GDBusInterfaceGetPropertyFunc function in @vtable returns a
 * #GVariant of incorrect type.
 *
 * If an existing callback is already registered at @object_path and
 * @interface_name, then @error is set to %G_IO_ERROR_EXISTS.
 *
 * GDBus automatically implements the standard D-Bus interfaces
 * org.freedesktop.DBus.Properties, org.freedesktop.DBus.Introspectable
 * and org.freedesktop.Peer, so you don't have to implement those for the
 * objects you export. You can implement org.freedesktop.DBus.Properties
 * yourself, e.g. to handle getting and setting of properties asynchronously.
 *
 * Note that the reference count on @interface_info will be
 * incremented by 1 (unless allocated statically, e.g. if the
 * reference count is -1, see g_dbus_interface_info_ref()) for as long
 * as the object is exported. Also note that @vtable will be copied.
 *
 * See this [server][class@Gio.DBusConnection#an-example-d-bus-server]
 * for an example of how to use this method.
 *
 * Returns: 0 if @error is set, otherwise a registration id (never 0)
 *     that can be used with g_dbus_connection_unregister_object()
 *
 * Since: 2.26
 */
guint
g_dbus_connection_register_object (GDBusConnection             *connection,
                                   const gchar                 *object_path,
                                   GDBusInterfaceInfo          *interface_info,
                                   const GDBusInterfaceVTable  *vtable,
                                   gpointer                     user_data,
                                   GDestroyNotify               user_data_free_func,
                                   GError                     **error)
{
  ExportedObject *eo;
  ExportedInterface *ei;
  guint ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), 0);
  g_return_val_if_fail (object_path != NULL && g_variant_is_object_path (object_path), 0);
  g_return_val_if_fail (interface_info != NULL, 0);
  g_return_val_if_fail (g_dbus_is_interface_name (interface_info->name), 0);
  g_return_val_if_fail (error == NULL || *error == NULL, 0);
  g_return_val_if_fail (check_initialized (connection), 0);

  ret = 0;

  CONNECTION_LOCK (connection);

  eo = g_hash_table_lookup (connection->map_object_path_to_eo, object_path);
  if (eo == NULL)
    {
      eo = g_new0 (ExportedObject, 1);
      eo->object_path = g_strdup (object_path);
      eo->connection = connection;
      eo->map_if_name_to_ei = g_hash_table_new_full (g_str_hash,
                                                     g_str_equal,
                                                     NULL,
                                                     (GDestroyNotify) exported_interface_unref);
      g_hash_table_insert (connection->map_object_path_to_eo, eo->object_path, eo);
    }

  ei = g_hash_table_lookup (eo->map_if_name_to_ei, interface_info->name);
  if (ei != NULL)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_EXISTS,
                   _("An object is already exported for the interface %s at %s"),
                   interface_info->name,
                   object_path);
      goto out;
    }

  ei = g_new0 (ExportedInterface, 1);
  ei->refcount = 1;
  ei->id = (guint) g_atomic_int_add (&_global_registration_id, 1); /* TODO: overflow etc. */
  ei->eo = eo;
  ei->user_data = user_data;
  ei->user_data_free_func = user_data_free_func;
  ei->vtable = _g_dbus_interface_vtable_copy (vtable);
  ei->interface_info = g_dbus_interface_info_ref (interface_info);
  g_dbus_interface_info_cache_build (ei->interface_info);
  ei->interface_name = g_strdup (interface_info->name);
  ei->context = g_main_context_ref_thread_default ();

  g_hash_table_insert (eo->map_if_name_to_ei,
                       (gpointer) ei->interface_name,
                       ei);
  g_hash_table_insert (connection->map_id_to_ei,
                       GUINT_TO_POINTER (ei->id),
                       ei);

  ret = ei->id;

 out:
  CONNECTION_UNLOCK (connection);

  if (ret == 0 && user_data_free_func != NULL)
    user_data_free_func (user_data);

  return ret;
}

/**
 * g_dbus_connection_unregister_object:
 * @connection: a #GDBusConnection
 * @registration_id: a registration id obtained from
 *     g_dbus_connection_register_object()
 *
 * Unregisters an object.
 *
 * Returns: %TRUE if the object was unregistered, %FALSE otherwise
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_unregister_object (GDBusConnection *connection,
                                     guint            registration_id)
{
  ExportedInterface *ei;
  ExportedObject *eo;
  gboolean ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (check_initialized (connection), FALSE);

  ret = FALSE;

  CONNECTION_LOCK (connection);

  ei = g_hash_table_lookup (connection->map_id_to_ei,
                            GUINT_TO_POINTER (registration_id));
  if (ei == NULL)
    goto out;

  eo = ei->eo;

  g_warn_if_fail (g_hash_table_remove (connection->map_id_to_ei, GUINT_TO_POINTER (ei->id)));
  g_warn_if_fail (g_hash_table_remove (eo->map_if_name_to_ei, ei->interface_name));
  /* unregister object path if we have no more exported interfaces */
  if (g_hash_table_size (eo->map_if_name_to_ei) == 0)
    g_warn_if_fail (g_hash_table_remove (connection->map_object_path_to_eo,
                                         eo->object_path));

  ret = TRUE;

 out:
  CONNECTION_UNLOCK (connection);

  return ret;
}

typedef struct {
  GClosure *method_call_closure;
  GClosure *get_property_closure;
  GClosure *set_property_closure;
} RegisterObjectData;

static RegisterObjectData *
register_object_data_new (GClosure *method_call_closure,
                          GClosure *get_property_closure,
                          GClosure *set_property_closure)
{
  RegisterObjectData *data;

  data = g_new0 (RegisterObjectData, 1);

  if (method_call_closure != NULL)
    {
      data->method_call_closure = g_closure_ref (method_call_closure);
      g_closure_sink (method_call_closure);
      if (G_CLOSURE_NEEDS_MARSHAL (method_call_closure))
        g_closure_set_marshal (method_call_closure, g_cclosure_marshal_generic);
    }

  if (get_property_closure != NULL)
    {
      data->get_property_closure = g_closure_ref (get_property_closure);
      g_closure_sink (get_property_closure);
      if (G_CLOSURE_NEEDS_MARSHAL (get_property_closure))
        g_closure_set_marshal (get_property_closure, g_cclosure_marshal_generic);
    }

  if (set_property_closure != NULL)
    {
      data->set_property_closure = g_closure_ref (set_property_closure);
      g_closure_sink (set_property_closure);
      if (G_CLOSURE_NEEDS_MARSHAL (set_property_closure))
        g_closure_set_marshal (set_property_closure, g_cclosure_marshal_generic);
    }

  return data;
}

static void
register_object_free_func (gpointer user_data)
{
  RegisterObjectData *data = user_data;

  g_clear_pointer (&data->method_call_closure, g_closure_unref);
  g_clear_pointer (&data->get_property_closure, g_closure_unref);
  g_clear_pointer (&data->set_property_closure, g_closure_unref);

  g_free (data);
}

static void
register_with_closures_on_method_call (GDBusConnection       *connection,
                                       const gchar           *sender,
                                       const gchar           *object_path,
                                       const gchar           *interface_name,
                                       const gchar           *method_name,
                                       GVariant              *parameters,
                                       GDBusMethodInvocation *invocation,
                                       gpointer               user_data)
{
  RegisterObjectData *data = user_data;
  GValue params[] = { G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT };

  g_value_init (&params[0], G_TYPE_DBUS_CONNECTION);
  g_value_set_object (&params[0], connection);

  g_value_init (&params[1], G_TYPE_STRING);
  g_value_set_string (&params[1], sender);

  g_value_init (&params[2], G_TYPE_STRING);
  g_value_set_string (&params[2], object_path);

  g_value_init (&params[3], G_TYPE_STRING);
  g_value_set_string (&params[3], interface_name);

  g_value_init (&params[4], G_TYPE_STRING);
  g_value_set_string (&params[4], method_name);

  g_value_init (&params[5], G_TYPE_VARIANT);
  g_value_set_variant (&params[5], parameters);

  g_value_init (&params[6], G_TYPE_DBUS_METHOD_INVOCATION);
  /* NOTE: This is deliberately *not* g_value_take_object(). A reference to
   * `invocation` is transferred in to this function, and it needs to be
   * transferred onwards to the `g_dbus_method_invocation_return_*()` method
   * call which must eventually happen (either in the closure function, or in
   * a delayed consequence from it). Changing this will break API. */
  g_value_set_object (&params[6], invocation);

  g_closure_invoke (data->method_call_closure, NULL, G_N_ELEMENTS (params), params, NULL);

  g_value_unset (params + 0);
  g_value_unset (params + 1);
  g_value_unset (params + 2);
  g_value_unset (params + 3);
  g_value_unset (params + 4);
  g_value_unset (params + 5);
  g_value_unset (params + 6);
}

static GVariant *
register_with_closures_on_get_property (GDBusConnection *connection,
                                        const gchar     *sender,
                                        const gchar     *object_path,
                                        const gchar     *interface_name,
                                        const gchar     *property_name,
                                        GError         **error,
                                        gpointer         user_data)
{
  RegisterObjectData *data = user_data;
  GValue params[] = { G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT };
  GValue result_value = G_VALUE_INIT;
  GVariant *result;

  g_value_init (&params[0], G_TYPE_DBUS_CONNECTION);
  g_value_set_object (&params[0], connection);

  g_value_init (&params[1], G_TYPE_STRING);
  g_value_set_string (&params[1], sender);

  g_value_init (&params[2], G_TYPE_STRING);
  g_value_set_string (&params[2], object_path);

  g_value_init (&params[3], G_TYPE_STRING);
  g_value_set_string (&params[3], interface_name);

  g_value_init (&params[4], G_TYPE_STRING);
  g_value_set_string (&params[4], property_name);

  g_value_init (&result_value, G_TYPE_VARIANT);

  g_closure_invoke (data->get_property_closure, &result_value, G_N_ELEMENTS (params), params, NULL);

  result = g_value_get_variant (&result_value);
  if (result)
    g_variant_ref (result);

  g_value_unset (params + 0);
  g_value_unset (params + 1);
  g_value_unset (params + 2);
  g_value_unset (params + 3);
  g_value_unset (params + 4);
  g_value_unset (&result_value);

  if (!result)
    g_set_error (error, G_DBUS_ERROR, G_DBUS_ERROR_FAILED,
                 _("Unable to retrieve property %s.%s"),
                 interface_name, property_name);

  return result;
}

static gboolean
register_with_closures_on_set_property (GDBusConnection *connection,
                                        const gchar     *sender,
                                        const gchar     *object_path,
                                        const gchar     *interface_name,
                                        const gchar     *property_name,
                                        GVariant        *value,
                                        GError         **error,
                                        gpointer         user_data)
{
  RegisterObjectData *data = user_data;
  GValue params[] = { G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT };
  GValue result_value = G_VALUE_INIT;
  gboolean result;

  g_value_init (&params[0], G_TYPE_DBUS_CONNECTION);
  g_value_set_object (&params[0], connection);

  g_value_init (&params[1], G_TYPE_STRING);
  g_value_set_string (&params[1], sender);

  g_value_init (&params[2], G_TYPE_STRING);
  g_value_set_string (&params[2], object_path);

  g_value_init (&params[3], G_TYPE_STRING);
  g_value_set_string (&params[3], interface_name);

  g_value_init (&params[4], G_TYPE_STRING);
  g_value_set_string (&params[4], property_name);

  g_value_init (&params[5], G_TYPE_VARIANT);
  g_value_set_variant (&params[5], value);

  g_value_init (&result_value, G_TYPE_BOOLEAN);

  g_closure_invoke (data->set_property_closure, &result_value, G_N_ELEMENTS (params), params, NULL);

  result = g_value_get_boolean (&result_value);

  g_value_unset (params + 0);
  g_value_unset (params + 1);
  g_value_unset (params + 2);
  g_value_unset (params + 3);
  g_value_unset (params + 4);
  g_value_unset (params + 5);
  g_value_unset (&result_value);

  if (!result)
    g_set_error (error,
                 G_DBUS_ERROR, G_DBUS_ERROR_FAILED,
                 _("Unable to set property %s.%s"),
                 interface_name, property_name);

  return result;
}

/**
 * g_dbus_connection_register_object_with_closures: (rename-to g_dbus_connection_register_object)
 * @connection: A #GDBusConnection.
 * @object_path: The object path to register at.
 * @interface_info: Introspection data for the interface.
 * @method_call_closure: (nullable): #GClosure for handling incoming method calls.
 * @get_property_closure: (nullable): #GClosure for getting a property.
 * @set_property_closure: (nullable): #GClosure for setting a property.
 * @error: Return location for error or %NULL.
 *
 * Version of g_dbus_connection_register_object() using closures instead of a
 * #GDBusInterfaceVTable for easier binding in other languages.
 *
 * Note that the reference counting semantics of the function wrapped by
 * @method_call_closure are the same as those of
 * [callback@Gio.DBusInterfaceMethodCallFunc]: ownership of a reference to the
 * [class@Gio.DBusMethodInvocation] is transferred to the function.
 *
 * Returns: 0 if @error is set, otherwise a registration ID (never 0)
 * that can be used with g_dbus_connection_unregister_object() .
 *
 * Since: 2.46
 * Deprecated: 2.84: Deprecated in favour of
 *    [method@Gio.DBusConnection.register_object_with_closures2], which has more
 *    binding-friendly reference counting semantics.
 */
guint
g_dbus_connection_register_object_with_closures (GDBusConnection     *connection,
                                                 const gchar         *object_path,
                                                 GDBusInterfaceInfo  *interface_info,
                                                 GClosure            *method_call_closure,
                                                 GClosure            *get_property_closure,
                                                 GClosure            *set_property_closure,
                                                 GError             **error)
{
  RegisterObjectData *data;
  GDBusInterfaceVTable vtable =
    {
      method_call_closure != NULL  ? register_with_closures_on_method_call  : NULL,
      get_property_closure != NULL ? register_with_closures_on_get_property : NULL,
      set_property_closure != NULL ? register_with_closures_on_set_property : NULL,
      { 0 }
    };

  data = register_object_data_new (method_call_closure, get_property_closure, set_property_closure);

  return g_dbus_connection_register_object (connection,
                                            object_path,
                                            interface_info,
                                            &vtable,
                                            g_steal_pointer (&data),
                                            register_object_free_func,
                                            error);
}

static void
register_with_closures_on_method_call2 (GDBusConnection       *connection,
                                        const gchar           *sender,
                                        const gchar           *object_path,
                                        const gchar           *interface_name,
                                        const gchar           *method_name,
                                        GVariant              *parameters,
                                        GDBusMethodInvocation *invocation,
                                        gpointer               user_data)
{
  RegisterObjectData *data = user_data;
  GValue params[] = { G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT, G_VALUE_INIT };

  g_value_init (&params[0], G_TYPE_DBUS_CONNECTION);
  g_value_set_object (&params[0], connection);

  g_value_init (&params[1], G_TYPE_STRING);
  g_value_set_string (&params[1], sender);

  g_value_init (&params[2], G_TYPE_STRING);
  g_value_set_string (&params[2], object_path);

  g_value_init (&params[3], G_TYPE_STRING);
  g_value_set_string (&params[3], interface_name);

  g_value_init (&params[4], G_TYPE_STRING);
  g_value_set_string (&params[4], method_name);

  g_value_init (&params[5], G_TYPE_VARIANT);
  g_value_set_variant (&params[5], parameters);

  g_value_init (&params[6], G_TYPE_DBUS_METHOD_INVOCATION);
  /* NOTE: This is deliberately *not* g_value_set_object(), in contrast with the
   * deprecated implementation in register_with_closures_on_method_call().
   *
   * A reference to `invocation` is transferred in to this function, but
   * bindings don’t expect a `GClosure` to provide any (transfer full)
   * arguments, so consume the reference here. Bindings need to add an
   * additional reference to the `GDBusMethodInvocation` before calling any
   * `g_dbus_method_invocation_return_*()` methods on it. They can do this
   * automatically based on the introspection annotations for those methods. */
  g_value_take_object (&params[6], invocation);

  g_closure_invoke (data->method_call_closure, NULL, G_N_ELEMENTS (params), params, NULL);

  g_value_unset (params + 0);
  g_value_unset (params + 1);
  g_value_unset (params + 2);
  g_value_unset (params + 3);
  g_value_unset (params + 4);
  g_value_unset (params + 5);
  g_value_unset (params + 6);
}

/**
 * g_dbus_connection_register_object_with_closures2:
 * @connection: A [class@Gio.DBusConnection].
 * @object_path: The object path to register at.
 * @interface_info: Introspection data for the interface.
 * @method_call_closure: (nullable): [type@GObject.Closure] for handling incoming method calls.
 * @get_property_closure: (nullable): [type@GObject.Closure] for getting a property.
 * @set_property_closure: (nullable): [type@GObject.Closure] for setting a property.
 * @error: Return location for error or `NULL`.
 *
 * Version of [method@Gio.DBusConnection.register_object] using closures instead
 * of a [type@Gio.DBusInterfaceVTable] for easier binding in other languages.
 *
 * In contrast to [method@Gio.DBusConnection.register_object] and
 * [method@Gio.DBusConnection.register_object_with_closures], the reference
 * counting semantics of the function wrapped by @method_call_closure are *not*
 * the same as those of [callback@Gio.DBusInterfaceMethodCallFunc]. Ownership of
 * a reference to the [class@Gio.DBusMethodInvocation] is *not* transferred to
 * the function. Bindings must ensure that they add a reference to the
 * [class@Gio.DBusMethodInvocation] before calling any
 * `g_dbus_method_invocation_return_*()` methods on it. This should be automatic
 * as a result of the introspection annotations on those methods.
 *
 * Returns: `0` if @error is set, otherwise a registration ID (never `0`)
 * that can be used with [method@Gio.DBusConnection.unregister_object].
 *
 * Since: 2.84
 */
guint
g_dbus_connection_register_object_with_closures2 (GDBusConnection     *connection,
                                                  const gchar         *object_path,
                                                  GDBusInterfaceInfo  *interface_info,
                                                  GClosure            *method_call_closure,
                                                  GClosure            *get_property_closure,
                                                  GClosure            *set_property_closure,
                                                  GError             **error)
{
  RegisterObjectData *data;
  GDBusInterfaceVTable vtable =
    {
      method_call_closure != NULL  ? register_with_closures_on_method_call2  : NULL,
      get_property_closure != NULL ? register_with_closures_on_get_property : NULL,
      set_property_closure != NULL ? register_with_closures_on_set_property : NULL,
      { 0 }
    };

  data = register_object_data_new (method_call_closure, get_property_closure, set_property_closure);

  return g_dbus_connection_register_object (connection,
                                            object_path,
                                            interface_info,
                                            &vtable,
                                            g_steal_pointer (&data),
                                            register_object_free_func,
                                            error);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_emit_signal:
 * @connection: a #GDBusConnection
 * @destination_bus_name: (nullable): the unique bus name for the destination
 *     for the signal or %NULL to emit to all listeners
 * @object_path: path of remote object
 * @interface_name: D-Bus interface to emit a signal on
 * @signal_name: the name of the signal to emit
 * @parameters: (nullable): a #GVariant tuple with parameters for the signal
 *              or %NULL if not passing parameters
 * @error: Return location for error or %NULL
 *
 * Emits a signal.
 *
 * If the parameters GVariant is floating, it is consumed.
 *
 * This can only fail if @parameters is not compatible with the D-Bus protocol
 * (%G_IO_ERROR_INVALID_ARGUMENT), or if @connection has been closed
 * (%G_IO_ERROR_CLOSED).
 *
 * Returns: %TRUE unless @error is set
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_emit_signal (GDBusConnection  *connection,
                               const gchar      *destination_bus_name,
                               const gchar      *object_path,
                               const gchar      *interface_name,
                               const gchar      *signal_name,
                               GVariant         *parameters,
                               GError          **error)
{
  GDBusMessage *message;
  gboolean ret;

  message = NULL;
  ret = FALSE;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (destination_bus_name == NULL || g_dbus_is_name (destination_bus_name), FALSE);
  g_return_val_if_fail (object_path != NULL && g_variant_is_object_path (object_path), FALSE);
  g_return_val_if_fail (interface_name != NULL && g_dbus_is_interface_name (interface_name), FALSE);
  g_return_val_if_fail (signal_name != NULL && g_dbus_is_member_name (signal_name), FALSE);
  g_return_val_if_fail (parameters == NULL || g_variant_is_of_type (parameters, G_VARIANT_TYPE_TUPLE), FALSE);
  g_return_val_if_fail (check_initialized (connection), FALSE);

  if (G_UNLIKELY (_g_dbus_debug_emission ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Emission:\n"
               " >>>> SIGNAL EMISSION %s.%s()\n"
               "      on object %s\n"
               "      destination %s\n",
               interface_name, signal_name,
               object_path,
               destination_bus_name != NULL ? destination_bus_name : "(none)");
      _g_dbus_debug_print_unlock ();
    }

  message = g_dbus_message_new_signal (object_path,
                                       interface_name,
                                       signal_name);

  if (destination_bus_name != NULL)
    g_dbus_message_set_header (message,
                               G_DBUS_MESSAGE_HEADER_FIELD_DESTINATION,
                               g_variant_new_string (destination_bus_name));

  if (parameters != NULL)
    g_dbus_message_set_body (message, parameters);

  ret = g_dbus_connection_send_message (connection, message, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, error);
  g_object_unref (message);

  return ret;
}

static void
add_call_flags (GDBusMessage           *message,
                         GDBusCallFlags  flags)
{
  GDBusMessageFlags msg_flags = 0;

  if (flags & G_DBUS_CALL_FLAGS_NO_AUTO_START)
    msg_flags |= G_DBUS_MESSAGE_FLAGS_NO_AUTO_START;
  if (flags & G_DBUS_CALL_FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION)
    msg_flags |= G_DBUS_MESSAGE_FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION;
  if (msg_flags)
    g_dbus_message_set_flags (message, msg_flags);
}

static GVariant *
decode_method_reply (GDBusMessage        *reply,
                     const gchar         *method_name,
                     const GVariantType  *reply_type,
                     GUnixFDList        **out_fd_list,
                     GError             **error)
{
  GVariant *result;

  result = NULL;
  switch (g_dbus_message_get_message_type (reply))
    {
    case G_DBUS_MESSAGE_TYPE_METHOD_RETURN:
      result = g_dbus_message_get_body (reply);
      if (result == NULL)
        {
          result = g_variant_new ("()");
          g_variant_ref_sink (result);
        }
      else
        {
          g_variant_ref (result);
        }

      if (!g_variant_is_of_type (result, reply_type))
        {
          gchar *type_string = g_variant_type_dup_string (reply_type);

          g_set_error (error,
                       G_IO_ERROR,
                       G_IO_ERROR_INVALID_ARGUMENT,
                       _("Method “%s” returned type “%s”, but expected “%s”"),
                       method_name, g_variant_get_type_string (result), type_string);

          g_variant_unref (result);
          g_free (type_string);
          result = NULL;
        }

#ifdef G_OS_UNIX
      if (result != NULL)
        {
          if (out_fd_list != NULL)
            {
              *out_fd_list = g_dbus_message_get_unix_fd_list (reply);
              if (*out_fd_list != NULL)
                g_object_ref (*out_fd_list);
            }
        }
#endif
      break;

    case G_DBUS_MESSAGE_TYPE_ERROR:
      g_dbus_message_to_gerror (reply, error);
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  return result;
}


typedef struct
{
  GVariantType *reply_type;
  gchar *method_name; /* for error message */

  GUnixFDList *fd_list;
} CallState;

static void
call_state_free (CallState *state)
{
  g_variant_type_free (state->reply_type);
  g_free (state->method_name);

  if (state->fd_list != NULL)
    g_object_unref (state->fd_list);
  g_slice_free (CallState, state);
}

/* called in any thread, with the connection's lock not held */
static void
g_dbus_connection_call_done (GObject      *source,
                             GAsyncResult *result,
                             gpointer      user_data)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (source);
  GTask *task = user_data;
  CallState *state = g_task_get_task_data (task);
  GError *error = NULL;
  GDBusMessage *reply;
  GVariant *value = NULL;

  reply = g_dbus_connection_send_message_with_reply_finish (connection,
                                                            result,
                                                            &error);

  if (G_UNLIKELY (_g_dbus_debug_call ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Call:\n"
               " <<<< ASYNC COMPLETE %s()",
               state->method_name);

      if (reply != NULL)
        {
          g_print (" (serial %d)\n"
                   "      SUCCESS\n",
                   g_dbus_message_get_reply_serial (reply));
        }
      else
        {
          g_print ("\n"
                   "      FAILED: %s\n",
                   error->message);
        }
      _g_dbus_debug_print_unlock ();
    }

  if (reply != NULL)
    value = decode_method_reply (reply, state->method_name, state->reply_type, &state->fd_list, &error);

  if (error != NULL)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, value, (GDestroyNotify) g_variant_unref);

  g_clear_object (&reply);
  g_object_unref (task);
}

/* called in any thread, with the connection's lock not held */
static void
g_dbus_connection_call_internal (GDBusConnection        *connection,
                                 const gchar            *bus_name,
                                 const gchar            *object_path,
                                 const gchar            *interface_name,
                                 const gchar            *method_name,
                                 GVariant               *parameters,
                                 const GVariantType     *reply_type,
                                 GDBusCallFlags          flags,
                                 gint                    timeout_msec,
                                 GUnixFDList            *fd_list,
                                 GCancellable           *cancellable,
                                 GAsyncReadyCallback     callback,
                                 gpointer                user_data)
{
  GDBusMessage *message;
  guint32 serial;

  g_return_if_fail (G_IS_DBUS_CONNECTION (connection));
  g_return_if_fail (bus_name == NULL || g_dbus_is_name (bus_name));
  g_return_if_fail (object_path != NULL && g_variant_is_object_path (object_path));
  g_return_if_fail (interface_name != NULL && g_dbus_is_interface_name (interface_name));
  g_return_if_fail (method_name != NULL && g_dbus_is_member_name (method_name));
  g_return_if_fail (timeout_msec >= 0 || timeout_msec == -1);
  g_return_if_fail ((parameters == NULL) || g_variant_is_of_type (parameters, G_VARIANT_TYPE_TUPLE));
  g_return_if_fail (check_initialized (connection));
#ifdef G_OS_UNIX
  g_return_if_fail (fd_list == NULL || G_IS_UNIX_FD_LIST (fd_list));
#else
  g_return_if_fail (fd_list == NULL);
#endif

  message = g_dbus_message_new_method_call (bus_name,
                                            object_path,
                                            interface_name,
                                            method_name);
  add_call_flags (message, flags);
  if (parameters != NULL)
    g_dbus_message_set_body (message, parameters);

#ifdef G_OS_UNIX
  if (fd_list != NULL)
    g_dbus_message_set_unix_fd_list (message, fd_list);
#endif

  /* If the user has no callback then we can just send the message with
   * the G_DBUS_MESSAGE_FLAGS_NO_REPLY_EXPECTED flag set and skip all
   * the logic for processing the reply.  If the service sends the reply
   * anyway then it will just be ignored.
   */
  if (callback != NULL)
    {
      CallState *state;
      GTask *task;

      state = g_slice_new0 (CallState);
      state->method_name = g_strjoin (".", interface_name, method_name, NULL);

      if (reply_type == NULL)
        reply_type = G_VARIANT_TYPE_ANY;

      state->reply_type = g_variant_type_copy (reply_type);

      task = g_task_new (connection, cancellable, callback, user_data);
      g_task_set_source_tag (task, g_dbus_connection_call_internal);
      g_task_set_task_data (task, state, (GDestroyNotify) call_state_free);

      g_dbus_connection_send_message_with_reply (connection,
                                                 message,
                                                 G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                 timeout_msec,
                                                 &serial,
                                                 cancellable,
                                                 g_dbus_connection_call_done,
                                                 task);
    }
  else
    {
      GDBusMessageFlags msg_flags;

      msg_flags = g_dbus_message_get_flags (message);
      msg_flags |= G_DBUS_MESSAGE_FLAGS_NO_REPLY_EXPECTED;
      g_dbus_message_set_flags (message, msg_flags);

      g_dbus_connection_send_message (connection,
                                      message,
                                      G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                      &serial, NULL);
    }

  if (G_UNLIKELY (_g_dbus_debug_call ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Call:\n"
               " >>>> ASYNC %s.%s()\n"
               "      on object %s\n"
               "      owned by name %s (serial %d)\n",
               interface_name,
               method_name,
               object_path,
               bus_name != NULL ? bus_name : "(none)",
               serial);
      _g_dbus_debug_print_unlock ();
    }

  if (message != NULL)
    g_object_unref (message);
}

/* called in any thread, with the connection's lock not held */
static GVariant *
g_dbus_connection_call_finish_internal (GDBusConnection  *connection,
                                        GUnixFDList     **out_fd_list,
                                        GAsyncResult     *res,
                                        GError          **error)
{
  GTask *task;
  CallState *state;
  GVariant *ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  g_return_val_if_fail (g_task_is_valid (res, connection), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  task = G_TASK (res);
  state = g_task_get_task_data (task);

  ret = g_task_propagate_pointer (task, error);
  if (!ret)
    return NULL;

  if (out_fd_list != NULL)
    *out_fd_list = state->fd_list != NULL ? g_object_ref (state->fd_list) : NULL;
  return ret;
}

/* called in any user thread, with the connection's lock not held */
static GVariant *
g_dbus_connection_call_sync_internal (GDBusConnection         *connection,
                                      const gchar             *bus_name,
                                      const gchar             *object_path,
                                      const gchar             *interface_name,
                                      const gchar             *method_name,
                                      GVariant                *parameters,
                                      const GVariantType      *reply_type,
                                      GDBusCallFlags           flags,
                                      gint                     timeout_msec,
                                      GUnixFDList             *fd_list,
                                      GUnixFDList            **out_fd_list,
                                      GCancellable            *cancellable,
                                      GError                 **error)
{
  GDBusMessage *message;
  GDBusMessage *reply;
  GVariant *result;
  GError *local_error;
  GDBusSendMessageFlags send_flags;

  message = NULL;
  reply = NULL;
  result = NULL;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  g_return_val_if_fail (bus_name == NULL || g_dbus_is_name (bus_name), NULL);
  g_return_val_if_fail (object_path != NULL && g_variant_is_object_path (object_path), NULL);
  g_return_val_if_fail (interface_name != NULL && g_dbus_is_interface_name (interface_name), NULL);
  g_return_val_if_fail (method_name != NULL && g_dbus_is_member_name (method_name), NULL);
  g_return_val_if_fail (timeout_msec >= 0 || timeout_msec == -1, NULL);
  g_return_val_if_fail ((parameters == NULL) || g_variant_is_of_type (parameters, G_VARIANT_TYPE_TUPLE), NULL);
#ifdef G_OS_UNIX
  g_return_val_if_fail (fd_list == NULL || G_IS_UNIX_FD_LIST (fd_list), NULL);
#else
  g_return_val_if_fail (fd_list == NULL, NULL);
#endif
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!(flags & CALL_FLAGS_INITIALIZING))
    g_return_val_if_fail (check_initialized (connection), FALSE);

  if (reply_type == NULL)
    reply_type = G_VARIANT_TYPE_ANY;

  message = g_dbus_message_new_method_call (bus_name,
                                            object_path,
                                            interface_name,
                                            method_name);
  add_call_flags (message, flags);
  if (parameters != NULL)
    g_dbus_message_set_body (message, parameters);

#ifdef G_OS_UNIX
  if (fd_list != NULL)
    g_dbus_message_set_unix_fd_list (message, fd_list);
#endif

  if (G_UNLIKELY (_g_dbus_debug_call ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Call:\n"
               " >>>> SYNC %s.%s()\n"
               "      on object %s\n"
               "      owned by name %s\n",
               interface_name,
               method_name,
               object_path,
               bus_name != NULL ? bus_name : "(none)");
      _g_dbus_debug_print_unlock ();
    }

  local_error = NULL;

  send_flags = G_DBUS_SEND_MESSAGE_FLAGS_NONE;

  /* translate from one flavour of flags to another... */
  if (flags & CALL_FLAGS_INITIALIZING)
    send_flags |= SEND_MESSAGE_FLAGS_INITIALIZING;

  reply = g_dbus_connection_send_message_with_reply_sync (connection,
                                                          message,
                                                          send_flags,
                                                          timeout_msec,
                                                          NULL, /* guint32 *out_serial */
                                                          cancellable,
                                                          &local_error);

  if (G_UNLIKELY (_g_dbus_debug_call ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Call:\n"
               " <<<< SYNC COMPLETE %s.%s()\n"
               "      ",
               interface_name,
               method_name);
      if (reply != NULL)
        {
          g_print ("SUCCESS\n");
        }
      else
        {
          g_print ("FAILED: %s\n",
                   local_error->message);
        }
      _g_dbus_debug_print_unlock ();
    }

  if (reply == NULL)
    {
      if (error != NULL)
        *error = local_error;
      else
        g_error_free (local_error);
      goto out;
    }

  result = decode_method_reply (reply, method_name, reply_type, out_fd_list, error);

 out:
  if (message != NULL)
    g_object_unref (message);
  if (reply != NULL)
    g_object_unref (reply);

  return result;
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_call:
 * @connection: a #GDBusConnection
 * @bus_name: (nullable): a unique or well-known bus name or %NULL if
 *     @connection is not a message bus connection
 * @object_path: path of remote object
 * @interface_name: D-Bus interface to invoke method on
 * @method_name: the name of the method to invoke
 * @parameters: (nullable): a #GVariant tuple with parameters for the method
 *     or %NULL if not passing parameters
 * @reply_type: (nullable): the expected type of the reply (which will be a
 *     tuple), or %NULL
 * @flags: flags from the #GDBusCallFlags enumeration
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: (nullable): a #GAsyncReadyCallback to call when the request
 *     is satisfied or %NULL if you don't care about the result of the
 *     method invocation
 * @user_data: the data to pass to @callback
 *
 * Asynchronously invokes the @method_name method on the
 * @interface_name D-Bus interface on the remote object at
 * @object_path owned by @bus_name.
 *
 * If @connection is closed then the operation will fail with
 * %G_IO_ERROR_CLOSED. If @cancellable is canceled, the operation will
 * fail with %G_IO_ERROR_CANCELLED. If @parameters contains a value
 * not compatible with the D-Bus protocol, the operation fails with
 * %G_IO_ERROR_INVALID_ARGUMENT.
 *
 * If @reply_type is non-%NULL then the reply will be checked for having this type and an
 * error will be raised if it does not match.  Said another way, if you give a @reply_type
 * then any non-%NULL return value will be of this type. Unless it’s
 * %G_VARIANT_TYPE_UNIT, the @reply_type will be a tuple containing one or more
 * values.
 *
 * If the @parameters #GVariant is floating, it is consumed. This allows
 * convenient 'inline' use of g_variant_new(), e.g.:
 * |[<!-- language="C" -->
 *  g_dbus_connection_call (connection,
 *                          "org.freedesktop.StringThings",
 *                          "/org/freedesktop/StringThings",
 *                          "org.freedesktop.StringThings",
 *                          "TwoStrings",
 *                          g_variant_new ("(ss)",
 *                                         "Thing One",
 *                                         "Thing Two"),
 *                          NULL,
 *                          G_DBUS_CALL_FLAGS_NONE,
 *                          -1,
 *                          NULL,
 *                          (GAsyncReadyCallback) two_strings_done,
 *                          NULL);
 * ]|
 *
 * This is an asynchronous method. When the operation is finished,
 * @callback will be invoked in the thread-default main context
 * (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from. You can then call
 * g_dbus_connection_call_finish() to get the result of the operation.
 * See g_dbus_connection_call_sync() for the synchronous version of this
 * function.
 *
 * If @callback is %NULL then the D-Bus method call message will be sent with
 * the %G_DBUS_MESSAGE_FLAGS_NO_REPLY_EXPECTED flag set.
 *
 * Since: 2.26
 */
void
g_dbus_connection_call (GDBusConnection     *connection,
                        const gchar         *bus_name,
                        const gchar         *object_path,
                        const gchar         *interface_name,
                        const gchar         *method_name,
                        GVariant            *parameters,
                        const GVariantType  *reply_type,
                        GDBusCallFlags       flags,
                        gint                 timeout_msec,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  g_dbus_connection_call_internal (connection, bus_name, object_path, interface_name, method_name, parameters, reply_type, flags, timeout_msec, NULL, cancellable, callback, user_data);
}

/**
 * g_dbus_connection_call_finish:
 * @connection: a #GDBusConnection
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed to g_dbus_connection_call()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_call().
 *
 * Returns: (transfer full): %NULL if @error is set. Otherwise a non-floating
 *     #GVariant tuple with return values. Free with g_variant_unref().
 *
 * Since: 2.26
 */
GVariant *
g_dbus_connection_call_finish (GDBusConnection  *connection,
                               GAsyncResult     *res,
                               GError          **error)
{
  return g_dbus_connection_call_finish_internal (connection, NULL, res, error);
}

/**
 * g_dbus_connection_call_sync:
 * @connection: a #GDBusConnection
 * @bus_name: (nullable): a unique or well-known bus name or %NULL if
 *     @connection is not a message bus connection
 * @object_path: path of remote object
 * @interface_name: D-Bus interface to invoke method on
 * @method_name: the name of the method to invoke
 * @parameters: (nullable): a #GVariant tuple with parameters for the method
 *     or %NULL if not passing parameters
 * @reply_type: (nullable): the expected type of the reply, or %NULL
 * @flags: flags from the #GDBusCallFlags enumeration
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously invokes the @method_name method on the
 * @interface_name D-Bus interface on the remote object at
 * @object_path owned by @bus_name.
 *
 * If @connection is closed then the operation will fail with
 * %G_IO_ERROR_CLOSED. If @cancellable is canceled, the
 * operation will fail with %G_IO_ERROR_CANCELLED. If @parameters
 * contains a value not compatible with the D-Bus protocol, the operation
 * fails with %G_IO_ERROR_INVALID_ARGUMENT.
 *
 * If @reply_type is non-%NULL then the reply will be checked for having
 * this type and an error will be raised if it does not match.  Said
 * another way, if you give a @reply_type then any non-%NULL return
 * value will be of this type.
 *
 * If the @parameters #GVariant is floating, it is consumed.
 * This allows convenient 'inline' use of g_variant_new(), e.g.:
 * |[<!-- language="C" -->
 *  g_dbus_connection_call_sync (connection,
 *                               "org.freedesktop.StringThings",
 *                               "/org/freedesktop/StringThings",
 *                               "org.freedesktop.StringThings",
 *                               "TwoStrings",
 *                               g_variant_new ("(ss)",
 *                                              "Thing One",
 *                                              "Thing Two"),
 *                               NULL,
 *                               G_DBUS_CALL_FLAGS_NONE,
 *                               -1,
 *                               NULL,
 *                               &error);
 * ]|
 *
 * The calling thread is blocked until a reply is received. See
 * g_dbus_connection_call() for the asynchronous version of
 * this method.
 *
 * Returns: (transfer full): %NULL if @error is set. Otherwise a non-floating
 *     #GVariant tuple with return values. Free with g_variant_unref().
 *
 * Since: 2.26
 */
GVariant *
g_dbus_connection_call_sync (GDBusConnection     *connection,
                             const gchar         *bus_name,
                             const gchar         *object_path,
                             const gchar         *interface_name,
                             const gchar         *method_name,
                             GVariant            *parameters,
                             const GVariantType  *reply_type,
                             GDBusCallFlags       flags,
                             gint                 timeout_msec,
                             GCancellable        *cancellable,
                             GError             **error)
{
  return g_dbus_connection_call_sync_internal (connection, bus_name, object_path, interface_name, method_name, parameters, reply_type, flags, timeout_msec, NULL, NULL, cancellable, error);
}

/* ---------------------------------------------------------------------------------------------------- */

#ifdef G_OS_UNIX

/**
 * g_dbus_connection_call_with_unix_fd_list:
 * @connection: a #GDBusConnection
 * @bus_name: (nullable): a unique or well-known bus name or %NULL if
 *     @connection is not a message bus connection
 * @object_path: path of remote object
 * @interface_name: D-Bus interface to invoke method on
 * @method_name: the name of the method to invoke
 * @parameters: (nullable): a #GVariant tuple with parameters for the method
 *     or %NULL if not passing parameters
 * @reply_type: (nullable): the expected type of the reply, or %NULL
 * @flags: flags from the #GDBusCallFlags enumeration
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @fd_list: (nullable): a #GUnixFDList or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: (nullable): a #GAsyncReadyCallback to call when the request is
 *     satisfied or %NULL if you don't * care about the result of the
 *     method invocation
 * @user_data: The data to pass to @callback.
 *
 * Like g_dbus_connection_call() but also takes a #GUnixFDList object.
 *
 * The file descriptors normally correspond to %G_VARIANT_TYPE_HANDLE
 * values in the body of the message. For example, if a message contains
 * two file descriptors, @fd_list would have length 2, and
 * `g_variant_new_handle (0)` and `g_variant_new_handle (1)` would appear
 * somewhere in the body of the message (not necessarily in that order!)
 * to represent the file descriptors at indexes 0 and 1 respectively.
 *
 * When designing D-Bus APIs that are intended to be interoperable,
 * please note that non-GDBus implementations of D-Bus can usually only
 * access file descriptors if they are referenced in this way by a
 * value of type %G_VARIANT_TYPE_HANDLE in the body of the message.
 *
 * This method is only available on UNIX.
 *
 * Since: 2.30
 */
void
g_dbus_connection_call_with_unix_fd_list (GDBusConnection     *connection,
                                          const gchar         *bus_name,
                                          const gchar         *object_path,
                                          const gchar         *interface_name,
                                          const gchar         *method_name,
                                          GVariant            *parameters,
                                          const GVariantType  *reply_type,
                                          GDBusCallFlags       flags,
                                          gint                 timeout_msec,
                                          GUnixFDList         *fd_list,
                                          GCancellable        *cancellable,
                                          GAsyncReadyCallback  callback,
                                          gpointer             user_data)
{
  g_dbus_connection_call_internal (connection, bus_name, object_path, interface_name, method_name, parameters, reply_type, flags, timeout_msec, fd_list, cancellable, callback, user_data);
}

/**
 * g_dbus_connection_call_with_unix_fd_list_finish:
 * @connection: a #GDBusConnection
 * @out_fd_list: (out) (optional) (nullable): return location for a #GUnixFDList or %NULL
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed to
 *     g_dbus_connection_call_with_unix_fd_list()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_dbus_connection_call_with_unix_fd_list().
 *
 * The file descriptors normally correspond to %G_VARIANT_TYPE_HANDLE
 * values in the body of the message. For example,
 * if g_variant_get_handle() returns 5, that is intended to be a reference
 * to the file descriptor that can be accessed by
 * `g_unix_fd_list_get (*out_fd_list, 5, ...)`.
 *
 * When designing D-Bus APIs that are intended to be interoperable,
 * please note that non-GDBus implementations of D-Bus can usually only
 * access file descriptors if they are referenced in this way by a
 * value of type %G_VARIANT_TYPE_HANDLE in the body of the message.
 *
 * Returns: (transfer full): %NULL if @error is set. Otherwise a non-floating
 *     #GVariant tuple with return values. Free with g_variant_unref().
 *
 * Since: 2.30
 */
GVariant *
g_dbus_connection_call_with_unix_fd_list_finish (GDBusConnection  *connection,
                                                 GUnixFDList     **out_fd_list,
                                                 GAsyncResult     *res,
                                                 GError          **error)
{
  return g_dbus_connection_call_finish_internal (connection, out_fd_list, res, error);
}

/**
 * g_dbus_connection_call_with_unix_fd_list_sync:
 * @connection: a #GDBusConnection
 * @bus_name: (nullable): a unique or well-known bus name or %NULL
 *     if @connection is not a message bus connection
 * @object_path: path of remote object
 * @interface_name: D-Bus interface to invoke method on
 * @method_name: the name of the method to invoke
 * @parameters: (nullable): a #GVariant tuple with parameters for
 *     the method or %NULL if not passing parameters
 * @reply_type: (nullable): the expected type of the reply, or %NULL
 * @flags: flags from the #GDBusCallFlags enumeration
 * @timeout_msec: the timeout in milliseconds, -1 to use the default
 *     timeout or %G_MAXINT for no timeout
 * @fd_list: (nullable): a #GUnixFDList or %NULL
 * @out_fd_list: (out) (optional) (nullable): return location for a #GUnixFDList or %NULL
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Like g_dbus_connection_call_sync() but also takes and returns #GUnixFDList objects.
 * See g_dbus_connection_call_with_unix_fd_list() and
 * g_dbus_connection_call_with_unix_fd_list_finish() for more details.
 *
 * This method is only available on UNIX.
 *
 * Returns: (transfer full): %NULL if @error is set. Otherwise a non-floating
 *     #GVariant tuple with return values. Free with g_variant_unref().
 *
 * Since: 2.30
 */
GVariant *
g_dbus_connection_call_with_unix_fd_list_sync (GDBusConnection     *connection,
                                               const gchar         *bus_name,
                                               const gchar         *object_path,
                                               const gchar         *interface_name,
                                               const gchar         *method_name,
                                               GVariant            *parameters,
                                               const GVariantType  *reply_type,
                                               GDBusCallFlags       flags,
                                               gint                 timeout_msec,
                                               GUnixFDList         *fd_list,
                                               GUnixFDList        **out_fd_list,
                                               GCancellable        *cancellable,
                                               GError             **error)
{
  return g_dbus_connection_call_sync_internal (connection, bus_name, object_path, interface_name, method_name, parameters, reply_type, flags, timeout_msec, fd_list, out_fd_list, cancellable, error);
}

#endif /* G_OS_UNIX */

/* ---------------------------------------------------------------------------------------------------- */

/* called without lock held in the thread where the caller registered
 * the subtree
 */
static gboolean
handle_subtree_introspect (GDBusConnection *connection,
                           ExportedSubtree *es,
                           GDBusMessage    *message)
{
  GString *s;
  gboolean handled;
  GDBusMessage *reply;
  gchar **children;
  gboolean is_root;
  const gchar *sender;
  const gchar *requested_object_path;
  const gchar *requested_node;
  GDBusInterfaceInfo **interfaces;
  guint n;
  gchar **subnode_paths;
  gboolean has_properties_interface;
  gboolean has_introspectable_interface;

  handled = FALSE;

  requested_object_path = g_dbus_message_get_path (message);
  sender = g_dbus_message_get_sender (message);
  is_root = (g_strcmp0 (requested_object_path, es->object_path) == 0);

  s = g_string_new (NULL);
  introspect_append_header (s);

  /* Strictly we don't need the children in dynamic mode, but we avoid the
   * conditionals to preserve code clarity
   */
  children = es->vtable->enumerate (es->connection,
                                    sender,
                                    es->object_path,
                                    es->user_data);

  if (!is_root)
    {
      requested_node = strrchr (requested_object_path, '/') + 1;

      /* Assert existence of object if we are not dynamic */
      if (!(es->flags & G_DBUS_SUBTREE_FLAGS_DISPATCH_TO_UNENUMERATED_NODES) &&
          !g_strv_contains ((const gchar * const *) children, requested_node))
        goto out;
    }
  else
    {
      requested_node = NULL;
    }

  interfaces = es->vtable->introspect (es->connection,
                                       sender,
                                       es->object_path,
                                       requested_node,
                                       es->user_data);
  if (interfaces != NULL)
    {
      has_properties_interface = FALSE;
      has_introspectable_interface = FALSE;

      for (n = 0; interfaces[n] != NULL; n++)
        {
          if (strcmp (interfaces[n]->name, DBUS_INTERFACE_PROPERTIES) == 0)
            has_properties_interface = TRUE;
          else if (strcmp (interfaces[n]->name, DBUS_INTERFACE_INTROSPECTABLE) == 0)
            has_introspectable_interface = TRUE;
        }
      if (!has_properties_interface)
        g_string_append (s, introspect_properties_interface);
      if (!has_introspectable_interface)
        g_string_append (s, introspect_introspectable_interface);

      for (n = 0; interfaces[n] != NULL; n++)
        {
          g_dbus_interface_info_generate_xml (interfaces[n], 2, s);
          g_dbus_interface_info_unref (interfaces[n]);
        }
      g_free (interfaces);
    }

  /* then include <node> entries from the Subtree for the root */
  if (is_root)
    {
      for (n = 0; children != NULL && children[n] != NULL; n++)
        g_string_append_printf (s, "  <node name=\"%s\"/>\n", children[n]);
    }

  /* finally include nodes registered below us */
  subnode_paths = g_dbus_connection_list_registered (es->connection, requested_object_path);
  for (n = 0; subnode_paths != NULL && subnode_paths[n] != NULL; n++)
    g_string_append_printf (s, "  <node name=\"%s\"/>\n", subnode_paths[n]);
  g_strfreev (subnode_paths);

  g_string_append (s, "</node>\n");

  reply = g_dbus_message_new_method_reply (message);
  g_dbus_message_set_body (reply, g_variant_new ("(s)", s->str));
  g_dbus_connection_send_message (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);

  handled = TRUE;

 out:
  g_string_free (s, TRUE);
  g_strfreev (children);
  return handled;
}

/* called without lock held in the thread where the caller registered
 * the subtree
 */
static gboolean
handle_subtree_method_invocation (GDBusConnection *connection,
                                  ExportedSubtree *es,
                                  GDBusMessage    *message)
{
  gboolean handled;
  const gchar *sender;
  const gchar *interface_name;
  const gchar *member;
  const gchar *signature;
  const gchar *requested_object_path;
  const gchar *requested_node;
  gboolean is_root;
  GDBusInterfaceInfo *interface_info;
  const GDBusInterfaceVTable *interface_vtable;
  gpointer interface_user_data;
  guint n;
  GDBusInterfaceInfo **interfaces;
  gboolean is_property_get;
  gboolean is_property_set;
  gboolean is_property_get_all;

  handled = FALSE;
  interfaces = NULL;

  requested_object_path = g_dbus_message_get_path (message);
  sender = g_dbus_message_get_sender (message);
  interface_name = g_dbus_message_get_interface (message);
  member = g_dbus_message_get_member (message);
  signature = g_dbus_message_get_signature (message);
  is_root = (g_strcmp0 (requested_object_path, es->object_path) == 0);

  is_property_get = FALSE;
  is_property_set = FALSE;
  is_property_get_all = FALSE;
  if (g_strcmp0 (interface_name, DBUS_INTERFACE_PROPERTIES) == 0)
    {
      if (g_strcmp0 (member, "Get") == 0 && g_strcmp0 (signature, "ss") == 0)
        is_property_get = TRUE;
      else if (g_strcmp0 (member, "Set") == 0 && g_strcmp0 (signature, "ssv") == 0)
        is_property_set = TRUE;
      else if (g_strcmp0 (member, "GetAll") == 0 && g_strcmp0 (signature, "s") == 0)
        is_property_get_all = TRUE;
    }

  if (!is_root)
    {
      requested_node = strrchr (requested_object_path, '/') + 1;

      if (~es->flags & G_DBUS_SUBTREE_FLAGS_DISPATCH_TO_UNENUMERATED_NODES)
        {
          /* We don't want to dispatch to unenumerated
           * nodes, so ensure that the child exists.
           */
          gchar **children;
          gboolean exists;

          children = es->vtable->enumerate (es->connection,
                                            sender,
                                            es->object_path,
                                            es->user_data);

          exists = g_strv_contains ((const gchar * const *) children, requested_node);
          g_strfreev (children);

          if (!exists)
            goto out;
        }
    }
  else
    {
      requested_node = NULL;
    }

  /* get introspection data for the node */
  interfaces = es->vtable->introspect (es->connection,
                                       sender,
                                       requested_object_path,
                                       requested_node,
                                       es->user_data);

  if (interfaces == NULL)
    goto out;

  interface_info = NULL;
  for (n = 0; interfaces[n] != NULL; n++)
    {
      if (g_strcmp0 (interfaces[n]->name, interface_name) == 0)
        interface_info = interfaces[n];
    }

  /* dispatch the call if the user wants to handle it */
  if (interface_info != NULL)
    {
      /* figure out where to dispatch the method call */
      interface_user_data = NULL;
      interface_vtable = es->vtable->dispatch (es->connection,
                                               sender,
                                               es->object_path,
                                               interface_name,
                                               requested_node,
                                               &interface_user_data,
                                               es->user_data);
      if (interface_vtable == NULL)
        goto out;

      CONNECTION_LOCK (connection);
      handled = validate_and_maybe_schedule_method_call (es->connection,
                                                         message,
                                                         0,
                                                         es->id,
                                                         interface_info,
                                                         interface_vtable,
                                                         es->context,
                                                         interface_user_data);
      CONNECTION_UNLOCK (connection);
    }
  /* handle DBUS_INTERFACE_PROPERTIES if not explicitly handled */
  else if (is_property_get || is_property_set || is_property_get_all)
    {
      if (is_property_get)
        g_variant_get (g_dbus_message_get_body (message), "(&s&s)", &interface_name, NULL);
      else if (is_property_set)
        g_variant_get (g_dbus_message_get_body (message), "(&s&sv)", &interface_name, NULL, NULL);
      else if (is_property_get_all)
        g_variant_get (g_dbus_message_get_body (message), "(&s)", &interface_name, NULL, NULL);
      else
        g_assert_not_reached ();

      /* see if the object supports this interface at all */
      for (n = 0; interfaces[n] != NULL; n++)
        {
          if (g_strcmp0 (interfaces[n]->name, interface_name) == 0)
            interface_info = interfaces[n];
        }

      /* Fail with DBUS_ERROR_INVALID_ARGS if the user-code
       * claims it won't support the interface
       */
      if (interface_info == NULL)
        {
          GDBusMessage *reply;
          reply = g_dbus_message_new_method_error (message,
                                                   DBUS_ERROR_INVALID_ARGS,
                                                   _("No such interface “%s”"),
                                                   interface_name);
          g_dbus_connection_send_message (es->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
          g_object_unref (reply);
          handled = TRUE;
          goto out;
        }

      /* figure out where to dispatch the property get/set/getall calls */
      interface_user_data = NULL;
      interface_vtable = es->vtable->dispatch (es->connection,
                                               sender,
                                               es->object_path,
                                               interface_name,
                                               requested_node,
                                               &interface_user_data,
                                               es->user_data);
      if (interface_vtable == NULL)
        {
          g_warning ("The subtree introspection function indicates that '%s' "
                     "is a valid interface name, but calling the dispatch "
                     "function on that interface gave us NULL", interface_name);
          goto out;
        }

      if (is_property_get || is_property_set)
        {
          CONNECTION_LOCK (connection);
          handled = validate_and_maybe_schedule_property_getset (es->connection,
                                                                 message,
                                                                 0,
                                                                 es->id,
                                                                 is_property_get,
                                                                 interface_info,
                                                                 interface_vtable,
                                                                 es->context,
                                                                 interface_user_data);
          CONNECTION_UNLOCK (connection);
        }
      else if (is_property_get_all)
        {
          CONNECTION_LOCK (connection);
          handled = validate_and_maybe_schedule_property_get_all (es->connection,
                                                                  message,
                                                                  0,
                                                                  es->id,
                                                                  interface_info,
                                                                  interface_vtable,
                                                                  es->context,
                                                                  interface_user_data);
          CONNECTION_UNLOCK (connection);
        }
    }

 out:
  if (interfaces != NULL)
    {
      for (n = 0; interfaces[n] != NULL; n++)
        g_dbus_interface_info_unref (interfaces[n]);
      g_free (interfaces);
    }

  return handled;
}

typedef struct
{
  GDBusMessage *message;  /* (owned) */
  ExportedSubtree *es;  /* (owned) */
} SubtreeDeferredData;

static void
subtree_deferred_data_free (SubtreeDeferredData *data)
{
  g_clear_object (&data->message);
  exported_subtree_unref (data->es);
  g_free (data);
}

/* called without lock held in the thread where the caller registered the subtree */
static gboolean
process_subtree_vtable_message_in_idle_cb (gpointer _data)
{
  SubtreeDeferredData *data = _data;
  gboolean handled;

  handled = FALSE;

  if (g_strcmp0 (g_dbus_message_get_interface (data->message), DBUS_INTERFACE_INTROSPECTABLE) == 0 &&
      g_strcmp0 (g_dbus_message_get_member (data->message), "Introspect") == 0 &&
      g_strcmp0 (g_dbus_message_get_signature (data->message), "") == 0)
    handled = handle_subtree_introspect (data->es->connection,
                                         data->es,
                                         data->message);
  else
    handled = handle_subtree_method_invocation (data->es->connection,
                                                data->es,
                                                data->message);

  if (!handled)
    {
      CONNECTION_LOCK (data->es->connection);
      handled = handle_generic_unlocked (data->es->connection, data->message);
      CONNECTION_UNLOCK (data->es->connection);
    }

  /* if we couldn't handle the request, just bail with the UnknownMethod error */
  if (!handled)
    {
      GDBusMessage *reply;
      reply = g_dbus_message_new_method_error (data->message,
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               _("Method “%s” on interface “%s” with signature “%s” does not exist"),
                                               g_dbus_message_get_member (data->message),
                                               g_dbus_message_get_interface (data->message),
                                               g_dbus_message_get_signature (data->message));
      g_dbus_connection_send_message (data->es->connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
      g_object_unref (reply);
    }

  return FALSE;
}

/* called in GDBusWorker thread with connection's lock held */
static gboolean
subtree_message_func (GDBusConnection *connection,
                      ExportedSubtree *es,
                      GDBusMessage    *message)
{
  GSource *idle_source;
  SubtreeDeferredData *data;

  data = g_new0 (SubtreeDeferredData, 1);
  data->message = g_object_ref (message);
  data->es = exported_subtree_ref (es);

  /* defer this call to an idle handler in the right thread */
  idle_source = g_idle_source_new ();
  g_source_set_priority (idle_source, G_PRIORITY_HIGH);
  g_source_set_callback (idle_source,
                         process_subtree_vtable_message_in_idle_cb,
                         data,
                         (GDestroyNotify) subtree_deferred_data_free);
  g_source_set_static_name (idle_source, "[gio] process_subtree_vtable_message_in_idle_cb");
  g_source_attach (idle_source, es->context);
  g_source_unref (idle_source);

  /* since we own the entire subtree, handlers for objects not in the subtree have been
   * tried already by libdbus-1 - so we just need to ensure that we're always going
   * to reply to the message
   */
  return TRUE;
}

/**
 * g_dbus_connection_register_subtree:
 * @connection: a #GDBusConnection
 * @object_path: the object path to register the subtree at
 * @vtable: a #GDBusSubtreeVTable to enumerate, introspect and
 *     dispatch nodes in the subtree
 * @flags: flags used to fine tune the behavior of the subtree
 * @user_data: data to pass to functions in @vtable
 * @user_data_free_func: function to call when the subtree is unregistered
 * @error: return location for error or %NULL
 *
 * Registers a whole subtree of dynamic objects.
 *
 * The @enumerate and @introspection functions in @vtable are used to
 * convey, to remote callers, what nodes exist in the subtree rooted
 * by @object_path.
 *
 * When handling remote calls into any node in the subtree, first the
 * @enumerate function is used to check if the node exists. If the node exists
 * or the %G_DBUS_SUBTREE_FLAGS_DISPATCH_TO_UNENUMERATED_NODES flag is set
 * the @introspection function is used to check if the node supports the
 * requested method. If so, the @dispatch function is used to determine
 * where to dispatch the call. The collected #GDBusInterfaceVTable and
 * #gpointer will be used to call into the interface vtable for processing
 * the request.
 *
 * All calls into user-provided code will be invoked in the thread-default
 * main context (see [method@GLib.MainContext.push_thread_default])
 * of the thread you are calling this method from.
 *
 * If an existing subtree is already registered at @object_path or
 * then @error is set to %G_IO_ERROR_EXISTS.
 *
 * Note that it is valid to register regular objects (using
 * g_dbus_connection_register_object()) in a subtree registered with
 * g_dbus_connection_register_subtree() - if so, the subtree handler
 * is tried as the last resort. One way to think about a subtree
 * handler is to consider it a fallback handler for object paths not
 * registered via g_dbus_connection_register_object() or other bindings.
 *
 * Note that @vtable will be copied so you cannot change it after
 * registration.
 *
 * See this [server][class@Gio.DBusConnection#an-example-for-exporting-a-subtree]
 * for an example of how to use this method.
 *
 * Returns: 0 if @error is set, otherwise a subtree registration ID (never 0)
 * that can be used with g_dbus_connection_unregister_subtree()
 *
 * Since: 2.26
 */
guint
g_dbus_connection_register_subtree (GDBusConnection           *connection,
                                    const gchar               *object_path,
                                    const GDBusSubtreeVTable  *vtable,
                                    GDBusSubtreeFlags          flags,
                                    gpointer                   user_data,
                                    GDestroyNotify             user_data_free_func,
                                    GError                   **error)
{
  guint ret;
  ExportedSubtree *es;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), 0);
  g_return_val_if_fail (object_path != NULL && g_variant_is_object_path (object_path), 0);
  g_return_val_if_fail (vtable != NULL, 0);
  g_return_val_if_fail (error == NULL || *error == NULL, 0);
  g_return_val_if_fail (check_initialized (connection), 0);

  ret = 0;

  CONNECTION_LOCK (connection);

  es = g_hash_table_lookup (connection->map_object_path_to_es, object_path);
  if (es != NULL)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_EXISTS,
                   _("A subtree is already exported for %s"),
                   object_path);
      goto out;
    }

  es = g_new0 (ExportedSubtree, 1);
  es->refcount = 1;
  es->object_path = g_strdup (object_path);
  es->connection = connection;

  es->vtable = _g_dbus_subtree_vtable_copy (vtable);
  es->flags = flags;
  es->id = (guint) g_atomic_int_add (&_global_subtree_registration_id, 1); /* TODO: overflow etc. */
  es->user_data = user_data;
  es->user_data_free_func = user_data_free_func;
  es->context = g_main_context_ref_thread_default ();

  g_hash_table_insert (connection->map_object_path_to_es, es->object_path, es);
  g_hash_table_insert (connection->map_id_to_es,
                       GUINT_TO_POINTER (es->id),
                       es);

  ret = es->id;

 out:
  CONNECTION_UNLOCK (connection);

  if (ret == 0 && user_data_free_func != NULL)
    user_data_free_func (user_data);

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * g_dbus_connection_unregister_subtree:
 * @connection: a #GDBusConnection
 * @registration_id: a subtree registration id obtained from
 *     g_dbus_connection_register_subtree()
 *
 * Unregisters a subtree.
 *
 * Returns: %TRUE if the subtree was unregistered, %FALSE otherwise
 *
 * Since: 2.26
 */
gboolean
g_dbus_connection_unregister_subtree (GDBusConnection *connection,
                                      guint            registration_id)
{
  ExportedSubtree *es;
  gboolean ret;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (check_initialized (connection), FALSE);

  ret = FALSE;

  CONNECTION_LOCK (connection);

  es = g_hash_table_lookup (connection->map_id_to_es,
                            GUINT_TO_POINTER (registration_id));
  if (es == NULL)
    goto out;

  g_warn_if_fail (g_hash_table_remove (connection->map_id_to_es, GUINT_TO_POINTER (es->id)));
  g_warn_if_fail (g_hash_table_remove (connection->map_object_path_to_es, es->object_path));

  ret = TRUE;

 out:
  CONNECTION_UNLOCK (connection);

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

/* may be called in any thread, with connection's lock held */
static void
handle_generic_ping_unlocked (GDBusConnection *connection,
                              const gchar     *object_path,
                              GDBusMessage    *message)
{
  GDBusMessage *reply;
  reply = g_dbus_message_new_method_reply (message);
  g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);
}

/* may be called in any thread, with connection's lock held */
static void
handle_generic_get_machine_id_unlocked (GDBusConnection *connection,
                                        const gchar     *object_path,
                                        GDBusMessage    *message)
{
  GDBusMessage *reply;

  reply = NULL;
  if (connection->machine_id == NULL)
    {
      GError *error;

      error = NULL;
      connection->machine_id = _g_dbus_get_machine_id (&error);
      if (connection->machine_id == NULL)
        {
          reply = g_dbus_message_new_method_error_literal (message,
                                                           DBUS_ERROR_FAILED,
                                                           error->message);
          g_error_free (error);
        }
    }

  if (reply == NULL)
    {
      reply = g_dbus_message_new_method_reply (message);
      g_dbus_message_set_body (reply, g_variant_new ("(s)", connection->machine_id));
    }
  g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);
}

/* may be called in any thread, with connection's lock held */
static void
handle_generic_introspect_unlocked (GDBusConnection *connection,
                                    const gchar     *object_path,
                                    GDBusMessage    *message)
{
  guint n;
  GString *s;
  gchar **registered;
  GDBusMessage *reply;

  /* first the header */
  s = g_string_new (NULL);
  introspect_append_header (s);

  registered = g_dbus_connection_list_registered_unlocked (connection, object_path);
  for (n = 0; registered != NULL && registered[n] != NULL; n++)
      g_string_append_printf (s, "  <node name=\"%s\"/>\n", registered[n]);
  g_strfreev (registered);
  g_string_append (s, "</node>\n");

  reply = g_dbus_message_new_method_reply (message);
  g_dbus_message_set_body (reply, g_variant_new ("(s)", s->str));
  g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);
  g_string_free (s, TRUE);
}

/* may be called in any thread, with connection's lock held */
static gboolean
handle_generic_unlocked (GDBusConnection *connection,
                         GDBusMessage    *message)
{
  gboolean handled;
  const gchar *interface_name;
  const gchar *member;
  const gchar *signature;
  const gchar *path;

  CONNECTION_ENSURE_LOCK (connection);

  handled = FALSE;

  interface_name = g_dbus_message_get_interface (message);
  member = g_dbus_message_get_member (message);
  signature = g_dbus_message_get_signature (message);
  path = g_dbus_message_get_path (message);

  if (g_strcmp0 (interface_name, DBUS_INTERFACE_INTROSPECTABLE) == 0 &&
      g_strcmp0 (member, "Introspect") == 0 &&
      g_strcmp0 (signature, "") == 0)
    {
      handle_generic_introspect_unlocked (connection, path, message);
      handled = TRUE;
    }
  else if (g_strcmp0 (interface_name, DBUS_INTERFACE_PEER) == 0 &&
           g_strcmp0 (member, "Ping") == 0 &&
           g_strcmp0 (signature, "") == 0)
    {
      handle_generic_ping_unlocked (connection, path, message);
      handled = TRUE;
    }
  else if (g_strcmp0 (interface_name, DBUS_INTERFACE_PEER) == 0 &&
           g_strcmp0 (member, "GetMachineId") == 0 &&
           g_strcmp0 (signature, "") == 0)
    {
      handle_generic_get_machine_id_unlocked (connection, path, message);
      handled = TRUE;
    }

  return handled;
}

/* ---------------------------------------------------------------------------------------------------- */

/* called in GDBusWorker thread with connection's lock held */
static void
distribute_method_call (GDBusConnection *connection,
                        GDBusMessage    *message)
{
  GDBusMessage *reply;
  ExportedObject *eo;
  ExportedSubtree *es;
  const gchar *path;
  const gchar *interface_name;
  const gchar *member;
  gchar *subtree_path;
  gchar *needle;
  gboolean object_found = FALSE;

  g_assert (g_dbus_message_get_message_type (message) == G_DBUS_MESSAGE_TYPE_METHOD_CALL);

  /* these are required, and should have been validated by validate_headers()
   * in gdbusmessage.c already */
  member = g_dbus_message_get_member (message);
  path = g_dbus_message_get_path (message);

  g_assert (member != NULL);
  g_assert (path != NULL);

  /* this is optional */
  interface_name = g_dbus_message_get_interface (message);

  subtree_path = g_strdup (path);
  needle = strrchr (subtree_path, '/');
  if (needle != NULL && needle != subtree_path)
    {
      *needle = '\0';
    }
  else
    {
      g_free (subtree_path);
      subtree_path = NULL;
    }

  if (G_UNLIKELY (_g_dbus_debug_incoming ()))
    {
      _g_dbus_debug_print_lock ();
      g_print ("========================================================================\n"
               "GDBus-debug:Incoming:\n"
               " <<<< METHOD INVOCATION %s.%s()\n"
               "      on object %s\n"
               "      invoked by name %s\n"
               "      serial %d\n",
               interface_name, member,
               path,
               g_dbus_message_get_sender (message) != NULL ? g_dbus_message_get_sender (message) : "(none)",
               g_dbus_message_get_serial (message));
      _g_dbus_debug_print_unlock ();
    }

  eo = g_hash_table_lookup (connection->map_object_path_to_eo, path);
  if (eo != NULL)
    {
      if (obj_message_func (connection, eo, message, &object_found))
        goto out;
    }

  es = g_hash_table_lookup (connection->map_object_path_to_es, path);
  if (es != NULL)
    {
      if (subtree_message_func (connection, es, message))
        goto out;
    }

  if (subtree_path != NULL)
    {
      es = g_hash_table_lookup (connection->map_object_path_to_es, subtree_path);
      if (es != NULL)
        {
          if (subtree_message_func (connection, es, message))
            goto out;
        }
    }

  if (handle_generic_unlocked (connection, message))
    goto out;

  /* if we end up here, the message has not been not handled - so return an error saying this */
  if (object_found == TRUE)
    {
      reply = g_dbus_message_new_method_error (message,
                                               DBUS_ERROR_UNKNOWN_METHOD,
                                               _("No such interface “%s” on object at path %s"),
                                               interface_name,
                                               path);
    }
  else
    {
      reply = g_dbus_message_new_method_error (message,
                                           DBUS_ERROR_UNKNOWN_METHOD,
                                           _("Object does not exist at path “%s”"),
                                           path);
    }

  g_dbus_connection_send_message_unlocked (connection, reply, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
  g_object_unref (reply);

 out:
  g_free (subtree_path);
}

/* ---------------------------------------------------------------------------------------------------- */

/* Called in any user thread, with the message_bus_lock held. */
static GWeakRef *
message_bus_get_singleton (GBusType   bus_type,
                           GError   **error)
{
  GWeakRef *ret;
  const gchar *starter_bus;

  ret = NULL;

  switch (bus_type)
    {
    case G_BUS_TYPE_SESSION:
      ret = &the_session_bus;
      break;

    case G_BUS_TYPE_SYSTEM:
      ret = &the_system_bus;
      break;

    case G_BUS_TYPE_STARTER:
      starter_bus = g_getenv ("DBUS_STARTER_BUS_TYPE");
      if (g_strcmp0 (starter_bus, "session") == 0)
        {
          ret = message_bus_get_singleton (G_BUS_TYPE_SESSION, error);
          goto out;
        }
      else if (g_strcmp0 (starter_bus, "system") == 0)
        {
          ret = message_bus_get_singleton (G_BUS_TYPE_SYSTEM, error);
          goto out;
        }
      else
        {
          if (starter_bus != NULL)
            {
              g_set_error (error,
                           G_IO_ERROR,
                           G_IO_ERROR_INVALID_ARGUMENT,
                           _("Cannot determine bus address from DBUS_STARTER_BUS_TYPE environment variable"
                             " — unknown value “%s”"),
                           starter_bus);
            }
          else
            {
              g_set_error_literal (error,
                                   G_IO_ERROR,
                                   G_IO_ERROR_INVALID_ARGUMENT,
                                   _("Cannot determine bus address because the DBUS_STARTER_BUS_TYPE environment "
                                     "variable is not set"));
            }
        }
      break;

    default:
      g_assert_not_reached ();
      break;
    }

 out:
  return ret;
}

/* Called in any user thread, without holding locks. */
static GDBusConnection *
get_uninitialized_connection (GBusType       bus_type,
                              GCancellable  *cancellable,
                              GError       **error)
{
  GWeakRef *singleton;
  GDBusConnection *ret;

  ret = NULL;

  G_LOCK (message_bus_lock);
  singleton = message_bus_get_singleton (bus_type, error);
  if (singleton == NULL)
    goto out;

  ret = g_weak_ref_get (singleton);

  if (ret == NULL)
    {
      gchar *address;
      address = g_dbus_address_get_for_bus_sync (bus_type, cancellable, error);
      if (address == NULL)
        goto out;
      ret = g_object_new (G_TYPE_DBUS_CONNECTION,
                          "address", address,
                          "flags", G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT |
#ifdef __linux__
                                   G_DBUS_CONNECTION_FLAGS_CROSS_NAMESPACE |
#endif
                                   G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION,
                          "exit-on-close", TRUE,
                          NULL);

      g_weak_ref_set (singleton, ret);
      g_free (address);
    }

  g_assert (ret != NULL);

 out:
  G_UNLOCK (message_bus_lock);
  return ret;
}

/* May be called from any thread. Must not hold message_bus_lock. */
GDBusConnection *
_g_bus_get_singleton_if_exists (GBusType bus_type)
{
  GWeakRef *singleton;
  GDBusConnection *ret = NULL;

  G_LOCK (message_bus_lock);
  singleton = message_bus_get_singleton (bus_type, NULL);
  if (singleton == NULL)
    goto out;

  ret = g_weak_ref_get (singleton);

 out:
  G_UNLOCK (message_bus_lock);
  return ret;
}

/* May be called from any thread. Must not hold message_bus_lock. */
void
_g_bus_forget_singleton (GBusType bus_type)
{
  GWeakRef *singleton;

  G_LOCK (message_bus_lock);

  singleton = message_bus_get_singleton (bus_type, NULL);

  if (singleton != NULL)
    g_weak_ref_set (singleton, NULL);

  G_UNLOCK (message_bus_lock);
}

/**
 * g_bus_get_sync:
 * @bus_type: a #GBusType
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @error: return location for error or %NULL
 *
 * Synchronously connects to the message bus specified by @bus_type.
 * Note that the returned object may shared with other callers,
 * e.g. if two separate parts of a process calls this function with
 * the same @bus_type, they will share the same object.
 *
 * This is a synchronous failable function. See g_bus_get() and
 * g_bus_get_finish() for the asynchronous version.
 *
 * The returned object is a singleton, that is, shared with other
 * callers of g_bus_get() and g_bus_get_sync() for @bus_type. In the
 * event that you need a private message bus connection, use
 * g_dbus_address_get_for_bus_sync() and
 * g_dbus_connection_new_for_address() with
 * G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT and
 * G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION flags.
 *
 * Note that the returned #GDBusConnection object will (usually) have
 * the #GDBusConnection:exit-on-close property set to %TRUE.
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set.
 *     Free with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_bus_get_sync (GBusType       bus_type,
                GCancellable  *cancellable,
                GError       **error)
{
  GDBusConnection *connection;

  _g_dbus_initialize ();

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  connection = get_uninitialized_connection (bus_type, cancellable, error);
  if (connection == NULL)
    goto out;

  if (!g_initable_init (G_INITABLE (connection), cancellable, error))
    {
      g_object_unref (connection);
      connection = NULL;
    }

 out:
  return connection;
}

static void
bus_get_async_initable_cb (GObject      *source_object,
                           GAsyncResult *res,
                           gpointer      user_data)
{
  GTask *task = user_data;
  GError *error = NULL;

  if (!g_async_initable_init_finish (G_ASYNC_INITABLE (source_object),
                                     res,
                                     &error))
    {
      g_assert (error != NULL);
      g_task_return_error (task, error);
      g_object_unref (source_object);
    }
  else
    {
      g_task_return_pointer (task, source_object, g_object_unref);
    }
  g_object_unref (task);
}

/**
 * g_bus_get:
 * @bus_type: a #GBusType
 * @cancellable: (nullable): a #GCancellable or %NULL
 * @callback: a #GAsyncReadyCallback to call when the request is satisfied
 * @user_data: the data to pass to @callback
 *
 * Asynchronously connects to the message bus specified by @bus_type.
 *
 * When the operation is finished, @callback will be invoked. You can
 * then call g_bus_get_finish() to get the result of the operation.
 *
 * This is an asynchronous failable function. See g_bus_get_sync() for
 * the synchronous version.
 *
 * Since: 2.26
 */
void
g_bus_get (GBusType             bus_type,
           GCancellable        *cancellable,
           GAsyncReadyCallback  callback,
           gpointer             user_data)
{
  GDBusConnection *connection;
  GTask *task;
  GError *error = NULL;

  _g_dbus_initialize ();

  task = g_task_new (NULL, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_bus_get);

  connection = get_uninitialized_connection (bus_type, cancellable, &error);
  if (connection == NULL)
    {
      g_assert (error != NULL);
      g_task_return_error (task, error);
      g_object_unref (task);
    }
  else
    {
      g_async_initable_init_async (G_ASYNC_INITABLE (connection),
                                   G_PRIORITY_DEFAULT,
                                   cancellable,
                                   bus_get_async_initable_cb,
                                   task);
    }
}

/**
 * g_bus_get_finish:
 * @res: a #GAsyncResult obtained from the #GAsyncReadyCallback passed
 *     to g_bus_get()
 * @error: return location for error or %NULL
 *
 * Finishes an operation started with g_bus_get().
 *
 * The returned object is a singleton, that is, shared with other
 * callers of g_bus_get() and g_bus_get_sync() for @bus_type. In the
 * event that you need a private message bus connection, use
 * g_dbus_address_get_for_bus_sync() and
 * g_dbus_connection_new_for_address() with
 * G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT and
 * G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION flags.
 *
 * Note that the returned #GDBusConnection object will (usually) have
 * the #GDBusConnection:exit-on-close property set to %TRUE.
 *
 * Returns: (transfer full): a #GDBusConnection or %NULL if @error is set.
 *     Free with g_object_unref().
 *
 * Since: 2.26
 */
GDBusConnection *
g_bus_get_finish (GAsyncResult  *res,
                  GError       **error)
{
  g_return_val_if_fail (g_task_is_valid (res, NULL), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  return g_task_propagate_pointer (G_TASK (res), error);
}

/* ---------------------------------------------------------------------------------------------------- */
