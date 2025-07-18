/* GObject - GLib Type, Object, Parameter and Signal Library
 * Copyright (C) 1998-1999, 2000-2001 Tim Janik and Red Hat, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * MT safe with regards to reference counting.
 */

#include "config.h"

#include <string.h>
#include <signal.h>

#include "../glib/glib-private.h"

#include "gobject.h"
#include "gtype-private.h"
#include "gvaluecollector.h"
#include "gsignal.h"
#include "gparamspecs.h"
#include "gvaluetypes.h"
#include "gobject_trace.h"
#include "../glib/gconstructor.h"

/**
 * GObject:
 *
 * The base object type.
 *
 * `GObject` is the fundamental type providing the common attributes and
 * methods for all object types in GTK, Pango and other libraries
 * based on GObject. The `GObject` class provides methods for object
 * construction and destruction, property access methods, and signal
 * support. Signals are described in detail [here][gobject-Signals].
 *
 * For a tutorial on implementing a new `GObject` class, see [How to define and
 * implement a new GObject](tutorial.html#how-to-define-and-implement-a-new-gobject).
 * For a list of naming conventions for GObjects and their methods, see the
 * [GType conventions](concepts.html#conventions). For the high-level concepts
 * behind GObject, read
 * [Instantiatable classed types: Objects](concepts.html#instantiatable-classed-types-objects).
 *
 * Since GLib 2.72, all `GObject`s are guaranteed to be aligned to at least the
 * alignment of the largest basic GLib type (typically this is `guint64` or
 * `gdouble`). If you need larger alignment for an element in a `GObject`, you
 * should allocate it on the heap (aligned), or arrange for your `GObject` to be
 * appropriately padded. This guarantee applies to the `GObject` (or derived)
 * struct, the `GObjectClass` (or derived) struct, and any private data allocated
 * by `G_ADD_PRIVATE()`.
 */

/* --- macros --- */
#define PARAM_SPEC_PARAM_ID(pspec)		((pspec)->param_id)
#define	PARAM_SPEC_SET_PARAM_ID(pspec, id)	((pspec)->param_id = (id))

#define OBJECT_HAS_TOGGLE_REF_FLAG 0x1
#define OBJECT_HAS_TOGGLE_REF(object) \
    ((g_datalist_get_flags (&(object)->qdata) & OBJECT_HAS_TOGGLE_REF_FLAG) != 0)
#define OBJECT_FLOATING_FLAG 0x2

#define CLASS_HAS_PROPS_FLAG 0x1
#define CLASS_HAS_PROPS(class) \
    ((class)->flags & CLASS_HAS_PROPS_FLAG)
#define CLASS_HAS_CUSTOM_CONSTRUCTOR(class) \
    ((class)->constructor != g_object_constructor)
#define CLASS_HAS_CUSTOM_CONSTRUCTED(class) \
    ((class)->constructed != g_object_constructed)
#define CLASS_HAS_NOTIFY(class) ((class)->notify != NULL)
#define CLASS_HAS_CUSTOM_DISPATCH(class) \
    ((class)->dispatch_properties_changed != g_object_dispatch_properties_changed)
#define CLASS_NEEDS_NOTIFY(class) \
    (CLASS_HAS_NOTIFY(class) || CLASS_HAS_CUSTOM_DISPATCH(class))

#define CLASS_HAS_DERIVED_CLASS_FLAG 0x2
#define CLASS_HAS_DERIVED_CLASS(class) \
    ((class)->flags & CLASS_HAS_DERIVED_CLASS_FLAG)

/* --- signals --- */
enum {
  NOTIFY,
  LAST_SIGNAL
};


/* --- properties --- */
enum {
  PROP_NONE
};

#define OPTIONAL_FLAG_IN_CONSTRUCTION    (1 << 0)
#define OPTIONAL_FLAG_HAS_SIGNAL_HANDLER (1 << 1) /* Set if object ever had a signal handler */
#define OPTIONAL_FLAG_HAS_NOTIFY_HANDLER (1 << 2) /* Same, specifically for "notify" */
#define OPTIONAL_FLAG_EVER_HAD_WEAK_REF  (1 << 4) /* whether on the object ever g_weak_ref_set() was called. */

#if SIZEOF_INT == 4 && GLIB_SIZEOF_VOID_P >= 8
#define HAVE_OPTIONAL_FLAGS_IN_GOBJECT 1
#else
#define HAVE_OPTIONAL_FLAGS_IN_GOBJECT 0
#endif

/* For now we only create a private struct if we don't have optional flags in
 * GObject. Currently we don't need it otherwise. In the future we might
 * always add a private struct. */
#define HAVE_PRIVATE (!HAVE_OPTIONAL_FLAGS_IN_GOBJECT)

#if HAVE_PRIVATE
typedef struct {
#if !HAVE_OPTIONAL_FLAGS_IN_GOBJECT
	guint optional_flags; /* (atomic) */
#endif
} GObjectPrivate;

static int GObject_private_offset;
#endif

typedef struct
{
  GTypeInstance  g_type_instance;

  /*< private >*/
  guint          ref_count;  /* (atomic) */
#if HAVE_OPTIONAL_FLAGS_IN_GOBJECT
  guint          optional_flags;  /* (atomic) */
#endif
  GData         *qdata;
} GObjectReal;

G_STATIC_ASSERT(sizeof(GObject) == sizeof(GObjectReal));
G_STATIC_ASSERT(G_STRUCT_OFFSET(GObject, ref_count) == G_STRUCT_OFFSET(GObjectReal, ref_count));
G_STATIC_ASSERT(G_STRUCT_OFFSET(GObject, qdata) == G_STRUCT_OFFSET(GObjectReal, qdata));


/* --- prototypes --- */
static void	g_object_base_class_init		(GObjectClass	*class);
static void	g_object_base_class_finalize		(GObjectClass	*class);
static void	g_object_do_class_init			(GObjectClass	*class);
static void	g_object_init				(GObject	*object,
							 GObjectClass	*class);
static GObject*	g_object_constructor			(GType                  type,
							 guint                  n_construct_properties,
							 GObjectConstructParam *construct_params);
static void     g_object_constructed                    (GObject        *object);
static void	g_object_real_dispose			(GObject	*object);
static void	g_object_finalize			(GObject	*object);
static void	g_object_do_set_property		(GObject        *object,
							 guint           property_id,
							 const GValue   *value,
							 GParamSpec     *pspec);
static void	g_object_do_get_property		(GObject        *object,
							 guint           property_id,
							 GValue         *value,
							 GParamSpec     *pspec);
static void	g_value_object_init			(GValue		*value);
static void	g_value_object_free_value		(GValue		*value);
static void	g_value_object_copy_value		(const GValue	*src_value,
							 GValue		*dest_value);
static void	g_value_object_transform_value		(const GValue	*src_value,
							 GValue		*dest_value);
static gpointer g_value_object_peek_pointer             (const GValue   *value);
static gchar*	g_value_object_collect_value		(GValue		*value,
							 guint           n_collect_values,
							 GTypeCValue    *collect_values,
							 guint           collect_flags);
static gchar*	g_value_object_lcopy_value		(const GValue	*value,
							 guint           n_collect_values,
							 GTypeCValue    *collect_values,
							 guint           collect_flags);
static void	g_object_dispatch_properties_changed	(GObject	*object,
							 guint		 n_pspecs,
							 GParamSpec    **pspecs);
static void closure_array_destroy_all (GObject *object);
static guint               object_floating_flag_handler (GObject        *object,
                                                         gint            job);
static inline void object_set_optional_flags (GObject *object,
                                              guint flags);
static void g_object_weak_release_all (GObject *object, gboolean release_all);

static void object_interface_check_properties           (gpointer        check_data,
							 gpointer        g_iface);

/* --- typedefs --- */

typedef struct
{
  guint16 freeze_count;
  guint16 len;
  guint16 alloc;
  GParamSpec *pspecs[];
} GObjectNotifyQueue;

/* --- variables --- */
static GQuark	            quark_closure_array = 0;
static GQuark	            quark_weak_notifies = 0;
static GQuark	            quark_toggle_refs = 0;
static GQuark               quark_notify_queue;
static GParamSpecPool      *pspec_pool = NULL; /* atomic */
static gulong	            gobject_signals[LAST_SIGNAL] = { 0, };
static guint (*floating_flag_handler) (GObject*, gint) = object_floating_flag_handler;
static GQuark	            quark_weak_locations = 0;

static gpointer (*_local_g_datalist_id_update_atomic) (GData **datalist,
                                                       GQuark key_id,
                                                       gboolean already_locked,
                                                       GDataListUpdateAtomicFunc callback,
                                                       gpointer user_data) = NULL;
#undef _g_datalist_id_update_atomic_full
#define _g_datalist_id_update_atomic_full(...) ((_local_g_datalist_id_update_atomic) (__VA_ARGS__))

#if HAVE_PRIVATE
G_ALWAYS_INLINE static inline GObjectPrivate *
g_object_get_instance_private (GObject *object)
{
  return G_STRUCT_MEMBER_P (object, GObject_private_offset);
}
#endif

G_ALWAYS_INLINE static inline guint *
object_get_optional_flags_p (GObject *object)
{
#if HAVE_OPTIONAL_FLAGS_IN_GOBJECT
  return &(((GObjectReal *) object)->optional_flags);
#else
  return &g_object_get_instance_private (object)->optional_flags;
#endif
}

/*****************************************************************************/

/* For GWeakRef, we need to take a lock per-object. However, in various cases
 * we cannot take a strong reference on the object to keep it alive. So the
 * mutex cannot be in the object itself, because when we want to release the
 * lock, we can no longer access object.
 *
 * Instead, the mutex is on the WeakRefData, which is itself ref-counted
 * and has a separate lifetime from the object. */
typedef struct
{
  /* This is both an atomic ref-count and bit 30 (WEAK_REF_DATA_LOCK_BIT) is
   * used for g_bit_lock(). */
  gint atomic_field;

  guint16 len;

  /* Only relevant when len > 1. In that case, it's the allocated size of
   * "list.many" array.  */
  guint16 alloc;

  /* Only relevant when len > 0. In that case, either "one" or "many" union
   * field is in use. */
  union
  {
    GWeakRef *one;
    GWeakRef **many;
  } list;
} WeakRefData;

/* We choose bit 30, and not bit 31. Bit 31 would be the sign for gint, so it
 * a bit awkward to use. Note that it probably also would work fine.
 *
 * But 30 is ok, because it still leaves us space for 2^30-1 references, which
 * is more than we ever need. */
#define WEAK_REF_DATA_LOCK_BIT 30

static void weak_ref_data_clear_list (WeakRefData *wrdata, GObject *object);

static WeakRefData *
weak_ref_data_ref (WeakRefData *wrdata)
{
  gint ref;

#if G_ENABLE_DEBUG
  g_assert (wrdata);
#endif

  ref = g_atomic_int_add (&wrdata->atomic_field, 1);

#if G_ENABLE_DEBUG
  /* Overflow is almost impossible to happen, because the user would need to
   * spawn that many operating system threads, that all call
   * g_weak_ref_{set,get}() in parallel.
   *
   * Still, assert in debug mode. */
  g_assert (ref < G_MAXINT32);

  /* the real ref-count would be the following: */
  ref = (ref + 1) & ~(1 << WEAK_REF_DATA_LOCK_BIT);

  /* assert that the ref-count is still in the valid range. */
  g_assert (ref > 0 && ref < (1 << WEAK_REF_DATA_LOCK_BIT));
#endif
  (void) ref;

  return wrdata;
}

static void
weak_ref_data_unref (WeakRefData *wrdata)
{
  if (!wrdata)
    return;

  /* Note that we also use WEAK_REF_DATA_LOCK_BIT on "atomic_field" as a bit
   * lock. However, we will always keep the @wrdata alive (having a reference)
   * while holding a lock (otherwise, we couldn't unlock anymore). Thus, at the
   * point when we decrement the ref-count to zero, we surely also have the
   * @wrdata unlocked.
   *
   * This means, using "aomit_field" both as ref-count and the lock bit is
   * fine. */

  if (!g_atomic_int_dec_and_test (&wrdata->atomic_field))
    return;

#if G_ENABLE_DEBUG
  /* We expect that the list of weak locations is empty at this point.
   * During g_object_unref() (_object_unref_clear_weak_locations()) it
   * should have been cleared.
   *
   * Calling weak_ref_data_clear_list() should be unnecessary. */
  g_assert (wrdata->len == 0);
#endif

  g_free_sized (wrdata, sizeof (WeakRefData));
}

static void
weak_ref_data_lock (WeakRefData *wrdata)
{
  /* Note that while holding a _weak_ref_lock() on the @weak_ref, we MUST not acquire a
   * weak_ref_data_lock() on the @wrdata. The other way around! */
  if (wrdata)
    g_bit_lock (&wrdata->atomic_field, WEAK_REF_DATA_LOCK_BIT);
}

static void
weak_ref_data_unlock (WeakRefData *wrdata)
{
  if (wrdata)
    g_bit_unlock (&wrdata->atomic_field, WEAK_REF_DATA_LOCK_BIT);
}

static gpointer
weak_ref_data_get_or_create_cb (gpointer *data,
                                GDestroyNotify *destroy_notify,
                                gpointer user_data)
{
  WeakRefData *wrdata = *data;
  GObject *object = user_data;

  if (!wrdata)
    {
      wrdata = g_new (WeakRefData, 1);

      /* The initial ref-count is 1. This one is owned by the GData until the
       * object gets destroyed.
       *
       * The WEAK_REF_DATA_LOCK_BIT bit is of course initially unset.  */
      wrdata->atomic_field = 1;
      wrdata->len = 0;
      /* Other fields are left uninitialized. They are only considered with a positive @len. */

      *data = wrdata;
      *destroy_notify = (GDestroyNotify) weak_ref_data_unref;

      /* Mark the @object that it was ever involved with GWeakRef. This flag
       * will stick until @object gets destroyed, just like the WeakRefData
       * also won't be freed for the remainder of the life of @object. */
      object_set_optional_flags (object, OPTIONAL_FLAG_EVER_HAD_WEAK_REF);
    }

  return wrdata;
}

static WeakRefData *
weak_ref_data_get_or_create (GObject *object)
{
  if (!object)
    return NULL;

  return _g_datalist_id_update_atomic (&object->qdata,
                                       quark_weak_locations,
                                       weak_ref_data_get_or_create_cb,
                                       object);
}

static WeakRefData *
weak_ref_data_get (GObject *object)
{
  return g_datalist_id_get_data (&object->qdata, quark_weak_locations);
}

static WeakRefData *
weak_ref_data_get_surely (GObject *object)
{
  WeakRefData *wrdata;

  /* The "surely" part is about that we expect to have a WeakRefData.
   *
   * Note that once a GObject gets a WeakRefData (during g_weak_ref_set() and
   * weak_ref_data_get_or_create()), it sticks and is not freed until the
   * object gets destroyed.
   *
   * Maybe we could release the unused WeakRefData in g_weak_ref_set(), but
   * then we would always need to take a reference during weak_ref_data_get().
   * That is likely not worth it. */

  wrdata = weak_ref_data_get (object);
#if G_ENABLE_DEBUG
  g_assert (wrdata);
#endif
  return wrdata;
}

static gint32
weak_ref_data_list_find (WeakRefData *wrdata, GWeakRef *weak_ref)
{
  if (wrdata->len == 1u)
    {
      if (wrdata->list.one == weak_ref)
        return 0;
    }
  else
    {
      guint16 i;

      for (i = 0; i < wrdata->len; i++)
        {
          if (wrdata->list.many[i] == weak_ref)
            return i;
        }
    }

  return -1;
}

static gboolean
weak_ref_data_list_add (WeakRefData *wrdata, GWeakRef *weak_ref)
{
  if (wrdata->len == 0u)
    wrdata->list.one = weak_ref;
  else
    {
      if (wrdata->len == 1u)
        {
          GWeakRef *weak_ref2 = wrdata->list.one;

          wrdata->alloc = 4u;
          wrdata->list.many = g_new (GWeakRef *, wrdata->alloc);
          wrdata->list.many[0] = weak_ref2;
        }
      else if (wrdata->len == wrdata->alloc)
        {
          guint16 alloc;

          alloc = wrdata->alloc * 2u;
          if (G_UNLIKELY (alloc < wrdata->len))
            {
              if (wrdata->len == G_MAXUINT16)
                return FALSE;
              alloc = G_MAXUINT16;
            }
          wrdata->list.many = g_renew (GWeakRef *, wrdata->list.many, alloc);
          wrdata->alloc = alloc;
        }

      wrdata->list.many[wrdata->len] = weak_ref;
    }

  wrdata->len++;
  return TRUE;
}

static GWeakRef *
weak_ref_data_list_remove (WeakRefData *wrdata, guint16 idx, gboolean allow_shrink)
{
  GWeakRef *weak_ref;

#if G_ENABLE_DEBUG
  g_assert (idx < wrdata->len);
#endif

  wrdata->len--;

  if (wrdata->len == 0u)
    {
      weak_ref = wrdata->list.one;
    }
  else
    {
      weak_ref = wrdata->list.many[idx];

      if (wrdata->len == 1u)
        {
          GWeakRef *weak_ref2 = wrdata->list.many[idx == 0 ? 1 : 0];

          g_free (wrdata->list.many);
          wrdata->list.one = weak_ref2;
        }
      else
        {
          wrdata->list.many[idx] = wrdata->list.many[wrdata->len];

          if (allow_shrink && G_UNLIKELY (wrdata->len <= wrdata->alloc / 4u))
            {
              /* Shrink the buffer. When 75% are empty, shrink it to 50%. */
              if (wrdata->alloc == G_MAXUINT16)
                wrdata->alloc = ((guint32) G_MAXUINT16 + 1u) / 2u;
              else
                wrdata->alloc /= 2u;
              wrdata->list.many = g_renew (GWeakRef *, wrdata->list.many, wrdata->alloc);
            }
        }
    }

  return weak_ref;
}

static gboolean
weak_ref_data_has (GObject *object, WeakRefData *wrdata, WeakRefData **out_new_wrdata)
{
  WeakRefData *wrdata2;

  /* Check whether @object has @wrdata as WeakRefData. Note that an GObject's
   * WeakRefData never changes (until destruction, once it's allocated).
   *
   * If you thus hold a reference to a @wrdata, you can check that the @object
   * is still the same as the object where we got the @wrdata originally from.
   *
   * You couldn't do this check by using pointer equality of the GObject pointers,
   * when you cannot hold strong references on the objects involved. Because then
   * the object pointer might be dangling (and even destroyed and recreated as another
   * object at the same memory location).
   *
   * Basically, weak_ref_data_has() is to compare for equality of two GObject pointers,
   * when we cannot hold a strong reference on both. Instead, we earlier took a reference
   * on the @wrdata and compare that instead.
   */

  if (!object)
    {
      /* If @object is NULL, then it does have a NULL @wrdata, and we return
       * TRUE in the case.  That's a convenient special case for some callers.
       *
       * In other words, weak_ref_data_has(NULL, NULL, out_new_wrdata) is TRUE.
       */
#if G_ENABLE_DEBUG
      g_assert (!out_new_wrdata);
#endif
      return !wrdata;
    }

  if (!wrdata)
    {
      /* We only call this function with an @object that was previously
       * registered as GWeakRef.
       *
       * That means, our @object will have a wrdata, and the result of the
       * evaluation will be %FALSE. */
      if (out_new_wrdata)
        *out_new_wrdata = weak_ref_data_ref (weak_ref_data_get (object));
#if G_ENABLE_DEBUG
      g_assert (out_new_wrdata
                    ? *out_new_wrdata
                    : weak_ref_data_get (object));
#endif
      return FALSE;
    }

  wrdata2 = weak_ref_data_get_surely (object);

  if (wrdata == wrdata2)
    {
      if (out_new_wrdata)
        *out_new_wrdata = NULL;
      return TRUE;
    }

  if (out_new_wrdata)
    *out_new_wrdata = weak_ref_data_ref (wrdata2);
  return FALSE;
}

/*****************************************************************************/

/* --- functions --- */

static const GObjectNotifyQueue notify_queue_empty = {
  .freeze_count = 0,
};

G_ALWAYS_INLINE static inline gboolean
_is_notify_queue_empty (const GObjectNotifyQueue *nqueue)
{
  /* Only the notify_queue_empty instance has a zero freeze count. We check
   * here for that condition instead of pointer comparing to
   * &notify_queue_empty. That seems better because callers will afterwards
   * dereference "freeze_count", so the value is already loaded.
   *
   * In any case, both conditions must be equivalent.
   */
#ifdef G_ENABLE_DEBUG
  g_assert ((nqueue == &notify_queue_empty) == (nqueue->freeze_count == 0));
#endif
  return nqueue->freeze_count == 0;
}

G_ALWAYS_INLINE static inline gsize
g_object_notify_queue_alloc_size (gsize alloc)
{
  return G_STRUCT_OFFSET (GObjectNotifyQueue, pspecs) + (alloc * sizeof (GParamSpec *));
}

static GObjectNotifyQueue *
g_object_notify_queue_new_frozen (void)
{
  GObjectNotifyQueue *nqueue;

  nqueue = g_malloc (g_object_notify_queue_alloc_size (4));

  nqueue->freeze_count = 1;
  nqueue->alloc = 4;
  nqueue->len = 0;

  return nqueue;
}

static gpointer
g_object_notify_queue_freeze_cb (gpointer *data,
                                 GDestroyNotify *destroy_notify,
                                 gpointer user_data)
{
  GObject *object = ((gpointer *) user_data)[0];
  gboolean freeze_always = GPOINTER_TO_INT (((gpointer *) user_data)[1]);
  GObjectNotifyQueue *nqueue = *data;

  if (!nqueue)
    {
      /* The nqueue doesn't exist yet. We use the dummy object that is shared
       * by all instances. */
      *data = (gpointer) &notify_queue_empty;
      *destroy_notify = NULL;
    }
  else if (!freeze_always)
    {
      /* The caller only wants to ensure we are frozen once. If we are already frozen,
       * don't freeze another time.
       *
       * This is only relevant during the object initialization. */
    }
  else
    {
      if (_is_notify_queue_empty (nqueue))
        {
          nqueue = g_object_notify_queue_new_frozen ();
          *data = nqueue;
          *destroy_notify = g_free;
          nqueue->freeze_count++;
        }
      else if (G_UNLIKELY (nqueue->freeze_count == G_MAXUINT16))
        {
          g_critical ("Free queue for %s (%p) is larger than 65535,"
                      " called g_object_freeze_notify() too often."
                      " Forgot to call g_object_thaw_notify() or infinite loop",
                      G_OBJECT_TYPE_NAME (object), object);
        }
      else
        nqueue->freeze_count++;
    }

  return NULL;
}

static void
g_object_notify_queue_freeze (GObject *object, gboolean freeze_always)
{
  _g_datalist_id_update_atomic (&object->qdata,
                                quark_notify_queue,
                                g_object_notify_queue_freeze_cb,
                                ((gpointer[]){ object, GINT_TO_POINTER (!!freeze_always) }));
}

static gpointer
g_object_notify_queue_thaw_cb (gpointer *data,
                               GDestroyNotify *destroy_notify,
                               gpointer user_data)
{
  GObject *object = user_data;
  GObjectNotifyQueue *nqueue = *data;

  if (G_UNLIKELY (!nqueue))
    {
      g_critical ("%s: property-changed notification for %s(%p) is not frozen",
                  G_STRFUNC, G_OBJECT_TYPE_NAME (object), object);
      return NULL;
    }

  if (_is_notify_queue_empty (nqueue))
    {
      *data = NULL;
      *destroy_notify = NULL;
      return NULL;
    }

  nqueue->freeze_count--;

  if (nqueue->freeze_count > 0)
    return NULL;

  *data = NULL;
  *destroy_notify = NULL;
  return nqueue;
}

static void
g_object_notify_queue_thaw (GObject *object, gboolean take_ref)
{
  GObjectNotifyQueue *nqueue;

  nqueue = _g_datalist_id_update_atomic (&object->qdata,
                                         quark_notify_queue,
                                         g_object_notify_queue_thaw_cb,
                                         object);

  if (!nqueue)
    return;

  if (nqueue->len > 0)
    {
      guint16 i;
      guint16 j;

      /* Reverse the list. This is the order that we historically had. */
      for (i = 0, j = nqueue->len - 1u; i < j; i++, j--)
        {
          GParamSpec *tmp;

          tmp = nqueue->pspecs[i];
          nqueue->pspecs[i] = nqueue->pspecs[j];
          nqueue->pspecs[j] = tmp;
        }

      if (take_ref)
        g_object_ref (object);

      G_OBJECT_GET_CLASS (object)->dispatch_properties_changed (object, nqueue->len, nqueue->pspecs);

      if (take_ref)
        g_object_unref (object);
    }

  g_free (nqueue);
}

