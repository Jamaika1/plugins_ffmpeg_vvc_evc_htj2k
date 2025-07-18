/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * gthread.c: MT safety related functions
 * Copyright 1998 Sebastian Wilhelmi; University of Karlsruhe
 *                Owen Taylor
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/* Prelude {{{1 ----------------------------------------------------------- */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

/*
 * MT safe
 */

/* implement gthread.h's inline functions */
#define G_IMPLEMENT_INLINES 1
#define __G_THREAD_C__

#include "config.h"

#include "glib-private.h"
#include "gthread.h"
#include "gthreadprivate.h"

#include <string.h>

#ifdef G_OS_UNIX
#include <unistd.h>

#if defined(THREADS_POSIX) && defined(HAVE_PTHREAD_GETAFFINITY_NP)
#include <pthread.h>
#endif
#endif /* G_OS_UNIX */

#ifndef G_OS_WIN32
#include <sys/time.h>
#include <time.h>
#else
#include <windows.h>
#endif /* G_OS_WIN32 */

#include "gslice.h"
#include "gstrfuncs.h"
#include "gtestutils.h"
#include "glib_trace.h"
#include "gtrace-private.h"

/* In order that the API can be defined in one place (this file), the platform
 * specific code is moved out into separate files so this one doesn’t turn into
 * a massive #ifdef tangle.
 *
 * To avoid the functions in this file becoming tiny trampolines (`jmp` to the
 * relevant `_impl` function only), which would be a performance hit on some
 * hot paths, #include the platform specific implementations. They are marked as
 * `inline` so should be inlined correctly by the compiler without the need for
 * link time optimisation or any fancy tricks.
 */
static inline void g_mutex_init_impl (GMutex *mutex);
static inline void g_mutex_clear_impl (GMutex *mutex);
static inline void g_mutex_lock_impl (GMutex *mutex);
static inline void g_mutex_unlock_impl (GMutex *mutex);
static inline gboolean g_mutex_trylock_impl (GMutex *mutex);

static inline void g_rec_mutex_init_impl (GRecMutex *rec_mutex);
static inline void g_rec_mutex_clear_impl (GRecMutex *rec_mutex);
static inline void g_rec_mutex_lock_impl (GRecMutex *mutex);
static inline void g_rec_mutex_unlock_impl (GRecMutex *rec_mutex);
static inline gboolean g_rec_mutex_trylock_impl (GRecMutex *rec_mutex);

static inline void g_rw_lock_init_impl (GRWLock *rw_lock);
static inline void g_rw_lock_clear_impl (GRWLock *rw_lock);
static inline void g_rw_lock_writer_lock_impl (GRWLock *rw_lock);
static inline gboolean g_rw_lock_writer_trylock_impl (GRWLock *rw_lock);
static inline void g_rw_lock_writer_unlock_impl (GRWLock *rw_lock);
static inline void g_rw_lock_reader_lock_impl (GRWLock *rw_lock);
static inline gboolean g_rw_lock_reader_trylock_impl (GRWLock *rw_lock);
static inline void g_rw_lock_reader_unlock_impl (GRWLock *rw_lock);

static inline void g_cond_init_impl (GCond *cond);
static inline void g_cond_clear_impl (GCond *cond);
static inline void g_cond_wait_impl (GCond  *cond,
                                     GMutex *mutex);
static inline void g_cond_signal_impl (GCond *cond);
static inline void g_cond_broadcast_impl (GCond *cond);
static inline gboolean g_cond_wait_until_impl (GCond  *cond,
                                               GMutex *mutex,
                                               gint64  end_time);

static inline gpointer g_private_get_impl (GPrivate *key);
static inline void g_private_set_impl (GPrivate *key,
                                       gpointer  value);
static inline void g_private_replace_impl (GPrivate *key,
                                           gpointer  value);

static inline void g_thread_yield_impl (void);

//#if defined(THREADS_POSIX)
#include "extra/gthread-posix.c"
//#elif defined(THREADS_WIN32)
//#include "extra/gthread-win32.c"
//#else
//#error "No threads implementation"
//#endif

/* G_LOCK Documentation {{{1 ---------------------------------------------- */

/**
 * G_LOCK_DEFINE:
 * @name: the name of the lock
 *
 * The `G_LOCK_` macros provide a convenient interface to #GMutex.
 * %G_LOCK_DEFINE defines a lock. It can appear in any place where
 * variable definitions may appear in programs, i.e. in the first block
 * of a function or outside of functions. The @name parameter will be
 * mangled to get the name of the #GMutex. This means that you
 * can use names of existing variables as the parameter - e.g. the name
 * of the variable you intend to protect with the lock. Look at our
 * give_me_next_number() example using the `G_LOCK` macros:
 *
 * Here is an example for using the `G_LOCK` convenience macros:
 *
 * |[<!-- language="C" -->
 *   G_LOCK_DEFINE (current_number);
 *
 *   int
 *   give_me_next_number (void)
 *   {
 *     static int current_number = 0;
 *     int ret_val;
 *
 *     G_LOCK (current_number);
 *     ret_val = current_number = calc_next_number (current_number);
 *     G_UNLOCK (current_number);
 *
 *     return ret_val;
 *   }
 * ]|
 */

/**
 * G_LOCK_DEFINE_STATIC:
 * @name: the name of the lock
 *
 * This works like %G_LOCK_DEFINE, but it creates a static object.
 */

/**
 * G_LOCK_EXTERN:
 * @name: the name of the lock
 *
 * This declares a lock, that is defined with %G_LOCK_DEFINE in another
 * module.
 */

/**
 * G_LOCK:
 * @name: the name of the lock
 *
 * Works like g_mutex_lock(), but for a lock defined with
 * %G_LOCK_DEFINE.
 */

/**
 * G_TRYLOCK:
 * @name: the name of the lock
 *
 * Works like g_mutex_trylock(), but for a lock defined with
 * %G_LOCK_DEFINE.
 *
 * Returns: %TRUE, if the lock could be locked.
 */

/**
 * G_UNLOCK:
 * @name: the name of the lock
 *
 * Works like g_mutex_unlock(), but for a lock defined with
 * %G_LOCK_DEFINE.
 */

/**
 * G_AUTO_LOCK:
 * @name: the name of the lock
 *
 * Works like [func@GLib.MUTEX_AUTO_LOCK], but for a lock defined with
 * [func@GLib.LOCK_DEFINE].
 *
 * This feature is only supported on GCC and clang. This macro is not defined on
 * other compilers and should not be used in programs that are intended to be
 * portable to those compilers.
 *
 * Since: 2.80
 */

/* GMutex Documentation {{{1 ------------------------------------------ */

