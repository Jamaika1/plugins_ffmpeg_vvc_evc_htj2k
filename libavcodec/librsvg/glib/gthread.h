/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_THREAD_H__
#define __G_THREAD_H__

#if !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif

#include "gatomic.h"
#include "gerror.h"
#include "gutils.h"

G_BEGIN_DECLS

#define G_THREAD_ERROR g_thread_error_quark ()
GLIB_AVAILABLE_IN_ALL
GQuark g_thread_error_quark (void);

typedef enum
{
  G_THREAD_ERROR_AGAIN /* Resource temporarily unavailable */
} GThreadError;

typedef gpointer (*GThreadFunc) (gpointer data);

typedef struct _GThread         GThread;

typedef union  _GMutex          GMutex;
typedef struct _GRecMutex       GRecMutex;
typedef struct _GRWLock         GRWLock;
typedef struct _GCond           GCond;
typedef struct _GPrivate        GPrivate;
typedef struct _GOnce           GOnce;

union _GMutex
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

struct _GRWLock
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

struct _GCond
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

struct _GRecMutex
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

#define G_PRIVATE_INIT(notify) { NULL, (notify), { NULL, NULL } }
struct _GPrivate
{
  /*< private >*/
  gpointer       p;
  GDestroyNotify notify;
  gpointer future[2];
};

typedef enum
{
  G_ONCE_STATUS_NOTCALLED,
  G_ONCE_STATUS_PROGRESS,
  G_ONCE_STATUS_READY
} GOnceStatus;

#define G_ONCE_INIT { G_ONCE_STATUS_NOTCALLED, NULL }
struct _GOnce
{
  volatile GOnceStatus status;
  volatile gpointer retval;
};

#define G_LOCK_NAME(name)             g__ ## name ## _lock
#define G_LOCK_DEFINE_STATIC(name)    static G_LOCK_DEFINE (name)
#define G_LOCK_DEFINE(name)           GMutex G_LOCK_NAME (name)
#define G_LOCK_EXTERN(name)           extern GMutex G_LOCK_NAME (name)

#ifdef G_DEBUG_LOCKS
#  define G_LOCK(name)                G_STMT_START{             \
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                   \
             "file %s: line %d (%s): locking: %s ",             \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name);                                            \
      g_mutex_lock (&G_LOCK_NAME (name));                       \
   }G_STMT_END
#  define G_UNLOCK(name)              G_STMT_START{             \
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                   \
             "file %s: line %d (%s): unlocking: %s ",           \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name);                                            \
     g_mutex_unlock (&G_LOCK_NAME (name));                      \
   }G_STMT_END
#  define G_TRYLOCK(name)                                       \
      (g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                  \
             "file %s: line %d (%s): try locking: %s ",         \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name), g_mutex_trylock (&G_LOCK_NAME (name)))
#else  /* !G_DEBUG_LOCKS */
#  define G_LOCK(name) g_mutex_lock       (&G_LOCK_NAME (name))
#  define G_UNLOCK(name) g_mutex_unlock   (&G_LOCK_NAME (name))
#  define G_TRYLOCK(name) g_mutex_trylock (&G_LOCK_NAME (name))
#endif /* !G_DEBUG_LOCKS */

#ifdef g_autoptr
#define G_AUTO_LOCK(name) G_MUTEX_AUTO_LOCK (&G_LOCK_NAME (name), g__##name##_locker)
#endif /* g_autoptr */

GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_ref                    (GThread        *thread);
GLIB_AVAILABLE_IN_2_32
void            g_thread_unref                  (GThread        *thread);
GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_new                    (const gchar    *name,
                                                 GThreadFunc     func,
                                                 gpointer        data);
GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_try_new                (const gchar    *name,
                                                 GThreadFunc     func,
                                                 gpointer        data,
                                                 GError        **error);
GLIB_AVAILABLE_IN_ALL
GThread *       g_thread_self                   (void);
G_NORETURN GLIB_AVAILABLE_IN_ALL
void            g_thread_exit                   (gpointer        retval);
GLIB_AVAILABLE_IN_ALL
gpointer        g_thread_join                   (GThread        *thread);
GLIB_AVAILABLE_IN_ALL
void            g_thread_yield                  (void);

GLIB_AVAILABLE_IN_2_84
const char *    g_thread_get_name               (GThread        *thread);

