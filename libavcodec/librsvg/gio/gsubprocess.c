/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright © 2012, 2013 Red Hat, Inc.
 * Copyright © 2012, 2013 Canonical Limited
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * See the included COPYING file for more information.
 *
 * Authors: Colin Walters <walters@verbum.org>
 *          Ryan Lortie <desrt@desrt.ca>
 */

/**
 * GSubprocess:
 *
 * `GSubprocess` allows the creation of and interaction with child
 * processes.
 *
 * Processes can be communicated with using standard GIO-style APIs (ie:
 * [class@Gio.InputStream], [class@Gio.OutputStream]). There are GIO-style APIs
 * to wait for process termination (ie: cancellable and with an asynchronous
 * variant).
 *
 * There is an API to force a process to terminate, as well as a
 * race-free API for sending UNIX signals to a subprocess.
 *
 * One major advantage that GIO brings over the core GLib library is
 * comprehensive API for asynchronous I/O, such
 * [method@Gio.OutputStream.splice_async].  This makes `GSubprocess`
 * significantly more powerful and flexible than equivalent APIs in
 * some other languages such as the `subprocess.py`
 * included with Python.  For example, using `GSubprocess` one could
 * create two child processes, reading standard output from the first,
 * processing it, and writing to the input stream of the second, all
 * without blocking the main loop.
 *
 * A powerful [method@Gio.Subprocess.communicate] API is provided similar to the
 * `communicate()` method of `subprocess.py`. This enables very easy
 * interaction with a subprocess that has been opened with pipes.
 *
 * `GSubprocess` defaults to tight control over the file descriptors open
 * in the child process, avoiding dangling-FD issues that are caused by
 * a simple `fork()`/`exec()`.  The only open file descriptors in the
 * spawned process are ones that were explicitly specified by the
 * `GSubprocess` API (unless `G_SUBPROCESS_FLAGS_INHERIT_FDS` was
 * specified).
 *
 * `GSubprocess` will quickly reap all child processes as they exit,
 * avoiding ‘zombie processes’ remaining around for long periods of
 * time.  [method@Gio.Subprocess.wait] can be used to wait for this to happen,
 * but it will happen even without the call being explicitly made.
 *
 * As a matter of principle, `GSubprocess` has no API that accepts
 * shell-style space-separated strings.  It will, however, match the
 * typical shell behaviour of searching the `PATH` for executables that do
 * not contain a directory separator in their name. By default, the `PATH`
 * of the current process is used.  You can specify
 * `G_SUBPROCESS_FLAGS_SEARCH_PATH_FROM_ENVP` to use the `PATH` of the
 * launcher environment instead.
 *
 * `GSubprocess` attempts to have a very simple API for most uses (ie:
 * spawning a subprocess with arguments and support for most typical
 * kinds of input and output redirection).  See [ctor@Gio.Subprocess.new]. The
 * [class@Gio.SubprocessLauncher] API is provided for more complicated cases
 * (advanced types of redirection, environment variable manipulation,
 * change of working directory, child setup functions, etc).
 *
 * A typical use of `GSubprocess` will involve calling
 * [ctor@Gio.Subprocess.new], followed by [method@Gio.Subprocess.wait_async] or
 * [method@Gio.Subprocess.wait].  After the process exits, the status can be
 * checked using functions such as [method@Gio.Subprocess.get_if_exited] (which
 * are similar to the familiar `WIFEXITED`-style POSIX macros).
 *
 * Note that as of GLib 2.82, creating a `GSubprocess` causes the signal
 * `SIGPIPE` to be ignored for the remainder of the program. If you are writing
 * a command-line utility that uses `GSubprocess`, you may need to take into
 * account the fact that your program will not automatically be killed
 * if it tries to write to `stdout` after it has been closed.
 *
 * Since: 2.40
 **/

#include "config.h"

#include "gsubprocess.h"
#include "gsubprocesslauncher-private.h"
#include "gasyncresult.h"
#include "giostream.h"
#include "gmemoryinputstream.h"
#include "../glib/glibintl.h"
#include "../glib/glib-private.h"

#include <string.h>
#ifdef G_OS_UNIX
#include "gunixoutputstream.h"
#include "gfiledescriptorbased.h"
#include "gunixinputstream.h"
#include "gstdio.h"
#include "glib-unix.h"
#include <fcntl.h>
#endif
#ifdef G_OS_WIN32
#include <windows.h>
#include <io.h>
#include "giowin32-priv.h"
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#else
#define HAVE_O_CLOEXEC 1
#endif

#define COMMUNICATE_READ_SIZE 4096

/* A GSubprocess can have two possible states: running and not.
 *
 * These two states are reflected by the value of 'pid'.  If it is
 * non-zero then the process is running, with that pid.
 *
 * When a GSubprocess is first created with g_object_new() it is not
 * running.  When it is finalized, it is also not running.
 *
 * During initable_init(), if the g_spawn() is successful then we
 * immediately register a child watch and take an extra ref on the
 * subprocess.  That reference doesn't drop until the child has quit,
 * which is why finalize can only happen in the non-running state.  In
 * the event that the g_spawn() failed we will still be finalizing a
 * non-running GSubprocess (before returning from g_subprocess_new())
 * with NULL.
 *
 * We make extensive use of the glib worker thread to guarantee
 * race-free operation.  As with all child watches, glib calls waitpid()
 * in the worker thread.  It reports the child exiting to us via the
 * worker thread (which means that we can do synchronous waits without
 * running a separate loop).  We also send signals to the child process
 * via the worker thread so that we don't race with waitpid() and
 * accidentally send a signal to an already-reaped child.
 */
static void initable_iface_init (GInitableIface         *initable_iface);

typedef GObjectClass GSubprocessClass;

struct _GSubprocess
{
  GObject parent;

  /* only used during construction */
  GSubprocessLauncher *launcher;
  GSubprocessFlags flags;
  gchar **argv;

  /* state tracking variables */
  gchar identifier[24];
  int status;
  GPid pid;

  /* list of GTask */
  GMutex pending_waits_lock;
  GSList *pending_waits;

  /* These are the streams created if a pipe is requested via flags. */
  GOutputStream *stdin_pipe;
  GInputStream  *stdout_pipe;
  GInputStream  *stderr_pipe;
};

G_DEFINE_TYPE_WITH_CODE (GSubprocess, g_subprocess, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, initable_iface_init))

enum
{
  PROP_0,
  PROP_FLAGS,
  PROP_ARGV,
  N_PROPS
};

static GInputStream *
platform_input_stream_from_spawn_fd (gint fd)
{
  if (fd < 0)
    return NULL;

#ifdef G_OS_UNIX
  return g_unix_input_stream_new (fd, TRUE);
#else
  return g_win32_input_stream_new_from_fd (fd, TRUE);
#endif
}

static GOutputStream *
platform_output_stream_from_spawn_fd (gint fd)
{
  if (fd < 0)
    return NULL;

#ifdef G_OS_UNIX
  return g_unix_output_stream_new (fd, TRUE);
#else
  return g_win32_output_stream_new_from_fd (fd, TRUE);
#endif
}

