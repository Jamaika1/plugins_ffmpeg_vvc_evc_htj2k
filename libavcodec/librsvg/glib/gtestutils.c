/* GLib testing utilities
 * Copyright (C) 2007 Imendio AB
 * Authors: Tim Janik, Sven Herzberg
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
 */

#include "config.h"

#include "gtestutils.h"
#include "gfileutils.h"

#include <sys/types.h>
#ifdef G_OS_UNIX
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#ifdef HAVE_FTW_H
#include <ftw.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef G_OS_WIN32
#include <crtdbg.h>
#include <io.h>
#include <windows.h>
#endif
#include <errno.h>
#include <signal.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif /* HAVE_SYS_SELECT_H */
#include "gstdio.h"

#include "gmain.h"
#include "gpattern.h"
#include "grand.h"
#include "gstrfuncs.h"
#include "gtimer.h"
#include "gslice.h"
#include "gspawn.h"
#include "glib-private.h"
#include "gutilsprivate.h"

#define TAP_VERSION G_STRINGIFY (14)
#define TAP_SUBTEST_PREFIX "    "  /* a 4-space indented line */

/**
 * g_test_initialized:
 *
 * Returns true if [func@GLib.test_init] has been called.
 *
 * Returns: true if [func@GLib.test_init] has been called.
 *
 * Since: 2.36
 */

/**
 * g_test_quick:
 *
 * Returns true if tests are run in quick mode.
 *
 * Tests are always run in slow mode or in fast mode; there is no "medium speed".
 *
 * By default, tests are run in quick mode. In tests that use
 * [func@GLib.test_init], the options `-m quick`, `-m slow` and `-m thorough`
 * can be used to change this.
 *
 * Returns: true if in quick mode
 */

/**
 * g_test_slow:
 *
 * Returns true if tests are run in slow mode.
 *
 * Tests are always run in slow mode or in fast mode; there is no "medium speed".
 *
 * By default, tests are run in quick mode. In tests that use
 * [func@GLib.test_init], the options `-m quick`, `-m slow` and `-m thorough`
 * can be used to change this.
 *
 * Returns: true if in slow mode
 */

/**
 * g_test_thorough:
 *
 * Returns true if tests are run in thorough mode.
 *
 * Thorough mode is equivalent to slow mode.
 *
 * By default, tests are run in quick mode. In tests that use
 * [func@GLib.test_init], the options `-m quick`, `-m slow` and `-m thorough`
 * can be used to change this.
 *
 * Returns: true if in thorough mode
 */

/**
 * g_test_perf:
 *
 * Returns true if tests are run in performance mode.
 *
 * By default, tests are run in quick mode. In tests that use
 * [func@GLib.test_init], the option `-m perf` enables performance tests, while
 * `-m quick` disables them.
 *
 * Returns: true if in performance mode
 */

/**
 * g_test_undefined:
 *
 * Returns true if tests may provoke assertions and other formally-undefined
 * behaviour, to verify that appropriate warnings are given.
 *
 * This might, in some cases, be useful to turn this off (e.g. if running tests
 * under valgrind).
 *
 * In tests that use [func@GLib.test_init], the option `-m no-undefined` disables
 * those tests, while `-m undefined` explicitly enables them (normally
 * the default behaviour).
 *
 * Since GLib 2.68, if GLib was compiled with gcc or clang and
 * [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
 * is enabled, the default changes to not exercising undefined behaviour.
 *
 * Returns: true if tests may provoke programming errors
 */

/**
 * g_test_verbose:
 *
 * Returns true if tests are run in verbose mode.
 *
 * In tests that use [func@GLib.test_init], the option `--verbose` enables this,
 * while `-q` or `--quiet` disables it.
 *
 * The default is neither verbose nor quiet.
 *
 * Returns: true if in verbose mode
 */

/**
 * g_test_quiet:
 *
 * Returns true if tests are run in quiet mode.
 *
 * In tests that use [func@GLib.test_init], the option `-q` or `--quiet` enables
 * this, while `--verbose` disables it.
 *
 * The default is neither verbose nor quiet.
 *
 * Returns: true if in quiet mode
 */

/**
 * g_test_queue_unref:
 * @gobject: the object to unref
 *
 * Enqueue an object to be released with g_object_unref() during
 * the next teardown phase.
 *
 * This is equivalent to calling [func@GLib.test_queue_destroy]
 * with a destroy callback of g_object_unref().
 *
 * Since: 2.16
 */

/**
 * GTestSubprocessFlags:
 * @G_TEST_SUBPROCESS_DEFAULT: Default behaviour. Since: 2.74
 * @G_TEST_SUBPROCESS_INHERIT_STDIN: If this flag is given, the child
 *   process will inherit the parent's stdin. Otherwise, the child's
 *   stdin is redirected to `/dev/null`.
 * @G_TEST_SUBPROCESS_INHERIT_STDOUT: If this flag is given, the child
 *   process will inherit the parent's stdout. Otherwise, the child's
 *   stdout will not be visible, but it will be captured to allow
 *   later tests with [func@GLib.test_trap_assert_stdout].
 * @G_TEST_SUBPROCESS_INHERIT_STDERR: If this flag is given, the child
 *   process will inherit the parent's stderr. Otherwise, the child's
 *   stderr will not be visible, but it will be captured to allow
 *   later tests with [func@GLib.test_trap_assert_stderr].
 *
 * Flags to pass to [func@GLib.test_trap_subprocess] to control input and output.
 *
 * Note that in contrast with [func@GLib.test_trap_fork], the default
 * behavior of [func@GLib.test_trap_subprocess] is to not show stdout
 * and stderr.
 */

/**
 * g_test_trap_assert_passed:
 *
 * Assert that the last test subprocess passed.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * Since: 2.16
 */

/**
 * g_test_trap_assert_failed:
 *
 * Assert that the last test subprocess failed.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * This is sometimes used to test situations that are formally considered to
 * be undefined behaviour, like inputs that fail a [func@GLib.return_if_fail]
 * check. In these situations you should skip the entire test, including the
 * call to [func@GLib.test_trap_subprocess], unless [func@GLib.test_undefined]
 * returns true to indicate that undefined behaviour may be tested.
 *
 * Since: 2.16
 */

/**
 * g_test_trap_assert_stdout:
 * @soutpattern: a string that follows glob-style [pattern][struct@PatternSpec] rules
 *
 * Assert that the stdout output of the last test subprocess matches
 * @soutpattern.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * Since: 2.16
 */

/**
 * g_test_trap_assert_stdout_unmatched:
 * @soutpattern: a string that follows glob-style [pattern][struct@PatternSpec] rules
 *
 * Assert that the stdout output of the last test subprocess
 * does not match @soutpattern.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * Since: 2.16
 */

/**
 * g_test_trap_assert_stderr:
 * @serrpattern: a string that follows glob-style [pattern][struct@PatternSpec] rules
 *
 * Assert that the stderr output of the last test subprocess
 * matches @serrpattern.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * This is sometimes used to test situations that are formally considered
 * to be undefined behaviour, like code that hits a [func@GLib.assert] or
 * [func@GLib.error]. In these situations you should skip the entire test,
 * including the call to [func@GLib.test_trap_subprocess], unless
 * [func@GLib.test_undefined] returns true to indicate that undefined
 * behaviour may be tested.
 *
 * Since: 2.16
 */

/**
 * g_test_trap_assert_stderr_unmatched:
 * @serrpattern: a string that follows glob-style [pattern][struct@PatternSpec] rules
 *
 * Assert that the stderr output of the last test subprocess
 * does not match @serrpattern.
 *
 * See [func@GLib.test_trap_subprocess].
 *
 * Since: 2.16
 */

/**
 * g_test_rand_bit:
 *
 * Get a reproducible random bit (0 or 1).
 *
 * See [func@GLib.test_rand_int] for details on test case random numbers.
 *
 * Since: 2.16
 */

/**
 * g_assert:
 * @expr: the expression to check
 *
 * Debugging macro to terminate the application if the assertion
 * fails.
 *
 * If the assertion fails (i.e. the expression is not true),
 * an error message is logged and the application is terminated.
 *
 * The macro can be turned off in final releases of code by defining
 * `G_DISABLE_ASSERT` when compiling the application, so code must not
 * depend on any side effects from @expr. Similarly, it must not be used
 * in unit tests, otherwise the unit tests will be ineffective if compiled
 * with `G_DISABLE_ASSERT`. Use [func@GLib.assert_true] and related macros
 * in unit tests instead.
 */

/**
 * g_assert_not_reached:
 *
 * Debugging macro to terminate the application if it is ever reached.
 *
 * If it is reached, an error message is logged and the application
 * is terminated.
 *
 * The macro can be turned off in final releases of code by defining
 * `G_DISABLE_ASSERT` when compiling the application. Hence, it should
 * not be used in unit tests, where assertions should always be effective.
 */

/**
 * g_assert_true:
 * @expr: the expression to check
 *
 * Debugging macro to check that an expression is true.
 *
 * If the assertion fails (i.e. the expression is not true),
 * an error message is logged and the application is either
 * terminated or the testcase marked as failed.
 *
 * Note that unlike [func@GLib.assert], this macro is unaffected by
 * whether `G_DISABLE_ASSERT` is defined. Hence it should only be used
 * in tests and, conversely, [func@GLib.assert] should not be used
 * in tests.
 *
 * See [func@GLib.test_set_nonfatal_assertions].
 *
 * Since: 2.38
 */

/**
 * g_assert_false:
 * @expr: the expression to check
 *
 * Debugging macro to check an expression is false.
 *
 * If the assertion fails (i.e. the expression is not false),
 * an error message is logged and the application is either
 * terminated or the testcase marked as failed.
 *
 * Note that unlike [func@GLib.assert], this macro is unaffected by whether
 * `G_DISABLE_ASSERT` is defined. Hence it should only be used in tests and,
 * conversely, [func@GLib.assert] should not be used in tests.
 *
 * See [func@GLib.test_set_nonfatal_assertions].
 *
 * Since: 2.38
 */

/**
 * g_assert_null:
 * @expr: the expression to check
 *
 * Debugging macro to check an expression is `NULL`.
 *
 * If the assertion fails (i.e. the expression is not `NULL`),
 * an error message is logged and the application is either
 * terminated or the testcase marked as failed.
 *
 * Note that unlike [func@GLib.assert], this macro is unaffected by whether
 * `G_DISABLE_ASSERT` is defined. Hence it should only be used in tests and,
 * conversely, [func@GLib.assert] should not be used in tests.
 *
 * See [func@GLib.test_set_nonfatal_assertions].
 *
 * Since: 2.38
 */

/**
 * g_assert_nonnull:
 * @expr: the expression to check
 *
 * Debugging macro to check an expression is not `NULL`.
 *
 * If the assertion fails (i.e. the expression is `NULL`),
 * an error message is logged and the application is either
 * terminated or the testcase marked as failed.
 *
 * Note that unlike [func@GLib.assert], this macro is unaffected by whether
 * `G_DISABLE_ASSERT` is defined. Hence it should only be used in tests and,
 * conversely, [func@GLib.assert] should not be used in tests.
 *
 * See [func@GLib.test_set_nonfatal_assertions].
 *
 * Since: 2.40
 */

/**
 * g_assert_cmpstr:
 * @s1: (nullable): a string
 * @cmp: the comparison operator to use.
 *     One of `==`, `!=`, `<`, `>`, `<=`, `>=`
 * @s2: (nullable): another string
 *
 * Debugging macro to compare two strings.
 *
 * If the comparison fails, an error message is logged and the
 * application is either terminated or the testcase marked as failed.
 * The strings are compared using [GLib.strcmp0].
 *
 * The effect of `g_assert_cmpstr (s1, op, s2)` is the same as
 * `g_assert_true (g_strcmp0 (s1, s2) op 0)`. The advantage of this
 * macro is that it can produce a message that includes the actual
 * values of @s1 and @s2.
 *
 * ```c
 *   g_assert_cmpstr (mystring, ==, "fubar");
 * ```
 *
 * Since: 2.16
 */

/**
 * g_assert_cmpstrv:
 * @strv1: (nullable): a string array
 * @strv2: (nullable): another string array
 *
 * Debugging macro to check if two `NULL`-terminated string arrays (i.e. 2
 * `GStrv`) are equal.
 *
 * If they are not equal, an error message is logged and the application is
 * either terminated or the testcase marked as failed. If both arrays are
 * `NULL`, the check passes. If one array is `NULL` but the other is not,
 * an error message is logged.
 *
 * The effect of `g_assert_cmpstrv (strv1, strv2)` is the same as
 * `g_assert_true (g_strv_equal (strv1, strv2))` (if both arrays are not
 * `NULL`). The advantage of this macro is that it can produce a message that
 * includes how @strv1 and @strv2 are different.
 *
 * ```c
 *   const char *expected[] = { "one", "two", "three", NULL };
 *   g_assert_cmpstrv (mystrv, expected);
 * ```
 *
 * Since: 2.68
 */

/**
 * g_assert_cmpint:
 * @n1: an integer
 * @cmp: the comparison operator to use.
 *   One of `==`, `!=`, `<`, `>`, `<=`, `>=`
 * @n2: another integer
 *
 * Debugging macro to compare two integers.
 *
 * The effect of `g_assert_cmpint (n1, op, n2)` is the same as
 * `g_assert_true (n1 op n2)`. The advantage of this macro is
 * that it can produce a message that includes the actual values
 * of @n1 and @n2.
 *
 * Since: 2.16
 */

/**
 * g_assert_cmpuint:
 * @n1: an unsigned integer
 * @cmp: the comparison operator to use.
 *   One of `==`, `!=`, `<`, `>`, `<=`, `>=`
 * @n2: another unsigned integer
 *
 * Debugging macro to compare two unsigned integers.
 *
 * The effect of `g_assert_cmpuint (n1, op, n2)` is the same as
 * `g_assert_true (n1 op n2)`. The advantage of this macro is
 * that it can produce a message that includes the actual values
 * of @n1 and @n2.
 *
 * Since: 2.16
 */

/**
 * g_assert_cmphex:
 * @n1: an unsigned integer
 * @cmp: the comparison operator to use.
 *   One of `==`, `!=`, `<`, `>`, `<=`, `>=`
 * @n2: another unsigned integer
 *
 * Debugging macro to compare to unsigned integers.
 *
 * This is a variant of [func@GLib.assert_cmpuint] that displays
 * the numbers in hexadecimal notation in the message.
 *
 * Since: 2.16
 */

/**
 * g_assert_cmpfloat:
 * @n1: a floating point number
 * @cmp: the comparison operator to use.
 *   One of `==`, `!=`, `<`, `>`, `<=`, `>=`
 * @n2: another floating point number
 *
 * Debugging macro to compare two floating point numbers.
 *
 * The effect of `g_assert_cmpfloat (n1, op, n2)` is the same as
 * `g_assert_true (n1 op n2)`. The advantage of this macro is
 * that it can produce a message that includes the actual values
 * of @n1 and @n2.
 *
 * Since: 2.16
 */

/**
 * g_assert_cmpfloat_with_epsilon:
 * @n1: a floating point number
 * @n2: another floating point number
 * @epsilon: a numeric value that expresses the expected tolerance
 *   between @n1 and @n2
 *
 * Debugging macro to compare two floating point numbers within an epsilon.
 *
 * The effect of `g_assert_cmpfloat_with_epsilon (n1, n2, epsilon)` is
 * the same as `g_assert_true (abs (n1 - n2) < epsilon)`. The advantage
 * of this macro is that it can produce a message that includes the
 * actual values of @n1 and @n2.
 *
 * Since: 2.58
 */

/**
 * g_assert_no_errno:
 * @expr: the expression to check
 *
 * Debugging macro to check that an expression has a non-negative return value,
 * as used by traditional POSIX functions (such as `rmdir()`) to indicate
 * success.
 *
 * If the assertion fails (i.e. the @expr returns a negative value), an error
 * message is logged and the testcase is marked as failed. The error message
 * will contain the value of `errno` and its human-readable message from
 * [func@GLib.strerror].
 *
 * This macro will clear the value of `errno` before executing @expr.
 *
 * Since: 2.66
 */

/**
 * g_assert_cmpmem:
 * @m1: (nullable): pointer to a buffer
 * @l1: length of @m1 in bytes
 * @m2: (nullable): pointer to another buffer
 * @l2: length of @m2 in bytes
 *
 * Debugging macro to compare memory regions.
 *
 * If the comparison fails, an error message is logged and the
 * application is either terminated or the testcase marked as failed.
 *
 * The effect of `g_assert_cmpmem (m1, l1, m2, l2)` is the same as
 * `g_assert_true (l1 == l2 && memcmp (m1, m2, l1) == 0)`. The advantage
 * of this macro is that it can produce a message that includes the actual
 * values of @l1 and @l2.
 *
 * @m1 may be `NULL` if (and only if) @l1 is zero; similarly for @m2 and @l2.
 *
 * ```c
 *   g_assert_cmpmem (buf->data, buf->len, expected, sizeof (expected));
 * ```
 *
 * Since: 2.46
 */

/**
 * g_assert_cmpvariant:
 * @v1: pointer to a `GVariant`
 * @v2: pointer to another `GVariant`
 *
 * Debugging macro to compare two [struct@GLib.Variant] values.
 *
 * If the comparison fails, an error message is logged and the
 * application is either terminated or the testcase marked as failed.
 * The variants are compared using [method@GLib.Variant.equal].
 *
 * The effect of `g_assert_cmpvariant (v1, v2)` is the same as
 * `g_assert_true (g_variant_equal (v1, v2))`. The advantage of
 * this macro is that it can produce a message that includes the
 * actual values of @v1 and @v2.
 *
 * Since: 2.60
 */

/**
 * g_assert_no_error:
 * @err: (nullable): a `GError`
 *
 * Debugging macro to check that a [struct@GLib.Error] is not set.
 *
 * The effect of `g_assert_no_error (err)` is the same as
 * `g_assert_true (err == NULL)`. The advantage of this macro
 * is that it can produce a message that includes the error
 * message and code.
 *
 * Since: 2.20
 */

/**
 * g_assert_error:
 * @err: (nullable): a `GError`
 * @dom: the expected error domain (a `GQuark`)
 * @c: the expected error code
 *
 * Debugging macro to check that a method has returned
 * the correct [struct@GLib.Error].
 *
 * The effect of `g_assert_error (err, dom, c)` is the same as
 * `g_assert_true (err != NULL && err->domain == dom && err->code == c)`.
 * The advantage of this macro is that it can produce a message that
 * includes the incorrect error message and code.
 *
 * This can only be used to test for a specific error. If you want to
 * test that @err is set, but don't care what it's set to, just use
 * `g_assert_nonnull (err)`.
 *
 * Since: 2.20
 */

/**
 * GTestCase:
 *
 * An opaque structure representing a test case.
 */