static gpointer
g_object_notify_queue_add_cb (gpointer *data,
                              GDestroyNotify *destroy_notify,
                              gpointer user_data)
{
  GParamSpec *pspec = ((gpointer *) user_data)[0];
  gboolean in_init = GPOINTER_TO_INT (((gpointer *) user_data)[1]);
  GObjectNotifyQueue *nqueue = *data;
  guint16 i;

  if (!nqueue)
    {
      if (!in_init)
        {
          /* We are not in-init and are currently not frozen. There is nothing
           * to do. We return FALSE to the caller, which then will dispatch
           * the event right away. */
          return GINT_TO_POINTER (FALSE);
        }

      /* If we are "in_init", we always want to create a queue now.
       *
       * Note in that case, the freeze will be balanced at the end of object
       * initialization.
       *
       * We only ensure that a nqueue exists. If it doesn't exist, we create
       * it (and freeze once). If it already exists (and is frozen), we don't
       * freeze an additional time. */
      nqueue = g_object_notify_queue_new_frozen ();
      *data = nqueue;
      *destroy_notify = g_free;
    }
  else if (_is_notify_queue_empty (nqueue))
    {
      nqueue = g_object_notify_queue_new_frozen ();
      *data = nqueue;
      *destroy_notify = g_free;
    }
  else
    {
      for (i = 0; i < nqueue->len; i++)
        {
          if (nqueue->pspecs[i] == pspec)
            goto out;
        }

      if (G_UNLIKELY (nqueue->len == nqueue->alloc))
        {
          guint32 alloc;

          alloc = ((guint32) nqueue->alloc) * 2u;
          if (alloc >= G_MAXUINT16)
            {
              if (G_UNLIKELY (nqueue->len >= G_MAXUINT16))
                g_error ("g_object_notify_queue_add_cb: cannot track more than 65535 properties for freeze notification");
              alloc = G_MAXUINT16;
            }
          nqueue = g_realloc (nqueue, g_object_notify_queue_alloc_size (alloc));
          nqueue->alloc = alloc;

          *data = nqueue;
        }
    }

  nqueue->pspecs[nqueue->len++] = pspec;

out:
  return GINT_TO_POINTER (TRUE);
}

static gboolean
g_object_notify_queue_add (GObject *object,
                           GParamSpec *pspec,
                           gboolean in_init)
{
  gpointer result;

  result = _g_datalist_id_update_atomic (&object->qdata,
                                         quark_notify_queue,
                                         g_object_notify_queue_add_cb,
                                         ((gpointer[]){ pspec, GINT_TO_POINTER (!!in_init) }));

  return GPOINTER_TO_INT (result);
}

#ifdef	G_ENABLE_DEBUG
G_LOCK_DEFINE_STATIC     (debug_objects);
static guint		 debug_objects_count = 0;
static GHashTable	*debug_objects_ht = NULL;

static void
debug_objects_foreach (gpointer key,
		       gpointer value,
		       gpointer user_data)
{
  GObject *object = value;

  g_message ("[%p] stale %s\tref_count=%u",
	     object,
	     G_OBJECT_TYPE_NAME (object),
	     object->ref_count);
}

#ifdef G_HAS_CONSTRUCTORS
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(debug_objects_atexit)
#endif
G_DEFINE_DESTRUCTOR(debug_objects_atexit)
#endif /* G_HAS_CONSTRUCTORS */

static void
debug_objects_atexit (void)
{
  GOBJECT_IF_DEBUG (OBJECTS,
    {
      G_LOCK (debug_objects);
      g_message ("stale GObjects: %u", debug_objects_count);
      g_hash_table_foreach (debug_objects_ht, debug_objects_foreach, NULL);
      G_UNLOCK (debug_objects);
    });
}
#endif	/* G_ENABLE_DEBUG */

void
_g_object_type_init (void)
{
  static gboolean initialized = FALSE;
  static const GTypeFundamentalInfo finfo = {
    G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE,
  };
  GTypeInfo info = {
    sizeof (GObjectClass),
    (GBaseInitFunc) g_object_base_class_init,
    (GBaseFinalizeFunc) g_object_base_class_finalize,
    (GClassInitFunc) g_object_do_class_init,
    NULL	/* class_destroy */,
    NULL	/* class_data */,
    sizeof (GObject),
    0		/* n_preallocs */,
    (GInstanceInitFunc) g_object_init,
    NULL,	/* value_table */
  };
  static const GTypeValueTable value_table = {
    g_value_object_init,	  /* value_init */
    g_value_object_free_value,	  /* value_free */
    g_value_object_copy_value,	  /* value_copy */
    g_value_object_peek_pointer,  /* value_peek_pointer */
    "p",			  /* collect_format */
    g_value_object_collect_value, /* collect_value */
    "p",			  /* lcopy_format */
    g_value_object_lcopy_value,	  /* lcopy_value */
  };
  GType type G_GNUC_UNUSED  /* when compiling with G_DISABLE_ASSERT */;

  g_return_if_fail (initialized == FALSE);
  initialized = TRUE;

  /* G_TYPE_OBJECT
   */
  info.value_table = &value_table;
  type = g_type_register_fundamental (G_TYPE_OBJECT, g_intern_static_string ("GObject"), &info, &finfo, 0);
  g_assert (type == G_TYPE_OBJECT);
  g_value_register_transform_func (G_TYPE_OBJECT, G_TYPE_OBJECT, g_value_object_transform_value);

#if G_ENABLE_DEBUG
  /* We cannot use GOBJECT_IF_DEBUG here because of the G_HAS_CONSTRUCTORS
   * conditional in between, as the C spec leaves conditionals inside macro
   * expansions as undefined behavior. Only GCC and Clang are known to work
   * but compilation breaks on MSVC.
   *
   * See: https://bugzilla.gnome.org/show_bug.cgi?id=769504
   */
  if (_g_type_debug_flags & G_TYPE_DEBUG_OBJECTS) \
    {
      debug_objects_ht = g_hash_table_new (g_direct_hash, NULL);
# ifndef G_HAS_CONSTRUCTORS
      g_atexit (debug_objects_atexit);
# endif /* G_HAS_CONSTRUCTORS */
    }
#endif /* G_ENABLE_DEBUG */

#if HAVE_PRIVATE
  GObject_private_offset =
      g_type_add_instance_private (G_TYPE_OBJECT, sizeof (GObjectPrivate));
#endif
}

/* Initialize the global GParamSpecPool; this function needs to be
 * called whenever we access the GParamSpecPool and we cannot guarantee
 * that g_object_do_class_init() has been called: for instance, by the
 * interface property API.
 *
 * To avoid yet another global lock, we use atomic pointer checks: the
 * first caller of this function will win the race. Any other access to
 * the GParamSpecPool is done under its own mutex.
 */
static inline GParamSpecPool *
g_object_maybe_init_pspec_pool (void)
{
  GParamSpecPool *pool = g_atomic_pointer_get (&pspec_pool);

  if (G_UNLIKELY (pool == NULL))
    {
      GParamSpecPool *new_pool = g_param_spec_pool_new (TRUE);
      if (g_atomic_pointer_compare_and_exchange_full (&pspec_pool, NULL,
                                                      new_pool, &pool))
        pool = g_steal_pointer (&new_pool);

      g_clear_pointer (&new_pool, g_param_spec_pool_free);
    }

  return pool;
}

static void
g_object_base_class_init (GObjectClass *class)
{
  GObjectClass *pclass = g_type_class_peek_parent (class);

  /* Don't inherit HAS_DERIVED_CLASS flag from parent class */
  class->flags &= (unsigned) ~CLASS_HAS_DERIVED_CLASS_FLAG;

  if (pclass)
    pclass->flags |= CLASS_HAS_DERIVED_CLASS_FLAG;

  /* reset instance specific fields and methods that don't get inherited */
  class->construct_properties = pclass ? g_slist_copy (pclass->construct_properties) : NULL;
  class->n_construct_properties = g_slist_length (class->construct_properties);
  class->get_property = NULL;
  class->set_property = NULL;
  class->pspecs = NULL;
  class->n_pspecs = 0;
}

static void
g_object_base_class_finalize (GObjectClass *class)
{
  GList *list, *node;
  GParamSpecPool *param_spec_pool;

  _g_signals_destroy (G_OBJECT_CLASS_TYPE (class));

  g_slist_free (class->construct_properties);
  class->construct_properties = NULL;
  class->n_construct_properties = 0;
  param_spec_pool = g_atomic_pointer_get (&pspec_pool);
  list = g_param_spec_pool_list_owned (param_spec_pool, G_OBJECT_CLASS_TYPE (class));
  for (node = list; node; node = node->next)
    {
      GParamSpec *pspec = node->data;
      g_param_spec_pool_remove (param_spec_pool, pspec);
      PARAM_SPEC_SET_PARAM_ID (pspec, 0);
      g_param_spec_unref (pspec);
    }
  g_list_free (list);
}

static void
g_object_do_class_init (GObjectClass *class)
{
  quark_closure_array = g_quark_from_static_string ("GObject-closure-array");
  quark_weak_notifies = g_quark_from_static_string ("GObject-weak-notifies");
  quark_weak_locations = g_quark_from_static_string ("GObject-weak-locations");
  quark_toggle_refs = g_quark_from_static_string ("GObject-toggle-references");
  quark_notify_queue = g_quark_from_static_string ("GObject-notify-queue");

  g_atomic_pointer_set (&_local_g_datalist_id_update_atomic, GLIB_PRIVATE_CALL (g_datalist_id_update_atomic));

  g_object_maybe_init_pspec_pool ();

  class->constructor = g_object_constructor;
  class->constructed = g_object_constructed;
  class->set_property = g_object_do_set_property;
  class->get_property = g_object_do_get_property;
  class->dispose = g_object_real_dispose;
  class->finalize = g_object_finalize;
  class->dispatch_properties_changed = g_object_dispatch_properties_changed;
  class->notify = NULL;

  /**
   * GObject::notify:
   * @gobject: the object which received the signal.
   * @pspec: the #GParamSpec of the property which changed.
   *
   * The notify signal is emitted on an object when one of its properties has
   * its value set through g_object_set_property(), g_object_set(), et al.
   *
   * Note that getting this signal doesn’t itself guarantee that the value of
   * the property has actually changed. When it is emitted is determined by the
   * derived GObject class. If the implementor did not create the property with
   * %G_PARAM_EXPLICIT_NOTIFY, then any call to g_object_set_property() results
   * in ::notify being emitted, even if the new value is the same as the old.
   * If they did pass %G_PARAM_EXPLICIT_NOTIFY, then this signal is emitted only
   * when they explicitly call g_object_notify() or g_object_notify_by_pspec(),
   * and common practice is to do that only when the value has actually changed.
   *
   * This signal is typically used to obtain change notification for a
   * single property, by specifying the property name as a detail in the
   * g_signal_connect() call, like this:
   *
   * |[<!-- language="C" -->
   * g_signal_connect (text_view->buffer, "notify::paste-target-list",
   *                   G_CALLBACK (gtk_text_view_target_list_notify),
   *                   text_view)
   * ]|
   *
   * It is important to note that you must use
   * [canonical parameter names][class@GObject.ParamSpec#parameter-names] as
   * detail strings for the notify signal.
   */
  gobject_signals[NOTIFY] =
    g_signal_new (g_intern_static_string ("notify"),
		  G_TYPE_FROM_CLASS (class),
		  G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_DETAILED | G_SIGNAL_NO_HOOKS | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GObjectClass, notify),
		  NULL, NULL,
		  NULL,
		  G_TYPE_NONE,
		  1, G_TYPE_PARAM);

  /* Install a check function that we'll use to verify that classes that
   * implement an interface implement all properties for that interface
   */
  g_type_add_interface_check (NULL, object_interface_check_properties);

#if HAVE_PRIVATE
  g_type_class_adjust_private_offset (class, &GObject_private_offset);
#endif
}

/* Sinks @pspec if it’s a floating ref. */
static inline gboolean
install_property_internal (GType       g_type,
			   guint       property_id,
			   GParamSpec *pspec)
{
  GParamSpecPool *param_spec_pool;
  g_param_spec_ref_sink (pspec);

  param_spec_pool = g_object_maybe_init_pspec_pool ();

  if (g_param_spec_pool_lookup (param_spec_pool, pspec->name, g_type, FALSE))
    {
      g_critical ("When installing property: type '%s' already has a property named '%s'",
                  g_type_name (g_type),
                  pspec->name);
      g_param_spec_unref (pspec);
      return FALSE;
    }

  PARAM_SPEC_SET_PARAM_ID (pspec, property_id);
  g_param_spec_pool_insert (param_spec_pool, g_steal_pointer (&pspec), g_type);
  return TRUE;
}

static gboolean
validate_pspec_to_install (GParamSpec *pspec)
{
  g_return_val_if_fail (G_IS_PARAM_SPEC (pspec), FALSE);
  g_return_val_if_fail (PARAM_SPEC_PARAM_ID (pspec) == 0, FALSE);	/* paranoid */

  g_return_val_if_fail (pspec->flags & (G_PARAM_READABLE | G_PARAM_WRITABLE), FALSE);

  if (pspec->flags & G_PARAM_CONSTRUCT)
    g_return_val_if_fail ((pspec->flags & G_PARAM_CONSTRUCT_ONLY) == 0, FALSE);

  if (pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY))
    g_return_val_if_fail (pspec->flags & G_PARAM_WRITABLE, FALSE);

  return TRUE;
}

/* Sinks @pspec if it’s a floating ref. */
static gboolean
validate_and_install_class_property (GObjectClass *class,
                                     GType         oclass_type,
                                     GType         parent_type,
                                     guint         property_id,
                                     GParamSpec   *pspec)
{
  if (!validate_pspec_to_install (pspec))
    {
      g_param_spec_ref_sink (pspec);
      g_param_spec_unref (pspec);
      return FALSE;
    }

  if (pspec->flags & G_PARAM_WRITABLE)
    g_return_val_if_fail (class->set_property != NULL, FALSE);
  if (pspec->flags & G_PARAM_READABLE)
    g_return_val_if_fail (class->get_property != NULL, FALSE);

  class->flags |= CLASS_HAS_PROPS_FLAG;
  if (install_property_internal (oclass_type, property_id, pspec))
    {
      if (pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY))
        {
          class->construct_properties = g_slist_append (class->construct_properties, pspec);
          class->n_construct_properties += 1;
        }

      /* for property overrides of construct properties, we have to get rid
       * of the overridden inherited construct property
       */
      pspec = g_param_spec_pool_lookup (g_atomic_pointer_get (&pspec_pool),
                                        pspec->name, parent_type, TRUE);
      if (pspec && pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY))
        {
          class->construct_properties = g_slist_remove (class->construct_properties, pspec);
          class->n_construct_properties -= 1;
        }

      return TRUE;
    }
  else
    return FALSE;
}

/**
 * g_object_class_install_property:
 * @oclass: a #GObjectClass
 * @property_id: the id for the new property
 * @pspec: the #GParamSpec for the new property
 *
 * Installs a new property.
 *
 * All properties should be installed during the class initializer.  It
 * is possible to install properties after that, but doing so is not
 * recommend, and specifically, is not guaranteed to be thread-safe vs.
 * use of properties on the same type on other threads.
 *
 * Note that it is possible to redefine a property in a derived class,
 * by installing a property with the same name. This can be useful at times,
 * e.g. to change the range of allowed values or the default value.
 */
void
g_object_class_install_property (GObjectClass *class,
				 guint	       property_id,
				 GParamSpec   *pspec)
{
  GType oclass_type, parent_type;

  g_return_if_fail (G_IS_OBJECT_CLASS (class));
  g_return_if_fail (property_id > 0);

  oclass_type = G_OBJECT_CLASS_TYPE (class);
  parent_type = g_type_parent (oclass_type);

  if (CLASS_HAS_DERIVED_CLASS (class))
    g_error ("Attempt to add property %s::%s to class after it was derived", G_OBJECT_CLASS_NAME (class), pspec->name);

  (void) validate_and_install_class_property (class,
                                              oclass_type,
                                              parent_type,
                                              property_id,
                                              pspec);
}

typedef struct {
  const char *name;
  GParamSpec *pspec;
} PspecEntry;

static int
compare_pspec_entry (const void *a,
                     const void *b)
{
  const PspecEntry *ae = a;
  const PspecEntry *be = b;

  return ae->name < be->name ? -1 : (ae->name > be->name ? 1 : 0);
}

/* This uses pointer comparisons with @property_name, so
 * will only work with string literals. */
static inline GParamSpec *
find_pspec (GObjectClass *class,
            const char   *property_name)
{
  const PspecEntry *pspecs = (const PspecEntry *)class->pspecs;
  gsize n_pspecs = class->n_pspecs;

  g_assert (n_pspecs <= G_MAXSSIZE);

  /* The limit for choosing between linear and binary search is
   * fairly arbitrary.
   *
   * Both searches use pointer comparisons against @property_name.
   * If this function is called with a non-static @property_name,
   * it will fall through to the g_param_spec_pool_lookup() case.
   * That’s OK; this is an opportunistic optimisation which relies
   * on the fact that *most* (but not all) property lookups use
   * static property names.
   */
  if (n_pspecs < 10)
    {
      for (gsize i = 0; i < n_pspecs; i++)
        {
          if (pspecs[i].name == property_name)
            return pspecs[i].pspec;
        }
    }
  else
    {
      gssize lower = 0;
      gssize upper = (int)class->n_pspecs - 1;
      gssize mid;

      while (lower <= upper)
        {
          mid = (lower + upper) / 2;

          if (property_name < pspecs[mid].name)
            upper = mid - 1;
          else if (property_name > pspecs[mid].name)
            lower = mid + 1;
          else
            return pspecs[mid].pspec;
        }
    }

  return g_param_spec_pool_lookup (g_atomic_pointer_get (&pspec_pool),
                                   property_name,
                                   ((GTypeClass *)class)->g_type,
                                   TRUE);
}

/**
 * g_object_class_install_properties:
 * @oclass: a #GObjectClass
 * @n_pspecs: the length of the #GParamSpecs array
 * @pspecs: (array length=n_pspecs): the #GParamSpecs array
 *   defining the new properties
 *
 * Installs new properties from an array of #GParamSpecs.
 *
 * All properties should be installed during the class initializer.  It
 * is possible to install properties after that, but doing so is not
 * recommend, and specifically, is not guaranteed to be thread-safe vs.
 * use of properties on the same type on other threads.
 *
 * The property id of each property is the index of each #GParamSpec in
 * the @pspecs array.
 *
 * The property id of 0 is treated specially by #GObject and it should not
 * be used to store a #GParamSpec.
 *
 * This function should be used if you plan to use a static array of
 * #GParamSpecs and g_object_notify_by_pspec(). For instance, this
 * class initialization:
 *
 * |[<!-- language="C" -->
 * typedef enum {
 *   PROP_FOO = 1,
 *   PROP_BAR,
 *   N_PROPERTIES
 * } MyObjectProperty;
 *
 * static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };
 *
 * static void
 * my_object_class_init (MyObjectClass *klass)
 * {
 *   GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
 *
 *   obj_properties[PROP_FOO] =
 *     g_param_spec_int ("foo", NULL, NULL,
 *                       -1, G_MAXINT,
 *                       0,
 *                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
 *
 *   obj_properties[PROP_BAR] =
 *     g_param_spec_string ("bar", NULL, NULL,
 *                          NULL,
 *                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
 *
 *   gobject_class->set_property = my_object_set_property;
 *   gobject_class->get_property = my_object_get_property;
 *   g_object_class_install_properties (gobject_class,
 *                                      G_N_ELEMENTS (obj_properties),
 *                                      obj_properties);
 * }
 * ]|
 *
 * allows calling g_object_notify_by_pspec() to notify of property changes:
 *
 * |[<!-- language="C" -->
 * void
 * my_object_set_foo (MyObject *self, gint foo)
 * {
 *   if (self->foo != foo)
 *     {
 *       self->foo = foo;
 *       g_object_notify_by_pspec (G_OBJECT (self), obj_properties[PROP_FOO]);
 *     }
 *  }
 * ]|
 *
 * Since: 2.26
 */
void
g_object_class_install_properties (GObjectClass  *oclass,
                                   guint          n_pspecs,
                                   GParamSpec   **pspecs)
{
  GType oclass_type, parent_type;
  guint i;

  g_return_if_fail (G_IS_OBJECT_CLASS (oclass));
  g_return_if_fail (n_pspecs > 1);
  g_return_if_fail (pspecs[0] == NULL);

  if (CLASS_HAS_DERIVED_CLASS (oclass))
    g_error ("Attempt to add properties to %s after it was derived",
             G_OBJECT_CLASS_NAME (oclass));

  oclass_type = G_OBJECT_CLASS_TYPE (oclass);
  parent_type = g_type_parent (oclass_type);

  /* we skip the first element of the array as it would have a 0 prop_id */
  for (i = 1; i < n_pspecs; i++)
    {
      GParamSpec *pspec = pspecs[i];

      if (!validate_and_install_class_property (oclass,
                                                oclass_type,
                                                parent_type,
                                                i,
                                                pspec))
        {
          break;
        }
    }

  /* Save a copy of the pspec array inside the class struct. This
   * makes it faster to look up pspecs for the class in future when
   * acting on those properties.
   *
   * If a pspec is not in this cache array, calling code will fall
   * back to using g_param_spec_pool_lookup(), so a pspec not being
   * in this array is a (potential) performance problem but not a
   * correctness problem. */
  if (oclass->pspecs == NULL)
    {
      PspecEntry *entries;

      entries = g_new (PspecEntry, n_pspecs - 1);

      for (i = 1; i < n_pspecs; i++)
        {
          entries[i - 1].name = pspecs[i]->name;
          entries[i - 1].pspec = pspecs[i];
        }

      qsort (entries, n_pspecs - 1, sizeof (PspecEntry), compare_pspec_entry);

      oclass->pspecs = entries;
      oclass->n_pspecs = n_pspecs - 1;
    }
}

/**
 * g_object_interface_install_property:
 * @g_iface: (type GObject.TypeInterface): any interface vtable for the
 *    interface, or the default
 *  vtable for the interface.
 * @pspec: the #GParamSpec for the new property
 *
 * Add a property to an interface; this is only useful for interfaces
 * that are added to GObject-derived types. Adding a property to an
 * interface forces all objects classes with that interface to have a
 * compatible property. The compatible property could be a newly
 * created #GParamSpec, but normally
 * g_object_class_override_property() will be used so that the object
 * class only needs to provide an implementation and inherits the
 * property description, default value, bounds, and so forth from the
 * interface property.
 *
 * This function is meant to be called from the interface's default
 * vtable initialization function (the @class_init member of
 * #GTypeInfo.) It must not be called after after @class_init has
 * been called for any object types implementing this interface.
 *
 * If @pspec is a floating reference, it will be consumed.
 *
 * Since: 2.4
 */
void
g_object_interface_install_property (gpointer      g_iface,
				     GParamSpec   *pspec)
{
  GTypeInterface *iface_class = g_iface;

  g_return_if_fail (G_TYPE_IS_INTERFACE (iface_class->g_type));
  g_return_if_fail (!G_IS_PARAM_SPEC_OVERRIDE (pspec)); /* paranoid */

  if (!validate_pspec_to_install (pspec))
    {
      g_param_spec_ref_sink (pspec);
      g_param_spec_unref (pspec);
      return;
    }

  (void) install_property_internal (iface_class->g_type, 0, pspec);
}

/* Inlined version of g_param_spec_get_redirect_target(), for speed */
static inline void
param_spec_follow_override (GParamSpec **pspec)
{
  if (((GTypeInstance *) (*pspec))->g_class->g_type == G_TYPE_PARAM_OVERRIDE)
    *pspec = ((GParamSpecOverride *) (*pspec))->overridden;
}

/**
 * g_object_class_find_property:
 * @oclass: a #GObjectClass
 * @property_name: the name of the property to look up
 *
 * Looks up the #GParamSpec for a property of a class.
 *
 * Returns: (transfer none): the #GParamSpec for the property, or
 *          %NULL if the class doesn't have a property of that name
 */
GParamSpec*
g_object_class_find_property (GObjectClass *class,
			      const gchar  *property_name)
{
  GParamSpec *pspec;

  g_return_val_if_fail (G_IS_OBJECT_CLASS (class), NULL);
  g_return_val_if_fail (property_name != NULL, NULL);

  pspec = find_pspec (class, property_name);

  if (pspec)
    param_spec_follow_override (&pspec);

  return pspec;
}

/**
 * g_object_interface_find_property:
 * @g_iface: (type GObject.TypeInterface): any interface vtable for the
 *  interface, or the default vtable for the interface
 * @property_name: name of a property to look up.
 *
 * Find the #GParamSpec with the given name for an
 * interface. Generally, the interface vtable passed in as @g_iface
 * will be the default vtable from g_type_default_interface_ref(), or,
 * if you know the interface has already been loaded,
 * g_type_default_interface_peek().
 *
 * Since: 2.4
 *
 * Returns: (transfer none): the #GParamSpec for the property of the
 *          interface with the name @property_name, or %NULL if no
 *          such property exists.
 */
