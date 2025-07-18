/*
 * Copyright © 2009, 2010 Codethink Limited
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Ryan Lortie <desrt@desrt.ca>
 */

/* Prelude {{{1 */
#include "config.h"

#include "../glib/glib.h"
#include "../glib/glibintl.h"

#include "gsettings.h"

#include "gdelayedsettingsbackend.h"
#include "gsettingsbackendinternal.h"
#include "gsettings-mapping.h"
#include "gsettingsschema-internal.h"
#include "gaction.h"
#include "gmarshal-internal.h"

#include "extra/strinfo.c"

/**
 * GSettings:
 *
 * The `GSettings` class provides a convenient API for storing and retrieving
 * application settings.
 *
 * Reads and writes can be considered to be non-blocking.  Reading
 * settings with `GSettings` is typically extremely fast: on
 * approximately the same order of magnitude (but slower than) a
 * [struct@GLib.HashTable] lookup.  Writing settings is also extremely fast in
 * terms of time to return to your application, but can be extremely expensive
 * for other threads and other processes.  Many settings backends
 * (including dconf) have lazy initialisation which means in the common
 * case of the user using their computer without modifying any settings
 * a lot of work can be avoided.  For dconf, the D-Bus service doesn’t
 * even need to be started in this case.  For this reason, you should
 * only ever modify `GSettings` keys in response to explicit user action.
 * Particular care should be paid to ensure that modifications are not
 * made during startup — for example, when setting the initial value
 * of preferences widgets.  The built-in [method@Gio.Settings.bind]
 * functionality is careful not to write settings in response to notify signals
 * as a result of modifications that it makes to widgets.
 *
 * When creating a `GSettings` instance, you have to specify a schema
 * that describes the keys in your settings and their types and default
 * values, as well as some other information.
 *
 * Normally, a schema has a fixed path that determines where the settings
 * are stored in the conceptual global tree of settings. However, schemas
 * can also be ‘[relocatable](#relocatable-schemas)’, i.e. not equipped with
 * a fixed path. This is
 * useful e.g. when the schema describes an ‘account’, and you want to be
 * able to store a arbitrary number of accounts.
 *
 * Paths must start with and end with a forward slash character (`/`)
 * and must not contain two sequential slash characters.  Paths should
 * be chosen based on a domain name associated with the program or
 * library to which the settings belong.  Examples of paths are
 * `/org/gtk/settings/file-chooser/` and `/ca/desrt/dconf-editor/`.
 * Paths should not start with `/apps/`, `/desktop/` or `/system/` as
 * they often did in GConf.
 *
 * Unlike other configuration systems (like GConf), GSettings does not
 * restrict keys to basic types like strings and numbers. GSettings stores
 * values as [struct@GLib.Variant], and allows any [type@GLib.VariantType] for
 * keys. Key names are restricted to lowercase characters, numbers and `-`.
 * Furthermore, the names must begin with a lowercase character, must not end
 * with a `-`, and must not contain consecutive dashes.
 *
 * Similar to GConf, the default values in GSettings schemas can be
 * localized, but the localized values are stored in gettext catalogs
 * and looked up with the domain that is specified in the
 * `gettext-domain` attribute of the `<schemalist>` or `<schema>`
 * elements and the category that is specified in the `l10n` attribute of
 * the `<default>` element. The string which is translated includes all text in
 * the `<default>` element, including any surrounding quotation marks.
 *
 * The `l10n` attribute must be set to `messages` or `time`, and sets the
 * [locale category for
 * translation](https://www.gnu.org/software/gettext/manual/html_node/Aspects.html#index-locale-categories-1).
 * The `messages` category should be used by default; use `time` for
 * translatable date or time formats. A translation comment can be added as an
 * XML comment immediately above the `<default>` element — it is recommended to
 * add these comments to aid translators understand the meaning and
 * implications of the default value. An optional translation `context`
 * attribute can be set on the `<default>` element to disambiguate multiple
 * defaults which use the same string.
 *
 * For example:
 * ```xml
 *  <!-- Translators: A list of words which are not allowed to be typed, in
 *       GVariant serialization syntax.
 *       See: https://developer.gnome.org/glib/stable/gvariant-text.html -->
 *  <default l10n='messages' context='Banned words'>['bad', 'words']</default>
 * ```
 *
 * Translations of default values must remain syntactically valid serialized
 * [struct@GLib.Variant]s (e.g. retaining any surrounding quotation marks) or
 * runtime errors will occur.
 *
 * GSettings uses schemas in a compact binary form that is created
 * by the [`glib-compile-schemas`](glib-compile-schemas.html)
 * utility. The input is a schema description in an XML format.
 *
 * A DTD for the gschema XML format can be found here:
 * [gschema.dtd](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/gschema.dtd)
 *
 * The [`glib-compile-schemas`](glib-compile-schemas.html) tool expects schema
 * files to have the extension `.gschema.xml`.
 *
 * At runtime, schemas are identified by their ID (as specified in the
 * `id` attribute of the `<schema>` element). The convention for schema
 * IDs is to use a dotted name, similar in style to a D-Bus bus name,
 * e.g. `org.gnome.SessionManager`. In particular, if the settings are
 * for a specific service that owns a D-Bus bus name, the D-Bus bus name
 * and schema ID should match. For schemas which deal with settings not
 * associated with one named application, the ID should not use
 * StudlyCaps, e.g. `org.gnome.font-rendering`.
 *
 * In addition to [struct@GLib.Variant] types, keys can have types that have
 * enumerated types. These can be described by a `<choice>`,
 * `<enum>` or `<flags>` element, as seen in the
 * second example below. The underlying type of such a key
 * is string, but you can use [method@Gio.Settings.get_enum],
 * [method@Gio.Settings.set_enum], [method@Gio.Settings.get_flags],
 * [method@Gio.Settings.set_flags] access the numeric values corresponding to
 * the string value of enum and flags keys.
 *
 * An example for default value:
 * ```xml
 * <schemalist>
 *   <schema id="org.gtk.Test" path="/org/gtk/Test/" gettext-domain="test">
 *
 *     <key name="greeting" type="s">
 *       <default l10n="messages">"Hello, earthlings"</default>
 *       <summary>A greeting</summary>
 *       <description>
 *         Greeting of the invading martians
 *       </description>
 *     </key>
 *
 *     <key name="box" type="(ii)">
 *       <default>(20,30)</default>
 *     </key>
 *
 *     <key name="empty-string" type="s">
 *       <default>""</default>
 *       <summary>Empty strings have to be provided in GVariant form</summary>
 *     </key>
 *
 *   </schema>
 * </schemalist>
 * ```
 *
 * An example for ranges, choices and enumerated types:
 * ```xml
 * <schemalist>
 *
 *   <enum id="org.gtk.Test.myenum">
 *     <value nick="first" value="1"/>
 *     <value nick="second" value="2"/>
 *   </enum>
 *
 *   <flags id="org.gtk.Test.myflags">
 *     <value nick="flag1" value="1"/>
 *     <value nick="flag2" value="2"/>
 *     <value nick="flag3" value="4"/>
 *   </flags>
 *
 *   <schema id="org.gtk.Test">
 *
 *     <key name="key-with-range" type="i">
 *       <range min="1" max="100"/>
 *       <default>10</default>
 *     </key>
 *
 *     <key name="key-with-choices" type="s">
 *       <choices>
 *         <choice value='Elisabeth'/>
 *         <choice value='Annabeth'/>
 *         <choice value='Joe'/>
 *       </choices>
 *       <aliases>
 *         <alias value='Anna' target='Annabeth'/>
 *         <alias value='Beth' target='Elisabeth'/>
 *       </aliases>
 *       <default>'Joe'</default>
 *     </key>
 *
 *     <key name='enumerated-key' enum='org.gtk.Test.myenum'>
 *       <default>'first'</default>
 *     </key>
 *
 *     <key name='flags-key' flags='org.gtk.Test.myflags'>
 *       <default>["flag1","flag2"]</default>
 *     </key>
 *   </schema>
 * </schemalist>
 * ```
 *
 * ## Vendor overrides
 *
 * Default values are defined in the schemas that get installed by
 * an application. Sometimes, it is necessary for a vendor or distributor
 * to adjust these defaults. Since patching the XML source for the schema
 * is inconvenient and error-prone,
 * [`glib-compile-schemas`](glib-compile-schemas.html) reads so-called ‘vendor
 * override’ files. These are keyfiles in the same directory as the XML
 * schema sources which can override default values. The schema ID serves
 * as the group name in the key file, and the values are expected in
 * serialized [struct@GLib.Variant] form, as in the following example:
 * ```
 * [org.gtk.Example]
 * key1='string'
 * key2=1.5
 * ```
 *
 * `glib-compile-schemas` expects schema files to have the extension
 * `.gschema.override`.
 *
 * ## Delay-apply mode
 *
 * By default, values set on a [class@Gio.Settings] instance immediately start
 * to be written to the backend (although these writes may not complete by the
 * time that [method@Gio.Settings.set]) returns; see [func@Gio.Settings.sync]).
 *
 * In order to allow groups of settings to be changed simultaneously and
 * atomically, GSettings also supports a ‘delay-apply’ mode. In this mode,
 * updated values are kept locally in the [class@Gio.Settings] instance until
 * they are explicitly applied by calling [method@Gio.Settings.apply].
 *
 * For example, this could be useful for a preferences dialog where the
 * preferences all need to be applied simultaneously when the user clicks ‘Save’.
 *
 * Switching a [class@Gio.Settings] instance to ‘delay-apply’ mode is a one-time
 * irreversible operation: from that point onwards, *all* changes made to that
 * [class@Gio.Settings] have to be explicitly applied by calling
 * [method@Gio.Settings.apply]. The ‘delay-apply’ mode is also propagated to any
 * child settings objects subsequently created using
 * [method@Gio.Settings.get_child].
 *
 * At any point, the set of unapplied changes can be queried using
 * [property@Gio.Settings:has-unapplied], and discarded by calling
 * [method@Gio.Settings.revert].
 *
 * ## Binding
 *
 * A very convenient feature of GSettings lets you bind [class@GObject.Object]
 * properties directly to settings, using [method@Gio.Settings.bind]. Once a
 * [class@GObject.Object] property has been bound to a setting, changes on
 * either side are automatically propagated to the other side. GSettings handles
 * details like mapping between [class@GObject.Object] and [struct@GLib.Variant]
 * types, and preventing infinite cycles.
 *
 * This makes it very easy to hook up a preferences dialog to the
 * underlying settings. To make this even more convenient, GSettings
 * looks for a boolean property with the name `sensitivity` and
 * automatically binds it to the writability of the bound setting.
 * If this ‘magic’ gets in the way, it can be suppressed with the
 * `G_SETTINGS_BIND_NO_SENSITIVITY` flag.
 *
 * ## Relocatable schemas
 *
 * A relocatable schema is one with no `path` attribute specified on its
 * `<schema>` element. By using [ctor@Gio.Settings.new_with_path], a `GSettings`
 * object can be instantiated for a relocatable schema, assigning a path to the
 * instance. Paths passed to [ctor@Gio.Settings.new_with_path] will typically be
 * constructed dynamically from a constant prefix plus some form of instance
 * identifier; but they must still be valid GSettings paths. Paths could also
 * be constant and used with a globally installed schema originating from a
 * dependency library.
 *
 * For example, a relocatable schema could be used to store geometry information
 * for different windows in an application. If the schema ID was
 * `org.foo.MyApp.Window`, it could be instantiated for paths
 * `/org/foo/MyApp/main/`, `/org/foo/MyApp/document-1/`,
 * `/org/foo/MyApp/document-2/`, etc. If any of the paths are well-known
 * they can be specified as `<child>` elements in the parent schema, e.g.:
 * ```xml
 * <schema id="org.foo.MyApp" path="/org/foo/MyApp/">
 *   <child name="main" schema="org.foo.MyApp.Window"/>
 * </schema>
 * ```
 *
 * ## Build system integration
 *
 * ### Meson
 *
 * GSettings is natively supported by Meson’s [GNOME module](https://mesonbuild.com/Gnome-module.html).
 *
 * You can install the schemas as any other data file:
 *
 * ```
 * install_data(
 *   'org.foo.MyApp.gschema.xml',
 *   install_dir: get_option('datadir') / 'glib-2.0/schemas',
 * )
 * ```
 *
 * You can use `gnome.post_install()` function to compile the schemas on
 * installation:
 *
 * ```
 * gnome = import('gnome')
 * gnome.post_install(
 *   glib_compile_schemas: true,
 * )
 * ```
 *
 * If an enumerated type defined in a C header file is to be used in a GSettings
 * schema, it can either be defined manually using an `<enum>` element in the
 * schema XML, or it can be extracted automatically from the C header. This
 * approach is preferred, as it ensures the two representations are always
 * synchronised. To do so, you will need to use the `gnome.mkenums()` function
 * with the following templates:
 *
 * ```
 * schemas_enums = gnome.mkenums('org.foo.MyApp.enums.xml',
 *   comments: '<!-- @comment@ -->',
 *   fhead: '<schemalist>',
 *   vhead: '  <@type@ id="org.foo.MyApp.@EnumName@">',
 *   vprod: '    <value nick="@valuenick@" value="@valuenum@"/>',
 *   vtail: '  </@type@>',
 *   ftail: '</schemalist>',
 *   sources: enum_sources,
 *   install_header: true,
 *   install_dir: get_option('datadir') / 'glib-2.0/schemas',
 * )
 * ```
 *
 * It is recommended to validate your schemas as part of the test suite for
 * your application:
 *
 * ```
 * test('validate-schema',
 *   find_program('glib-compile-schemas'),
 *   args: ['--strict', '--dry-run', meson.current_source_dir()],
 * )
 * ```
 *
 * If your application allows running uninstalled, you should also use the
 * `gnome.compile_schemas()` function to compile the schemas in the current
 * build directory:
 *
 * ```
 * gnome.compile_schemas()
 * ```
 *
 * ### Autotools
 *
 * GSettings comes with autotools integration to simplify compiling and
 * installing schemas. To add GSettings support to an application, add the
 * following to your `configure.ac`:
 * ```
 * GLIB_GSETTINGS
 * ```
 *
 * In the appropriate `Makefile.am`, use the following snippet to compile and
 * install the named schema:
 * ```
 * gsettings_SCHEMAS = org.foo.MyApp.gschema.xml
 * EXTRA_DIST = $(gsettings_SCHEMAS)
 *
 * @GSETTINGS_RULES@
 * ```
 *
 * If an enumerated type defined in a C header file is to be used in a GSettings
 * schema, it can either be defined manually using an `<enum>` element in the
 * schema XML, or it can be extracted automatically from the C header. This
 * approach is preferred, as it ensures the two representations are always
 * synchronised. To do so, add the following to the relevant `Makefile.am`:
 * ```
 * gsettings_ENUM_NAMESPACE = org.foo.MyApp
 * gsettings_ENUM_FILES = my-app-enums.h my-app-misc.h
 * ```
 *
 * `gsettings_ENUM_NAMESPACE` specifies the schema namespace for the enum files,
 * which are specified in `gsettings_ENUM_FILES`. This will generate a
 * `org.foo.MyApp.enums.xml` file containing the extracted enums, which will be
 * automatically included in the schema compilation, install and uninstall
 * rules. It should not be committed to version control or included in
 * `EXTRA_DIST`.
 *
 * ## Localization
 *
 * No changes are needed to the build system to mark a schema XML file for
 * translation. Assuming it sets the `gettext-domain` attribute, a schema may
 * be marked for translation by adding it to `POTFILES.in`, assuming gettext
 * 0.19 or newer is in use (the preferred method for translation):
 * ```
 * data/org.foo.MyApp.gschema.xml
 * ```
 *
 * Alternatively, if intltool 0.50.1 is in use:
 * ```
 * [type: gettext/gsettings]data/org.foo.MyApp.gschema.xml
 * ```
 *
 * GSettings will use gettext to look up translations for the `<summary>` and
 * `<description>` elements, and also any `<default>` elements which have a
 * `l10n` attribute set.
 *
 * Translations **must not** be included in the `.gschema.xml` file by the build
 * system, for example by using a rule to generate the XML file from a template.
 */