#ifdef G_OS_UNIX
static gint
unix_open_file (const char  *filename,
                gint         mode,
                GError     **error)
{
  gint my_fd;

  my_fd = g_open (filename, mode | O_BINARY | O_CLOEXEC, 0666);

  /* If we return -1 we should also set the error */
  if (my_fd < 0)
    {
      gint saved_errno = errno;
      char *display_name;

      display_name = g_filename_display_name (filename);
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (saved_errno),
                   _("Error opening file “%s”: %s"), display_name,
                   g_strerror (saved_errno));
      g_free (display_name);
      /* fall through... */
    }
#ifndef HAVE_O_CLOEXEC
  else
    fcntl (my_fd, F_SETFD, FD_CLOEXEC);
#endif

  return my_fd;
}
#endif

static void
g_subprocess_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GSubprocess *self = G_SUBPROCESS (object);

  switch (prop_id)
    {
    case PROP_FLAGS:
      self->flags = g_value_get_flags (value);
      break;

    case PROP_ARGV:
      self->argv = g_value_dup_boxed (value);
      break;

    default:
      g_assert_not_reached ();
    }
}

static gboolean
g_subprocess_exited (GPid     pid,
                     gint     status,
                     gpointer user_data)
{
  GSubprocess *self = user_data;
  GSList *tasks;

  g_mutex_lock (&self->pending_waits_lock);
  g_assert (self->pid == pid);
  self->status = status;
  tasks = self->pending_waits;
  self->pending_waits = NULL;
  self->pid = 0;
  g_mutex_unlock (&self->pending_waits_lock);

  /* Signal anyone in g_subprocess_wait_async() to wake up now */
  while (tasks)
    {
      g_task_return_boolean (tasks->data, TRUE);
      g_object_unref (tasks->data);
      tasks = g_slist_delete_link (tasks, tasks);
    }

  g_spawn_close_pid (pid);

  return FALSE;
}

static gboolean
initable_init (GInitable     *initable,
               GCancellable  *cancellable,
               GError       **error)
{
  GSubprocess *self = G_SUBPROCESS (initable);
  gint *pipe_ptrs[3] = { NULL, NULL, NULL };
  gint pipe_fds[3] = { -1, -1, -1 };
  gint close_fds[3] = { -1, -1, -1 };
  GPid pid = 0;
#ifdef G_OS_UNIX
  gint stdin_fd = -1, stdout_fd = -1, stderr_fd = -1;
#endif
  GSpawnFlags spawn_flags = 0;
  gboolean success = FALSE;
  gint i;

  /* this is a programmer error */
  if (!self->argv || !self->argv[0] || !self->argv[0][0])
    return FALSE;

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  /* We must setup the three fds that will end up in the child as stdin,
   * stdout and stderr.
   *
   * First, stdin.
   */
  if (self->flags & G_SUBPROCESS_FLAGS_STDIN_INHERIT)
    spawn_flags |= G_SPAWN_CHILD_INHERITS_STDIN;
  else if (self->flags & G_SUBPROCESS_FLAGS_STDIN_PIPE)
    pipe_ptrs[0] = &pipe_fds[0];
#ifdef G_OS_UNIX
  else if (self->launcher)
    {
      if (self->launcher->stdin_fd != -1)
        stdin_fd = self->launcher->stdin_fd;
      else if (self->launcher->stdin_path != NULL)
        {
          stdin_fd = close_fds[0] = unix_open_file (self->launcher->stdin_path, O_RDONLY, error);
          if (stdin_fd == -1)
            goto out;
        }
    }
#endif

  /* Next, stdout. */
  if (self->flags & G_SUBPROCESS_FLAGS_STDOUT_SILENCE)
    spawn_flags |= G_SPAWN_STDOUT_TO_DEV_NULL;
  else if (self->flags & G_SUBPROCESS_FLAGS_STDOUT_PIPE)
    pipe_ptrs[1] = &pipe_fds[1];
#ifdef G_OS_UNIX
  else if (self->launcher)
    {
      if (self->launcher->stdout_fd != -1)
        stdout_fd = self->launcher->stdout_fd;
      else if (self->launcher->stdout_path != NULL)
        {
          stdout_fd = close_fds[1] = unix_open_file (self->launcher->stdout_path, O_CREAT | O_WRONLY, error);
          if (stdout_fd == -1)
            goto out;
        }
    }
#endif

  /* Finally, stderr. */
  if (self->flags & G_SUBPROCESS_FLAGS_STDERR_SILENCE)
    spawn_flags |= G_SPAWN_STDERR_TO_DEV_NULL;
  else if (self->flags & G_SUBPROCESS_FLAGS_STDERR_PIPE)
    pipe_ptrs[2] = &pipe_fds[2];
#ifdef G_OS_UNIX
  else if (self->flags & G_SUBPROCESS_FLAGS_STDERR_MERGE)
    /* This will work because stderr gets set up after stdout. */
    stderr_fd = 1;
  else if (self->launcher)
    {
      if (self->launcher->stderr_fd != -1)
        stderr_fd = self->launcher->stderr_fd;
      else if (self->launcher->stderr_path != NULL)
        {
          stderr_fd = close_fds[2] = unix_open_file (self->launcher->stderr_path, O_CREAT | O_WRONLY, error);
          if (stderr_fd == -1)
            goto out;
        }
    }
#endif

  /* argv0 has no '/' in it?  We better do a PATH lookup. */
  if (strchr (self->argv[0], G_DIR_SEPARATOR) == NULL)
    {
      if (self->launcher && self->launcher->flags & G_SUBPROCESS_FLAGS_SEARCH_PATH_FROM_ENVP)
        spawn_flags |= G_SPAWN_SEARCH_PATH_FROM_ENVP;
      else
        spawn_flags |= G_SPAWN_SEARCH_PATH;
    }

  if (self->flags & G_SUBPROCESS_FLAGS_INHERIT_FDS)
    spawn_flags |= G_SPAWN_LEAVE_DESCRIPTORS_OPEN;

  spawn_flags |= G_SPAWN_DO_NOT_REAP_CHILD;
  spawn_flags |= G_SPAWN_CLOEXEC_PIPES;

  success = g_spawn_async_with_pipes_and_fds (self->launcher ? self->launcher->cwd : NULL,
                                              (const gchar * const *) self->argv,
                                              (const gchar * const *) (self->launcher ? self->launcher->envp : NULL),
                                              spawn_flags,
#ifdef G_OS_UNIX
                                              self->launcher ? self->launcher->child_setup_func : NULL,
                                              self->launcher ? self->launcher->child_setup_user_data : NULL,
                                              stdin_fd, stdout_fd, stderr_fd,
                                              self->launcher ? (const gint *) self->launcher->source_fds->data : NULL,
                                              self->launcher ? (const gint *) self->launcher->target_fds->data : NULL,
                                              self->launcher ? self->launcher->source_fds->len : 0,
#else
                                              NULL, NULL,
                                              -1, -1, -1,
                                              NULL, NULL, 0,
#endif
                                              &pid,
                                              pipe_ptrs[0], pipe_ptrs[1], pipe_ptrs[2],
                                              error);
  g_assert (success == (pid != 0));

  g_mutex_lock (&self->pending_waits_lock);
  self->pid = pid;
  g_mutex_unlock (&self->pending_waits_lock);

  {
    guint64 identifier;
    gint s G_GNUC_UNUSED  /* when compiling with G_DISABLE_ASSERT */;

#ifdef G_OS_WIN32
    identifier = (guint64) GetProcessId (pid);
#else
    identifier = (guint64) pid;
#endif

    s = g_snprintf (self->identifier, sizeof self->identifier, "%"G_GUINT64_FORMAT, identifier);
    g_assert (0 < s && (gsize) s < sizeof self->identifier);
  }

  /* Start attempting to reap the child immediately */
  if (success)
    {
      GMainContext *worker_context;
      GSource *source;

      worker_context = GLIB_PRIVATE_CALL (g_get_worker_context) ();
      source = g_child_watch_source_new (pid);
      g_source_set_callback (source, (GSourceFunc) g_subprocess_exited, g_object_ref (self), g_object_unref);
      g_source_attach (source, worker_context);
      g_source_unref (source);
    }

#ifdef G_OS_UNIX
out:
#endif
  /* we don't need this past init... */
  self->launcher = NULL;

  for (i = 0; i < 3; i++)
    if (close_fds[i] != -1)
      close (close_fds[i]);

  self->stdin_pipe = platform_output_stream_from_spawn_fd (pipe_fds[0]);
  self->stdout_pipe = platform_input_stream_from_spawn_fd (pipe_fds[1]);
  self->stderr_pipe = platform_input_stream_from_spawn_fd (pipe_fds[2]);

  return success;
}

