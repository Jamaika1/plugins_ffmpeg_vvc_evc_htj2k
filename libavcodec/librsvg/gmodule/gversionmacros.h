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
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
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

#ifndef __J_VERSION_MACROS_H__
#define __J_VERSION_MACROS_H__

/*#if !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif*/

/* Version boundaries checks */

#define G_ENCODE_VERSION(major,minor)   ((major) << 16 | (minor) << 8)

/* XXX: Every new stable minor release bump should add a macro here */

/**
 * GMODULE_VERSION_2_26:
 *
 * A macro that evaluates to the 2.26 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.32
 */
#define GMODULE_VERSION_2_26       (G_ENCODE_VERSION (2, 26))

/**
 * GMODULE_VERSION_2_28:
 *
 * A macro that evaluates to the 2.28 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.32
 */
#define GMODULE_VERSION_2_28       (G_ENCODE_VERSION (2, 28))

/**
 * GMODULE_VERSION_2_30:
 *
 * A macro that evaluates to the 2.30 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.32
 */
#define GMODULE_VERSION_2_30       (G_ENCODE_VERSION (2, 30))

/**
 * GMODULE_VERSION_2_32:
 *
 * A macro that evaluates to the 2.32 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.32
 */
#define GMODULE_VERSION_2_32       (G_ENCODE_VERSION (2, 32))

/**
 * GMODULE_VERSION_2_34:
 *
 * A macro that evaluates to the 2.34 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.34
 */
#define GMODULE_VERSION_2_34       (G_ENCODE_VERSION (2, 34))

/**
 * GMODULE_VERSION_2_36:
 *
 * A macro that evaluates to the 2.36 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.36
 */
#define GMODULE_VERSION_2_36       (G_ENCODE_VERSION (2, 36))

/**
 * GMODULE_VERSION_2_38:
 *
 * A macro that evaluates to the 2.38 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.38
 */
#define GMODULE_VERSION_2_38       (G_ENCODE_VERSION (2, 38))

/**
 * GMODULE_VERSION_2_40:
 *
 * A macro that evaluates to the 2.40 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.40
 */
#define GMODULE_VERSION_2_40       (G_ENCODE_VERSION (2, 40))

/**
 * GMODULE_VERSION_2_42:
 *
 * A macro that evaluates to the 2.42 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.42
 */
#define GMODULE_VERSION_2_42       (G_ENCODE_VERSION (2, 42))

/**
 * GMODULE_VERSION_2_44:
 *
 * A macro that evaluates to the 2.44 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.44
 */
#define GMODULE_VERSION_2_44       (G_ENCODE_VERSION (2, 44))

/**
 * GMODULE_VERSION_2_46:
 *
 * A macro that evaluates to the 2.46 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.46
 */
#define GMODULE_VERSION_2_46       (G_ENCODE_VERSION (2, 46))

/**
 * GMODULE_VERSION_2_48:
 *
 * A macro that evaluates to the 2.48 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.48
 */
#define GMODULE_VERSION_2_48       (G_ENCODE_VERSION (2, 48))

/**
 * GMODULE_VERSION_2_50:
 *
 * A macro that evaluates to the 2.50 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.50
 */
#define GMODULE_VERSION_2_50       (G_ENCODE_VERSION (2, 50))

/**
 * GMODULE_VERSION_2_52:
 *
 * A macro that evaluates to the 2.52 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.52
 */
#define GMODULE_VERSION_2_52       (G_ENCODE_VERSION (2, 52))

/**
 * GMODULE_VERSION_2_54:
 *
 * A macro that evaluates to the 2.54 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.54
 */
#define GMODULE_VERSION_2_54       (G_ENCODE_VERSION (2, 54))

/**
 * GMODULE_VERSION_2_56:
 *
 * A macro that evaluates to the 2.56 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.56
 */
#define GMODULE_VERSION_2_56       (G_ENCODE_VERSION (2, 56))

/**
 * GMODULE_VERSION_2_58:
 *
 * A macro that evaluates to the 2.58 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.58
 */
#define GMODULE_VERSION_2_58       (G_ENCODE_VERSION (2, 58))

/**
 * GMODULE_VERSION_2_60:
 *
 * A macro that evaluates to the 2.60 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.60
 */
#define GMODULE_VERSION_2_60       (G_ENCODE_VERSION (2, 60))

/**
 * GMODULE_VERSION_2_62:
 *
 * A macro that evaluates to the 2.62 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.62
 */
#define GMODULE_VERSION_2_62       (G_ENCODE_VERSION (2, 62))

/**
 * GMODULE_VERSION_2_64:
 *
 * A macro that evaluates to the 2.64 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.64
 */
#define GMODULE_VERSION_2_64       (G_ENCODE_VERSION (2, 64))

/**
 * GMODULE_VERSION_2_66:
 *
 * A macro that evaluates to the 2.66 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.66
 */
#define GMODULE_VERSION_2_66       (G_ENCODE_VERSION (2, 66))

/**
 * GMODULE_VERSION_2_68:
 *
 * A macro that evaluates to the 2.68 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.68
 */
#define GMODULE_VERSION_2_68       (G_ENCODE_VERSION (2, 68))

/**
 * GMODULE_VERSION_2_70:
 *
 * A macro that evaluates to the 2.70 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.70
 */
#define GMODULE_VERSION_2_70       (G_ENCODE_VERSION (2, 70))