/**
 * GMutex:
 *
 * The #GMutex struct is an opaque data structure to represent a mutex
 * (mutual exclusion). It can be used to protect data against shared
 * access.
 *
 * Take for example the following function:
 * |[<!-- language="C" -->
 *   int
 *   give_me_next_number (void)
 *   {
 *     static int current_number = 0;
 *
 *     // now do a very complicated calculation to calculate the new
 *     // number, this might for example be a random number generator
 *     current_number = calc_next_number (current_number);
 *
 *     return current_number;
 *   }
 * ]|
 * It is easy to see that this won't work in a multi-threaded
 * application. There current_number must be protected against shared
 * access. A #GMutex can be used as a solution to this problem:
 * |[<!-- language="C" -->
 *   int
 *   give_me_next_number (void)
 *   {
 *     static GMutex mutex;
 *     static int current_number = 0;
 *     int ret_val;
 *
 *     g_mutex_lock (&mutex);
 *     ret_val = current_number = calc_next_number (current_number);
 *     g_mutex_unlock (&mutex);
 *
 *     return ret_val;
 *   }
 * ]|
 * Notice that the #GMutex is not initialised to any particular value.
 * Its placement in static storage ensures that it will be initialised
 * to all-zeros, which is appropriate.
 *
 * If a #GMutex is placed in other contexts (eg: embedded in a struct)
 * then it must be explicitly initialised using g_mutex_init().
 *
 * A #GMutex should only be accessed via g_mutex_ functions.
 */

/* GRecMutex Documentation {{{1 -------------------------------------- */

/**
 * GRecMutex:
 *
 * The GRecMutex struct is an opaque data structure to represent a
 * recursive mutex. It is similar to a #GMutex with the difference
 * that it is possible to lock a GRecMutex multiple times in the same
 * thread without deadlock. When doing so, care has to be taken to
 * unlock the recursive mutex as often as it has been locked.
 *
 * If a #GRecMutex is allocated in static storage then it can be used
 * without initialisation.  Otherwise, you should call
 * g_rec_mutex_init() on it and g_rec_mutex_clear() when done.
 *
 * A GRecMutex should only be accessed with the
 * g_rec_mutex_ functions.
 *
 * Since: 2.32
 */

/* GRWLock Documentation {{{1 ---------------------------------------- */

/**
 * GRWLock:
 *
 * The GRWLock struct is an opaque data structure to represent a
 * reader-writer lock. It is similar to a #GMutex in that it allows
 * multiple threads to coordinate access to a shared resource.
 *
 * The difference to a mutex is that a reader-writer lock discriminates
 * between read-only ('reader') and full ('writer') access. While only
 * one thread at a time is allowed write access (by holding the 'writer'
 * lock via g_rw_lock_writer_lock()), multiple threads can gain
 * simultaneous read-only access (by holding the 'reader' lock via
 * g_rw_lock_reader_lock()).
 *
 * It is unspecified whether readers or writers have priority in acquiring the
 * lock when a reader already holds the lock and a writer is queued to acquire
 * it.
 *
 * Here is an example for an array with access functions:
 * |[<!-- language="C" -->
 *   GRWLock lock;
 *   GPtrArray *array;
 *
 *   gpointer
 *   my_array_get (guint index)
 *   {
 *     gpointer retval = NULL;
 *
 *     if (!array)
 *       return NULL;
 *
 *     g_rw_lock_reader_lock (&lock);
 *     if (index < array->len)
 *       retval = g_ptr_array_index (array, index);
 *     g_rw_lock_reader_unlock (&lock);
 *
 *     return retval;
 *   }
 *
 *   void
 *   my_array_set (guint index, gpointer data)
 *   {
 *     g_rw_lock_writer_lock (&lock);
 *
 *     if (!array)
 *       array = g_ptr_array_new ();
 *
 *     if (index >= array->len)
 *       g_ptr_array_set_size (array, index+1);
 *     g_ptr_array_index (array, index) = data;
 *
 *     g_rw_lock_writer_unlock (&lock);
 *   }
 *  ]|
 * This example shows an array which can be accessed by many readers
 * (the my_array_get() function) simultaneously, whereas the writers
 * (the my_array_set() function) will only be allowed one at a time
 * and only if no readers currently access the array. This is because
 * of the potentially dangerous resizing of the array. Using these
 * functions is fully multi-thread safe now.
 *
 * If a #GRWLock is allocated in static storage then it can be used
 * without initialisation.  Otherwise, you should call
 * g_rw_lock_init() on it and g_rw_lock_clear() when done.
 *
 * A GRWLock should only be accessed with the g_rw_lock_ functions.
 *
 * Since: 2.32
 */

/* GCond Documentation {{{1 ------------------------------------------ */

/**
 * GCond:
 *
 * The #GCond struct is an opaque data structure that represents a
 * condition. Threads can block on a #GCond if they find a certain
 * condition to be false. If other threads change the state of this
 * condition they signal the #GCond, and that causes the waiting
 * threads to be woken up.
 *
 * Consider the following example of a shared variable.  One or more
 * threads can wait for data to be published to the variable and when
 * another thread publishes the data, it can signal one of the waiting
 * threads to wake up to collect the data.
 *
 * Here is an example for using GCond to block a thread until a condition
 * is satisfied:
 * |[<!-- language="C" -->
 *   gpointer current_data = NULL;
 *   GMutex data_mutex;
 *   GCond data_cond;
 *
 *   void
 *   push_data (gpointer data)
 *   {
 *     g_mutex_lock (&data_mutex);
 *     current_data = data;
 *     g_cond_signal (&data_cond);
 *     g_mutex_unlock (&data_mutex);
 *   }
 *
 *   gpointer
 *   pop_data (void)
 *   {
 *     gpointer data;
 *
 *     g_mutex_lock (&data_mutex);
 *     while (!current_data)
 *       g_cond_wait (&data_cond, &data_mutex);
 *     data = current_data;
 *     current_data = NULL;
 *     g_mutex_unlock (&data_mutex);
 *
 *     return data;
 *   }
 * ]|
 * Whenever a thread calls pop_data() now, it will wait until
 * current_data is non-%NULL, i.e. until some other thread
 * has called push_data().
 *
 * The example shows that use of a condition variable must always be
 * paired with a mutex.  Without the use of a mutex, there would be a
 * race between the check of @current_data by the while loop in
 * pop_data() and waiting. Specifically, another thread could set
 * @current_data after the check, and signal the cond (with nobody
 * waiting on it) before the first thread goes to sleep. #GCond is
 * specifically useful for its ability to release the mutex and go
 * to sleep atomically.
 *
 * It is also important to use the g_cond_wait() and g_cond_wait_until()
 * functions only inside a loop which checks for the condition to be
 * true.  See g_cond_wait() for an explanation of why the condition may
 * not be true even after it returns.
 *
 * If a #GCond is allocated in static storage then it can be used
 * without initialisation.  Otherwise, you should call g_cond_init()
 * on it and g_cond_clear() when done.
 *
 * A #GCond should only be accessed via the g_cond_ functions.
 */

/* GThread Documentation {{{1 ---------------------------------------- */

/**
 * GThread:
 *
 * The #GThread struct represents a running thread. This struct
 * is returned by g_thread_new() or g_thread_try_new(). You can
 * obtain the #GThread struct representing the current thread by
 * calling g_thread_self().
 *
 * GThread is refcounted, see g_thread_ref() and g_thread_unref().
 * The thread represented by it holds a reference while it is running,
 * and g_thread_join() consumes the reference that it is given, so
 * it is normally not necessary to manage GThread references
 * explicitly.
 *
 * The structure is opaque -- none of its fields may be directly
 * accessed.
 */