struct _GSettingsPrivate
{
  /* where the signals go... */
  GMainContext *main_context;

  GSettingsBackend *backend;
  GSettingsSchema *schema;
  gchar *path;
};

enum
{
  PROP_0,
  PROP_SCHEMA,
  PROP_SCHEMA_ID,
  PROP_BACKEND,
  PROP_PATH,
  PROP_HAS_UNAPPLIED,
  PROP_DELAY_APPLY
};

enum
{
  SIGNAL_WRITABLE_CHANGE_EVENT,
  SIGNAL_WRITABLE_CHANGED,
  SIGNAL_CHANGE_EVENT,
  SIGNAL_CHANGED,
  N_SIGNALS
};

static guint g_settings_signals[N_SIGNALS];

G_DEFINE_TYPE_WITH_PRIVATE (GSettings, g_settings, G_TYPE_OBJECT)

/* Signals {{{1 */
static gboolean
g_settings_real_change_event (GSettings    *settings,
                              const GQuark *keys,
                              gint          n_keys)
{
  gint i;

  if (keys == NULL)
    keys = g_settings_schema_list (settings->priv->schema, &n_keys);

  for (i = 0; i < n_keys; i++)
    {
      const gchar *key = g_quark_to_string (keys[i]);

      if (g_str_has_suffix (key, "/"))
        continue;

      g_signal_emit (settings, g_settings_signals[SIGNAL_CHANGED], keys[i], key);
    }

  return FALSE;
}

static gboolean
g_settings_real_writable_change_event (GSettings *settings,
                                       GQuark     key)
{
  const GQuark *keys = &key;
  gint n_keys = 1;
  gint i;

  if (key == 0)
    keys = g_settings_schema_list (settings->priv->schema, &n_keys);

  for (i = 0; i < n_keys; i++)
    {
      const gchar *key_name = g_quark_to_string (keys[i]);

      if (g_str_has_suffix (key_name, "/"))
        continue;

      g_signal_emit (settings, g_settings_signals[SIGNAL_WRITABLE_CHANGED], keys[i], key_name);
    }

  return FALSE;
}

static void
settings_backend_changed (GObject             *target,
                          GSettingsBackend    *backend,
                          const gchar         *key,
                          gpointer             origin_tag)
{
  GSettings *settings = G_SETTINGS (target);
  gboolean ignore_this;
  gint i;

  /* We used to assert here:
   *
   *   settings->priv->backend == backend
   *
   * but it could be the case that a notification is queued for delivery
   * while someone calls g_settings_delay() (which changes the backend).
   *
   * Since the delay backend would just pass that straight through
   * anyway, it doesn't make sense to try to detect this case.
   * Therefore, we just accept it.
   */

  for (i = 0; key[i] == settings->priv->path[i]; i++);

  if (settings->priv->path[i] == '\0' &&
      g_settings_schema_has_key (settings->priv->schema, key + i))
    {
      GQuark quark;

      quark = g_quark_from_string (key + i);
      g_signal_emit (settings, g_settings_signals[SIGNAL_CHANGE_EVENT],
                     0, &quark, 1, &ignore_this);
    }
}

static void
settings_backend_path_changed (GObject          *target,
                               GSettingsBackend *backend,
                               const gchar      *path,
                               gpointer          origin_tag)
{
  GSettings *settings = G_SETTINGS (target);
  gboolean ignore_this;

  if (g_str_has_prefix (settings->priv->path, path))
    g_signal_emit (settings, g_settings_signals[SIGNAL_CHANGE_EVENT],
                   0, NULL, 0, &ignore_this);
}

static void
settings_backend_keys_changed (GObject             *target,
                               GSettingsBackend    *backend,
                               const gchar         *path,
                               gpointer             origin_tag,
                               const gchar * const *items)
{
  GSettings *settings = G_SETTINGS (target);
  gboolean ignore_this;
  gint i;

  for (i = 0; settings->priv->path[i] &&
              settings->priv->path[i] == path[i]; i++);

  if (path[i] == '\0')
    {
      GQuark quarks[256];
      gint j, l = 0;

      for (j = 0; items[j]; j++)
         {
           const gchar *item = items[j];
           gint k;

           for (k = 0; item[k] == settings->priv->path[i + k]; k++);

           if (settings->priv->path[i + k] == '\0' &&
               g_settings_schema_has_key (settings->priv->schema, item + k))
             quarks[l++] = g_quark_from_string (item + k);

           /* "256 quarks ought to be enough for anybody!"
            * If this bites you, I'm sorry.  Please file a bug.
            */
           g_assert (l < 256);
         }

      if (l > 0)
        g_signal_emit (settings, g_settings_signals[SIGNAL_CHANGE_EVENT],
                       0, quarks, l, &ignore_this);
    }
}

static void
settings_backend_writable_changed (GObject          *target,
                                   GSettingsBackend *backend,
                                   const gchar      *key)
{
  GSettings *settings = G_SETTINGS (target);
  gboolean ignore_this;
  gint i;

  for (i = 0; key[i] == settings->priv->path[i]; i++);

  if (settings->priv->path[i] == '\0' &&
      g_settings_schema_has_key (settings->priv->schema, key + i))
    g_signal_emit (settings, g_settings_signals[SIGNAL_WRITABLE_CHANGE_EVENT],
                   0, g_quark_from_string (key + i), &ignore_this);
}

static void
settings_backend_path_writable_changed (GObject          *target,
                                        GSettingsBackend *backend,
                                        const gchar      *path)
{
  GSettings *settings = G_SETTINGS (target);
  gboolean ignore_this;

  if (g_str_has_prefix (settings->priv->path, path))
    g_signal_emit (settings, g_settings_signals[SIGNAL_WRITABLE_CHANGE_EVENT],
                   0, (GQuark) 0, &ignore_this);
}

/* Properties, Construction, Destruction {{{1 */
static void
g_settings_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GSettings *settings = G_SETTINGS (object);

  switch (prop_id)
    {
    case PROP_SCHEMA:
      {
        GSettingsSchema *schema;

        schema = g_value_dup_boxed (value);

        /* we receive a set_property() call for "settings-schema" even
         * if it was not specified (ie: with NULL value).  ->schema
         * could already be set at this point (ie: via "schema-id").
         * check for NULL to avoid clobbering the existing value.
         */
        if (schema != NULL)
          {
            g_assert (settings->priv->schema == NULL);
            settings->priv->schema = schema;
          }
      }
      break;

    case PROP_SCHEMA_ID:
      {
        const gchar *schema_id;

        schema_id = g_value_get_string (value);

        /* we receive a set_property() call for both "schema" and
         * "schema-id", even if they are not set.  Hopefully only one of
         * them is non-NULL.
         */
        if (schema_id != NULL)
          {
            GSettingsSchemaSource *default_source;

            g_assert (settings->priv->schema == NULL);
            default_source = g_settings_schema_source_get_default ();

            if (default_source == NULL)
              g_error ("No GSettings schemas are installed on the system");

            settings->priv->schema = g_settings_schema_source_lookup (default_source, schema_id, TRUE);

            if (settings->priv->schema == NULL)
              g_error ("Settings schema '%s' is not installed", schema_id);
          }
      }
      break;

    case PROP_PATH:
      settings->priv->path = g_value_dup_string (value);
      break;

    case PROP_BACKEND:
      settings->priv->backend = g_value_dup_object (value);
      break;

    default:
      g_assert_not_reached ();
    }
}

static void
g_settings_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GSettings *settings = G_SETTINGS (object);

  switch (prop_id)
    {
    case PROP_SCHEMA:
      g_value_set_boxed (value, settings->priv->schema);
      break;

     case PROP_SCHEMA_ID:
      g_value_set_string (value, g_settings_schema_get_id (settings->priv->schema));
      break;

     case PROP_BACKEND:
      g_value_set_object (value, settings->priv->backend);
      break;

     case PROP_PATH:
      g_value_set_string (value, settings->priv->path);
      break;

     case PROP_HAS_UNAPPLIED:
      g_value_set_boolean (value, g_settings_get_has_unapplied (settings));
      break;

     case PROP_DELAY_APPLY:
      g_value_set_boolean (value, G_IS_DELAYED_SETTINGS_BACKEND (settings->priv->backend));
      break;

     default:
      g_assert_not_reached ();
    }
}

static const GSettingsListenerVTable listener_vtable = {
  settings_backend_changed,
  settings_backend_path_changed,
  settings_backend_keys_changed,
  settings_backend_writable_changed,
  settings_backend_path_writable_changed
};

static void
g_settings_constructed (GObject *object)
{
  GSettings *settings = G_SETTINGS (object);
  const gchar *schema_path;

  schema_path = g_settings_schema_get_path (settings->priv->schema);

  if (settings->priv->path && schema_path && strcmp (settings->priv->path, schema_path) != 0)
    g_error ("settings object created with schema '%s' and path '%s', but path '%s' is specified by schema",
             g_settings_schema_get_id (settings->priv->schema), settings->priv->path, schema_path);

  if (settings->priv->path == NULL)
    {
      if (schema_path == NULL)
        g_error ("attempting to create schema '%s' without a path",
                 g_settings_schema_get_id (settings->priv->schema));

      settings->priv->path = g_strdup (schema_path);
    }

  if (settings->priv->backend == NULL)
    settings->priv->backend = g_settings_backend_get_default ();

  g_settings_backend_watch (settings->priv->backend,
                            &listener_vtable, G_OBJECT (settings),
                            settings->priv->main_context);
  g_settings_backend_subscribe (settings->priv->backend,
                                settings->priv->path);
}