/**
 * GMODULE_VERSION_2_72:
 *
 * A macro that evaluates to the 2.72 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.72
 */
#define GMODULE_VERSION_2_72       (G_ENCODE_VERSION (2, 72))

/**
 * GMODULE_VERSION_2_74:
 *
 * A macro that evaluates to the 2.74 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.74
 */
#define GMODULE_VERSION_2_74       (G_ENCODE_VERSION (2, 74))

/**
 * GMODULE_VERSION_2_76:
 *
 * A macro that evaluates to the 2.76 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.76
 */
#define GMODULE_VERSION_2_76       (G_ENCODE_VERSION (2, 76))

/**
 * GMODULE_VERSION_2_78:
 *
 * A macro that evaluates to the 2.78 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.78
 */
#define GMODULE_VERSION_2_78       (G_ENCODE_VERSION (2, 78))

/**
 * GMODULE_VERSION_2_80:
 *
 * A macro that evaluates to the 2.80 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.80
 */
#define GMODULE_VERSION_2_80       (G_ENCODE_VERSION (2, 80))

/**
 * GMODULE_VERSION_2_82:
 *
 * A macro that evaluates to the 2.82 version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 2.82
 */
#define GMODULE_VERSION_2_82       (G_ENCODE_VERSION (2, 82))

/**
 * GMODULE_VERSION_CUR_STABLE:
 *
 * A macro that evaluates to the current stable version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * During an unstable development cycle, this evaluates to the next stable
 * (unreleased) version which will be the result of the development cycle.
 *
 * Since: 2.32
 */
#if (GMODULE_MINOR_VERSION % 2)
#define GMODULE_VERSION_CUR_STABLE         (G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION + 1))
#else
#define GMODULE_VERSION_CUR_STABLE         (G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION))
#endif

/**
 * GMODULE_VERSION_PREV_STABLE:
 *
 * A macro that evaluates to the previous stable version of GLib, in a format
 * that can be used by the C pre-processor.
 *
 * During an unstable development cycle, this evaluates to the most recent
 * released stable release, which preceded this development cycle.
 *
 * Since: 2.32
 */
#if (GMODULE_MINOR_VERSION % 2)
#define GMODULE_VERSION_PREV_STABLE        (G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION - 1))
#else
#define GMODULE_VERSION_PREV_STABLE        (G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION - 2))
#endif

/**
 * GMODULE_VERSION_MIN_REQUIRED:
 *
 * A macro that should be defined by the user prior to including
 * the glib.h header.
 * The definition should be one of the predefined GLib version
 * macros: %GMODULE_VERSION_2_26, %GMODULE_VERSION_2_28,...
 *
 * This macro defines the earliest version of GLib that the package is
 * required to be able to compile against.
 *
 * If the compiler is configured to warn about the use of deprecated
 * functions, then using functions that were deprecated in version
 * %GMODULE_VERSION_MIN_REQUIRED or earlier will cause warnings (but
 * using functions deprecated in later releases will not).
 *
 * Since: 2.32
 */
/* If the package sets GMODULE_VERSION_MIN_REQUIRED to some future
 * GMODULE_VERSION_X_Y value that we don't know about, it will compare as
 * 0 in preprocessor tests.
 */
#ifndef GMODULE_VERSION_MIN_REQUIRED
# define GMODULE_VERSION_MIN_REQUIRED      (GMODULE_VERSION_CUR_STABLE)
#elif GMODULE_VERSION_MIN_REQUIRED == 0
# undef  GMODULE_VERSION_MIN_REQUIRED
# define GMODULE_VERSION_MIN_REQUIRED      (GMODULE_VERSION_CUR_STABLE + 2)
#endif

/**
 * GMODULE_VERSION_MAX_ALLOWED:
 *
 * A macro that should be defined by the user prior to including
 * the glib.h header.
 * The definition should be one of the predefined GLib version
 * macros: %GMODULE_VERSION_2_26, %GMODULE_VERSION_2_28,...
 *
 * This macro defines the latest version of the GLib API that the
 * package is allowed to make use of.
 *
 * If the compiler is configured to warn about the use of deprecated
 * functions, then using functions added after version
 * %GMODULE_VERSION_MAX_ALLOWED will cause warnings.
 *
 * Unless you are using GMODULE_CHECK_VERSION() or the like to compile
 * different code depending on the GLib version, then this should be
 * set to the same value as %GMODULE_VERSION_MIN_REQUIRED.
 *
 * Since: 2.32
 */
#if !defined (GMODULE_VERSION_MAX_ALLOWED) || (GMODULE_VERSION_MAX_ALLOWED == 0)
# undef GMODULE_VERSION_MAX_ALLOWED
# define GMODULE_VERSION_MAX_ALLOWED      (GMODULE_VERSION_CUR_STABLE)
#endif

/* sanity checks */
/*#if GMODULE_VERSION_MIN_REQUIRED > GMODULE_VERSION_CUR_STABLE
#error "GMODULE_VERSION_MIN_REQUIRED must be <= GMODULE_VERSION_CUR_STABLE"
#endif
#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_MIN_REQUIRED
#error "GMODULE_VERSION_MAX_ALLOWED must be >= GMODULE_VERSION_MIN_REQUIRED"
#endif
#if GMODULE_VERSION_MIN_REQUIRED < GMODULE_VERSION_2_26
#error "GMODULE_VERSION_MIN_REQUIRED must be >= GMODULE_VERSION_2_26"
#endif*/