/**
 * GThreadFunc:
 * @data: data passed to the thread
 *
 * Specifies the type of the @func functions passed to g_thread_new()
 * or g_thread_try_new().
 *
 * Returns: the return value of the thread
 */

/**
 * g_thread_supported:
 *
 * This macro returns %TRUE if the thread system is initialized,
 * and %FALSE if it is not.
 *
 * For language bindings, g_thread_get_initialized() provides
 * the same functionality as a function.
 *
 * Returns: %TRUE, if the thread system is initialized
 */

/* GThreadError {{{1 ------------------------------------------------------- */
/**
 * GThreadError:
 * @G_THREAD_ERROR_AGAIN: a thread couldn't be created due to resource
 *                        shortage. Try again later.
 *
 * Possible errors of thread related functions.
 **/

/**
 * G_THREAD_ERROR:
 *
 * The error domain of the GLib thread subsystem.
 **/
G_DEFINE_QUARK (g_thread_error, g_thread_error)

/* Local Data {{{1 -------------------------------------------------------- */

static GMutex    g_once_mutex;
static GCond     g_once_cond;
static GSList   *g_once_init_list = NULL;

static guint g_thread_n_created_counter = 0;  /* (atomic) */

static void g_thread_cleanup (gpointer data);
static GPrivate     g_thread_specific_private = G_PRIVATE_INIT (g_thread_cleanup);

/*
 * g_private_set_alloc0:
 * @key: a #GPrivate
 * @size: size of the allocation, in bytes
 *
 * Sets the thread local variable @key to have a newly-allocated and zero-filled
 * value of given @size, and returns a pointer to that memory. Allocations made
 * using this API will be suppressed in valgrind (when the GLib default
 * suppression file, `glib.supp`, is used) and leak sanitizer: it is intended to
 * be used for one-time allocations which are known to be leaked, such as those
 * for per-thread initialisation data. Otherwise, this function behaves the same
 * as g_private_set().
 *
 * Returns: (transfer full): new thread-local heap allocation of size @size
 * Since: 2.60
 */
/*< private >*/
gpointer
g_private_set_alloc0 (GPrivate *key,
                      gsize     size)
{
  gpointer allocated = g_malloc0 (size);

  g_ignore_leak (allocated);
  g_private_set (key, allocated);

  return g_steal_pointer (&allocated);
}

/* GOnce {{{1 ------------------------------------------------------------- */

/**
 * GOnce:
 * @status: the status of the #GOnce
 * @retval: the value returned by the call to the function, if @status
 *          is %G_ONCE_STATUS_READY
 *
 * A #GOnce struct controls a one-time initialization function. Any
 * one-time initialization function must have its own unique #GOnce
 * struct.
 *
 * Since: 2.4
 */

/**
 * G_ONCE_INIT:
 *
 * A #GOnce must be initialized with this macro before it can be used.
 *
 * |[<!-- language="C" -->
 *   GOnce my_once = G_ONCE_INIT;
 * ]|
 *
 * Since: 2.4
 */

/**
 * GOnceStatus:
 * @G_ONCE_STATUS_NOTCALLED: the function has not been called yet.
 * @G_ONCE_STATUS_PROGRESS: the function call is currently in progress.
 * @G_ONCE_STATUS_READY: the function has been called.
 *
 * The possible statuses of a one-time initialization function
 * controlled by a #GOnce struct.
 *
 * Since: 2.4
 */

/**
 * g_once:
 * @once: a #GOnce structure
 * @func: the #GThreadFunc function associated to @once. This function
 *        is called only once, regardless of the number of times it and
 *        its associated #GOnce struct are passed to g_once().
 * @arg: data to be passed to @func
 *
 * The first call to this routine by a process with a given #GOnce
 * struct calls @func with the given argument. Thereafter, subsequent
 * calls to g_once()  with the same #GOnce struct do not call @func
 * again, but return the stored result of the first call. On return
 * from g_once(), the status of @once will be %G_ONCE_STATUS_READY.
 *
 * For example, a mutex or a thread-specific data key must be created
 * exactly once. In a threaded environment, calling g_once() ensures
 * that the initialization is serialized across multiple threads.
 *
 * Calling g_once() recursively on the same #GOnce struct in
 * @func will lead to a deadlock.
 *
 * |[<!-- language="C" -->
 *   gpointer
 *   get_debug_flags (void)
 *   {
 *     static GOnce my_once = G_ONCE_INIT;
 *
 *     g_once (&my_once, parse_debug_flags, NULL);
 *
 *     return my_once.retval;
 *   }
 * ]|
 *
 * Since: 2.4
 */
gpointer
g_once_impl (GOnce       *once,
	     GThreadFunc  func,
	     gpointer     arg)
{
  g_mutex_lock (&g_once_mutex);

  while (once->status == G_ONCE_STATUS_PROGRESS)
    g_cond_wait (&g_once_cond, &g_once_mutex);

  if (once->status != G_ONCE_STATUS_READY)
    {
      gpointer retval;

      once->status = G_ONCE_STATUS_PROGRESS;
      g_mutex_unlock (&g_once_mutex);

      retval = func (arg);

      g_mutex_lock (&g_once_mutex);
/* We prefer the new C11-style atomic extension of GCC if available. If not,
 * fall back to always locking. */
#if defined(G_ATOMIC_LOCK_FREE) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && defined(__ATOMIC_SEQ_CST)
      /* Only the second store needs to be atomic, as the two writes are related
       * by a happens-before relationship here. */
      once->retval = retval;
      __atomic_store_n (&once->status, G_ONCE_STATUS_READY, __ATOMIC_RELEASE);
#else
      once->retval = retval;
      once->status = G_ONCE_STATUS_READY;
#endif
      g_cond_broadcast (&g_once_cond);
    }

  g_mutex_unlock (&g_once_mutex);

  return once->retval;
}

/**
 * g_once_init_enter:
 * @location: (inout) (not optional): location of a static initializable variable
 *    containing 0
 *
 * Function to be called when starting a critical initialization
 * section. The argument @location must point to a static
 * 0-initialized variable that will be set to a value other than 0 at
 * the end of the initialization section. In combination with
 * g_once_init_leave() and the unique address @value_location, it can
 * be ensured that an initialization section will be executed only once
 * during a program's life time, and that concurrent threads are
 * blocked until initialization completed. To be used in constructs
 * like this:
 *
 * |[<!-- language="C" -->
 *   static gsize initialization_value = 0;
 *
 *   if (g_once_init_enter (&initialization_value))
 *     {
 *       gsize setup_value = 42; // initialization code here
 *
 *       g_once_init_leave (&initialization_value, setup_value);
 *     }
 *
 *   // use initialization_value here
 * ]|
 *
 * While @location has a `volatile` qualifier, this is a historical artifact and
 * the pointer passed to it should not be `volatile`.
 *
 * Returns: %TRUE if the initialization section should be entered,
 *     %FALSE and blocks otherwise
 *
 * Since: 2.14
 */