/**
 * GTestSuite:
 *
 * An opaque structure representing a test suite.
 */


/* Global variable for storing assertion messages; this is the counterpart to
 * glibc's (private) __abort_msg variable, and allows developers and crash
 * analysis systems like Apport and ABRT to fish out assertion messages from
 * core dumps, instead of having to catch them on screen output.
 */
GLIB_VAR char *__glib_assert_msg;
char *__glib_assert_msg = NULL;

/* --- constants --- */
#define G_TEST_STATUS_TIMED_OUT 1024

/* --- structures --- */
struct GTestCase
{
  gchar  *name;
  guint   fixture_size;
  void   (*fixture_setup)    (void*, gconstpointer);
  void   (*fixture_test)     (void*, gconstpointer);
  void   (*fixture_teardown) (void*, gconstpointer);
  gpointer test_data;
};
struct GTestSuite
{
  gchar  *name;
  GSList *suites;
  GSList *cases;
};
typedef struct DestroyEntry DestroyEntry;
struct DestroyEntry
{
  DestroyEntry *next;
  GDestroyNotify destroy_func;
  gpointer       destroy_data;
};

/* --- prototypes --- */
static void     test_cleanup                    (void);
static void     test_run_seed                   (const gchar *rseed);
static void     test_trap_clear                 (void);
static guint8*  g_test_log_dump                 (GTestLogMsg *msg,
                                                 guint       *len);
static void     gtest_default_log_handler       (const gchar    *log_domain,
                                                 GLogLevelFlags  log_level,
                                                 const gchar    *message,
                                                 gpointer        unused_data);
static void     g_test_tap_print                (unsigned    subtest_level,
                                                 gboolean    commented,
                                                 const char *format,
                                                 ...) G_GNUC_PRINTF (3, 4);

static const char * const g_test_result_names[] = {
  "OK",
  "SKIP",
  "FAIL",
  "TODO"
};

/* --- variables --- */
static int         test_log_fd = -1;
static gboolean    test_mode_fatal = TRUE;
static gboolean    g_test_run_once = TRUE;
static gboolean    test_isolate_dirs = FALSE;
static gchar      *test_isolate_dirs_tmpdir = NULL;
static const gchar *test_tmpdir = NULL;
static gboolean    test_run_list = FALSE;
static gchar      *test_run_seedstr = NULL;
G_LOCK_DEFINE_STATIC (test_run_rand);
static GRand      *test_run_rand = NULL;
static gchar      *test_run_name = "";
static gchar      *test_run_name_path = "";
static GSList    **test_filename_free_list;
static guint       test_run_forks = 0;
static guint       test_run_count = 0;
static guint       test_count = 0;
static guint       test_skipped_count = 0;
static GTestResult test_run_success = G_TEST_RUN_FAILURE;
static gchar      *test_run_msg = NULL;
static guint       test_startup_skip_count = 0;
static GTimer     *test_user_timer = NULL;
static double      test_user_stamp = 0;
static GSList     *test_paths = NULL;
static gboolean    test_prefix = FALSE;
static gboolean    test_prefix_extended = FALSE;
static GSList     *test_paths_skipped = NULL;
static gboolean    test_prefix_skipped = FALSE;
static gboolean    test_prefix_extended_skipped = FALSE;
static GTestSuite *test_suite_root = NULL;
static int         test_trap_last_status = 0;  /* unmodified platform-specific status */
static GPid        test_trap_last_pid = 0;
static char       *test_trap_last_subprocess = NULL;
static char       *test_trap_last_stdout = NULL;
static char       *test_trap_last_stderr = NULL;
static char       *test_uri_base = NULL;
static gboolean    test_debug_log = FALSE;
static gboolean    test_tap_log = TRUE;  /* default to TAP as of GLib 2.62; see #1619; the non-TAP output mode is deprecated */
static gboolean    test_nonfatal_assertions = FALSE;
static DestroyEntry *test_destroy_queue = NULL;
static const char *test_argv0 = NULL;           /* (nullable), points into global argv */
static char       *test_argv0_dirname = NULL;   /* owned by GLib */
static const char *test_disted_files_dir;       /* points into test_argv0_dirname or an environment variable */
static const char *test_built_files_dir;        /* points into test_argv0_dirname or an environment variable */
static char       *test_initial_cwd = NULL;
static gboolean    test_in_forked_child = FALSE;
static gboolean    test_in_subprocess = FALSE;
static gboolean    test_is_subtest = FALSE;
static GTestConfig mutable_test_config_vars = {
  FALSE,        /* test_initialized */
  TRUE,         /* test_quick */
  FALSE,        /* test_perf */
  FALSE,        /* test_verbose */
  FALSE,        /* test_quiet */
  TRUE,         /* test_undefined */
};
const GTestConfig * const g_test_config_vars = &mutable_test_config_vars;
static gboolean  no_g_set_prgname = FALSE;
static GPrintFunc g_default_print_func = NULL;

enum
{
  G_TEST_CASE_LARGS_RESULT = 0,  /* a GTestResult */
  G_TEST_CASE_LARGS_RUN_FORKS = 1,  /* a gint */
  G_TEST_CASE_LARGS_EXECUTION_TIME = 2,  /* a gdouble */

  G_TEST_CASE_LARGS_MAX
};

/* --- functions --- */
static inline gboolean
is_subtest (void)
{
  return test_is_subtest || test_in_forked_child || test_in_subprocess;
}

static void
g_test_print_handler_full (const gchar *string,
                           gboolean     use_tap_format,
                           gboolean     is_tap_comment,
                           unsigned     subtest_level)
{
  g_assert (string != NULL);

  if (G_LIKELY (use_tap_format) && strchr (string, '\n') != NULL)
    {
      static gboolean last_had_final_newline = TRUE;
      GString *output = g_string_new_len (NULL, strlen (string) + 2);
      const char *line = string;

      do
        {
          const char *next = strchr (line, '\n');

          if (last_had_final_newline && (next || *line != '\0'))
            {
              for (unsigned l = 0; l < subtest_level; ++l)
                g_string_append (output, TAP_SUBTEST_PREFIX);

              if G_LIKELY (is_tap_comment)
                g_string_append (output, "# ");
            }

          if (next)
            {
              next += 1; /* Include the newline */
              g_string_append_len (output, line, next - line);
            }
          else
            {
              g_string_append (output, line);
              last_had_final_newline = (*line == '\0');
            }

          line = next;
        }
      while (line != NULL);

      g_default_print_func (output->str);
      g_string_free (g_steal_pointer (&output), TRUE);
    }
  else
    {
      g_default_print_func (string);
    }
}

static void
g_test_print_handler (const gchar *string)
{
  g_test_print_handler_full (string, test_tap_log, TRUE, is_subtest () ? 1 : 0);
}

static void
g_test_tap_print (unsigned    subtest_level,
                  gboolean    commented,
                  const char *format,
                  ...)
{
  va_list args;
  char *string;

  va_start (args, format);
  string = g_strdup_vprintf (format, args);
  va_end (args);

  g_test_print_handler_full (string, TRUE, commented, subtest_level);
  g_free (string);
}

const char*
g_test_log_type_name (GTestLogType log_type)
{
  switch (log_type)
    {
    case G_TEST_LOG_NONE:               return "none";
    case G_TEST_LOG_ERROR:              return "error";
    case G_TEST_LOG_START_BINARY:       return "binary";
    case G_TEST_LOG_LIST_CASE:          return "list";
    case G_TEST_LOG_SKIP_CASE:          return "skip";
    case G_TEST_LOG_START_CASE:         return "start";
    case G_TEST_LOG_STOP_CASE:          return "stop";
    case G_TEST_LOG_MIN_RESULT:         return "minperf";
    case G_TEST_LOG_MAX_RESULT:         return "maxperf";
    case G_TEST_LOG_MESSAGE:            return "message";
    case G_TEST_LOG_START_SUITE:        return "start suite";
    case G_TEST_LOG_STOP_SUITE:         return "stop suite";
    }
  return "???";
}

static void
g_test_log_send (guint         n_bytes,
                 const guint8 *buffer)
{
  if (test_log_fd >= 0)
    {
      int r;
      do
        r = write (test_log_fd, buffer, n_bytes);
      while (r < 0 && errno == EINTR);
    }
  if (test_debug_log)
    {
      GTestLogBuffer *lbuffer = g_test_log_buffer_new ();
      GTestLogMsg *msg;
      GString *output;
      guint ui;
      g_test_log_buffer_push (lbuffer, n_bytes, buffer);
      msg = g_test_log_buffer_pop (lbuffer);
      g_warn_if_fail (msg != NULL);
      g_warn_if_fail (lbuffer->data->len == 0);
      g_test_log_buffer_free (lbuffer);
      /* print message */
      output = g_string_new (NULL);
      g_string_printf (output, "{*LOG(%s)", g_test_log_type_name (msg->log_type));
      for (ui = 0; ui < msg->n_strings; ui++)
        g_string_append_printf (output, ":{%s}", msg->strings[ui]);
      if (msg->n_nums)
        {
          g_string_append (output, ":(");
          for (ui = 0; ui < msg->n_nums; ui++)
            {
              if ((long double) (long) msg->nums[ui] == msg->nums[ui])
                g_string_append_printf (output, "%s%ld", ui ? ";" : "", (long) msg->nums[ui]);
              else
                g_string_append_printf (output, "%s%.16g", ui ? ";" : "", (double) msg->nums[ui]);
            }
          g_string_append_c (output, ')');
        }
      g_string_append (output, ":LOG*}");
      g_printerr ("%s\n", output->str);
      g_string_free (output, TRUE);
      g_test_log_msg_free (msg);
    }
}

static void
g_test_log (GTestLogType lbit,
            const gchar *string1,
            const gchar *string2,
            guint        n_args,
            long double *largs)
{
  GTestResult result;
  gboolean fail;
  GTestLogMsg msg;
  gchar *astrings[3] = { NULL, NULL, NULL };
  guint8 *dbuffer;
  guint32 dbufferlen;
  unsigned subtest_level;
  gdouble timing;

  if (g_once_init_enter_pointer (&g_default_print_func))
    {
      g_once_init_leave_pointer (&g_default_print_func,
                                 g_set_print_handler (g_test_print_handler));
      g_assert_nonnull (g_default_print_func);
    }

  subtest_level = is_subtest () ? 1 : 0;

  switch (lbit)
    {
    case G_TEST_LOG_START_BINARY:
      if (test_tap_log)
        {
          if (!is_subtest ())
            {
              g_test_tap_print (0, FALSE, "TAP version " TAP_VERSION "\n");
            }
          else
            {
              g_test_tap_print (subtest_level > 0 ? subtest_level - 1 : 0, TRUE,
                                "Subtest: %s\n", test_argv0);
            }

          g_print ("random seed: %s\n", string2);
        }
      else if (g_test_verbose ())
        {
          g_print ("GTest: random seed: %s\n", string2);
        }
      break;
    case G_TEST_LOG_START_SUITE:
      if (test_tap_log)
        {
          /* We only print the TAP "plan" (1..n) ahead of time if we did
           * not use the -p option to select specific tests to be run. */
          if (string1[0] != 0)
            g_print ("Start of %s tests\n", string1);
          else if (test_paths == NULL)
            g_test_tap_print (subtest_level, FALSE, "1..%d\n", test_count);
        }
      break;
    case G_TEST_LOG_STOP_SUITE:
      if (test_tap_log)
        {
          /* If we didn't print the TAP "plan" at the beginning because
           * we were using -p, we need to print how many tests we ran at
           * the end instead. */
          if (string1[0] != 0)
            g_print ("End of %s tests\n", string1);
          else if (test_paths != NULL)
            g_test_tap_print (subtest_level, FALSE, "1..%d\n", test_run_count);
        }
      break;
    case G_TEST_LOG_STOP_CASE:
      result = largs[G_TEST_CASE_LARGS_RESULT];
      timing = (gdouble) largs[G_TEST_CASE_LARGS_EXECUTION_TIME];
      fail = result == G_TEST_RUN_FAILURE;
      if (test_tap_log)
        {
          GString *tap_output;

          /* The TAP representation for an expected failure starts with
           * "not ok", even though it does not actually count as failing
           * due to the use of the TODO directive. "ok # TODO" would mean
           * a test that was expected to fail unexpectedly succeeded,
           * for which GTestResult does not currently have a
           * representation. */
          if (fail || result == G_TEST_RUN_INCOMPLETE)
            tap_output = g_string_new ("not ok");
          else
            tap_output = g_string_new ("ok");

          if (is_subtest ())
            g_string_prepend (tap_output, TAP_SUBTEST_PREFIX);

          g_string_append_printf (tap_output, " %d %s", test_run_count, string1);
          if (result == G_TEST_RUN_INCOMPLETE)
            g_string_append_printf (tap_output, " # TODO %s", string2 ? string2 : "");
          else if (result == G_TEST_RUN_SKIPPED)
            g_string_append_printf (tap_output, " # SKIP %s", string2 ? string2 : "");
          else if (result == G_TEST_RUN_FAILURE && string2 != NULL)
            g_string_append_printf (tap_output, " - %s", string2);

          g_string_append_c (tap_output, '\n');
          g_default_print_func (tap_output->str);
          g_string_free (g_steal_pointer (&tap_output), TRUE);

          /* Print msg for any slow tests, where 'slow' means >= 0.5 secs */
          if (timing > 0.5)
            {
              tap_output = g_string_new ("# ");
              g_string_append_printf (tap_output, "slow test %s executed in %0.2lf secs\n",
                                      string1, timing);
              g_default_print_func (tap_output->str);
              g_string_free (g_steal_pointer (&tap_output), TRUE);
            }
        }
      else if (g_test_verbose ())
        g_print ("GTest: result: %s\n", g_test_result_names[result]);
      else if (!g_test_quiet () && !test_in_subprocess)
        g_print ("%s\n", g_test_result_names[result]);
      if (fail && test_mode_fatal)
        {
          if (test_tap_log)
            g_test_tap_print (0, FALSE, "Bail out!\n");
          g_abort ();
        }
      if (result == G_TEST_RUN_SKIPPED || result == G_TEST_RUN_INCOMPLETE)
        test_skipped_count++;
      break;
    case G_TEST_LOG_SKIP_CASE:
      if (test_tap_log)
        {
          g_test_tap_print (subtest_level, FALSE, "ok %d %s # SKIP\n",
                            test_run_count, string1);
        }
      break;
    case G_TEST_LOG_MIN_RESULT:
      if (test_tap_log)
        g_print ("min perf: %s\n", string1);
      else if (g_test_verbose ())
        g_print ("(MINPERF:%s)\n", string1);
      break;
    case G_TEST_LOG_MAX_RESULT:
      if (test_tap_log)
        g_print ("max perf: %s\n", string1);
      else if (g_test_verbose ())
        g_print ("(MAXPERF:%s)\n", string1);
      break;
    case G_TEST_LOG_MESSAGE:
      if (test_tap_log)
        g_print ("%s\n", string1);
      else if (g_test_verbose ())
        g_print ("(MSG: %s)\n", string1);
      break;
    case G_TEST_LOG_ERROR:
      if (test_tap_log)
        {
          char *message = g_strdup (string1);

          if (message)
            {
              char *line = message;

              while ((line = strchr (line, '\n')))
                  *(line++) = ' ';

              message = g_strstrip (message);
            }

          if (test_run_name && *test_run_name != '\0')
            {
              if (message && *message != '\0')
                g_test_tap_print (subtest_level, FALSE, "not ok %s - %s\n",
                                  test_run_name, message);
              else
                g_test_tap_print (subtest_level, FALSE, "not ok %s\n",
                                  test_run_name);

              g_clear_pointer (&message, g_free);
            }

          if (message && *message != '\0')
            g_test_tap_print (subtest_level, FALSE, "Bail out! %s\n", message);
          else
            g_test_tap_print (subtest_level, FALSE, "Bail out!\n");

          g_free (message);
        }
      else if (g_test_verbose ())
        {
          g_print ("(ERROR: %s)\n", string1);
        }
      break;
    default: ;
    }

  msg.log_type = lbit;
  msg.n_strings = (string1 != NULL) + (string1 && string2);
  msg.strings = astrings;
  astrings[0] = (gchar*) string1;
  astrings[1] = astrings[0] ? (gchar*) string2 : NULL;
  msg.n_nums = n_args;
  msg.nums = largs;
  dbuffer = g_test_log_dump (&msg, &dbufferlen);
  g_test_log_send (dbufferlen, dbuffer);
  g_free (dbuffer);

  switch (lbit)
    {
    case G_TEST_LOG_START_CASE:
      if (test_tap_log)
        ;
      else if (g_test_verbose ())
        g_print ("GTest: run: %s\n", string1);
      else if (!g_test_quiet ())
        g_print ("%s: ", string1);
      break;
    default: ;
    }
}

/**
 * g_test_disable_crash_reporting:
 *
 * Attempts to disable system crash reporting infrastructure.
 *
 * This function should be called before exercising code paths that are
 * expected or intended to crash, to avoid wasting resources in system-wide
 * crash collection infrastructure such as systemd-coredump or abrt.
 *
 * Since: 2.78
 */
void
g_test_disable_crash_reporting (void)
{
#ifdef HAVE_SYS_RESOURCE_H
  struct rlimit limit = { 0, 0 };

  (void) setrlimit (RLIMIT_CORE, &limit);
#endif

#if defined(HAVE_PRCTL) && defined(PR_SET_DUMPABLE)
  /* On Linux, RLIMIT_CORE = 0 is ignored if core dumps are
   * configured to be written to a pipe, but PR_SET_DUMPABLE is not. */
  (void) prctl (PR_SET_DUMPABLE, 0, 0, 0, 0);
#endif
}

/* We intentionally parse the command line without GOptionContext
 * because otherwise you would never be able to test it.
 */