static void
g_subprocess_finalize (GObject *object)
{
  GSubprocess *self = G_SUBPROCESS (object);

  g_assert (self->pending_waits == NULL);
  g_assert (self->pid == 0);

  g_clear_object (&self->stdin_pipe);
  g_clear_object (&self->stdout_pipe);
  g_clear_object (&self->stderr_pipe);
  g_strfreev (self->argv);

  g_mutex_clear (&self->pending_waits_lock);

  G_OBJECT_CLASS (g_subprocess_parent_class)->finalize (object);
}

static void
g_subprocess_init (GSubprocess  *self)
{
  g_mutex_init (&self->pending_waits_lock);
}

static void
initable_iface_init (GInitableIface *initable_iface)
{
  initable_iface->init = initable_init;
}

static void
g_subprocess_class_init (GSubprocessClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

#ifdef SIGPIPE
  /* There is no portable, thread-safe way to avoid having the process
   * be killed by SIGPIPE when calling write() on a pipe to a subprocess, so we
   * are forced to simply ignore the signal process-wide.
   *
   * This can happen if `G_SUBPROCESS_FLAGS_STDIN_PIPE` is used and the
   * subprocess calls close() on its stdin FD while the parent process is
   * running g_subprocess_communicate().
   *
   * Even if we ignore it though, gdb will still stop if the app
   * receives a SIGPIPE, which can be confusing and annoying. In `gsocket.c`,
   * we can handily also set `MSG_NO_SIGNAL` / `SO_NOSIGPIPE`, but unfortunately
   * there isn’t an equivalent of those for `pipe2`() FDs.
   */
  signal (SIGPIPE, SIG_IGN);
#endif

  gobject_class->finalize = g_subprocess_finalize;
  gobject_class->set_property = g_subprocess_set_property;

  /**
   * GSubprocess:flags:
   *
   * Subprocess flags.
   *
   * Since: 2.40
   */
  g_object_class_install_property (gobject_class, PROP_FLAGS,
                                   g_param_spec_flags ("flags", NULL, NULL,
                                                       G_TYPE_SUBPROCESS_FLAGS, 0, G_PARAM_WRITABLE |
                                                       G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * GSubprocess:argv:
   *
   * Argument vector.
   *
   * Since: 2.40
   */
  g_object_class_install_property (gobject_class, PROP_ARGV,
                                   g_param_spec_boxed ("argv", NULL, NULL,
                                                       G_TYPE_STRV, G_PARAM_WRITABLE |
                                                       G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
}

/**
 * g_subprocess_new: (skip)
 * @flags: flags that define the behaviour of the subprocess
 * @error: (nullable): return location for an error, or %NULL
 * @argv0: first commandline argument to pass to the subprocess
 * @...:   more commandline arguments, followed by %NULL
 *
 * Create a new process with the given flags and varargs argument
 * list.  By default, matching the g_spawn_async() defaults, the
 * child's stdin will be set to the system null device, and
 * stdout/stderr will be inherited from the parent.  You can use
 * @flags to control this behavior.
 *
 * The argument list must be terminated with %NULL.
 *
 * Returns: A newly created #GSubprocess, or %NULL on error (and @error
 *   will be set)
 *
 * Since: 2.40
 */
GSubprocess *
g_subprocess_new (GSubprocessFlags   flags,
                  GError           **error,
                  const gchar       *argv0,
                  ...)
{
  GSubprocess *result;
  GPtrArray *args;
  const gchar *arg;
  va_list ap;

  g_return_val_if_fail (argv0 != NULL && argv0[0] != '\0', NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  args = g_ptr_array_new ();

  va_start (ap, argv0);
  g_ptr_array_add (args, (gchar *) argv0);
  while ((arg = va_arg (ap, const gchar *)))
    g_ptr_array_add (args, (gchar *) arg);
  g_ptr_array_add (args, NULL);
  va_end (ap);

  result = g_subprocess_newv ((const gchar * const *) args->pdata, flags, error);

  g_ptr_array_free (args, TRUE);

  return result;
}

/**
 * g_subprocess_newv: (rename-to g_subprocess_new)
 * @argv: (array zero-terminated=1) (element-type filename): commandline arguments for the subprocess
 * @flags: flags that define the behaviour of the subprocess
 * @error: (nullable): return location for an error, or %NULL
 *
 * Create a new process with the given flags and argument list.
 *
 * The argument list is expected to be %NULL-terminated.
 *
 * Returns: A newly created #GSubprocess, or %NULL on error (and @error
 *   will be set)
 *
 * Since: 2.40
 */
GSubprocess *
g_subprocess_newv (const gchar * const  *argv,
                   GSubprocessFlags      flags,
                   GError              **error)
{
  g_return_val_if_fail (argv != NULL && argv[0] != NULL && argv[0][0] != '\0', NULL);

  return g_initable_new (G_TYPE_SUBPROCESS, NULL, error,
                         "argv", argv,
                         "flags", flags,
                         NULL);
}

/**
 * g_subprocess_get_identifier:
 * @subprocess: a #GSubprocess
 *
 * On UNIX, returns the process ID as a decimal string.
 * On Windows, returns the result of GetProcessId() also as a string.
 * If the subprocess has terminated, this will return %NULL.
 *
 * Returns: (nullable): the subprocess identifier, or %NULL if the subprocess
 *    has terminated
 * Since: 2.40
 */
const gchar *
g_subprocess_get_identifier (GSubprocess *subprocess)
{
  const char *identifier;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), NULL);

  g_mutex_lock (&subprocess->pending_waits_lock);
  identifier = subprocess->pid ? subprocess->identifier : NULL;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  return identifier;
}

/**
 * g_subprocess_get_stdin_pipe:
 * @subprocess: a #GSubprocess
 *
 * Gets the #GOutputStream that you can write to in order to give data
 * to the stdin of @subprocess.
 *
 * The process must have been created with %G_SUBPROCESS_FLAGS_STDIN_PIPE and
 * not %G_SUBPROCESS_FLAGS_STDIN_INHERIT, otherwise %NULL will be returned.
 *
 * Returns: (nullable) (transfer none): the stdout pipe
 *
 * Since: 2.40
 **/
GOutputStream *
g_subprocess_get_stdin_pipe (GSubprocess *subprocess)
{
  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), NULL);

  return subprocess->stdin_pipe;
}

/**
 * g_subprocess_get_stdout_pipe:
 * @subprocess: a #GSubprocess
 *
 * Gets the #GInputStream from which to read the stdout output of
 * @subprocess.
 *
 * The process must have been created with %G_SUBPROCESS_FLAGS_STDOUT_PIPE,
 * otherwise %NULL will be returned.
 *
 * Returns: (nullable) (transfer none): the stdout pipe
 *
 * Since: 2.40
 **/
GInputStream *
g_subprocess_get_stdout_pipe (GSubprocess *subprocess)
{
  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), NULL);

  return subprocess->stdout_pipe;
}