gboolean
(g_once_init_enter) (volatile void *location)
{
  gsize *value_location = (gsize *) location;
  gboolean need_init = FALSE;
  g_mutex_lock (&g_once_mutex);
  if (g_atomic_pointer_get (value_location) == 0)
    {
      if (!g_slist_find (g_once_init_list, (void*) value_location))
        {
          need_init = TRUE;
          g_once_init_list = g_slist_prepend (g_once_init_list, (void*) value_location);
        }
      else
        do
          g_cond_wait (&g_once_cond, &g_once_mutex);
        while (g_slist_find (g_once_init_list, (void*) value_location));
    }
  g_mutex_unlock (&g_once_mutex);
  return need_init;
}

/**
 * g_once_init_enter_pointer:
 * @location: (not nullable): location of a static initializable variable
 *    containing `NULL`
 *
 * This functions behaves in the same way as g_once_init_enter(), but can
 * can be used to initialize pointers (or #guintptr) instead of #gsize.
 *
 * |[<!-- language="C" -->
 *   static MyStruct *interesting_struct = NULL;
 *
 *   if (g_once_init_enter_pointer (&interesting_struct))
 *     {
 *       MyStruct *setup_value = allocate_my_struct (); // initialization code here
 *
 *       g_once_init_leave_pointer (&interesting_struct, g_steal_pointer (&setup_value));
 *     }
 *
 *   // use interesting_struct here
 * ]|
 *
 * Returns: %TRUE if the initialization section should be entered,
 *     %FALSE and blocks otherwise
 *
 * Since: 2.80
 */
gboolean
(g_once_init_enter_pointer) (gpointer location)
{
  gpointer *value_location = (gpointer *) location;
  gboolean need_init = FALSE;
  g_mutex_lock (&g_once_mutex);
  if (g_atomic_pointer_get (value_location) == 0)
    {
      if (!g_slist_find (g_once_init_list, (void *) value_location))
        {
          need_init = TRUE;
          g_once_init_list = g_slist_prepend (g_once_init_list, (void *) value_location);
        }
      else
        do
          g_cond_wait (&g_once_cond, &g_once_mutex);
        while (g_slist_find (g_once_init_list, (void *) value_location));
    }
  g_mutex_unlock (&g_once_mutex);
  return need_init;
}

/**
 * g_once_init_leave:
 * @location: (inout) (not optional): location of a static initializable variable
 *    containing 0
 * @result: new non-0 value for `*value_location`
 *
 * Counterpart to g_once_init_enter(). Expects a location of a static
 * 0-initialized initialization variable, and an initialization value
 * other than 0. Sets the variable to the initialization value, and
 * releases concurrent threads blocking in g_once_init_enter() on this
 * initialization variable.
 *
 * While @location has a `volatile` qualifier, this is a historical artifact and
 * the pointer passed to it should not be `volatile`.
 *
 * Since: 2.14
 */
void
(g_once_init_leave) (volatile void *location,
                     gsize          result)
{
  gsize *value_location = (gsize *) location;
  gsize old_value;

  g_return_if_fail (result != 0);

  old_value = (gsize) g_atomic_pointer_exchange (value_location, result);
  g_return_if_fail (old_value == 0);

  g_mutex_lock (&g_once_mutex);
  g_return_if_fail (g_once_init_list != NULL);
  g_once_init_list = g_slist_remove (g_once_init_list, (void*) value_location);
  g_cond_broadcast (&g_once_cond);
  g_mutex_unlock (&g_once_mutex);
}

/**
 * g_once_init_leave_pointer:
 * @location: (not nullable): location of a static initializable variable
 *    containing `NULL`
 * @result: new non-`NULL` value for `*location`
 *
 * Counterpart to g_once_init_enter_pointer(). Expects a location of a static
 * `NULL`-initialized initialization variable, and an initialization value
 * other than `NULL`. Sets the variable to the initialization value, and
 * releases concurrent threads blocking in g_once_init_enter_pointer() on this
 * initialization variable.
 *
 * This functions behaves in the same way as g_once_init_leave(), but
 * can be used to initialize pointers (or #guintptr) instead of #gsize.
 *
 * Since: 2.80
 */
void
(g_once_init_leave_pointer) (gpointer location,
                             gpointer result)
{
  gpointer *value_location = (gpointer *) location;
  gpointer old_value;

  g_return_if_fail (result != 0);

  old_value = g_atomic_pointer_exchange (value_location, result);
  g_return_if_fail (old_value == 0);

  g_mutex_lock (&g_once_mutex);
  g_return_if_fail (g_once_init_list != NULL);
  g_once_init_list = g_slist_remove (g_once_init_list, (void *) value_location);
  g_cond_broadcast (&g_once_cond);
  g_mutex_unlock (&g_once_mutex);
}

/* GThread {{{1 -------------------------------------------------------- */

/**
 * g_thread_ref:
 * @thread: a #GThread
 *
 * Increase the reference count on @thread.
 *
 * Returns: (transfer full): a new reference to @thread
 *
 * Since: 2.32
 */
GThread *
g_thread_ref (GThread *thread)
{
  GRealThread *real = (GRealThread *) thread;

  g_atomic_int_inc (&real->ref_count);

  return thread;
}

/**
 * g_thread_unref:
 * @thread: (transfer full): a #GThread
 *
 * Decrease the reference count on @thread, possibly freeing all
 * resources associated with it.
 *
 * Note that each thread holds a reference to its #GThread while
 * it is running, so it is safe to drop your own reference to it
 * if you don't need it anymore.
 *
 * Since: 2.32
 */
void
g_thread_unref (GThread *thread)
{
  GRealThread *real = (GRealThread *) thread;

  if (g_atomic_int_dec_and_test (&real->ref_count))
    {
      if (real->ours)
        g_system_thread_free (real);
      else
        g_slice_free (GRealThread, real);
    }
}

static void
g_thread_cleanup (gpointer data)
{
  g_thread_unref (data);
}

gpointer
g_thread_proxy (gpointer data)
{
  GRealThread* thread = data;

  g_assert (data);
  g_private_set (&g_thread_specific_private, data);

  TRACE (GLIB_THREAD_SPAWNED (thread->thread.func, thread->thread.data,
                              thread->name));

  if (thread->name[0] != '\0')
    g_system_thread_set_name (thread->name);
  else
    g_system_thread_get_name (thread->name, sizeof (thread->name));

  thread->retval = thread->thread.func (thread->thread.data);

  return NULL;
}

guint
g_thread_n_created (void)
{
  return g_atomic_int_get (&g_thread_n_created_counter);
}