/* These macros are used to mark deprecated functions in GLib headers,
 * and thus have to be exposed in installed headers. But please
 * do *not* use them in other projects. Instead, use G_DEPRECATED
 * or define your own wrappers around it.
 */
#define GMODULE_AVAILABLE_IN_ALL                   _GLIB_EXTERN

/* XXX: Every new stable minor release should add a set of macros here */

# define GMODULE_DEPRECATED_ENUMERATOR G_DEPRECATED
# define GMODULE_DEPRECATED_ENUMERATOR_FOR(f) G_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED _GLIB_EXTERN
# define GMODULE_DEPRECATED_FOR(f) _GLIB_EXTERN

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_26
# define GMODULE_DEPRECATED_IN_2_26                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_26_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_26          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_26_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_26          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_26_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_26           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_26_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_26                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_26_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_26
# define GMODULE_DEPRECATED_MACRO_IN_2_26_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_26
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_26_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_26
# define GMODULE_DEPRECATED_TYPE_IN_2_26_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_26
# define GMODULE_AVAILABLE_IN_2_26                 GMODULE_UNAVAILABLE(2, 26)
# define GMODULE_AVAILABLE_MACRO_IN_2_26           GMODULE_UNAVAILABLE_MACRO(2, 26)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_26      GMODULE_UNAVAILABLE_ENUMERATOR(2, 26)
# define GMODULE_AVAILABLE_TYPE_IN_2_26            GMODULE_UNAVAILABLE_TYPE(2, 26)
#else
# define GMODULE_AVAILABLE_IN_2_26                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_26
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_26
# define GMODULE_AVAILABLE_TYPE_IN_2_26
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_28
# define GMODULE_DEPRECATED_IN_2_28                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_28_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_28          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_28_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_28          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_28_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_28           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_28_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_28                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_28_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_28
# define GMODULE_DEPRECATED_MACRO_IN_2_28_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_28
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_28_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_28
# define GMODULE_DEPRECATED_TYPE_IN_2_28_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_28
# define GMODULE_AVAILABLE_IN_2_28                 GMODULE_UNAVAILABLE(2, 28)
# define GMODULE_AVAILABLE_MACRO_IN_2_28           GMODULE_UNAVAILABLE_MACRO(2, 28)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_28      GMODULE_UNAVAILABLE_ENUMERATOR(2, 28)
# define GMODULE_AVAILABLE_TYPE_IN_2_28            GMODULE_UNAVAILABLE_TYPE(2, 28)
#else
# define GMODULE_AVAILABLE_IN_2_28                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_28
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_28
# define GMODULE_AVAILABLE_TYPE_IN_2_28
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_30
# define GMODULE_DEPRECATED_IN_2_30                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_30_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_30          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_30_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_30          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_30_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_30           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_30_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_30                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_30_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_30
# define GMODULE_DEPRECATED_MACRO_IN_2_30_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_30
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_30_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_30
# define GMODULE_DEPRECATED_TYPE_IN_2_30_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_30
# define GMODULE_AVAILABLE_IN_2_30                 GMODULE_UNAVAILABLE(2, 30)
# define GMODULE_AVAILABLE_MACRO_IN_2_30           GMODULE_UNAVAILABLE_MACRO(2, 30)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_30      GMODULE_UNAVAILABLE_ENUMERATOR(2, 30)
# define GMODULE_AVAILABLE_TYPE_IN_2_30            GMODULE_UNAVAILABLE_TYPE(2, 30)
#else
# define GMODULE_AVAILABLE_IN_2_30                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_30
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_30
# define GMODULE_AVAILABLE_TYPE_IN_2_30
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_32
# define GMODULE_DEPRECATED_IN_2_32                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_32_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_32          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_32_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_32           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_32_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_32                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_32_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_32
# define GMODULE_DEPRECATED_MACRO_IN_2_32_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_32
# define GMODULE_DEPRECATED_TYPE_IN_2_32_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_32_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_32
# define GMODULE_DEPRECATED_TYPE_IN_2_32_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_32
# define GMODULE_AVAILABLE_IN_2_32                 GMODULE_UNAVAILABLE(2, 32)
# define GMODULE_AVAILABLE_MACRO_IN_2_32           GMODULE_UNAVAILABLE_MACRO(2, 32)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_32      GMODULE_UNAVAILABLE_ENUMERATOR(2, 32)
# define GMODULE_AVAILABLE_TYPE_IN_2_32            GMODULE_UNAVAILABLE_TYPE(2, 32)
#else
# define GMODULE_AVAILABLE_IN_2_32                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_32
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_32
# define GMODULE_AVAILABLE_TYPE_IN_2_32
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_34
# define GMODULE_DEPRECATED_IN_2_34                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_34_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_34          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_34_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_34          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_34_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_34           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_34_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_34                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_34_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_34
# define GMODULE_DEPRECATED_MACRO_IN_2_34_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_34
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_34_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_34
# define GMODULE_DEPRECATED_TYPE_IN_2_34_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_34
# define GMODULE_AVAILABLE_IN_2_34                 GMODULE_UNAVAILABLE(2, 34)
# define GMODULE_AVAILABLE_MACRO_IN_2_34           GMODULE_UNAVAILABLE_MACRO(2, 34)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_34      GMODULE_UNAVAILABLE_ENUMERATOR(2, 34)
# define GMODULE_AVAILABLE_TYPE_IN_2_34            GMODULE_UNAVAILABLE_TYPE(2, 34)
#else
# define GMODULE_AVAILABLE_IN_2_34                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_34
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_34
# define GMODULE_AVAILABLE_TYPE_IN_2_34
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_36
# define GMODULE_DEPRECATED_IN_2_36                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_36_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_36          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_36_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_36          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_36_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_36           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_36_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_36                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_36_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_36
# define GMODULE_DEPRECATED_MACRO_IN_2_36_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_36
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_36_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_36
# define GMODULE_DEPRECATED_TYPE_IN_2_36_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_36
# define GMODULE_AVAILABLE_IN_2_36                 GMODULE_UNAVAILABLE(2, 36)
# define GMODULE_AVAILABLE_MACRO_IN_2_36           GMODULE_UNAVAILABLE_MACRO(2, 36)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_36      GMODULE_UNAVAILABLE_ENUMERATOR(2, 36)
# define GMODULE_AVAILABLE_TYPE_IN_2_36            GMODULE_UNAVAILABLE_TYPE(2, 36)
#else
# define GMODULE_AVAILABLE_IN_2_36                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_36
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_36
# define GMODULE_AVAILABLE_TYPE_IN_2_36
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_38
# define GMODULE_DEPRECATED_IN_2_38                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_38_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_38          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_38_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_38          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_38_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_38           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_38_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_38                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_38_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_38
# define GMODULE_DEPRECATED_MACRO_IN_2_38_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_38
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_38_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_38
# define GMODULE_DEPRECATED_TYPE_IN_2_38_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_38
# define GMODULE_AVAILABLE_IN_2_38                 GMODULE_UNAVAILABLE(2, 38)
# define GMODULE_AVAILABLE_MACRO_IN_2_38           GMODULE_UNAVAILABLE_MACRO(2, 38)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_38      GMODULE_UNAVAILABLE_ENUMERATOR(2, 38)
# define GMODULE_AVAILABLE_TYPE_IN_2_38            GMODULE_UNAVAILABLE_TYPE(2, 38)
#else
# define GMODULE_AVAILABLE_IN_2_38                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_38
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_38
# define GMODULE_AVAILABLE_TYPE_IN_2_38
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_40
# define GMODULE_DEPRECATED_IN_2_40                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_40_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_40          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_40_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_40          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_40_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_40           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_40_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_40                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_40_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_40
# define GMODULE_DEPRECATED_MACRO_IN_2_40_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_40
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_40_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_40
# define GMODULE_DEPRECATED_TYPE_IN_2_40_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_40
# define GMODULE_AVAILABLE_IN_2_40                 GMODULE_UNAVAILABLE(2, 40)
# define GMODULE_AVAILABLE_MACRO_IN_2_40           GMODULE_UNAVAILABLE_MACRO(2, 40)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_40      GMODULE_UNAVAILABLE_ENUMERATOR(2, 40)
# define GMODULE_AVAILABLE_TYPE_IN_2_40            GMODULE_UNAVAILABLE_TYPE(2, 40)
#else
# define GMODULE_AVAILABLE_IN_2_40                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_40
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_40
# define GMODULE_AVAILABLE_TYPE_IN_2_40
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_42
# define GMODULE_DEPRECATED_IN_2_42                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_42_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_42          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_42_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_42                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_42_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_42
# define GMODULE_DEPRECATED_MACRO_IN_2_42_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_42
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_42_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_42
# define GMODULE_DEPRECATED_TYPE_IN_2_42_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_42
# define GMODULE_AVAILABLE_IN_2_42                 GMODULE_UNAVAILABLE(2, 42)
# define GMODULE_AVAILABLE_MACRO_IN_2_42           GMODULE_UNAVAILABLE_MACRO(2, 42)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_42      GMODULE_UNAVAILABLE_ENUMERATOR(2, 42)
# define GMODULE_AVAILABLE_TYPE_IN_2_42            GMODULE_UNAVAILABLE_TYPE(2, 42)
#else
# define GMODULE_AVAILABLE_IN_2_42                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_42
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_42
# define GMODULE_AVAILABLE_TYPE_IN_2_42
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_44
# define GMODULE_DEPRECATED_IN_2_44                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_44_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_44          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_44_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_44          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_44_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_44           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_44_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_44                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_44_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_44
# define GMODULE_DEPRECATED_MACRO_IN_2_44_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_44
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_44_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_44
# define GMODULE_DEPRECATED_TYPE_IN_2_44_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_44
# define GMODULE_AVAILABLE_IN_2_44                 GMODULE_UNAVAILABLE(2, 44)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_44   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 44)
# define GMODULE_AVAILABLE_MACRO_IN_2_44           GMODULE_UNAVAILABLE_MACRO(2, 44)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_44      GMODULE_UNAVAILABLE_ENUMERATOR(2, 44)
# define GMODULE_AVAILABLE_TYPE_IN_2_44            GMODULE_UNAVAILABLE_TYPE(2, 44)
#else
# define GMODULE_AVAILABLE_IN_2_44                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_44
# define GMODULE_AVAILABLE_MACRO_IN_2_44
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_44
# define GMODULE_AVAILABLE_TYPE_IN_2_44
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_46
# define GMODULE_DEPRECATED_IN_2_46                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_46_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_46          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_46_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_46                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_46_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_46
# define GMODULE_DEPRECATED_MACRO_IN_2_46_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_46
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_46_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_46
# define GMODULE_DEPRECATED_TYPE_IN_2_46_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_46
# define GMODULE_AVAILABLE_IN_2_46                 GMODULE_UNAVAILABLE(2, 46)
# define GMODULE_AVAILABLE_MACRO_IN_2_46           GMODULE_UNAVAILABLE_MACRO(2, 46)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_46      GMODULE_UNAVAILABLE_ENUMERATOR(2, 46)
# define GMODULE_AVAILABLE_TYPE_IN_2_46            GMODULE_UNAVAILABLE_TYPE(2, 46)
#else
# define GMODULE_AVAILABLE_IN_2_46                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_46
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_46
# define GMODULE_AVAILABLE_TYPE_IN_2_46
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_48
# define GMODULE_DEPRECATED_IN_2_48                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_48_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_48          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_48_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_48          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_48_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_48           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_48_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_48                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_48_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_48
# define GMODULE_DEPRECATED_MACRO_IN_2_48_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_48
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_48_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_48
# define GMODULE_DEPRECATED_TYPE_IN_2_48_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_48
# define GMODULE_AVAILABLE_IN_2_48                 GMODULE_UNAVAILABLE(2, 48)
# define GMODULE_AVAILABLE_MACRO_IN_2_48           GMODULE_UNAVAILABLE_MACRO(2, 48)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_48      GMODULE_UNAVAILABLE_ENUMERATOR(2, 48)
# define GMODULE_AVAILABLE_TYPE_IN_2_48            GMODULE_UNAVAILABLE_TYPE(2, 48)
#else
# define GMODULE_AVAILABLE_IN_2_48                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_48
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_48
# define GMODULE_AVAILABLE_TYPE_IN_2_48
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_50
# define GMODULE_DEPRECATED_IN_2_50                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_50_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_50          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_50_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_50          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_50_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_50           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_50_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_50                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_50_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_50
# define GMODULE_DEPRECATED_MACRO_IN_2_50_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_50
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_50_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_50
# define GMODULE_DEPRECATED_TYPE_IN_2_50_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_50
# define GMODULE_AVAILABLE_IN_2_50                 GMODULE_UNAVAILABLE(2, 50)
# define GMODULE_AVAILABLE_MACRO_IN_2_50           GMODULE_UNAVAILABLE_MACRO(2, 50)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_50      GMODULE_UNAVAILABLE_ENUMERATOR(2, 50)
# define GMODULE_AVAILABLE_TYPE_IN_2_50            GMODULE_UNAVAILABLE_TYPE(2, 50)
#else
# define GMODULE_AVAILABLE_IN_2_50                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_50
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_50
# define GMODULE_AVAILABLE_TYPE_IN_2_50
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_52
# define GMODULE_DEPRECATED_IN_2_52                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_52_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_52          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_52_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_52          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_52_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_52           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_52_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_52                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_52_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_52
# define GMODULE_DEPRECATED_MACRO_IN_2_52_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_52
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_52_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_52
# define GMODULE_DEPRECATED_TYPE_IN_2_52_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_52
# define GMODULE_AVAILABLE_IN_2_52                 GMODULE_UNAVAILABLE(2, 52)
# define GMODULE_AVAILABLE_MACRO_IN_2_52           GMODULE_UNAVAILABLE_MACRO(2, 52)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_52      GMODULE_UNAVAILABLE_ENUMERATOR(2, 52)
# define GMODULE_AVAILABLE_TYPE_IN_2_52            GMODULE_UNAVAILABLE_TYPE(2, 52)
#else
# define GMODULE_AVAILABLE_IN_2_52                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_52
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_52
# define GMODULE_AVAILABLE_TYPE_IN_2_52
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_54
# define GMODULE_DEPRECATED_IN_2_54                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_54_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_54          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_54_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_54          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_54_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_54           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_54_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_54                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_54_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_54
# define GMODULE_DEPRECATED_MACRO_IN_2_54_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_54
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_54_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_54
# define GMODULE_DEPRECATED_TYPE_IN_2_54_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_54
# define GMODULE_AVAILABLE_IN_2_54                 GMODULE_UNAVAILABLE(2, 54)
# define GMODULE_AVAILABLE_MACRO_IN_2_54           GMODULE_UNAVAILABLE_MACRO(2, 54)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_54      GMODULE_UNAVAILABLE_ENUMERATOR(2, 54)
# define GMODULE_AVAILABLE_TYPE_IN_2_54            GMODULE_UNAVAILABLE_TYPE(2, 54)
#else
# define GMODULE_AVAILABLE_IN_2_54                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_54
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_54
# define GMODULE_AVAILABLE_TYPE_IN_2_54
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_56
# define GMODULE_DEPRECATED_IN_2_56                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_56_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_56          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_56_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_56          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_56_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_56           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_56_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_56                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_56_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_56
# define GMODULE_DEPRECATED_MACRO_IN_2_56_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_56
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_56_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_56
# define GMODULE_DEPRECATED_TYPE_IN_2_56_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_56
# define GMODULE_AVAILABLE_IN_2_56                 GMODULE_UNAVAILABLE(2, 56)
# define GMODULE_AVAILABLE_MACRO_IN_2_56           GMODULE_UNAVAILABLE_MACRO(2, 56)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_56      GMODULE_UNAVAILABLE_ENUMERATOR(2, 56)
# define GMODULE_AVAILABLE_TYPE_IN_2_56            GMODULE_UNAVAILABLE_TYPE(2, 56)
#else
# define GMODULE_AVAILABLE_IN_2_56                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_56
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_56
# define GMODULE_AVAILABLE_TYPE_IN_2_56
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_58
# define GMODULE_DEPRECATED_IN_2_58                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_58_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_58          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_58_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_58          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_58_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_58           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_58_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_58                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_58_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_58
# define GMODULE_DEPRECATED_MACRO_IN_2_58_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_58
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_58_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_58
# define GMODULE_DEPRECATED_TYPE_IN_2_58_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_58
# define GMODULE_AVAILABLE_IN_2_58                 GMODULE_UNAVAILABLE(2, 58)
# define GMODULE_AVAILABLE_MACRO_IN_2_58           GMODULE_UNAVAILABLE_MACRO(2, 58)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_58      GMODULE_UNAVAILABLE_ENUMERATOR(2, 58)
# define GMODULE_AVAILABLE_TYPE_IN_2_58            GMODULE_UNAVAILABLE_TYPE(2, 58)
#else
# define GMODULE_AVAILABLE_IN_2_58                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_MACRO_IN_2_58
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_58
# define GMODULE_AVAILABLE_TYPE_IN_2_58
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_60
# define GMODULE_DEPRECATED_IN_2_60                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_60_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_60          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_60_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_60          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_60_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_60           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_60_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_60                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_60_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_60
# define GMODULE_DEPRECATED_MACRO_IN_2_60_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_60
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_60_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_60
# define GMODULE_DEPRECATED_TYPE_IN_2_60_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_60
# define GMODULE_AVAILABLE_IN_2_60                 GMODULE_UNAVAILABLE(2, 60)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_60   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 60)
# define GMODULE_AVAILABLE_MACRO_IN_2_60           GMODULE_UNAVAILABLE_MACRO(2, 60)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_60      GMODULE_UNAVAILABLE_ENUMERATOR(2, 60)
# define GMODULE_AVAILABLE_TYPE_IN_2_60            GMODULE_UNAVAILABLE_TYPE(2, 60)
#else
# define GMODULE_AVAILABLE_IN_2_60                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_60
# define GMODULE_AVAILABLE_MACRO_IN_2_60
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_60
# define GMODULE_AVAILABLE_TYPE_IN_2_60
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_62
# define GMODULE_DEPRECATED_IN_2_62                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_62_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_62          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_62_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_62          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_62_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_62           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_62_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_62                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_62_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_62
# define GMODULE_DEPRECATED_MACRO_IN_2_62_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_62
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_62_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_62
# define GMODULE_DEPRECATED_TYPE_IN_2_62_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_62
# define GMODULE_AVAILABLE_IN_2_62                 GMODULE_UNAVAILABLE(2, 62)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_62   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 62)
# define GMODULE_AVAILABLE_MACRO_IN_2_62           GMODULE_UNAVAILABLE_MACRO(2, 62)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_62      GMODULE_UNAVAILABLE_ENUMERATOR(2, 62)
# define GMODULE_AVAILABLE_TYPE_IN_2_62            GMODULE_UNAVAILABLE_TYPE(2, 62)
#else
# define GMODULE_AVAILABLE_IN_2_62                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_62
# define GMODULE_AVAILABLE_MACRO_IN_2_62
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_62
# define GMODULE_AVAILABLE_TYPE_IN_2_62
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_64
# define GMODULE_DEPRECATED_IN_2_64                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_64_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_64          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_64_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_64          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_64_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_64           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_64_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_64                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_64_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_64
# define GMODULE_DEPRECATED_MACRO_IN_2_64_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_64
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_64_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_64
# define GMODULE_DEPRECATED_TYPE_IN_2_64_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_64
# define GMODULE_AVAILABLE_IN_2_64                 GMODULE_UNAVAILABLE(2, 64)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_64   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 64)
# define GMODULE_AVAILABLE_MACRO_IN_2_64           GMODULE_UNAVAILABLE_MACRO(2, 64)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_64      GMODULE_UNAVAILABLE_ENUMERATOR(2, 64)
# define GMODULE_AVAILABLE_TYPE_IN_2_64            GMODULE_UNAVAILABLE_TYPE(2, 64)
#else
# define GMODULE_AVAILABLE_IN_2_64                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_64
# define GMODULE_AVAILABLE_MACRO_IN_2_64
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_64
# define GMODULE_AVAILABLE_TYPE_IN_2_64
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_66
# define GMODULE_DEPRECATED_IN_2_66                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_66_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_66          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_66_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_66          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_66_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_66           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_66_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_66                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_66_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_66
# define GMODULE_DEPRECATED_MACRO_IN_2_66_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_66
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_66_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_66
# define GMODULE_DEPRECATED_TYPE_IN_2_66_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_66
# define GMODULE_AVAILABLE_IN_2_66                 GMODULE_UNAVAILABLE(2, 66)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_66   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 66)
# define GMODULE_AVAILABLE_MACRO_IN_2_66           GMODULE_UNAVAILABLE_MACRO(2, 66)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_66      GMODULE_UNAVAILABLE_ENUMERATOR(2, 66)
# define GMODULE_AVAILABLE_TYPE_IN_2_66            GMODULE_UNAVAILABLE_TYPE(2, 66)
#else
# define GMODULE_AVAILABLE_IN_2_66                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_66
# define GMODULE_AVAILABLE_MACRO_IN_2_66
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_66
# define GMODULE_AVAILABLE_TYPE_IN_2_66
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_68
# define GMODULE_DEPRECATED_IN_2_68                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_68_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_68          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_68_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_68          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_68_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_68           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_68_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_68                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_68_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_68
# define GMODULE_DEPRECATED_MACRO_IN_2_68_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_68
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_68_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_68
# define GMODULE_DEPRECATED_TYPE_IN_2_68_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_68
# define GMODULE_AVAILABLE_IN_2_68                 GMODULE_UNAVAILABLE(2, 68)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_68   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 68)
# define GMODULE_AVAILABLE_MACRO_IN_2_68           GMODULE_UNAVAILABLE_MACRO(2, 68)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_68      GMODULE_UNAVAILABLE_ENUMERATOR(2, 68)
# define GMODULE_AVAILABLE_TYPE_IN_2_68            GMODULE_UNAVAILABLE_TYPE(2, 68)
#else
# define GMODULE_AVAILABLE_IN_2_68                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_68
# define GMODULE_AVAILABLE_MACRO_IN_2_68
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_68
# define GMODULE_AVAILABLE_TYPE_IN_2_68
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_70
# define GMODULE_DEPRECATED_IN_2_70                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_70_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_70          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_70_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_70          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_70_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_70           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_70_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_70                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_70_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_70
# define GMODULE_DEPRECATED_MACRO_IN_2_70_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_70
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_70_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_70
# define GMODULE_DEPRECATED_TYPE_IN_2_70_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_70
# define GMODULE_AVAILABLE_IN_2_70                 GMODULE_UNAVAILABLE(2, 70)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_70   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 70)
# define GMODULE_AVAILABLE_MACRO_IN_2_70           GMODULE_UNAVAILABLE_MACRO(2, 70)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_70      GMODULE_UNAVAILABLE_ENUMERATOR(2, 70)
# define GMODULE_AVAILABLE_TYPE_IN_2_70            GMODULE_UNAVAILABLE_TYPE(2, 70)
#else
# define GMODULE_AVAILABLE_IN_2_70                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_70
# define GMODULE_AVAILABLE_MACRO_IN_2_70
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_70
# define GMODULE_AVAILABLE_TYPE_IN_2_70
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_72
# define GMODULE_DEPRECATED_IN_2_72                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_72_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_72          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_72_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_72          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_72_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_72           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_72_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_72                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_72_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_72
# define GMODULE_DEPRECATED_MACRO_IN_2_72_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_72
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_72_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_72
# define GMODULE_DEPRECATED_TYPE_IN_2_72_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_72
# define GMODULE_AVAILABLE_IN_2_72                 GMODULE_UNAVAILABLE(2, 72)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_72   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 72)
# define GMODULE_AVAILABLE_MACRO_IN_2_72           GMODULE_UNAVAILABLE_MACRO(2, 72)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_72      GMODULE_UNAVAILABLE_ENUMERATOR(2, 72)
# define GMODULE_AVAILABLE_TYPE_IN_2_72            GMODULE_UNAVAILABLE_TYPE(2, 72)
#else
# define GMODULE_AVAILABLE_IN_2_72                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_72
# define GMODULE_AVAILABLE_MACRO_IN_2_72
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_72
# define GMODULE_AVAILABLE_TYPE_IN_2_72
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_74
# define GMODULE_DEPRECATED_IN_2_74                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_74_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_74          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_74_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_74          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_74_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_74           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_74_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_74                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_74_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_74
# define GMODULE_DEPRECATED_MACRO_IN_2_74_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_74
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_74_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_74
# define GMODULE_DEPRECATED_TYPE_IN_2_74_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_74
# define GMODULE_AVAILABLE_IN_2_74                 GMODULE_UNAVAILABLE(2, 74)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_74   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 74)
# define GMODULE_AVAILABLE_MACRO_IN_2_74           GMODULE_UNAVAILABLE_MACRO(2, 74)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_74      GMODULE_UNAVAILABLE_ENUMERATOR(2, 74)
# define GMODULE_AVAILABLE_TYPE_IN_2_74            GMODULE_UNAVAILABLE_TYPE(2, 74)
#else
# define GMODULE_AVAILABLE_IN_2_74                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_74
# define GMODULE_AVAILABLE_MACRO_IN_2_74
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_74
# define GMODULE_AVAILABLE_TYPE_IN_2_74
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_76
# define GMODULE_DEPRECATED_IN_2_76                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_76_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_76          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_76_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_76          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_76_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_76           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_76_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_76                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_76_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_76
# define GMODULE_DEPRECATED_MACRO_IN_2_76_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_76
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_76_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_76
# define GMODULE_DEPRECATED_TYPE_IN_2_76_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_76
# define GMODULE_AVAILABLE_IN_2_76                 GMODULE_UNAVAILABLE(2, 76)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_76   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 76)
# define GMODULE_AVAILABLE_MACRO_IN_2_76           GMODULE_UNAVAILABLE_MACRO(2, 76)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_76      GMODULE_UNAVAILABLE_ENUMERATOR(2, 76)
# define GMODULE_AVAILABLE_TYPE_IN_2_76            GMODULE_UNAVAILABLE_TYPE(2, 76)
#else
# define GMODULE_AVAILABLE_IN_2_76                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_76
# define GMODULE_AVAILABLE_MACRO_IN_2_76
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_76
# define GMODULE_AVAILABLE_TYPE_IN_2_76
#endif