/**
 * g_subprocess_get_stderr_pipe:
 * @subprocess: a #GSubprocess
 *
 * Gets the #GInputStream from which to read the stderr output of
 * @subprocess.
 *
 * The process must have been created with %G_SUBPROCESS_FLAGS_STDERR_PIPE,
 * otherwise %NULL will be returned.
 *
 * Returns: (nullable) (transfer none): the stderr pipe
 *
 * Since: 2.40
 **/
GInputStream *
g_subprocess_get_stderr_pipe (GSubprocess *subprocess)
{
  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), NULL);

  return subprocess->stderr_pipe;
}

/* Remove the first list element containing @data, and return %TRUE. If no
 * such element is found, return %FALSE. */
static gboolean
slist_remove_if_present (GSList        **list,
                         gconstpointer   data)
{
  GSList *l, *prev;

  for (l = *list, prev = NULL; l != NULL; prev = l, l = prev->next)
    {
      if (l->data == data)
        {
          if (prev != NULL)
            prev->next = l->next;
          else
            *list = l->next;

          g_slist_free_1 (l);

          return TRUE;
        }
    }

  return FALSE;
}

static void
g_subprocess_wait_cancelled (GCancellable *cancellable,
                             gpointer      user_data)
{
  GTask *task = user_data;
  GSubprocess *self;
  gboolean task_was_pending;

  self = g_task_get_source_object (task);

  g_mutex_lock (&self->pending_waits_lock);
  task_was_pending = slist_remove_if_present (&self->pending_waits, task);
  g_mutex_unlock (&self->pending_waits_lock);

  if (task_was_pending)
    {
      g_task_return_boolean (task, FALSE);
      g_object_unref (task);  /* ref from pending_waits */
    }
}

/**
 * g_subprocess_wait_async:
 * @subprocess: a #GSubprocess
 * @cancellable: a #GCancellable, or %NULL
 * @callback: a #GAsyncReadyCallback to call when the operation is complete
 * @user_data: user_data for @callback
 *
 * Wait for the subprocess to terminate.
 *
 * This is the asynchronous version of g_subprocess_wait().
 *
 * Since: 2.40
 */
void
g_subprocess_wait_async (GSubprocess         *subprocess,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data)
{
  GTask *task;

  task = g_task_new (subprocess, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_subprocess_wait_async);

  g_mutex_lock (&subprocess->pending_waits_lock);
  if (subprocess->pid)
    {
      /* Only bother with cancellable if we're putting it in the list.
       * If not, it's going to dispatch immediately anyway and we will
       * see the cancellation in the _finish().
       */
      if (cancellable)
        g_signal_connect_object (cancellable, "cancelled",
                                 G_CALLBACK (g_subprocess_wait_cancelled),
                                 task, G_CONNECT_DEFAULT);

      subprocess->pending_waits = g_slist_prepend (subprocess->pending_waits, task);
      task = NULL;
    }
  g_mutex_unlock (&subprocess->pending_waits_lock);

  /* If we still have task then it's because did_exit is already TRUE */
  if (task != NULL)
    {
      g_task_return_boolean (task, TRUE);
      g_object_unref (task);
    }
}

/**
 * g_subprocess_wait_finish:
 * @subprocess: a #GSubprocess
 * @result: the #GAsyncResult passed to your #GAsyncReadyCallback
 * @error: a pointer to a %NULL #GError, or %NULL
 *
 * Collects the result of a previous call to
 * g_subprocess_wait_async().
 *
 * Returns: %TRUE if successful, or %FALSE with @error set
 *
 * Since: 2.40
 */
gboolean
g_subprocess_wait_finish (GSubprocess   *subprocess,
                          GAsyncResult  *result,
                          GError       **error)
{
  return g_task_propagate_boolean (G_TASK (result), error);
}

/* Some generic helpers for emulating synchronous operations using async
 * operations.
 */
static void
g_subprocess_sync_setup (void)
{
  g_main_context_push_thread_default (g_main_context_new ());
}

static void
g_subprocess_sync_done (GObject      *source_object,
                        GAsyncResult *result,
                        gpointer      user_data)
{
  GAsyncResult **result_ptr = user_data;

  *result_ptr = g_object_ref (result);
}

static void
g_subprocess_sync_complete (GAsyncResult **result)
{
  GMainContext *context = g_main_context_get_thread_default ();

  while (!*result)
    g_main_context_iteration (context, TRUE);

  g_main_context_pop_thread_default (context);
  g_main_context_unref (context);
}

/**
 * g_subprocess_wait:
 * @subprocess: a #GSubprocess
 * @cancellable: a #GCancellable
 * @error: a #GError
 *
 * Synchronously wait for the subprocess to terminate.
 *
 * After the process terminates you can query its exit status with
 * functions such as g_subprocess_get_if_exited() and
 * g_subprocess_get_exit_status().
 *
 * This function does not fail in the case of the subprocess having
 * abnormal termination.  See g_subprocess_wait_check() for that.
 *
 * Cancelling @cancellable doesn't kill the subprocess.  Call
 * g_subprocess_force_exit() if it is desirable.
 *
 * Returns: %TRUE on success, %FALSE if @cancellable was cancelled
 *
 * Since: 2.40
 */
gboolean
g_subprocess_wait (GSubprocess   *subprocess,
                   GCancellable  *cancellable,
                   GError       **error)
{
  GAsyncResult *result = NULL;
  gboolean success;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);

  /* Synchronous waits are actually the 'more difficult' case because we
   * need to deal with the possibility of cancellation.  That more or
   * less implies that we need a main context (to dispatch either of the
   * possible reasons for the operation ending).
   *
   * So we make one and then do this async...
   */

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  /* We can shortcut in the case that the process already quit (but only
   * after we checked the cancellable).
   */
  g_mutex_lock (&subprocess->pending_waits_lock);
  success = subprocess->pid == 0;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  if (success)
    return TRUE;

  /* Otherwise, we need to do this the long way... */
  g_subprocess_sync_setup ();
  g_subprocess_wait_async (subprocess, cancellable, g_subprocess_sync_done, &result);
  g_subprocess_sync_complete (&result);
  success = g_subprocess_wait_finish (subprocess, result, error);
  g_object_unref (result);

  return success;
}

/**
 * g_subprocess_wait_check:
 * @subprocess: a #GSubprocess
 * @cancellable: a #GCancellable
 * @error: a #GError
 *
 * Combines g_subprocess_wait() with g_spawn_check_wait_status().
 *
 * Returns: %TRUE on success, %FALSE if process exited abnormally, or
 * @cancellable was cancelled
 *
 * Since: 2.40
 */
gboolean
g_subprocess_wait_check (GSubprocess   *subprocess,
                         GCancellable  *cancellable,
                         GError       **error)
{
  gint status;

  if (!g_subprocess_wait (subprocess, cancellable, error))
    return FALSE;

  g_mutex_lock (&subprocess->pending_waits_lock);
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  return g_spawn_check_wait_status (status, error);
}