/**
 * g_thread_new:
 * @name: (nullable): an (optional) name for the new thread
 * @func: (closure data) (scope async): a function to execute in the new thread
 * @data: (nullable): an argument to supply to the new thread
 *
 * This function creates a new thread. The new thread starts by invoking
 * @func with the argument data. The thread will run until @func returns
 * or until g_thread_exit() is called from the new thread. The return value
 * of @func becomes the return value of the thread, which can be obtained
 * with g_thread_join().
 *
 * The @name can be useful for discriminating threads in a debugger.
 * It is not used for other purposes and does not have to be unique.
 * Some systems restrict the length of @name to 16 bytes.
 *
 * If the thread can not be created the program aborts. See
 * g_thread_try_new() if you want to attempt to deal with failures.
 *
 * If you are using threads to offload (potentially many) short-lived tasks,
 * #GThreadPool may be more appropriate than manually spawning and tracking
 * multiple #GThreads.
 *
 * To free the struct returned by this function, use g_thread_unref().
 * Note that g_thread_join() implicitly unrefs the #GThread as well.
 *
 * New threads by default inherit their scheduler policy (POSIX) or thread
 * priority (Windows) of the thread creating the new thread.
 *
 * This behaviour changed in GLib 2.64: before threads on Windows were not
 * inheriting the thread priority but were spawned with the default priority.
 * Starting with GLib 2.64 the behaviour is now consistent between Windows and
 * POSIX and all threads inherit their parent thread's priority.
 *
 * Returns: (transfer full): the new #GThread
 *
 * Since: 2.32
 */
GThread *
g_thread_new (const gchar *name,
              GThreadFunc  func,
              gpointer     data)
{
  GError *error = NULL;
  GThread *thread;

  thread = g_thread_new_internal (name, g_thread_proxy, func, data, 0, &error);

  if G_UNLIKELY (thread == NULL)
    g_error ("creating thread '%s': %s", name ? name : "", error->message);

  return thread;
}

/**
 * g_thread_try_new:
 * @name: (nullable): an (optional) name for the new thread
 * @func: (closure data) (scope async): a function to execute in the new thread
 * @data: (nullable): an argument to supply to the new thread
 * @error: return location for error, or %NULL
 *
 * This function is the same as g_thread_new() except that
 * it allows for the possibility of failure.
 *
 * If a thread can not be created (due to resource limits),
 * @error is set and %NULL is returned.
 *
 * Returns: (transfer full): the new #GThread, or %NULL if an error occurred
 *
 * Since: 2.32
 */
GThread *
g_thread_try_new (const gchar  *name,
                  GThreadFunc   func,
                  gpointer      data,
                  GError      **error)
{
  return g_thread_new_internal (name, g_thread_proxy, func, data, 0, error);
}

GThread *
g_thread_new_internal (const gchar *name,
                       GThreadFunc proxy,
                       GThreadFunc func,
                       gpointer data,
                       gsize stack_size,
                       GError **error)
{
  g_return_val_if_fail (func != NULL, NULL);

  g_atomic_int_inc (&g_thread_n_created_counter);

  g_trace_mark (G_TRACE_CURRENT_TIME, 0, "GLib", "GThread created", "%s", name ? name : "(unnamed)");
  return (GThread *) g_system_thread_new (proxy, stack_size, name, func, data, error);
}

/**
 * g_thread_exit:
 * @retval: the return value of this thread
 *
 * Terminates the current thread.
 *
 * If another thread is waiting for us using g_thread_join() then the
 * waiting thread will be woken up and get @retval as the return value
 * of g_thread_join().
 *
 * Calling g_thread_exit() with a parameter @retval is equivalent to
 * returning @retval from the function @func, as given to g_thread_new().
 *
 * You must only call g_thread_exit() from a thread that you created
 * yourself with g_thread_new() or related APIs. You must not call
 * this function from a thread created with another threading library
 * or or from within a #GThreadPool.
 */
void
g_thread_exit (gpointer retval)
{
  GRealThread* real = (GRealThread*) g_thread_self ();

  if G_UNLIKELY (!real->ours)
    g_error ("attempt to g_thread_exit() a thread not created by GLib");

  real->retval = retval;

  g_system_thread_exit ();
}

/**
 * g_thread_join:
 * @thread: (transfer full): a #GThread
 *
 * Waits until @thread finishes, i.e. the function @func, as
 * given to g_thread_new(), returns or g_thread_exit() is called.
 * If @thread has already terminated, then g_thread_join()
 * returns immediately.
 *
 * Any thread can wait for any other thread by calling g_thread_join(),
 * not just its 'creator'. Calling g_thread_join() from multiple threads
 * for the same @thread leads to undefined behaviour.
 *
 * The value returned by @func or given to g_thread_exit() is
 * returned by this function.
 *
 * g_thread_join() consumes the reference to the passed-in @thread.
 * This will usually cause the #GThread struct and associated resources
 * to be freed. Use g_thread_ref() to obtain an extra reference if you
 * want to keep the GThread alive beyond the g_thread_join() call.
 *
 * Returns: (transfer full): the return value of the thread
 */
gpointer
g_thread_join (GThread *thread)
{
  GRealThread *real = (GRealThread*) thread;
  gpointer retval;

  g_return_val_if_fail (thread, NULL);
  g_return_val_if_fail (real->ours, NULL);

  g_system_thread_wait (real);

  retval = real->retval;

  /* Just to make sure, this isn't used any more */
  thread->joinable = 0;

  g_thread_unref (thread);

  return retval;
}

/**
 * g_thread_self:
 *
 * This function returns the #GThread corresponding to the
 * current thread. Note that this function does not increase
 * the reference count of the returned struct.
 *
 * This function will return a #GThread even for threads that
 * were not created by GLib (i.e. those created by other threading
 * APIs). This may be useful for thread identification purposes
 * (i.e. comparisons) but you must not use GLib functions (such
 * as g_thread_join()) on these threads.
 *
 * Returns: (transfer none): the #GThread representing the current thread
 */
GThread*
g_thread_self (void)
{
  GRealThread* thread = g_private_get (&g_thread_specific_private);

  if (!thread)
    {
      /* If no thread data is available, provide and set one.
       * This can happen for the main thread and for threads
       * that are not created by GLib.
       */
      thread = g_slice_new0 (GRealThread);
      thread->ref_count = 1;

      g_private_set (&g_thread_specific_private, thread);
    }

  return (GThread*) thread;
}

/**
 * g_thread_get_name:
 * @thread: a thread
 *
 * Gets the name of the thread.
 *
 * This function is intended for debugging purposes.
 *
 * Returns: the name of the thread
 *
 * Since: 2.84
 */
const char *
g_thread_get_name (GThread *thread)
{
  GRealThread *real = (GRealThread*) thread;

  return real->name;
}

/**
 * g_get_num_processors:
 *
 * Determine the approximate number of threads that the system will
 * schedule simultaneously for this process.  This is intended to be
 * used as a parameter to g_thread_pool_new() for CPU bound tasks and
 * similar cases.
 *
 * Returns: Number of schedulable threads, always greater than 0
 *
 * Since: 2.36
 */