GParamSpec*
g_object_interface_find_property (gpointer      g_iface,
				  const gchar  *property_name)
{
  GTypeInterface *iface_class = g_iface;
  GParamSpecPool *param_spec_pool;

  g_return_val_if_fail (G_TYPE_IS_INTERFACE (iface_class->g_type), NULL);
  g_return_val_if_fail (property_name != NULL, NULL);

  param_spec_pool = g_object_maybe_init_pspec_pool ();

  return g_param_spec_pool_lookup (param_spec_pool,
				   property_name,
				   iface_class->g_type,
				   FALSE);
}

/**
 * g_object_class_override_property:
 * @oclass: a #GObjectClass
 * @property_id: the new property ID
 * @name: the name of a property registered in a parent class or
 *  in an interface of this class.
 *
 * Registers @property_id as referring to a property with the name
 * @name in a parent class or in an interface implemented by @oclass.
 * This allows this class to "override" a property implementation in
 * a parent class or to provide the implementation of a property from
 * an interface.
 *
 * Internally, overriding is implemented by creating a property of type
 * #GParamSpecOverride; generally operations that query the properties of
 * the object class, such as g_object_class_find_property() or
 * g_object_class_list_properties() will return the overridden
 * property. However, in one case, the @construct_properties argument of
 * the @constructor virtual function, the #GParamSpecOverride is passed
 * instead, so that the @param_id field of the #GParamSpec will be
 * correct.  For virtually all uses, this makes no difference. If you
 * need to get the overridden property, you can call
 * g_param_spec_get_redirect_target().
 *
 * Since: 2.4
 */
void
g_object_class_override_property (GObjectClass *oclass,
				  guint         property_id,
				  const gchar  *name)
{
  GParamSpecPool *param_spec_pool;
  GParamSpec *overridden = NULL;
  GParamSpec *new;
  GType parent_type;

  g_return_if_fail (G_IS_OBJECT_CLASS (oclass));
  g_return_if_fail (property_id > 0);
  g_return_if_fail (name != NULL);

  param_spec_pool = g_atomic_pointer_get (&pspec_pool);

  /* Find the overridden property; first check parent types
   */
  parent_type = g_type_parent (G_OBJECT_CLASS_TYPE (oclass));
  if (parent_type != G_TYPE_NONE)
    overridden = g_param_spec_pool_lookup (param_spec_pool,
					   name,
					   parent_type,
					   TRUE);
  if (!overridden)
    {
      GType *ifaces;
      guint n_ifaces;

      /* Now check interfaces
       */
      ifaces = g_type_interfaces (G_OBJECT_CLASS_TYPE (oclass), &n_ifaces);
      while (n_ifaces-- && !overridden)
	{
	  overridden = g_param_spec_pool_lookup (param_spec_pool,
						 name,
						 ifaces[n_ifaces],
						 FALSE);
	}

      g_free (ifaces);
    }

  if (!overridden)
    {
      g_critical ("%s: Can't find property to override for '%s::%s'",
		  G_STRFUNC, G_OBJECT_CLASS_NAME (oclass), name);
      return;
    }

  new = g_param_spec_override (name, overridden);
  g_object_class_install_property (oclass, property_id, new);
}

/**
 * g_object_class_list_properties:
 * @oclass: a #GObjectClass
 * @n_properties: (out): return location for the length of the returned array
 *
 * Get an array of #GParamSpec* for all properties of a class.
 *
 * Returns: (array length=n_properties) (transfer container): an array of
 *          #GParamSpec* which should be freed after use
 */
GParamSpec** /* free result */
g_object_class_list_properties (GObjectClass *class,
				guint        *n_properties_p)
{
  GParamSpec **pspecs;
  guint n;

  g_return_val_if_fail (G_IS_OBJECT_CLASS (class), NULL);

  pspecs = g_param_spec_pool_list (g_atomic_pointer_get (&pspec_pool),
				   G_OBJECT_CLASS_TYPE (class),
				   &n);
  if (n_properties_p)
    *n_properties_p = n;

  return pspecs;
}

/**
 * g_object_interface_list_properties:
 * @g_iface: (type GObject.TypeInterface): any interface vtable for the
 *  interface, or the default vtable for the interface
 * @n_properties_p: (out): location to store number of properties returned.
 *
 * Lists the properties of an interface.Generally, the interface
 * vtable passed in as @g_iface will be the default vtable from
 * g_type_default_interface_ref(), or, if you know the interface has
 * already been loaded, g_type_default_interface_peek().
 *
 * Since: 2.4
 *
 * Returns: (array length=n_properties_p) (transfer container): a
 *   pointer to an array of pointers to #GParamSpec
 *   structures. The paramspecs are owned by GLib, but the
 *   array should be freed with g_free() when you are done with
 *   it.
 */
GParamSpec**
g_object_interface_list_properties (gpointer      g_iface,
				    guint        *n_properties_p)
{
  GTypeInterface *iface_class = g_iface;
  GParamSpecPool *param_spec_pool;
  GParamSpec **pspecs;
  guint n;

  g_return_val_if_fail (G_TYPE_IS_INTERFACE (iface_class->g_type), NULL);

  param_spec_pool = g_object_maybe_init_pspec_pool ();

  pspecs = g_param_spec_pool_list (param_spec_pool,
				   iface_class->g_type,
				   &n);
  if (n_properties_p)
    *n_properties_p = n;

  return pspecs;
}

static inline guint
object_get_optional_flags (GObject *object)
{
  return (guint) g_atomic_int_get ((gint *) object_get_optional_flags_p (object));
}

static inline void
object_set_optional_flags (GObject *object,
                          guint flags)
{
  g_atomic_int_or ((gint *) object_get_optional_flags_p (object), (int) flags);
}

static inline void
object_unset_optional_flags (GObject *object,
                               guint flags)
{
  g_atomic_int_and ((gint *) object_get_optional_flags_p (object), (int) ~flags);
}

gboolean
_g_object_has_signal_handler (GObject *object)
{
  return (object_get_optional_flags (object) & OPTIONAL_FLAG_HAS_SIGNAL_HANDLER) != 0;
}

static inline gboolean
_g_object_has_notify_handler (GObject *object)
{
  return CLASS_NEEDS_NOTIFY (G_OBJECT_GET_CLASS (object)) ||
         (object_get_optional_flags (object) & OPTIONAL_FLAG_HAS_NOTIFY_HANDLER) != 0;
}

void
_g_object_set_has_signal_handler (GObject *object,
                                  guint    signal_id)
{
  guint flags = OPTIONAL_FLAG_HAS_SIGNAL_HANDLER;
  if (signal_id == gobject_signals[NOTIFY])
    flags |= OPTIONAL_FLAG_HAS_NOTIFY_HANDLER;
  object_set_optional_flags (object, flags);
}

static inline gboolean
object_in_construction (GObject *object)
{
  return (object_get_optional_flags (object) & OPTIONAL_FLAG_IN_CONSTRUCTION) != 0;
}

static inline void
set_object_in_construction (GObject *object)
{
  object_set_optional_flags (object, OPTIONAL_FLAG_IN_CONSTRUCTION);
}

static inline void
unset_object_in_construction (GObject *object)
{
  object_unset_optional_flags (object, OPTIONAL_FLAG_IN_CONSTRUCTION);
}

static void
g_object_init (GObject		*object,
	       GObjectClass	*class)
{
  object->ref_count = 1;
  object->qdata = NULL;

  if (CLASS_HAS_PROPS (class) && CLASS_NEEDS_NOTIFY (class))
    {
      /* freeze object's notification queue, g_object_new_internal() preserves pairedness */
      g_object_notify_queue_freeze (object, TRUE);
    }

  /* mark object in-construction for notify_queue_thaw() and to allow construct-only properties */
  set_object_in_construction (object);

  GOBJECT_IF_DEBUG (OBJECTS,
    {
      G_LOCK (debug_objects);
      debug_objects_count++;
      g_hash_table_add (debug_objects_ht, object);
      G_UNLOCK (debug_objects);
    });
}

static void
g_object_do_set_property (GObject      *object,
			  guint         property_id,
			  const GValue *value,
			  GParamSpec   *pspec)
{
  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
g_object_do_get_property (GObject     *object,
			  guint        property_id,
			  GValue      *value,
			  GParamSpec  *pspec)
{
  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
g_object_real_dispose (GObject *object)
{
  g_signal_handlers_destroy (object);

  /* GWeakNotify and GClosure can call into user code */
  g_object_weak_release_all (object, FALSE);
  closure_array_destroy_all (object);
}

static gboolean
g_diagnostic_is_enabled (void)
{
  static const char *g_enable_diagnostic = NULL;

  if (g_once_init_enter_pointer (&g_enable_diagnostic))
    {
      const gchar *value = g_getenv ("G_ENABLE_DIAGNOSTIC");

      if (value == NULL)
        value = "0";

      g_once_init_leave_pointer (&g_enable_diagnostic, value);
    }

  return g_enable_diagnostic[0] == '1';
}

#ifdef G_ENABLE_DEBUG
static gboolean
floating_check (GObject *object)
{
  if (g_diagnostic_is_enabled ())
    return g_object_is_floating (object);

  return FALSE;
}
#endif

static void
g_object_finalize (GObject *object)
{
#ifdef G_ENABLE_DEBUG
  if (object_in_construction (object))
    {
      g_critical ("object %s %p finalized while still in-construction",
                  G_OBJECT_TYPE_NAME (object), object);
    }

 if (floating_check (object))
   {
      g_critical ("A floating object %s %p was finalized. This means that someone\n"
                  "called g_object_unref() on an object that had only a floating\n"
                  "reference; the initial floating reference is not owned by anyone\n"
                  "and must be removed with g_object_ref_sink().",
                  G_OBJECT_TYPE_NAME (object), object);
   }
#endif

  g_datalist_clear (&object->qdata);

  GOBJECT_IF_DEBUG (OBJECTS,
    {
      G_LOCK (debug_objects);
      g_assert (g_hash_table_contains (debug_objects_ht, object));
      g_hash_table_remove (debug_objects_ht, object);
      debug_objects_count--;
      G_UNLOCK (debug_objects);
    });
}

static void
g_object_dispatch_properties_changed (GObject     *object,
				      guint        n_pspecs,
				      GParamSpec **pspecs)
{
  guint i;

  for (i = 0; i < n_pspecs; i++)
    g_signal_emit (object, gobject_signals[NOTIFY], g_param_spec_get_name_quark (pspecs[i]), pspecs[i]);
}

/**
 * g_object_run_dispose:
 * @object: a #GObject
 *
 * Releases all references to other objects. This can be used to break
 * reference cycles.
 *
 * This function should only be called from object system implementations.
 */
void
g_object_run_dispose (GObject *object)
{
  WeakRefData *wrdata;

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (g_atomic_int_get (&object->ref_count) > 0);

  g_object_ref (object);

  TRACE (GOBJECT_OBJECT_DISPOSE(object,G_TYPE_FROM_INSTANCE(object), 0));
  G_OBJECT_GET_CLASS (object)->dispose (object);
  TRACE (GOBJECT_OBJECT_DISPOSE_END(object,G_TYPE_FROM_INSTANCE(object), 0));

  if ((object_get_optional_flags (object) & OPTIONAL_FLAG_EVER_HAD_WEAK_REF))
    {
      wrdata = weak_ref_data_get_surely (object);
      weak_ref_data_lock (wrdata);
      weak_ref_data_clear_list (wrdata, object);
      weak_ref_data_unlock (wrdata);
    }

  g_object_unref (object);
}

/**
 * g_object_freeze_notify:
 * @object: a #GObject
 *
 * Increases the freeze count on @object. If the freeze count is
 * non-zero, the emission of "notify" signals on @object is
 * stopped. The signals are queued until the freeze count is decreased
 * to zero. Duplicate notifications are squashed so that at most one
 * #GObject::notify signal is emitted for each property modified while the
 * object is frozen.
 *
 * This is necessary for accessors that modify multiple properties to prevent
 * premature notification while the object is still being modified.
 */
void
g_object_freeze_notify (GObject *object)
{
  g_return_if_fail (G_IS_OBJECT (object));

#ifndef G_DISABLE_CHECKS
  if (G_UNLIKELY (g_atomic_int_get (&object->ref_count) <= 0))
    {
      g_critical ("Attempting to freeze the notification queue for object %s[%p]; "
                  "Property notification does not work during instance finalization.",
                  G_OBJECT_TYPE_NAME (object),
                  object);
      return;
    }
#endif

  g_object_notify_queue_freeze (object, TRUE);
}

static inline void
g_object_notify_by_spec_internal (GObject    *object,
                                  GParamSpec *pspec)
{
  guint object_flags;
  gboolean needs_notify;
  gboolean in_init;

  if (G_UNLIKELY (~pspec->flags & G_PARAM_READABLE))
    return;

  param_spec_follow_override (&pspec);

  /* get all flags we need with a single atomic read */
  object_flags = object_get_optional_flags (object);
  needs_notify = ((object_flags & OPTIONAL_FLAG_HAS_NOTIFY_HANDLER) != 0) ||
                  CLASS_NEEDS_NOTIFY (G_OBJECT_GET_CLASS (object));
  in_init = (object_flags & OPTIONAL_FLAG_IN_CONSTRUCTION) != 0;

  if (pspec != NULL && needs_notify)
    {
      if (!g_object_notify_queue_add (object, pspec, in_init))
        {
          /*
           * Coverity doesn’t understand the paired ref/unref here and seems to
           * ignore the ref, thus reports every call to g_object_notify() as
           * causing a double-free. That’s incorrect, but I can’t get a model
           * file to work for avoiding the false positives, so instead comment
           * out the ref/unref when doing static analysis.
           */
#ifndef __COVERITY__
          g_object_ref (object);
#endif

          /* not frozen, so just dispatch the notification directly */
          G_OBJECT_GET_CLASS (object)
              ->dispatch_properties_changed (object, 1, &pspec);

#ifndef __COVERITY__
          g_object_unref (object);
#endif
        }
    }
}

/**
 * g_object_notify:
 * @object: a #GObject
 * @property_name: the name of a property installed on the class of @object.
 *
 * Emits a "notify" signal for the property @property_name on @object.
 *
 * When possible, eg. when signaling a property change from within the class
 * that registered the property, you should use g_object_notify_by_pspec()
 * instead.
 *
 * Note that emission of the notify signal may be blocked with
 * g_object_freeze_notify(). In this case, the signal emissions are queued
 * and will be emitted (in reverse order) when g_object_thaw_notify() is
 * called.
 */
void
g_object_notify (GObject     *object,
		 const gchar *property_name)
{
  GParamSpec *pspec;

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (property_name != NULL);

  /* We don't need to get the redirect target
   * (by, e.g. calling g_object_class_find_property())
   * because g_object_notify_queue_add() does that
   */
  pspec = g_param_spec_pool_lookup (g_atomic_pointer_get (&pspec_pool),
				    property_name,
				    G_OBJECT_TYPE (object),
				    TRUE);

  if (!pspec)
    g_critical ("%s: object class '%s' has no property named '%s'",
	        G_STRFUNC,
	        G_OBJECT_TYPE_NAME (object),
	        property_name);
  else
    g_object_notify_by_spec_internal (object, pspec);
}

/**
 * g_object_notify_by_pspec:
 * @object: a #GObject
 * @pspec: the #GParamSpec of a property installed on the class of @object.
 *
 * Emits a "notify" signal for the property specified by @pspec on @object.
 *
 * This function omits the property name lookup, hence it is faster than
 * g_object_notify().
 *
 * One way to avoid using g_object_notify() from within the
 * class that registered the properties, and using g_object_notify_by_pspec()
 * instead, is to store the GParamSpec used with
 * g_object_class_install_property() inside a static array, e.g.:
 *
 *|[<!-- language="C" -->
 *   typedef enum
 *   {
 *     PROP_FOO = 1,
 *     PROP_LAST
 *   } MyObjectProperty;
 *
 *   static GParamSpec *properties[PROP_LAST];
 *
 *   static void
 *   my_object_class_init (MyObjectClass *klass)
 *   {
 *     properties[PROP_FOO] = g_param_spec_int ("foo", NULL, NULL,
 *                                              0, 100,
 *                                              50,
 *                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
 *     g_object_class_install_property (gobject_class,
 *                                      PROP_FOO,
 *                                      properties[PROP_FOO]);
 *   }
 * ]|
 *
 * and then notify a change on the "foo" property with:
 *
 * |[<!-- language="C" -->
 *   g_object_notify_by_pspec (self, properties[PROP_FOO]);
 * ]|
 *
 * Since: 2.26
 */
void
g_object_notify_by_pspec (GObject    *object,
			  GParamSpec *pspec)
{

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (G_IS_PARAM_SPEC (pspec));

  g_object_notify_by_spec_internal (object, pspec);
}

/**
 * g_object_thaw_notify:
 * @object: a #GObject
 *
 * Reverts the effect of a previous call to
 * g_object_freeze_notify(). The freeze count is decreased on @object
 * and when it reaches zero, queued "notify" signals are emitted.
 *
 * Duplicate notifications for each property are squashed so that at most one
 * #GObject::notify signal is emitted for each property, in the reverse order
 * in which they have been queued.
 *
 * It is an error to call this function when the freeze count is zero.
 */
void
g_object_thaw_notify (GObject *object)
{
  g_return_if_fail (G_IS_OBJECT (object));

#ifndef G_DISABLE_CHECKS
  if (G_UNLIKELY (g_atomic_int_get (&object->ref_count) <= 0))
    {
      g_critical ("Attempting to thaw the notification queue for object %s[%p]; "
                  "Property notification does not work during instance finalization.",
                  G_OBJECT_TYPE_NAME (object),
                  object);
      return;
    }
#endif

  g_object_notify_queue_thaw (object, TRUE);
}

static void
maybe_issue_property_deprecation_warning (const GParamSpec *pspec)
{
  static GHashTable *already_warned_table;
  static GMutex already_warned_lock;
  gboolean already;

  if (!g_diagnostic_is_enabled ())
    return;

  /* We hash only on property names: this means that we could end up in
   * a situation where we fail to emit a warning about a pair of
   * same-named deprecated properties used on two separate types.
   * That's pretty unlikely to occur, and even if it does, you'll still
   * have seen the warning for the first one...
   *
   * Doing it this way lets us hash directly on the (interned) property
   * name pointers.
   */
  g_mutex_lock (&already_warned_lock);

  if (already_warned_table == NULL)
    already_warned_table = g_hash_table_new (NULL, NULL);

  already = g_hash_table_contains (already_warned_table, (gpointer) pspec->name);
  if (!already)
    g_hash_table_add (already_warned_table, (gpointer) pspec->name);

  g_mutex_unlock (&already_warned_lock);

  if (!already)
    g_warning ("The property %s:%s is deprecated and shouldn't be used "
               "anymore. It will be removed in a future version.",
               g_type_name (pspec->owner_type), pspec->name);
}

static inline void
consider_issuing_property_deprecation_warning (const GParamSpec *pspec)
{
  if (G_UNLIKELY (pspec->flags & G_PARAM_DEPRECATED))
    maybe_issue_property_deprecation_warning (pspec);
}

static inline void
object_get_property (GObject     *object,
		     GParamSpec  *pspec,
		     GValue      *value)
{
  GTypeInstance *inst = (GTypeInstance *) object;
  GObjectClass *class;
  guint param_id = PARAM_SPEC_PARAM_ID (pspec);

  if (G_LIKELY (inst->g_class->g_type == pspec->owner_type))
    class = (GObjectClass *) inst->g_class;
  else
    class = g_type_class_peek (pspec->owner_type);

  g_assert (class != NULL);

  param_spec_follow_override (&pspec);

  consider_issuing_property_deprecation_warning (pspec);

  class->get_property (object, param_id, value, pspec);
}

static inline void
object_set_property (GObject             *object,
		     GParamSpec          *pspec,
		     const GValue        *value,
		     gboolean             nqueue_is_frozen,
		     gboolean             user_specified)
{
  GTypeInstance *inst = (GTypeInstance *) object;
  GObjectClass *class;
  GParamSpecClass *pclass;
  guint param_id = PARAM_SPEC_PARAM_ID (pspec);

  if (G_LIKELY (inst->g_class->g_type == pspec->owner_type))
    class = (GObjectClass *) inst->g_class;
  else
    class = g_type_class_peek (pspec->owner_type);

  g_assert (class != NULL);

  param_spec_follow_override (&pspec);

  if (user_specified)
    consider_issuing_property_deprecation_warning (pspec);

  pclass = G_PARAM_SPEC_GET_CLASS (pspec);
  if (g_value_type_compatible (G_VALUE_TYPE (value), pspec->value_type) &&
      (pclass->value_validate == NULL ||
       (pclass->value_is_valid != NULL && pclass->value_is_valid (pspec, value))))
    {
      class->set_property (object, param_id, value, pspec);
    }
  else
    {
      /* provide a copy to work from, convert (if necessary) and validate */
      GValue tmp_value = G_VALUE_INIT;

      g_value_init (&tmp_value, pspec->value_type);

      if (!g_value_transform (value, &tmp_value))
        g_critical ("unable to set property '%s' of type '%s' from value of type '%s'",
                    pspec->name,
                    g_type_name (pspec->value_type),
                    G_VALUE_TYPE_NAME (value));
      else if (g_param_value_validate (pspec, &tmp_value) && !(pspec->flags & G_PARAM_LAX_VALIDATION))
        {
          gchar *contents = g_strdup_value_contents (value);

          g_critical ("value \"%s\" of type '%s' is invalid or out of range for property '%s' of type '%s'",
                      contents,
                      G_VALUE_TYPE_NAME (value),
                      pspec->name,
                      g_type_name (pspec->value_type));
          g_free (contents);
        }
      else
        {
          class->set_property (object, param_id, &tmp_value, pspec);
        }

      g_value_unset (&tmp_value);
    }

  if ((pspec->flags & (G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READABLE)) == G_PARAM_READABLE &&
      nqueue_is_frozen)
    g_object_notify_queue_add (object, pspec, FALSE);
}

static void
object_interface_check_properties (gpointer check_data,
				   gpointer g_iface)
{
  GTypeInterface *iface_class = g_iface;
  GObjectClass *class;
  GParamSpecPool *param_spec_pool;
  GType iface_type = iface_class->g_type;
  GParamSpec **pspecs;
  guint n;

  class = g_type_class_ref (iface_class->g_instance_type);

  if (class == NULL)
    return;

  if (!G_IS_OBJECT_CLASS (class))
    goto out;

  param_spec_pool = g_atomic_pointer_get (&pspec_pool);
  pspecs = g_param_spec_pool_list (param_spec_pool, iface_type, &n);

  while (n--)
    {
      GParamSpec *class_pspec = g_param_spec_pool_lookup (param_spec_pool,
							  pspecs[n]->name,
							  G_OBJECT_CLASS_TYPE (class),
							  TRUE);

      if (!class_pspec)
	{
	  g_critical ("Object class %s doesn't implement property "
		      "'%s' from interface '%s'",
		      g_type_name (G_OBJECT_CLASS_TYPE (class)),
		      pspecs[n]->name,
		      g_type_name (iface_type));

	  continue;
	}

      /* We do a number of checks on the properties of an interface to
       * make sure that all classes implementing the interface are
       * overriding the properties correctly.
       *
       * We do the checks in order of importance so that we can give
       * more useful error messages first.
       *
       * First, we check that the implementation doesn't remove the
       * basic functionality (readability, writability) advertised by
       * the interface.  Next, we check that it doesn't introduce
       * additional restrictions (such as construct-only).  Finally, we
       * make sure the types are compatible.
       */

#define SUBSET(a,b,mask) (((a) & ~(b) & (mask)) == 0)
      /* If the property on the interface is readable then the
       * implementation must be readable.  If the interface is writable
       * then the implementation must be writable.
       */
      if (!SUBSET (pspecs[n]->flags, class_pspec->flags, G_PARAM_READABLE | G_PARAM_WRITABLE))
        {
          g_critical ("Flags for property '%s' on class '%s' remove functionality compared with the "
                      "property on interface '%s'\n", pspecs[n]->name,
                      g_type_name (G_OBJECT_CLASS_TYPE (class)), g_type_name (iface_type));
          continue;
        }

      /* If the property on the interface is writable then we need to
       * make sure the implementation doesn't introduce new restrictions
       * on that writability (ie: construct-only).
       *
       * If the interface was not writable to begin with then we don't
       * really have any problems here because "writable at construct
       * time only" is still more permissive than "read only".
       */
      if (pspecs[n]->flags & G_PARAM_WRITABLE)
        {
          if (!SUBSET (class_pspec->flags, pspecs[n]->flags, G_PARAM_CONSTRUCT_ONLY))
            {
              g_critical ("Flags for property '%s' on class '%s' introduce additional restrictions on "
                          "writability compared with the property on interface '%s'\n", pspecs[n]->name,
                          g_type_name (G_OBJECT_CLASS_TYPE (class)), g_type_name (iface_type));
              continue;
            }
        }
#undef SUBSET

      /* If the property on the interface is readable then we are
       * effectively advertising that reading the property will return a
       * value of a specific type.  All implementations of the interface
       * need to return items of this type -- but may be more
       * restrictive.  For example, it is legal to have:
       *
       *   GtkWidget *get_item();
       *
       * that is implemented by a function that always returns a
       * GtkEntry.  In short: readability implies that the
       * implementation  value type must be equal or more restrictive.
       *
       * Similarly, if the property on the interface is writable then
       * must be able to accept the property being set to any value of
       * that type, including subclasses.  In this case, we may also be
       * less restrictive.  For example, it is legal to have:
       *
       *   set_item (GtkEntry *);
       *
       * that is implemented by a function that will actually work with
       * any GtkWidget.  In short: writability implies that the
       * implementation value type must be equal or less restrictive.
       *
       * In the case that the property is both readable and writable
       * then the only way that both of the above can be satisfied is
       * with a type that is exactly equal.
       */
      switch (pspecs[n]->flags & (G_PARAM_READABLE | G_PARAM_WRITABLE))
        {
        case G_PARAM_READABLE | G_PARAM_WRITABLE:
          /* class pspec value type must have exact equality with interface */
          if (pspecs[n]->value_type != class_pspec->value_type)
            g_critical ("Read/writable property '%s' on class '%s' has type '%s' which is not exactly equal to the "
                        "type '%s' of the property on the interface '%s'\n", pspecs[n]->name,
                        g_type_name (G_OBJECT_CLASS_TYPE (class)), g_type_name (G_PARAM_SPEC_VALUE_TYPE (class_pspec)),
                        g_type_name (G_PARAM_SPEC_VALUE_TYPE (pspecs[n])), g_type_name (iface_type));
          break;

        case G_PARAM_READABLE:
          /* class pspec value type equal or more restrictive than interface */
          if (!g_type_is_a (class_pspec->value_type, pspecs[n]->value_type))
            g_critical ("Read-only property '%s' on class '%s' has type '%s' which is not equal to or more "
                        "restrictive than the type '%s' of the property on the interface '%s'\n", pspecs[n]->name,
                        g_type_name (G_OBJECT_CLASS_TYPE (class)), g_type_name (G_PARAM_SPEC_VALUE_TYPE (class_pspec)),
                        g_type_name (G_PARAM_SPEC_VALUE_TYPE (pspecs[n])), g_type_name (iface_type));
          break;

        case G_PARAM_WRITABLE:
          /* class pspec value type equal or less restrictive than interface */
          if (!g_type_is_a (pspecs[n]->value_type, class_pspec->value_type))
            g_critical ("Write-only property '%s' on class '%s' has type '%s' which is not equal to or less "
                        "restrictive than the type '%s' of the property on the interface '%s' \n", pspecs[n]->name,
                        g_type_name (G_OBJECT_CLASS_TYPE (class)), g_type_name (G_PARAM_SPEC_VALUE_TYPE (class_pspec)),
                        g_type_name (G_PARAM_SPEC_VALUE_TYPE (pspecs[n])), g_type_name (iface_type));
          break;

        default:
          g_assert_not_reached ();
        }
    }

  g_free (pspecs);

 out:
  g_type_class_unref (class);
}

GType
g_object_get_type (void)
{
    return G_TYPE_OBJECT;
}

/**
 * g_object_new: (skip)
 * @object_type: the type id of the #GObject subtype to instantiate
 * @first_property_name: the name of the first property
 * @...: the value of the first property, followed optionally by more
 *   name/value pairs, followed by %NULL
 *
 * Creates a new instance of a #GObject subtype and sets its properties.
 *
 * Construction parameters (see %G_PARAM_CONSTRUCT, %G_PARAM_CONSTRUCT_ONLY)
 * which are not explicitly specified are set to their default values. Any
 * private data for the object is guaranteed to be initialized with zeros, as
 * per g_type_create_instance().
 *
 * Note that in C, small integer types in variable argument lists are promoted
 * up to `gint` or `guint` as appropriate, and read back accordingly. `gint` is
 * 32 bits on every platform on which GLib is currently supported. This means that
 * you can use C expressions of type `gint` with g_object_new() and properties of
 * type `gint` or `guint` or smaller. Specifically, you can use integer literals
 * with these property types.
 *
 * When using property types of `gint64` or `guint64`, you must ensure that the
 * value that you provide is 64 bit. This means that you should use a cast or
 * make use of the %G_GINT64_CONSTANT or %G_GUINT64_CONSTANT macros.
 *
 * Similarly, `gfloat` is promoted to `gdouble`, so you must ensure that the value
 * you provide is a `gdouble`, even for a property of type `gfloat`.
 *
 * Since GLib 2.72, all #GObjects are guaranteed to be aligned to at least the
 * alignment of the largest basic GLib type (typically this is `guint64` or
 * `gdouble`). If you need larger alignment for an element in a #GObject, you
 * should allocate it on the heap (aligned), or arrange for your #GObject to be
 * appropriately padded.
 *
 * Returns: (transfer full) (type GObject.Object): a new instance of
 *   @object_type
 */
gpointer
g_object_new (GType	   object_type,
	      const gchar *first_property_name,
	      ...)
{
  GObject *object;
  va_list var_args;

  /* short circuit for calls supplying no properties */
  if (!first_property_name)
    return g_object_new_with_properties (object_type, 0, NULL, NULL);

  va_start (var_args, first_property_name);
  object = g_object_new_valist (object_type, first_property_name, var_args);
  va_end (var_args);

  return object;
}

/* Check alignment. (See https://gitlab.gnome.org/GNOME/glib/-/issues/1231.)
 * This should never fail, since g_type_create_instance() uses g_slice_alloc0().
 * The GSlice allocator always aligns to the next power of 2 greater than the
 * allocation size. The allocation size for a GObject is
 *   sizeof(GTypeInstance) + sizeof(guint) + sizeof(GData*)
 * which is 12B on 32-bit platforms, and larger on 64-bit systems. In both
 * cases, that’s larger than the 8B needed for a guint64 or gdouble.
 *
 * If GSlice falls back to malloc(), it’s documented to return something
 * suitably aligned for any basic type. */
static inline gboolean
g_object_is_aligned (GObject *object)
{
  return ((((guintptr) (void *) object) %
             MAX (G_ALIGNOF (gdouble),
                  MAX (G_ALIGNOF (guint64),
                       MAX (G_ALIGNOF (gint),
                            G_ALIGNOF (glong))))) == 0);
}

static gpointer
g_object_new_with_custom_constructor (GObjectClass          *class,
                                      GObjectConstructParam *params,
                                      guint                  n_params)
{
  gboolean nqueue_is_frozen = FALSE;
  gboolean newly_constructed;
  GObjectConstructParam *cparams;
  gboolean free_cparams = FALSE;
  GObject *object;
  GValue *cvalues;
  gint cvals_used;
  GSList *node;
  guint i;

  /* If we have ->constructed() then we have to do a lot more work.
   * It's possible that this is a singleton and it's also possible
   * that the user's constructor() will attempt to modify the values
   * that we pass in, so we'll need to allocate copies of them.
   * It's also possible that the user may attempt to call
   * g_object_set() from inside of their constructor, so we need to
   * add ourselves to a list of objects for which that is allowed
   * while their constructor() is running.
   */

  /* Create the array of GObjectConstructParams for constructor(),
   * The 1024 here is an arbitrary, high limit that no sane code
   * will ever hit, just to avoid the possibility of stack overflow.
   */
  if (G_LIKELY (class->n_construct_properties < 1024))
    {
      cparams = g_newa0 (GObjectConstructParam, class->n_construct_properties);
      cvalues = g_newa0 (GValue, class->n_construct_properties);
    }
  else
    {
      cparams = g_new0 (GObjectConstructParam, class->n_construct_properties);
      cvalues = g_new0 (GValue, class->n_construct_properties);
      free_cparams = TRUE;
    }
  cvals_used = 0;
  i = 0;

  /* As above, we may find the value in the passed-in params list.
   *
   * If we have the value passed in then we can use the GValue from
   * it directly because it is safe to modify.  If we use the
   * default value from the class, we had better not pass that in
   * and risk it being modified, so we create a new one.
   * */
  for (node = class->construct_properties; node; node = node->next)
    {
      GParamSpec *pspec;
      GValue *value;
      guint j;

      pspec = node->data;
      value = NULL; /* to silence gcc... */

      for (j = 0; j < n_params; j++)
        if (params[j].pspec == pspec)
          {
            consider_issuing_property_deprecation_warning (pspec);
            value = params[j].value;
            break;
          }

      if (value == NULL)
        {
          value = &cvalues[cvals_used++];
          g_value_init (value, pspec->value_type);
          g_param_value_set_default (pspec, value);
        }

      cparams[i].pspec = pspec;
      cparams[i].value = value;
      i++;
    }

  /* construct object from construction parameters */
  object = class->constructor (class->g_type_class.g_type, class->n_construct_properties, cparams);
  /* free construction values */
  while (cvals_used--)
    g_value_unset (&cvalues[cvals_used]);

  if (free_cparams)
    {
      g_free (cparams);
      g_free (cvalues);
    }

  /* There is code in the wild that relies on being able to return NULL
   * from its custom constructor.  This was never a supported operation,
   * but since the code is already out there...
   */
  if (object == NULL)
    {
      g_critical ("Custom constructor for class %s returned NULL (which is invalid). "
                  "Please use GInitable instead.", G_OBJECT_CLASS_NAME (class));
      return NULL;
    }

  if (!g_object_is_aligned (object))
    {
      g_critical ("Custom constructor for class %s returned a non-aligned "
                  "GObject (which is invalid since GLib 2.72). Assuming any "
                  "code using this object doesn’t require it to be aligned. "
                  "Please fix your constructor to align to the largest GLib "
                  "basic type (typically gdouble or guint64).",
                  G_OBJECT_CLASS_NAME (class));
    }

  /* g_object_init() will have marked the object as being in-construction.
   * Check if the returned object still is so marked, or if this is an
   * already-existing singleton (in which case we should not do 'constructed').
   */
  newly_constructed = object_in_construction (object);
  if (newly_constructed)
    unset_object_in_construction (object);

  if (CLASS_HAS_PROPS (class))
    {
      if ((newly_constructed && _g_object_has_notify_handler (object)) ||
          _g_object_has_notify_handler (object))
        {
          /* This may or may not have been setup in g_object_init().
           * If it hasn't, we do it now.
           */
          g_object_notify_queue_freeze (object, FALSE);
          nqueue_is_frozen = TRUE;
        }
    }

  /* run 'constructed' handler if there is a custom one */
  if (newly_constructed && CLASS_HAS_CUSTOM_CONSTRUCTED (class))
    class->constructed (object);

  /* set remaining properties */
  for (i = 0; i < n_params; i++)
    if (!(params[i].pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY)))
      object_set_property (object, params[i].pspec, params[i].value, nqueue_is_frozen, TRUE);

  if (nqueue_is_frozen)
    g_object_notify_queue_thaw (object, FALSE);

  return object;
}

static gpointer
g_object_new_internal (GObjectClass          *class,
                       GObjectConstructParam *params,
                       guint                  n_params)
{
  gboolean nqueue_is_frozen = FALSE;
  GObject *object;
  guint i;

  if G_UNLIKELY (CLASS_HAS_CUSTOM_CONSTRUCTOR (class))
    return g_object_new_with_custom_constructor (class, params, n_params);

  object = (GObject *) g_type_create_instance (class->g_type_class.g_type);

  g_assert (g_object_is_aligned (object));

  unset_object_in_construction (object);

  if (CLASS_HAS_PROPS (class))
    {
      GSList *node;

      if (_g_object_has_notify_handler (object))
        {
          /* This may or may not have been setup in g_object_init().
           * If it hasn't, we do it now.
           */
          g_object_notify_queue_freeze (object, FALSE);
          nqueue_is_frozen = TRUE;
        }

      /* We will set exactly n_construct_properties construct
       * properties, but they may come from either the class default
       * values or the passed-in parameter list.
       */
      for (node = class->construct_properties; node; node = node->next)
        {
          const GValue *value;
          GParamSpec *pspec;
          guint j;
          gboolean user_specified = FALSE;

          pspec = node->data;
          value = NULL; /* to silence gcc... */

          for (j = 0; j < n_params; j++)
            if (params[j].pspec == pspec)
              {
                value = params[j].value;
                user_specified = TRUE;
                break;
              }

          if (value == NULL)
            value = g_param_spec_get_default_value (pspec);

          object_set_property (object, pspec, value, nqueue_is_frozen, user_specified);
        }
    }

  /* run 'constructed' handler if there is a custom one */
  if (CLASS_HAS_CUSTOM_CONSTRUCTED (class))
    class->constructed (object);

  /* Set remaining properties.  The construct properties will
   * already have been taken, so set only the non-construct ones.
   */
  for (i = 0; i < n_params; i++)
    if (!(params[i].pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY)))
      object_set_property (object, params[i].pspec, params[i].value, nqueue_is_frozen, TRUE);

  if (nqueue_is_frozen)
    g_object_notify_queue_thaw (object, FALSE);

  return object;
}