/*#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_78
# define GMODULE_DEPRECATED_IN_2_78                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_78_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_78          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_78_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_78          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_78_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_78           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_78_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_78                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_78_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_78
# define GMODULE_DEPRECATED_MACRO_IN_2_78_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_78
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_78_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_78
# define GMODULE_DEPRECATED_TYPE_IN_2_78_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_78
# define GMODULE_AVAILABLE_IN_2_78                 GMODULE_UNAVAILABLE(2, 78)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_78   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 78)
# define GMODULE_AVAILABLE_MACRO_IN_2_78           GMODULE_UNAVAILABLE_MACRO(2, 78)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_78      GMODULE_UNAVAILABLE_ENUMERATOR(2, 78)
# define GMODULE_AVAILABLE_TYPE_IN_2_78            GMODULE_UNAVAILABLE_TYPE(2, 78)
#else
# define GMODULE_AVAILABLE_IN_2_78                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_78
# define GMODULE_AVAILABLE_MACRO_IN_2_78
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_78
# define GMODULE_AVAILABLE_TYPE_IN_2_78
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_80
# define GMODULE_DEPRECATED_IN_2_80                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_80_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_80          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_80_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_80          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_80_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_80           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_80_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_80                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_80_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_80
# define GMODULE_DEPRECATED_MACRO_IN_2_80_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_80
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_80_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_80
# define GMODULE_DEPRECATED_TYPE_IN_2_80_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_80
# define GMODULE_AVAILABLE_IN_2_80                 GMODULE_UNAVAILABLE(2, 80)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_80   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 80)
# define GMODULE_AVAILABLE_MACRO_IN_2_80           GMODULE_UNAVAILABLE_MACRO(2, 80)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_80      GMODULE_UNAVAILABLE_ENUMERATOR(2, 80)
# define GMODULE_AVAILABLE_TYPE_IN_2_80            GMODULE_UNAVAILABLE_TYPE(2, 80)
#else
# define GMODULE_AVAILABLE_IN_2_80                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_80
# define GMODULE_AVAILABLE_MACRO_IN_2_80
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_80
# define GMODULE_AVAILABLE_TYPE_IN_2_80
#endif

#if GMODULE_VERSION_MIN_REQUIRED >= GMODULE_VERSION_2_82
# define GMODULE_DEPRECATED_IN_2_82                GMODULE_DEPRECATED
# define GMODULE_DEPRECATED_IN_2_82_FOR(f)         GMODULE_DEPRECATED_FOR(f)
# define GMODULE_DEPRECATED_MACRO_IN_2_82          GMODULE_DEPRECATED_MACRO
# define GMODULE_DEPRECATED_MACRO_IN_2_82_FOR(f)   GMODULE_DEPRECATED_MACRO_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_82          GMODULE_DEPRECATED_ENUMERATOR
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_82_FOR(f)   GMODULE_DEPRECATED_ENUMERATOR_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_82           GMODULE_DEPRECATED_TYPE
# define GMODULE_DEPRECATED_TYPE_IN_2_82_FOR(f)    GMODULE_DEPRECATED_TYPE_FOR(f)
#else
# define GMODULE_DEPRECATED_IN_2_82                _GLIB_EXTERN
# define GMODULE_DEPRECATED_IN_2_82_FOR(f)         _GLIB_EXTERN
# define GMODULE_DEPRECATED_MACRO_IN_2_82
# define GMODULE_DEPRECATED_MACRO_IN_2_82_FOR(f)
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_82
# define GMODULE_DEPRECATED_ENUMERATOR_IN_2_82_FOR(f)
# define GMODULE_DEPRECATED_TYPE_IN_2_82
# define GMODULE_DEPRECATED_TYPE_IN_2_82_FOR(f)
#endif

#if GMODULE_VERSION_MAX_ALLOWED < GMODULE_VERSION_2_82
# define GMODULE_AVAILABLE_IN_2_82                 GMODULE_UNAVAILABLE(2, 82)
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_82   GMODULE_UNAVAILABLE_STATIC_INLINE(2, 82)
# define GMODULE_AVAILABLE_MACRO_IN_2_82           GMODULE_UNAVAILABLE_MACRO(2, 82)
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_82      GMODULE_UNAVAILABLE_ENUMERATOR(2, 82)
# define GMODULE_AVAILABLE_TYPE_IN_2_82            GMODULE_UNAVAILABLE_TYPE(2, 82)
#else
# define GMODULE_AVAILABLE_IN_2_82                 _GLIB_EXTERN
# define GMODULE_AVAILABLE_STATIC_INLINE_IN_2_82
# define GMODULE_AVAILABLE_MACRO_IN_2_82
# define GMODULE_AVAILABLE_ENUMERATOR_IN_2_82
# define GMODULE_AVAILABLE_TYPE_IN_2_82
#endif*/

#endif /*  __G_VERSION_MACROS_H__ */