/**
 * g_subprocess_wait_check_async:
 * @subprocess: a #GSubprocess
 * @cancellable: a #GCancellable, or %NULL
 * @callback: a #GAsyncReadyCallback to call when the operation is complete
 * @user_data: user_data for @callback
 *
 * Combines g_subprocess_wait_async() with g_spawn_check_wait_status().
 *
 * This is the asynchronous version of g_subprocess_wait_check().
 *
 * Since: 2.40
 */
void
g_subprocess_wait_check_async (GSubprocess         *subprocess,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  g_subprocess_wait_async (subprocess, cancellable, callback, user_data);
}

/**
 * g_subprocess_wait_check_finish:
 * @subprocess: a #GSubprocess
 * @result: the #GAsyncResult passed to your #GAsyncReadyCallback
 * @error: a pointer to a %NULL #GError, or %NULL
 *
 * Collects the result of a previous call to
 * g_subprocess_wait_check_async().
 *
 * Returns: %TRUE if successful, or %FALSE with @error set
 *
 * Since: 2.40
 */
gboolean
g_subprocess_wait_check_finish (GSubprocess   *subprocess,
                                GAsyncResult  *result,
                                GError       **error)
{
  gint status;

  if (!g_subprocess_wait_finish (subprocess, result, error))
    return FALSE;

  g_mutex_lock (&subprocess->pending_waits_lock);
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  return g_spawn_check_wait_status (status, error);
}

#ifdef G_OS_UNIX
typedef struct
{
  GSubprocess *subprocess;
  gint signalnum;
} SignalRecord;

static gboolean
g_subprocess_actually_send_signal (gpointer user_data)
{
  SignalRecord *signal_record = user_data;

  /* The pid is set to zero from the worker thread as well, so we don't
   * need to take a lock in order to prevent it from changing under us.
   */
  g_mutex_lock (&signal_record->subprocess->pending_waits_lock);
  if (signal_record->subprocess->pid)
    kill (signal_record->subprocess->pid, signal_record->signalnum);
  g_mutex_unlock (&signal_record->subprocess->pending_waits_lock);

  g_object_unref (signal_record->subprocess);

  g_slice_free (SignalRecord, signal_record);

  return FALSE;
}

static void
g_subprocess_dispatch_signal (GSubprocess *subprocess,
                              gint         signalnum)
{
  SignalRecord signal_record = { g_object_ref (subprocess), signalnum };

  g_return_if_fail (G_IS_SUBPROCESS (subprocess));

  /* This MUST be a lower priority than the priority that the child
   * watch source uses in initable_init().
   *
   * Reaping processes, reporting the results back to GSubprocess and
   * sending signals is all done in the glib worker thread.  We cannot
   * have a kill() done after the reap and before the report without
   * risking killing a process that's no longer there so the kill()
   * needs to have the lower priority.
   *
   * G_PRIORITY_HIGH_IDLE is lower priority than G_PRIORITY_DEFAULT.
   */
  g_main_context_invoke_full (GLIB_PRIVATE_CALL (g_get_worker_context) (),
                              G_PRIORITY_HIGH_IDLE,
                              g_subprocess_actually_send_signal,
                              g_slice_dup (SignalRecord, &signal_record),
                              NULL);
}

/**
 * g_subprocess_send_signal:
 * @subprocess: a #GSubprocess
 * @signal_num: the signal number to send
 *
 * Sends the UNIX signal @signal_num to the subprocess, if it is still
 * running.
 *
 * This API is race-free.  If the subprocess has terminated, it will not
 * be signalled.
 *
 * This API is not available on Windows.
 *
 * Since: 2.40
 **/
void
g_subprocess_send_signal (GSubprocess *subprocess,
                          gint         signal_num)
{
  g_return_if_fail (G_IS_SUBPROCESS (subprocess));

  g_subprocess_dispatch_signal (subprocess, signal_num);
}
#endif

/**
 * g_subprocess_force_exit:
 * @subprocess: a #GSubprocess
 *
 * Use an operating-system specific method to attempt an immediate,
 * forceful termination of the process.  There is no mechanism to
 * determine whether or not the request itself was successful;
 * however, you can use g_subprocess_wait() to monitor the status of
 * the process after calling this function.
 *
 * On Unix, this function sends %SIGKILL.
 *
 * Since: 2.40
 **/
void
g_subprocess_force_exit (GSubprocess *subprocess)
{
  g_return_if_fail (G_IS_SUBPROCESS (subprocess));

#ifdef G_OS_UNIX
  g_subprocess_dispatch_signal (subprocess, SIGKILL);
#else
  g_mutex_lock (&subprocess->pending_waits_lock);
  TerminateProcess (subprocess->pid, 1);
  g_mutex_unlock (&subprocess->pending_waits_lock);
#endif
}

/**
 * g_subprocess_get_status:
 * @subprocess: a #GSubprocess
 *
 * Gets the raw status code of the process, as from waitpid().
 *
 * This value has no particular meaning, but it can be used with the
 * macros defined by the system headers such as WIFEXITED.  It can also
 * be used with g_spawn_check_wait_status().
 *
 * It is more likely that you want to use g_subprocess_get_if_exited()
 * followed by g_subprocess_get_exit_status().
 *
 * It is an error to call this function before g_subprocess_wait() has
 * returned.
 *
 * Returns: the (meaningless) waitpid() exit status from the kernel
 *
 * Since: 2.40
 **/
gint
g_subprocess_get_status (GSubprocess *subprocess)
{
  gint status;
  GPid pid;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, FALSE);

  return status;
}

/**
 * g_subprocess_get_successful:
 * @subprocess: a #GSubprocess
 *
 * Checks if the process was "successful".  A process is considered
 * successful if it exited cleanly with an exit status of 0, either by
 * way of the exit() system call or return from main().
 *
 * It is an error to call this function before g_subprocess_wait() has
 * returned.
 *
 * Returns: %TRUE if the process exited cleanly with a exit status of 0
 *
 * Since: 2.40
 **/
gboolean
g_subprocess_get_successful (GSubprocess *subprocess)
{
  GPid pid;
  gint status;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, FALSE);

#ifdef G_OS_UNIX
  return WIFEXITED (status) && WEXITSTATUS (status) == 0;
#else
  return status == 0;
#endif
}

/**
 * g_subprocess_get_if_exited:
 * @subprocess: a #GSubprocess
 *
 * Check if the given subprocess exited normally (ie: by way of exit()
 * or return from main()).
 *
 * This is equivalent to the system WIFEXITED macro.
 *
 * It is an error to call this function before g_subprocess_wait() has
 * returned.
 *
 * Returns: %TRUE if the case of a normal exit
 *
 * Since: 2.40
 **/
gboolean
g_subprocess_get_if_exited (GSubprocess *subprocess)
{
  GPid pid;
  gint status G_GNUC_UNUSED;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, FALSE);

#ifdef G_OS_UNIX
  return WIFEXITED (status);
#else
  return TRUE;
#endif
}

/**
 * g_subprocess_get_exit_status:
 * @subprocess: a #GSubprocess
 *
 * Check the exit status of the subprocess, given that it exited
 * normally.  This is the value passed to the exit() system call or the
 * return value from main.
 *
 * This is equivalent to the system WEXITSTATUS macro.
 *
 * It is an error to call this function before g_subprocess_wait() and
 * unless g_subprocess_get_if_exited() returned %TRUE.
 *
 * Returns: the exit status
 *
 * Since: 2.40
 **/