static inline gboolean
g_object_new_is_valid_property (GType                  object_type,
                                GParamSpec            *pspec,
                                const char            *name,
                                GObjectConstructParam *params,
                                guint                  n_params)
{
  guint i;

  if (G_UNLIKELY (pspec == NULL))
    {
      g_critical ("%s: object class '%s' has no property named '%s'",
                  G_STRFUNC, g_type_name (object_type), name);
      return FALSE;
    }

  if (G_UNLIKELY (~pspec->flags & G_PARAM_WRITABLE))
    {
      g_critical ("%s: property '%s' of object class '%s' is not writable",
                  G_STRFUNC, pspec->name, g_type_name (object_type));
      return FALSE;
    }

  if (G_UNLIKELY (pspec->flags & (G_PARAM_CONSTRUCT | G_PARAM_CONSTRUCT_ONLY)))
    {
      for (i = 0; i < n_params; i++)
        if (params[i].pspec == pspec)
          break;
      if (G_UNLIKELY (i != n_params))
        {
          g_critical ("%s: property '%s' for type '%s' cannot be set twice",
                      G_STRFUNC, name, g_type_name (object_type));
          return FALSE;
        }
    }
  return TRUE;
}


/**
 * g_object_new_with_properties: (skip)
 * @object_type: the object type to instantiate
 * @n_properties: the number of properties
 * @names: (array length=n_properties): the names of each property to be set
 * @values: (array length=n_properties): the values of each property to be set
 *
 * Creates a new instance of a #GObject subtype and sets its properties using
 * the provided arrays. Both arrays must have exactly @n_properties elements,
 * and the names and values correspond by index.
 *
 * Construction parameters (see %G_PARAM_CONSTRUCT, %G_PARAM_CONSTRUCT_ONLY)
 * which are not explicitly specified are set to their default values.
 *
 * Returns: (type GObject.Object) (transfer full): a new instance of
 * @object_type
 *
 * Since: 2.54
 */
GObject *
g_object_new_with_properties (GType          object_type,
                              guint          n_properties,
                              const char    *names[],
                              const GValue   values[])
{
  GObjectClass *class, *unref_class = NULL;
  GObject *object;

  g_return_val_if_fail (G_TYPE_IS_OBJECT (object_type), NULL);

  /* Try to avoid thrashing the ref_count if we don't need to (since
   * it's a locked operation).
   */
  class = g_type_class_peek_static (object_type);

  if (class == NULL)
    class = unref_class = g_type_class_ref (object_type);

  if (n_properties > 0)
    {
      guint i, count = 0;
      GObjectConstructParam *params;

      params = g_newa (GObjectConstructParam, n_properties);
      for (i = 0; i < n_properties; i++)
        {
          GParamSpec *pspec = find_pspec (class, names[i]);

          if (!g_object_new_is_valid_property (object_type, pspec, names[i], params, count))
            continue;
          params[count].pspec = pspec;
          params[count].value = (GValue *) &values[i];
          count++;
        }
      object = g_object_new_internal (class, params, count);
    }
  else
    object = g_object_new_internal (class, NULL, 0);

  if (unref_class != NULL)
    g_type_class_unref (unref_class);

  return object;
}

/**
 * g_object_newv:
 * @object_type: the type id of the #GObject subtype to instantiate
 * @n_parameters: the length of the @parameters array
 * @parameters: (array length=n_parameters): an array of #GParameter
 *
 * Creates a new instance of a #GObject subtype and sets its properties.
 *
 * Construction parameters (see %G_PARAM_CONSTRUCT, %G_PARAM_CONSTRUCT_ONLY)
 * which are not explicitly specified are set to their default values.
 *
 * Returns: (type GObject.Object) (transfer full): a new instance of
 * @object_type
 *
 * Deprecated: 2.54: Use g_object_new_with_properties() instead.
 * deprecated. See #GParameter for more information.
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
gpointer
g_object_newv (GType       object_type,
               guint       n_parameters,
               GParameter *parameters)
{
  GObjectClass *class, *unref_class = NULL;
  GObject *object;

  g_return_val_if_fail (G_TYPE_IS_OBJECT (object_type), NULL);
  g_return_val_if_fail (n_parameters == 0 || parameters != NULL, NULL);

  /* Try to avoid thrashing the ref_count if we don't need to (since
   * it's a locked operation).
   */
  class = g_type_class_peek_static (object_type);

  if (!class)
    class = unref_class = g_type_class_ref (object_type);

  if (n_parameters)
    {
      GObjectConstructParam *cparams;
      guint i, j;

      cparams = g_newa (GObjectConstructParam, n_parameters);
      j = 0;

      for (i = 0; i < n_parameters; i++)
        {
          GParamSpec *pspec = find_pspec (class, parameters[i].name);

          if (!g_object_new_is_valid_property (object_type, pspec, parameters[i].name, cparams, j))
            continue;

          cparams[j].pspec = pspec;
          cparams[j].value = &parameters[i].value;
          j++;
        }

      object = g_object_new_internal (class, cparams, j);
    }
  else
    /* Fast case: no properties passed in. */
    object = g_object_new_internal (class, NULL, 0);

  if (unref_class)
    g_type_class_unref (unref_class);

  return object;
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_object_new_valist: (skip)
 * @object_type: the type id of the #GObject subtype to instantiate
 * @first_property_name: the name of the first property
 * @var_args: the value of the first property, followed optionally by more
 *  name/value pairs, followed by %NULL
 *
 * Creates a new instance of a #GObject subtype and sets its properties.
 *
 * Construction parameters (see %G_PARAM_CONSTRUCT, %G_PARAM_CONSTRUCT_ONLY)
 * which are not explicitly specified are set to their default values.
 *
 * Returns: a new instance of @object_type
 */
GObject*
g_object_new_valist (GType        object_type,
                     const gchar *first_property_name,
                     va_list      var_args)
{
  GObjectClass *class, *unref_class = NULL;
  GObject *object;

  g_return_val_if_fail (G_TYPE_IS_OBJECT (object_type), NULL);

  /* Try to avoid thrashing the ref_count if we don't need to (since
   * it's a locked operation).
   */
  class = g_type_class_peek_static (object_type);

  if (!class)
    class = unref_class = g_type_class_ref (object_type);

  if (first_property_name)
    {
      GObjectConstructParam params_stack[16];
      GValue values_stack[G_N_ELEMENTS (params_stack)];
      GTypeValueTable *vtabs_stack[G_N_ELEMENTS (params_stack)];
      const gchar *name;
      GObjectConstructParam *params = params_stack;
      GValue *values = values_stack;
      GTypeValueTable **vtabs = vtabs_stack;
      guint n_params = 0;
      guint n_params_alloc = G_N_ELEMENTS (params_stack);

      name = first_property_name;

      do
        {
          gchar *error = NULL;
          GParamSpec *pspec = find_pspec (class, name);

          if (!g_object_new_is_valid_property (object_type, pspec, name, params, n_params))
            break;

          if (G_UNLIKELY (n_params == n_params_alloc))
            {
              guint i;

              if (n_params_alloc == G_N_ELEMENTS (params_stack))
                {
                  n_params_alloc = G_N_ELEMENTS (params_stack) * 2u;
                  params = g_new (GObjectConstructParam, n_params_alloc);
                  values = g_new (GValue, n_params_alloc);
                  vtabs = g_new (GTypeValueTable *, n_params_alloc);
                  memcpy (params, params_stack, sizeof (GObjectConstructParam) * n_params);
                  memcpy (values, values_stack, sizeof (GValue) * n_params);
                  memcpy (vtabs, vtabs_stack, sizeof (GTypeValueTable *) * n_params);
                }
              else
                {
                  n_params_alloc *= 2u;
                  params = g_realloc (params, sizeof (GObjectConstructParam) * n_params_alloc);
                  values = g_realloc (values, sizeof (GValue) * n_params_alloc);
                  vtabs = g_realloc (vtabs, sizeof (GTypeValueTable *) * n_params_alloc);
                }

              for (i = 0; i < n_params; i++)
                params[i].value = &values[i];
            }

          params[n_params].pspec = pspec;
          params[n_params].value = &values[n_params];
          memset (&values[n_params], 0, sizeof (GValue));

          G_VALUE_COLLECT_INIT2 (&values[n_params], vtabs[n_params], pspec->value_type, var_args, G_VALUE_NOCOPY_CONTENTS, &error);

          if (error)
            {
              g_critical ("%s: %s", G_STRFUNC, error);
              g_value_unset (&values[n_params]);
              g_free (error);
              break;
            }

          n_params++;
        }
      while ((name = va_arg (var_args, const gchar *)));

      object = g_object_new_internal (class, params, n_params);

      while (n_params--)
        {
          /* We open-code g_value_unset() here to avoid the
           * cost of looking up the GTypeValueTable again.
           */
          if (vtabs[n_params]->value_free)
            vtabs[n_params]->value_free (params[n_params].value);
        }

      if (G_UNLIKELY (n_params_alloc != G_N_ELEMENTS (params_stack)))
        {
          g_free (params);
          g_free (values);
          g_free (vtabs);
        }
    }
  else
    /* Fast case: no properties passed in. */
    object = g_object_new_internal (class, NULL, 0);

  if (unref_class)
    g_type_class_unref (unref_class);

  return object;
}

static GObject*
g_object_constructor (GType                  type,
		      guint                  n_construct_properties,
		      GObjectConstructParam *construct_params)
{
  GObject *object;

  /* create object */
  object = (GObject*) g_type_create_instance (type);

  /* set construction parameters */
  if (n_construct_properties)
    {
      g_object_notify_queue_freeze (object, TRUE);

      /* set construct properties */
      while (n_construct_properties--)
	{
	  GValue *value = construct_params->value;
	  GParamSpec *pspec = construct_params->pspec;

	  construct_params++;
	  object_set_property (object, pspec, value, TRUE, FALSE);
	}

      g_object_notify_queue_thaw (object, FALSE);
      /* the notification queue is still frozen from g_object_init(), so
       * we don't need to handle it here, g_object_newv() takes
       * care of that
       */
    }

  return object;
}

static void
g_object_constructed (GObject *object)
{
  /* empty default impl to allow unconditional upchaining */
}

static inline gboolean
g_object_set_is_valid_property (GObject         *object,
                                GParamSpec      *pspec,
                                const char      *property_name)
{
  if (G_UNLIKELY (pspec == NULL))
    {
      g_critical ("%s: object class '%s' has no property named '%s'",
                  G_STRFUNC, G_OBJECT_TYPE_NAME (object), property_name);
      return FALSE;
    }
  if (G_UNLIKELY (!(pspec->flags & G_PARAM_WRITABLE)))
    {
      g_critical ("%s: property '%s' of object class '%s' is not writable",
                  G_STRFUNC, pspec->name, G_OBJECT_TYPE_NAME (object));
      return FALSE;
    }
  if (G_UNLIKELY (((pspec->flags & G_PARAM_CONSTRUCT_ONLY) && !object_in_construction (object))))
    {
      g_critical ("%s: construct property \"%s\" for object '%s' can't be set after construction",
                  G_STRFUNC, pspec->name, G_OBJECT_TYPE_NAME (object));
      return FALSE;
    }
  return TRUE;
}

/**
 * g_object_setv: (skip)
 * @object: a #GObject
 * @n_properties: the number of properties
 * @names: (array length=n_properties): the names of each property to be set
 * @values: (array length=n_properties): the values of each property to be set
 *
 * Sets @n_properties properties for an @object.
 * Properties to be set will be taken from @values. All properties must be
 * valid. Warnings will be emitted and undefined behaviour may result if invalid
 * properties are passed in.
 *
 * Since: 2.54
 */
