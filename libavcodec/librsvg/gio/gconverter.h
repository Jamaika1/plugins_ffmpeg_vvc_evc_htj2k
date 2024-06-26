/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright (C) 2009 Red Hat, Inc.
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
 * Author: Alexander Larsson <alexl@redhat.com>
 */

#ifndef __G_CONVERTER_H__
#define __G_CONVERTER_H__

#if !defined (__GIO_GIO_H_INSIDE__) && !defined (GIO_COMPILATION)
#error "Only <gio/gio.h> can be included directly."
#endif

#include "giotypes.h"

G_BEGIN_DECLS

#define G_TYPE_CONVERTER            (g_converter_get_type ())
#define G_CONVERTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_CONVERTER, GConverter))
#define G_IS_CONVERTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_CONVERTER))
#define G_CONVERTER_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), G_TYPE_CONVERTER, GConverterIface))

typedef struct _GConverterIface   GConverterIface;

/**
 * GConverterIface:
 * @g_iface: The parent interface.
 * @convert: Converts data.
 * @reset: Reverts the internal state of the converter to its initial state.
 *
 * Provides an interface for converting data from one type
 * to another type. The conversion can be stateful
 * and may fail at any place.
 *
 * Since: 2.24
 **/
struct _GConverterIface
{
  GTypeInterface g_iface;

  /* Virtual Table */

  GConverterResult (* convert) (GConverter *converter,
				const void *inbuf,
				gsize       inbuf_size,
				void       *outbuf,
				gsize       outbuf_size,
				GConverterFlags flags,
				gsize      *bytes_read,
				gsize      *bytes_written,
				GError    **error);
  void  (* reset)   (GConverter *converter);
};

GIO_AVAILABLE_IN_ALL
GType            g_converter_get_type     (void) G_GNUC_CONST;

GIO_AVAILABLE_IN_ALL
GConverterResult g_converter_convert (GConverter       *converter,
				      const void       *inbuf,
				      gsize             inbuf_size,
				      void             *outbuf,
				      gsize             outbuf_size,
				      GConverterFlags   flags,
				      gsize            *bytes_read,
				      gsize            *bytes_written,
				      GError          **error);
GIO_AVAILABLE_IN_ALL
void             g_converter_reset   (GConverter       *converter);

GIO_AVAILABLE_IN_2_82
GBytes *         g_converter_convert_bytes (GConverter  *converter,
                                            GBytes      *bytes,
                                            GError     **error);


G_END_DECLS


#endif /* __G_CONVERTER_H__ */