guint
g_get_num_processors (void)
{
#ifdef G_OS_WIN32
  unsigned int count;
  SYSTEM_INFO sysinfo;
  DWORD_PTR process_cpus;
  DWORD_PTR system_cpus;

  /* This *never* fails, use it as fallback */
  GetNativeSystemInfo (&sysinfo);
  count = (int) sysinfo.dwNumberOfProcessors;

  if (GetProcessAffinityMask (GetCurrentProcess (),
                              &process_cpus, &system_cpus))
    {
      unsigned int af_count;

      for (af_count = 0; process_cpus != 0; process_cpus >>= 1)
        if (process_cpus & 1)
          af_count++;

      /* Prefer affinity-based result, if available */
      if (af_count > 0)
        count = af_count;
    }

  if (count > 0)
    return count;
#elif defined(_SC_NPROCESSORS_ONLN) && defined(THREADS_POSIX) && defined(HAVE_PTHREAD_GETAFFINITY_NP)
  {
    int ncores = MIN (sysconf (_SC_NPROCESSORS_ONLN), CPU_SETSIZE);
    cpu_set_t cpu_mask;
    CPU_ZERO (&cpu_mask);

    int af_count = 0;
    int err = pthread_getaffinity_np (pthread_self (), sizeof (cpu_mask), &cpu_mask);
    if (!err)
      af_count = CPU_COUNT (&cpu_mask);

    int count = (af_count > 0) ? af_count : ncores;
    return count;
  }
#elif defined(_SC_NPROCESSORS_ONLN)
  {
    int count;

    count = sysconf (_SC_NPROCESSORS_ONLN);
    if (count > 0)
      return count;
  }
#elif defined HW_NCPU
  {
    int mib[2], count = 0;
    size_t len;

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(count);

    if (sysctl (mib, 2, &count, &len, NULL, 0) == 0 && count > 0)
      return count;
  }
#endif

  return 1; /* Fallback */
}

/**
 * g_mutex_init:
 * @mutex: an uninitialized #GMutex
 *
 * Initializes a #GMutex so that it can be used.
 *
 * This function is useful to initialize a mutex that has been
 * allocated on the stack, or as part of a larger structure.
 * It is not necessary to initialize a mutex that has been
 * statically allocated.
 *
 * |[<!-- language="C" -->
 *   typedef struct {
 *     GMutex m;
 *     ...
 *   } Blob;
 *
 * Blob *b;
 *
 * b = g_new (Blob, 1);
 * g_mutex_init (&b->m);
 * ]|
 *
 * To undo the effect of g_mutex_init() when a mutex is no longer
 * needed, use g_mutex_clear().
 *
 * Calling g_mutex_init() on an already initialized #GMutex leads
 * to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_mutex_init (GMutex *mutex)
{
  g_mutex_init_impl (mutex);
}

/**
 * g_mutex_clear:
 * @mutex: an initialized #GMutex
 *
 * Frees the resources allocated to a mutex with g_mutex_init().
 *
 * This function should not be used with a #GMutex that has been
 * statically allocated.
 *
 * Calling g_mutex_clear() on a locked mutex leads to undefined
 * behaviour.
 *
 * Since: 2.32
 */
void
g_mutex_clear (GMutex *mutex)
{
  g_mutex_clear_impl (mutex);
}

/**
 * g_mutex_lock:
 * @mutex: a #GMutex
 *
 * Locks @mutex. If @mutex is already locked by another thread, the
 * current thread will block until @mutex is unlocked by the other
 * thread.
 *
 * #GMutex is neither guaranteed to be recursive nor to be
 * non-recursive.  As such, calling g_mutex_lock() on a #GMutex that has
 * already been locked by the same thread results in undefined behaviour
 * (including but not limited to deadlocks).
 */
void
g_mutex_lock (GMutex *mutex)
{
  g_mutex_lock_impl (mutex);
}

/**
 * g_mutex_unlock:
 * @mutex: a #GMutex
 *
 * Unlocks @mutex. If another thread is blocked in a g_mutex_lock()
 * call for @mutex, it will become unblocked and can lock @mutex itself.
 *
 * Calling g_mutex_unlock() on a mutex that is not locked by the
 * current thread leads to undefined behaviour.
 */
void
g_mutex_unlock (GMutex *mutex)
{
  g_mutex_unlock_impl (mutex);
}

/**
 * g_mutex_trylock:
 * @mutex: a #GMutex
 *
 * Tries to lock @mutex. If @mutex is already locked by another thread,
 * it immediately returns %FALSE. Otherwise it locks @mutex and returns
 * %TRUE.
 *
 * #GMutex is neither guaranteed to be recursive nor to be
 * non-recursive.  As such, calling g_mutex_lock() on a #GMutex that has
 * already been locked by the same thread results in undefined behaviour
 * (including but not limited to deadlocks or arbitrary return values).
 *
 * Returns: %TRUE if @mutex could be locked
 */
gboolean
g_mutex_trylock (GMutex *mutex)
{
  return g_mutex_trylock_impl (mutex);
}

/**
 * g_rec_mutex_init:
 * @rec_mutex: an uninitialized #GRecMutex
 *
 * Initializes a #GRecMutex so that it can be used.
 *
 * This function is useful to initialize a recursive mutex
 * that has been allocated on the stack, or as part of a larger
 * structure.
 *
 * It is not necessary to initialise a recursive mutex that has been
 * statically allocated.
 *
 * |[<!-- language="C" -->
 *   typedef struct {
 *     GRecMutex m;
 *     ...
 *   } Blob;
 *
 * Blob *b;
 *
 * b = g_new (Blob, 1);
 * g_rec_mutex_init (&b->m);
 * ]|
 *
 * Calling g_rec_mutex_init() on an already initialized #GRecMutex
 * leads to undefined behaviour.
 *
 * To undo the effect of g_rec_mutex_init() when a recursive mutex
 * is no longer needed, use g_rec_mutex_clear().
 *
 * Since: 2.32
 */
void
g_rec_mutex_init (GRecMutex *rec_mutex)
{
  g_rec_mutex_init_impl (rec_mutex);
}

/**
 * g_rec_mutex_clear:
 * @rec_mutex: an initialized #GRecMutex
 *
 * Frees the resources allocated to a recursive mutex with
 * g_rec_mutex_init().
 *
 * This function should not be used with a #GRecMutex that has been
 * statically allocated.
 *
 * Calling g_rec_mutex_clear() on a locked recursive mutex leads
 * to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rec_mutex_clear (GRecMutex *rec_mutex)
{
  g_rec_mutex_clear_impl (rec_mutex);
}

/**
 * g_rec_mutex_lock:
 * @rec_mutex: a #GRecMutex
 *
 * Locks @rec_mutex. If @rec_mutex is already locked by another
 * thread, the current thread will block until @rec_mutex is
 * unlocked by the other thread. If @rec_mutex is already locked
 * by the current thread, the 'lock count' of @rec_mutex is increased.
 * The mutex will only become available again when it is unlocked
 * as many times as it has been locked.
 *
 * Since: 2.32
 */
void
g_rec_mutex_lock (GRecMutex *mutex)
{
  g_rec_mutex_lock_impl (mutex);
}

/**
 * g_rec_mutex_unlock:
 * @rec_mutex: a #GRecMutex
 *
 * Unlocks @rec_mutex. If another thread is blocked in a
 * g_rec_mutex_lock() call for @rec_mutex, it will become unblocked
 * and can lock @rec_mutex itself.
 *
 * Calling g_rec_mutex_unlock() on a recursive mutex that is not
 * locked by the current thread leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rec_mutex_unlock (GRecMutex *rec_mutex)
{
  g_rec_mutex_unlock_impl (rec_mutex);
}

/**
 * g_rec_mutex_trylock:
 * @rec_mutex: a #GRecMutex
 *
 * Tries to lock @rec_mutex. If @rec_mutex is already locked
 * by another thread, it immediately returns %FALSE. Otherwise
 * it locks @rec_mutex and returns %TRUE.
 *
 * Returns: %TRUE if @rec_mutex could be locked
 *
 * Since: 2.32
 */