GLIB_AVAILABLE_IN_2_32
void            g_mutex_init                    (GMutex         *mutex);
GLIB_AVAILABLE_IN_2_32
void            g_mutex_clear                   (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_mutex_lock                    (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
gboolean        g_mutex_trylock                 (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_mutex_unlock                  (GMutex         *mutex);

GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_init                  (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_clear                 (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_writer_lock           (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rw_lock_writer_trylock        (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_writer_unlock         (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_reader_lock           (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rw_lock_reader_trylock        (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_reader_unlock         (GRWLock        *rw_lock);

GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_init                (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_clear               (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_lock                (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rec_mutex_trylock             (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_unlock              (GRecMutex      *rec_mutex);

GLIB_AVAILABLE_IN_2_32
void            g_cond_init                     (GCond          *cond);
GLIB_AVAILABLE_IN_2_32
void            g_cond_clear                    (GCond          *cond);
GLIB_AVAILABLE_IN_ALL
void            g_cond_wait                     (GCond          *cond,
                                                 GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_cond_signal                   (GCond          *cond);
GLIB_AVAILABLE_IN_ALL
void            g_cond_broadcast                (GCond          *cond);
GLIB_AVAILABLE_IN_2_32
gboolean        g_cond_wait_until               (GCond          *cond,
                                                 GMutex         *mutex,
                                                 gint64          end_time);

GLIB_AVAILABLE_IN_ALL
gpointer        g_private_get                   (GPrivate       *key);
GLIB_AVAILABLE_IN_ALL
void            g_private_set                   (GPrivate       *key,
                                                 gpointer        value);
GLIB_AVAILABLE_IN_2_32
void            g_private_replace               (GPrivate       *key,
                                                 gpointer        value);

GLIB_AVAILABLE_IN_ALL
gpointer        g_once_impl                     (GOnce          *once,
                                                 GThreadFunc     func,
                                                 gpointer        arg);
GLIB_AVAILABLE_IN_ALL
gboolean        g_once_init_enter               (volatile void  *location);
GLIB_AVAILABLE_IN_ALL
void            g_once_init_leave               (volatile void  *location,
                                                 gsize           result);

GLIB_AVAILABLE_IN_2_80
gboolean g_once_init_enter_pointer              (void *location);
GLIB_AVAILABLE_IN_2_80
void g_once_init_leave_pointer                  (void *location,
                                                 gpointer result);

/* Use C11-style atomic extensions to check the fast path for status=ready. If
 * they are not available, fall back to using a mutex and condition variable in
 * g_once_impl().
 *
 * On the C11-style codepath, only the load of once->status needs to be atomic,
 * as the writes to it and once->retval in g_once_impl() are related by a
 * happens-before relation. Release-acquire semantics are defined such that any
 * atomic/non-atomic write which happens-before a store/release is guaranteed to
 * be seen by the load/acquire of the same atomic variable. */
#if defined(G_ATOMIC_LOCK_FREE) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && defined(__ATOMIC_SEQ_CST)
# define g_once(once, func, arg) \
  ((__atomic_load_n (&(once)->status, __ATOMIC_ACQUIRE) == G_ONCE_STATUS_READY) ? \
   (once)->retval : \
   g_once_impl ((once), (func), (arg)))
#else
# define g_once(once, func, arg) g_once_impl ((once), (func), (arg))
#endif

#ifdef __GNUC__
# define g_once_init_enter(location) \
  (G_GNUC_EXTENSION ({                                               \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer));       \
    (void) (0 ? (gpointer) *(location) : NULL);                      \
    (!g_atomic_pointer_get (location) &&                             \
     g_once_init_enter (location));                                  \
  }))
# define g_once_init_leave(location, result) \
  (G_GNUC_EXTENSION ({                                               \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer));       \
    0 ? (void) (*(location) = (result)) : (void) 0;                  \
    g_once_init_leave ((location), (gsize) (result));                \
  }))
# define g_once_init_enter_pointer(location)                   \
  (G_GNUC_EXTENSION ({                                         \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer)); \
    (void) (0 ? (gpointer) * (location) : NULL);               \
    (!g_atomic_pointer_get (location) &&                       \
     g_once_init_enter_pointer (location));                    \
  })) GLIB_AVAILABLE_MACRO_IN_2_80
# define g_once_init_leave_pointer(location, result)                        \
  (G_GNUC_EXTENSION ({                                                      \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer));              \
    0 ? (void) (*(location) = (result)) : (void) 0;                         \
    g_once_init_leave_pointer ((location), (gpointer) (guintptr) (result)); \
  })) GLIB_AVAILABLE_MACRO_IN_2_80
#else
# define g_once_init_enter(location) \
  (g_once_init_enter((location)))
# define g_once_init_leave(location, result) \
  (g_once_init_leave((location), (gsize) (result)))
# define g_once_init_enter_pointer(location) \
  (g_once_init_enter_pointer((location))) \
  GLIB_AVAILABLE_MACRO_IN_2_80
# define g_once_init_leave_pointer(location, result) \
  (g_once_init_leave_pointer((location), (gpointer) (guintptr) (result))) \
  GLIB_AVAILABLE_MACRO_IN_2_80
#endif

GLIB_AVAILABLE_IN_2_36
guint          g_get_num_processors (void);

/**
 * GMutexLocker:
 *
 * Opaque type. See g_mutex_locker_new() for details.
 * Since: 2.44
 */
typedef void GMutexLocker;

/**
 * g_mutex_locker_new:
 * @mutex: a mutex to lock
 *
 * Lock @mutex and return a new #GMutexLocker. Unlock with
 * g_mutex_locker_free(). Using g_mutex_unlock() on @mutex
 * while a #GMutexLocker exists can lead to undefined behaviour.
 *
 * No allocation is performed, it is equivalent to a g_mutex_lock() call.
 *
 * This is intended to be used with g_autoptr().  Note that g_autoptr()
 * is only available when using GCC or clang, so the following example
 * will only work with those compilers:
 * |[
 * typedef struct
 * {
 *   ...
 *   GMutex mutex;
 *   ...
 * } MyObject;
 *
 * static void
 * my_object_do_stuff (MyObject *self)
 * {
 *   g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&self->mutex);
 *
 *   // Code with mutex locked here
 *
 *   if (condition)
 *     // No need to unlock
 *     return;
 *
 *   // Optionally early unlock
 *   g_clear_pointer (&locker, g_mutex_locker_free);
 *
 *   // Code with mutex unlocked here
 * }
 * ]|
 *
 * Note that it is common for the declared variable to not be used in the scope,
 * which causes some compilers to warn. That can be avoided by using
 * `G_GNUC_UNUSED` or, since 2.80, [func@GLib.MUTEX_AUTO_LOCK].
 *
 * Returns: a #GMutexLocker
 * Since: 2.44
 */
GLIB_AVAILABLE_STATIC_INLINE_IN_2_44
static inline GMutexLocker *
g_mutex_locker_new (GMutex *mutex)
{
  g_mutex_lock (mutex);
  return (GMutexLocker *) mutex;
}

/**
 * g_mutex_locker_free:
 * @locker: a GMutexLocker
 *
 * Unlock @locker's mutex. See g_mutex_locker_new() for details.
 *
 * No memory is freed, it is equivalent to a g_mutex_unlock() call.
 *
 * Since: 2.44
 */
GLIB_AVAILABLE_STATIC_INLINE_IN_2_44
static inline void
g_mutex_locker_free (GMutexLocker *locker)
{
  g_mutex_unlock ((GMutex *) locker);
}

/**
 * G_MUTEX_AUTO_LOCK:
 * @mutex: a [type@GLib.Mutex]
 * @var: a variable name to be declared
 *
 * Declare a [type@GLib.MutexLocker] variable with `g_autoptr()` and lock the
 * mutex. The mutex will be unlocked automatically when leaving the scope. The
 * variable is declared with `G_GNUC_UNUSED` to avoid compiler warning if it is
 * not used in the scope.
 *
 * This feature is only supported on GCC and clang. This macro is not defined on
 * other compilers and should not be used in programs that are intended to be
 * portable to those compilers.
 *
 * Note that this should be used in a place where it is allowed to declare a
 * variable, which could be before any statement in the case
 * `-Wdeclaration-after-statement` is used, or C standard prior to C99.
 *
 * ```c
 * {
 *   G_MUTEX_AUTO_LOCK (&obj->mutex, locker);
 *
 *   obj->stuff_with_lock ();
 *   if (condition)
 *     {
 *       // No need to unlock
 *       return;
 *     }
 *
 *   // Unlock before end of scope
 *   g_clear_pointer (&locker, g_mutex_locker_free);
 *   obj->stuff_without_lock ();
 * }
 * ```
 *
 * Since: 2.80.0
 */
#ifdef g_autoptr
#define G_MUTEX_AUTO_LOCK(mutex, var)                   \
  GLIB_AVAILABLE_MACRO_IN_2_80 g_autoptr (GMutexLocker) \
  G_GNUC_UNUSED var = g_mutex_locker_new (mutex)
#endif /* g_autoptr */

/**
 * GRecMutexLocker:
 *
 * Opaque type. See g_rec_mutex_locker_new() for details.
 * Since: 2.60
 */
typedef void GRecMutexLocker;

/**
 * g_rec_mutex_locker_new:
 * @rec_mutex: a recursive mutex to lock
 *
 * Lock @rec_mutex and return a new #GRecMutexLocker. Unlock with
 * g_rec_mutex_locker_free(). Using g_rec_mutex_unlock() on @rec_mutex
 * while a #GRecMutexLocker exists can lead to undefined behaviour.
 *
 * No allocation is performed, it is equivalent to a g_rec_mutex_lock() call.
 *
 * This is intended to be used with g_autoptr().  Note that g_autoptr()
 * is only available when using GCC or clang, so the following example
 * will only work with those compilers:
 * |[
 * typedef struct
 * {
 *   ...
 *   GRecMutex rec_mutex;
 *   ...
 * } MyObject;
 *
 * static void
 * my_object_do_stuff (MyObject *self)
 * {
 *   g_autoptr(GRecMutexLocker) locker = g_rec_mutex_locker_new (&self->rec_mutex);
 *
 *   // Code with rec_mutex locked here
 *
 *   if (condition)
 *     // No need to unlock
 *     return;
 *
 *   // Optionally early unlock
 *   g_clear_pointer (&locker, g_rec_mutex_locker_free);
 *
 *   // Code with rec_mutex unlocked here
 * }
 * ]|
 *
 * Note that it is common for the declared variable to not be used in the scope,
 * which causes some compilers to warn. That can be avoided by using
 * `G_GNUC_UNUSED` or, since 2.80, [func@GLib.REC_MUTEX_AUTO_LOCK].
 *
 * Returns: a #GRecMutexLocker
 * Since: 2.60
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_60
static inline GRecMutexLocker *
g_rec_mutex_locker_new (GRecMutex *rec_mutex)
{
  g_rec_mutex_lock (rec_mutex);
  return (GRecMutexLocker *) rec_mutex;
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_rec_mutex_locker_free:
 * @locker: a GRecMutexLocker
 *
 * Unlock @locker's recursive mutex. See g_rec_mutex_locker_new() for details.
 *
 * No memory is freed, it is equivalent to a g_rec_mutex_unlock() call.
 *
 * Since: 2.60
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_60
static inline void
g_rec_mutex_locker_free (GRecMutexLocker *locker)
{
  g_rec_mutex_unlock ((GRecMutex *) locker);
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * G_REC_MUTEX_AUTO_LOCK:
 * @mutex: a [type@GLib.RecMutex]
 * @var: a variable name to be declared
 *
 * Declare a [type@GLib.RecMutexLocker] variable with `g_autoptr()` and lock the
 * mutex. The mutex will be unlocked automatically when leaving the scope. The
 * variable is declared with `G_GNUC_UNUSED` to avoid compiler warning if it is
 * not used in the scope.
 *
 * This feature is only supported on GCC and clang. This macro is not defined on
 * other compilers and should not be used in programs that are intended to be
 * portable to those compilers.
 *
 * Note that this should be used in a place where it is allowed to declare a
 * variable, which could be before any statement in the case
 * `-Wdeclaration-after-statement` is used, or C standard prior to C99.
 *
 * ```c
 * {
 *   G_REC_MUTEX_AUTO_LOCK (&obj->rec_mutex, locker);
 *
 *   obj->stuff_with_lock ();
 *   if (condition)
 *     {
 *       // No need to unlock
 *       return;
 *     }
 *
 *   // Unlock before end of scope
 *   g_clear_pointer (&locker, g_rec_mutex_locker_free);
 *   obj->stuff_without_lock ();
 * }
 * ```
 *
 * Since: 2.80.0
 */
#ifdef g_autoptr
#define G_REC_MUTEX_AUTO_LOCK(mutex, var)                  \
  GLIB_AVAILABLE_MACRO_IN_2_80 g_autoptr (GRecMutexLocker) \
  G_GNUC_UNUSED var = g_rec_mutex_locker_new (mutex)
#endif /* g_autoptr */

/**
 * GRWLockWriterLocker:
 *
 * Opaque type. See g_rw_lock_writer_locker_new() for details.
 * Since: 2.62
 */
typedef void GRWLockWriterLocker;

/**
 * g_rw_lock_writer_locker_new:
 * @rw_lock: a #GRWLock
 *
 * Obtain a write lock on @rw_lock and return a new #GRWLockWriterLocker.
 * Unlock with g_rw_lock_writer_locker_free(). Using g_rw_lock_writer_unlock()
 * on @rw_lock while a #GRWLockWriterLocker exists can lead to undefined
 * behaviour.
 *
 * No allocation is performed, it is equivalent to a g_rw_lock_writer_lock() call.
 *
 * This is intended to be used with g_autoptr().  Note that g_autoptr()
 * is only available when using GCC or clang, so the following example
 * will only work with those compilers:
 * |[
 * typedef struct
 * {
 *   ...
 *   GRWLock rw_lock;
 *   GPtrArray *array;
 *   ...
 * } MyObject;
 *
 * static gchar *
 * my_object_get_data (MyObject *self, guint index)
 * {
 *   g_autoptr(GRWLockReaderLocker) locker = g_rw_lock_reader_locker_new (&self->rw_lock);
 *
 *   // Code with a read lock obtained on rw_lock here
 *
 *   if (self->array == NULL)
 *     // No need to unlock
 *     return NULL;
 *
 *   if (index < self->array->len)
 *     // No need to unlock
 *     return g_ptr_array_index (self->array, index);
 *
 *   // Optionally early unlock
 *   g_clear_pointer (&locker, g_rw_lock_reader_locker_free);
 *
 *   // Code with rw_lock unlocked here
 *   return NULL;
 * }
 *
 * static void
 * my_object_set_data (MyObject *self, guint index, gpointer data)
 * {
 *   g_autoptr(GRWLockWriterLocker) locker = g_rw_lock_writer_locker_new (&self->rw_lock);
 *
 *   // Code with a write lock obtained on rw_lock here
 *
 *   if (self->array == NULL)
 *     self->array = g_ptr_array_new ();
 *
 *   if (condition)
 *     // No need to unlock
 *     return;
 *
 *   if (index >= self->array->len)
 *     g_ptr_array_set_size (self->array, index+1);
 *   g_ptr_array_index (self->array, index) = data;
 *
 *   // Optionally early unlock
 *   g_clear_pointer (&locker, g_rw_lock_writer_locker_free);
 *
 *   // Code with rw_lock unlocked here
 * }
 * ]|
 *
 * Note that it is common for the declared variable to not be used in the scope,
 * which causes some compilers to warn. That can be avoided by using
 * `G_GNUC_UNUSED` or, since 2.80, [func@GLib.RW_LOCK_WRITER_AUTO_LOCK].
 *
 * Returns: a #GRWLockWriterLocker
 * Since: 2.62
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_62
static inline GRWLockWriterLocker *
g_rw_lock_writer_locker_new (GRWLock *rw_lock)
{
  g_rw_lock_writer_lock (rw_lock);
  return (GRWLockWriterLocker *) rw_lock;
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_rw_lock_writer_locker_free:
 * @locker: a GRWLockWriterLocker
 *
 * Release a write lock on @locker's read-write lock. See
 * g_rw_lock_writer_locker_new() for details.
 *
 * No memory is freed, it is equivalent to a g_rw_lock_writer_unlock() call.
 *
 * Since: 2.62
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_62
static inline void
g_rw_lock_writer_locker_free (GRWLockWriterLocker *locker)
{
  g_rw_lock_writer_unlock ((GRWLock *) locker);
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * G_RW_LOCK_WRITER_AUTO_LOCK:
 * @mutex: a [type@GLib.RWLock]
 * @var: a variable name to be declared
 *
 * Declare a [type@GLib.RWLockWriterLocker] variable with `g_autoptr()` and lock
 * for writing. The mutex will be unlocked automatically when leaving the scope.
 * The variable is declared with `G_GNUC_UNUSED` to avoid compiler warning if it
 * is not used in the scope.
 *
 * This feature is only supported on GCC and clang. This macro is not defined on
 * other compilers and should not be used in programs that are intended to be
 * portable to those compilers.
 *
 * Note that this should be used in a place where it is allowed to declare a
 * variable, which could be before any statement in the case
 * `-Wdeclaration-after-statement` is used, or C standard prior to C99.
 *
 * ```c
 * {
 *   G_RW_LOCK_WRITER_AUTO_LOCK (&obj->rw_lock, locker);
 *
 *   obj->stuff_with_lock ();
 *   if (condition)
 *     {
 *       // No need to unlock
 *       return;
 *     }
 *
 *   // Unlock before end of scope
 *   g_clear_pointer (&locker, g_rw_lock_writer_locker_free);
 *   obj->stuff_without_lock ();
 * }
 * ```
 *
 * Since: 2.80.0
 */
#ifdef g_autoptr
#define G_RW_LOCK_WRITER_AUTO_LOCK(mutex, var)                 \
  GLIB_AVAILABLE_MACRO_IN_2_80 g_autoptr (GRWLockWriterLocker) \
  G_GNUC_UNUSED var = g_rw_lock_writer_locker_new (mutex)
#endif /* g_autoptr */

/**
 * GRWLockReaderLocker:
 *
 * Opaque type. See g_rw_lock_reader_locker_new() for details.
 * Since: 2.62
 */
typedef void GRWLockReaderLocker;

/**
 * g_rw_lock_reader_locker_new:
 * @rw_lock: a #GRWLock
 *
 * Obtain a read lock on @rw_lock and return a new #GRWLockReaderLocker.
 * Unlock with g_rw_lock_reader_locker_free(). Using g_rw_lock_reader_unlock()
 * on @rw_lock while a #GRWLockReaderLocker exists can lead to undefined
 * behaviour.
 *
 * No allocation is performed, it is equivalent to a g_rw_lock_reader_lock() call.
 *
 * This is intended to be used with g_autoptr(). For a code sample, see
 * g_rw_lock_writer_locker_new().
 *
 * Returns: a #GRWLockReaderLocker
 * Since: 2.62
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_62
static inline GRWLockReaderLocker *
g_rw_lock_reader_locker_new (GRWLock *rw_lock)
{
  g_rw_lock_reader_lock (rw_lock);
  return (GRWLockReaderLocker *) rw_lock;
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_rw_lock_reader_locker_free:
 * @locker: a GRWLockReaderLocker
 *
 * Release a read lock on @locker's read-write lock. See
 * g_rw_lock_reader_locker_new() for details.
 *
 * No memory is freed, it is equivalent to a g_rw_lock_reader_unlock() call.
 *
 * Since: 2.62
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
GLIB_AVAILABLE_STATIC_INLINE_IN_2_62
static inline void
g_rw_lock_reader_locker_free (GRWLockReaderLocker *locker)
{
  g_rw_lock_reader_unlock ((GRWLock *) locker);
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * G_RW_LOCK_READER_AUTO_LOCK:
 * @mutex: a [type@GLib.RWLock]
 * @var: a variable name to be declared
 *
 * Declare a [type@GLib.RWLockReaderLocker] variable with `g_autoptr()` and lock
 * for reading. The mutex will be unlocked automatically when leaving the scope.
 * The variable is declared with `G_GNUC_UNUSED` to avoid compiler warning if it
 * is not used in the scope.
 *
 * This feature is only supported on GCC and clang. This macro is not defined on
 * other compilers and should not be used in programs that are intended to be
 * portable to those compilers.
 *
 * Note that this should be used in a place where it is allowed to declare a
 * variable, which could be before any statement in the case
 * `-Wdeclaration-after-statement` is used, or C standard prior to C99.
 *
 * ```c
 * {
 *   G_RW_LOCK_READER_AUTO_LOCK (&obj->rw_lock, locker);
 *
 *   obj->stuff_with_lock ();
 *   if (condition)
 *     {
 *       // No need to unlock
 *       return;
 *     }
 *
 *   // Unlock before end of scope
 *   g_clear_pointer (&locker, g_rw_lock_reader_locker_free);
 *   obj->stuff_without_lock ();
 * }
 * ```
 *
 * Since: 2.80.0
 */
#ifdef g_autoptr
#define G_RW_LOCK_READER_AUTO_LOCK(mutex, var)                 \
  GLIB_AVAILABLE_MACRO_IN_2_80 g_autoptr (GRWLockReaderLocker) \
  G_GNUC_UNUSED var = g_rw_lock_reader_locker_new (mutex)
#endif /* g_autoptr */

G_END_DECLS

#endif /* __G_THREAD_H__ */