void
g_object_setv (GObject       *object,
               guint          n_properties,
               const gchar   *names[],
               const GValue   values[])
{
  guint i;
  gboolean nqueue_is_frozen = FALSE;
  GParamSpec *pspec;
  GObjectClass *class;

  g_return_if_fail (G_IS_OBJECT (object));

  if (n_properties == 0)
    return;

  g_object_ref (object);

  class = G_OBJECT_GET_CLASS (object);

  if (_g_object_has_notify_handler (object))
    {
      g_object_notify_queue_freeze (object, TRUE);
      nqueue_is_frozen = TRUE;
    }

  for (i = 0; i < n_properties; i++)
    {
      pspec = find_pspec (class, names[i]);

      if (!g_object_set_is_valid_property (object, pspec, names[i]))
        break;

      object_set_property (object, pspec, &values[i], nqueue_is_frozen, TRUE);
    }

  if (nqueue_is_frozen)
    g_object_notify_queue_thaw (object, FALSE);

  g_object_unref (object);
}

/**
 * g_object_set_valist: (skip)
 * @object: a #GObject
 * @first_property_name: name of the first property to set
 * @var_args: value for the first property, followed optionally by more
 *  name/value pairs, followed by %NULL
 *
 * Sets properties on an object.
 */
void
g_object_set_valist (GObject	 *object,
		     const gchar *first_property_name,
		     va_list	  var_args)
{
  gboolean nqueue_is_frozen = FALSE;
  const gchar *name;
  GObjectClass *class;

  g_return_if_fail (G_IS_OBJECT (object));

  g_object_ref (object);

  if (_g_object_has_notify_handler (object))
    {
      g_object_notify_queue_freeze (object, TRUE);
      nqueue_is_frozen = TRUE;
    }

  class = G_OBJECT_GET_CLASS (object);

  name = first_property_name;
  while (name)
    {
      GValue value = G_VALUE_INIT;
      GParamSpec *pspec;
      gchar *error = NULL;
      GTypeValueTable *vtab;

      pspec = find_pspec (class, name);

      if (!g_object_set_is_valid_property (object, pspec, name))
        break;

      G_VALUE_COLLECT_INIT2 (&value, vtab, pspec->value_type, var_args, G_VALUE_NOCOPY_CONTENTS, &error);
      if (error)
	{
	  g_critical ("%s: %s", G_STRFUNC, error);
	  g_free (error);
          g_value_unset (&value);
	  break;
	}

      object_set_property (object, pspec, &value, nqueue_is_frozen, TRUE);

      /* We open-code g_value_unset() here to avoid the
       * cost of looking up the GTypeValueTable again.
       */
      if (vtab->value_free)
        vtab->value_free (&value);

      name = va_arg (var_args, gchar*);
    }

  if (nqueue_is_frozen)
    g_object_notify_queue_thaw (object, FALSE);

  g_object_unref (object);
}

static inline gboolean
g_object_get_is_valid_property (GObject          *object,
                                GParamSpec       *pspec,
                                const char       *property_name)
{
  if (G_UNLIKELY (pspec == NULL))
    {
      g_critical ("%s: object class '%s' has no property named '%s'",
                  G_STRFUNC, G_OBJECT_TYPE_NAME (object), property_name);
      return FALSE;
    }
  if (G_UNLIKELY (!(pspec->flags & G_PARAM_READABLE)))
    {
      g_critical ("%s: property '%s' of object class '%s' is not readable",
                  G_STRFUNC, pspec->name, G_OBJECT_TYPE_NAME (object));
      return FALSE;
    }
  return TRUE;
}

/**
 * g_object_getv:
 * @object: a #GObject
 * @n_properties: the number of properties
 * @names: (array length=n_properties): the names of each property to get
 * @values: (array length=n_properties): the values of each property to get
 *
 * Gets @n_properties properties for an @object.
 * Obtained properties will be set to @values. All properties must be valid.
 * Warnings will be emitted and undefined behaviour may result if invalid
 * properties are passed in.
 *
 * Since: 2.54
 */
void
g_object_getv (GObject      *object,
               guint         n_properties,
               const gchar  *names[],
               GValue        values[])
{
  guint i;
  GParamSpec *pspec;
  GObjectClass *class;

  g_return_if_fail (G_IS_OBJECT (object));

  if (n_properties == 0)
    return;

  g_object_ref (object);

  class = G_OBJECT_GET_CLASS (object);

  memset (values, 0, n_properties * sizeof (GValue));

  for (i = 0; i < n_properties; i++)
    {
      pspec = find_pspec (class, names[i]);

      if (!g_object_get_is_valid_property (object, pspec, names[i]))
        break;
      g_value_init (&values[i], pspec->value_type);
      object_get_property (object, pspec, &values[i]);
    }
  g_object_unref (object);
}

/**
 * g_object_get_valist: (skip)
 * @object: a #GObject
 * @first_property_name: name of the first property to get
 * @var_args: return location for the first property, followed optionally by more
 *  name/return location pairs, followed by %NULL
 *
 * Gets properties of an object.
 *
 * In general, a copy is made of the property contents and the caller
 * is responsible for freeing the memory in the appropriate manner for
 * the type, for instance by calling g_free() or g_object_unref().
 *
 * See g_object_get().
 */
void
g_object_get_valist (GObject	 *object,
		     const gchar *first_property_name,
		     va_list	  var_args)
{
  const gchar *name;
  GObjectClass *class;

  g_return_if_fail (G_IS_OBJECT (object));

  g_object_ref (object);

  class = G_OBJECT_GET_CLASS (object);

  name = first_property_name;

  while (name)
    {
      GValue value = G_VALUE_INIT;
      GParamSpec *pspec;
      gchar *error;

      pspec = find_pspec (class, name);

      if (!g_object_get_is_valid_property (object, pspec, name))
        break;

      g_value_init (&value, pspec->value_type);

      object_get_property (object, pspec, &value);

      G_VALUE_LCOPY (&value, var_args, 0, &error);
      if (error)
	{
	  g_critical ("%s: %s", G_STRFUNC, error);
	  g_free (error);
	  g_value_unset (&value);
	  break;
	}

      g_value_unset (&value);

      name = va_arg (var_args, gchar*);
    }

  g_object_unref (object);
}

/**
 * g_object_set: (skip)
 * @object: (type GObject.Object): a #GObject
 * @first_property_name: name of the first property to set
 * @...: value for the first property, followed optionally by more
 *  name/value pairs, followed by %NULL
 *
 * Sets properties on an object.
 *
 * The same caveats about passing integer literals as varargs apply as with
 * g_object_new(). In particular, any integer literals set as the values for
 * properties of type #gint64 or #guint64 must be 64 bits wide, using the
 * %G_GINT64_CONSTANT or %G_GUINT64_CONSTANT macros.
 *
 * Note that the "notify" signals are queued and only emitted (in
 * reverse order) after all properties have been set. See
 * g_object_freeze_notify().
 */
void
g_object_set (gpointer     _object,
	      const gchar *first_property_name,
	      ...)
{
  GObject *object = _object;
  va_list var_args;

  g_return_if_fail (G_IS_OBJECT (object));

  va_start (var_args, first_property_name);
  g_object_set_valist (object, first_property_name, var_args);
  va_end (var_args);
}

/**
 * g_object_get: (skip)
 * @object: (type GObject.Object): a #GObject
 * @first_property_name: name of the first property to get
 * @...: return location for the first property, followed optionally by more
 *  name/return location pairs, followed by %NULL
 *
 * Gets properties of an object.
 *
 * In general, a copy is made of the property contents and the caller
 * is responsible for freeing the memory in the appropriate manner for
 * the type, for instance by calling g_free() or g_object_unref().
 *
 * Here is an example of using g_object_get() to get the contents
 * of three properties: an integer, a string and an object:
 * |[<!-- language="C" -->
 *  gint intval;
 *  guint64 uint64val;
 *  gchar *strval;
 *  GObject *objval;
 *
 *  g_object_get (my_object,
 *                "int-property", &intval,
 *                "uint64-property", &uint64val,
 *                "str-property", &strval,
 *                "obj-property", &objval,
 *                NULL);
 *
 *  // Do something with intval, uint64val, strval, objval
 *
 *  g_free (strval);
 *  g_object_unref (objval);
 * ]|
 */
void
g_object_get (gpointer     _object,
	      const gchar *first_property_name,
	      ...)
{
  GObject *object = _object;
  va_list var_args;

  g_return_if_fail (G_IS_OBJECT (object));

  va_start (var_args, first_property_name);
  g_object_get_valist (object, first_property_name, var_args);
  va_end (var_args);
}

/**
 * g_object_set_property:
 * @object: a #GObject
 * @property_name: the name of the property to set
 * @value: the value
 *
 * Sets a property on an object.
 */
void
g_object_set_property (GObject	    *object,
		       const gchar  *property_name,
		       const GValue *value)
{
  g_object_setv (object, 1, &property_name, value);
}

/**
 * g_object_get_property:
 * @object: a #GObject
 * @property_name: the name of the property to get
 * @value: return location for the property value
 *
 * Gets a property of an object.
 *
 * The @value can be:
 *
 *  - an empty #GValue initialized by %G_VALUE_INIT, which will be
 *    automatically initialized with the expected type of the property
 *    (since GLib 2.60)
 *  - a #GValue initialized with the expected type of the property
 *  - a #GValue initialized with a type to which the expected type
 *    of the property can be transformed
 *
 * In general, a copy is made of the property contents and the caller is
 * responsible for freeing the memory by calling g_value_unset().
 *
 * Note that g_object_get_property() is really intended for language
 * bindings, g_object_get() is much more convenient for C programming.
 */
void
g_object_get_property (GObject	   *object,
		       const gchar *property_name,
		       GValue	   *value)
{
  GParamSpec *pspec;

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (property_name != NULL);
  g_return_if_fail (value != NULL);

  g_object_ref (object);

  pspec = find_pspec (G_OBJECT_GET_CLASS (object), property_name);

  if (g_object_get_is_valid_property (object, pspec, property_name))
    {
      GValue *prop_value, tmp_value = G_VALUE_INIT;

      if (G_VALUE_TYPE (value) == G_TYPE_INVALID)
        {
          /* zero-initialized value */
          g_value_init (value, pspec->value_type);
          prop_value = value;
        }
      else if (G_VALUE_TYPE (value) == pspec->value_type)
        {
          /* auto-conversion of the callers value type */
          g_value_reset (value);
          prop_value = value;
        }
      else if (!g_value_type_transformable (pspec->value_type, G_VALUE_TYPE (value)))
        {
          g_critical ("%s: can't retrieve property '%s' of type '%s' as value of type '%s'",
                      G_STRFUNC, pspec->name,
                      g_type_name (pspec->value_type),
                      G_VALUE_TYPE_NAME (value));
          g_object_unref (object);
          return;
        }
      else
        {
          g_value_init (&tmp_value, pspec->value_type);
          prop_value = &tmp_value;
        }
      object_get_property (object, pspec, prop_value);
      if (prop_value != value)
        {
          g_value_transform (prop_value, value);
          g_value_unset (&tmp_value);
        }
    }

  g_object_unref (object);
}

/**
 * g_object_connect: (skip)
 * @object: (type GObject.Object): a #GObject
 * @signal_spec: the spec for the first signal
 * @...: [type@GObject.Callback] for the first signal, followed by data for the
 *   first signal, followed optionally by more signal
 *   spec/callback/data triples, followed by `NULL`
 *
 * A convenience function to connect multiple signals at once.
 *
 * The signal specs expected by this function have the form
 * `modifier::signal_name`, where `modifier` can be one of the
 * following:
 *
 * - `signal`: equivalent to `g_signal_connect_data (..., NULL, G_CONNECT_DEFAULT)`
 * - `object-signal`, `object_signal`: equivalent to `g_signal_connect_object (..., G_CONNECT_DEFAULT)`
 * - `swapped-signal`, `swapped_signal`: equivalent to `g_signal_connect_data (..., NULL, G_CONNECT_SWAPPED)`
 * - `swapped_object_signal`, `swapped-object-signal`: equivalent to `g_signal_connect_object (..., G_CONNECT_SWAPPED)`
 * - `signal_after`, `signal-after`: equivalent to `g_signal_connect_data (..., NULL, G_CONNECT_AFTER)`
 * - `object_signal_after`, `object-signal-after`: equivalent to `g_signal_connect_object (..., G_CONNECT_AFTER)`
 * - `swapped_signal_after`, `swapped-signal-after`: equivalent to `g_signal_connect_data (..., NULL, G_CONNECT_SWAPPED | G_CONNECT_AFTER)`
 * - `swapped_object_signal_after`, `swapped-object-signal-after`: equivalent to `g_signal_connect_object (..., G_CONNECT_SWAPPED | G_CONNECT_AFTER)`
 *
 * ```c
 * menu->toplevel = g_object_connect (g_object_new (GTK_TYPE_WINDOW,
 *                                                  "type", GTK_WINDOW_POPUP,
 *                                                  "child", menu,
 *                                                  NULL),
 *                                    "signal::event", gtk_menu_window_event, menu,
 *                                    "signal::size_request", gtk_menu_window_size_request, menu,
 *                                    "signal::destroy", gtk_widget_destroyed, &menu->toplevel,
 *                                    NULL);
 * ```
 *
 * Returns: (transfer none) (type GObject.Object): the object
 */
gpointer
g_object_connect (gpointer     _object,
		  const gchar *signal_spec,
		  ...)
{
  GObject *object = _object;
  va_list var_args;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (object->ref_count > 0, object);

  va_start (var_args, signal_spec);
  while (signal_spec)
    {
      GCallback callback = va_arg (var_args, GCallback);
      gpointer data = va_arg (var_args, gpointer);

      if (strncmp (signal_spec, "signal::", 8) == 0)
	g_signal_connect_data (object, signal_spec + 8,
			       callback, data, NULL,
			       G_CONNECT_DEFAULT);
      else if (strncmp (signal_spec, "object_signal::", 15) == 0 ||
               strncmp (signal_spec, "object-signal::", 15) == 0)
	g_signal_connect_object (object, signal_spec + 15,
				 callback, data,
				 G_CONNECT_DEFAULT);
      else if (strncmp (signal_spec, "swapped_signal::", 16) == 0 ||
               strncmp (signal_spec, "swapped-signal::", 16) == 0)
	g_signal_connect_data (object, signal_spec + 16,
			       callback, data, NULL,
			       G_CONNECT_SWAPPED);
      else if (strncmp (signal_spec, "swapped_object_signal::", 23) == 0 ||
               strncmp (signal_spec, "swapped-object-signal::", 23) == 0)
	g_signal_connect_object (object, signal_spec + 23,
				 callback, data,
				 G_CONNECT_SWAPPED);
      else if (strncmp (signal_spec, "signal_after::", 14) == 0 ||
               strncmp (signal_spec, "signal-after::", 14) == 0)
	g_signal_connect_data (object, signal_spec + 14,
			       callback, data, NULL,
			       G_CONNECT_AFTER);
      else if (strncmp (signal_spec, "object_signal_after::", 21) == 0 ||
               strncmp (signal_spec, "object-signal-after::", 21) == 0)
	g_signal_connect_object (object, signal_spec + 21,
				 callback, data,
				 G_CONNECT_AFTER);
      else if (strncmp (signal_spec, "swapped_signal_after::", 22) == 0 ||
               strncmp (signal_spec, "swapped-signal-after::", 22) == 0)
	g_signal_connect_data (object, signal_spec + 22,
			       callback, data, NULL,
			       G_CONNECT_SWAPPED | G_CONNECT_AFTER);
      else if (strncmp (signal_spec, "swapped_object_signal_after::", 29) == 0 ||
               strncmp (signal_spec, "swapped-object-signal-after::", 29) == 0)
	g_signal_connect_object (object, signal_spec + 29,
				 callback, data,
				 G_CONNECT_SWAPPED | G_CONNECT_AFTER);
      else
	{
	  g_critical ("%s: invalid signal spec \"%s\"", G_STRFUNC, signal_spec);
	  break;
	}
      signal_spec = va_arg (var_args, gchar*);
    }
  va_end (var_args);

  return object;
}

/**
 * g_object_disconnect: (skip)
 * @object: (type GObject.Object): a #GObject
 * @signal_spec: the spec for the first signal
 * @...: #GCallback for the first signal, followed by data for the first signal,
 *  followed optionally by more signal spec/callback/data triples,
 *  followed by %NULL
 *
 * A convenience function to disconnect multiple signals at once.
 *
 * The signal specs expected by this function have the form
 * "any_signal", which means to disconnect any signal with matching
 * callback and data, or "any_signal::signal_name", which only
 * disconnects the signal named "signal_name".
 */
void
g_object_disconnect (gpointer     _object,
		     const gchar *signal_spec,
		     ...)
{
  GObject *object = _object;
  va_list var_args;

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (object->ref_count > 0);

  va_start (var_args, signal_spec);
  while (signal_spec)
    {
      GCallback callback = va_arg (var_args, GCallback);
      gpointer data = va_arg (var_args, gpointer);
      guint sid = 0, detail = 0, mask = 0;

      if (strncmp (signal_spec, "any_signal::", 12) == 0 ||
          strncmp (signal_spec, "any-signal::", 12) == 0)
	{
	  signal_spec += 12;
	  mask = G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA;
	}
      else if (strcmp (signal_spec, "any_signal") == 0 ||
               strcmp (signal_spec, "any-signal") == 0)
	{
	  signal_spec += 10;
	  mask = G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA;
	}
      else
	{
	  g_critical ("%s: invalid signal spec \"%s\"", G_STRFUNC, signal_spec);
	  break;
	}

      if ((mask & G_SIGNAL_MATCH_ID) &&
	  !g_signal_parse_name (signal_spec, G_OBJECT_TYPE (object), &sid, &detail, FALSE))
	g_critical ("%s: invalid signal name \"%s\"", G_STRFUNC, signal_spec);
      else if (!g_signal_handlers_disconnect_matched (object, mask | (detail ? G_SIGNAL_MATCH_DETAIL : 0),
						      sid, detail,
						      NULL, (gpointer)callback, data))
	g_critical ("%s: signal handler %p(%p) is not connected", G_STRFUNC, callback, data);
      signal_spec = va_arg (var_args, gchar*);
    }
  va_end (var_args);
}

typedef struct
{
  GWeakNotify notify;
  gpointer data;
} WeakRefTuple;

struct _WeakRefReleaseAllState;

typedef struct _WeakRefReleaseAllState
{
  guint remaining_to_notify;
  struct _WeakRefReleaseAllState *release_all_next;
} WeakRefReleaseAllState;

typedef struct
{
  guint n_weak_refs;
  guint alloc_size;
  WeakRefReleaseAllState *release_all_states;
  WeakRefTuple weak_refs[1]; /* flexible array */
} WeakRefStack;

#define WEAK_REF_STACK_ALLOC_SIZE(alloc_size) (G_STRUCT_OFFSET (WeakRefStack, weak_refs) + sizeof (WeakRefTuple) * (alloc_size))

G_GNUC_UNUSED G_ALWAYS_INLINE static inline gboolean
_weak_ref_release_all_state_contains (WeakRefReleaseAllState *release_all_state, WeakRefReleaseAllState *needle)
{
  for (; release_all_state; release_all_state = release_all_state->release_all_next)
    {
      if (release_all_state == needle)
        return TRUE;
    }
  return FALSE;
}

G_ALWAYS_INLINE static inline void
_weak_ref_stack_free (WeakRefStack *wstack)
{
#ifdef G_ENABLE_DEBUG
  g_assert (!wstack->release_all_states);
#endif
  g_free (wstack);
}

G_ALWAYS_INLINE static inline void
_weak_ref_stack_update_release_all_state (WeakRefStack *wstack, guint idx)
{
  WeakRefReleaseAllState **previous_ptr;
  WeakRefReleaseAllState *release_all_state;

#ifdef G_ENABLE_DEBUG
  g_assert (idx < wstack->n_weak_refs);
#endif

  previous_ptr = &wstack->release_all_states;

  while (G_UNLIKELY ((release_all_state = *previous_ptr)))
    {
      if (idx >= release_all_state->remaining_to_notify)
        {
#ifdef G_ENABLE_DEBUG
          g_assert (release_all_state->remaining_to_notify <= wstack->n_weak_refs);
#endif
          /* We removed an index higher than the "remaining_to_notify" count. */
          goto next;
        }

      /* Lower the "remaining_to_notify" bar of the entries we consider, as we
       * just removed an entry at index @idx (below that bar). */
      release_all_state->remaining_to_notify--;

      if (release_all_state->remaining_to_notify > 0)
        goto next;

      /* Remove the entry from the linked list. No need to reset
       * release_all_state->release_all_next pointer to NULL as it has no
       * purpose when not being linked. */
      *previous_ptr = release_all_state->release_all_next;
      continue;

    next:
      previous_ptr = &release_all_state->release_all_next;
    }
}

static gpointer
g_object_weak_ref_cb (gpointer *data,
                      GDestroyNotify *destroy_notify,
                      gpointer user_data)
{
  WeakRefTuple *tuple = user_data;
  WeakRefStack *wstack = *data;
  guint i;

  if (!wstack)
    {
      wstack = g_malloc (WEAK_REF_STACK_ALLOC_SIZE (1));
      wstack->alloc_size = 1;
      wstack->n_weak_refs = 1;
      wstack->release_all_states = NULL;
      i = 0;

      *data = wstack;
      /* We don't set a @destroy_notify. Shortly before finalize(), we call
       * g_object_weak_release_all(), which frees the WeakRefStack. At that
       * point the ref-count is already at zero and g_object_weak_ref() will
       * assert against being called. This means, we expect that there is
       * never anything to destroy. */
#ifdef G_ENABLE_DEBUG
      *destroy_notify = g_destroy_notify_assert_not_reached;
#endif
    }
  else
    {
      i = wstack->n_weak_refs++;

      if (G_UNLIKELY (wstack->n_weak_refs > wstack->alloc_size))
        {
          if (G_UNLIKELY (wstack->alloc_size >= (G_MAXUINT / 2u + 1u)))
            g_error ("g_object_weak_ref(): cannot register more than 2^31 references");
          wstack->alloc_size = wstack->alloc_size * 2u;

          wstack = g_realloc (wstack, WEAK_REF_STACK_ALLOC_SIZE (wstack->alloc_size));
          *data = wstack;
        }
    }

  wstack->weak_refs[i] = *tuple;

  return NULL;
}

/**
 * g_object_weak_ref: (skip)
 * @object: #GObject to reference weakly
 * @notify: callback to invoke before the object is freed
 * @data: extra data to pass to notify
 *
 * Adds a weak reference callback to an object. Weak references are
 * used for notification when an object is disposed. They are called
 * "weak references" because they allow you to safely hold a pointer
 * to an object without calling g_object_ref() (g_object_ref() adds a
 * strong reference, that is, forces the object to stay alive).
 *
 * Note that the weak references created by this method are not
 * thread-safe: they cannot safely be used in one thread if the
 * object's last g_object_unref() might happen in another thread.
 * Use #GWeakRef if thread-safety is required.
 */
void
g_object_weak_ref (GObject    *object,
		   GWeakNotify notify,
		   gpointer    data)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (notify != NULL);
  g_return_if_fail (g_atomic_int_get (&object->ref_count) >= 1);

  _g_datalist_id_update_atomic (&object->qdata,
                                quark_weak_notifies,
                                g_object_weak_ref_cb,
                                &((WeakRefTuple){
                                    .notify = notify,
                                    .data = data,
                                }));
}

static gpointer
g_object_weak_unref_cb (gpointer *data,
                        GDestroyNotify *destroy_notify,
                        gpointer user_data)
{
  WeakRefTuple *tuple = user_data;
  WeakRefStack *wstack = *data;
  gboolean found_one = FALSE;
  guint i;

  if (wstack)
    {
      for (i = 0; i < wstack->n_weak_refs; i++)
        {
          if (wstack->weak_refs[i].notify != tuple->notify ||
              wstack->weak_refs[i].data != tuple->data)
            continue;

          _weak_ref_stack_update_release_all_state (wstack, i);

          wstack->n_weak_refs -= 1;
          if (wstack->n_weak_refs == 0)
            {
              _weak_ref_stack_free (wstack);
              *data = NULL;
            }
          else
            {
              if (i != wstack->n_weak_refs)
                {
                  memmove (&wstack->weak_refs[i],
                           &wstack->weak_refs[i + 1],
                           sizeof (wstack->weak_refs[i]) * (wstack->n_weak_refs - i));
                }

              if (G_UNLIKELY (wstack->n_weak_refs <= wstack->alloc_size / 4u))
                {
                  wstack->alloc_size = wstack->alloc_size / 2u;
                  wstack = g_realloc (wstack, WEAK_REF_STACK_ALLOC_SIZE (wstack->alloc_size));
                  *data = wstack;
                }
            }

          found_one = TRUE;
          break;
        }
    }

  if (!found_one)
    g_critical ("%s: couldn't find weak ref %p(%p)", G_STRFUNC, tuple->notify, tuple->data);

  return NULL;
}