gboolean
g_rec_mutex_trylock (GRecMutex *rec_mutex)
{
  return g_rec_mutex_trylock_impl (rec_mutex);
}

/* {{{1 GRWLock */

/**
 * g_rw_lock_init:
 * @rw_lock: an uninitialized #GRWLock
 *
 * Initializes a #GRWLock so that it can be used.
 *
 * This function is useful to initialize a lock that has been
 * allocated on the stack, or as part of a larger structure.  It is not
 * necessary to initialise a reader-writer lock that has been statically
 * allocated.
 *
 * |[<!-- language="C" -->
 *   typedef struct {
 *     GRWLock l;
 *     ...
 *   } Blob;
 *
 * Blob *b;
 *
 * b = g_new (Blob, 1);
 * g_rw_lock_init (&b->l);
 * ]|
 *
 * To undo the effect of g_rw_lock_init() when a lock is no longer
 * needed, use g_rw_lock_clear().
 *
 * Calling g_rw_lock_init() on an already initialized #GRWLock leads
 * to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rw_lock_init (GRWLock *rw_lock)
{
  g_rw_lock_init_impl (rw_lock);
}

/**
 * g_rw_lock_clear:
 * @rw_lock: an initialized #GRWLock
 *
 * Frees the resources allocated to a lock with g_rw_lock_init().
 *
 * This function should not be used with a #GRWLock that has been
 * statically allocated.
 *
 * Calling g_rw_lock_clear() when any thread holds the lock
 * leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rw_lock_clear (GRWLock *rw_lock)
{
  g_rw_lock_clear_impl (rw_lock);
}

/**
 * g_rw_lock_writer_lock:
 * @rw_lock: a #GRWLock
 *
 * Obtain a write lock on @rw_lock. If another thread currently holds
 * a read or write lock on @rw_lock, the current thread will block
 * until all other threads have dropped their locks on @rw_lock.
 *
 * Calling g_rw_lock_writer_lock() while the current thread already
 * owns a read or write lock on @rw_lock leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rw_lock_writer_lock (GRWLock *rw_lock)
{
  g_rw_lock_writer_lock_impl (rw_lock);
}

/**
 * g_rw_lock_writer_trylock:
 * @rw_lock: a #GRWLock
 *
 * Tries to obtain a write lock on @rw_lock. If another thread
 * currently holds a read or write lock on @rw_lock, it immediately
 * returns %FALSE.
 * Otherwise it locks @rw_lock and returns %TRUE.
 *
 * Returns: %TRUE if @rw_lock could be locked
 *
 * Since: 2.32
 */
gboolean
g_rw_lock_writer_trylock (GRWLock *rw_lock)
{
  return g_rw_lock_writer_trylock_impl (rw_lock);
}

/**
 * g_rw_lock_writer_unlock:
 * @rw_lock: a #GRWLock
 *
 * Release a write lock on @rw_lock.
 *
 * Calling g_rw_lock_writer_unlock() on a lock that is not held
 * by the current thread leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rw_lock_writer_unlock (GRWLock *rw_lock)
{
  g_rw_lock_writer_unlock_impl (rw_lock);
}

/**
 * g_rw_lock_reader_lock:
 * @rw_lock: a #GRWLock
 *
 * Obtain a read lock on @rw_lock. If another thread currently holds
 * the write lock on @rw_lock, the current thread will block until the
 * write lock was (held and) released. If another thread does not hold
 * the write lock, but is waiting for it, it is implementation defined
 * whether the reader or writer will block. Read locks can be taken
 * recursively.
 *
 * Calling g_rw_lock_reader_lock() while the current thread already
 * owns a write lock leads to undefined behaviour. Read locks however
 * can be taken recursively, in which case you need to make sure to
 * call g_rw_lock_reader_unlock() the same amount of times.
 *
 * It is implementation-defined how many read locks are allowed to be
 * held on the same lock simultaneously. If the limit is hit,
 * or if a deadlock is detected, a critical warning will be emitted.
 *
 * Since: 2.32
 */
void
g_rw_lock_reader_lock (GRWLock *rw_lock)
{
  g_rw_lock_reader_lock_impl (rw_lock);
}

/**
 * g_rw_lock_reader_trylock:
 * @rw_lock: a #GRWLock
 *
 * Tries to obtain a read lock on @rw_lock and returns %TRUE if
 * the read lock was successfully obtained. Otherwise it
 * returns %FALSE.
 *
 * Returns: %TRUE if @rw_lock could be locked
 *
 * Since: 2.32
 */
gboolean
g_rw_lock_reader_trylock (GRWLock *rw_lock)
{
  return g_rw_lock_reader_trylock_impl (rw_lock);
}

/**
 * g_rw_lock_reader_unlock:
 * @rw_lock: a #GRWLock
 *
 * Release a read lock on @rw_lock.
 *
 * Calling g_rw_lock_reader_unlock() on a lock that is not held
 * by the current thread leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_rw_lock_reader_unlock (GRWLock *rw_lock)
{
  g_rw_lock_reader_unlock_impl (rw_lock);
}

/* {{{1 GCond */

/**
 * g_cond_init:
 * @cond: an uninitialized #GCond
 *
 * Initialises a #GCond so that it can be used.
 *
 * This function is useful to initialise a #GCond that has been
 * allocated as part of a larger structure.  It is not necessary to
 * initialise a #GCond that has been statically allocated.
 *
 * To undo the effect of g_cond_init() when a #GCond is no longer
 * needed, use g_cond_clear().
 *
 * Calling g_cond_init() on an already-initialised #GCond leads
 * to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_cond_init (GCond *cond)
{
  g_cond_init_impl (cond);
}

/**
 * g_cond_clear:
 * @cond: an initialised #GCond
 *
 * Frees the resources allocated to a #GCond with g_cond_init().
 *
 * This function should not be used with a #GCond that has been
 * statically allocated.
 *
 * Calling g_cond_clear() for a #GCond on which threads are
 * blocking leads to undefined behaviour.
 *
 * Since: 2.32
 */
void
g_cond_clear (GCond *cond)
{
  g_cond_clear_impl (cond);
}

/**
 * g_cond_wait:
 * @cond: a #GCond
 * @mutex: a #GMutex that is currently locked
 *
 * Atomically releases @mutex and waits until @cond is signalled.
 * When this function returns, @mutex is locked again and owned by the
 * calling thread.
 *
 * When using condition variables, it is possible that a spurious wakeup
 * may occur (ie: g_cond_wait() returns even though g_cond_signal() was
 * not called).  It's also possible that a stolen wakeup may occur.
 * This is when g_cond_signal() is called, but another thread acquires
 * @mutex before this thread and modifies the state of the program in
 * such a way that when g_cond_wait() is able to return, the expected
 * condition is no longer met.
 *
 * For this reason, g_cond_wait() must always be used in a loop.  See
 * the documentation for #GCond for a complete example.
 **/