static void
parse_args (gint    *argc_p,
            gchar ***argv_p)
{
  guint argc = *argc_p;
  gchar **argv = *argv_p;
  guint i, e;

  test_argv0 = argv[0];  /* will be NULL iff argc == 0 */
  test_initial_cwd = g_get_current_dir ();

  /* parse known args */
  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "--g-fatal-warnings") == 0)
        {
          GLogLevelFlags fatal_mask = (GLogLevelFlags) g_log_set_always_fatal ((GLogLevelFlags) G_LOG_FATAL_MASK);
          fatal_mask = (GLogLevelFlags) (fatal_mask | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
          g_log_set_always_fatal (fatal_mask);
          argv[i] = NULL;
        }
      else if (strcmp (argv[i], "--keep-going") == 0 ||
               strcmp (argv[i], "-k") == 0)
        {
          test_mode_fatal = FALSE;
          argv[i] = NULL;
        }
      else if (strcmp (argv[i], "--debug-log") == 0)
        {
          test_debug_log = TRUE;
          argv[i] = NULL;
        }
      else if (strcmp (argv[i], "--tap") == 0)
        {
          test_tap_log = TRUE;
          argv[i] = NULL;
        }
      else if (strcmp ("--GTestLogFD", argv[i]) == 0 || strncmp ("--GTestLogFD=", argv[i], 13) == 0)
        {
          gchar *equal = argv[i] + 12;
          if (*equal == '=')
            test_log_fd = g_ascii_strtoull (equal + 1, NULL, 0);
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_log_fd = g_ascii_strtoull (argv[i], NULL, 0);
            }
          argv[i] = NULL;

          /* Force non-TAP output when using gtester */
          test_tap_log = FALSE;
        }
      else if (strcmp ("--GTestSkipCount", argv[i]) == 0 || strncmp ("--GTestSkipCount=", argv[i], 17) == 0)
        {
          gchar *equal = argv[i] + 16;
          if (*equal == '=')
            test_startup_skip_count = g_ascii_strtoull (equal + 1, NULL, 0);
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_startup_skip_count = g_ascii_strtoull (argv[i], NULL, 0);
            }
          argv[i] = NULL;
        }
      else if (strcmp ("--GTestSubprocess", argv[i]) == 0)
        {
          test_in_subprocess = TRUE;
          /* We typically expect these child processes to crash, and some
           * tests spawn a *lot* of them.  Avoid spamming system crash
           * collection programs such as systemd-coredump and abrt.
           */
          g_test_disable_crash_reporting ();

          argv[i] = NULL;

          /* Force non-TAP output when spawning a subprocess, since people often
           * test the stdout/stderr of the subprocess strictly */
          test_tap_log = FALSE;
        }
      else if (strcmp ("-p", argv[i]) == 0 || strncmp ("-p=", argv[i], 3) == 0)
        {
          gchar *equal = argv[i] + 2;
          if (*equal == '=')
            test_paths = g_slist_prepend (test_paths, equal + 1);
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_paths = g_slist_prepend (test_paths, argv[i]);
            }
          argv[i] = NULL;
          if (test_prefix_extended) {
            printf ("do not mix [-r | --run-prefix] with '-p'\n");
            exit (1);
          }
          test_prefix = TRUE;
        }
      else if (strcmp ("-r", argv[i]) == 0 ||
               strncmp ("-r=", argv[i], 3) == 0 ||
               strcmp ("--run-prefix", argv[i]) == 0 ||
               strncmp ("--run-prefix=", argv[i], 13) == 0)
        {
            gchar *equal = argv[i] + 2;
            if (*equal == '=')
              test_paths = g_slist_prepend (test_paths, equal + 1);
            else if (i + 1 < argc)
              {
                argv[i++] = NULL;
                test_paths = g_slist_prepend (test_paths, argv[i]);
              }
            argv[i] = NULL;
            if (test_prefix) {
              printf ("do not mix [-r | --run-prefix] with '-p'\n");
              exit (1);
            }
            test_prefix_extended = TRUE;
        }
      else if (strcmp ("-s", argv[i]) == 0 || strncmp ("-s=", argv[i], 3) == 0)
        {
          gchar *equal = argv[i] + 2;
          if (*equal == '=')
            test_paths_skipped = g_slist_prepend (test_paths_skipped, equal + 1);
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_paths_skipped = g_slist_prepend (test_paths_skipped, argv[i]);
            }
          argv[i] = NULL;
          if (test_prefix_extended_skipped) {
            printf ("do not mix [-x | --skip-prefix] with '-s'\n");
            exit (1);
          }
          test_prefix_skipped = TRUE;
        }
      else if (strcmp ("-x", argv[i]) == 0 ||
               strncmp ("-x=", argv[i], 3) == 0 ||
               strcmp ("--skip-prefix", argv[i]) == 0 ||
               strncmp ("--skip-prefix=", argv[i], 14) == 0)
        {
          gchar *equal = argv[i] + 2;
          if (*equal == '=')
            test_paths_skipped = g_slist_prepend (test_paths_skipped, equal + 1);
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_paths_skipped = g_slist_prepend (test_paths_skipped, argv[i]);
            }
          argv[i] = NULL;
          if (test_prefix_skipped) {
            printf ("do not mix [-x | --skip-prefix] with '-s'\n");
            exit (1);
          }
          test_prefix_extended_skipped = TRUE;
        }
      else if (strcmp ("-m", argv[i]) == 0 || strncmp ("-m=", argv[i], 3) == 0)
        {
          gchar *equal = argv[i] + 2;
          const gchar *mode = "";
          if (*equal == '=')
            mode = equal + 1;
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              mode = argv[i];
            }
          if (strcmp (mode, "perf") == 0)
            mutable_test_config_vars.test_perf = TRUE;
          else if (strcmp (mode, "slow") == 0)
            mutable_test_config_vars.test_quick = FALSE;
          else if (strcmp (mode, "thorough") == 0)
            mutable_test_config_vars.test_quick = FALSE;
          else if (strcmp (mode, "quick") == 0)
            {
              mutable_test_config_vars.test_quick = TRUE;
              mutable_test_config_vars.test_perf = FALSE;
            }
          else if (strcmp (mode, "undefined") == 0)
            mutable_test_config_vars.test_undefined = TRUE;
          else if (strcmp (mode, "no-undefined") == 0)
            mutable_test_config_vars.test_undefined = FALSE;
          else
            g_error ("unknown test mode: -m %s", mode);
          argv[i] = NULL;
        }
      else if (strcmp ("-q", argv[i]) == 0 || strcmp ("--quiet", argv[i]) == 0)
        {
          mutable_test_config_vars.test_quiet = TRUE;
          mutable_test_config_vars.test_verbose = FALSE;
          argv[i] = NULL;
        }
      else if (strcmp ("--verbose", argv[i]) == 0)
        {
          mutable_test_config_vars.test_quiet = FALSE;
          mutable_test_config_vars.test_verbose = TRUE;
          argv[i] = NULL;
        }
      else if (strcmp ("-l", argv[i]) == 0)
        {
          test_run_list = TRUE;
          argv[i] = NULL;
        }
      else if (strcmp ("--seed", argv[i]) == 0 || strncmp ("--seed=", argv[i], 7) == 0)
        {
          gchar *equal = argv[i] + 6;
          if (*equal == '=')
            test_run_seedstr = equal + 1;
          else if (i + 1 < argc)
            {
              argv[i++] = NULL;
              test_run_seedstr = argv[i];
            }
          argv[i] = NULL;
        }
      else if (strcmp ("-?", argv[i]) == 0 ||
               strcmp ("-h", argv[i]) == 0 ||
               strcmp ("--help", argv[i]) == 0)
        {
          printf ("Usage:\n"
                  "  %s [OPTION...]\n\n"
                  "Help Options:\n"
                  "  -h, --help                     Show help options\n\n"
                  "Test Options:\n"
                  "  --g-fatal-warnings             Make all warnings fatal\n"
                  "  -l                             List test cases available in a test executable\n"
                  "  -m {perf|slow|thorough|quick}  Execute tests according to mode\n"
                  "  -m {undefined|no-undefined}    Execute tests according to mode\n"
                  "  -p TESTPATH                    Only start test cases matching TESTPATH\n"
                  "  -s TESTPATH                    Skip all tests matching TESTPATH\n"
                  "  [-r | --run-prefix] PREFIX     Only start test cases (or suites) matching PREFIX (incompatible with -p).\n"
                  "                                 Unlike the -p option (which only goes one level deep), this option would \n"
                  "                                 run all tests path that have PREFIX at the beginning of their name.\n"
                  "                                 Note that the prefix used should be a valid test path (and not a simple prefix).\n"
                  "  [-x | --skip-prefix] PREFIX    Skip all tests matching PREFIX (incompatible with -s)\n"
                  "                                 Unlike the -s option (which only skips the exact TESTPATH), this option will \n"
                  "                                 skip all the tests that begins with PREFIX).\n"
                  "  --seed=SEEDSTRING              Start tests with random seed SEEDSTRING\n"
                  "  --debug-log                    debug test logging output\n"
                  "  -q, --quiet                    Run tests quietly\n"
                  "  --verbose                      Run tests verbosely\n",
                  argv[0]);
          exit (0);
        }
    }

  /* We've been prepending to test_paths, but its order matters, so
   * permute it */
  test_paths = g_slist_reverse (test_paths);

  /* collapse argv */
  e = 0;
  for (i = 0; i < argc; i++)
    if (argv[i])
      {
        argv[e++] = argv[i];
        if (i >= e)
          argv[i] = NULL;
      }
  *argc_p = e;
}

#ifdef HAVE_FTW_H
static int
rm_rf_nftw_visitor (const char *fpath,
                    const struct stat *sb,
                    int typeflag,
                    struct FTW *ftwbuf)
{
  switch (typeflag)
    {
    case FTW_DP:
    case FTW_D:
    case FTW_DNR:
      if (g_rmdir (fpath) != 0)
        {
          int errsv = errno;
          g_printerr ("Unable to clean up temporary directory %s: %s\n",
                      fpath,
                      g_strerror (errsv));
        }
      break;

    default:
      if (g_remove (fpath) != 0)
        {
          int errsv = errno;
          g_printerr ("Unable to clean up temporary file %s: %s\n",
                      fpath,
                      g_strerror (errsv));
        }
      break;
    }

  return 0;
}

static void
rm_rf (const gchar *path)
{
  /* nopenfd specifies the maximum number of directories that [n]ftw() will
   * hold open simultaneously. Rather than attempt to determine how many file
   * descriptors are available, we assume that 5 are available when tearing
   * down a test case; if that assumption is invalid, the only harm is leaving
   * a temporary directory on disk.
   */
  const int nopenfd = 5;
  int ret = nftw (path, rm_rf_nftw_visitor, nopenfd, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
  if (ret != 0)
    {
      int errsv = errno;
      g_printerr ("Unable to clean up temporary directory %s: %s\n",
                  path,
                  g_strerror (errsv));
    }
}
#else
/* A fairly naive `rm -rf` implementation to clean up after unit tests. */
static void
rm_rf (const gchar *path)
{
  GDir *dir = NULL;
  const gchar *entry;

  dir = g_dir_open (path, 0, NULL);
  if (dir == NULL)
    {
      /* Assume it’s a file. Ignore failure. */
      (void) g_remove (path);
      return;
    }

  while ((entry = g_dir_read_name (dir)) != NULL)
    {
      gchar *sub_path = g_build_filename (path, entry, NULL);
      rm_rf (sub_path);
      g_free (sub_path);
    }

  g_dir_close (dir);

  g_rmdir (path);
}
#endif

/* Implement the %G_TEST_OPTION_ISOLATE_DIRS option, iff it’s enabled. Create
 * a temporary directory for this unit test (disambiguated using @test_run_name)
 * and use g_set_user_dirs() to point various XDG directories into it, without
 * having to call setenv() in a process which potentially has threads running.
 *
 * Note that this is called for each unit test, and hence won’t have taken
 * effect before g_test_run() is called in the unit test’s main(). Hence
 * references to XDG variables in main() will not be using the temporary
 * directory. */
static gboolean
test_do_isolate_dirs (GError **error)
{
  gchar *subdir = NULL;
  gchar *home_dir = NULL, *cache_dir = NULL, *config_dir = NULL;
  gchar *state_dir = NULL, *data_dir = NULL, *runtime_dir = NULL;
  gchar *config_dirs[3];
  gchar *data_dirs[3];

  if (!test_isolate_dirs)
    return TRUE;

  /* The @test_run_name includes the test suites, so may be several directories
   * deep. Add a `.dirs` directory to contain all the paths we create, and
   * guarantee none of them clash with test paths below the current one — test
   * paths may not contain components starting with `.`. */
  subdir = g_build_filename (test_tmpdir, test_run_name_path, ".dirs", NULL);

  /* We have to create the runtime directory (because it must be bound to
   * the session lifetime, which we consider to be the lifetime of the unit
   * test for testing purposes — see
   * https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html.
   * We don’t need to create the other directories — the specification
   * requires that client code create them if they don’t exist. Not creating
   * them automatically is a good test of clients’ adherence to the spec
   * and error handling of missing directories. */
  runtime_dir = g_build_filename (subdir, "runtime", NULL);
  if (g_mkdir_with_parents (runtime_dir, 0700) != 0)
    {
      gint saved_errno = errno;
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (saved_errno),
                   "Failed to create XDG_RUNTIME_DIR ‘%s’: %s",
                  runtime_dir, g_strerror (saved_errno));
      g_free (runtime_dir);
      g_free (subdir);
      return FALSE;
    }

  home_dir = g_build_filename (subdir, "home", NULL);
  cache_dir = g_build_filename (subdir, "cache", NULL);
  config_dir = g_build_filename (subdir, "config", NULL);
  data_dir = g_build_filename (subdir, "data", NULL);
  state_dir = g_build_filename (subdir, "state", NULL);

  config_dirs[0] = g_build_filename (subdir, "system-config1", NULL);
  config_dirs[1] = g_build_filename (subdir, "system-config2", NULL);
  config_dirs[2] = NULL;

  data_dirs[0] = g_build_filename (subdir, "system-data1", NULL);
  data_dirs[1] = g_build_filename (subdir, "system-data2", NULL);
  data_dirs[2] = NULL;

  /* Remember to update the documentation for %G_TEST_OPTION_ISOLATE_DIRS if
   * this list changes. */
  g_set_user_dirs ("HOME", home_dir,
                   "XDG_CACHE_HOME", cache_dir,
                   "XDG_CONFIG_DIRS", config_dirs,
                   "XDG_CONFIG_HOME", config_dir,
                   "XDG_DATA_DIRS", data_dirs,
                   "XDG_DATA_HOME", data_dir,
                   "XDG_STATE_HOME", state_dir,
                   "XDG_RUNTIME_DIR", runtime_dir,
                   NULL);

  g_free (runtime_dir);
  g_free (state_dir);
  g_free (data_dir);
  g_free (config_dir);
  g_free (cache_dir);
  g_free (home_dir);
  g_free (data_dirs[1]);
  g_free (data_dirs[0]);
  g_free (config_dirs[1]);
  g_free (config_dirs[0]);
  g_free (subdir);

  return TRUE;
}

/* Clean up after test_do_isolate_dirs(). */
static void
test_rm_isolate_dirs (void)
{
  gchar *subdir = NULL;

  if (!test_isolate_dirs)
    return;

  subdir = g_build_filename (test_tmpdir, test_run_name_path, NULL);
  rm_rf (subdir);
  g_free (subdir);
}

/**
 * g_test_init:
 * @argc: address of the @argc parameter of `main()`
 * @argv: address of the @argv parameter of `main()`
 * @...: `NULL`-terminated list of special options
 *
 * Initializes the GLib testing framework.
 *
 * This includes seeding the test random number generator,
 * setting the program name, and parsing test-related commandline args.
 *
 * This should be called before calling any other `g_test_*()` functions.
 *
 * The following arguments are understood:
 *
 * - `-l`: List test cases available in a test executable.
 * - `--seed=SEED`: Provide a random seed to reproduce test
 *   runs using random numbers.
 * - `--verbose`: Run tests verbosely.
 * - `-q`, `--quiet`: Run tests quietly.
 * - `-p PATH`: Execute all tests matching the given path.
 * - `-s PATH`: Skip all tests matching the given path.
 *   This can also be used to force a test to run that would otherwise
 *   be skipped (ie, a test whose name contains "/subprocess").
 * - `-m {perf|slow|thorough|quick|undefined|no-undefined}`: Execute tests according
 *   to these test modes:
 *
 *   `perf`: Performance tests, may take long and report results (off by default).
 *
 *   `slow`, `thorough`: Slow and thorough tests, may take quite long and maximize
 *   coverage (off by default).
 *
 *   `quick`: Quick tests, should run really quickly and give good coverage (the default).
 *
 *   `undefined`: Tests for undefined behaviour, may provoke programming errors
 *   under [func@GLib.test_trap_subprocess] or [func@GLib.test_expect_message]
 *   to check that appropriate assertions or warnings are given (the default).
 *
 *   `no-undefined`: Avoid tests for undefined behaviour.
 *
 * - `--debug-log`: Debug test logging output.
 *
 * Any parsed arguments are removed from @argv, and @argc is adjust accordingly.
 *
 * The following options are supported:
 *
 * - `G_TEST_OPTION_NO_PRGNAME`: Causes g_test_init() to not call
 *   [func@GLib.set_prgname]. Since. 2.84
 * - `G_TEST_OPTION_ISOLATE_DIRS`: Creates a unique temporary directory for each
 *   unit test and sets XDG directories to point there for the duration of the unit
 *   test. See [const@GLib.TEST_OPTION_ISOLATE_DIRS].
 * - `G_TEST_OPTION_NONFATAL_ASSERTIONS`: This has the same effect as
 *   [func@GLib.test_set_nonfatal_assertions]. Since 2.84
 *
 * Since 2.58, if tests are compiled with `G_DISABLE_ASSERT` defined, `g_test_init()`
 * will print an error and exit. This is to prevent no-op tests from being executed,
 * as [func@GLib.assert] is commonly (erroneously) used in unit tests, and is a no-op
 * when compiled with `G_DISABLE_ASSERT`. Ensure your tests are compiled without
 * `G_DISABLE_ASSERT` defined.
 *
 * Since: 2.16
 */