gint
g_subprocess_get_exit_status (GSubprocess *subprocess)
{
  gint status;
  GPid pid;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), 1);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, 1);

#ifdef G_OS_UNIX
  g_return_val_if_fail (WIFEXITED (status), 1);

  return WEXITSTATUS (status);
#else
  return status;
#endif
}

/**
 * g_subprocess_get_if_signaled:
 * @subprocess: a #GSubprocess
 *
 * Check if the given subprocess terminated in response to a signal.
 *
 * This is equivalent to the system WIFSIGNALED macro.
 *
 * It is an error to call this function before g_subprocess_wait() has
 * returned.
 *
 * Returns: %TRUE if the case of termination due to a signal
 *
 * Since: 2.40
 **/
gboolean
g_subprocess_get_if_signaled (GSubprocess *subprocess)
{
  GPid pid;
  gint status G_GNUC_UNUSED;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, FALSE);

#ifdef G_OS_UNIX
  return WIFSIGNALED (status);
#else
  return FALSE;
#endif
}

/**
 * g_subprocess_get_term_sig:
 * @subprocess: a #GSubprocess
 *
 * Get the signal number that caused the subprocess to terminate, given
 * that it terminated due to a signal.
 *
 * This is equivalent to the system WTERMSIG macro.
 *
 * It is an error to call this function before g_subprocess_wait() and
 * unless g_subprocess_get_if_signaled() returned %TRUE.
 *
 * Returns: the signal causing termination
 *
 * Since: 2.40
 **/
gint
g_subprocess_get_term_sig (GSubprocess *subprocess)
{
  GPid pid;
  gint status G_GNUC_UNUSED;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), 0);

  g_mutex_lock (&subprocess->pending_waits_lock);
  pid = subprocess->pid;
  status = subprocess->status;
  g_mutex_unlock (&subprocess->pending_waits_lock);

  g_return_val_if_fail (pid == 0, 0);

#ifdef G_OS_UNIX
  g_return_val_if_fail (WIFSIGNALED (status), 0);

  return WTERMSIG (status);
#else
  g_critical ("g_subprocess_get_term_sig() called on Windows, where "
              "g_subprocess_get_if_signaled() always returns FALSE...");
  return 0;
#endif
}

/*< private >*/
void
g_subprocess_set_launcher (GSubprocess         *subprocess,
                           GSubprocessLauncher *launcher)
{
  subprocess->launcher = launcher;
}


/* g_subprocess_communicate implementation below:
 *
 * This is a tough problem.  We have to watch 5 things at the same time:
 *
 *  - writing to stdin made progress
 *  - reading from stdout made progress
 *  - reading from stderr made progress
 *  - process terminated
 *  - cancellable being cancelled by caller
 *
 * We use a GMainContext for all of these (either as async function
 * calls or as a GSource (in the case of the cancellable).  That way at
 * least we don't have to worry about threading.
 *
 * For the sync case we use the usual trick of creating a private main
 * context and iterating it until completion.
 *
 * It's very possible that the process will dump a lot of data to stdout
 * just before it quits, so we can easily have data to read from stdout
 * and see the process has terminated at the same time.  We want to make
 * sure that we read all of the data from the pipes first, though, so we
 * do IO operations at a higher priority than the wait operation (which
 * is at G_IO_PRIORITY_DEFAULT).  Even in the case that we have to do
 * multiple reads to get this data, the pipe() will always be polling
 * as ready and with the async result for the read at a higher priority,
 * the main context will not dispatch the completion for the wait().
 *
 * We keep our own private GCancellable.  In the event that any of the
 * above suffers from an error condition (including the user cancelling
 * their cancellable) we immediately dispatch the GTask with the error
 * result and fire our cancellable to cleanup any pending operations.
 * In the case that the error is that the user's cancellable was fired,
 * it's vaguely wasteful to report an error because GTask will handle
 * this automatically, so we just return FALSE.
 *
 * We let each pending sub-operation take a ref on the GTask of the
 * communicate operation.  We have to be careful that we don't report
 * the task completion more than once, though, so we keep a flag for
 * that.
 */
typedef struct
{
  const gchar *stdin_data;
  gsize stdin_length;
  gsize stdin_offset;

  gboolean add_nul;

  GInputStream *stdin_buf;
  GMemoryOutputStream *stdout_buf;
  GMemoryOutputStream *stderr_buf;

  GCancellable *cancellable;
  GSource      *cancellable_source;

  guint         outstanding_ops;
  gboolean      reported_error;
} CommunicateState;

static void
g_subprocess_communicate_made_progress (GObject      *source_object,
                                        GAsyncResult *result,
                                        gpointer      user_data)
{
  CommunicateState *state;
  GSubprocess *subprocess;
  GError *error = NULL;
  gpointer source;
  GTask *task;

  g_assert (source_object != NULL);

  task = user_data;
  subprocess = g_task_get_source_object (task);
  state = g_task_get_task_data (task);
  source = source_object;

  state->outstanding_ops--;

  if (source == subprocess->stdin_pipe ||
      source == state->stdout_buf ||
      source == state->stderr_buf)
    {
      if (g_output_stream_splice_finish ((GOutputStream*) source, result, &error) == -1)
        goto out;

      if (source == state->stdout_buf ||
          source == state->stderr_buf)
        {
          /* This is a memory stream, so it can't be cancelled or return
           * an error really.
           */
          if (state->add_nul)
            {
              gsize bytes_written;
              if (!g_output_stream_write_all (source, "\0", 1, &bytes_written,
                                              NULL, &error))
                goto out;
            }
          if (!g_output_stream_close (source, NULL, &error))
            goto out;
        }
    }
  else if (source == subprocess)
    {
      (void) g_subprocess_wait_finish (subprocess, result, &error);
    }
  else
    g_assert_not_reached ();

 out:
  if (error)
    {
      /* Only report the first error we see.
       *
       * We might be seeing an error as a result of the cancellation
       * done when the process quits.
       */
      if (!state->reported_error)
        {
          state->reported_error = TRUE;
          g_cancellable_cancel (state->cancellable);
          g_task_return_error (task, error);
        }
      else
        g_error_free (error);
    }
  else if (state->outstanding_ops == 0)
    {
      g_task_return_boolean (task, TRUE);
    }

  /* And drop the original ref */
  g_object_unref (task);
}

static gboolean
g_subprocess_communicate_cancelled (GCancellable *cancellable,
                                    gpointer      user_data)
{
  CommunicateState *state = user_data;

  g_cancellable_cancel (state->cancellable);

  return FALSE;
}

static void
g_subprocess_communicate_state_free (gpointer data)
{
  CommunicateState *state = data;

  g_clear_object (&state->cancellable);
  g_clear_object (&state->stdin_buf);
  g_clear_object (&state->stdout_buf);
  g_clear_object (&state->stderr_buf);

  if (state->cancellable_source)
    {
      g_source_destroy (state->cancellable_source);
      g_source_unref (state->cancellable_source);
    }

  g_slice_free (CommunicateState, state);
}