/**
 * g_object_weak_unref: (skip)
 * @object: #GObject to remove a weak reference from
 * @notify: callback to search for
 * @data: data to search for
 *
 * Removes a weak reference callback to an object.
 */
void
g_object_weak_unref (GObject    *object,
		     GWeakNotify notify,
		     gpointer    data)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (notify != NULL);

  _g_datalist_id_update_atomic (&object->qdata,
                                quark_weak_notifies,
                                g_object_weak_unref_cb,
                                &((WeakRefTuple){
                                    .notify = notify,
                                    .data = data,
                                }));
}

typedef struct
{
  WeakRefReleaseAllState *const release_all_state;
  WeakRefTuple tuple;
  gboolean release_all_done;
} WeakRefReleaseAllData;

static gpointer
g_object_weak_release_all_cb (gpointer *data,
                              GDestroyNotify *destroy_notify,
                              gpointer user_data)
{
  WeakRefStack *wstack = *data;
  WeakRefReleaseAllData *wdata = user_data;
  WeakRefReleaseAllState *release_all_state = wdata->release_all_state;

  if (!wstack)
    return NULL;

#ifdef G_ENABLE_DEBUG
  g_assert (wstack->n_weak_refs > 0);
#endif

  if (release_all_state)
    {
      if (release_all_state->remaining_to_notify == G_MAXUINT)
        {
          if (wstack->n_weak_refs == 1u)
            {
              /* We only pop the single entry. */
              wdata->release_all_done = TRUE;
              release_all_state = NULL;
            }
          else
            {
              release_all_state->remaining_to_notify = wstack->n_weak_refs;

              /* Prepend to linked list. */
              release_all_state->release_all_next = wstack->release_all_states;
              wstack->release_all_states = release_all_state;
            }
        }
      else
        {
          if (release_all_state->remaining_to_notify == 0u)
            {
#ifdef G_ENABLE_DEBUG
              g_assert (!_weak_ref_release_all_state_contains (wstack->release_all_states, release_all_state));
#endif
              return NULL;
            }
#ifdef G_ENABLE_DEBUG
          g_assert (release_all_state->remaining_to_notify <= wstack->n_weak_refs);
          g_assert (_weak_ref_release_all_state_contains (wstack->release_all_states, release_all_state));
#endif
        }
    }

  _weak_ref_stack_update_release_all_state (wstack, 0);

  if (release_all_state && release_all_state->remaining_to_notify == 0)
    wdata->release_all_done = TRUE;

  wstack->n_weak_refs--;

  /* Emit the notifications in FIFO order. */
  wdata->tuple = wstack->weak_refs[0];

  if (wstack->n_weak_refs == 0)
    {
      _weak_ref_stack_free (wstack);
      *data = NULL;

      /* Also set release_all_done.
       *
       * If g_object_weak_release_all() was called during dispose (with
       * release_all FALSE), we anyway have an upper limit of how many
       * notifications we want to pop. We only pop the notifications that were
       * registered when the loop initially starts. In that case, we surely
       * don't want the caller to call back.
       *
       * g_object_weak_release_all() is also being called before finalize. At
       * that point, the ref count is already at zero, and g_object_weak_ref()
       * asserts against being called. So nobody can register a new weak ref
       * anymore.
       *
       * In both cases, we don't require the calling loop to call back. This
       * saves an additional GData lookup. */
      wdata->release_all_done = TRUE;
    }
  else
    {
      memmove (&wstack->weak_refs[0],
               &wstack->weak_refs[1],
               sizeof (wstack->weak_refs[0]) * wstack->n_weak_refs);

      /* Don't bother to shrink the buffer. Most likely the object gets
       * destroyed soon after. */
    }

  return wdata;
}

static void
g_object_weak_release_all (GObject *object, gboolean release_all)
{
  WeakRefReleaseAllState release_all_state = {
    .remaining_to_notify = G_MAXUINT,
  };
  WeakRefReleaseAllData wdata = {
    .release_all_state = release_all ? NULL : &release_all_state,
    .release_all_done = FALSE,
  };

  while (TRUE)
    {
      if (!_g_datalist_id_update_atomic (&object->qdata,
                                         quark_weak_notifies,
                                         g_object_weak_release_all_cb,
                                         &wdata))
        break;

      wdata.tuple.notify (wdata.tuple.data, object);

      if (wdata.release_all_done)
        break;
    }
}

/**
 * g_object_add_weak_pointer: (skip)
 * @object: The object that should be weak referenced.
 * @weak_pointer_location: (inout) (not optional): The memory address
 *    of a pointer.
 *
 * Adds a weak reference from weak_pointer to @object to indicate that
 * the pointer located at @weak_pointer_location is only valid during
 * the lifetime of @object. When the @object is finalized,
 * @weak_pointer will be set to %NULL.
 *
 * Note that as with g_object_weak_ref(), the weak references created by
 * this method are not thread-safe: they cannot safely be used in one
 * thread if the object's last g_object_unref() might happen in another
 * thread. Use #GWeakRef if thread-safety is required.
 */
void
g_object_add_weak_pointer (GObject  *object,
                           gpointer *weak_pointer_location)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (weak_pointer_location != NULL);

  g_object_weak_ref (object,
                     (GWeakNotify) g_nullify_pointer,
                     weak_pointer_location);
}

/**
 * g_object_remove_weak_pointer: (skip)
 * @object: The object that is weak referenced.
 * @weak_pointer_location: (inout) (not optional): The memory address
 *    of a pointer.
 *
 * Removes a weak reference from @object that was previously added
 * using g_object_add_weak_pointer(). The @weak_pointer_location has
 * to match the one used with g_object_add_weak_pointer().
 */
void
g_object_remove_weak_pointer (GObject  *object,
                              gpointer *weak_pointer_location)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (weak_pointer_location != NULL);

  g_object_weak_unref (object,
                       (GWeakNotify) g_nullify_pointer,
                       weak_pointer_location);
}

static guint
object_floating_flag_handler (GObject        *object,
                              gint            job)
{
  switch (job)
    {
      gpointer oldvalue;
    case +1:    /* force floating if possible */
      oldvalue = g_atomic_pointer_get (&object->qdata);
      while (!g_atomic_pointer_compare_and_exchange_full (
        (void**) &object->qdata, oldvalue,
        (void *) ((guintptr) oldvalue | OBJECT_FLOATING_FLAG),
        &oldvalue))
        ;
      return (gsize) oldvalue & OBJECT_FLOATING_FLAG;
    case -1:    /* sink if possible */
      oldvalue = g_atomic_pointer_get (&object->qdata);
      while (!g_atomic_pointer_compare_and_exchange_full (
        (void**) &object->qdata, oldvalue,
        (void *) ((guintptr) oldvalue & ~(gsize) OBJECT_FLOATING_FLAG),
        &oldvalue))
        ;
      return (gsize) oldvalue & OBJECT_FLOATING_FLAG;
    default:    /* check floating */
      return 0 != ((gsize) g_atomic_pointer_get (&object->qdata) & OBJECT_FLOATING_FLAG);
    }
}

/**
 * g_object_is_floating:
 * @object: (type GObject.Object): a #GObject
 *
 * Checks whether @object has a [floating][floating-ref] reference.
 *
 * Since: 2.10
 *
 * Returns: %TRUE if @object has a floating reference
 */
gboolean
g_object_is_floating (gpointer _object)
{
  GObject *object = _object;
  g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
  return (floating_flag_handler (object, 0) != 0);
}

/**
 * g_object_ref_sink:
 * @object: (type GObject.Object): a #GObject
 *
 * Increase the reference count of @object, and possibly remove the
 * [floating][floating-ref] reference, if @object has a floating reference.
 *
 * In other words, if the object is floating, then this call "assumes
 * ownership" of the floating reference, converting it to a normal
 * reference by clearing the floating flag while leaving the reference
 * count unchanged.  If the object is not floating, then this call
 * adds a new normal reference increasing the reference count by one.
 *
 * Since GLib 2.56, the type of @object will be propagated to the return type
 * under the same conditions as for g_object_ref().
 *
 * Since: 2.10
 *
 * Returns: (type GObject.Object) (transfer none): @object
 */
gpointer
(g_object_ref_sink) (gpointer _object)
{
  GObject *object = _object;
  gboolean was_floating;
  g_return_val_if_fail (G_IS_OBJECT (object), object);
  g_return_val_if_fail (g_atomic_int_get (&object->ref_count) >= 1, object);
  g_object_ref (object);
  was_floating = (floating_flag_handler (object, -1) != 0);
  if (was_floating)
    g_object_unref (object);
  return object;
}

/**
 * g_object_take_ref: (skip)
 * @object: (type GObject.Object): a #GObject
 *
 * If @object is floating, sink it.  Otherwise, do nothing.
 *
 * In other words, this function will convert a floating reference (if
 * present) into a full reference.
 *
 * Typically you want to use g_object_ref_sink() in order to
 * automatically do the correct thing with respect to floating or
 * non-floating references, but there is one specific scenario where
 * this function is helpful.
 *
 * The situation where this function is helpful is when creating an API
 * that allows the user to provide a callback function that returns a
 * GObject. We certainly want to allow the user the flexibility to
 * return a non-floating reference from this callback (for the case
 * where the object that is being returned already exists).
 *
 * At the same time, the API style of some popular GObject-based
 * libraries (such as Gtk) make it likely that for newly-created GObject
 * instances, the user can be saved some typing if they are allowed to
 * return a floating reference.
 *
 * Using this function on the return value of the user's callback allows
 * the user to do whichever is more convenient for them. The caller will
 * always receives exactly one full reference to the value: either the
 * one that was returned in the first place, or a floating reference
 * that has been converted to a full reference.
 *
 * This function has an odd interaction when combined with
 * g_object_ref_sink() running at the same time in another thread on
 * the same #GObject instance. If g_object_ref_sink() runs first then
 * the result will be that the floating reference is converted to a hard
 * reference. If g_object_take_ref() runs first then the result will be
 * that the floating reference is converted to a hard reference and an
 * additional reference on top of that one is added. It is best to avoid
 * this situation.
 *
 * Since: 2.70
 *
 * Returns: (type GObject.Object) (transfer full): @object
 */
gpointer
g_object_take_ref (gpointer _object)
{
  GObject *object = _object;
  g_return_val_if_fail (G_IS_OBJECT (object), object);
  g_return_val_if_fail (g_atomic_int_get (&object->ref_count) >= 1, object);

  floating_flag_handler (object, -1);

  return object;
}

/**
 * g_object_force_floating:
 * @object: a #GObject
 *
 * This function is intended for #GObject implementations to re-enforce
 * a [floating][floating-ref] object reference. Doing this is seldom
 * required: all #GInitiallyUnowneds are created with a floating reference
 * which usually just needs to be sunken by calling g_object_ref_sink().
 *
 * Since: 2.10
 */
void
g_object_force_floating (GObject *object)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (g_atomic_int_get (&object->ref_count) >= 1);

  floating_flag_handler (object, +1);
}

typedef struct
{
  GToggleNotify notify;
  gpointer data;
} ToggleRefTuple;

typedef struct
{
  GObject *object;
  ToggleRefTuple tuple;
} ToggleRefCallbackData;

typedef struct
{
  guint n_toggle_refs;
  ToggleRefTuple toggle_refs[1]; /* flexible array */
} ToggleRefStack;

static gpointer
toggle_refs_check_and_ref_cb (gpointer *data,
                              GDestroyNotify *destroy_notify,
                              gpointer user_data)
{
  GToggleNotify *toggle_notify = ((gpointer *) user_data)[0];
  gpointer *toggle_data = ((gpointer *) user_data)[1];
  ToggleRefStack *tstack = *data;

  if (G_UNLIKELY (tstack->n_toggle_refs != 1))
    {
      /* We only reach this line after we checked that the ref-count was 1
       * and that OBJECT_HAS_TOGGLE_REF(). We expect that there is exactly
       * one toggle reference registered. */
      g_critical ("Unexpected number of toggle-refs. g_object_add_toggle_ref() must be paired with g_object_remove_toggle_ref()");
      *toggle_notify = NULL;
      return NULL;
    }

  *toggle_notify = tstack->toggle_refs[0].notify;
  *toggle_data = tstack->toggle_refs[0].data;
  return NULL;
}

G_ALWAYS_INLINE static inline gboolean
toggle_refs_check_and_ref_or_deref (GObject *object,
                                    gboolean is_ref,
                                    gint *old_ref,
                                    GToggleNotify *toggle_notify,
                                    gpointer *toggle_data)
{
  const gint ref_curr = is_ref ? 1 : 2;
  const gint ref_next = is_ref ? 2 : 1;
  gboolean success;

#if G_ENABLE_DEBUG
  g_assert (ref_curr == *old_ref);
#endif

  *toggle_notify = NULL;
  *toggle_data = NULL;

  /* This is called from g_object_ref()/g_object_unref() and a hot path.
   *
   * We hack the GData open and take the g_datalist_lock() outside. Then we
   * perform checks, that most likely will tell us that there is not toggle
   * notifications. Only if we have a toggle notification, we call
   * _g_datalist_id_update_atomic_full(). */

  g_datalist_lock (&object->qdata);

  /* @old_ref is mainly an (out) parameter. On failure to compare-and-exchange,
   * we MUST return the new value which the caller will use for retry.*/

  success = g_atomic_int_compare_and_exchange_full ((int *) &object->ref_count,
                                                    ref_curr,
                                                    ref_next,
                                                    old_ref);

  /* Note that if we are called during g_object_unref (@is_ref set to FALSE),
   * then we drop the ref count from 2 to 1 and give up our reference. We thus
   * no longer hold a strong reference and another thread may race against
   * destroying the object.
   *
   * After this point with is_ref=FALSE and success=TRUE, @object must no
   * longer be accessed.
   *
   * The exception is here. While we still hold the lock, we know that @object
   * could not be destroyed, because g_object_unref() also needs to acquire the
   * same lock before finalizing @object. Thus, we know object cannot yet be
   * destroyed and we can access it until the unlock below. */

  if (G_UNLIKELY (!success))
    {
      g_datalist_unlock (&object->qdata);
      return FALSE;
    }

  if (G_LIKELY (!OBJECT_HAS_TOGGLE_REF (object)))
    {
      g_datalist_unlock (&object->qdata);
      return TRUE;
    }

  /* slow-path. We have a toggle reference. Call into g_datalist_id_update_atomic().
   *
   * Note that _g_datalist_id_update_atomic_full() will release the lock! */
  _g_datalist_id_update_atomic_full (&object->qdata,
                                     quark_toggle_refs,
                                     TRUE,
                                     toggle_refs_check_and_ref_cb,
                                     (gpointer[2]){ toggle_notify, toggle_data });

  return TRUE;
}

static gpointer
toggle_refs_ref_cb (gpointer *data,
                    GDestroyNotify *destroy_notify,
                    gpointer user_data)
{
  ToggleRefCallbackData *trdata = user_data;
  ToggleRefStack *tstack = *data;
  guint i;

  if (!tstack)
    {
      tstack = g_new (ToggleRefStack, 1);
      tstack->n_toggle_refs = 1;
      i = 0;

      g_datalist_set_flags (&trdata->object->qdata, OBJECT_HAS_TOGGLE_REF_FLAG);

      *destroy_notify = g_free;
    }
  else
    {
      i = tstack->n_toggle_refs++;
      tstack = g_realloc (tstack, sizeof (*tstack) + sizeof (tstack->toggle_refs[0]) * i);
    }

  *data = tstack;

  tstack->toggle_refs[i] = trdata->tuple;

  return NULL;
}

/**
 * g_object_add_toggle_ref: (skip)
 * @object: a #GObject
 * @notify: a function to call when this reference is the
 *  last reference to the object, or is no longer
 *  the last reference.
 * @data: data to pass to @notify
 *
 * Increases the reference count of the object by one and sets a
 * callback to be called when all other references to the object are
 * dropped, or when this is already the last reference to the object
 * and another reference is established.
 *
 * This functionality is intended for binding @object to a proxy
 * object managed by another memory manager. This is done with two
 * paired references: the strong reference added by
 * g_object_add_toggle_ref() and a reverse reference to the proxy
 * object which is either a strong reference or weak reference.
 *
 * The setup is that when there are no other references to @object,
 * only a weak reference is held in the reverse direction from @object
 * to the proxy object, but when there are other references held to
 * @object, a strong reference is held. The @notify callback is called
 * when the reference from @object to the proxy object should be
 * "toggled" from strong to weak (@is_last_ref true) or weak to strong
 * (@is_last_ref false).
 *
 * Since a (normal) reference must be held to the object before
 * calling g_object_add_toggle_ref(), the initial state of the reverse
 * link is always strong.
 *
 * Multiple toggle references may be added to the same gobject,
 * however if there are multiple toggle references to an object, none
 * of them will ever be notified until all but one are removed.  For
 * this reason, you should only ever use a toggle reference if there
 * is important state in the proxy object.
 *
 * Note that if you unref the object on another thread, then @notify might
 * still be invoked after g_object_remove_toggle_ref(), and the object argument
 * might be a dangling pointer. If the object is destroyed on other threads,
 * you must take care of that yourself.
 *
 * A g_object_add_toggle_ref() must be released with g_object_remove_toggle_ref().
 *
 * Since: 2.8
 */
void
g_object_add_toggle_ref (GObject       *object,
			 GToggleNotify  notify,
			 gpointer       data)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (notify != NULL);
  g_return_if_fail (g_atomic_int_get (&object->ref_count) >= 1);

  g_object_ref (object);

  _g_datalist_id_update_atomic (&object->qdata,
                                quark_toggle_refs,
                                toggle_refs_ref_cb,
                                &((ToggleRefCallbackData){
                                    .object = object,
                                    .tuple = {
                                        .notify = notify,
                                        .data = data,
                                    },
                                }));
}

static gpointer
toggle_refs_unref_cb (gpointer *data,
                      GDestroyNotify *destroy_notify,
                      gpointer user_data)
{
  ToggleRefCallbackData *trdata = user_data;
  ToggleRefStack *tstack = *data;
  gboolean found_one = FALSE;
  guint i;

  if (tstack)
    {
      for (i = 0; i < tstack->n_toggle_refs; i++)
        {
          if (tstack->toggle_refs[i].notify == trdata->tuple.notify &&
              (tstack->toggle_refs[i].data == trdata->tuple.data || trdata->tuple.data == NULL))
            {
              found_one = TRUE;
              break;
            }
        }
    }

  if (G_LIKELY (found_one))
    {

      tstack->n_toggle_refs -= 1;
      if (tstack->n_toggle_refs == 0)
        {
          g_datalist_unset_flags (&trdata->object->qdata, OBJECT_HAS_TOGGLE_REF_FLAG);
          g_free (tstack);
          *data = NULL;
          *destroy_notify = NULL;
        }
      else if (i != tstack->n_toggle_refs)
        tstack->toggle_refs[i] = tstack->toggle_refs[tstack->n_toggle_refs];
    }

  return GINT_TO_POINTER (found_one);
}

/**
 * g_object_remove_toggle_ref: (skip)
 * @object: a #GObject
 * @notify: a function to call when this reference is the
 *  last reference to the object, or is no longer
 *  the last reference.
 * @data: (nullable): data to pass to @notify, or %NULL to
 *  match any toggle refs with the @notify argument.
 *
 * Removes a reference added with g_object_add_toggle_ref(). The
 * reference count of the object is decreased by one.
 *
 * Note that if you unref the object on another thread, then @notify might
 * still be invoked after g_object_remove_toggle_ref(), and the object argument
 * might be a dangling pointer. If the object is destroyed on other threads,
 * you must take care of that yourself.
 *
 * Since: 2.8
 */
void
g_object_remove_toggle_ref (GObject       *object,
			    GToggleNotify  notify,
			    gpointer       data)
{
  gboolean found_one;
  gpointer result;

  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (notify != NULL);

  result = _g_datalist_id_update_atomic (&object->qdata,
                                         quark_toggle_refs,
                                         toggle_refs_unref_cb,
                                         &((ToggleRefCallbackData){
                                             .object = object,
                                             .tuple = {
                                                 .notify = notify,
                                                 .data = data,
                                             },
                                         }));

  found_one = GPOINTER_TO_INT (result);

  if (!found_one)
    {
      g_critical ("%s: couldn't find toggle ref %p(%p)", G_STRFUNC, notify, data);
      return;
    }

  g_object_unref (object);
}

/* Internal implementation of g_object_ref() which doesn't call out to user code.
 * @out_toggle_notify and @out_toggle_data *must* be provided, and if non-`NULL`
 * values are returned, then the caller *must* call that toggle notify function
 * as soon as it is safe to do so. It may call (or be) user-provided code so should
 * only be called once all locks are released. */
static gpointer
object_ref (GObject *object,
            GToggleNotify *out_toggle_notify,
            gpointer *out_toggle_data)
{
  GToggleNotify toggle_notify;
  gpointer toggle_data;
  gint old_ref;

  old_ref = g_atomic_int_get (&object->ref_count);

retry:
  toggle_notify = NULL;
  toggle_data = NULL;
  if (old_ref > 1 && old_ref < G_MAXINT)
    {
      /* Fast-path. We have apparently more than 1 references already. No
       * special handling for toggle references, just increment the ref count. */
      if (!g_atomic_int_compare_and_exchange_full ((int *) &object->ref_count,
                                                   old_ref, old_ref + 1, &old_ref))
        goto retry;
    }
  else if (old_ref == 1)
    {
      /* With ref count 1, check whether we need to emit a toggle notification. */
      if (!toggle_refs_check_and_ref_or_deref (object, TRUE, &old_ref, &toggle_notify, &toggle_data))
        goto retry;
    }
  else
    {
      gboolean object_already_finalized = TRUE;

      *out_toggle_notify = NULL;
      *out_toggle_data = NULL;
      g_return_val_if_fail (!object_already_finalized, NULL);
      return NULL;
    }

  TRACE (GOBJECT_OBJECT_REF (object, G_TYPE_FROM_INSTANCE (object), old_ref));

  *out_toggle_notify = toggle_notify;
  *out_toggle_data = toggle_data;
  return object;
}

/**
 * g_object_ref:
 * @object: (type GObject.Object): a #GObject
 *
 * Increases the reference count of @object.
 *
 * Since GLib 2.56, if `GLIB_VERSION_MAX_ALLOWED` is 2.56 or greater, the type
 * of @object will be propagated to the return type (using the GCC typeof()
 * extension), so any casting the caller needs to do on the return type must be
 * explicit.
 *
 * Returns: (type GObject.Object) (transfer none): the same @object
 */
gpointer
(g_object_ref) (gpointer _object)
{
  GObject *object = _object;
  GToggleNotify toggle_notify;
  gpointer toggle_data;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);

  object = object_ref (object, &toggle_notify, &toggle_data);

  if (toggle_notify)
    toggle_notify (toggle_data, object, FALSE);

  return object;
}

static gboolean
_object_unref_clear_weak_locations (GObject *object, gint *p_old_ref, gboolean do_unref)
{
  WeakRefData *wrdata;
  gboolean success;

  /* Fast path, for objects that never had a GWeakRef registered. */
  if (!(object_get_optional_flags (object) & OPTIONAL_FLAG_EVER_HAD_WEAK_REF))
    {
      /* The caller previously just checked atomically that the ref-count was
       * one.
       *
       * At this point still, @object never ever had a GWeakRef registered.
       * That means, nobody else holds a strong reference and also nobody else
       * can hold a weak reference, to race against obtaining another
       * reference. We are good to proceed. */
      if (do_unref)
        {
          if (!g_atomic_int_compare_and_exchange ((gint *) &object->ref_count, 1, 0))
            {
#if G_ENABLE_DEBUG
              g_assert_not_reached ();
#endif
            }
        }
      return TRUE;
    }

  /* Slow path. We must obtain a lock on the @wrdata, to atomically release
   * weak references and check that the ref count is as expected. */

  wrdata = weak_ref_data_get_surely (object);

  weak_ref_data_lock (wrdata);

  if (do_unref)
    {
      success = g_atomic_int_compare_and_exchange_full ((gint *) &object->ref_count,
                                                        1, 0,
                                                        p_old_ref);
    }
  else
    {
      *p_old_ref = g_atomic_int_get ((gint *) &object->ref_count);
      success = (*p_old_ref == 1);
    }

  if (success)
    weak_ref_data_clear_list (wrdata, object);

  weak_ref_data_unlock (wrdata);

  return success;
}

/**
 * g_object_unref:
 * @object: (type GObject.Object): a #GObject
 *
 * Decreases the reference count of @object. When its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 *
 * If the pointer to the #GObject may be reused in future (for example, if it is
 * an instance variable of another object), it is recommended to clear the
 * pointer to %NULL rather than retain a dangling pointer to a potentially
 * invalid #GObject instance. Use g_clear_object() for this.
 */
