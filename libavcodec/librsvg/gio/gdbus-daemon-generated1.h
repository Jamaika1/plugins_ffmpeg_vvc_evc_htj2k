G_BEGIN_DECLS

#define G_TYPE_FREEDESKTOP_DBUS         (0)
#define FREEDESKTOP_DBUS(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_FREEDESKTOP_DBUS, GFreedesktopDBus))
#define G_IS_FREEDESKTOP_DBUS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_FREEDESKTOP_DBUS))
#define G_FREEDESKTOP_DBUS_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), G_TYPE_FREEDESKTOP_DBUS, GFreedesktopDBusIface))

typedef struct _GFreedesktopDBus GFreedesktopDBus;
typedef struct _GFreedesktopDBusIface GFreedesktopDBusIface;

/**
 * GDBusObjectIface:
 * @parent_iface: The parent interface.
 * @get_object_path: Returns the object path. See g_dbus_object_get_object_path().
 * @get_interfaces: Returns all interfaces. See g_dbus_object_get_interfaces().
 * @get_interface: Returns an interface by name. See g_dbus_object_get_interface().
 * @interface_added: Signal handler for the #GDBusObject::interface-added signal.
 * @interface_removed: Signal handler for the #GDBusObject::interface-removed signal.
 *
 * Base object type for D-Bus objects.
 *
 * Since: 2.30
 */
struct _GFreedesktopDBusIface
{
  GTypeInterface parent_iface;

  /* Virtual Functions */
  const gchar     *(*get_object_path) (GFreedesktopDBus  *object);
  GList           *(*get_interfaces)  (GFreedesktopDBus  *object);
  GDBusInterface  *(*get_interface)   (GFreedesktopDBus  *object,
                                       const gchar  *interface_name);

  gboolean (*handle_add_match) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_rule);
  gboolean (*handle_get_connection_selinux_security_context) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_get_connection_unix_process_id) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_get_connection_unix_user) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_get_id) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation);
  gboolean (*handle_get_name_owner) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_hello) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation);
  gboolean (*handle_list_activatable_names) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation);
  gboolean (*handle_list_names) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation);
  gboolean (*handle_list_queued_owners) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_name_has_owner) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_release_name) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name);
  gboolean (*handle_reload_config) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation);
  gboolean (*handle_update_activation_environment) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                GVariant *arg_environment);
  gboolean (*handle_remove_match) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_rule);
  gboolean (*handle_request_name) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name,
                                guint flags);
  gboolean (*handle_start_service_by_name) (GFreedesktopDBus       *object,
                                GDBusMethodInvocation  *invocation,
                                const gchar *arg_name,
                                guint arg_flags);
  /* Signals */
  void (*interface_added)   (GFreedesktopDBus     *object,
                             GDBusInterface  *interface_);
  void (*interface_removed) (GFreedesktopDBus     *object,
                             GDBusInterface  *interface_);

};

GIO_AVAILABLE_IN_ALL
GType            g_freedesktop_dbus_get_type        (void) G_GNUC_CONST;
GIO_AVAILABLE_IN_ALL
const gchar     *g_freedesktop_dbus_get_object_path (GFreedesktopDBus  *object);
GIO_AVAILABLE_IN_ALL
GList           *g_freedesktop_dbus_get_interfaces  (GFreedesktopDBus  *object);
GIO_AVAILABLE_IN_ALL
GDBusInterface  *g_freedesktop_dbus_get_interface   (GFreedesktopDBus  *object,
                                                const gchar  *interface_name);

#define G_TYPE_FREEDESKTOP_DBUS_SKELETON         (0)
#define G_FREEDESKTOP_DBUS_SKELETON(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_FREEDESKTOP_DBUS_SKELETON, GFreedesktopDBusSkeleton))
#define G_FREEDESKTOP_DBUS_SKELETON_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), G_TYPE_FREEDESKTOP_DBUS_SKELETON, GFreedesktopDBusSkeletonClass))
#define G_IS_FREEDESKTOP_DBUS_SKELETON(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_FREEDESKTOP_DBUS_SKELETON))
#define G_IS_FREEDESKTOP_DBUS_SKELETON_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), G_TYPE_FREEDESKTOP_DBUS_SKELETON))
#define G_FREEDESKTOP_DBUS_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), G_TYPE_FREEDESKTOP_DBUS_SKELETON, GFreedesktopDBusSkeletonClass))

/**
 * GFreedesktopDBusSkeleton:
 *
 * A base class for all input streams that work on an underlying stream.
 **/
typedef struct _GFreedesktopDBusSkeletonClass    GFreedesktopDBusSkeletonClass;
typedef struct _GFreedesktopDBusSkeleton  GFreedesktopDBusSkeleton;
//typedef struct _GFreedesktopDBusIface            GFreedesktopDBusIface;

struct _GFreedesktopDBusSkeleton
{
  /*< private >*/
  GObject parent_instance;
  //GFreedesktopDBusSkeletonPrivate *priv;
};

struct _GFreedesktopDBusSkeletonClass
{
  GObjectClass parent_class;

  /* Signals */
  gboolean (*authorize_method) (GFreedesktopDBusSkeleton       *object,
                                GDBusInterface  *interface_,
                                GDBusMethodInvocation *invocation);

  /*< private >*/
  //gpointer padding[8];
};

GIO_AVAILABLE_IN_ALL
GType          g_freedesktop_dbus_skeleton_get_type              (void) G_GNUC_CONST;
GIO_AVAILABLE_IN_ALL
GFreedesktopDBusSkeleton *g_freedesktop_dbus_skeleton_new                       (const gchar            *object_path);
GIO_AVAILABLE_IN_ALL
void                 g_freedesktop_dbus_skeleton_flush                     (GFreedesktopDBusSkeleton    *object);
GIO_AVAILABLE_IN_ALL
void                 g_freedesktop_dbus_skeleton_add_interface             (GFreedesktopDBusSkeleton    *object,
                                                                       GDBusInterface  *interface_);
GIO_AVAILABLE_IN_ALL
void                 g_freedesktop_dbus_skeleton_remove_interface          (GFreedesktopDBusSkeleton    *object,
                                                                       GDBusInterface  *interface_);
GIO_AVAILABLE_IN_ALL
void                 g_freedesktop_dbus_skeleton_remove_interface_by_name  (GFreedesktopDBusSkeleton    *object,
                                                                       const gchar            *iface_name);
GIO_AVAILABLE_IN_ALL
void                 g_freedesktop_dbus_skeleton_set_object_path           (GFreedesktopDBusSkeleton    *object,
                                                                       const gchar            *object_path);

G_END_DECLS