static CommunicateState *
g_subprocess_communicate_internal (GSubprocess         *subprocess,
                                   gboolean             add_nul,
                                   GBytes              *stdin_buf,
                                   GCancellable        *cancellable,
                                   GAsyncReadyCallback  callback,
                                   gpointer             user_data)
{
  CommunicateState *state;
  GTask *task;

  task = g_task_new (subprocess, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_subprocess_communicate_internal);

  state = g_slice_new0 (CommunicateState);
  g_task_set_task_data (task, state, g_subprocess_communicate_state_free);

  state->cancellable = g_cancellable_new ();
  state->add_nul = add_nul;

  if (cancellable)
    {
      state->cancellable_source = g_cancellable_source_new (cancellable);
      /* No ref held here, but we unref the source from state's free function */
      g_source_set_callback (state->cancellable_source,
                             G_SOURCE_FUNC (g_subprocess_communicate_cancelled),
                             state, NULL);
      g_source_attach (state->cancellable_source, g_main_context_get_thread_default ());
    }

  if (subprocess->stdin_pipe)
    {
      g_assert (stdin_buf != NULL);

#ifdef G_OS_UNIX
      /* We're doing async writes to the pipe, and the async write mechanism assumes
       * that streams polling as writable do SOME progress (possibly partial) and then
       * stop, but never block.
       *
       * However, for blocking pipes, unix will return writable if there is *any* space left
       * but still block until the full buffer size is available before returning from write.
       * So, to avoid async blocking on the main loop we make this non-blocking here.
       *
       * It should be safe to change the fd because we're the only user at this point as
       * per the g_subprocess_communicate() docs, and all the code called by this function
       * properly handles non-blocking fds.
       */
      g_unix_set_fd_nonblocking (g_unix_output_stream_get_fd (G_UNIX_OUTPUT_STREAM (subprocess->stdin_pipe)), TRUE, NULL);
#endif

      state->stdin_buf = g_memory_input_stream_new_from_bytes (stdin_buf);
      g_output_stream_splice_async (subprocess->stdin_pipe, (GInputStream*)state->stdin_buf,
                                    G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE | G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
                                    G_PRIORITY_DEFAULT, state->cancellable,
                                    g_subprocess_communicate_made_progress, g_object_ref (task));
      state->outstanding_ops++;
    }

  if (subprocess->stdout_pipe)
    {
      state->stdout_buf = (GMemoryOutputStream*)g_memory_output_stream_new_resizable ();
      g_output_stream_splice_async ((GOutputStream*)state->stdout_buf, subprocess->stdout_pipe,
                                    G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE,
                                    G_PRIORITY_DEFAULT, state->cancellable,
                                    g_subprocess_communicate_made_progress, g_object_ref (task));
      state->outstanding_ops++;
    }

  if (subprocess->stderr_pipe)
    {
      state->stderr_buf = (GMemoryOutputStream*)g_memory_output_stream_new_resizable ();
      g_output_stream_splice_async ((GOutputStream*)state->stderr_buf, subprocess->stderr_pipe,
                                    G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE,
                                    G_PRIORITY_DEFAULT, state->cancellable,
                                    g_subprocess_communicate_made_progress, g_object_ref (task));
      state->outstanding_ops++;
    }

  g_subprocess_wait_async (subprocess, state->cancellable,
                           g_subprocess_communicate_made_progress, g_object_ref (task));
  state->outstanding_ops++;

  g_object_unref (task);
  return state;
}

/**
 * g_subprocess_communicate:
 * @subprocess: a #GSubprocess
 * @stdin_buf: (nullable): data to send to the stdin of the subprocess, or %NULL
 * @cancellable: a #GCancellable
 * @stdout_buf: (out) (nullable) (optional) (transfer full): data read from the subprocess stdout
 * @stderr_buf: (out) (nullable) (optional) (transfer full): data read from the subprocess stderr
 * @error: a pointer to a %NULL #GError pointer, or %NULL
 *
 * Communicate with the subprocess until it terminates, and all input
 * and output has been completed.
 *
 * If @stdin_buf is given, the subprocess must have been created with
 * %G_SUBPROCESS_FLAGS_STDIN_PIPE.  The given data is fed to the
 * stdin of the subprocess and the pipe is closed (ie: EOF).
 *
 * At the same time (as not to cause blocking when dealing with large
 * amounts of data), if %G_SUBPROCESS_FLAGS_STDOUT_PIPE or
 * %G_SUBPROCESS_FLAGS_STDERR_PIPE were used, reads from those
 * streams.  The data that was read is returned in @stdout and/or
 * the @stderr.
 *
 * If the subprocess was created with %G_SUBPROCESS_FLAGS_STDOUT_PIPE,
 * @stdout_buf will contain the data read from stdout.  Otherwise, for
 * subprocesses not created with %G_SUBPROCESS_FLAGS_STDOUT_PIPE,
 * @stdout_buf will be set to %NULL.  Similar provisions apply to
 * @stderr_buf and %G_SUBPROCESS_FLAGS_STDERR_PIPE.
 *
 * As usual, any output variable may be given as %NULL to ignore it.
 *
 * If you desire the stdout and stderr data to be interleaved, create
 * the subprocess with %G_SUBPROCESS_FLAGS_STDOUT_PIPE and
 * %G_SUBPROCESS_FLAGS_STDERR_MERGE.  The merged result will be returned
 * in @stdout_buf and @stderr_buf will be set to %NULL.
 *
 * In case of any error (including cancellation), %FALSE will be
 * returned with @error set.  Some or all of the stdin data may have
 * been written.  Any stdout or stderr data that has been read will be
 * discarded. None of the out variables (aside from @error) will have
 * been set to anything in particular and should not be inspected.
 *
 * In the case that %TRUE is returned, the subprocess has exited and the
 * exit status inspection APIs (eg: g_subprocess_get_if_exited(),
 * g_subprocess_get_exit_status()) may be used.
 *
 * You should not attempt to use any of the subprocess pipes after
 * starting this function, since they may be left in strange states,
 * even if the operation was cancelled.  You should especially not
 * attempt to interact with the pipes while the operation is in progress
 * (either from another thread or if using the asynchronous version).
 *
 * Returns: %TRUE if successful
 *
 * Since: 2.40
 **/
gboolean
g_subprocess_communicate (GSubprocess   *subprocess,
                          GBytes        *stdin_buf,
                          GCancellable  *cancellable,
                          GBytes       **stdout_buf,
                          GBytes       **stderr_buf,
                          GError       **error)
{
  GAsyncResult *result = NULL;
  gboolean success;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);
  g_return_val_if_fail (stdin_buf == NULL || (subprocess->flags & G_SUBPROCESS_FLAGS_STDIN_PIPE), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  g_subprocess_sync_setup ();
  g_subprocess_communicate_internal (subprocess, FALSE, stdin_buf, cancellable,
                                     g_subprocess_sync_done, &result);
  g_subprocess_sync_complete (&result);
  success = g_subprocess_communicate_finish (subprocess, result, stdout_buf, stderr_buf, error);
  g_object_unref (result);

  return success;
}

/**
 * g_subprocess_communicate_async:
 * @subprocess: Self
 * @stdin_buf: (nullable): Input data, or %NULL
 * @cancellable: (nullable): Cancellable
 * @callback: Callback
 * @user_data: User data
 *
 * Asynchronous version of g_subprocess_communicate().  Complete
 * invocation with g_subprocess_communicate_finish().
 */