void
(g_test_init) (int    *argc,
               char ***argv,
               ...)
{
  static char seedstr[4 + 4 * 8 + 1];
  va_list args;
  gpointer option;
  /* make warnings and criticals fatal for all test programs */
  GLogLevelFlags fatal_mask = (GLogLevelFlags) g_log_set_always_fatal ((GLogLevelFlags) G_LOG_FATAL_MASK);

  fatal_mask = (GLogLevelFlags) (fatal_mask | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
  g_log_set_always_fatal (fatal_mask);
  /* check caller args */
  g_return_if_fail (argc != NULL);
  g_return_if_fail (argv != NULL);
  g_return_if_fail (g_test_config_vars->test_initialized == FALSE);
  mutable_test_config_vars.test_initialized = TRUE;

#ifdef _GLIB_ADDRESS_SANITIZER
  mutable_test_config_vars.test_undefined = FALSE;
#endif

#ifdef G_OS_WIN32
  // don't open a window for errors (like the "abort() was called one")
  _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile (_CRT_ERROR, _CRTDBG_FILE_STDERR);
  // while gtest tests tend to use g_assert and friends
  // if they do use the C standard assert macro we want to
  // output a message to stderr, not open a popup window
  _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile (_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  // in release mode abort() will pop up a windows error
  // reporting dialog, let's prevent that. Only msvcrxx and
  // the UCRT have this function, but there's no great way to
  // detect msvcrxx (that I know of) so only call this when using
  // the UCRT
#ifdef _UCRT
  _set_abort_behavior (0, _CALL_REPORTFAULT);
#endif
#endif

  va_start (args, argv);
  while ((option = va_arg (args, char *)))
    {
      if (g_strcmp0 (option, G_TEST_OPTION_NO_PRGNAME) == 0)
        no_g_set_prgname = TRUE;
      else if (g_strcmp0 (option, G_TEST_OPTION_ISOLATE_DIRS) == 0)
        test_isolate_dirs = TRUE;
      else if (g_strcmp0 (option, G_TEST_OPTION_NONFATAL_ASSERTIONS) == 0)
        test_nonfatal_assertions = TRUE;
    }
  va_end (args);

  /* parse args, sets up mode, changes seed, etc. */
  parse_args (argc, argv);

  if (test_run_seedstr == NULL)
    {
      /* setup random seed string */
      g_snprintf (seedstr, sizeof (seedstr), "R02S%08x%08x%08x%08x",
                  g_random_int(), g_random_int(), g_random_int(), g_random_int());
      test_run_seedstr = seedstr;
    }

  if (!g_get_prgname () && !no_g_set_prgname)
    g_set_prgname_once ((*argv)[0]);

  if (g_getenv ("G_TEST_ROOT_PROCESS"))
    {
      test_is_subtest = TRUE;
    }
  else if (!g_setenv ("G_TEST_ROOT_PROCESS", test_argv0 ? test_argv0 : "root", TRUE))
    {
      g_printerr ("%s: Failed to set environment variable ‘%s’\n",
                  test_argv0, "G_TEST_ROOT_PROCESS");
      exit (1);
    }

  /* Set up the temporary directory for isolating the test. We have to do this
   * early, as we want the return values from g_get_user_data_dir() (and
   * friends) to return subdirectories of the temporary directory throughout
   * the setup function, test, and teardown function, for each unit test.
   * See test_do_isolate_dirs().
   *
   * The directory is deleted at the bottom of g_test_run().
   *
   * Rather than setting the XDG_* environment variables we use a new
   * G_TEST_TMPDIR variable which gives the top-level temporary directory. This
   * allows test subprocesses to reuse the same temporary directory when
   * g_test_init() is called in them. */
  if (test_isolate_dirs)
    {
      if (g_getenv ("G_TEST_TMPDIR") == NULL)
        {
          gchar *test_prgname = NULL;
          gchar *tmpl = NULL;
          GError *local_error = NULL;

          test_prgname = g_path_get_basename (g_get_prgname ());
          if (*test_prgname == '\0')
            {
              g_free (test_prgname);
              test_prgname = g_strdup ("unknown");
            }
          tmpl = g_strdup_printf ("test_%s_XXXXXX", test_prgname);
          g_free (test_prgname);

          test_isolate_dirs_tmpdir = g_dir_make_tmp (tmpl, &local_error);
          if (local_error != NULL)
            {
              g_printerr ("%s: Failed to create temporary directory: %s\n",
                          (*argv)[0], local_error->message);
              g_error_free (local_error);
              exit (1);
            }
          g_free (tmpl);

          /* Propagate the temporary directory to subprocesses. */
          if (!g_setenv ("G_TEST_TMPDIR", test_isolate_dirs_tmpdir, TRUE))
            {
              g_printerr ("%s: Failed to set environment variable ‘%s’\n",
                          (*argv)[0], "G_TEST_TMPDIR");
              exit (1);
            }
          _g_unset_cached_tmp_dir ();

          /* And clear the traditional environment variables so subprocesses
           * spawned by the code under test can’t trash anything. If a test
           * spawns a process, the test is responsible for propagating
           * appropriate environment variables.
           *
           * We assume that any in-process code will use g_get_user_data_dir()
           * and friends, rather than getenv() directly.
           *
           * We set them to ‘/dev/null’ as that should fairly obviously not
           * accidentally work, and should be fairly greppable. */
            {
              const gchar *overridden_environment_variables[] =
                {
                  "HOME",
                  "XDG_CACHE_HOME",
                  "XDG_CONFIG_DIRS",
                  "XDG_CONFIG_HOME",
                  "XDG_DATA_DIRS",
                  "XDG_DATA_HOME",
                  "XDG_RUNTIME_DIR",
                };
              gsize i;

              for (i = 0; i < G_N_ELEMENTS (overridden_environment_variables); i++)
                {
                  if (!g_setenv (overridden_environment_variables[i], "/dev/null", TRUE))
                    {
                      g_printerr ("%s: Failed to set environment variable ‘%s’\n",
                                  (*argv)[0], overridden_environment_variables[i]);
                      exit (1);
                    }
                }
            }
        }

      /* Cache this for the remainder of this process’ lifetime. */
      test_tmpdir = g_getenv ("G_TEST_TMPDIR");
    }

  /* verify GRand reliability, needed for reliable seeds */
  if (1)
    {
      GRand *rg = g_rand_new_with_seed (0xc8c49fb6);
      guint32 t1 = g_rand_int (rg), t2 = g_rand_int (rg), t3 = g_rand_int (rg), t4 = g_rand_int (rg);
      /* g_print ("GRand-current: 0x%x 0x%x 0x%x 0x%x\n", t1, t2, t3, t4); */
      if (t1 != 0xfab39f9b || t2 != 0xb948fb0e || t3 != 0x3d31be26 || t4 != 0x43a19d66)
        g_warning ("random numbers are not GRand-2.2 compatible, seeds may be broken (check $G_RANDOM_VERSION)");
      g_rand_free (rg);
    }

  /* check rand seed */
  test_run_seed (test_run_seedstr);

  /* report program start */
  g_log_set_default_handler (gtest_default_log_handler, NULL);
  g_test_log (G_TEST_LOG_START_BINARY, g_get_prgname(), test_run_seedstr, 0, NULL);

  test_argv0_dirname = (test_argv0 != NULL) ? g_path_get_dirname (test_argv0) : g_strdup (".");

  /* Make sure we get the real dirname that the test was run from */
  if (g_str_has_suffix (test_argv0_dirname, "/.libs"))
    {
      gchar *tmp;
      tmp = g_path_get_dirname (test_argv0_dirname);
      g_free (test_argv0_dirname);
      test_argv0_dirname = tmp;
    }

  test_disted_files_dir = g_getenv ("G_TEST_SRCDIR");
  if (!test_disted_files_dir)
    test_disted_files_dir = test_argv0_dirname;

  test_built_files_dir = g_getenv ("G_TEST_BUILDDIR");
  if (!test_built_files_dir)
    test_built_files_dir = test_argv0_dirname;
}

static void
test_cleanup (void)
{
  /* Free statically allocated variables */

  g_clear_pointer (&test_run_rand, g_rand_free);

  g_clear_pointer (&test_argv0_dirname, g_free);

  g_clear_pointer (&test_initial_cwd, g_free);
}

static void
test_run_seed (const gchar *rseed)
{
  guint seed_failed = 0;
  if (test_run_rand)
    g_rand_free (test_run_rand);
  test_run_rand = NULL;
  while (strchr (" \t\v\r\n\f", *rseed))
    rseed++;
  if (strncmp (rseed, "R02S", 4) == 0)  /* seed for random generator 02 (GRand-2.2) */
    {
      const char *s = rseed + 4;
      if (strlen (s) >= 32)             /* require 4 * 8 chars */
        {
          guint32 seedarray[4];
          gchar *p, hexbuf[9] = { 0, };
          memcpy (hexbuf, s + 0, 8);
          seedarray[0] = g_ascii_strtoull (hexbuf, &p, 16);
          seed_failed += p != NULL && *p != 0;
          memcpy (hexbuf, s + 8, 8);
          seedarray[1] = g_ascii_strtoull (hexbuf, &p, 16);
          seed_failed += p != NULL && *p != 0;
          memcpy (hexbuf, s + 16, 8);
          seedarray[2] = g_ascii_strtoull (hexbuf, &p, 16);
          seed_failed += p != NULL && *p != 0;
          memcpy (hexbuf, s + 24, 8);
          seedarray[3] = g_ascii_strtoull (hexbuf, &p, 16);
          seed_failed += p != NULL && *p != 0;
          if (!seed_failed)
            {
              test_run_rand = g_rand_new_with_seed_array (seedarray, 4);
              return;
            }
        }
    }
  g_error ("Unknown or invalid random seed: %s", rseed);
}

/**
 * g_test_rand_int:
 *
 * Gets a reproducible random integer number.
 *
 * The random numbers generated by the g_test_rand_*() family of functions
 * change with every new test program start, unless the --seed option is
 * given when starting test programs.
 *
 * For individual test cases however, the random number generator is
 * reseeded, to avoid dependencies between tests and to make --seed
 * effective for all test cases.
 *
 * Returns: a random number from the seeded random number generator
 *
 * Since: 2.16
 */
gint32
g_test_rand_int (void)
{
  gint32 r;

  G_LOCK (test_run_rand);
  r = g_rand_int (test_run_rand);
  G_UNLOCK (test_run_rand);

  return r;
}

/**
 * g_test_rand_int_range:
 * @begin: the minimum value returned by this function
 * @end: the smallest value not to be returned by this function
 *
 * Gets a reproducible random integer number out of a specified range.
 *
 * See [func@GLib.test_rand_int] for details on test case random numbers.
 *
 * Returns: a number with @begin <= number < @end
 *
 * Since: 2.16
 */
gint32
g_test_rand_int_range (gint32          begin,
                       gint32          end)
{
  gint32 r;

  G_LOCK (test_run_rand);
  r = g_rand_int_range (test_run_rand, begin, end);
  G_UNLOCK (test_run_rand);

  return r;
}

/**
 * g_test_rand_double:
 *
 * Gets a reproducible random floating point number.
 *
 * See [func@GLib.test_rand_int] for details on test case random numbers.
 *
 * Returns: a random number from the seeded random number generator
 *
 * Since: 2.16
 */
double
g_test_rand_double (void)
{
  double r;

  G_LOCK (test_run_rand);
  r = g_rand_double (test_run_rand);
  G_UNLOCK (test_run_rand);

  return r;
}

/**
 * g_test_rand_double_range:
 * @range_start: the minimum value returned by this function
 * @range_end: the minimum value not returned by this function
 *
 * Gets a reproducible random floating point number out of a specified range.
 *
 * See [func@GLib.test_rand_int] for details on test case random numbers.
 *
 * Returns: a number with @range_start <= number < @range_end
 *
 * Since: 2.16
 */
double
g_test_rand_double_range (double          range_start,
                          double          range_end)
{
  double r;

  G_LOCK (test_run_rand);
  r = g_rand_double_range (test_run_rand, range_start, range_end);
  G_UNLOCK (test_run_rand);

  return r;
}

/**
 * g_test_timer_start:
 *
 * Starts a timing test.
 *
 * Call [func@GLib.test_timer_elapsed] when the task is supposed
 * to be done. Call this function again to restart the timer.
 *
 * Since: 2.16
 */
void
g_test_timer_start (void)
{
  if (!test_user_timer)
    test_user_timer = g_timer_new();
  test_user_stamp = 0;
  g_timer_start (test_user_timer);
}

/**
 * g_test_timer_elapsed:
 *
 * Gets the number of seconds since the last start of the timer with
 * [func@GLib.test_timer_start].
 *
 * Returns: the time since the last start of the timer in seconds
 *
 * Since: 2.16
 */
double
g_test_timer_elapsed (void)
{
  test_user_stamp = test_user_timer ? g_timer_elapsed (test_user_timer, NULL) : 0;
  return test_user_stamp;
}

/**
 * g_test_timer_last:
 *
 * Reports the last result of [func@GLib.test_timer_elapsed].
 *
 * Returns: the last result of [func@GLib.test_timer_elapsed]
 *
 * Since: 2.16
 */
double
g_test_timer_last (void)
{
  return test_user_stamp;
}

/**
 * g_test_minimized_result:
 * @minimized_quantity: the reported value
 * @format: the format string of the report message
 * @...: printf-like arguments to @format
 *
 * Reports the result of a performance or measurement test.
 *
 * The test should generally strive to minimize the reported
 * quantities (smaller values are better than larger ones),
 * this and @minimized_quantity can determine sorting
 * order for test result reports.
 *
 * Since: 2.16
 */
void
g_test_minimized_result (double          minimized_quantity,
                         const char     *format,
                         ...)
{
  long double largs = minimized_quantity;
  gchar *buffer;
  va_list args;

  va_start (args, format);
  buffer = g_strdup_vprintf (format, args);
  va_end (args);

  g_test_log (G_TEST_LOG_MIN_RESULT, buffer, NULL, 1, &largs);
  g_free (buffer);
}

/**
 * g_test_maximized_result:
 * @maximized_quantity: the reported value
 * @format: the format string of the report message
 * @...: printf-like arguments to @format
 *
 * Reports the result of a performance or measurement test.
 *
 * The test should generally strive to maximize the reported
 * quantities (larger values are better than smaller ones),
 * this and @maximized_quantity can determine sorting
 * order for test result reports.
 *
 * Since: 2.16
 */
void
g_test_maximized_result (double          maximized_quantity,
                         const char     *format,
                         ...)
{
  long double largs = maximized_quantity;
  gchar *buffer;
  va_list args;

  va_start (args, format);
  buffer = g_strdup_vprintf (format, args);
  va_end (args);

  g_test_log (G_TEST_LOG_MAX_RESULT, buffer, NULL, 1, &largs);
  g_free (buffer);
}

/**
 * g_test_message:
 * @format: the format string
 * @...: printf-like arguments to @format
 *
 * Adds a message to the test report.
 *
 * Since: 2.16
 */
void
g_test_message (const char *format,
                ...)
{
  gchar *buffer;
  va_list args;

  va_start (args, format);
  buffer = g_strdup_vprintf (format, args);
  va_end (args);

  g_test_log (G_TEST_LOG_MESSAGE, buffer, NULL, 0, NULL);
  g_free (buffer);
}

/**
 * g_test_bug_base:
 * @uri_pattern: the base pattern for bug URIs
 *
 * Specifies the base URI for bug reports.
 *
 * The base URI is used to construct bug report messages for
 * [func@GLib.test_message] when [func@GLib.test_bug] is called.
 * Calling this function outside of a test case sets the
 * default base URI for all test cases. Calling it from within
 * a test case changes the base URI for the scope of the test
 * case only.
 * Bug URIs are constructed by appending a bug specific URI
 * portion to @uri_pattern, or by replacing the special string
 * `%s` within @uri_pattern if that is present.
 *
 * If [func@GLib.test_bug_base] is not called, bug URIs are formed
 * solely from the value provided by [func@GLib.test_bug].
 *
 * Since: 2.16
 */
void
g_test_bug_base (const char *uri_pattern)
{
  g_free (test_uri_base);
  test_uri_base = g_strdup (uri_pattern);
}

/**
 * g_test_bug:
 * @bug_uri_snippet: Bug specific bug tracker URI or URI portion.
 *
 * Adds a message to test reports that associates a bug URI with a test case.
 *
 * Bug URIs are constructed from a base URI set with [func@GLib.test_bug_base]
 * and @bug_uri_snippet. If [func@GLib.test_bug_base] has not been called, it is
 * assumed to be the empty string, so a full URI can be provided to
 * [func@GLib.test_bug] instead.
 *
 * See also [func@GLib.test_summary].
 *
 * Since GLib 2.70, the base URI is not prepended to @bug_uri_snippet
 * if it is already a valid URI.
 *
 * Since: 2.16
 */
void
g_test_bug (const char *bug_uri_snippet)
{
  const char *c = NULL;

  g_return_if_fail (bug_uri_snippet != NULL);

  if (g_str_has_prefix (bug_uri_snippet, "http:") ||
      g_str_has_prefix (bug_uri_snippet, "https:"))
    {
      g_test_message ("Bug Reference: %s", bug_uri_snippet);
      return;
    }

  if (test_uri_base != NULL)
    c = strstr (test_uri_base, "%s");
  if (c)
    {
      char *b = g_strndup (test_uri_base, c - test_uri_base);
      char *s = g_strconcat (b, bug_uri_snippet, c + 2, NULL);
      g_free (b);
      g_test_message ("Bug Reference: %s", s);
      g_free (s);
    }
  else
    g_test_message ("Bug Reference: %s%s",
                    test_uri_base ? test_uri_base : "", bug_uri_snippet);
}

/**
 * g_test_summary:
 * @summary: summary of the test purpose
 *
 * Sets the summary for a test.
 *
 * This may be included in test report output, and is useful documentation for
 * anyone reading the source code or modifying a test in future. It must be a
 * single line, and it should summarise what the test checks, and how.
 *
 * This should be called at the top of a test function.
 *
 * For example:
 *
 * ```c
 * static void
 * test_array_sort (void)
 * {
 *   g_test_summary ("Test my_array_sort() sorts the array correctly and stably, "
 *                   "including testing zero length and one-element arrays.");
 *
 *   // ...
 * }
 * ```
 *
 * See also [func@GLib.test_bug].
 *
 * Since: 2.62
 */
void
g_test_summary (const char *summary)
{
  g_return_if_fail (summary != NULL);
  g_return_if_fail (strchr (summary, '\n') == NULL);
  g_return_if_fail (strchr (summary, '\r') == NULL);

  g_test_message ("%s summary: %s", test_run_name, summary);
}

/**
 * g_test_get_root:
 *
 * Gets the toplevel test suite for the test path API.
 *
 * Returns: the toplevel test suite
 *
 * Since: 2.16
 */
GTestSuite*
g_test_get_root (void)
{
  if (!test_suite_root)
    {
      test_suite_root = g_test_create_suite ("root");
      g_free (test_suite_root->name);
      test_suite_root->name = g_strdup ("");
    }

  return test_suite_root;
}

/**
 * g_test_run:
 *
 * Runs all tests under the toplevel suite.
 *
 * The toplevel suite can be retrieved with [func@GLib.test_get_root].
 *
 * Similar to [func@GLib.test_run_suite], the test cases to be run are
 * filtered according to test path arguments (`-p testpath` and `-s testpath`)
 * as parsed by [func@GLib.test_init]. [func@GLib.test_run_suite] or
 * [func@GLib.test_run] may only be called once in a program.
 *
 * In general, the tests and sub-suites within each suite are run in
 * the order in which they are defined. However, note that prior to
 * GLib 2.36, there was a bug in the `g_test_add_*`
 * functions which caused them to create multiple suites with the same
 * name, meaning that if you created tests "/foo/simple",
 * "/bar/simple", and "/foo/using-bar" in that order, they would get
 * run in that order (since [func@GLib.test_run] would run the first "/foo"
 * suite, then the "/bar" suite, then the second "/foo" suite). As of
 * 2.36, this bug is fixed, and adding the tests in that order would
 * result in a running order of "/foo/simple", "/foo/using-bar",
 * "/bar/simple". If this new ordering is sub-optimal (because it puts
 * more-complicated tests before simpler ones, making it harder to
 * figure out exactly what has failed), you can fix it by changing the
 * test paths to group tests by suite in a way that will result in the
 * desired running order. Eg, "/simple/foo", "/simple/bar",
 * "/complex/foo-using-bar".
 *
 * However, you should never make the actual result of a test depend
 * on the order that tests are run in. If you need to ensure that some
 * particular code runs before or after a given test case, use
 * [func@GLib.test_add], which lets you specify setup and teardown functions.
 *
 * If all tests are skipped or marked as incomplete (expected failures),
 * this function will return 0 if producing TAP output, or 77 (treated
 * as "skip test" by Automake) otherwise.
 *
 * Returns: 0 on success, 1 on failure (assuming it returns at all),
 *   0 or 77 if all tests were skipped or marked as incomplete
 *
 * Since: 2.16
 */
int
g_test_run (void)
{
  int ret;
  GTestSuite *suite;

  if (atexit (test_cleanup) != 0)
    {
      int errsv = errno;
      g_error ("Unable to register test cleanup to be run at exit: %s",
               g_strerror (errsv));
    }

  suite = g_test_get_root ();
  if (g_test_run_suite (suite) != 0)
    {
      ret = 1;
      goto out;
    }

  /* Clean up the temporary directory. */
  if (test_isolate_dirs_tmpdir != NULL)
    {
      rm_rf (test_isolate_dirs_tmpdir);
      g_free (test_isolate_dirs_tmpdir);
      test_isolate_dirs_tmpdir = NULL;
    }

  /* 77 is special to Automake's default driver, but not Automake's TAP driver
   * or Perl's prove(1) TAP driver. */
  if (test_tap_log)
    {
      ret = 0;
      goto out;
    }

  if (test_run_count > 0 && test_run_count == test_skipped_count)
    {
      ret = 77;
      goto out;
    }
  else
    {
      ret = 0;
      goto out;
    }

out:
  g_test_suite_free (suite);
  return ret;
}

/**
 * g_test_create_case:
 * @test_name: the name for the test case
 * @data_size: the size of the fixture data structure
 * @test_data: test data argument for the test functions
 * @data_setup: (scope async): the function to set up the fixture data
 * @data_test: (scope async): the actual test function
 * @data_teardown: (scope async): the function to teardown the fixture data
 *
 * Creates a new [struct@GLib.TestCase].
 *
 * This API is fairly low level, and calling [func@GLib.test_add] or
 * [func@GLib.test_add_func] is preferable.
 *
 * When this test is executed, a fixture structure of size @data_size
 * will be automatically allocated and filled with zeros. Then @data_setup
 * is called to initialize the fixture. After fixture setup, the actual test
 * function @data_test is called. Once the test run completes, the
 * fixture structure is torn down by calling @data_teardown and after
 * that the memory is automatically released by the test framework.
 *
 * Splitting up a test run into fixture setup, test function and
 * fixture teardown is most useful if the same fixture type is used for
 * multiple tests. In this cases, [func@GLib.test_create_case] will be
 * called with the same type of fixture (the @data_size argument), but
 * varying @test_name and @data_test arguments.
 *
 * Returns: a newly allocated test case
 *
 * Since: 2.16
 */
GTestCase*
g_test_create_case (const char       *test_name,
                    gsize             data_size,
                    gconstpointer     test_data,
                    GTestFixtureFunc  data_setup,
                    GTestFixtureFunc  data_test,
                    GTestFixtureFunc  data_teardown)
{
  GTestCase *tc;

  g_return_val_if_fail (test_name != NULL, NULL);
  g_return_val_if_fail (strchr (test_name, '/') == NULL, NULL);
  g_return_val_if_fail (test_name[0] != 0, NULL);
  g_return_val_if_fail (data_test != NULL, NULL);

  tc = g_slice_new0 (GTestCase);
  tc->name = g_strdup (test_name);
  tc->test_data = (gpointer) test_data;
  tc->fixture_size = data_size;
  tc->fixture_setup = (void*) data_setup;
  tc->fixture_test = (void*) data_test;
  tc->fixture_teardown = (void*) data_teardown;

  return tc;
}

static gint
find_suite (gconstpointer l, gconstpointer s)
{
  const GTestSuite *suite = l;
  const gchar *str = s;

  return strcmp (suite->name, str);
}

static gint
find_case (gconstpointer l, gconstpointer s)
{
  const GTestCase *tc = l;
  const gchar *str = s;

  return strcmp (tc->name, str);
}

/**
 * GTestFixtureFunc:
 * @fixture: (not nullable): the test fixture
 * @user_data: the data provided when registering the test
 *
 * The type used for functions that operate on test fixtures.
 *
 * This is used for the fixture setup and teardown functions
 * as well as for the testcases themselves.
 *
 * @user_data is a pointer to the data that was given when
 * registering the test case.
 *
 * @fixture will be a pointer to the area of memory allocated by the
 * test framework, of the size requested.  If the requested size was
 * zero then @fixture will be equal to @user_data.
 *
 * Since: 2.28
 */
void
g_test_add_vtable (const char       *testpath,
                   gsize             data_size,
                   gconstpointer     test_data,
                   GTestFixtureFunc  data_setup,
                   GTestFixtureFunc  fixture_test_func,
                   GTestFixtureFunc  data_teardown)
{
  gchar **segments;
  guint ui;
  GTestSuite *suite;

  g_return_if_fail (testpath != NULL);
  g_return_if_fail (g_path_is_absolute (testpath));
  g_return_if_fail (fixture_test_func != NULL);
  g_return_if_fail (!test_isolate_dirs || strstr (testpath, "/.") == NULL);

  suite = g_test_get_root();
  segments = g_strsplit (testpath, "/", -1);
  for (ui = 0; segments[ui] != NULL; ui++)
    {
      const char *seg = segments[ui];
      gboolean islast = segments[ui + 1] == NULL;
      if (islast && !seg[0])
        g_error ("invalid test case path: %s", testpath);
      else if (!seg[0])
        continue;       /* initial or duplicate slash */
      else if (!islast)
        {
          GSList *l;
          GTestSuite *csuite;
          l = g_slist_find_custom (suite->suites, seg, find_suite);
          if (l)
            {
              csuite = l->data;
            }
          else
            {
              csuite = g_test_create_suite (seg);
              g_test_suite_add_suite (suite, csuite);
            }
          suite = csuite;
        }
      else /* islast */
        {
          GTestCase *tc;

          if (g_slist_find_custom (suite->cases, seg, find_case))
            g_error ("duplicate test case path: %s", testpath);

          tc = g_test_create_case (seg, data_size, test_data, data_setup, fixture_test_func, data_teardown);
          g_test_suite_add (suite, tc);
        }
    }
  g_strfreev (segments);
}

/**
 * g_test_fail:
 *
 * Indicates that a test failed.
 *
 * This function can be called multiple times from the same test.
 * You can use this function if your test failed in a recoverable way.
 *
 * Do not use this function if the failure of a test could cause
 * other tests to malfunction.
 *
 * Calling this function will not stop the test from running, you
 * need to return from the test function yourself. So you can
 * produce additional diagnostic messages or even continue running
 * the test.
 *
 * If not called from inside a test, this function does nothing.
 *
 * Note that unlike [func@GLib.test_skip] and [func@GLib.test_incomplete],
 * this function does not log a message alongside the test failure.
 * If details of the test failure are available, either log them with
 * [func@GLib.test_message] before [func@GLib.test_fail], or use
 * [func@GLib.test_fail_printf] instead.
 *
 * Since: 2.30
 **/
void
g_test_fail (void)
{
  test_run_success = G_TEST_RUN_FAILURE;
  g_clear_pointer (&test_run_msg, g_free);
}

/**
 * g_test_fail_printf:
 * @format: the format string
 * @...: printf-like arguments to @format
 *
 * Indicates that a test failed and records a message.
 *
 * Also see [func@GLib.test_fail].
 *
 * The message is formatted as if by [func@GLib.strdup_printf].
 *
 * Since: 2.70
 **/
void
g_test_fail_printf (const char *format,
                    ...)
{
  va_list args;

  test_run_success = G_TEST_RUN_FAILURE;
  va_start (args, format);
  g_free (test_run_msg);
  test_run_msg = g_strdup_vprintf (format, args);
  va_end (args);
}

/**
 * g_test_incomplete:
 * @msg: (nullable): explanation
 *
 * Indicates that a test failed because of some incomplete
 * functionality.
 *
 * This function can be called multiple times from the same test.
 *
 * Calling this function will not stop the test from running, you
 * need to return from the test function yourself. So you can
 * produce additional diagnostic messages or even continue running
 * the test.
 *
 * If not called from inside a test, this function does nothing.
 *
 * Since: 2.38
 */
void
g_test_incomplete (const gchar *msg)
{
  test_run_success = G_TEST_RUN_INCOMPLETE;
  g_free (test_run_msg);
  test_run_msg = g_strdup (msg);
}

/**
 * g_test_incomplete_printf:
 * @format: the format string
 * @...: printf-like arguments to @format
 *
 * Indicates that a test failed because of some incomplete
 * functionality.
 *
 * Equivalent to [func@GLib.test_incomplete], but the explanation
 * is formatted as if by [func@GLib.strdup_printf].
 *
 * Since: 2.70
 */
void
g_test_incomplete_printf (const char *format,
                          ...)
{
  va_list args;

  test_run_success = G_TEST_RUN_INCOMPLETE;
  va_start (args, format);
  g_free (test_run_msg);
  test_run_msg = g_strdup_vprintf (format, args);
  va_end (args);
}

/**
 * g_test_skip:
 * @msg: (nullable): explanation
 *
 * Indicates that a test was skipped.
 *
 * Calling this function will not stop the test from running, you
 * need to return from the test function yourself. So you can
 * produce additional diagnostic messages or even continue running
 * the test.
 *
 * If not called from inside a test, this function does nothing.
 *
 * Since: 2.38
 */
void
g_test_skip (const gchar *msg)
{
  test_run_success = G_TEST_RUN_SKIPPED;
  g_free (test_run_msg);
  test_run_msg = g_strdup (msg);
}

/**
 * g_test_skip_printf:
 * @format: the format string
 * @...: printf-like arguments to @format
 *
 * Indicates that a test was skipped.
 *
 * Equivalent to [func@GLib.test_skip], but the explanation
 * is formatted as if by [func@GLib.strdup_printf].
 *
 * Since: 2.70
 */
void
g_test_skip_printf (const char *format,
                    ...)
{
  va_list args;

  test_run_success = G_TEST_RUN_SKIPPED;
  va_start (args, format);
  g_free (test_run_msg);
  test_run_msg = g_strdup_vprintf (format, args);
  va_end (args);
}

/**
 * g_test_failed:
 *
 * Returns whether a test has already failed.
 *
 * This will be the case when [func@GLib.test_fail],
 * [func@GLib.test_incomplete] or [func@GLib.test_skip] have
 * been called, but also if an assertion has failed.
 *
 * This can be useful to return early from a test if
 * continuing after a failed assertion might be harmful.
 *
 * The return value of this function is only meaningful
 * if it is called from inside a test function.
 *
 * Returns: true if the test has failed
 *
 * Since: 2.38
 */
gboolean
g_test_failed (void)
{
  return test_run_success != G_TEST_RUN_SUCCESS;
}

/**
 * g_test_set_nonfatal_assertions:
 *
 * Changes the behaviour of the various assertion macros.
 *
 * The `g_assert_*()` macros, `g_test_assert_expected_messages()`
 * and the various `g_test_trap_assert_*()` macros are changed
 * to not abort to program.
 *
 * Instead, they will call [func@GLib.test_fail] and continue.
 * (This also changes the behavior of [func@GLib.test_fail] so that
 * it will not cause the test program to abort after completing
 * the failed test.)
 *
 * Note that the [func@GLib.assert_not_reached] and [func@GLib.assert]
 * macros are not affected by this.
 *
 * This function can only be called after [func@GLib.test_init].
 *
 * Since: 2.38
 */
void
g_test_set_nonfatal_assertions (void)
{
  if (!g_test_config_vars->test_initialized)
    g_error ("g_test_set_nonfatal_assertions called without g_test_init");
  test_nonfatal_assertions = TRUE;
  test_mode_fatal = FALSE;
}

/**
 * GTestFunc:
 *
 * The type used for test case functions.
 *
 * Since: 2.28
 */

/**
 * g_test_add_func:
 * @testpath: a /-separated name for the test
 * @test_func: (scope async): the test function to invoke for this test
 *
 * Creates a new test case.
 *
 * This function is similar to [func@GLib.test_create_case].
 * However the test is assumed to use no fixture, and test suites are
 * automatically created on the fly and added to the root fixture,
 * based on the /-separated portions of @testpath.
 *
 * If @testpath includes the component "subprocess" anywhere in it,
 * the test will be skipped by default, and only run if explicitly
 * required via the `-p` command-line option or [func@GLib.test_trap_subprocess].
 *
 * No component of @testpath may start with a dot (`.`) if the
 * [const@GLib.TEST_OPTION_ISOLATE_DIRS] option is being used; and
 * it is recommended to do so even if it isn’t.
 *
 * Since: 2.16
 */
void
g_test_add_func (const char *testpath,
                 GTestFunc   test_func)
{
  g_return_if_fail (testpath != NULL);
  g_return_if_fail (testpath[0] == '/');
  g_return_if_fail (test_func != NULL);
  g_test_add_vtable (testpath, 0, NULL, NULL, (GTestFixtureFunc) test_func, NULL);
}

/**
 * GTestDataFunc:
 * @user_data: the data provided when registering the test
 *
 * The type used for test case functions that take an extra pointer
 * argument.
 *
 * Since: 2.28
 */

/**
 * g_test_add_data_func:
 * @testpath: a /-separated name for the test
 * @test_data: data for the @test_func
 * @test_func: (scope async): the test function to invoke for this test
 *
 * Creates a new test case.
 *
 * This function is similar to [func@GLib.test_create_case].
 * However the test is assumed to use no fixture, and test suites are
 * automatically created on the fly and added to the root fixture,
 * based on the /-separated portions of @testpath. The @test_data
 * argument will be passed as first argument to @test_func.
 *
 * If @testpath includes the component "subprocess" anywhere in it,
 * the test will be skipped by default, and only run if explicitly
 * required via the `-p` command-line option or [func@GLib.test_trap_subprocess].
 *
 * No component of @testpath may start with a dot (`.`) if the
 * [const@GLib.TEST_OPTION_ISOLATE_DIRS] option is being used;
 * and it is recommended to do so even if it isn’t.
 *
 * Since: 2.16
 */
void
g_test_add_data_func (const char     *testpath,
                      gconstpointer   test_data,
                      GTestDataFunc   test_func)
{
  g_return_if_fail (testpath != NULL);
  g_return_if_fail (testpath[0] == '/');
  g_return_if_fail (test_func != NULL);

  g_test_add_vtable (testpath, 0, test_data, NULL, (GTestFixtureFunc) test_func, NULL);
}

/**
 * g_test_add_data_func_full:
 * @testpath: a /-separated name for the test
 * @test_data: data for @test_func
 * @test_func: the test function to invoke for this test
 * @data_free_func: #GDestroyNotify for @test_data
 *
 * Creates a new test case.
 *
 * In constract to [func@GLib.test_add_data_func], this function
 * is freeing @test_data after the test run is complete.
 *
 * Since: 2.34
 */
void
g_test_add_data_func_full (const char     *testpath,
                           gpointer        test_data,
                           GTestDataFunc   test_func,
                           GDestroyNotify  data_free_func)
{
  g_return_if_fail (testpath != NULL);
  g_return_if_fail (testpath[0] == '/');
  g_return_if_fail (test_func != NULL);

  g_test_add_vtable (testpath, 0, test_data, NULL,
                     (GTestFixtureFunc) test_func,
                     (GTestFixtureFunc) data_free_func);
}

static gboolean
g_test_suite_case_exists (GTestSuite *suite,
                          const char *test_path)
{
  GSList *iter;
  char *slash;
  GTestCase *tc;

  test_path++;
  slash = strchr (test_path, '/');

  if (slash)
    {
      for (iter = suite->suites; iter; iter = iter->next)
        {
          GTestSuite *child_suite = iter->data;

          if (!strncmp (child_suite->name, test_path, slash - test_path))
            if (g_test_suite_case_exists (child_suite, slash))
              return TRUE;
        }
    }
  else
    {
      for (iter = suite->cases; iter; iter = iter->next)
        {
          tc = iter->data;
          if (!strcmp (tc->name, test_path))
            return TRUE;
        }
    }

  return FALSE;
}

/**
 * g_test_create_suite:
 * @suite_name: a name for the suite
 *
 * Creates a new test suite with the name @suite_name.
 *
 * Returns: a newly allocated test suite
 *
 * Since: 2.16
 */
GTestSuite*
g_test_create_suite (const char *suite_name)
{
  GTestSuite *ts;
  g_return_val_if_fail (suite_name != NULL, NULL);
  g_return_val_if_fail (strchr (suite_name, '/') == NULL, NULL);
  g_return_val_if_fail (suite_name[0] != 0, NULL);
  ts = g_slice_new0 (GTestSuite);
  ts->name = g_strdup (suite_name);
  return ts;
}

/**
 * g_test_suite_add:
 * @suite: a test suite
 * @test_case: a test case
 *
 * Adds @test_case to @suite.
 *
 * Since: 2.16
 */
void
g_test_suite_add (GTestSuite     *suite,
                  GTestCase      *test_case)
{
  g_return_if_fail (suite != NULL);
  g_return_if_fail (test_case != NULL);

  suite->cases = g_slist_append (suite->cases, test_case);
}

/**
 * g_test_suite_add_suite:
 * @suite: a test suite
 * @nestedsuite: another test suite
 *
 * Adds @nestedsuite to @suite.
 *
 * Since: 2.16
 */
void
g_test_suite_add_suite (GTestSuite     *suite,
                        GTestSuite     *nestedsuite)
{
  g_return_if_fail (suite != NULL);
  g_return_if_fail (nestedsuite != NULL);

  suite->suites = g_slist_append (suite->suites, nestedsuite);
}

/**
 * g_test_queue_free:
 * @gfree_pointer: the pointer to be stored
 *
 * Enqueues a pointer to be released with [func@GLib.free]
 * during the next teardown phase.
 *
 * This is equivalent to calling [func@GLib.test_queue_destroy]
 * with a destroy callback of [func@GLib.free].
 *
 * Since: 2.16
 */
void
g_test_queue_free (gpointer gfree_pointer)
{
  if (gfree_pointer)
    g_test_queue_destroy (g_free, gfree_pointer);
}

/**
 * g_test_queue_destroy:
 * @destroy_func: destroy callback for teardown phase
 * @destroy_data: destroy callback data
 *
 * Enqueues a callback @destroy_func to be executed during the next test case
 * teardown phase.
 *
 * This is most useful to auto destroy allocated test resources at the end
 * of a test run. Resources are released in reverse queue order, that means
 * enqueueing callback `A` before callback `B` will cause `B()` to be called
 * before `A()` during teardown.
 *
 * Since: 2.16
 */
void
g_test_queue_destroy (GDestroyNotify destroy_func,
                      gpointer       destroy_data)
{
  DestroyEntry *dentry;

  g_return_if_fail (destroy_func != NULL);

  dentry = g_slice_new0 (DestroyEntry);
  dentry->destroy_func = destroy_func;
  dentry->destroy_data = destroy_data;
  dentry->next = test_destroy_queue;
  test_destroy_queue = dentry;
}

static gint
test_has_prefix (gconstpointer a,
                 gconstpointer b)
{
    const gchar *test_path_skipped_local = (const gchar *)a;
    const gchar* test_run_name_local = (const gchar*)b;
    if (test_prefix_extended_skipped)
      {
        /* If both are null, we consider that it doesn't match */
        if (!test_path_skipped_local || !test_run_name_local)
          return FALSE;
        return strncmp (test_run_name_local, test_path_skipped_local, strlen (test_path_skipped_local));
      }
    return g_strcmp0 (test_run_name_local, test_path_skipped_local);
}

static gboolean test_should_run (const char *test_path,
                                 const char *cmp_path);

static gboolean
test_case_run (GTestCase  *tc,
               const char *test_run_name,
               const char *path)
{
  gchar *old_base = NULL;
  GSList **old_free_list, *filename_free_list = NULL;
  gboolean success = G_TEST_RUN_SUCCESS;

  old_base = g_strdup (test_uri_base);
  old_free_list = test_filename_free_list;
  test_filename_free_list = &filename_free_list;

  if (!test_should_run (test_run_name, path))
    {
      /* Silently skip the test and return success. This happens if it’s a
       * /subprocess path. */
      success = G_TEST_RUN_SKIPPED;
    }
  else if (++test_run_count <= test_startup_skip_count)
    g_test_log (G_TEST_LOG_SKIP_CASE, test_run_name, NULL, 0, NULL);
  else if (test_run_list)
    {
      g_print ("%s\n", test_run_name);
      g_test_log (G_TEST_LOG_LIST_CASE, test_run_name, NULL, 0, NULL);
    }
  else
    {
      GTimer *test_run_timer = g_timer_new();
      long double largs[G_TEST_CASE_LARGS_MAX];
      void *fixture;
      g_test_log (G_TEST_LOG_START_CASE, test_run_name, NULL, 0, NULL);
      test_run_forks = 0;
      test_run_success = G_TEST_RUN_SUCCESS;
      g_clear_pointer (&test_run_msg, g_free);
      g_test_log_set_fatal_handler (NULL, NULL);
      if (test_paths_skipped && g_slist_find_custom (test_paths_skipped, test_run_name, (GCompareFunc)test_has_prefix))
        g_test_skip ("by request (-s option)");
      else
        {
          GError *local_error = NULL;

          if (!test_do_isolate_dirs (&local_error))
            {
              g_test_log (G_TEST_LOG_ERROR, local_error->message, NULL, 0, NULL);
              g_test_fail ();
              g_error_free (local_error);
            }
          else
            {
              g_timer_start (test_run_timer);
              fixture = tc->fixture_size ? g_malloc0 (tc->fixture_size) : tc->test_data;
              test_run_seed (test_run_seedstr);
              if (tc->fixture_setup)
                tc->fixture_setup (fixture, tc->test_data);
              tc->fixture_test (fixture, tc->test_data);
              test_trap_clear();
              while (test_destroy_queue)
                {
                  DestroyEntry *dentry = test_destroy_queue;
                  test_destroy_queue = dentry->next;
                  dentry->destroy_func (dentry->destroy_data);
                  g_slice_free (DestroyEntry, dentry);
                }
              if (tc->fixture_teardown)
                tc->fixture_teardown (fixture, tc->test_data);
              tc->fixture_teardown = NULL;
              if (tc->fixture_size)
                g_free (fixture);
              g_timer_stop (test_run_timer);
            }

          test_rm_isolate_dirs ();
        }
      success = test_run_success;
      test_run_success = G_TEST_RUN_FAILURE;
      largs[G_TEST_CASE_LARGS_RESULT] = success; /* OK */
      largs[G_TEST_CASE_LARGS_RUN_FORKS] = test_run_forks;
      largs[G_TEST_CASE_LARGS_EXECUTION_TIME] = g_timer_elapsed (test_run_timer, NULL);
      g_test_log (G_TEST_LOG_STOP_CASE, test_run_name, test_run_msg, G_N_ELEMENTS (largs), largs);
      g_clear_pointer (&test_run_msg, g_free);
      g_timer_destroy (test_run_timer);
    }

  g_slist_free_full (filename_free_list, g_free);
  test_filename_free_list = old_free_list;
  g_free (test_uri_base);
  test_uri_base = old_base;

  return (success == G_TEST_RUN_SUCCESS ||
          success == G_TEST_RUN_SKIPPED ||
          success == G_TEST_RUN_INCOMPLETE);
}

static gboolean
path_has_prefix (const char *path,
                 const char *prefix)
{
  size_t prefix_len = strlen (prefix);

  return (strncmp (path, prefix, prefix_len) == 0 &&
          (path[prefix_len] == '\0' ||
           path[prefix_len] == '/'));
}

static gboolean
test_should_run (const char *test_path,
                 const char *cmp_path)
{
  if (strstr (test_run_name, "/subprocess"))
    {
      if (g_strcmp0 (test_path, cmp_path) == 0)
        return TRUE;

      if (g_test_verbose ())
        {
          if (test_tap_log)
            g_print ("skipping: %s\n", test_run_name);
          else
            g_print ("GTest: skipping: %s\n", test_run_name);
        }
      return FALSE;
    }

  return !cmp_path || path_has_prefix (test_path, cmp_path);
}

/* Recurse through @suite, running tests matching @path (or all tests
 * if @path is `NULL`).
 */
static int
g_test_run_suite_internal (GTestSuite *suite,
                           const char *path)
{
  guint n_bad = 0;
  gchar *old_name = test_run_name;
  gchar *old_name_path = test_run_name_path;
  GSList *iter;

  g_return_val_if_fail (suite != NULL, -1);

  g_test_log (G_TEST_LOG_START_SUITE, suite->name, NULL, 0, NULL);

  for (iter = suite->cases; iter; iter = iter->next)
    {
      GTestCase *tc = iter->data;

      test_run_name = g_build_path ("/", old_name, tc->name, NULL);
      test_run_name_path = g_build_path (G_DIR_SEPARATOR_S, old_name_path, tc->name, NULL);

      if (!test_case_run (tc, test_run_name, path))
        n_bad++;

      g_free (test_run_name);
      g_free (test_run_name_path);
    }

  for (iter = suite->suites; iter; iter = iter->next)
    {
      GTestSuite *ts = iter->data;

      test_run_name = g_build_path ("/", old_name, ts->name, NULL);
      test_run_name_path = g_build_path (G_DIR_SEPARATOR_S, old_name_path, ts->name, NULL);
      if (test_prefix_extended) {
        if (!path || path_has_prefix (test_run_name, path))
          n_bad += g_test_run_suite_internal (ts, test_run_name);
        else if (!path || path_has_prefix (path, test_run_name))
          n_bad += g_test_run_suite_internal (ts, path);
      } else if (!path || path_has_prefix (path, test_run_name)) {
        n_bad += g_test_run_suite_internal (ts, path);
      }

      g_free (test_run_name);
      g_free (test_run_name_path);
    }

  test_run_name = old_name;
  test_run_name_path = old_name_path;

  g_test_log (G_TEST_LOG_STOP_SUITE, suite->name, NULL, 0, NULL);

  return n_bad;
}

static int
g_test_suite_count (GTestSuite *suite)
{
  int n = 0;
  GSList *iter;

  g_return_val_if_fail (suite != NULL, -1);

  for (iter = suite->cases; iter; iter = iter->next)
    {
      GTestCase *tc = iter->data;

      if (strcmp (tc->name, "subprocess") != 0)
        n++;
    }

  for (iter = suite->suites; iter; iter = iter->next)
    {
      GTestSuite *ts = iter->data;

      if (strcmp (ts->name, "subprocess") != 0)
        n += g_test_suite_count (ts);
    }

  return n;
}

/**
 * g_test_run_suite:
 * @suite: a test suite
 *
 * Executes the tests within @suite and all nested test suites.
 *
 * The test suites to be executed are filtered according to
 * test path arguments (`-p testpath` and `-s testpath`) as parsed by
 * [func@GLib.test_init]. See the [func@GLib.test_run] documentation
 * for more information on the order that tests are run in.
 *
 * [func@GLib.test_run_suite] or [func@GLib.test_run] may only be
 * called once in a program.
 *
 * Returns: 0 on success
 *
 * Since: 2.16
 */
int
g_test_run_suite (GTestSuite *suite)
{
  int n_bad = 0;

  g_return_val_if_fail (g_test_run_once == TRUE, -1);

  g_test_run_once = FALSE;
  test_count = g_test_suite_count (suite);

  test_run_name = g_strdup_printf ("/%s", suite->name);
  test_run_name_path = g_build_path (G_DIR_SEPARATOR_S, suite->name, NULL);

  if (test_paths)
    {
      GSList *iter;

      for (iter = test_paths; iter; iter = iter->next)
        n_bad += g_test_run_suite_internal (suite, iter->data);
    }
  else
    n_bad = g_test_run_suite_internal (suite, NULL);

  g_clear_pointer (&test_run_name, g_free);
  g_clear_pointer (&test_run_name_path, g_free);

  return n_bad;
}

/**
 * g_test_case_free:
 * @test_case: a test case
 *
 * Free the @test_case.
 *
 * Since: 2.70
 */
void
g_test_case_free (GTestCase *test_case)
{
  /* In case the test didn’t run (due to being skipped or an error), the test
   * data may still need to be freed, as the client’s main() function may have
   * passed ownership of it into g_test_add_data_func_full() with a
   * #GDestroyNotify. */
  if (test_case->fixture_size == 0 && test_case->fixture_teardown != NULL)
    test_case->fixture_teardown (test_case->test_data, test_case->test_data);

  g_free (test_case->name);
  g_slice_free (GTestCase, test_case);
}

/**
 * g_test_suite_free:
 * @suite: a test suite
 *
 * Frees the @suite and all nested suites.
 *
 * Since: 2.70
 */
void
g_test_suite_free (GTestSuite *suite)
{
  g_slist_free_full (suite->cases, (GDestroyNotify)g_test_case_free);

  g_free (suite->name);

  g_slist_free_full (suite->suites, (GDestroyNotify)g_test_suite_free);

  g_slice_free (GTestSuite, suite);
}

static void
gtest_default_log_handler (const gchar    *log_domain,
                           GLogLevelFlags  log_level,
                           const gchar    *message,
                           gpointer        unused_data)
{
  const gchar *strv[16];
  gboolean fatal = FALSE;
  gchar *msg;
  guint i = 0;

  if (log_domain)
    {
      strv[i++] = log_domain;
      strv[i++] = "-";
    }
  if (log_level & G_LOG_FLAG_FATAL)
    {
      strv[i++] = "FATAL-";
      fatal = TRUE;
    }
  if (log_level & G_LOG_FLAG_RECURSION)
    strv[i++] = "RECURSIVE-";
  if (log_level & G_LOG_LEVEL_ERROR)
    strv[i++] = "ERROR";
  if (log_level & G_LOG_LEVEL_CRITICAL)
    strv[i++] = "CRITICAL";
  if (log_level & G_LOG_LEVEL_WARNING)
    strv[i++] = "WARNING";
  if (log_level & G_LOG_LEVEL_MESSAGE)
    strv[i++] = "MESSAGE";
  if (log_level & G_LOG_LEVEL_INFO)
    strv[i++] = "INFO";
  if (log_level & G_LOG_LEVEL_DEBUG)
    strv[i++] = "DEBUG";
  strv[i++] = ": ";
  strv[i++] = message;
  strv[i++] = NULL;

  msg = g_strjoinv ("", (gchar**) strv);
  g_test_log (fatal ? G_TEST_LOG_ERROR : G_TEST_LOG_MESSAGE, msg, NULL, 0, NULL);
  g_free (msg);

  if (!test_tap_log)
    g_log_default_handler (log_domain, log_level, message, unused_data);
}

void
g_assertion_message (const char     *domain,
                     const char     *file,
                     int             line,
                     const char     *func,
                     const char     *message)
{
  char lstr[32];
  char *s;

  if (!message)
    message = "code should not be reached";
  g_snprintf (lstr, 32, "%d", line);
  s = g_strconcat (domain ? domain : "", domain && domain[0] ? ":" : "",
                   "ERROR:", file, ":", lstr, ":",
                   func, func[0] ? ":" : "",
                   " ", message, NULL);
  g_printerr ("**\n%s\n", s);

  /* Don't print a fatal error indication if assertions are non-fatal, or
   * if we are a child process that might be sharing the parent's stdout. */
  if (test_nonfatal_assertions || test_in_subprocess || test_in_forked_child)
    g_test_log (G_TEST_LOG_MESSAGE, s, NULL, 0, NULL);
  else
    g_test_log (G_TEST_LOG_ERROR, s, NULL, 0, NULL);

  if (test_nonfatal_assertions)
    {
      g_free (s);
      g_test_fail ();
      return;
    }

  /* store assertion message in global variable, so that it can be found in a
   * core dump */
  if (__glib_assert_msg != NULL)
    /* free the old one */
    free (__glib_assert_msg);
  __glib_assert_msg = (char*) malloc (strlen (s) + 1);
  strcpy (__glib_assert_msg, s);

  g_free (s);

  if (test_in_subprocess)
    {
      /* If this is a test case subprocess then it probably hit this
       * assertion on purpose, so just exit() rather than abort()ing,
       * to avoid triggering any system crash-reporting daemon.
       */
      _exit (1);
    }
  else
    g_abort ();
}

/**
 * g_assertion_message_expr: (skip)
 * @domain: (nullable): log domain
 * @file: file containing the assertion
 * @line: line number of the assertion
 * @func: function containing the assertion
 * @expr: (nullable): expression which failed
 *
 * Internal function used to print messages from the public
 * g_assert() and g_assert_not_reached() macros.
 */
void
g_assertion_message_expr (const char     *domain,
                          const char     *file,
                          int             line,
                          const char     *func,
                          const char     *expr)
{
  char *s;
  if (!expr)
    s = g_strdup ("code should not be reached");
  else
    s = g_strconcat ("assertion failed: (", expr, ")", NULL);
  g_assertion_message (domain, file, line, func, s);
  g_free (s);

  /* Normally g_assertion_message() won't return, but we need this for
   * when test_nonfatal_assertions is set, since
   * g_assertion_message_expr() is used for always-fatal assertions.
   */
  if (test_in_subprocess)
    _exit (1);
  else
    g_abort ();
}

void
g_assertion_message_cmpint (const char     *domain,
                            const char     *file,
                            int             line,
                            const char     *func,
                            const char     *expr,
                            guint64         arg1,
                            const char     *cmp,
                            guint64         arg2,
                            char            numtype)
{
  char *s = NULL;

  switch (numtype)
    {
    case 'i':
      s = g_strdup_printf ("assertion failed (%s): "
                           "(%" PRIi64 " %s %" PRIi64 ")",
                           expr, (int64_t) arg1, cmp, (int64_t) arg2);
      break;
    case 'u':
      s = g_strdup_printf ("assertion failed (%s): "
                           "(%" PRIu64 " %s %" PRIu64 ")",
                           expr, (uint64_t) arg1, cmp, (uint64_t) arg2);
      break;
    case 'x':
      s = g_strdup_printf ("assertion failed (%s): "
                           "(0x%08" PRIx64 " %s 0x%08" PRIx64 ")",
                           expr, (uint64_t) arg1, cmp, (uint64_t) arg2);
      break;
    default:
      g_assert_not_reached ();
    }
  g_assertion_message (domain, file, line, func, s);
  g_free (s);
}

void
g_assertion_message_cmpnum (const char     *domain,
                            const char     *file,
                            int             line,
                            const char     *func,
                            const char     *expr,
                            long double     arg1,
                            const char     *cmp,
                            long double     arg2,
                            char            numtype)
{
  char *s = NULL;

  switch (numtype)
    {
    case 'f':   s = g_strdup_printf ("assertion failed (%s): (%.9g %s %.9g)", expr, (double) arg1, cmp, (double) arg2); break;
      /* ideally use: floats=%.7g double=%.17g */
    case 'i':
    case 'x':
      /* Backwards compatibility to apps compiled before 2.78 */
      g_assertion_message_cmpint (domain, file, line, func, expr,
                                  (guint64) arg1, cmp, (guint64) arg2, numtype);
      break;
    default:
      g_assert_not_reached ();
    }
  g_assertion_message (domain, file, line, func, s);
  g_free (s);
}

void
g_assertion_message_cmpstr (const char     *domain,
                            const char     *file,
                            int             line,
                            const char     *func,
                            const char     *expr,
                            const char     *arg1,
                            const char     *cmp,
                            const char     *arg2)
{
  char *a1, *a2, *s, *t1 = NULL, *t2 = NULL;
  a1 = arg1 ? g_strconcat ("\"", t1 = g_strescape (arg1, NULL), "\"", NULL) : g_strdup ("NULL");
  a2 = arg2 ? g_strconcat ("\"", t2 = g_strescape (arg2, NULL), "\"", NULL) : g_strdup ("NULL");
  g_free (t1);
  g_free (t2);
  s = g_strdup_printf ("assertion failed (%s): (%s %s %s)", expr, a1, cmp, a2);
  g_free (a1);
  g_free (a2);
  g_assertion_message (domain, file, line, func, s);
  g_free (s);
}

void
g_assertion_message_cmpstrv (const char         *domain,
                             const char         *file,
                             int                 line,
                             const char         *func,
                             const char         *expr,
                             const char * const *arg1,
                             const char * const *arg2,
                             gsize               first_wrong_idx)
{
  const char *s1 = arg1[first_wrong_idx], *s2 = arg2[first_wrong_idx];
  char *a1, *a2, *s, *t1 = NULL, *t2 = NULL;

  a1 = g_strconcat ("\"", t1 = g_strescape (s1, NULL), "\"", NULL);
  a2 = g_strconcat ("\"", t2 = g_strescape (s2, NULL), "\"", NULL);
  g_free (t1);
  g_free (t2);
  s = g_strdup_printf ("assertion failed (%s): first differing element at index %" G_GSIZE_FORMAT ": %s does not equal %s",
                       expr, first_wrong_idx, a1, a2);
  g_free (a1);
  g_free (a2);
  g_assertion_message (domain, file, line, func, s);
  g_free (s);
}

void
g_assertion_message_error (const char     *domain,
			   const char     *file,
			   int             line,
			   const char     *func,
			   const char     *expr,
			   const GError   *error,
			   GQuark          error_domain,
			   int             error_code)
{
  GString *gstring;

  /* This is used by both g_assert_error() and g_assert_no_error(), so there
   * are three cases: expected an error but got the wrong error, expected
   * an error but got no error, and expected no error but got an error.
   */

  gstring = g_string_new ("assertion failed ");
  if (error_domain)
      g_string_append_printf (gstring, "(%s == (%s, %d)): ", expr,
			      g_quark_to_string (error_domain), error_code);
  else
    g_string_append_printf (gstring, "(%s == NULL): ", expr);

  if (error)
      g_string_append_printf (gstring, "%s (%s, %d)", error->message,
			      g_quark_to_string (error->domain), error->code);
  else
    g_string_append_printf (gstring, "%s is NULL", expr);

  g_assertion_message (domain, file, line, func, gstring->str);
  g_string_free (gstring, TRUE);
}

/**
 * g_strcmp0:
 * @str1: (nullable): a string
 * @str2: (nullable): another string
 *
 * Compares @str1 and @str2 like `strcmp()`.
 *
 * Handles `NULL` gracefully by sorting it before non-`NULL` strings.
 * Comparing two `NULL` pointers returns 0.
 *
 * Returns: an integer less than, equal to, or greater than zero,
 *   if @str1 is <, == or > than @str2
 *
 * Since: 2.16
 */
int
g_strcmp0 (const char     *str1,
           const char     *str2)
{
  if (!str1)
    return -(str1 != str2);
  if (!str2)
    return str1 != str2;
  return strcmp (str1, str2);
}

static void
test_trap_clear (void)
{
  test_trap_last_status = 0;
  test_trap_last_pid = 0;
  g_clear_pointer (&test_trap_last_subprocess, g_free);
  g_clear_pointer (&test_trap_last_stdout, g_free);
  g_clear_pointer (&test_trap_last_stderr, g_free);
}

#ifdef G_OS_UNIX

static int
safe_dup2 (int fd1,
           int fd2)
{
  int ret;
  do
    ret = dup2 (fd1, fd2);
  while (ret < 0 && errno == EINTR);
  return ret;
}

#endif

typedef struct {
  GPid pid;
  GMainLoop *loop;
  int child_status;  /* unmodified platform-specific status */

  GIOChannel *stdout_io;
  gboolean echo_stdout;
  GString *stdout_str;

  GIOChannel *stderr_io;
  gboolean echo_stderr;
  GString *stderr_str;
} WaitForChildData;

static void
check_complete (WaitForChildData *data)
{
  if (data->child_status != -1 && data->stdout_io == NULL && data->stderr_io == NULL)
    g_main_loop_quit (data->loop);
}

static void
child_exited (GPid     pid,
              gint     status,
              gpointer user_data)
{
  WaitForChildData *data = user_data;

  g_assert (status != -1);
  data->child_status = status;

  check_complete (data);
}

static gboolean
child_timeout (gpointer user_data)
{
  WaitForChildData *data = user_data;

#ifdef G_OS_WIN32
  TerminateProcess (data->pid, G_TEST_STATUS_TIMED_OUT);
#else
  kill (data->pid, SIGALRM);
#endif

  return FALSE;
}

static gboolean
child_read (GIOChannel *io, GIOCondition cond, gpointer user_data)
{
  WaitForChildData *data = user_data;
  GIOStatus status;
  gsize nread, nwrote, total;
  gchar buf[4096];
  FILE *echo_file = NULL;

  status = g_io_channel_read_chars (io, buf, sizeof (buf), &nread, NULL);
  if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
    {
      // FIXME data->error = (status == G_IO_STATUS_ERROR);
      if (io == data->stdout_io)
        g_clear_pointer (&data->stdout_io, g_io_channel_unref);
      else
        g_clear_pointer (&data->stderr_io, g_io_channel_unref);

      check_complete (data);
      return FALSE;
    }
  else if (status == G_IO_STATUS_AGAIN)
    return TRUE;

  if (io == data->stdout_io)
    {
      g_string_append_len (data->stdout_str, buf, nread);
      if (data->echo_stdout)
        {
          if G_UNLIKELY (!test_tap_log)
            echo_file = stdout;
        }
    }
  else
    {
      g_string_append_len (data->stderr_str, buf, nread);
      if (data->echo_stderr)
        echo_file = stderr;
    }

  if (echo_file)
    {
      for (total = 0; total < nread; total += nwrote)
        {
          int errsv;

          nwrote = fwrite (buf + total, 1, nread - total, echo_file);
          errsv = errno;
          if (nwrote == 0)
            g_error ("write failed: %s", g_strerror (errsv));
        }
    }

  return TRUE;
}

static void
wait_for_child (GPid pid,
                int stdout_fd, gboolean echo_stdout,
                int stderr_fd, gboolean echo_stderr,
                guint64 timeout)
{
  WaitForChildData data;
  GMainContext *context;
  GSource *source;

  data.pid = pid;
  data.child_status = -1;

  context = g_main_context_new ();
  data.loop = g_main_loop_new (context, FALSE);

  source = g_child_watch_source_new (pid);
  g_source_set_callback (source, (GSourceFunc) child_exited, &data, NULL);
  g_source_attach (source, context);
  g_source_unref (source);

  data.echo_stdout = echo_stdout;
  data.stdout_str = g_string_new (NULL);
  data.stdout_io = g_io_channel_unix_new (stdout_fd);
  g_io_channel_set_close_on_unref (data.stdout_io, TRUE);
  g_io_channel_set_encoding (data.stdout_io, NULL, NULL);
  g_io_channel_set_buffered (data.stdout_io, FALSE);
  source = g_io_create_watch (data.stdout_io, G_IO_IN | G_IO_ERR | G_IO_HUP);
  g_source_set_callback (source, (GSourceFunc) child_read, &data, NULL);
  g_source_attach (source, context);
  g_source_unref (source);

  data.echo_stderr = echo_stderr;
  data.stderr_str = g_string_new (NULL);
  data.stderr_io = g_io_channel_unix_new (stderr_fd);
  g_io_channel_set_close_on_unref (data.stderr_io, TRUE);
  g_io_channel_set_encoding (data.stderr_io, NULL, NULL);
  g_io_channel_set_buffered (data.stderr_io, FALSE);
  source = g_io_create_watch (data.stderr_io, G_IO_IN | G_IO_ERR | G_IO_HUP);
  g_source_set_callback (source, (GSourceFunc) child_read, &data, NULL);
  g_source_attach (source, context);
  g_source_unref (source);

  if (timeout)
    {
      source = g_timeout_source_new (0);
      g_source_set_ready_time (source, g_get_monotonic_time () + timeout);
      g_source_set_callback (source, (GSourceFunc) child_timeout, &data, NULL);
      g_source_attach (source, context);
      g_source_unref (source);
    }

  g_main_loop_run (data.loop);
  g_main_loop_unref (data.loop);
  g_main_context_unref (context);

  if (echo_stdout && test_tap_log && data.stdout_str->len > 0)
    {
      gboolean added_newline = FALSE;

      if (data.stdout_str->str[data.stdout_str->len - 1] != '\n')
        {
          g_string_append_c (data.stdout_str, '\n');
          added_newline = TRUE;
        }

      g_test_print_handler_full (data.stdout_str->str, TRUE, TRUE, 1);

      if (added_newline)
        g_string_truncate (data.stdout_str, data.stdout_str->len - 1);
    }

  test_trap_last_pid = pid;
  test_trap_last_status = data.child_status;
  test_trap_last_stdout = g_string_free (data.stdout_str, FALSE);
  test_trap_last_stderr = g_string_free (data.stderr_str, FALSE);

  g_clear_pointer (&data.stdout_io, g_io_channel_unref);
  g_clear_pointer (&data.stderr_io, g_io_channel_unref);
}

/**
 * g_test_trap_fork:
 * @usec_timeout: timeout for the forked test in microseconds
 * @test_trap_flags: flags to modify forking behaviour
 *
 * Forks the current test program to execute a test case that might
 * not return or that might abort.
 *
 * If @usec_timeout is non-0, the forked test case is aborted and
 * considered failing if its run time exceeds it.
 *
 * The forking behavior can be configured with [flags@GLib.TestTrapFlags]
 * flags.
 *
 * In the following example, the test code forks, the forked child
 * process produces some sample output and exits successfully.
 * The forking parent process then asserts successful child program
 * termination and validates child program outputs.
 *
 * ```c
 *   static void
 *   test_fork_patterns (void)
 *   {
 *     if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
 *       {
 *         g_print ("some stdout text: somagic17
");
 *         g_printerr ("some stderr text: semagic43
");
 *         exit (0); // successful test run
 *       }
 *     g_test_trap_assert_passed ();
 *     g_test_trap_assert_stdout ("*somagic17*");
 *     g_test_trap_assert_stderr ("*semagic43*");
 *   }
 * ```
 *
 * Returns: true for the forked child and false for the executing parent process.
 *
 * Since: 2.16
 *
 * Deprecated: This function is implemented only on Unix platforms,
 * is not always reliable due to problems inherent in fork-without-exec
 * and doesn't set close-on-exec flag on its file descriptors.
 * Use func@GLib.test_trap_subprocess] instead.
 */
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
gboolean
g_test_trap_fork (guint64        usec_timeout,
                  GTestTrapFlags test_trap_flags)
{
#ifdef G_OS_UNIX
  int stdout_pipe[2] = { -1, -1 };
  int stderr_pipe[2] = { -1, -1 };
  int errsv;

  test_trap_clear();
  if (pipe (stdout_pipe) < 0 || pipe (stderr_pipe) < 0)
    {
      errsv = errno;
      g_error ("failed to create pipes to fork test program: %s", g_strerror (errsv));
    }
  test_trap_last_pid = fork ();
  errsv = errno;
  if (test_trap_last_pid < 0)
    g_error ("failed to fork test program: %s", g_strerror (errsv));
  if (test_trap_last_pid == 0)  /* child */
    {
      int fd0 = -1;
      test_in_forked_child = TRUE;
      close (stdout_pipe[0]);
      close (stderr_pipe[0]);
      if (!(test_trap_flags & G_TEST_TRAP_INHERIT_STDIN))
        {
          fd0 = g_open ("/dev/null", O_RDONLY, 0);
          if (fd0 < 0)
            g_error ("failed to open /dev/null for stdin redirection");
        }
      if (safe_dup2 (stdout_pipe[1], 1) < 0 || safe_dup2 (stderr_pipe[1], 2) < 0 || (fd0 >= 0 && safe_dup2 (fd0, 0) < 0))
        {
          errsv = errno;
          g_error ("failed to dup2() in forked test program: %s", g_strerror (errsv));
        }
      if (fd0 >= 3)
        close (fd0);
      if (stdout_pipe[1] >= 3)
        close (stdout_pipe[1]);
      if (stderr_pipe[1] >= 3)
        close (stderr_pipe[1]);

      /* We typically expect these child processes to crash, and some
       * tests spawn a *lot* of them.  Avoid spamming system crash
       * collection programs such as systemd-coredump and abrt.
       */
      g_test_disable_crash_reporting ();

      return TRUE;
    }
  else                          /* parent */
    {
      test_run_forks++;
      close (stdout_pipe[1]);
      close (stderr_pipe[1]);

      wait_for_child (test_trap_last_pid,
                      stdout_pipe[0], !(test_trap_flags & G_TEST_TRAP_SILENCE_STDOUT),
                      stderr_pipe[0], !(test_trap_flags & G_TEST_TRAP_SILENCE_STDERR),
                      usec_timeout);
      return FALSE;
    }
#else
  g_message ("Not implemented: g_test_trap_fork");

  return FALSE;
#endif
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * g_test_trap_subprocess:
 * @test_path: (nullable): test to run in a subprocess
 * @usec_timeout: timeout for the subprocess test in microseconds.
 * @test_flags: flags to modify subprocess behaviour
 *
 * Respawns the test program to run only @test_path in a subprocess.
 *
 * This is equivalent to calling [func@GLib.test_trap_subprocess_with_envp]
 * with `envp` set to `NULL`. See the documentation for that function
 * for full details.
 *
 * Since: 2.38
 */
void
g_test_trap_subprocess (const char           *test_path,
                        guint64               usec_timeout,
                        GTestSubprocessFlags  test_flags)
{
  g_test_trap_subprocess_with_envp (test_path, NULL, usec_timeout, test_flags);
}

/**
 * g_test_trap_subprocess_with_envp:
 * @test_path: (nullable): test to run in a subprocess
 * @envp: (array zero-terminated=1) (nullable) (element-type filename): environment
 *   to run the test in
 * @usec_timeout: timeout for the subprocess test in microseconds
 * @test_flags: flags to modify subprocess behaviour
 *
 * Respawns the test program to run only @test_path in a subprocess with
 * a given environment.
 *
 * This can be used for a test case that might not return, or that
 * might abort.
 *
 * If @test_path is `NULL` then the same test is re-run in a subprocess.
 * You can use [func@GLib.test_subprocess] to determine whether the test
 * is in a subprocess or not.
 *
 * @test_path can also be the name of the parent test, followed by
 * "`/subprocess/`" and then a name for the specific subtest (or just
 * ending with "`/subprocess`" if the test only has one child test);
 * tests with names of this form will automatically be skipped in the
 * parent process.
 *
 * If @envp is `NULL`, the parent process’ environment will be inherited.
 *
 * If @usec_timeout is non-0, the test subprocess is aborted and
 * considered failing if its run time exceeds it.
 *
 * The subprocess behavior can be configured with [flags@GLib.TestSubprocessFlags]
 * flags.
 *
 * You can use methods such as [func@GLib.test_trap_assert_passed],
 * [func@GLib.test_trap_assert_failed], and [func@GLib.test_trap_assert_stderr] to
 * check the results of the subprocess. (But note that
 * [func@GLib.test_trap_assert_stdout] and [func@GLib.test_trap_assert_stderr]
 * cannot be used if @test_flags specifies that the child should
 * inherit the parent stdout/stderr.)
 *
 * If your `main ()` needs to behave differently in the subprocess, you can
 * call [func@GLib.test_subprocess] (after calling [func@GLib.test_init])
 * to see whether you are in a subprocess.
 *
 * Internally, this function tracks the child process using
 * [func@GLib.child_watch_source_new], so your process must not ignore
 * `SIGCHLD`, and must not attempt to watch or wait for the child process
 * via another mechanism.
 *
 * The following example tests that calling `my_object_new(1000000)` will
 * abort with an error message.
 *
 * ```c
 *   static void
 *   test_create_large_object (void)
 *   {
 *     if (g_test_subprocess ())
 *       {
 *         my_object_new (1000000);
 *         return;
 *       }
 *
 *     // Reruns this same test in a subprocess
 *     g_test_trap_subprocess (NULL, 0, G_TEST_SUBPROCESS_DEFAULT);
 *     g_test_trap_assert_failed ();
 *     g_test_trap_assert_stderr ("*ERROR*too large*");
 *   }
 *
 *   static void
 *   test_different_username (void)
 *   {
 *     if (g_test_subprocess ())
 *       {
 *         // Code under test goes here
 *         g_message ("Username is now simulated as %s", g_getenv ("USER"));
 *         return;
 *       }
 *
 *     // Reruns this same test in a subprocess
 *     g_autoptr(GStrv) envp = g_get_environ ();
 *     envp = g_environ_setenv (g_steal_pointer (&envp), "USER", "charlie", TRUE);
 *     g_test_trap_subprocess_with_envp (NULL, envp, 0, G_TEST_SUBPROCESS_DEFAULT);
 *     g_test_trap_assert_passed ();
 *     g_test_trap_assert_stdout ("Username is now simulated as charlie");
 *   }
 *
 *   int
 *   main (int argc, char **argv)
 *   {
 *     g_test_init (&argc, &argv, NULL);
 *
 *     g_test_add_func ("/myobject/create-large-object",
 *                      test_create_large_object);
 *     g_test_add_func ("/myobject/different-username",
 *                      test_different_username);
 *     return g_test_run ();
 *   }
 * ```
 *
 * Since: 2.80
 */
void
g_test_trap_subprocess_with_envp (const char           *test_path,
                                  const char * const   *envp,
                                  guint64               usec_timeout,
                                  GTestSubprocessFlags  test_flags)
{
  GError *error = NULL;
  GPtrArray *argv;
  GSpawnFlags flags;
  int stdout_fd, stderr_fd;
  GPid pid;

  /* Sanity check that they used GTestSubprocessFlags, not GTestTrapFlags */
  g_assert ((test_flags & (G_TEST_TRAP_INHERIT_STDIN | G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR)) == 0);

  if (test_path)
    {
      if (!g_test_suite_case_exists (g_test_get_root (), test_path))
        g_error ("g_test_trap_subprocess: test does not exist: %s", test_path);
    }
  else
    {
      test_path = test_run_name;
    }

  if (g_test_verbose ())
    {
      if (test_tap_log)
        g_print ("subprocess: %s\n", test_path);
      else
        g_print ("GTest: subprocess: %s\n", test_path);
    }

  test_trap_clear ();
  test_trap_last_subprocess = g_strdup (test_path);

  if (test_argv0 == NULL)
    g_error ("g_test_trap_subprocess() requires argv0 to be passed to g_test_init()");

  argv = g_ptr_array_new ();
  g_ptr_array_add (argv, (char *) test_argv0);
  g_ptr_array_add (argv, "-q");
  g_ptr_array_add (argv, "-p");
  g_ptr_array_add (argv, (char *)test_path);
  g_ptr_array_add (argv, "--GTestSubprocess");
  if (test_log_fd != -1)
    {
      char log_fd_buf[128];

      g_ptr_array_add (argv, "--GTestLogFD");
      g_snprintf (log_fd_buf, sizeof (log_fd_buf), "%d", test_log_fd);
      g_ptr_array_add (argv, log_fd_buf);
    }
  g_ptr_array_add (argv, NULL);

  flags = G_SPAWN_DO_NOT_REAP_CHILD;
  if (test_log_fd != -1)
    flags |= G_SPAWN_LEAVE_DESCRIPTORS_OPEN;
  if (test_flags & G_TEST_TRAP_INHERIT_STDIN)
    flags |= G_SPAWN_CHILD_INHERITS_STDIN;

  if (!g_spawn_async_with_pipes (test_initial_cwd,
                                 (char **)argv->pdata,
                                 (char **) envp, flags,
                                 NULL, NULL,
                                 &pid, NULL, &stdout_fd, &stderr_fd,
                                 &error))
    {
      g_error ("g_test_trap_subprocess() failed: %s",
               error->message);
    }
  g_ptr_array_free (argv, TRUE);

  wait_for_child (pid,
                  stdout_fd, !!(test_flags & G_TEST_SUBPROCESS_INHERIT_STDOUT),
                  stderr_fd, !!(test_flags & G_TEST_SUBPROCESS_INHERIT_STDERR),
                  usec_timeout);
}

/**
 * g_test_subprocess:
 *
 * Returns true if the test program is running under [func@GLib.test_trap_subprocess].
 *
 * Returns: true if the test program is running under [func@GLib.test_trap_subprocess]
 *
 * Since: 2.38
 */
gboolean
g_test_subprocess (void)
{
  return test_in_subprocess;
}

/**
 * g_test_trap_has_passed:
 *
 * Checks the result of the last [func@GLib.test_trap_subprocess] call.
 *
 * Returns: true if the last test subprocess terminated successfully
 *
 * Since: 2.16
 */
gboolean
g_test_trap_has_passed (void)
{
#ifdef G_OS_UNIX
  return (WIFEXITED (test_trap_last_status) &&
      WEXITSTATUS (test_trap_last_status) == 0);
#else
  return test_trap_last_status == 0;
#endif
}

/**
 * g_test_trap_reached_timeout:
 *
 * Checks the result of the last [func@GLib.test_trap_subprocess] call.
 *
 * Returns: true if the last test subprocess got killed due to a timeout
 *
 * Since: 2.16
 */
gboolean
g_test_trap_reached_timeout (void)
{
#ifdef G_OS_UNIX
  return (WIFSIGNALED (test_trap_last_status) &&
      WTERMSIG (test_trap_last_status) == SIGALRM);
#else
  return test_trap_last_status == G_TEST_STATUS_TIMED_OUT;
#endif
}

static gboolean
log_child_output (const gchar *process_id)
{
  gchar *escaped;

#ifdef G_OS_UNIX
  if (WIFEXITED (test_trap_last_status)) /* normal exit */
    {
      if (WEXITSTATUS (test_trap_last_status) == 0)
        g_test_message ("child process (%s) exit status: 0 (success)",
            process_id);
      else
        g_test_message ("child process (%s) exit status: %d (error)",
            process_id, WEXITSTATUS (test_trap_last_status));
    }
  else if (WIFSIGNALED (test_trap_last_status) &&
      WTERMSIG (test_trap_last_status) == SIGALRM)
    {
      g_test_message ("child process (%s) timed out", process_id);
    }
  else if (WIFSIGNALED (test_trap_last_status))
    {
      const gchar *maybe_dumped_core = "";

#ifdef WCOREDUMP
      if (WCOREDUMP (test_trap_last_status))
        maybe_dumped_core = ", core dumped";
#endif

      g_test_message ("child process (%s) killed by signal %d (%s)%s",
          process_id, WTERMSIG (test_trap_last_status),
          g_strsignal (WTERMSIG (test_trap_last_status)),
          maybe_dumped_core);
    }
  else
    {
      g_test_message ("child process (%s) unknown wait status %d",
          process_id, test_trap_last_status);
    }
#else
  if (test_trap_last_status == 0)
    g_test_message ("child process (%s) exit status: 0 (success)",
        process_id);
  else
    g_test_message ("child process (%s) exit status: %d (error)",
        process_id, test_trap_last_status);
#endif

  escaped = g_strescape (test_trap_last_stdout, NULL);
  g_test_message ("child process (%s) stdout: \"%s\"", process_id, escaped);
  g_free (escaped);

  escaped = g_strescape (test_trap_last_stderr, NULL);
  g_test_message ("child process (%s) stderr: \"%s\"", process_id, escaped);
  g_free (escaped);

  /* so we can use short-circuiting:
   * logged_child_output = logged_child_output || log_child_output (...) */
  return TRUE;
}

void
g_test_trap_assertions (const char     *domain,
                        const char     *file,
                        int             line,
                        const char     *func,
                        guint64         assertion_flags, /* 0-pass, 1-fail, 2-outpattern, 4-errpattern */
                        const char     *pattern)
{
  gboolean must_pass = assertion_flags == 0;
  gboolean must_fail = assertion_flags == 1;
  gboolean match_result = 0 == (assertion_flags & 1);
  gboolean logged_child_output = FALSE;
  const char *stdout_pattern = (assertion_flags & 2) ? pattern : NULL;
  const char *stderr_pattern = (assertion_flags & 4) ? pattern : NULL;
  const char *match_error = match_result ? "failed to match" : "contains invalid match";
  char *process_id;

#ifdef G_OS_UNIX
  if (test_trap_last_subprocess != NULL)
    {
      process_id = g_strdup_printf ("%s [%d]", test_trap_last_subprocess,
                                    test_trap_last_pid);
    }
  else if (test_trap_last_pid != 0)
    process_id = g_strdup_printf ("%d", test_trap_last_pid);
#else
  if (test_trap_last_subprocess != NULL)
    process_id = g_strdup (test_trap_last_subprocess);
#endif
  else
    g_error ("g_test_trap_ assertion with no trapped test");

  if (must_pass && !g_test_trap_has_passed())
    {
      char *msg;

      logged_child_output = logged_child_output || log_child_output (process_id);

      msg = g_strdup_printf ("child process (%s) failed unexpectedly", process_id);
      g_assertion_message (domain, file, line, func, msg);
      g_free (msg);
    }
  if (must_fail && g_test_trap_has_passed())
    {
      char *msg;

      logged_child_output = logged_child_output || log_child_output (process_id);

      msg = g_strdup_printf ("child process (%s) did not fail as expected", process_id);
      g_assertion_message (domain, file, line, func, msg);
      g_free (msg);
    }
  if (stdout_pattern && match_result == !g_pattern_match_simple (stdout_pattern, test_trap_last_stdout))
    {
      char *msg;

      logged_child_output = logged_child_output || log_child_output (process_id);

      g_test_message ("stdout was:\n%s", test_trap_last_stdout);

      msg = g_strdup_printf ("stdout of child process (%s) %s: %s",
                             process_id, match_error, stdout_pattern);
      g_assertion_message (domain, file, line, func, msg);
      g_free (msg);
    }
  if (stderr_pattern && match_result == !g_pattern_match_simple (stderr_pattern, test_trap_last_stderr))
    {
      char *msg;

      logged_child_output = logged_child_output || log_child_output (process_id);

      g_test_message ("stderr was:\n%s", test_trap_last_stderr);

      msg = g_strdup_printf ("stderr of child process (%s) %s: %s",
                             process_id, match_error, stderr_pattern);
      g_assertion_message (domain, file, line, func, msg);
      g_free (msg);
    }

  (void) logged_child_output;  /* shut up scan-build about the final unread assignment */

  g_free (process_id);
}

static void
gstring_overwrite_int (GString *gstring,
                       guint    pos,
                       guint32  vuint)
{
  vuint = g_htonl (vuint);
  g_string_overwrite_len (gstring, pos, (const gchar*) &vuint, 4);
}

static void
gstring_append_int (GString *gstring,
                    guint32  vuint)
{
  vuint = g_htonl (vuint);
  g_string_append_len (gstring, (const gchar*) &vuint, 4);
}

static void
gstring_append_double (GString *gstring,
                       double   vdouble)
{
  union { double vdouble; guint64 vuint64; } u;
  u.vdouble = vdouble;
  u.vuint64 = GUINT64_TO_BE (u.vuint64);
  g_string_append_len (gstring, (const gchar*) &u.vuint64, 8);
}

static guint8*
g_test_log_dump (GTestLogMsg *msg,
                 guint       *len)
{
  GString *gstring = g_string_sized_new (1024);
  guint ui;
  gstring_append_int (gstring, 0);              /* message length */
  gstring_append_int (gstring, msg->log_type);
  gstring_append_int (gstring, msg->n_strings);
  gstring_append_int (gstring, msg->n_nums);
  gstring_append_int (gstring, 0);      /* reserved */
  for (ui = 0; ui < msg->n_strings; ui++)
    {
      guint l;
      g_assert (msg->strings[ui] != NULL);
      l = strlen (msg->strings[ui]);
      gstring_append_int (gstring, l);
      g_string_append_len (gstring, msg->strings[ui], l);
    }
  for (ui = 0; ui < msg->n_nums; ui++)
    gstring_append_double (gstring, (gdouble) msg->nums[ui]);
  *len = gstring->len;
  gstring_overwrite_int (gstring, 0, *len);     /* message length */
  return (guint8*) g_string_free (gstring, FALSE);
}

static inline long double
net_double (const gchar **ipointer)
{
  union { guint64 vuint64; double vdouble; } u;
  guint64 aligned_int64;
  memcpy (&aligned_int64, *ipointer, 8);
  *ipointer += 8;
  u.vuint64 = GUINT64_FROM_BE (aligned_int64);
  return u.vdouble;
}

static inline guint32
net_int (const gchar **ipointer)
{
  guint32 aligned_int;
  memcpy (&aligned_int, *ipointer, 4);
  *ipointer += 4;
  return g_ntohl (aligned_int);
}

static gboolean
g_test_log_extract (GTestLogBuffer *tbuffer)
{
  const gchar *p = tbuffer->data->str;
  GTestLogMsg msg;
  guint mlength;
  if (tbuffer->data->len < 4 * 5)
    return FALSE;
  mlength = net_int (&p);
  if (tbuffer->data->len < mlength)
    return FALSE;
  msg.log_type = net_int (&p);
  msg.n_strings = net_int (&p);
  msg.n_nums = net_int (&p);
  if (net_int (&p) == 0)
    {
      guint ui;
      msg.strings = g_new0 (gchar*, msg.n_strings + 1);
      msg.nums = g_new0 (long double, msg.n_nums);
      for (ui = 0; ui < msg.n_strings; ui++)
        {
          guint sl = net_int (&p);
          msg.strings[ui] = g_strndup (p, sl);
          p += sl;
        }
      for (ui = 0; ui < msg.n_nums; ui++)
        msg.nums[ui] = net_double (&p);
      if (p <= tbuffer->data->str + mlength)
        {
          g_string_erase (tbuffer->data, 0, mlength);
          tbuffer->msgs = g_slist_prepend (tbuffer->msgs, g_memdup2 (&msg, sizeof (msg)));
          return TRUE;
        }

      g_free (msg.nums);
      g_strfreev (msg.strings);
    }

  g_error ("corrupt log stream from test program");
  return FALSE;
}

/**
 * g_test_log_buffer_new:
 *
 * Internal function for gtester to decode test log messages, no ABI guarantees provided.
 */
GTestLogBuffer*
g_test_log_buffer_new (void)
{
  GTestLogBuffer *tb = g_new0 (GTestLogBuffer, 1);
  tb->data = g_string_sized_new (1024);
  return tb;
}

/**
 * g_test_log_buffer_free:
 *
 * Internal function for gtester to free test log messages, no ABI guarantees provided.
 */
void
g_test_log_buffer_free (GTestLogBuffer *tbuffer)
{
  g_return_if_fail (tbuffer != NULL);
  while (tbuffer->msgs)
    g_test_log_msg_free (g_test_log_buffer_pop (tbuffer));
  g_string_free (tbuffer->data, TRUE);
  g_free (tbuffer);
}

/**
 * g_test_log_buffer_push:
 *
 * Internal function for gtester to decode test log messages, no ABI guarantees provided.
 */
void
g_test_log_buffer_push (GTestLogBuffer *tbuffer,
                        guint           n_bytes,
                        const guint8   *bytes)
{
  g_return_if_fail (tbuffer != NULL);
  if (n_bytes)
    {
      gboolean more_messages;
      g_return_if_fail (bytes != NULL);
      g_string_append_len (tbuffer->data, (const gchar*) bytes, n_bytes);
      do
        more_messages = g_test_log_extract (tbuffer);
      while (more_messages);
    }
}

/**
 * g_test_log_buffer_pop:
 *
 * Internal function for gtester to retrieve test log messages, no ABI guarantees provided.
 */
GTestLogMsg*
g_test_log_buffer_pop (GTestLogBuffer *tbuffer)
{
  GTestLogMsg *msg = NULL;
  g_return_val_if_fail (tbuffer != NULL, NULL);
  if (tbuffer->msgs)
    {
      GSList *slist = g_slist_last (tbuffer->msgs);
      msg = slist->data;
      tbuffer->msgs = g_slist_delete_link (tbuffer->msgs, slist);
    }
  return msg;
}

/**
 * g_test_log_msg_free:
 *
 * Internal function for gtester to free test log messages, no ABI guarantees provided.
 */
void
g_test_log_msg_free (GTestLogMsg *tmsg)
{
  g_return_if_fail (tmsg != NULL);
  g_strfreev (tmsg->strings);
  g_free (tmsg->nums);
  g_free (tmsg);
}

static gchar *
g_test_build_filename_va (GTestFileType  file_type,
                          const gchar   *first_path,
                          va_list        ap)
{
  const gchar *pathv[16];
  gsize num_path_segments;

  if (file_type == G_TEST_DIST)
    pathv[0] = test_disted_files_dir;
  else if (file_type == G_TEST_BUILT)
    pathv[0] = test_built_files_dir;
  else
    g_assert_not_reached ();

  pathv[1] = first_path;

  for (num_path_segments = 2; num_path_segments < G_N_ELEMENTS (pathv); num_path_segments++)
    {
      pathv[num_path_segments] = va_arg (ap, const char *);
      if (pathv[num_path_segments] == NULL)
        break;
    }

  g_assert_cmpint (num_path_segments, <, G_N_ELEMENTS (pathv));

  return g_build_filenamev ((gchar **) pathv);
}

/**
 * g_test_build_filename:
 * @file_type: the type of file (built vs. distributed)
 * @first_path: the first segment of the pathname
 * @...: `NULL`-terminated additional path segments
 *
 * Creates the pathname to a data file that is required for a test.
 *
 * This function is conceptually similar to [func@GLib.build_filename]
 * except that the first argument has been replaced with a
 * [enum@GLib.TestFileType] argument.
 *
 * The data file should either have been distributed with the module
 * containing the test ([enum@GLib.TestFileType.dist] or built as part of the
 * buildcsystem of that module ([enum@GLib.TestFileType.built]).
 *
 * In order for this function to work in srcdir != builddir situations,
 * the `G_TEST_SRCDIR` and `G_TEST_BUILDDIR` environment variables need
 * to have been defined. As of 2.38, this is done by the glib.mk that is
 * included in GLib. Please ensure that your copy is up to date before
 * using this function.
 *
 * In case neither variable is set, this function will fall back to
 * using the dirname portion of `argv[0]`, possibly removing ".libs".
 * This allows for casual running of tests directly from the commandline
 * in the srcdir == builddir case and should also support running of
 * installed tests, assuming the data files have been installed in the
 * same relative path as the test binary.
 *
 * Returns: the path of the file, to be freed using [func@GLib.free]
 *
 * Since: 2.38
 **/
/**
 * GTestFileType:
 * @G_TEST_DIST: a file that was included in the distribution tarball
 * @G_TEST_BUILT: a file that was built on the compiling machine
 *
 * The type of file to return the filename for, when used with
 * [func@GLib.test_build_filename].
 *
 * These two options correspond rather directly to the 'dist' and
 * 'built' terminology that automake uses and are explicitly used to
 * distinguish between the 'srcdir' and 'builddir' being separate. All
 * files in your project should either be dist (in the `EXTRA_DIST` or
 * `dist_schema_DATA` sense, in which case they will always be in the
 * srcdir) or built (in the `BUILT_SOURCES` sense, in which case they
 * will always be in the builddir).
 *
 * Note: As a general rule of automake, files that are generated only as
 * part of the build-from-git process (but then are distributed with the
 * tarball) always go in srcdir (even if doing a srcdir != builddir
 * build from git) and are considered as distributed files.
 *
 * The same principles apply for other build systems, such as meson.
 *
 * Since: 2.38
 **/
gchar *
g_test_build_filename (GTestFileType  file_type,
                       const gchar   *first_path,
                       ...)
{
  gchar *result;
  va_list ap;

  g_assert (g_test_initialized ());

  va_start (ap, first_path);
  result = g_test_build_filename_va (file_type, first_path, ap);
  va_end (ap);

  return result;
}

/**
 * g_test_get_dir:
 * @file_type: the type of file (built vs. distributed)
 *
 * Gets the pathname of the directory containing test files of the type
 * specified by @file_type.
 *
 * This is approximately the same as calling `g_test_build_filename(".")`,
 * but you don't need to free the return value.
 *
 * Returns: (type filename): the path of the directory, owned by GLib
 *
 * Since: 2.38
 **/
const gchar *
g_test_get_dir (GTestFileType file_type)
{
  g_assert (g_test_initialized ());

  if (file_type == G_TEST_DIST)
    return test_disted_files_dir;
  else if (file_type == G_TEST_BUILT)
    return test_built_files_dir;

  g_assert_not_reached ();
}

/**
 * g_test_get_filename:
 * @file_type: the type of file (built vs. distributed)
 * @first_path: the first segment of the pathname
 * @...: `NULL`-terminated additional path segments
 *
 * Gets the pathname to a data file that is required for a test.
 *
 * This is the same as [func@GLib.test_build_filename] with two differences.
 * The first difference is that you must only use this function from within
 * a testcase function. The second difference is that you need not free
 * the return value — it will be automatically freed when the testcase
 * finishes running.
 *
 * It is safe to use this function from a thread inside of a testcase
 * but you must ensure that all such uses occur before the main testcase
 * function returns (ie: it is best to ensure that all threads have been
 * joined).
 *
 * Returns: the path, automatically freed at the end of the testcase
 *
 * Since: 2.38
 **/
const gchar *
g_test_get_filename (GTestFileType  file_type,
                     const gchar   *first_path,
                     ...)
{
  gchar *result;
  GSList *node;
  va_list ap;

  g_assert (g_test_initialized ());
  if (test_filename_free_list == NULL)
    g_error ("g_test_get_filename() can only be used within testcase functions");

  va_start (ap, first_path);
  result = g_test_build_filename_va (file_type, first_path, ap);
  va_end (ap);

  node = g_slist_prepend (NULL, result);
  do
    node->next = *test_filename_free_list;
  while (!g_atomic_pointer_compare_and_exchange (test_filename_free_list, node->next, node));

  return result;
}

/**
 * g_test_get_path:
 *
 * Gets the test path for the test currently being run.
 *
 * In essence, it will be the same string passed as the first argument
 * to e.g. [func@GLib.test_add] when the test was added.
 *
 * This function returns a valid string only within a test function.
 *
 * Note that this is a test path, not a file system path.
 *
 * Returns: the test path for the test currently being run
 *
 * Since: 2.68
 **/
const char *
g_test_get_path (void)
{
  return test_run_name;
}