G_LIB_EXPORT
void
g_object_unref (gpointer _object)
{
  GObject *object = _object;
  gint old_ref;
  GToggleNotify toggle_notify;
  gpointer toggle_data;
  gboolean nqueue_is_frozen;
  GType obj_gtype;

  g_return_if_fail (G_IS_OBJECT (object));

  /* obj_gtype will be needed for TRACE(GOBJECT_OBJECT_UNREF()) later. Note
   * that we issue the TRACE() after decrementing the ref-counter. If at that
   * point the reference counter does not reach zero, somebody else can race
   * and destroy the object.
   *
   * This means, TRACE() can be called with a dangling object pointer. This
   * could only be avoided, by emitting the TRACE before doing the actual
   * unref, but at that point we wouldn't know the correct "old_ref" value.
   * Maybe this should change.
   *
   * Anyway. At that later point we can also no longer safely get the GType for
   * the TRACE(). Do it now.
   */
  obj_gtype = G_TYPE_FROM_INSTANCE (object);
  (void) obj_gtype;

  old_ref = g_atomic_int_get (&object->ref_count);

retry_beginning:

  if (old_ref > 2)
    {
      /* We have many references. If we can decrement the ref counter, we are done. */
      if (!g_atomic_int_compare_and_exchange_full ((int *) &object->ref_count,
                                                   old_ref, old_ref - 1, &old_ref))
        goto retry_beginning;

      /* Beware: object might be a dangling pointer. */
      TRACE (GOBJECT_OBJECT_UNREF (object, obj_gtype, old_ref));
      return;
    }

  if (old_ref == 2)
    {
      /* We are about to return the second-to-last reference. In that case we
       * might need to notify a toggle reference.
       *
       * Note that a g_object_add_toggle_ref() MUST always be released
       * via g_object_remove_toggle_ref(). Thus, if we are here with
       * an old_ref of 2, then at most one of the references can be
       * a toggle reference.
       *
       * We need to take a lock, to avoid races. */

      if (!toggle_refs_check_and_ref_or_deref (object, FALSE, &old_ref, &toggle_notify, &toggle_data))
        goto retry_beginning;

      /* Beware: object might be a dangling pointer. */
      TRACE (GOBJECT_OBJECT_UNREF (object, obj_gtype, old_ref));
      if (toggle_notify)
        toggle_notify (toggle_data, object, TRUE);
      return;
    }

  if (G_UNLIKELY (old_ref != 1))
    {
      gboolean object_already_finalized = TRUE;

      g_return_if_fail (!object_already_finalized);
      return;
    }

  /* We only have one reference left. Proceed to (maybe) clear weak locations. */
  if (!_object_unref_clear_weak_locations (object, &old_ref, FALSE))
    goto retry_beginning;

  /* At this point, we checked with an atomic read that we only hold only one
   * reference. Weak locations are cleared (and toggle references are not to
   * be considered in this case). Proceed with dispose().
   *
   * First, freeze the notification queue, so we don't accidentally emit
   * notifications during dispose() and finalize().
   *
   * The notification queue stays frozen unless the instance acquires a
   * reference during dispose(), in which case we thaw it and dispatch all the
   * notifications. If the instance gets through to finalize(), the
   * notification queue gets automatically drained when g_object_finalize() is
   * reached and the qdata is cleared.
   *
   * Important: Note that g_object_notify_queue_freeze() takes an object lock.
   * That happens to be the same lock that is also taken by
   * toggle_refs_check_and_ref_or_deref(), that is very important. See also the
   * code comment in toggle_refs_check_and_ref_or_deref().
   */
  g_object_notify_queue_freeze (object, TRUE);
  nqueue_is_frozen = TRUE;

  TRACE (GOBJECT_OBJECT_DISPOSE (object, G_TYPE_FROM_INSTANCE (object), 1));
  G_OBJECT_GET_CLASS (object)->dispose (object);
  TRACE (GOBJECT_OBJECT_DISPOSE_END (object, G_TYPE_FROM_INSTANCE (object), 1));

  /* Must re-fetch old-ref. _object_unref_clear_weak_locations() relies on
   * that.  */
  old_ref = g_atomic_int_get (&object->ref_count);

retry_decrement:
  /* Here, old_ref is 1 if we just come from dispose(). If the object was resurrected,
   * we can hit `goto retry_decrement` and be here with a larger old_ref. */

  if (old_ref > 1 && nqueue_is_frozen)
    {
      /* If the object was resurrected, we need to unfreeze the notify
       * queue. */
      g_object_notify_queue_thaw (object, FALSE);
      nqueue_is_frozen = FALSE;

      /* Note at this point, @old_ref might be wrong.
       *
       * Also note that _object_unref_clear_weak_locations() requires that we
       * atomically checked that @old_ref is 1. However, as @old_ref is larger
       * than 1, that will not be called. Instead, all other code paths below,
       * handle the possibility of a bogus @old_ref.
       *
       * No need to re-fetch. */
    }

  if (old_ref > 2)
    {
      if (!g_atomic_int_compare_and_exchange_full ((int *) &object->ref_count,
                                                   old_ref, old_ref - 1,
                                                   &old_ref))
        goto retry_decrement;

      /* Beware: object might be a dangling pointer. */
      TRACE (GOBJECT_OBJECT_UNREF (object, obj_gtype, old_ref));
      return;
    }

  if (old_ref == 2)
    {
      /* If the object was resurrected and the current ref-count is 2, then we
       * are about to drop the ref-count to 1. We may need to emit a toggle
       * notification. Take a lock and check for that.
       *
       * In that case, we need a lock to get the toggle notification. */
      if (!toggle_refs_check_and_ref_or_deref (object, FALSE, &old_ref, &toggle_notify, &toggle_data))
        goto retry_decrement;

      /* Beware: object might be a dangling pointer. */
      TRACE (GOBJECT_OBJECT_UNREF (object, obj_gtype, old_ref));
      if (toggle_notify)
        toggle_notify (toggle_data, object, TRUE);
      return;
    }

  /* old_ref is (atomically!) checked to be 1, we are about to drop the
   * reference count to zero in _object_unref_clear_weak_locations(). */
  if (!_object_unref_clear_weak_locations (object, &old_ref, TRUE))
    goto retry_decrement;

  TRACE (GOBJECT_OBJECT_UNREF (object, obj_gtype, old_ref));

  /* The object is almost gone. Finalize. */

  closure_array_destroy_all (object);
  g_signal_handlers_destroy (object);
  g_object_weak_release_all (object, TRUE);

  TRACE (GOBJECT_OBJECT_FINALIZE (object, G_TYPE_FROM_INSTANCE (object)));
  G_OBJECT_GET_CLASS (object)->finalize (object);
  TRACE (GOBJECT_OBJECT_FINALIZE_END (object, G_TYPE_FROM_INSTANCE (object)));

  GOBJECT_IF_DEBUG (OBJECTS,
                    {
                      gboolean was_present;

                      /* catch objects not chaining finalize handlers */
                      G_LOCK (debug_objects);
                      was_present = g_hash_table_remove (debug_objects_ht, object);
                      G_UNLOCK (debug_objects);

                      if (was_present)
                        g_critical ("Object %p of type %s not finalized correctly.",
                                    object, G_OBJECT_TYPE_NAME (object));
                    });
  g_type_free_instance ((GTypeInstance *) object);
}

/**
 * g_clear_object: (skip)
 * @object_ptr: a pointer to a #GObject reference
 *
 * Clears a reference to a #GObject.
 *
 * @object_ptr must not be %NULL.
 *
 * If the reference is %NULL then this function does nothing.
 * Otherwise, the reference count of the object is decreased and the
 * pointer is set to %NULL.
 *
 * A macro is also included that allows this function to be used without
 * pointer casts.
 *
 * Since: 2.28
 **/
#undef g_clear_object
void
g_clear_object (GObject **object_ptr)
{
  g_clear_pointer (object_ptr, g_object_unref);
}

/**
 * g_object_get_qdata:
 * @object: The GObject to get a stored user data pointer from
 * @quark: A #GQuark, naming the user data pointer
 *
 * This function gets back user data pointers stored via
 * g_object_set_qdata().
 *
 * Returns: (transfer none) (nullable): The user data pointer set, or %NULL
 */
gpointer
g_object_get_qdata (GObject *object,
		    GQuark   quark)
{
  g_return_val_if_fail (G_IS_OBJECT (object), NULL);

  return quark ? g_datalist_id_get_data (&object->qdata, quark) : NULL;
}

/**
 * g_object_set_qdata: (skip)
 * @object: The GObject to set store a user data pointer
 * @quark: A #GQuark, naming the user data pointer
 * @data: (nullable): An opaque user data pointer
 *
 * This sets an opaque, named pointer on an object.
 * The name is specified through a #GQuark (retrieved e.g. via
 * g_quark_from_static_string()), and the pointer
 * can be gotten back from the @object with g_object_get_qdata()
 * until the @object is finalized.
 * Setting a previously set user data pointer, overrides (frees)
 * the old pointer set, using #NULL as pointer essentially
 * removes the data stored.
 */
void
g_object_set_qdata (GObject *object,
		    GQuark   quark,
		    gpointer data)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (quark > 0);

  g_datalist_id_set_data (&object->qdata, quark, data);
}

/**
 * g_object_dup_qdata: (skip)
 * @object: the #GObject to store user data on
 * @quark: a #GQuark, naming the user data pointer
 * @dup_func: (nullable): function to dup the value
 * @user_data: (nullable): passed as user_data to @dup_func
 *
 * This is a variant of g_object_get_qdata() which returns
 * a 'duplicate' of the value. @dup_func defines the
 * meaning of 'duplicate' in this context, it could e.g.
 * take a reference on a ref-counted object.
 *
 * If the @quark is not set on the object then @dup_func
 * will be called with a %NULL argument.
 *
 * Note that @dup_func is called while user data of @object
 * is locked.
 *
 * This function can be useful to avoid races when multiple
 * threads are using object data on the same key on the same
 * object.
 *
 * Returns: the result of calling @dup_func on the value
 *     associated with @quark on @object, or %NULL if not set.
 *     If @dup_func is %NULL, the value is returned
 *     unmodified.
 *
 * Since: 2.34
 */
gpointer
g_object_dup_qdata (GObject        *object,
                    GQuark          quark,
                    GDuplicateFunc   dup_func,
                    gpointer         user_data)
{
  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (quark > 0, NULL);

  return g_datalist_id_dup_data (&object->qdata, quark, dup_func, user_data);
}

/**
 * g_object_replace_qdata: (skip)
 * @object: the #GObject to store user data on
 * @quark: a #GQuark, naming the user data pointer
 * @oldval: (nullable): the old value to compare against
 * @newval: (nullable): the new value
 * @destroy: (nullable): a destroy notify for the new value
 * @old_destroy: (out) (optional): destroy notify for the existing value
 *
 * Compares the user data for the key @quark on @object with
 * @oldval, and if they are the same, replaces @oldval with
 * @newval.
 *
 * This is like a typical atomic compare-and-exchange
 * operation, for user data on an object.
 *
 * If the previous value was replaced then ownership of the
 * old value (@oldval) is passed to the caller, including
 * the registered destroy notify for it (passed out in @old_destroy).
 * It’s up to the caller to free this as needed, which may
 * or may not include using @old_destroy as sometimes replacement
 * should not destroy the object in the normal way.
 *
 * Returns: %TRUE if the existing value for @quark was replaced
 *  by @newval, %FALSE otherwise.
 *
 * Since: 2.34
 */
gboolean
g_object_replace_qdata (GObject        *object,
                        GQuark          quark,
                        gpointer        oldval,
                        gpointer        newval,
                        GDestroyNotify  destroy,
                        GDestroyNotify *old_destroy)
{
  g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (quark > 0, FALSE);

  return g_datalist_id_replace_data (&object->qdata, quark,
                                     oldval, newval, destroy,
                                     old_destroy);
}

/**
 * g_object_set_qdata_full: (skip)
 * @object: The GObject to set store a user data pointer
 * @quark: A #GQuark, naming the user data pointer
 * @data: (nullable): An opaque user data pointer
 * @destroy: (nullable): Function to invoke with @data as argument, when @data
 *           needs to be freed
 *
 * This function works like g_object_set_qdata(), but in addition,
 * a void (*destroy) (gpointer) function may be specified which is
 * called with @data as argument when the @object is finalized, or
 * the data is being overwritten by a call to g_object_set_qdata()
 * with the same @quark.
 */
void
g_object_set_qdata_full (GObject       *object,
			 GQuark		quark,
			 gpointer	data,
			 GDestroyNotify destroy)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (quark > 0);

  g_datalist_id_set_data_full (&object->qdata, quark, data,
			       data ? destroy : (GDestroyNotify) NULL);
}

/**
 * g_object_steal_qdata:
 * @object: The GObject to get a stored user data pointer from
 * @quark: A #GQuark, naming the user data pointer
 *
 * This function gets back user data pointers stored via
 * g_object_set_qdata() and removes the @data from object
 * without invoking its destroy() function (if any was
 * set).
 * Usually, calling this function is only required to update
 * user data pointers with a destroy notifier, for example:
 * |[<!-- language="C" -->
 * void
 * object_add_to_user_list (GObject     *object,
 *                          const gchar *new_string)
 * {
 *   // the quark, naming the object data
 *   GQuark quark_string_list = g_quark_from_static_string ("my-string-list");
 *   // retrieve the old string list
 *   GList *list = g_object_steal_qdata (object, quark_string_list);
 *
 *   // prepend new string
 *   list = g_list_prepend (list, g_strdup (new_string));
 *   // this changed 'list', so we need to set it again
 *   g_object_set_qdata_full (object, quark_string_list, list, free_string_list);
 * }
 * static void
 * free_string_list (gpointer data)
 * {
 *   GList *node, *list = data;
 *
 *   for (node = list; node; node = node->next)
 *     g_free (node->data);
 *   g_list_free (list);
 * }
 * ]|
 * Using g_object_get_qdata() in the above example, instead of
 * g_object_steal_qdata() would have left the destroy function set,
 * and thus the partial string list would have been freed upon
 * g_object_set_qdata_full().
 *
 * Returns: (transfer full) (nullable): The user data pointer set, or %NULL
 */
gpointer
g_object_steal_qdata (GObject *object,
		      GQuark   quark)
{
  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (quark > 0, NULL);

  return g_datalist_id_remove_no_notify (&object->qdata, quark);
}

/**
 * g_object_get_data:
 * @object: #GObject containing the associations
 * @key: name of the key for that association
 *
 * Gets a named field from the objects table of associations (see g_object_set_data()).
 *
 * Returns: (transfer none) (nullable): the data if found,
 *          or %NULL if no such data exists.
 */
gpointer
g_object_get_data (GObject     *object,
                   const gchar *key)
{
  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  return g_datalist_get_data (&object->qdata, key);
}

/**
 * g_object_set_data:
 * @object: #GObject containing the associations.
 * @key: name of the key
 * @data: (nullable): data to associate with that key
 *
 * Each object carries around a table of associations from
 * strings to pointers.  This function lets you set an association.
 *
 * If the object already had an association with that name,
 * the old association will be destroyed.
 *
 * Internally, the @key is converted to a #GQuark using g_quark_from_string().
 * This means a copy of @key is kept permanently (even after @object has been
 * finalized) — so it is recommended to only use a small, bounded set of values
 * for @key in your program, to avoid the #GQuark storage growing unbounded.
 */
void
g_object_set_data (GObject     *object,
                   const gchar *key,
                   gpointer     data)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (key != NULL);

  g_datalist_id_set_data (&object->qdata, g_quark_from_string (key), data);
}

/**
 * g_object_dup_data: (skip)
 * @object: the #GObject to store user data on
 * @key: a string, naming the user data pointer
 * @dup_func: (nullable): function to dup the value
 * @user_data: (nullable): passed as user_data to @dup_func
 *
 * This is a variant of g_object_get_data() which returns
 * a 'duplicate' of the value. @dup_func defines the
 * meaning of 'duplicate' in this context, it could e.g.
 * take a reference on a ref-counted object.
 *
 * If the @key is not set on the object then @dup_func
 * will be called with a %NULL argument.
 *
 * Note that @dup_func is called while user data of @object
 * is locked.
 *
 * This function can be useful to avoid races when multiple
 * threads are using object data on the same key on the same
 * object.
 *
 * Returns: the result of calling @dup_func on the value
 *     associated with @key on @object, or %NULL if not set.
 *     If @dup_func is %NULL, the value is returned
 *     unmodified.
 *
 * Since: 2.34
 */
gpointer
g_object_dup_data (GObject        *object,
                   const gchar    *key,
                   GDuplicateFunc   dup_func,
                   gpointer         user_data)
{
  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  return g_datalist_id_dup_data (&object->qdata,
                                 g_quark_from_string (key),
                                 dup_func, user_data);
}

/**
 * g_object_replace_data: (skip)
 * @object: the #GObject to store user data on
 * @key: a string, naming the user data pointer
 * @oldval: (nullable): the old value to compare against
 * @newval: (nullable): the new value
 * @destroy: (nullable): a destroy notify for the new value
 * @old_destroy: (out) (optional): destroy notify for the existing value
 *
 * Compares the user data for the key @key on @object with
 * @oldval, and if they are the same, replaces @oldval with
 * @newval.
 *
 * This is like a typical atomic compare-and-exchange
 * operation, for user data on an object.
 *
 * If the previous value was replaced then ownership of the
 * old value (@oldval) is passed to the caller, including
 * the registered destroy notify for it (passed out in @old_destroy).
 * It’s up to the caller to free this as needed, which may
 * or may not include using @old_destroy as sometimes replacement
 * should not destroy the object in the normal way.
 *
 * See g_object_set_data() for guidance on using a small, bounded set of values
 * for @key.
 *
 * Returns: %TRUE if the existing value for @key was replaced
 *  by @newval, %FALSE otherwise.
 *
 * Since: 2.34
 */
gboolean
g_object_replace_data (GObject        *object,
                       const gchar    *key,
                       gpointer        oldval,
                       gpointer        newval,
                       GDestroyNotify  destroy,
                       GDestroyNotify *old_destroy)
{
  g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  return g_datalist_id_replace_data (&object->qdata,
                                     g_quark_from_string (key),
                                     oldval, newval, destroy,
                                     old_destroy);
}

/**
 * g_object_set_data_full: (skip)
 * @object: #GObject containing the associations
 * @key: name of the key
 * @data: (nullable): data to associate with that key
 * @destroy: (nullable): function to call when the association is destroyed
 *
 * Like g_object_set_data() except it adds notification
 * for when the association is destroyed, either by setting it
 * to a different value or when the object is destroyed.
 *
 * Note that the @destroy callback is not called if @data is %NULL.
 */
void
g_object_set_data_full (GObject       *object,
                        const gchar   *key,
                        gpointer       data,
                        GDestroyNotify destroy)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (key != NULL);

  g_datalist_id_set_data_full (&object->qdata, g_quark_from_string (key), data,
			       data ? destroy : (GDestroyNotify) NULL);
}

/**
 * g_object_steal_data:
 * @object: #GObject containing the associations
 * @key: name of the key
 *
 * Remove a specified datum from the object's data associations,
 * without invoking the association's destroy handler.
 *
 * Returns: (transfer full) (nullable): the data if found, or %NULL
 *          if no such data exists.
 */
gpointer
g_object_steal_data (GObject     *object,
                     const gchar *key)
{
  GQuark quark;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  quark = g_quark_try_string (key);

  return quark ? g_datalist_id_remove_no_notify (&object->qdata, quark) : NULL;
}

static void
g_value_object_init (GValue *value)
{
  value->data[0].v_pointer = NULL;
}

static void
g_value_object_free_value (GValue *value)
{
  g_clear_object ((GObject**) &value->data[0].v_pointer);
}

static void
g_value_object_copy_value (const GValue *src_value,
			   GValue	*dest_value)
{
  g_set_object ((GObject**) &dest_value->data[0].v_pointer,
                src_value->data[0].v_pointer);
}

static void
g_value_object_transform_value (const GValue *src_value,
				GValue       *dest_value)
{
  if (src_value->data[0].v_pointer && g_type_is_a (G_OBJECT_TYPE (src_value->data[0].v_pointer), G_VALUE_TYPE (dest_value)))
    dest_value->data[0].v_pointer = g_object_ref (src_value->data[0].v_pointer);
  else
    dest_value->data[0].v_pointer = NULL;
}

static gpointer
g_value_object_peek_pointer (const GValue *value)
{
  return value->data[0].v_pointer;
}

static gchar*
g_value_object_collect_value (GValue	  *value,
			      guint        n_collect_values,
			      GTypeCValue *collect_values,
			      guint        collect_flags)
{
  if (collect_values[0].v_pointer)
    {
      GObject *object = collect_values[0].v_pointer;

      if (object->g_type_instance.g_class == NULL)
	return g_strconcat ("invalid unclassed object pointer for value type '",
			    G_VALUE_TYPE_NAME (value),
			    "'",
			    NULL);
      else if (!g_value_type_compatible (G_OBJECT_TYPE (object), G_VALUE_TYPE (value)))
	return g_strconcat ("invalid object type '",
			    G_OBJECT_TYPE_NAME (object),
			    "' for value type '",
			    G_VALUE_TYPE_NAME (value),
			    "'",
			    NULL);
      /* never honour G_VALUE_NOCOPY_CONTENTS for ref-counted types */
      value->data[0].v_pointer = g_object_ref (object);
    }
  else
    value->data[0].v_pointer = NULL;

  return NULL;
}

static gchar*
g_value_object_lcopy_value (const GValue *value,
			    guint        n_collect_values,
			    GTypeCValue *collect_values,
			    guint        collect_flags)
{
  GObject **object_p = collect_values[0].v_pointer;

  g_return_val_if_fail (object_p != NULL, g_strdup_printf ("value location for '%s' passed as NULL", G_VALUE_TYPE_NAME (value)));

  if (!value->data[0].v_pointer)
    *object_p = NULL;
  else if (collect_flags & G_VALUE_NOCOPY_CONTENTS)
    *object_p = value->data[0].v_pointer;
  else
    *object_p = g_object_ref (value->data[0].v_pointer);

  return NULL;
}

/**
 * g_value_set_object:
 * @value: a valid #GValue of %G_TYPE_OBJECT derived type
 * @v_object: (type GObject.Object) (nullable): object value to be set
 *
 * Set the contents of a %G_TYPE_OBJECT derived #GValue to @v_object.
 *
 * g_value_set_object() increases the reference count of @v_object
 * (the #GValue holds a reference to @v_object).  If you do not wish
 * to increase the reference count of the object (i.e. you wish to
 * pass your current reference to the #GValue because you no longer
 * need it), use g_value_take_object() instead.
 *
 * It is important that your #GValue holds a reference to @v_object (either its
 * own, or one it has taken) to ensure that the object won't be destroyed while
 * the #GValue still exists).
 */
void
g_value_set_object (GValue   *value,
		    gpointer  v_object)
{
  GObject *old;

  g_return_if_fail (G_VALUE_HOLDS_OBJECT (value));

  if G_UNLIKELY (value->data[0].v_pointer == v_object)
    return;

  old = g_steal_pointer (&value->data[0].v_pointer);

  if (v_object)
    {
      g_return_if_fail (G_IS_OBJECT (v_object));
      g_return_if_fail (g_value_type_compatible (G_OBJECT_TYPE (v_object), G_VALUE_TYPE (value)));

      value->data[0].v_pointer = g_object_ref (v_object);
    }

  g_clear_object (&old);
}

/**
 * g_value_set_object_take_ownership: (skip)
 * @value: a valid #GValue of %G_TYPE_OBJECT derived type
 * @v_object: (nullable): object value to be set
 *
 * This is an internal function introduced mainly for C marshallers.
 *
 * Deprecated: 2.4: Use g_value_take_object() instead.
 */
void
g_value_set_object_take_ownership (GValue  *value,
				   gpointer v_object)
{
  g_value_take_object (value, v_object);
}

/**
 * g_value_take_object: (skip)
 * @value: a valid #GValue of %G_TYPE_OBJECT derived type
 * @v_object: (nullable): object value to be set
 *
 * Sets the contents of a %G_TYPE_OBJECT derived #GValue to @v_object
 * and takes over the ownership of the caller’s reference to @v_object;
 * the caller doesn’t have to unref it any more (i.e. the reference
 * count of the object is not increased).
 *
 * If you want the #GValue to hold its own reference to @v_object, use
 * g_value_set_object() instead.
 *
 * Since: 2.4
 */
void
g_value_take_object (GValue  *value,
		     gpointer v_object)
{
  g_return_if_fail (G_VALUE_HOLDS_OBJECT (value));

  g_clear_object ((GObject **) &value->data[0].v_pointer);

  if (v_object)
    {
      g_return_if_fail (G_IS_OBJECT (v_object));
      g_return_if_fail (g_value_type_compatible (G_OBJECT_TYPE (v_object), G_VALUE_TYPE (value)));

      value->data[0].v_pointer = g_steal_pointer (&v_object);
    }
}