void
g_cond_wait (GCond  *cond,
             GMutex *mutex)
{
  g_cond_wait_impl (cond, mutex);
}

/**
 * g_cond_signal:
 * @cond: a #GCond
 *
 * If threads are waiting for @cond, at least one of them is unblocked.
 * If no threads are waiting for @cond, this function has no effect.
 * It is good practice to hold the same lock as the waiting thread
 * while calling this function, though not required.
 */
void
g_cond_signal (GCond *cond)
{
  g_cond_signal_impl (cond);
}

/**
 * g_cond_broadcast:
 * @cond: a #GCond
 *
 * If threads are waiting for @cond, all of them are unblocked.
 * If no threads are waiting for @cond, this function has no effect.
 * It is good practice to lock the same mutex as the waiting threads
 * while calling this function, though not required.
 */
void
g_cond_broadcast (GCond *cond)
{
  g_cond_broadcast_impl (cond);
}

/**
 * g_cond_wait_until:
 * @cond: a #GCond
 * @mutex: a #GMutex that is currently locked
 * @end_time: the monotonic time to wait until
 *
 * Waits until either @cond is signalled or @end_time has passed.
 *
 * As with g_cond_wait() it is possible that a spurious or stolen wakeup
 * could occur.  For that reason, waiting on a condition variable should
 * always be in a loop, based on an explicitly-checked predicate.
 *
 * %TRUE is returned if the condition variable was signalled (or in the
 * case of a spurious wakeup).  %FALSE is returned if @end_time has
 * passed.
 *
 * The following code shows how to correctly perform a timed wait on a
 * condition variable (extending the example presented in the
 * documentation for #GCond):
 *
 * |[<!-- language="C" -->
 * gpointer
 * pop_data_timed (void)
 * {
 *   gint64 end_time;
 *   gpointer data;
 *
 *   g_mutex_lock (&data_mutex);
 *
 *   end_time = g_get_monotonic_time () + 5 * G_TIME_SPAN_SECOND;
 *   while (!current_data)
 *     if (!g_cond_wait_until (&data_cond, &data_mutex, end_time))
 *       {
 *         // timeout has passed.
 *         g_mutex_unlock (&data_mutex);
 *         return NULL;
 *       }
 *
 *   // there is data for us
 *   data = current_data;
 *   current_data = NULL;
 *
 *   g_mutex_unlock (&data_mutex);
 *
 *   return data;
 * }
 * ]|
 *
 * Notice that the end time is calculated once, before entering the
 * loop and reused.  This is the motivation behind the use of absolute
 * time on this API -- if a relative time of 5 seconds were passed
 * directly to the call and a spurious wakeup occurred, the program would
 * have to start over waiting again (which would lead to a total wait
 * time of more than 5 seconds).
 *
 * Returns: %TRUE on a signal, %FALSE on a timeout
 * Since: 2.32
 **/
gboolean
g_cond_wait_until (GCond  *cond,
                   GMutex *mutex,
                   gint64  end_time)
{
  return g_cond_wait_until_impl (cond, mutex, end_time);
}

/* {{{1 GPrivate */

/**
 * GPrivate:
 *
 * The #GPrivate struct is an opaque data structure to represent a
 * thread-local data key. It is approximately equivalent to the
 * pthread_setspecific()/pthread_getspecific() APIs on POSIX and to
 * TlsSetValue()/TlsGetValue() on Windows.
 *
 * If you don't already know why you might want this functionality,
 * then you probably don't need it.
 *
 * #GPrivate is a very limited resource (as far as 128 per program,
 * shared between all libraries). It is also not possible to destroy a
 * #GPrivate after it has been used. As such, it is only ever acceptable
 * to use #GPrivate in static scope, and even then sparingly so.
 *
 * See G_PRIVATE_INIT() for a couple of examples.
 *
 * The #GPrivate structure should be considered opaque.  It should only
 * be accessed via the g_private_ functions.
 */

/**
 * G_PRIVATE_INIT:
 * @notify: a #GDestroyNotify
 *
 * A macro to assist with the static initialisation of a #GPrivate.
 *
 * This macro is useful for the case that a #GDestroyNotify function
 * should be associated with the key.  This is needed when the key will be
 * used to point at memory that should be deallocated when the thread
 * exits.
 *
 * Additionally, the #GDestroyNotify will also be called on the previous
 * value stored in the key when g_private_replace() is used.
 *
 * If no #GDestroyNotify is needed, then use of this macro is not
 * required -- if the #GPrivate is declared in static scope then it will
 * be properly initialised by default (ie: to all zeros).  See the
 * examples below.
 *
 * |[<!-- language="C" -->
 * static GPrivate name_key = G_PRIVATE_INIT (g_free);
 *
 * // return value should not be freed
 * const gchar *
 * get_local_name (void)
 * {
 *   return g_private_get (&name_key);
 * }
 *
 * void
 * set_local_name (const gchar *name)
 * {
 *   g_private_replace (&name_key, g_strdup (name));
 * }
 *
 *
 * static GPrivate count_key;   // no free function
 *
 * gint
 * get_local_count (void)
 * {
 *   return GPOINTER_TO_INT (g_private_get (&count_key));
 * }
 *
 * void
 * set_local_count (gint count)
 * {
 *   g_private_set (&count_key, GINT_TO_POINTER (count));
 * }
 * ]|
 *
 * Since: 2.32
 **/

/**
 * g_private_get:
 * @key: a #GPrivate
 *
 * Returns the current value of the thread local variable @key.
 *
 * If the value has not yet been set in this thread, %NULL is returned.
 * Values are never copied between threads (when a new thread is
 * created, for example).
 *
 * Returns: the thread-local value
 */
gpointer
g_private_get (GPrivate *key)
{
  return g_private_get_impl (key);
}

/**
 * g_private_set:
 * @key: a #GPrivate
 * @value: the new value
 *
 * Sets the thread local variable @key to have the value @value in the
 * current thread.
 *
 * This function differs from g_private_replace() in the following way:
 * the #GDestroyNotify for @key is not called on the old value.
 */
void
g_private_set (GPrivate *key,
               gpointer  value)
{
  g_private_set_impl (key, value);
}

/**
 * g_private_replace:
 * @key: a #GPrivate
 * @value: the new value
 *
 * Sets the thread local variable @key to have the value @value in the
 * current thread.
 *
 * This function differs from g_private_set() in the following way: if
 * the previous value was non-%NULL then the #GDestroyNotify handler for
 * @key is run on it.
 *
 * Since: 2.32
 **/
void
g_private_replace (GPrivate *key,
                   gpointer  value)
{
  g_private_replace_impl (key, value);
}

/* {{{1 GThread */

/**
 * g_thread_yield:
 *
 * Causes the calling thread to voluntarily relinquish the CPU, so
 * that other threads can run.
 *
 * This function is often used as a method to make busy wait less evil.
 */
void
g_thread_yield (void)
{
  g_thread_yield_impl ();
}

/* Epilogue {{{1 */
/* vim: set foldmethod=marker: */