void
g_subprocess_communicate_async (GSubprocess         *subprocess,
                                GBytes              *stdin_buf,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  g_return_if_fail (G_IS_SUBPROCESS (subprocess));
  g_return_if_fail (stdin_buf == NULL || (subprocess->flags & G_SUBPROCESS_FLAGS_STDIN_PIPE));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  g_subprocess_communicate_internal (subprocess, FALSE, stdin_buf, cancellable, callback, user_data);
}

/**
 * g_subprocess_communicate_finish:
 * @subprocess: Self
 * @result: Result
 * @stdout_buf: (out) (nullable) (optional) (transfer full): Return location for stdout data
 * @stderr_buf: (out) (nullable) (optional) (transfer full): Return location for stderr data
 * @error: Error
 *
 * Complete an invocation of g_subprocess_communicate_async().
 */
gboolean
g_subprocess_communicate_finish (GSubprocess   *subprocess,
                                 GAsyncResult  *result,
                                 GBytes       **stdout_buf,
                                 GBytes       **stderr_buf,
                                 GError       **error)
{
  gboolean success;
  CommunicateState *state;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);
  g_return_val_if_fail (g_task_is_valid (result, subprocess), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  g_object_ref (result);

  state = g_task_get_task_data ((GTask*)result);
  success = g_task_propagate_boolean ((GTask*)result, error);

  if (success)
    {
      if (stdout_buf)
        *stdout_buf = (state->stdout_buf != NULL) ? g_memory_output_stream_steal_as_bytes (state->stdout_buf) : NULL;
      if (stderr_buf)
        *stderr_buf = (state->stderr_buf != NULL) ? g_memory_output_stream_steal_as_bytes (state->stderr_buf) : NULL;
    }

  g_object_unref (result);
  return success;
}

/**
 * g_subprocess_communicate_utf8:
 * @subprocess: a #GSubprocess
 * @stdin_buf: (nullable): data to send to the stdin of the subprocess, or %NULL
 * @cancellable: a #GCancellable
 * @stdout_buf: (out) (nullable) (optional) (transfer full): data read from the subprocess stdout
 * @stderr_buf: (out) (nullable) (optional) (transfer full): data read from the subprocess stderr
 * @error: a pointer to a %NULL #GError pointer, or %NULL
 *
 * Like g_subprocess_communicate(), but validates the output of the
 * process as UTF-8, and returns it as a regular NUL terminated string.
 *
 * On error, @stdout_buf and @stderr_buf will be set to undefined values and
 * should not be used.
 */
gboolean
g_subprocess_communicate_utf8 (GSubprocess   *subprocess,
                               const char    *stdin_buf,
                               GCancellable  *cancellable,
                               char         **stdout_buf,
                               char         **stderr_buf,
                               GError       **error)
{
  GAsyncResult *result = NULL;
  gboolean success;
  GBytes *stdin_bytes;
  size_t stdin_buf_len = 0;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);
  g_return_val_if_fail (stdin_buf == NULL || (subprocess->flags & G_SUBPROCESS_FLAGS_STDIN_PIPE), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (stdin_buf != NULL)
    stdin_buf_len = strlen (stdin_buf);
  stdin_bytes = g_bytes_new (stdin_buf, stdin_buf_len);

  g_subprocess_sync_setup ();
  g_subprocess_communicate_internal (subprocess, TRUE, stdin_bytes, cancellable,
                                     g_subprocess_sync_done, &result);
  g_subprocess_sync_complete (&result);
  success = g_subprocess_communicate_utf8_finish (subprocess, result, stdout_buf, stderr_buf, error);
  g_object_unref (result);

  g_bytes_unref (stdin_bytes);
  return success;
}

/**
 * g_subprocess_communicate_utf8_async:
 * @subprocess: Self
 * @stdin_buf: (nullable): Input data, or %NULL
 * @cancellable: Cancellable
 * @callback: Callback
 * @user_data: User data
 *
 * Asynchronous version of g_subprocess_communicate_utf8().  Complete
 * invocation with g_subprocess_communicate_utf8_finish().
 */
void
g_subprocess_communicate_utf8_async (GSubprocess         *subprocess,
                                     const char          *stdin_buf,
                                     GCancellable        *cancellable,
                                     GAsyncReadyCallback  callback,
                                     gpointer             user_data)
{
  GBytes *stdin_bytes;
  size_t stdin_buf_len = 0;

  g_return_if_fail (G_IS_SUBPROCESS (subprocess));
  g_return_if_fail (stdin_buf == NULL || (subprocess->flags & G_SUBPROCESS_FLAGS_STDIN_PIPE));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  if (stdin_buf != NULL)
    stdin_buf_len = strlen (stdin_buf);
  stdin_bytes = g_bytes_new (stdin_buf, stdin_buf_len);

  g_subprocess_communicate_internal (subprocess, TRUE, stdin_bytes, cancellable, callback, user_data);

  g_bytes_unref (stdin_bytes);
}

static gboolean
communicate_result_validate_utf8 (const char            *stream_name,
                                  char                 **return_location,
                                  GMemoryOutputStream   *buffer,
                                  GError               **error)
{
  if (return_location == NULL)
    return TRUE;

  if (buffer)
    {
      const char *end;
      *return_location = g_memory_output_stream_steal_data (buffer);
      if (!g_utf8_validate (*return_location, -1, &end))
        {
          g_free (*return_location);
          *return_location = NULL;
          g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                       "Invalid UTF-8 in child %s at offset %lu",
                       stream_name,
                       (unsigned long) (end - *return_location));
          return FALSE;
        }
    }
  else
    *return_location = NULL;

  return TRUE;
}

/**
 * g_subprocess_communicate_utf8_finish:
 * @subprocess: Self
 * @result: Result
 * @stdout_buf: (out) (nullable) (optional) (transfer full): Return location for stdout data
 * @stderr_buf: (out) (nullable) (optional) (transfer full): Return location for stderr data
 * @error: Error
 *
 * Complete an invocation of g_subprocess_communicate_utf8_async().
 */
gboolean
g_subprocess_communicate_utf8_finish (GSubprocess   *subprocess,
                                      GAsyncResult  *result,
                                      char         **stdout_buf,
                                      char         **stderr_buf,
                                      GError       **error)
{
  gboolean ret = FALSE;
  CommunicateState *state;
  gchar *local_stdout_buf = NULL, *local_stderr_buf = NULL;

  g_return_val_if_fail (G_IS_SUBPROCESS (subprocess), FALSE);
  g_return_val_if_fail (g_task_is_valid (result, subprocess), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  g_object_ref (result);

  state = g_task_get_task_data ((GTask*)result);
  if (!g_task_propagate_boolean ((GTask*)result, error))
    goto out;

  /* TODO - validate UTF-8 while streaming, rather than all at once.
   */
  if (!communicate_result_validate_utf8 ("stdout", &local_stdout_buf,
                                         state->stdout_buf,
                                         error))
    goto out;
  if (!communicate_result_validate_utf8 ("stderr", &local_stderr_buf,
                                         state->stderr_buf,
                                         error))
    goto out;

  ret = TRUE;
 out:
  g_object_unref (result);

  if (ret && stdout_buf != NULL)
    *stdout_buf = g_steal_pointer (&local_stdout_buf);
  if (ret && stderr_buf != NULL)
    *stderr_buf = g_steal_pointer (&local_stderr_buf);

  g_free (local_stderr_buf);
  g_free (local_stdout_buf);

  return ret;
}