/**
 * g_value_get_object:
 * @value: a valid #GValue of %G_TYPE_OBJECT derived type
 *
 * Get the contents of a %G_TYPE_OBJECT derived #GValue.
 *
 * Returns: (type GObject.Object) (transfer none) (nullable): object contents of @value
 */
gpointer
g_value_get_object (const GValue *value)
{
  g_return_val_if_fail (G_VALUE_HOLDS_OBJECT (value), NULL);

  return value->data[0].v_pointer;
}

/**
 * g_value_dup_object:
 * @value: a valid #GValue whose type is derived from %G_TYPE_OBJECT
 *
 * Get the contents of a %G_TYPE_OBJECT derived #GValue, increasing
 * its reference count. If the contents of the #GValue are %NULL, then
 * %NULL will be returned.
 *
 * Returns: (type GObject.Object) (transfer full) (nullable): object content of @value,
 *          should be unreferenced when no longer needed.
 */
gpointer
g_value_dup_object (const GValue *value)
{
  g_return_val_if_fail (G_VALUE_HOLDS_OBJECT (value), NULL);

  return value->data[0].v_pointer ? g_object_ref (value->data[0].v_pointer) : NULL;
}

/**
 * g_signal_connect_object: (skip)
 * @instance: (type GObject.TypeInstance): the instance to connect to.
 * @detailed_signal: a string of the form "signal-name::detail".
 * @c_handler: the #GCallback to connect.
 * @gobject: (type GObject.Object) (nullable): the object to pass as data
 *    to @c_handler.
 * @connect_flags: a combination of #GConnectFlags.
 *
 * This is similar to g_signal_connect_data(), but uses a closure which
 * ensures that the @gobject stays alive during the call to @c_handler
 * by temporarily adding a reference count to @gobject.
 *
 * When the @gobject is destroyed the signal handler will be automatically
 * disconnected.  Note that this is not currently threadsafe (ie:
 * emitting a signal while @gobject is being destroyed in another thread
 * is not safe).
 *
 * This function cannot fail. If the given signal name doesn’t exist,
 * a critical warning is emitted. No validation is performed on the
 * "detail" string when specified in @detailed_signal, other than a
 * non-empty check.
 *
 * Refer to the [signals documentation](signals.html) for more
 * details.
 *
 * Returns: the handler id.
 */
gulong
g_signal_connect_object (gpointer      instance,
			 const gchar  *detailed_signal,
			 GCallback     c_handler,
			 gpointer      gobject,
			 GConnectFlags connect_flags)
{
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE (instance), 0);
  g_return_val_if_fail (detailed_signal != NULL, 0);
  g_return_val_if_fail (c_handler != NULL, 0);

  if (gobject)
    {
      GClosure *closure;

      g_return_val_if_fail (G_IS_OBJECT (gobject), 0);

      closure = ((connect_flags & G_CONNECT_SWAPPED) ? g_cclosure_new_object_swap : g_cclosure_new_object) (c_handler, gobject);

      return g_signal_connect_closure (instance, detailed_signal, closure, connect_flags & G_CONNECT_AFTER);
    }
  else
    return g_signal_connect_data (instance, detailed_signal, c_handler, NULL, NULL, connect_flags);
}

typedef struct {
  GObject  *object;
  guint     n_closures;
  GClosure *closures[1]; /* flexible array */
} CArray;

static gpointer
object_remove_closure_cb (gpointer *data,
                          GDestroyNotify *destroy_notify,
                          gpointer user_data)
{
  GClosure *closure = user_data;
  CArray *carray = *data;
  guint i;

  for (i = 0; i < carray->n_closures; i++)
    {
      if (carray->closures[i] == closure)
        {
          carray->n_closures--;
          if (carray->n_closures == 0)
            {
              g_free (carray);
              *data = NULL;
            }
          else if (i < carray->n_closures)
            carray->closures[i] = carray->closures[carray->n_closures];
          return NULL;
        }
    }

  g_return_val_if_reached (NULL);
}

static void
object_remove_closure (gpointer data,
                       GClosure *closure)
{
  GObject *object = data;

  _g_datalist_id_update_atomic (&object->qdata,
                                quark_closure_array,
                                object_remove_closure_cb,
                                closure);
}

static gpointer
closure_array_destroy_all_cb (gpointer *data,
                              GDestroyNotify *destroy_notify,
                              gpointer user_data)
{
  CArray *carray = *data;
  GClosure *closure;

  if (!carray)
    return NULL;

  closure = carray->closures[--carray->n_closures];

  if (carray->n_closures == 0)
    {
      g_free (carray);
      *data = NULL;
    }

  return closure;
}

static void
closure_array_destroy_all (GObject *object)
{
  GClosure *closure;

  /* We invalidate closures in a loop. As this emits external callbacks, a callee
   * could register another closure, which the loop would invalidate too.
   *
   * This is an intentional choice. Maybe it would be instead better to only
   * only release the closures that were registered when the loop started. That
   * would be possible, but is not done that way. */
  while ((closure = _g_datalist_id_update_atomic (&object->qdata,
                                                  quark_closure_array,
                                                  closure_array_destroy_all_cb,
                                                  NULL)))
    {
      g_closure_remove_invalidate_notifier (closure, object, object_remove_closure);
      g_closure_invalidate (closure);
    }
}

static gpointer
g_object_watch_closure_cb (gpointer *data,
                           GDestroyNotify *destroy_notify,
                           gpointer user_data)
{
  GObject *object = ((gpointer *) user_data)[0];
  GClosure *closure = ((gpointer *) user_data)[1];
  CArray *carray = *data;
  guint i;

  if (!carray)
    {
      carray = g_new (CArray, 1);
      carray->object = object;
      carray->n_closures = 1;
      i = 0;

#if G_ENABLE_DEBUG
      /* We never expect there is anything to destroy. We require
       * these entries to be released via closure_array_destroy_all(). */
      *destroy_notify = g_destroy_notify_assert_not_reached;
#endif
    }
  else
    {
      i = carray->n_closures++;
      carray = g_realloc (carray, sizeof (*carray) + sizeof (carray->closures[0]) * i);
    }

  *data = carray;

  carray->closures[i] = closure;

  return NULL;
}

/**
 * g_object_watch_closure:
 * @object: #GObject restricting lifetime of @closure
 * @closure: #GClosure to watch
 *
 * This function essentially limits the life time of the @closure to
 * the life time of the object. That is, when the object is finalized,
 * the @closure is invalidated by calling g_closure_invalidate() on
 * it, in order to prevent invocations of the closure with a finalized
 * (nonexisting) object. Also, g_object_ref() and g_object_unref() are
 * added as marshal guards to the @closure, to ensure that an extra
 * reference count is held on @object during invocation of the
 * @closure.  Usually, this function will be called on closures that
 * use this @object as closure data.
 */
void
g_object_watch_closure (GObject  *object,
			GClosure *closure)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (closure != NULL);
  g_return_if_fail (closure->is_invalid == FALSE);
  g_return_if_fail (closure->in_marshal == FALSE);
  g_return_if_fail (g_atomic_int_get (&object->ref_count) > 0); /* this doesn't work on finalizing objects */

  g_closure_add_invalidate_notifier (closure, object, object_remove_closure);
  g_closure_add_marshal_guards (closure,
                                object, (GClosureNotify) g_object_ref,
                                object, (GClosureNotify) g_object_unref);

  _g_datalist_id_update_atomic (&object->qdata,
                                quark_closure_array,
                                g_object_watch_closure_cb,
                                ((gpointer[]){ object, closure }));
}

/**
 * g_closure_new_object:
 * @sizeof_closure: the size of the structure to allocate, must be at least
 *  `sizeof (GClosure)`
 * @object: a #GObject pointer to store in the @data field of the newly
 *  allocated #GClosure
 *
 * A variant of g_closure_new_simple() which stores @object in the
 * @data field of the closure and calls g_object_watch_closure() on
 * @object and the created closure. This function is mainly useful
 * when implementing new types of closures.
 *
 * Returns: (transfer floating): a newly allocated #GClosure
 */
GClosure *
g_closure_new_object (guint    sizeof_closure,
		      GObject *object)
{
  GClosure *closure;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (g_atomic_int_get (&object->ref_count) > 0, NULL);     /* this doesn't work on finalizing objects */

  closure = g_closure_new_simple (sizeof_closure, object);
  g_object_watch_closure (object, closure);

  return closure;
}

/**
 * g_cclosure_new_object: (skip)
 * @callback_func: the function to invoke
 * @object: a #GObject pointer to pass to @callback_func
 *
 * A variant of g_cclosure_new() which uses @object as @user_data and
 * calls g_object_watch_closure() on @object and the created
 * closure. This function is useful when you have a callback closely
 * associated with a #GObject, and want the callback to no longer run
 * after the object is is freed.
 *
 * Returns: (transfer floating): a new #GCClosure
 */
GClosure *
g_cclosure_new_object (GCallback callback_func,
		       GObject  *object)
{
  GClosure *closure;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (g_atomic_int_get (&object->ref_count) > 0, NULL);     /* this doesn't work on finalizing objects */
  g_return_val_if_fail (callback_func != NULL, NULL);

  closure = g_cclosure_new (callback_func, object, NULL);
  g_object_watch_closure (object, closure);

  return closure;
}

/**
 * g_cclosure_new_object_swap: (skip)
 * @callback_func: the function to invoke
 * @object: a #GObject pointer to pass to @callback_func
 *
 * A variant of g_cclosure_new_swap() which uses @object as @user_data
 * and calls g_object_watch_closure() on @object and the created
 * closure. This function is useful when you have a callback closely
 * associated with a #GObject, and want the callback to no longer run
 * after the object is is freed.
 *
 * Returns: (transfer floating): a new #GCClosure
 */
GClosure *
g_cclosure_new_object_swap (GCallback callback_func,
			    GObject  *object)
{
  GClosure *closure;

  g_return_val_if_fail (G_IS_OBJECT (object), NULL);
  g_return_val_if_fail (g_atomic_int_get (&object->ref_count) > 0, NULL);     /* this doesn't work on finalizing objects */
  g_return_val_if_fail (callback_func != NULL, NULL);

  closure = g_cclosure_new_swap (callback_func, object, NULL);
  g_object_watch_closure (object, closure);

  return closure;
}

gsize
g_object_compat_control (gsize           what,
                         gpointer        data)
{
  switch (what)
    {
      gpointer *pp;
    case 1:     /* floating base type */
      return (gsize) G_TYPE_INITIALLY_UNOWNED;
    case 2:     /* FIXME: remove this once GLib/Gtk+ break ABI again */
      floating_flag_handler = (guint(*)(GObject*,gint)) data;
      return 1;
    case 3:     /* FIXME: remove this once GLib/Gtk+ break ABI again */
      pp = data;
      *pp = floating_flag_handler;
      return 1;
    default:
      return 0;
    }
}

G_DEFINE_TYPE (GInitiallyUnowned, g_initially_unowned, G_TYPE_OBJECT)

static void
g_initially_unowned_init (GInitiallyUnowned *object)
{
  g_object_force_floating (object);
}

static void
g_initially_unowned_class_init (GInitiallyUnownedClass *klass)
{
}

/**
 * GWeakRef:
 *
 * A structure containing a weak reference to a #GObject.
 *
 * A `GWeakRef` can either be empty (i.e. point to %NULL), or point to an
 * object for as long as at least one "strong" reference to that object
 * exists. Before the object's #GObjectClass.dispose method is called,
 * every #GWeakRef associated with becomes empty (i.e. points to %NULL).
 *
 * Like #GValue, #GWeakRef can be statically allocated, stack- or
 * heap-allocated, or embedded in larger structures.
 *
 * Unlike g_object_weak_ref() and g_object_add_weak_pointer(), this weak
 * reference is thread-safe: converting a weak pointer to a reference is
 * atomic with respect to invalidation of weak pointers to destroyed
 * objects.
 *
 * If the object's #GObjectClass.dispose method results in additional
 * references to the object being held (‘re-referencing’), any #GWeakRefs taken
 * before it was disposed will continue to point to %NULL.  Any #GWeakRefs taken
 * during disposal and after re-referencing, or after disposal has returned due
 * to the re-referencing, will continue to point to the object until its refcount
 * goes back to zero, at which point they too will be invalidated.
 *
 * It is invalid to take a #GWeakRef on an object during #GObjectClass.dispose
 * without first having or creating a strong reference to the object.
 */

#define WEAK_REF_LOCK_BIT 0

static GObject *
_weak_ref_clean_pointer (gpointer ptr)
{
  /* Drop the lockbit WEAK_REF_LOCK_BIT from @ptr (if set). */
  return g_pointer_bit_lock_mask_ptr (ptr, WEAK_REF_LOCK_BIT, FALSE, 0, NULL);
}

static void
_weak_ref_lock (GWeakRef *weak_ref, GObject **out_object)
{
  /* Note that while holding a _weak_ref_lock() on the @weak_ref, we MUST not acquire a
   * weak_ref_data_lock() on the @wrdata. The other way around! */

  if (out_object)
    {
      guintptr ptr;

      g_pointer_bit_lock_and_get (&weak_ref->priv.p, WEAK_REF_LOCK_BIT, &ptr);
      *out_object = _weak_ref_clean_pointer ((gpointer) ptr);
    }
  else
    g_pointer_bit_lock (&weak_ref->priv.p, WEAK_REF_LOCK_BIT);
}

static void
_weak_ref_unlock (GWeakRef *weak_ref)
{
  g_pointer_bit_unlock (&weak_ref->priv.p, WEAK_REF_LOCK_BIT);
}

static void
_weak_ref_unlock_and_set (GWeakRef *weak_ref, GObject *object)
{
  g_pointer_bit_unlock_and_set (&weak_ref->priv.p, WEAK_REF_LOCK_BIT, object, 0);
}

static void
weak_ref_data_clear_list (WeakRefData *wrdata, GObject *object)
{
  while (wrdata->len > 0u)
    {
      GWeakRef *weak_ref;
      gpointer ptr;

      /* pass "allow_shrink=FALSE", so we don't reallocate needlessly. We
       * anyway are about to clear the entire list. */
      weak_ref = weak_ref_data_list_remove (wrdata, wrdata->len - 1u, FALSE);

      /* Fast-path. Most likely @weak_ref is currently not locked, so we can
       * just atomically set the pointer to NULL. */
      ptr = g_atomic_pointer_get (&weak_ref->priv.p);
#if G_ENABLE_DEBUG
      g_assert (G_IS_OBJECT (_weak_ref_clean_pointer (ptr)));
      g_assert (!object || object == _weak_ref_clean_pointer (ptr));
#endif
      if (G_LIKELY (ptr == _weak_ref_clean_pointer (ptr)))
        {
          /* The pointer is unlocked. Try an atomic compare-and-exchange... */
          if (g_atomic_pointer_compare_and_exchange (&weak_ref->priv.p, ptr, NULL))
            {
              /* Done. Go to the next. */
              continue;
            }
        }

      /* The @weak_ref is locked. Acquire the lock to set the pointer to NULL. */
      _weak_ref_lock (weak_ref, NULL);
      _weak_ref_unlock_and_set (weak_ref, NULL);
    }
}

static void
_weak_ref_set (GWeakRef *weak_ref,
               GObject *new_object,
               gboolean called_by_init)
{
  WeakRefData *old_wrdata;
  WeakRefData *new_wrdata;
  GObject *old_object;

  new_wrdata = weak_ref_data_get_or_create (new_object);

#if G_ENABLE_DEBUG
  g_assert (!new_object || object_get_optional_flags (new_object) & OPTIONAL_FLAG_EVER_HAD_WEAK_REF);
#endif

  if (called_by_init)
    {
      /* The caller is g_weak_ref_init(). We know that the weak_ref should be
       * NULL. We thus set @old_wrdata to NULL without checking.
       *
       * Also important, the caller ensured that @new_object is not NULL. So we
       * are expected to set @weak_ref from NULL to a non-NULL @new_object. */
      old_wrdata = NULL;
#if G_ENABLE_DEBUG
      g_assert (new_object);
#endif
    }
  else
    {
      /* We must get a wrdata object @old_wrdata for the current @old_object. */
      _weak_ref_lock (weak_ref, &old_object);

      if (old_object == new_object)
        {
          /* Already set. We are done. */
          _weak_ref_unlock (weak_ref);
          return;
        }

      old_wrdata = old_object
                       ? weak_ref_data_ref (weak_ref_data_get (old_object))
                       : NULL;
      _weak_ref_unlock (weak_ref);
    }

  /* We need a lock on @old_wrdata, @new_wrdata and @weak_ref. We need to take
   * these locks in a certain order to avoid deadlock. We sort them by pointer
   * value.
   *
   * Note that @old_wrdata or @new_wrdata may be NULL, which is handled
   * correctly.
   *
   * Note that @old_wrdata and @new_wrdata are never identical at this point.
   */
  if (new_wrdata && old_wrdata && (((guintptr) (gpointer) old_wrdata) < ((guintptr) ((gpointer) new_wrdata))))
    {
      weak_ref_data_lock (old_wrdata);
      weak_ref_data_lock (new_wrdata);
    }
  else
    {
      weak_ref_data_lock (new_wrdata);
      weak_ref_data_lock (old_wrdata);
    }
  _weak_ref_lock (weak_ref, &old_object);

  if (!weak_ref_data_has (old_object, old_wrdata, NULL))
    {
      /* A race. @old_object no longer has the expected @old_wrdata after
       * getting all the locks. */
      if (old_object)
        {
          /* We lost the race and find a different object set. It's fine, our
           * action was lost in the race and we are done. No need to retry. */
          weak_ref_data_unlock (old_wrdata);
          weak_ref_data_unlock (new_wrdata);
          _weak_ref_unlock (weak_ref);
          weak_ref_data_unref (old_wrdata);
          return;
        }

      /* @old_object is NULL after a race. We didn't expect that, but it's
       * fine. Proceed to set @new_object... */
    }

  if (old_object)
    {
      gint32 idx;

      idx = weak_ref_data_list_find (old_wrdata, weak_ref);
      if (idx < 0)
        g_critical ("unexpected missing GWeakRef data");
      else
        weak_ref_data_list_remove (old_wrdata, idx, TRUE);
    }

  weak_ref_data_unlock (old_wrdata);

  if (new_object)
    {
#if G_ENABLE_DEBUG
      g_assert (new_wrdata != NULL);
      g_assert (weak_ref_data_list_find (new_wrdata, weak_ref) < 0);
#endif
      if (g_atomic_int_get (&new_object->ref_count) < 1)
        {
          g_critical ("calling g_weak_ref_set() with already destroyed object");
          new_object = NULL;
        }
      else
        {
          if (!weak_ref_data_list_add (new_wrdata, weak_ref))
            {
              g_critical ("Too many GWeakRef registered");
              new_object = NULL;
            }
        }
    }

  _weak_ref_unlock_and_set (weak_ref, new_object);
  weak_ref_data_unlock (new_wrdata);

  weak_ref_data_unref (old_wrdata);
}

/**
 * g_weak_ref_init: (skip)
 * @weak_ref: uninitialized or empty location for a weak reference
 * @object: (type GObject.Object) (nullable): a #GObject or %NULL
 *
 * Initialise a non-statically-allocated #GWeakRef.
 *
 * This function also calls g_weak_ref_set() with @object on the
 * freshly-initialised weak reference.
 *
 * This function should always be matched with a call to
 * g_weak_ref_clear().  It is not necessary to use this function for a
 * #GWeakRef in static storage because it will already be
 * properly initialised.  Just use g_weak_ref_set() directly.
 *
 * Since: 2.32
 */
void
g_weak_ref_init (GWeakRef *weak_ref,
                 gpointer object)
{
  g_return_if_fail (weak_ref);
  g_return_if_fail (object == NULL || G_IS_OBJECT (object));

  g_atomic_pointer_set (&weak_ref->priv.p, NULL);
  if (object)
    {
      /* We give a hint that the weak_ref is currently NULL. Unlike
       * g_weak_ref_set(), we then don't need the extra lock just to
       * find out that we have no object. */
      _weak_ref_set (weak_ref, object, TRUE);
    }
}

/**
 * g_weak_ref_clear: (skip)
 * @weak_ref: location of a weak reference, which
 *  may be empty
 *
 * Frees resources associated with a non-statically-allocated #GWeakRef.
 * After this call, the #GWeakRef is left in an undefined state.
 *
 * You should only call this on a #GWeakRef that previously had
 * g_weak_ref_init() called on it.
 *
 * Since: 2.32
 */
void
g_weak_ref_clear (GWeakRef *weak_ref)
{
  g_weak_ref_set (weak_ref, NULL);

  /* be unkind */
  weak_ref->priv.p = (void *) 0xccccccccu;
}

/**
 * g_weak_ref_get: (skip)
 * @weak_ref: location of a weak reference to a #GObject
 *
 * If @weak_ref is not empty, atomically acquire a strong
 * reference to the object it points to, and return that reference.
 *
 * This function is needed because of the potential race between taking
 * the pointer value and g_object_ref() on it, if the object was losing
 * its last reference at the same time in a different thread.
 *
 * The caller should release the resulting reference in the usual way,
 * by using g_object_unref().
 *
 * Returns: (transfer full) (type GObject.Object): the object pointed to
 *     by @weak_ref, or %NULL if it was empty
 *
 * Since: 2.32
 */
gpointer
g_weak_ref_get (GWeakRef *weak_ref)
{
  WeakRefData *wrdata;
  WeakRefData *new_wrdata;
  GToggleNotify toggle_notify = NULL;
  gpointer toggle_data = NULL;
  GObject *object;

  g_return_val_if_fail (weak_ref, NULL);

  /* We cannot take the strong reference on @object yet. Otherwise,
   * _object_unref_clear_weak_locations() might have just taken the lock on
   * @wrdata, see that the ref-count is 1 and plan to proceed clearing weak
   * locations. If we then take a strong reference here, the object becomes
   * alive and well, but _object_unref_clear_weak_locations() would proceed and
   * clear the @weak_ref.
   *
   * We avoid that, by can only taking the strong reference when having a lock
   * on @wrdata, so we are in sync with _object_unref_clear_weak_locations().
   *
   * But first we must get a reference to the @wrdata.
   */
  _weak_ref_lock (weak_ref, &object);
  wrdata = object
               ? weak_ref_data_ref (weak_ref_data_get (object))
               : NULL;
  _weak_ref_unlock (weak_ref);

  if (!wrdata)
    {
      /* There is no @wrdata and no object. We are done. */
      return NULL;
    }

retry:

  /* Now proceed to get the strong reference. This time with acquiring a lock
   * on the per-object @wrdata and on @weak_ref.
   *
   * As the order in which locks are taken is important, we previously had to
   * get a _weak_ref_lock(), to obtain the @wrdata. Now we have to lock on the
   * @wrdata first, and the @weak_ref again. */
  weak_ref_data_lock (wrdata);
  _weak_ref_lock (weak_ref, &object);

  if (!object)
    {
      /* Object is gone in the meantime. That is fine. */
      new_wrdata = NULL;
    }
  else
    {
      /* Check that @object still refers to the same object as before. We do
       * that by comparing the @wrdata object. A GObject keeps its (unique!)
       * wrdata instance until the end, and since @wrdata is still alive,
       * @object is the same as before, if-and-only-if its @wrdata is the same.
       */
      if (weak_ref_data_has (object, wrdata, &new_wrdata))
        {
          /* We are (still) good. Take a strong ref while holding the necessary locks. */
          object = object_ref (object, &toggle_notify, &toggle_data);
        }
      else
        {
          /* The @object changed and has no longer the same @wrdata. In this
           * case, we need to start over.
           *
           * Note that @new_wrdata references the wrdata of the now current
           * @object. We will use that during the retry. */
        }
    }

  _weak_ref_unlock (weak_ref);
  weak_ref_data_unlock (wrdata);
  weak_ref_data_unref (wrdata);

  if (new_wrdata)
    {
      /* There was a race. The object changed. Retry, with @new_wrdata. */
      wrdata = new_wrdata;
      goto retry;
    }

  if (toggle_notify)
    toggle_notify (toggle_data, object, FALSE);

  return object;
}

/**
 * g_weak_ref_set: (skip)
 * @weak_ref: location for a weak reference
 * @object: (type GObject.Object) (nullable): a #GObject or %NULL
 *
 * Change the object to which @weak_ref points, or set it to
 * %NULL.
 *
 * You must own a strong reference on @object while calling this
 * function.
 *
 * Since: 2.32
 */
void
g_weak_ref_set (GWeakRef *weak_ref,
                gpointer object)
{
  g_return_if_fail (weak_ref != NULL);
  g_return_if_fail (object == NULL || G_IS_OBJECT (object));

  _weak_ref_set (weak_ref, object, FALSE);
}
