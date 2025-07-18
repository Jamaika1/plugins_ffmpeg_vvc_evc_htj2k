/* GLIB - Library of useful routines for C programming
 * gdataset-private.h: Internal macros for accessing dataset values
 * Copyright (C) 2005  Red Hat
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

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_DATASETPRIVATE_H__
#define __G_DATASETPRIVATE_H__

#include "gatomic.h"

G_BEGIN_DECLS

/* GET_FLAGS is implemented via atomic pointer access, to allow memory
 * barriers to take effect without acquiring the global dataset mutex.
 */
#define G_DATALIST_GET_FLAGS(datalist)				\
  ((gsize) g_atomic_pointer_get (datalist) & G_DATALIST_FLAGS_MASK)

#define _G_DATALIST_LOCK_BIT 2

#define g_datalist_lock(datalist)                                   \
  G_STMT_START                                                      \
  {                                                                 \
    GData **const _datalist = (datalist);                           \
                                                                    \
    g_pointer_bit_lock ((void **) _datalist, _G_DATALIST_LOCK_BIT); \
  }                                                                 \
  G_STMT_END

#define g_datalist_unlock(datalist)                                   \
  G_STMT_START                                                        \
  {                                                                   \
    GData **const _datalist = (datalist);                             \
                                                                      \
    g_pointer_bit_unlock ((void **) _datalist, _G_DATALIST_LOCK_BIT); \
  }                                                                   \
  G_STMT_END

/*< private >
 * GDataListUpdateAtomicFunc:
 * @data: (inout) (nullable) (not optional): the existing data corresponding to
 *   the "key_id" parameter of g_datalist_id_update_atomic(), and return location
 *   for the new value for it
 * @destroy_notify: (inout) (nullable) (not optional): the existing destroy
 *   notify function for @data, and return location for the destroy notify
 *   function for the new value for it
 * @user_data: user data passed in to [func@GLib.datalist_id_update_atomic]
 *
 * Callback from [func@GLib.datalist_id_update_atomic].
 *
 * Since: 2.80
 */
typedef gpointer (*GDataListUpdateAtomicFunc) (gpointer *data,
                                               GDestroyNotify *destroy_notify,
                                               gpointer user_data);

gpointer g_datalist_id_update_atomic (GData **datalist,
                                      GQuark key_id,
                                      gboolean already_locked,
                                      GDataListUpdateAtomicFunc callback,
                                      gpointer user_data);

G_END_DECLS

#endif /* __G_DATASETPRIVATE_H__ */