static void
g_settings_finalize (GObject *object)
{
  GSettings *settings = G_SETTINGS (object);

  g_settings_backend_unsubscribe (settings->priv->backend,
                                  settings->priv->path);
  g_main_context_unref (settings->priv->main_context);
  g_object_unref (settings->priv->backend);
  g_settings_schema_unref (settings->priv->schema);
  g_free (settings->priv->path);

  G_OBJECT_CLASS (g_settings_parent_class)->finalize (object);
}

static void
g_settings_init (GSettings *settings)
{
  settings->priv = g_settings_get_instance_private (settings);
  settings->priv->main_context = g_main_context_ref_thread_default ();
}

static void
g_settings_class_init (GSettingsClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  class->writable_change_event = g_settings_real_writable_change_event;
  class->change_event = g_settings_real_change_event;

  object_class->set_property = g_settings_set_property;
  object_class->get_property = g_settings_get_property;
  object_class->constructed = g_settings_constructed;
  object_class->finalize = g_settings_finalize;

  /**
   * GSettings::changed:
   * @settings: the object on which the signal was emitted
   * @key: the name of the key that changed
   *
   * Emitted when a key has potentially changed.
   *
   * You should call one of the [method@Gio.Settings.get] calls to check the new
   * value.
   *
   * This signal supports detailed connections.  You can connect to the
   * detailed signal `changed::x` in order to only receive callbacks
   * when key `x` changes.
   *
   * Note that @settings only emits this signal if you have read @key at
   * least once while a signal handler was already connected for @key.
   */
  g_settings_signals[SIGNAL_CHANGED] =
    g_signal_new (I_("changed"), G_TYPE_SETTINGS,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  G_STRUCT_OFFSET (GSettingsClass, changed),
                  NULL, NULL, NULL, G_TYPE_NONE,
                  1, G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * GSettings::change-event:
   * @settings: the object on which the signal was emitted
   * @keys: (array length=n_keys) (element-type GQuark) (nullable): array of the
   *   keys which have changed
   * @n_keys: the length of the @keys array, or `0`
   *
   * Emitted once per change event that affects this settings object.
   *
   * You should connect to this signal
   * only if you are interested in viewing groups of changes before they
   * are split out into multiple emissions of the [signal@Gio.Settings::changed] signal.
   * For most use cases it is more appropriate to use the [signal@Gio.Settings::changed] signal.
   *
   * In the event that the change event applies to one or more specified
   * keys, @keys will be an array of [alias@GLib.Quark]s of length @n_keys.  In the
   * event that the change event applies to the [class@Gio.Settings] object as a
   * whole (ie: potentially every key has been changed) then @keys will
   * be `NULL` and @n_keys will be `0`.
   *
   * The default handler for this signal invokes the [signal@Gio.Settings::changed] signal
   * for each affected key.  If any other connected handler returns
   * true then this default functionality will be suppressed.
   *
   * Returns: true to stop other handlers from being invoked for the
   *   event, false to propagate the event further
   */
  g_settings_signals[SIGNAL_CHANGE_EVENT] =
    g_signal_new (I_("change-event"), G_TYPE_SETTINGS,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GSettingsClass, change_event),
                  g_signal_accumulator_true_handled, NULL,
                  _g_cclosure_marshal_BOOLEAN__POINTER_INT,
                  G_TYPE_BOOLEAN, 2, G_TYPE_POINTER, G_TYPE_INT);
  g_signal_set_va_marshaller (g_settings_signals[SIGNAL_CHANGE_EVENT],
                              G_TYPE_FROM_CLASS (class),
                              _g_cclosure_marshal_BOOLEAN__POINTER_INTv);

  /**
   * GSettings::writable-changed:
   * @settings: the object on which the signal was emitted
   * @key: the key
   *
   * Emitted when the writability of a key has potentially changed.
   *
   * You should call [method@Gio.Settings.is_writable] in order to determine the
   * new status.
   *
   * This signal supports detailed connections.  You can connect to the
   * detailed signal `writable-changed::x` in order to only receive
   * callbacks when the writability of `x` changes.
   */
  g_settings_signals[SIGNAL_WRITABLE_CHANGED] =
    g_signal_new (I_("writable-changed"), G_TYPE_SETTINGS,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  G_STRUCT_OFFSET (GSettingsClass, writable_changed),
                  NULL, NULL, NULL, G_TYPE_NONE,
                  1, G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * GSettings::writable-change-event:
   * @settings: the object on which the signal was emitted
   * @key: the quark of the key, or `0`
   *
   * Emitted once per writability change event that affects this settings object.
   *
   * You should connect
   * to this signal if you are interested in viewing groups of changes
   * before they are split out into multiple emissions of the
   * [signal@Gio.Settings::writable-changed] signal.  For most use cases it is more
   * appropriate to use the [signal@Gio.Settings::writable-changed] signal.
   *
   * In the event that the writability change applies only to a single
   * key, @key will be set to the [alias@GLib.Quark] for that key.  In the event
   * that the writability change affects the entire settings object,
   * @key will be `0`.
   *
   * The default handler for this signal invokes the [signal@Gio.Settings::writable-changed]
   * and [signal@Gio.Settings::changed] signals for each affected key.  This is done because
   * changes in writability might also imply changes in value (if for
   * example, a new mandatory setting is introduced).  If any other
   * connected handler returns true then this default functionality
   * will be suppressed.
   *
   * Returns: true to stop other handlers from being invoked for the
   *   event, false to propagate the event further
   */
  g_settings_signals[SIGNAL_WRITABLE_CHANGE_EVENT] =
    g_signal_new (I_("writable-change-event"), G_TYPE_SETTINGS,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GSettingsClass, writable_change_event),
                  g_signal_accumulator_true_handled, NULL,
                  _g_cclosure_marshal_BOOLEAN__UINT,
                  G_TYPE_BOOLEAN, 1, G_TYPE_UINT);
  g_signal_set_va_marshaller (g_settings_signals[SIGNAL_WRITABLE_CHANGE_EVENT],
                              G_TYPE_FROM_CLASS (class),
                              _g_cclosure_marshal_BOOLEAN__UINTv);

  /**
   * GSettings:backend:
   *
   * The name of the context that the settings are stored in.
   */
  g_object_class_install_property (object_class, PROP_BACKEND,
    g_param_spec_object ("backend", NULL, NULL,
                         G_TYPE_SETTINGS_BACKEND, G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  /**
   * GSettings:settings-schema:
   *
   * The [struct@Gio.SettingsSchema] describing the types of keys for this
   * [class@Gio.Settings] object.
   *
   * Ideally, this property would be called [property@Gio.Settings:schema].
   * [struct@Gio.SettingsSchema]
   * has only existed since version 2.32, however, and before then the
   * [property@Gio.Settings:schema] property was used to refer to the ID of the schema rather
   * than the schema itself.  Take care.
   */
  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_boxed ("settings-schema", NULL, NULL,
                        G_TYPE_SETTINGS_SCHEMA,
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  /**
   * GSettings:schema:
   *
   * The name of the schema that describes the types of keys
   * for this [class@Gio.Settings] object.
   *
   * The type of this property is *not* [struct@Gio.SettingsSchema].
   * [struct@Gio.SettingsSchema] has only existed since version 2.32 and
   * unfortunately this name was used in previous versions to refer to
   * the schema ID rather than the schema itself.  Take care to use the
   * [property@Gio.Settings:settings-schema] property if you wish to pass in a
   * [struct@Gio.SettingsSchema].
   *
   * Deprecated:2.32:Use the [property@Gio.Settings:schema-id] property instead.
   *   In a future version, this property may instead refer to a [struct@Gio.SettingsSchema].
   */
  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema", NULL, NULL,
                         NULL,
                         G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_DEPRECATED | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  /**
   * GSettings:schema-id:
   *
   * The name of the schema that describes the types of keys
   * for this [class@Gio.Settings] object.
   */
  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema-id", NULL, NULL,
                         NULL,
                         G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

   /**
    * GSettings:path:
    *
    * The path within the backend where the settings are stored.
    */
   g_object_class_install_property (object_class, PROP_PATH,
     g_param_spec_string ("path", NULL, NULL,
                          NULL,
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

   /**
    * GSettings:has-unapplied:
    *
    * Whether the [class@Gio.Settings] object has outstanding changes.
    *
    * These changes will be applied when [method@Gio.Settings.apply] is called.
    */
   g_object_class_install_property (object_class, PROP_HAS_UNAPPLIED,
     g_param_spec_boolean ("has-unapplied", NULL, NULL,
                           FALSE,
                           G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

   /**
    * GSettings:delay-apply:
    *
    * Whether the [class@Gio.Settings] object is in
    * [‘delay-apply’ mode](class.Settings.html#delay-apply-mode).
    *
    * Since: 2.28
    */
   g_object_class_install_property (object_class, PROP_DELAY_APPLY,
     g_param_spec_boolean ("delay-apply", NULL, NULL,
                           FALSE,
                           G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

/* Construction (new, new_with_path, etc.) {{{1 */
/**
 * g_settings_new:
 * @schema_id: the ID of the schema
 *
 * Creates a new [class@Gio.Settings] object with the schema specified by
 * @schema_id.
 *
 * It is an error for the schema to not exist: schemas are an
 * essential part of a program, as they provide type information.
 * If schemas need to be dynamically loaded (for example, from an
 * optional runtime dependency), [method@Gio.SettingsSchemaSource.lookup]
 * can be used to test for their existence before loading them.
 *
 * Signals on the newly created [class@Gio.Settings] object will be dispatched
 * via the thread-default [struct@GLib.MainContext] in effect at the time of the
 * call to [ctor@Gio.Settings.new].  The new [class@Gio.Settings] will hold a reference
 * on the context.  See [method@GLib.MainContext.push_thread_default].
 *
 * Returns: (not nullable) (transfer full): a new [class@Gio.Settings] object
 *
 * Since: 2.26
 */
GSettings *
g_settings_new (const gchar *schema_id)
{
  g_return_val_if_fail (schema_id != NULL, NULL);

  return g_object_new (G_TYPE_SETTINGS,
                       "schema-id", schema_id,
                       NULL);
}

static gboolean
path_is_valid (const gchar *path)
{
  if (!path)
    return FALSE;

  if (path[0] != '/')
    return FALSE;

  if (!g_str_has_suffix (path, "/"))
    return FALSE;

  return strstr (path, "//") == NULL;
}

/**
 * g_settings_new_with_path:
 * @schema_id: the ID of the schema
 * @path: the path to use
 *
 * Creates a new [class@Gio.Settings] object with the relocatable schema specified
 * by @schema_id and a given path.
 *
 * You only need to do this if you want to directly create a settings
 * object with a schema that doesn’t have a specified path of its own.
 * That’s quite rare.
 *
 * It is a programmer error to call this function for a schema that
 * has an explicitly specified path.
 *
 * It is a programmer error if @path is not a valid path.  A valid path
 * begins and ends with `/` and does not contain two consecutive `/`
 * characters.
 *
 * Returns: (not nullable) (transfer full): a new [class@Gio.Settings] object
 *
 * Since: 2.26
 */
GSettings *
g_settings_new_with_path (const gchar *schema_id,
                          const gchar *path)
{
  g_return_val_if_fail (schema_id != NULL, NULL);
  g_return_val_if_fail (path_is_valid (path), NULL);

  return g_object_new (G_TYPE_SETTINGS,
                       "schema-id", schema_id,
                       "path", path,
                       NULL);
}

/**
 * g_settings_new_with_backend:
 * @schema_id: the ID of the schema
 * @backend: the settings backend to use
 *
 * Creates a new [class@Gio.Settings] object with the schema specified by
 * @schema_id and a given [class@Gio.SettingsBackend].
 *
 * Creating a [class@Gio.Settings] object with a different backend allows accessing
 * settings from a database other than the usual one. For example, it may make
 * sense to pass a backend corresponding to the ‘defaults’ settings database on
 * the system to get a settings object that modifies the system default
 * settings instead of the settings for this user.
 *
 * Returns: (not nullable) (transfer full): a new [class@Gio.Settings] object
 *
 * Since: 2.26
 */
GSettings *
g_settings_new_with_backend (const gchar      *schema_id,
                             GSettingsBackend *backend)
{
  g_return_val_if_fail (schema_id != NULL, NULL);
  g_return_val_if_fail (G_IS_SETTINGS_BACKEND (backend), NULL);

  return g_object_new (G_TYPE_SETTINGS,
                       "schema-id", schema_id,
                       "backend", backend,
                       NULL);
}

/**
 * g_settings_new_with_backend_and_path:
 * @schema_id: the ID of the schema
 * @backend: the settings backend to use
 * @path: the path to use
 *
 * Creates a new [class@Gio.Settings] object with the schema specified by
 * @schema_id and a given [class@Gio.SettingsBackend] and path.
 *
 * This is a mix of [ctor@Gio.Settings.new_with_backend] and
 * [ctor@Gio.Settings.new_with_path].
 *
 * Returns: (not nullable) (transfer full): a new [class@Gio.Settings] object
 *
 * Since: 2.26
 */
GSettings *
g_settings_new_with_backend_and_path (const gchar      *schema_id,
                                      GSettingsBackend *backend,
                                      const gchar      *path)
{
  g_return_val_if_fail (schema_id != NULL, NULL);
  g_return_val_if_fail (G_IS_SETTINGS_BACKEND (backend), NULL);
  g_return_val_if_fail (path_is_valid (path), NULL);

  return g_object_new (G_TYPE_SETTINGS,
                       "schema-id", schema_id,
                       "backend", backend,
                       "path", path,
                       NULL);
}

/**
 * g_settings_new_full:
 * @schema: the schema describing the settings
 * @backend: (nullable): the settings backend to use
 * @path: (nullable): the path to use
 *
 * Creates a new [class@Gio.Settings] object with a given schema, backend and
 * path.
 *
 * It should be extremely rare that you ever want to use this function.
 * It is made available for advanced use-cases (such as plugin systems
 * that want to provide access to schemas loaded from custom locations,
 * etc).
 *
 * At the most basic level, a [class@Gio.Settings] object is a pure composition of
 * four things: a [struct@Gio.SettingsSchema], a [class@Gio.SettingsBackend], a path within that
 * backend, and a [struct@GLib.MainContext] to which signals are dispatched.
 *
 * This constructor therefore gives you full control over constructing
 * [class@Gio.Settings] instances.  The first 3 parameters are given directly as
 * @schema, @backend and @path, and the main context is taken from the
 * thread-default (as per [ctor@Gio.Settings.new]).
 *
 * If @backend is `NULL` then the default backend is used.
 *
 * If @path is `NULL` then the path from the schema is used.  It is an
 * error if @path is `NULL` and the schema has no path of its own or if
 * @path is non-`NULL` and not equal to the path that the schema does
 * have.
 *
 * Returns: (not nullable) (transfer full): a new [class@Gio.Settings] object
 *
 * Since: 2.32
 */
GSettings *
g_settings_new_full (GSettingsSchema  *schema,
                     GSettingsBackend *backend,
                     const gchar      *path)
{
  g_return_val_if_fail (schema != NULL, NULL);
  g_return_val_if_fail (backend == NULL || G_IS_SETTINGS_BACKEND (backend), NULL);
  g_return_val_if_fail (path == NULL || path_is_valid (path), NULL);

  return g_object_new (G_TYPE_SETTINGS,
                       "settings-schema", schema,
                       "backend", backend,
                       "path", path,
                       NULL);
}

/* Internal read/write utilities {{{1 */

/* @value will be sunk */
static gboolean
g_settings_write_to_backend (GSettings          *settings,
                             GSettingsSchemaKey *key,
                             GVariant           *value)
{
  gboolean success;
  gchar *path;

  path = g_strconcat (settings->priv->path, key->name, NULL);
  success = g_settings_backend_write (settings->priv->backend, path, value, NULL);
  g_free (path);

  return success;
}

static GVariant *
g_settings_read_from_backend (GSettings          *settings,
                              GSettingsSchemaKey *key,
                              gboolean            user_value_only,
                              gboolean            default_value)
{
  GVariant *value;
  GVariant *fixup;
  gchar *path;

  path = g_strconcat (settings->priv->path, key->name, NULL);
  if (user_value_only)
    value = g_settings_backend_read_user_value (settings->priv->backend, path, key->type);
  else
    value = g_settings_backend_read (settings->priv->backend, path, key->type, default_value);
  g_free (path);

  if (value != NULL)
    {
      fixup = g_settings_schema_key_range_fixup (key, value);
      g_variant_unref (value);
    }
  else
    fixup = NULL;

  return fixup;
}

/* Public Get/Set API {{{1 (get, get_value, set, set_value, get_mapped) */
/**
 * g_settings_get_value:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored in @settings for @key.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings.
 *
 * Returns: (not nullable) (transfer full): a new [struct@GLib.Variant]
 *
 * Since: 2.26
 */
GVariant *
g_settings_get_value (GSettings   *settings,
                      const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *value;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);
  value = g_settings_read_from_backend (settings, &skey, FALSE, FALSE);

  if (value == NULL)
    value = g_settings_schema_key_get_default_value (&skey);

  g_settings_schema_key_clear (&skey);

  return value;
}

/**
 * g_settings_get_user_value:
 * @settings: the settings object
 * @key: the key to get the user value for
 *
 * Checks the ‘user value’ of a key, if there is one.
 *
 * The user value of a key is the last value that was set by the user.
 *
 * After calling [method@Gio.Settings.reset] this function should always return
 * `NULL` (assuming something is not wrong with the system
 * configuration).
 *
 * It is possible that [method@Gio.Settings.get_value] will return a different
 * value than this function.  This can happen in the case that the user
 * set a value for a key that was subsequently locked down by the system
 * administrator — this function will return the user’s old value.
 *
 * This function may be useful for adding a ‘reset’ option to a UI or
 * for providing indication that a particular value has been changed.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings.
 *
 * Returns: (nullable) (transfer full): the user’s value, if set
 *
 * Since: 2.40
 **/
GVariant *
g_settings_get_user_value (GSettings   *settings,
                           const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *value;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);
  value = g_settings_read_from_backend (settings, &skey, TRUE, FALSE);
  g_settings_schema_key_clear (&skey);

  return value;
}

/**
 * g_settings_get_default_value:
 * @settings: the settings object
 * @key: the key to get the default value for
 *
 * Gets the ‘default value’ of a key.
 *
 * This is the value that would be read if [method@Gio.Settings.reset] were to be
 * called on the key.
 *
 * Note that this may be a different value than returned by
 * [method@Gio.SettingsSchemaKey.get_default_value] if the system administrator
 * has provided a default value.
 *
 * Comparing the return values of [method@Gio.Settings.get_default_value] and
 * [method@Gio.Settings.get_value] is not sufficient for determining if a value
 * has been set because the user may have explicitly set the value to
 * something that happens to be equal to the default.  The difference
 * here is that if the default changes in the future, the user’s key
 * will still be set.
 *
 * This function may be useful for adding an indication to a UI of what
 * the default value was before the user set it.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings.
 *
 * Returns: (nullable) (transfer full): the default value
 *
 * Since: 2.40
 **/
GVariant *
g_settings_get_default_value (GSettings   *settings,
                              const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *value;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);
  value = g_settings_read_from_backend (settings, &skey, FALSE, TRUE);

  if (value == NULL)
    value = g_settings_schema_key_get_default_value (&skey);

  g_settings_schema_key_clear (&skey);

  return value;
}

/**
 * g_settings_get_enum:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored in @settings for @key and converts it
 * to the enum value that it represents.
 *
 * In order to use this function the type of the value must be a string
 * and it must be marked in the schema file as an enumerated type.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or is not marked as an enumerated type.
 *
 * If the value stored in the configuration database is not a valid
 * value for the enumerated type then this function will return the
 * default value.
 *
 * Returns: the enum value
 *
 * Since: 2.26
 **/
gint
g_settings_get_enum (GSettings   *settings,
                     const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *value;
  gint result;

  g_return_val_if_fail (G_IS_SETTINGS (settings), -1);
  g_return_val_if_fail (key != NULL, -1);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if (!skey.is_enum)
    {
      g_critical ("g_settings_get_enum() called on key '%s' which is not "
                  "associated with an enumerated type", skey.name);
      g_settings_schema_key_clear (&skey);
      return -1;
    }

  value = g_settings_read_from_backend (settings, &skey, FALSE, FALSE);

  if (value == NULL)
    value = g_settings_schema_key_get_default_value (&skey);

  result = g_settings_schema_key_to_enum (&skey, value);
  g_settings_schema_key_clear (&skey);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_enum:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: an enumerated value
 *
 * Looks up the enumerated type nick for @value and writes it to @key,
 * within @settings.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or is not marked as an enumerated type, or for
 * @value not to be a valid value for the named type.
 *
 * After performing the write, accessing @key directly with
 * [method@Gio.Settings.get_string] will return the ‘nick’ associated with
 * @value.
 *
 * Returns: true if the set succeeds, false otherwise
 **/
gboolean
g_settings_set_enum (GSettings   *settings,
                     const gchar *key,
                     gint         value)
{
  GSettingsSchemaKey skey;
  GVariant *variant;
  gboolean success;

  g_return_val_if_fail (G_IS_SETTINGS (settings), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if (!skey.is_enum)
    {
      g_critical ("g_settings_set_enum() called on key '%s' which is not "
                  "associated with an enumerated type", skey.name);
      return FALSE;
    }

  if (!(variant = g_settings_schema_key_from_enum (&skey, value)))
    {
      g_critical ("g_settings_set_enum(): invalid enum value %d for key '%s' "
                  "in schema '%s'.  Doing nothing.", value, skey.name,
                  g_settings_schema_get_id (skey.schema));
      g_settings_schema_key_clear (&skey);
      return FALSE;
    }

  success = g_settings_write_to_backend (settings, &skey, g_steal_pointer (&variant));
  g_settings_schema_key_clear (&skey);

  return success;
}

/**
 * g_settings_get_flags:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored in @settings for @key and converts it
 * to the flags value that it represents.
 *
 * In order to use this function the type of the value must be an array
 * of strings and it must be marked in the schema file as a flags type.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or is not marked as a flags type.
 *
 * If the value stored in the configuration database is not a valid
 * value for the flags type then this function will return the default
 * value.
 *
 * Returns: the flags value
 *
 * Since: 2.26
 **/
guint
g_settings_get_flags (GSettings   *settings,
                      const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *value;
  guint result;

  g_return_val_if_fail (G_IS_SETTINGS (settings), -1);
  g_return_val_if_fail (key != NULL, -1);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if (!skey.is_flags)
    {
      g_critical ("g_settings_get_flags() called on key '%s' which is not "
                  "associated with a flags type", skey.name);
      g_settings_schema_key_clear (&skey);
      return -1;
    }

  value = g_settings_read_from_backend (settings, &skey, FALSE, FALSE);

  if (value == NULL)
    value = g_settings_schema_key_get_default_value (&skey);

  result = g_settings_schema_key_to_flags (&skey, value);
  g_settings_schema_key_clear (&skey);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_flags:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: a flags value
 *
 * Looks up the flags type nicks for the bits specified by @value, puts
 * them in an array of strings and writes the array to @key, within
 * @settings.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or is not marked as a flags type, or for @value
 * to contain any bits that are not value for the named type.
 *
 * After performing the write, accessing @key directly with
 * [method@Gio.Settings.get_strv] will return an array of ‘nicks’; one for each
 * bit in @value.
 *
 * Returns: true if the set succeeds, false otherwise
 **/
gboolean
g_settings_set_flags (GSettings   *settings,
                      const gchar *key,
                      guint        value)
{
  GSettingsSchemaKey skey;
  GVariant *variant;
  gboolean success;

  g_return_val_if_fail (G_IS_SETTINGS (settings), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if (!skey.is_flags)
    {
      g_critical ("g_settings_set_flags() called on key '%s' which is not "
                  "associated with a flags type", skey.name);
      return FALSE;
    }

  if (!(variant = g_settings_schema_key_from_flags (&skey, value)))
    {
      g_critical ("g_settings_set_flags(): invalid flags value 0x%08x "
                  "for key '%s' in schema '%s'.  Doing nothing.",
                  value, skey.name, g_settings_schema_get_id (skey.schema));
      g_settings_schema_key_clear (&skey);
      return FALSE;
    }

  success = g_settings_write_to_backend (settings, &skey, g_steal_pointer (&variant));
  g_settings_schema_key_clear (&skey);

  return success;
}

/**
 * g_settings_set_value:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: a [struct@GLib.Variant] of the correct type
 *
 * Sets @key in @settings to @value.
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or for @value to have the incorrect type, per
 * the schema.
 *
 * If @value is floating then this function consumes the reference.
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 **/
gboolean
g_settings_set_value (GSettings   *settings,
                      const gchar *key,
                      GVariant    *value)
{
  GSettingsSchemaKey skey;
  gboolean success;

  g_return_val_if_fail (G_IS_SETTINGS (settings), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  g_variant_ref_sink (value);
  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if (!g_settings_schema_key_type_check (&skey, value))
    {
      g_critical ("g_settings_set_value: key '%s' in '%s' expects type '%s', but a GVariant of type '%s' was given",
                  key,
                  g_settings_schema_get_id (settings->priv->schema),
                  g_variant_type_peek_string (skey.type),
                  g_variant_get_type_string (value));
      success = FALSE;
    }
  else if (!g_settings_schema_key_range_check (&skey, value))
    {
      g_warning ("g_settings_set_value: value for key '%s' in schema '%s' "
                 "is outside of valid range",
                 key,
                 g_settings_schema_get_id (settings->priv->schema));
      success = FALSE;
    }
  else
    {
      success = g_settings_write_to_backend (settings, &skey, value);
    }

  g_settings_schema_key_clear (&skey);
  g_variant_unref (value);

  return success;
}

/**
 * g_settings_get:
 * @settings: the settings object
 * @key: the key to get the value for
 * @format: a [struct@GLib.Variant] format string
 * @...: arguments as per @format
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience function that combines [method@Gio.Settings.get_value] with
 * [method@GLib.Variant.get].
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or for the [struct@GLib.VariantType] of @format to mismatch
 * the type given in the schema.
 *
 * Since: 2.26
 */
void
g_settings_get (GSettings   *settings,
                const gchar *key,
                const gchar *format,
                ...)
{
  GVariant *value;
  va_list ap;

  value = g_settings_get_value (settings, key);

  if (strchr (format, '&'))
    {
      g_critical ("%s: the format string may not contain '&' (key '%s' from schema '%s'). "
                  "This call will probably stop working with a future version of glib.",
                  G_STRFUNC, key, g_settings_schema_get_id (settings->priv->schema));
    }

  va_start (ap, format);
  g_variant_get_va (value, format, NULL, &ap);
  va_end (ap);

  g_variant_unref (value);
}

/**
 * g_settings_set:
 * @settings: the settings object
 * @key: the key to set the value for
 * @format: a [struct@GLib.Variant] format string
 * @...: arguments as per @format
 *
 * Sets @key in @settings to @value.
 *
 * A convenience function that combines [method@Gio.Settings.set_value] with
 * [ctor@GLib.Variant.new].
 *
 * It is a programmer error to give a @key that isn’t contained in the
 * schema for @settings or for the [struct@GLib.VariantType] of @format to mismatch
 * the type given in the schema.
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set (GSettings   *settings,
                const gchar *key,
                const gchar *format,
                ...)
{
  GVariant *value;
  va_list ap;

  va_start (ap, format);
  value = g_variant_new_va (format, NULL, &ap);
  va_end (ap);

  return g_settings_set_value (settings, key, g_steal_pointer (&value));
}

/**
 * g_settings_get_mapped:
 * @settings: the settings object
 * @key: the key to get the value for
 * @mapping: (scope call): the function to map the value in the
 *   settings database to the value used by the application
 * @user_data: user data for @mapping
 *
 * Gets the value that is stored at @key in @settings, subject to
 * application-level validation/mapping.
 *
 * You should use this function when the application needs to perform
 * some processing on the value of the key (for example, parsing).  The
 * @mapping function performs that processing.  If the function
 * indicates that the processing was unsuccessful (due to a parse error,
 * for example) then the mapping is tried again with another value.
 *
 * This allows a robust ‘fall back to defaults’ behaviour to be
 * implemented somewhat automatically.
 *
 * The first value that is tried is the user’s setting for the key.  If
 * the mapping function fails to map this value, other values may be
 * tried in an unspecified order (system or site defaults, translated
 * schema default values, untranslated schema default values, etc).
 *
 * If the mapping function fails for all possible values, one additional
 * attempt is made: the mapping function is called with a `NULL` value.
 * If the mapping function still indicates failure at this point then
 * the application will be aborted.
 *
 * The result parameter for the @mapping function is pointed to a
 * `gpointer` which is initially set to `NULL`.  The same pointer is given
 * to each invocation of @mapping.  The final value of that `gpointer` is
 * what is returned by this function.  `NULL` is valid; it is returned
 * just as any other value would be.
 *
 * Returns: (nullable) (transfer full): the result, which may be `NULL`
 **/
gpointer
g_settings_get_mapped (GSettings           *settings,
                       const gchar         *key,
                       GSettingsGetMapping  mapping,
                       gpointer             user_data)
{
  gpointer result = NULL;
  GSettingsSchemaKey skey;
  GVariant *value;
  gboolean okay;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);
  g_return_val_if_fail (mapping != NULL, NULL);

  g_settings_schema_key_init (&skey, settings->priv->schema, key);

  if ((value = g_settings_read_from_backend (settings, &skey, FALSE, FALSE)))
    {
      okay = mapping (value, &result, user_data);
      g_variant_unref (value);
      if (okay) goto okay;
    }

  if ((value = g_settings_schema_key_get_translated_default (&skey)))
    {
      okay = mapping (value, &result, user_data);
      g_variant_unref (value);
      if (okay) goto okay;
    }

  if ((value = g_settings_schema_key_get_per_desktop_default (&skey)))
    {
      okay = mapping (value, &result, user_data);
      g_variant_unref (value);
      if (okay) goto okay;
    }

  if (mapping (skey.default_value, &result, user_data))
    goto okay;

  if (!mapping (NULL, &result, user_data))
    g_error ("The mapping function given to g_settings_get_mapped() for key "
             "'%s' in schema '%s' returned FALSE when given a NULL value.",
             key, g_settings_schema_get_id (settings->priv->schema));

 okay:
  g_settings_schema_key_clear (&skey);

  return result;
}

/* Convenience API (get, set_string, int, double, boolean, strv) {{{1 */
/**
 * g_settings_get_string:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for strings.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `s` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: (not nullable) (transfer full): a newly-allocated string
 *
 * Since: 2.26
 */
gchar *
g_settings_get_string (GSettings   *settings,
                       const gchar *key)
{
  GVariant *value;
  gchar *result;

  value = g_settings_get_value (settings, key);
  result = g_variant_dup_string (value, NULL);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_string:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for strings.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `s` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set_string (GSettings   *settings,
                       const gchar *key,
                       const gchar *value)
{
  return g_settings_set_value (settings, key, g_variant_new_string (value));
}

/**
 * g_settings_get_int:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for 32-bit integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `i` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: an integer
 *
 * Since: 2.26
 */
gint
g_settings_get_int (GSettings   *settings,
                    const gchar *key)
{
  GVariant *value;
  gint result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_int32 (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_int:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for 32-bit integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `i` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set_int (GSettings   *settings,
                    const gchar *key,
                    gint         value)
{
  return g_settings_set_value (settings, key, g_variant_new_int32 (value));
}

/**
 * g_settings_get_int64:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for 64-bit integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `x` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: a 64-bit integer
 *
 * Since: 2.50
 */
gint64
g_settings_get_int64 (GSettings   *settings,
                      const gchar *key)
{
  GVariant *value;
  gint64 result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_int64 (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_int64:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for 64-bit integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `x` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.50
 */
gboolean
g_settings_set_int64 (GSettings   *settings,
                      const gchar *key,
                      gint64       value)
{
  return g_settings_set_value (settings, key, g_variant_new_int64 (value));
}

/**
 * g_settings_get_uint:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for 32-bit unsigned
 * integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `u` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: an unsigned integer
 *
 * Since: 2.30
 */
guint
g_settings_get_uint (GSettings   *settings,
                     const gchar *key)
{
  GVariant *value;
  guint result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_uint32 (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_uint:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for 32-bit unsigned
 * integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `u` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.30
 */
gboolean
g_settings_set_uint (GSettings   *settings,
                     const gchar *key,
                     guint        value)
{
  return g_settings_set_value (settings, key, g_variant_new_uint32 (value));
}

/**
 * g_settings_get_uint64:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for 64-bit unsigned
 * integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `t` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: a 64-bit unsigned integer
 *
 * Since: 2.50
 */
guint64
g_settings_get_uint64 (GSettings   *settings,
                       const gchar *key)
{
  GVariant *value;
  guint64 result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_uint64 (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_uint64:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for 64-bit unsigned
 * integers.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `t` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.50
 */
gboolean
g_settings_set_uint64 (GSettings   *settings,
                       const gchar *key,
                       guint64      value)
{
  return g_settings_set_value (settings, key, g_variant_new_uint64 (value));
}

/**
 * g_settings_get_double:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for doubles.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `d` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: a double
 *
 * Since: 2.26
 */
gdouble
g_settings_get_double (GSettings   *settings,
                       const gchar *key)
{
  GVariant *value;
  gdouble result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_double (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_double:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for doubles.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `d` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set_double (GSettings   *settings,
                       const gchar *key,
                       gdouble      value)
{
  return g_settings_set_value (settings, key, g_variant_new_double (value));
}

/**
 * g_settings_get_boolean:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * Gets the value that is stored at @key in @settings.
 *
 * A convenience variant of [method@Gio.Settings.get] for booleans.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `b` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: a boolean
 *
 * Since: 2.26
 */
gboolean
g_settings_get_boolean (GSettings  *settings,
                       const gchar *key)
{
  GVariant *value;
  gboolean result;

  value = g_settings_get_value (settings, key);
  result = g_variant_get_boolean (value);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_boolean:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for booleans.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having a `b` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set_boolean (GSettings  *settings,
                       const gchar *key,
                       gboolean     value)
{
  return g_settings_set_value (settings, key, g_variant_new_boolean (value));
}

/**
 * g_settings_get_strv:
 * @settings: the settings object
 * @key: the key to get the value for
 *
 * A convenience variant of [method@Gio.Settings.get] for string arrays.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `as` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: (array zero-terminated=1) (not nullable) (transfer full): a
 *   newly-allocated, `NULL`-terminated array of strings, the value that
 *   is stored at @key in @settings.
 *
 * Since: 2.26
 */
gchar **
g_settings_get_strv (GSettings   *settings,
                     const gchar *key)
{
  GVariant *value;
  gchar **result;

  value = g_settings_get_value (settings, key);
  result = g_variant_dup_strv (value, NULL);
  g_variant_unref (value);

  return result;
}

/**
 * g_settings_set_strv:
 * @settings: the settings object
 * @key: the key to set the value for
 * @value: (nullable) (array zero-terminated=1): the value to set it to
 *
 * Sets @key in @settings to @value.
 *
 * A convenience variant of [method@Gio.Settings.set] for string arrays.  If
 * @value is `NULL`, then @key is set to be the empty array.
 *
 * It is a programmer error to give a @key that isn’t specified as
 * having an `as` type in the schema for @settings (see [struct@GLib.VariantType]).
 *
 * Returns: true if setting the key succeeded,
 *   false if the key was not writable
 *
 * Since: 2.26
 */
gboolean
g_settings_set_strv (GSettings           *settings,
                     const gchar         *key,
                     const gchar * const *value)
{
  GVariant *array;

  if (value != NULL)
    array = g_variant_new_strv (value, -1);
  else
    array = g_variant_new_strv (NULL, 0);

  return g_settings_set_value (settings, key, array);
}

/* Delayed apply (delay, apply, revert, get_has_unapplied) {{{1 */
/**
 * g_settings_delay:
 * @settings: the settings object
 *
 * Changes the [class@Gio.Settings] object into
 * [‘delay-apply’ mode](class.Settings.html#delay-apply-mode).
 *
 * In this
 * mode, changes to @settings are not immediately propagated to the
 * backend, but kept locally until [method@Gio.Settings.apply] is called.
 *
 * Since: 2.26
 */
void
g_settings_delay (GSettings *settings)
{
  GDelayedSettingsBackend *delayed = NULL;

  g_return_if_fail (G_IS_SETTINGS (settings));

  if (G_IS_DELAYED_SETTINGS_BACKEND (settings->priv->backend))
    return;

  delayed = g_delayed_settings_backend_new (settings->priv->backend,
                                            settings,
                                            settings->priv->main_context);
  g_settings_backend_unwatch (settings->priv->backend, G_OBJECT (settings));
  g_object_unref (settings->priv->backend);

  settings->priv->backend = G_SETTINGS_BACKEND (delayed);
  g_settings_backend_watch (settings->priv->backend,
                            &listener_vtable, G_OBJECT (settings),
                            settings->priv->main_context);

  g_object_notify (G_OBJECT (settings), "delay-apply");
}

/**
 * g_settings_apply:
 * @settings: the settings object
 *
 * Applies any changes that have been made to the settings.
 *
 * This function does nothing unless @settings is in
 * [‘delay-apply’ mode](class.Settings.html#delay-apply-mode).  In the normal
 * case settings are always applied immediately.
 **/
void
g_settings_apply (GSettings *settings)
{
  if (G_IS_DELAYED_SETTINGS_BACKEND (settings->priv->backend))
    {
      GDelayedSettingsBackend *delayed;

      delayed = G_DELAYED_SETTINGS_BACKEND (settings->priv->backend);
      g_delayed_settings_backend_apply (delayed);
    }
}

/**
 * g_settings_revert:
 * @settings: the settings object
 *
 * Reverts all unapplied changes to the settings.
 *
 * This function does nothing unless @settings is in
 * [‘delay-apply’ mode](class.Settings.html#delay-apply-mode).  In the normal
 * case settings are always applied immediately.
 *
 * Change notifications will be emitted for affected keys.
 **/
void
g_settings_revert (GSettings *settings)
{
  if (G_IS_DELAYED_SETTINGS_BACKEND (settings->priv->backend))
    {
      GDelayedSettingsBackend *delayed;

      delayed = G_DELAYED_SETTINGS_BACKEND (settings->priv->backend);
      g_delayed_settings_backend_revert (delayed);
    }
}

/**
 * g_settings_get_has_unapplied:
 * @settings: the settings object
 *
 * Returns whether the [class@Gio.Settings] object has any unapplied
 * changes.
 *
 * This can only be the case if it is in
 * [‘delay-apply’ mode](class.Settings.html#delay-apply-mode).
 *
 * Returns: true if @settings has unapplied changes, false otherwise
 *
 * Since: 2.26
 */
gboolean
g_settings_get_has_unapplied (GSettings *settings)
{
  g_return_val_if_fail (G_IS_SETTINGS (settings), FALSE);

  return G_IS_DELAYED_SETTINGS_BACKEND (settings->priv->backend) &&
         g_delayed_settings_backend_get_has_unapplied (
           G_DELAYED_SETTINGS_BACKEND (settings->priv->backend));
}

/* Extra API (reset, sync, get_child, is_writable, list_*, ranges) {{{1 */
/**
 * g_settings_reset:
 * @settings: the settings object
 * @key: the name of a key
 *
 * Resets @key to its default value.
 *
 * This call resets the key, as much as possible, to its default value.
 * That might be the value specified in the schema or the one set by the
 * administrator.
 **/
void
g_settings_reset (GSettings *settings,
                  const gchar *key)
{
  gchar *path;

  g_return_if_fail (G_IS_SETTINGS (settings));
  g_return_if_fail (key != NULL);

  path = g_strconcat (settings->priv->path, key, NULL);
  g_settings_backend_reset (settings->priv->backend, path, NULL);
  g_free (path);
}

/**
 * g_settings_sync:
 *
 * Ensures that all pending operations are complete for the default backend.
 *
 * Writes made to a [class@Gio.Settings] are handled asynchronously.  For this
 * reason, it is very unlikely that the changes have it to disk by the
 * time [method@Gio.Settings.set] returns.
 *
 * This call will block until all of the writes have made it to the
 * backend.  Since the main loop is not running, no change notifications
 * will be dispatched during this call (but some may be queued by the
 * time the call is done).
 **/
void
g_settings_sync (void)
{
  g_settings_backend_sync_default ();
}

/**
 * g_settings_is_writable:
 * @settings: the settings object
 * @name: the name of a key
 *
 * Finds out if a key can be written.
 *
 * Returns: true if the key @name is writable, false otherwise
 *
 * Since: 2.26
 */
gboolean
g_settings_is_writable (GSettings   *settings,
                        const gchar *name)
{
  gboolean writable;
  gchar *path;

  g_return_val_if_fail (G_IS_SETTINGS (settings), FALSE);

  path = g_strconcat (settings->priv->path, name, NULL);
  writable = g_settings_backend_get_writable (settings->priv->backend, path);
  g_free (path);

  return writable;
}

/**
 * g_settings_get_child:
 * @settings: the settings object
 * @name: the name of the child schema
 *
 * Creates a child settings object which has a base path of
 * `base-path/name`, where `base-path` is the base path of
 * @settings and `name` is as specified by the caller.
 *
 * The schema for the child settings object must have been declared
 * in the schema of @settings using a `<child>` element.
 *
 * The created child settings object will inherit the
 * [property@Gio.Settings:delay-apply] mode from @settings.
 *
 * Returns: (not nullable) (transfer full): a ‘child’ settings object
 *
 * Since: 2.26
 */
GSettings *
g_settings_get_child (GSettings   *settings,
                      const gchar *name)
{
  GSettingsSchema *child_schema;
  gchar *child_path;
  GSettings *child;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);

  child_schema = g_settings_schema_get_child_schema (settings->priv->schema,
                                                     name);
  if (child_schema == NULL)
    g_error ("Schema '%s' has no child '%s' or child schema not found",
             g_settings_schema_get_id (settings->priv->schema), name);

  child_path = g_strconcat (settings->priv->path, name, "/", NULL);
  child = g_settings_new_full (child_schema,
                               settings->priv->backend,
                               child_path);
  g_settings_schema_unref (child_schema);
  g_free (child_path);

  return child;
}

/**
 * g_settings_list_keys:
 * @settings: the settings object
 *
 * Introspects the list of keys on @settings.
 *
 * You should probably not be calling this function from ‘normal’ code
 * (since you should already know what keys are in your schema).  This
 * function is intended for introspection reasons.
 *
 * You should free the return value with [func@GLib.strfreev] when you are done
 * with it.
 *
 * Returns: (not nullable) (transfer full) (element-type utf8): a list
 *   of the keys on @settings, in no defined order
 * Deprecated: 2.46: Use [method@Gio.SettingsSchema.list_keys] instead.
 */
gchar **
g_settings_list_keys (GSettings *settings)
{
  return g_settings_schema_list_keys (settings->priv->schema);
}

/**
 * g_settings_list_children:
 * @settings: the settings object
 *
 * Gets the list of children on @settings.
 *
 * The list is exactly the list of strings for which it is not an error
 * to call [method@Gio.Settings.get_child].
 *
 * There is little reason to call this function from ‘normal’ code, since
 * you should already know what children are in your schema. This function
 * may still be useful there for introspection reasons, however.
 *
 * You should free the return value with [func@GLib.strfreev] when you are done
 * with it.
 *
 * Returns: (not nullable) (transfer full) (element-type utf8): a list of the children
 *   on @settings, in no defined order
 */
gchar **
g_settings_list_children (GSettings *settings)
{
  return g_settings_schema_list_children (settings->priv->schema);
}

/**
 * g_settings_get_range:
 * @settings: the settings object
 * @key: the key to query the range of
 *
 * Queries the range of a key.
 *
 * Since: 2.28
 *
 * Deprecated:2.40:Use [method@Gio.SettingsSchemaKey.get_range] instead.
 **/
GVariant *
g_settings_get_range (GSettings   *settings,
                      const gchar *key)
{
  GSettingsSchemaKey skey;
  GVariant *range;

  g_settings_schema_key_init (&skey, settings->priv->schema, key);
  range = g_settings_schema_key_get_range (&skey);
  g_settings_schema_key_clear (&skey);

  return range;
}

/**
 * g_settings_range_check:
 * @settings: the settings object
 * @key: the key to check
 * @value: the value to check
 *
 * Checks if the given @value is of the correct type and within the
 * permitted range for @key.
 *
 * Returns: true if @value is valid for @key, false otherwise
 *
 * Since: 2.28
 *
 * Deprecated:2.40:Use [method@Gio.SettingsSchemaKey.range_check] instead.
 **/
gboolean
g_settings_range_check (GSettings   *settings,
                        const gchar *key,
                        GVariant    *value)
{
  GSettingsSchemaKey skey;
  gboolean good;

  g_settings_schema_key_init (&skey, settings->priv->schema, key);
  good = g_settings_schema_key_range_check (&skey, value);
  g_settings_schema_key_clear (&skey);

  return good;
}

/* Binding {{{1 */
typedef struct
{
  GSettingsSchemaKey key;
  GSettings *settings;
  GObject *object;

  GSettingsBindGetMapping get_mapping;
  GSettingsBindSetMapping set_mapping;
  gpointer user_data;
  GDestroyNotify destroy;

  guint writable_handler_id;
  guint property_handler_id;
  const GParamSpec *property;
  guint key_handler_id;

  /* prevent recursion */
  gboolean running;
} GSettingsBinding;

static void
g_settings_binding_free (gpointer data)
{
  GSettingsBinding *binding = data;

  g_assert (!binding->running);

  if (binding->writable_handler_id)
    g_signal_handler_disconnect (binding->settings,
                                 binding->writable_handler_id);

  if (binding->key_handler_id)
    g_signal_handler_disconnect (binding->settings,
                                 binding->key_handler_id);

  if (g_signal_handler_is_connected (binding->object,
                                     binding->property_handler_id))
  g_signal_handler_disconnect (binding->object,
                               binding->property_handler_id);

  g_settings_schema_key_clear (&binding->key);

  if (binding->destroy)
    binding->destroy (binding->user_data);

  g_object_unref (binding->settings);

  g_slice_free (GSettingsBinding, binding);
}

static GQuark
g_settings_binding_quark (const char *property)
{
  GQuark quark;
  gchar *tmp;

  tmp = g_strdup_printf ("gsettingsbinding-%s", property);
  quark = g_quark_from_string (tmp);
  g_free (tmp);

  return quark;
}

static void
g_settings_binding_key_changed (GSettings   *settings,
                                const gchar *key,
                                gpointer     user_data)
{
  GSettingsBinding *binding = user_data;
  GValue value = G_VALUE_INIT;
  GVariant *variant;

  g_assert (settings == binding->settings);
  g_assert (key == binding->key.name);

  if (binding->running)
    return;

  binding->running = TRUE;

  g_value_init (&value, binding->property->value_type);

  variant = g_settings_read_from_backend (binding->settings, &binding->key, FALSE, FALSE);
  if (variant && !binding->get_mapping (&value, variant, binding->user_data))
    {
      /* silently ignore errors in the user's config database */
      g_variant_unref (variant);
      variant = NULL;
    }

  if (variant == NULL)
    {
      variant = g_settings_schema_key_get_translated_default (&binding->key);
      if (variant &&
          !binding->get_mapping (&value, variant, binding->user_data))
        {
          /* flag translation errors with a warning */
          g_warning ("Translated default '%s' for key '%s' in schema '%s' "
                     "was rejected by the binding mapping function",
                     binding->key.unparsed, binding->key.name,
                     g_settings_schema_get_id (binding->key.schema));
          g_variant_unref (variant);
          variant = NULL;
        }
    }

  if (variant == NULL)
    {
      variant = g_settings_schema_key_get_per_desktop_default (&binding->key);
      if (variant &&
          !binding->get_mapping (&value, variant, binding->user_data))
        {
          g_error ("Per-desktop default value for key '%s' in schema '%s' "
                   "was rejected by the binding mapping function.",
                   binding->key.name, g_settings_schema_get_id (binding->key.schema));
          g_variant_unref (variant);
          variant = NULL;
        }
    }

  if (variant == NULL)
    {
      variant = g_variant_ref (binding->key.default_value);
      if (!binding->get_mapping (&value, variant, binding->user_data))
        g_error ("The schema default value for key '%s' in schema '%s' "
                 "was rejected by the binding mapping function.",
                 binding->key.name, g_settings_schema_get_id (binding->key.schema));
    }

  g_object_set_property (binding->object, binding->property->name, &value);
  g_variant_unref (variant);
  g_value_unset (&value);

  binding->running = FALSE;
}

static void
g_settings_binding_property_changed (GObject          *object,
                                     const GParamSpec *pspec,
                                     gpointer          user_data)
{
  GSettingsBinding *binding = user_data;
  GValue value = G_VALUE_INIT;
  GVariant *variant;
  gboolean valid = TRUE;

  g_assert (object == binding->object);
  g_assert (pspec == binding->property);

  if (binding->running)
    return;

  binding->running = TRUE;

  g_value_init (&value, pspec->value_type);
  g_object_get_property (object, pspec->name, &value);
  if ((variant = binding->set_mapping (&value, binding->key.type,
                                       binding->user_data)))
    {
      g_variant_take_ref (variant);

      if (!g_settings_schema_key_type_check (&binding->key, variant))
        {
          gchar *type_str;
          type_str = g_variant_type_dup_string (binding->key.type);
          g_critical ("binding mapping function for key '%s' returned "
                      "GVariant of type '%s' when type '%s' was requested",
                      binding->key.name, g_variant_get_type_string (variant),
                      type_str);
          g_free (type_str);
          valid = FALSE;
        }

      if (valid && !g_settings_schema_key_range_check (&binding->key, variant))
        {
          gchar *variant_str;
          variant_str = g_variant_print (variant, TRUE);
          g_critical ("GObject property '%s' on a '%s' object is out of "
                      "schema-specified range for key '%s' of '%s': %s",
                      binding->property->name, g_type_name (binding->property->owner_type),
                      binding->key.name, g_settings_schema_get_id (binding->key.schema),
                      variant_str);
          g_free (variant_str);
          valid = FALSE;
        }

      if (valid)
        {
          g_settings_write_to_backend (binding->settings, &binding->key, variant);
        }
      g_variant_unref (variant);
    }
  g_value_unset (&value);

  binding->running = FALSE;
}

static gboolean
g_settings_bind_invert_boolean_get_mapping (GValue   *value,
                                            GVariant *variant,
                                            gpointer  user_data)
{
  g_value_set_boolean (value, !g_variant_get_boolean (variant));
  return TRUE;
}

static GVariant *
g_settings_bind_invert_boolean_set_mapping (const GValue       *value,
                                            const GVariantType *expected_type,
                                            gpointer            user_data)
{
  return g_variant_new_boolean (!g_value_get_boolean (value));
}

/**
 * g_settings_bind:
 * @settings: the settings object
 * @key: the key to bind
 * @object: (type GObject.Object): the object with property to bind
 * @property: the name of the property to bind
 * @flags: flags for the binding
 *
 * Create a binding between the @key in the @settings object
 * and the property @property of @object.
 *
 * The binding uses the default GIO mapping functions to map
 * between the settings and property values. These functions
 * handle booleans, numeric types and string types in a
 * straightforward way. Use [method@Gio.Settings.bind_with_mapping] if
 * you need a custom mapping, or map between types that are not
 * supported by the default mapping functions.
 *
 * Unless the @flags include [flags@Gio.SettingsBindFlags.NO_SENSITIVITY], this
 * function also establishes a binding between the writability of
 * @key and the `sensitive` property of @object (if @object has
 * a boolean property by that name). See [method@Gio.Settings.bind_writable]
 * for more details about writable bindings.
 *
 * Note that the lifecycle of the binding is tied to @object,
 * and that you can have only one binding per object property.
 * If you bind the same property twice on the same object, the second
 * binding overrides the first one.
 *
 * Since: 2.26
 */
void
g_settings_bind (GSettings          *settings,
                 const gchar        *key,
                 gpointer            object,
                 const gchar        *property,
                 GSettingsBindFlags  flags)
{
  GSettingsBindGetMapping get_mapping = NULL;
  GSettingsBindSetMapping set_mapping = NULL;

  if (flags & G_SETTINGS_BIND_INVERT_BOOLEAN)
    {
      get_mapping = g_settings_bind_invert_boolean_get_mapping;
      set_mapping = g_settings_bind_invert_boolean_set_mapping;

      /* can't pass this flag to g_settings_bind_with_mapping() */
      flags &= ~G_SETTINGS_BIND_INVERT_BOOLEAN;
    }

  g_settings_bind_with_mapping (settings, key, object, property, flags,
                                get_mapping, set_mapping, NULL, NULL);
}

/**
 * g_settings_bind_with_mapping: (skip)
 * @settings: the settings object
 * @key: the key to bind
 * @object: (type GObject.Object): the object with property to bind
 * @property: the name of the property to bind
 * @flags: flags for the binding
 * @get_mapping: (nullable): a function that gets called to convert values
 *   from @settings to @object, or `NULL` to use the default GIO mapping
 * @set_mapping: (nullable): a function that gets called to convert values
 *   from @object to @settings, or `NULL` to use the default GIO mapping
 * @user_data: data that gets passed to @get_mapping and @set_mapping
 * @destroy: destroy notify function for @user_data
 *
 * Create a binding between the @key in the @settings object
 * and the property @property of @object.
 *
 * The binding uses the provided mapping functions to map between
 * settings and property values.
 *
 * Note that the lifecycle of the binding is tied to @object,
 * and that you can have only one binding per object property.
 * If you bind the same property twice on the same object, the second
 * binding overrides the first one.
 *
 * Since: 2.26
 */
void
g_settings_bind_with_mapping (GSettings               *settings,
                              const gchar             *key,
                              gpointer                 object,
                              const gchar             *property,
                              GSettingsBindFlags       flags,
                              GSettingsBindGetMapping  get_mapping,
                              GSettingsBindSetMapping  set_mapping,
                              gpointer                 user_data,
                              GDestroyNotify           destroy)
{
  GSettingsBinding *binding;
  GObjectClass *objectclass;
  gchar *detailed_signal;
  GQuark binding_quark;

  g_return_if_fail (G_IS_SETTINGS (settings));
  g_return_if_fail (key != NULL);
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (property != NULL);
  g_return_if_fail (~flags & G_SETTINGS_BIND_INVERT_BOOLEAN);

  objectclass = G_OBJECT_GET_CLASS (object);

  binding = g_slice_new0 (GSettingsBinding);
  g_settings_schema_key_init (&binding->key, settings->priv->schema, key);
  binding->settings = g_object_ref (settings);
  binding->object = object;
  binding->property = g_object_class_find_property (objectclass, property);
  binding->user_data = user_data;
  binding->destroy = destroy;
  binding->get_mapping = get_mapping ? get_mapping : g_settings_get_mapping;
  binding->set_mapping = set_mapping ? set_mapping : g_settings_set_mapping;

  if (!(flags & (G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET)))
    flags |= G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET;

  if (binding->property == NULL)
    {
      g_critical ("g_settings_bind: no property '%s' on class '%s'",
                  property, G_OBJECT_TYPE_NAME (object));
      return;
    }

  if ((flags & G_SETTINGS_BIND_GET) &&
      (binding->property->flags & G_PARAM_WRITABLE) == 0)
    {
      g_critical ("g_settings_bind: property '%s' on class '%s' is not "
                  "writable", binding->property->name, G_OBJECT_TYPE_NAME (object));
      return;
    }
  if ((flags & G_SETTINGS_BIND_SET) &&
      (binding->property->flags & G_PARAM_READABLE) == 0)
    {
      g_critical ("g_settings_bind: property '%s' on class '%s' is not "
                  "readable", binding->property->name, G_OBJECT_TYPE_NAME (object));
      return;
    }

  if (get_mapping == g_settings_bind_invert_boolean_get_mapping)
    {
      /* g_settings_bind_invert_boolean_get_mapping() is a private
       * function, so if we are here it means that g_settings_bind() was
       * called with G_SETTINGS_BIND_INVERT_BOOLEAN.
       *
       * Ensure that both sides are boolean.
       */

      if (binding->property->value_type != G_TYPE_BOOLEAN)
        {
          g_critical ("g_settings_bind: G_SETTINGS_BIND_INVERT_BOOLEAN "
                      "was specified, but property '%s' on type '%s' has "
                      "type '%s'", binding->property->name, G_OBJECT_TYPE_NAME (object),
                      g_type_name ((binding->property->value_type)));
          return;
        }

      if (!g_variant_type_equal (binding->key.type, G_VARIANT_TYPE_BOOLEAN))
        {
          gchar *type_string = g_variant_type_dup_string (binding->key.type);
          g_critical ("g_settings_bind: G_SETTINGS_BIND_INVERT_BOOLEAN "
                      "was specified, but key '%s' on schema '%s' has "
                      "type '%s'", key, g_settings_schema_get_id (settings->priv->schema),
                      type_string);
          g_free (type_string);
          return;
        }

    }

  else if (((get_mapping == NULL && (flags & G_SETTINGS_BIND_GET)) ||
            (set_mapping == NULL && (flags & G_SETTINGS_BIND_SET))) &&
           !g_settings_mapping_is_compatible (binding->property->value_type,
                                              binding->key.type))
    {
      gchar *type_string = g_variant_type_dup_string (binding->key.type);
      g_critical ("g_settings_bind: property '%s' on class '%s' has type "
                  "'%s' which is not compatible with type '%s' of key '%s' "
                  "on schema '%s'", binding->property->name, G_OBJECT_TYPE_NAME (object),
                  g_type_name (binding->property->value_type),
                  type_string, key,
                  g_settings_schema_get_id (settings->priv->schema));
      g_free (type_string);
      return;
    }

  if ((flags & G_SETTINGS_BIND_SET) &&
      (~flags & G_SETTINGS_BIND_NO_SENSITIVITY))
    {
      GParamSpec *sensitive;

      sensitive = g_object_class_find_property (objectclass, "sensitive");

      if (sensitive && sensitive->value_type == G_TYPE_BOOLEAN &&
          (sensitive->flags & G_PARAM_WRITABLE))
        g_settings_bind_writable (settings, binding->key.name, object, "sensitive", FALSE);
    }

  if (flags & G_SETTINGS_BIND_SET)
    {
      detailed_signal = g_strdup_printf ("notify::%s", binding->property->name);
      binding->property_handler_id =
        g_signal_connect (object, detailed_signal,
                          G_CALLBACK (g_settings_binding_property_changed),
                          binding);
      g_free (detailed_signal);

      if (~flags & G_SETTINGS_BIND_GET)
        g_settings_binding_property_changed (object,
                                             binding->property,
                                             binding);
    }

  if (flags & G_SETTINGS_BIND_GET)
    {
      if (~flags & G_SETTINGS_BIND_GET_NO_CHANGES)
        {
          detailed_signal = g_strdup_printf ("changed::%s", key);
          binding->key_handler_id =
            g_signal_connect (settings, detailed_signal,
                              G_CALLBACK (g_settings_binding_key_changed),
                              binding);
          g_free (detailed_signal);
        }

      g_settings_binding_key_changed (settings, binding->key.name, binding);
    }

  binding_quark = g_settings_binding_quark (binding->property->name);
  g_object_set_qdata_full (object, binding_quark,
                           binding, g_settings_binding_free);
}

typedef struct _BindWithMappingClosuresData
{
  GClosure *get_mapping_closure;
  GClosure *set_mapping_closure;
} BindWithMappingClosuresData;

static BindWithMappingClosuresData *
bind_with_mapping_closures_data_new (GClosure *get_mapping_closure,
                                     GClosure *set_mapping_closure)
{
  BindWithMappingClosuresData *data;

  data = g_new0 (BindWithMappingClosuresData, 1);

  if (get_mapping_closure != NULL)
    {
      data->get_mapping_closure = g_closure_ref (get_mapping_closure);
      g_closure_sink (get_mapping_closure);
      if (G_CLOSURE_NEEDS_MARSHAL (get_mapping_closure))
        g_closure_set_marshal (get_mapping_closure, g_cclosure_marshal_generic);
    }

  if (set_mapping_closure != NULL)
    {
      data->set_mapping_closure = g_closure_ref (set_mapping_closure);
      g_closure_sink (set_mapping_closure);
      if (G_CLOSURE_NEEDS_MARSHAL (set_mapping_closure))
        g_closure_set_marshal (set_mapping_closure, g_cclosure_marshal_generic);
    }

  return data;
}

static void
bind_with_mapping_closures_data_free (BindWithMappingClosuresData *data)
{
  if (data->get_mapping_closure != NULL)
    g_closure_unref (data->get_mapping_closure);

  if (data->set_mapping_closure != NULL)
    g_closure_unref (data->set_mapping_closure);

  g_free (data);
}

static gboolean
bind_with_mapping_invoke_get (GValue *value,
                              GVariant *variant,
                              void *user_data)
{
  BindWithMappingClosuresData *data = (BindWithMappingClosuresData *) user_data;
  GValue params[2] = { G_VALUE_INIT, G_VALUE_INIT };
  GValue out = G_VALUE_INIT;
  gboolean retval;

  g_value_init (&params[0], G_TYPE_VALUE);
  g_value_set_boxed (&params[0], value);
  g_value_init (&params[1], G_TYPE_VARIANT);
  g_value_set_variant (&params[1], variant);
  g_value_init (&out, G_TYPE_BOOLEAN);

  g_closure_invoke (data->get_mapping_closure, &out, 2, params, /* hint = */ NULL);

  retval = g_value_get_boolean (&out);
  if (retval)
    {
      const GValue *out_value = g_value_get_boxed (&params[0]);

      g_assert (out_value != NULL);

      g_value_copy (out_value, value);
    }

  g_value_unset (&out);
  g_value_unset (&params[0]);
  g_value_unset (&params[1]);

  return retval;
}

static GVariant *
bind_with_mapping_invoke_set (const GValue *value,
                              const GVariantType *expected_type,
                              void *user_data)
{
  BindWithMappingClosuresData *data = (BindWithMappingClosuresData *) user_data;
  GValue params[2] = { G_VALUE_INIT, G_VALUE_INIT };
  GValue out = G_VALUE_INIT;
  GVariant *retval;

  g_value_init (&params[0], G_TYPE_VALUE);
  g_value_set_boxed (&params[0], value);
  g_value_init (&params[1], G_TYPE_VARIANT_TYPE);
  g_value_set_boxed (&params[1], expected_type);
  g_value_init (&out, G_TYPE_VARIANT);

  g_closure_invoke (data->set_mapping_closure, &out, 2, params, /* hint = */ NULL);

  retval = g_value_dup_variant (&out);

  g_value_unset (&out);
  g_value_unset (&params[0]);
  g_value_unset (&params[1]);

  return retval;
}

static void
bind_with_mapping_destroy (void *user_data)
{
  BindWithMappingClosuresData *data = (BindWithMappingClosuresData *) user_data;
  bind_with_mapping_closures_data_free (data);
}

/**
 * g_settings_bind_with_mapping_closures: (rename-to g_settings_bind_with_mapping):
 * @settings: the settings object
 * @key: the key to bind
 * @object: the object with property to bind
 * @property: the name of the property to bind
 * @flags: flags for the binding
 * @get_mapping: (nullable): a function that gets called to convert values
 *   from @settings to @object, or `NULL` to use the default GIO mapping
 * @set_mapping: (nullable): a function that gets called to convert values
 *   from @object to @settings, or `NULL` to use the default GIO mapping
 *
 * Version of [method@Gio.Settings.bind_with_mapping] using closures instead of
 * callbacks for easier binding in other languages.
 *
 * Since: 2.82
 */
void
g_settings_bind_with_mapping_closures (GSettings *settings,
                                       const char *key,
                                       GObject *object,
                                       const char *property,
                                       GSettingsBindFlags flags,
                                       GClosure *get_mapping,
                                       GClosure *set_mapping)
{
  BindWithMappingClosuresData *data;

  data = bind_with_mapping_closures_data_new (get_mapping, set_mapping);

  g_settings_bind_with_mapping (settings, key, object, property, flags,
                                bind_with_mapping_invoke_get,
                                bind_with_mapping_invoke_set, data,
                                bind_with_mapping_destroy);
}

/* Writability binding {{{1 */
typedef struct
{
  GSettings *settings;
  gpointer object;
  const gchar *key;
  const gchar *property;
  gboolean inverted;
  gulong handler_id;
} GSettingsWritableBinding;

static void
g_settings_writable_binding_free (gpointer data)
{
  GSettingsWritableBinding *binding = data;

  g_signal_handler_disconnect (binding->settings, binding->handler_id);
  g_object_unref (binding->settings);
  g_slice_free (GSettingsWritableBinding, binding);
}

static void
g_settings_binding_writable_changed (GSettings   *settings,
                                     const gchar *key,
                                     gpointer     user_data)
{
  GSettingsWritableBinding *binding = user_data;
  gboolean writable;

  g_assert (settings == binding->settings);
  g_assert (key == binding->key);

  writable = g_settings_is_writable (settings, key);

  if (binding->inverted)
    writable = !writable;

  g_object_set (binding->object, binding->property, writable, NULL);
}

/**
 * g_settings_bind_writable:
 * @settings: the settings object
 * @key: the key to bind
 * @object: (type GObject.Object): the object with property to bind
 * @property: the name of a boolean property to bind
 * @inverted: whether to ‘invert’ the value
 *
 * Create a binding between the writability of @key in the
 * @settings object and the property @property of @object.
 *
 * The property must be boolean; `sensitive` or `visible`
 * properties of widgets are the most likely candidates.
 *
 * Writable bindings are always uni-directional; changes of the
 * writability of the setting will be propagated to the object
 * property, not the other way.
 *
 * When the @inverted argument is true, the binding inverts the
 * value as it passes from the setting to the object, i.e. @property
 * will be set to true if the key is not writable.
 *
 * Note that the lifecycle of the binding is tied to @object,
 * and that you can have only one binding per object property.
 * If you bind the same property twice on the same object, the second
 * binding overrides the first one.
 *
 * Since: 2.26
 */
void
g_settings_bind_writable (GSettings   *settings,
                          const gchar *key,
                          gpointer     object,
                          const gchar *property,
                          gboolean     inverted)
{
  GSettingsWritableBinding *binding;
  gchar *detailed_signal;
  GParamSpec *pspec;

  g_return_if_fail (G_IS_SETTINGS (settings));

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (object), property);
  if (pspec == NULL)
    {
      g_critical ("g_settings_bind_writable: no property '%s' on class '%s'",
                  property, G_OBJECT_TYPE_NAME (object));
      return;
    }
  if ((pspec->flags & G_PARAM_WRITABLE) == 0)
    {
      g_critical ("g_settings_bind_writable: property '%s' on class '%s' is not writable",
                  property, G_OBJECT_TYPE_NAME (object));
      return;
    }

  binding = g_slice_new (GSettingsWritableBinding);
  binding->settings = g_object_ref (settings);
  binding->object = object;
  binding->key = g_intern_string (key);
  binding->property = g_intern_string (property);
  binding->inverted = inverted;

  detailed_signal = g_strdup_printf ("writable-changed::%s", key);
  binding->handler_id =
    g_signal_connect (settings, detailed_signal,
                      G_CALLBACK (g_settings_binding_writable_changed),
                      binding);
  g_free (detailed_signal);

  g_object_set_qdata_full (object, g_settings_binding_quark (property),
                           binding, g_settings_writable_binding_free);

  g_settings_binding_writable_changed (settings, binding->key, binding);
}

/**
 * g_settings_unbind:
 * @object: (type GObject.Object): the object with property to unbind
 * @property: the property whose binding is removed
 *
 * Removes an existing binding for @property on @object.
 *
 * Note that bindings are automatically removed when the
 * object is finalized, so it is rarely necessary to call this
 * function.
 *
 * Since: 2.26
 */
void
g_settings_unbind (gpointer     object,
                   const gchar *property)
{
  GQuark binding_quark;

  binding_quark = g_settings_binding_quark (property);
  g_object_set_qdata (object, binding_quark, NULL);
}

/* GAction {{{1 */

typedef struct
{
  GObject parent_instance;

  GSettingsSchemaKey key;
  GSettings *settings;
} GSettingsAction;

typedef GObjectClass GSettingsActionClass;

static GType g_settings_action_get_type (void);
static void g_settings_action_iface_init (GActionInterface *iface);
G_DEFINE_TYPE_WITH_CODE (GSettingsAction, g_settings_action, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_ACTION, g_settings_action_iface_init))

enum
{
  ACTION_PROP_0,
  ACTION_PROP_NAME,
  ACTION_PROP_PARAMETER_TYPE,
  ACTION_PROP_ENABLED,
  ACTION_PROP_STATE_TYPE,
  ACTION_PROP_STATE
};

static const gchar *
g_settings_action_get_name (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  return gsa->key.name;
}

static const GVariantType *
g_settings_action_get_parameter_type (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;
  const GVariantType *type;

  type = g_variant_get_type (gsa->key.default_value);
  if (g_variant_type_equal (type, G_VARIANT_TYPE_BOOLEAN))
    type = NULL;

  return type;
}

static gboolean
g_settings_action_get_enabled (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  return g_settings_is_writable (gsa->settings, gsa->key.name);
}

static const GVariantType *
g_settings_action_get_state_type (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  return g_variant_get_type (gsa->key.default_value);
}

static GVariant *
g_settings_action_get_state (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;
  GVariant *value;

  value = g_settings_read_from_backend (gsa->settings, &gsa->key, FALSE, FALSE);

  if (value == NULL)
    value = g_settings_schema_key_get_default_value (&gsa->key);

  return value;
}

static GVariant *
g_settings_action_get_state_hint (GAction *action)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  /* no point in reimplementing this... */
  return g_settings_schema_key_get_range (&gsa->key);
}

static void
g_settings_action_change_state (GAction  *action,
                                GVariant *value)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  if (g_settings_schema_key_type_check (&gsa->key, value) && g_settings_schema_key_range_check (&gsa->key, value))
    g_settings_write_to_backend (gsa->settings, &gsa->key, value);
}

static void
g_settings_action_activate (GAction  *action,
                            GVariant *parameter)
{
  GSettingsAction *gsa = (GSettingsAction *) action;

  if (g_variant_is_of_type (gsa->key.default_value, G_VARIANT_TYPE_BOOLEAN))
    {
      GVariant *old;

      if (parameter != NULL)
        return;

      old = g_settings_action_get_state (action);
      parameter = g_variant_new_boolean (!g_variant_get_boolean (old));
      g_variant_unref (old);
    }

  g_action_change_state (action, parameter);
}

static void
g_settings_action_get_property (GObject *object, guint prop_id,
                                GValue *value, GParamSpec *pspec)
{
  GAction *action = G_ACTION (object);

  switch (prop_id)
    {
    case ACTION_PROP_NAME:
      g_value_set_string (value, g_settings_action_get_name (action));
      break;

    case ACTION_PROP_PARAMETER_TYPE:
      g_value_set_boxed (value, g_settings_action_get_parameter_type (action));
      break;

    case ACTION_PROP_ENABLED:
      g_value_set_boolean (value, g_settings_action_get_enabled (action));
      break;

    case ACTION_PROP_STATE_TYPE:
      g_value_set_boxed (value, g_settings_action_get_state_type (action));
      break;

    case ACTION_PROP_STATE:
      g_value_take_variant (value, g_settings_action_get_state (action));
      break;

    default:
      g_assert_not_reached ();
    }
}

static void
g_settings_action_finalize (GObject *object)
{
  GSettingsAction *gsa = (GSettingsAction *) object;

  g_signal_handlers_disconnect_by_data (gsa->settings, gsa);
  g_object_unref (gsa->settings);
  g_settings_schema_key_clear (&gsa->key);

  G_OBJECT_CLASS (g_settings_action_parent_class)
    ->finalize (object);
}

static void
g_settings_action_init (GSettingsAction *gsa)
{
}

static void
g_settings_action_iface_init (GActionInterface *iface)
{
  iface->get_name = g_settings_action_get_name;
  iface->get_parameter_type = g_settings_action_get_parameter_type;
  iface->get_enabled = g_settings_action_get_enabled;
  iface->get_state_type = g_settings_action_get_state_type;
  iface->get_state = g_settings_action_get_state;
  iface->get_state_hint = g_settings_action_get_state_hint;
  iface->change_state = g_settings_action_change_state;
  iface->activate = g_settings_action_activate;
}

static void
g_settings_action_class_init (GSettingsActionClass *class)
{
  class->get_property = g_settings_action_get_property;
  class->finalize = g_settings_action_finalize;

  g_object_class_override_property (class, ACTION_PROP_NAME, "name");
  g_object_class_override_property (class, ACTION_PROP_PARAMETER_TYPE, "parameter-type");
  g_object_class_override_property (class, ACTION_PROP_ENABLED, "enabled");
  g_object_class_override_property (class, ACTION_PROP_STATE_TYPE, "state-type");
  g_object_class_override_property (class, ACTION_PROP_STATE, "state");
}

static void
g_settings_action_changed (GSettings   *settings,
                           const gchar *key,
                           gpointer     user_data)
{
  g_object_notify (user_data, "state");
}

static void
g_settings_action_enabled_changed (GSettings   *settings,
                                   const gchar *key,
                                   gpointer     user_data)
{
  g_object_notify (user_data, "enabled");
}

/**
 * g_settings_create_action:
 * @settings: the settings object
 * @key: the name of a key in @settings
 *
 * Creates a [iface@Gio.Action] corresponding to a given [class@Gio.Settings] key.
 *
 * The action has the same name as the key.
 *
 * The value of the key becomes the state of the action and the action
 * is enabled when the key is writable.  Changing the state of the
 * action results in the key being written to.  Changes to the value or
 * writability of the key cause appropriate change notifications to be
 * emitted for the action.
 *
 * For boolean-valued keys, action activations take no parameter and
 * result in the toggling of the value.  For all other types,
 * activations take the new value for the key (which must have the
 * correct type).
 *
 * Returns: (not nullable) (transfer full): a new [iface@Gio.Action]
 *
 * Since: 2.32
 **/
GAction *
g_settings_create_action (GSettings   *settings,
                          const gchar *key)
{
  GSettingsAction *gsa;
  gchar *detailed_signal;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  gsa = g_object_new (g_settings_action_get_type (), NULL);
  gsa->settings = g_object_ref (settings);
  g_settings_schema_key_init (&gsa->key, settings->priv->schema, key);

  detailed_signal = g_strdup_printf ("changed::%s", key);
  g_signal_connect (settings, detailed_signal, G_CALLBACK (g_settings_action_changed), gsa);
  g_free (detailed_signal);
  detailed_signal = g_strdup_printf ("writable-changed::%s", key);
  g_signal_connect (settings, detailed_signal, G_CALLBACK (g_settings_action_enabled_changed), gsa);
  g_free (detailed_signal);

  return G_ACTION (gsa);
}

/* Epilogue {{{1 */

/* vim:set foldmethod=marker: */
