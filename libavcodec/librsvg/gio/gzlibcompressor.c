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

#include "config.h"

#include "gzlibcompressor.h"

#include <errno.h>
#include <zlib.h>
#include <string.h>

#include "gfileinfo.h"
#include "gioerror.h"
#include "gioenums.h"
#include "gioenumtypes.h"
#include "../glib/glibintl.h"


enum {
  PROP_0,
  PROP_FORMAT,
  PROP_LEVEL,
  PROP_FILE_INFO,
  PROP_OS
};

/**
 * GZlibCompressor:
 *
 * `GZlibCompressor` is an implementation of [iface@Gio.Converter] that
 * compresses data using zlib.
 */

static void g_zlib_compressor_iface_init          (GConverterIface *iface);

struct _GZlibCompressor
{
  GObject parent_instance;

  GZlibCompressorFormat format;
  int level;
  z_stream zstream;
  gz_header gzheader;
  GFileInfo *file_info;
  int os;
};

static void
g_zlib_compressor_set_gzheader (GZlibCompressor *compressor)
{
  /* On win32, these functions were not exported before 1.2.4 */
#if !defined (G_OS_WIN32) || ZLIB_VERNUM >= 0x1240
  const gchar *filename;

  if (compressor->format != G_ZLIB_COMPRESSOR_FORMAT_GZIP ||
      (compressor->file_info == NULL &&
       compressor->os < 0))
    return;

  memset (&compressor->gzheader, 0, sizeof (gz_header));

  if (compressor->os >= 0)
    compressor->gzheader.os = compressor->os;
  else
    compressor->gzheader.os = 0x03; /* Unix */

  if (compressor->file_info)
    {
      filename = g_file_info_get_name (compressor->file_info);
      compressor->gzheader.name = (Bytef *) filename;
      compressor->gzheader.name_max = filename ? strlen (filename) + 1 : 0;

      compressor->gzheader.time =
          (uLong) g_file_info_get_attribute_uint64 (compressor->file_info,
                                                    G_FILE_ATTRIBUTE_TIME_MODIFIED);
    }

  if (deflateSetHeader (&compressor->zstream, &compressor->gzheader) != Z_OK)
    g_warning ("unexpected zlib error: %s", compressor->zstream.msg);
#endif /* !G_OS_WIN32 || ZLIB >= 1.2.4 */
}

G_DEFINE_TYPE_WITH_CODE (GZlibCompressor, g_zlib_compressor, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (G_TYPE_CONVERTER,
						g_zlib_compressor_iface_init))

static void
g_zlib_compressor_finalize (GObject *object)
{
  GZlibCompressor *compressor;

  compressor = G_ZLIB_COMPRESSOR (object);

  deflateEnd (&compressor->zstream);

  if (compressor->file_info)
    g_object_unref (compressor->file_info);

  G_OBJECT_CLASS (g_zlib_compressor_parent_class)->finalize (object);
}


static void
g_zlib_compressor_set_property (GObject      *object,
				  guint         prop_id,
				  const GValue *value,
				  GParamSpec   *pspec)
{
  GZlibCompressor *compressor;

  compressor = G_ZLIB_COMPRESSOR (object);

  switch (prop_id)
    {
    case PROP_FORMAT:
      compressor->format = g_value_get_enum (value);
      break;

    case PROP_LEVEL:
      compressor->level = g_value_get_int (value);
      break;

    case PROP_FILE_INFO:
      g_zlib_compressor_set_file_info (compressor, g_value_get_object (value));
      break;

    case PROP_OS:
      g_zlib_compressor_set_os (compressor, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

}

static void
g_zlib_compressor_get_property (GObject    *object,
				  guint       prop_id,
				  GValue     *value,
				  GParamSpec *pspec)
{
  GZlibCompressor *compressor;

  compressor = G_ZLIB_COMPRESSOR (object);

  switch (prop_id)
    {
    case PROP_FORMAT:
      g_value_set_enum (value, compressor->format);
      break;

    case PROP_LEVEL:
      g_value_set_int (value, compressor->level);
      break;

    case PROP_FILE_INFO:
      g_value_set_object (value, compressor->file_info);
      break;

    case PROP_OS:
      g_value_set_int (value, compressor->os);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
g_zlib_compressor_init (GZlibCompressor *compressor)
{
  compressor->os = -1;
}

static void
g_zlib_compressor_constructed (GObject *object)
{
  GZlibCompressor *compressor;
  int res;

  compressor = G_ZLIB_COMPRESSOR (object);

  if (compressor->format == G_ZLIB_COMPRESSOR_FORMAT_GZIP)
    {
      /* + 16 for gzip */
      res = deflateInit2 (&compressor->zstream,
			  compressor->level, Z_DEFLATED,
			  MAX_WBITS + 16, 8,
			  Z_DEFAULT_STRATEGY);
    }
  else if (compressor->format == G_ZLIB_COMPRESSOR_FORMAT_RAW)
    {
      /* negative wbits for raw */
      res = deflateInit2 (&compressor->zstream,
			  compressor->level, Z_DEFLATED,
			  -MAX_WBITS, 8,
			  Z_DEFAULT_STRATEGY);
    }
  else /* ZLIB */
    res = deflateInit (&compressor->zstream, compressor->level);

  if (res == Z_MEM_ERROR )
    g_error ("GZlibCompressor: Not enough memory for zlib use");

  if (res != Z_OK)
    g_warning ("unexpected zlib error: %s", compressor->zstream.msg);

  g_zlib_compressor_set_gzheader (compressor);
}

static void
g_zlib_compressor_class_init (GZlibCompressorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = g_zlib_compressor_finalize;
  gobject_class->constructed = g_zlib_compressor_constructed;
  gobject_class->get_property = g_zlib_compressor_get_property;
  gobject_class->set_property = g_zlib_compressor_set_property;

  /**
   * GZlibCompressor:format:
   *
   * The format of the compressed data.
   *
   * Since: 2.24
   */
  g_object_class_install_property (gobject_class,
				   PROP_FORMAT,
				   g_param_spec_enum ("format", NULL, NULL,
						      G_TYPE_ZLIB_COMPRESSOR_FORMAT,
						      G_ZLIB_COMPRESSOR_FORMAT_ZLIB,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
						      G_PARAM_STATIC_STRINGS));

  /**
   * GZlibCompressor:level:
   *
   * The level of compression from `0` (no compression) to `9` (most
   * compression).
   *
   * `-1` for the default level.
   *
   * Since: 2.24
   */
  g_object_class_install_property (gobject_class,
				   PROP_LEVEL,
				   g_param_spec_int ("level", NULL, NULL,
						     -1, 9,
						     -1,
						     G_PARAM_READWRITE |
						     G_PARAM_CONSTRUCT_ONLY |
						     G_PARAM_STATIC_STRINGS));

  /**
   * GZlibCompressor:file-info:
   *
   * A [class@Gio.FileInfo] containing file information to put into the gzip
   * header.
   *
   * The file name and modification time from the file info will be used.
   *
   * This will only be used if non-`NULL` and
   * [property@Gio.ZlibCompressor:format] is
   * [enum@Gio.ZlibCompressorFormat.GZIP].
   *
   * Since: 2.26
   */
  g_object_class_install_property (gobject_class,
                                   PROP_FILE_INFO,
                                   g_param_spec_object ("file-info", NULL, NULL,
                                                       G_TYPE_FILE_INFO,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_STATIC_STRINGS));

  /**
   * GZlibCompressor:os:
   *
   * The OS code of the gzip header.
   *
   * This will be used if set to a non-negative value, and if
   * [property@Gio.ZlibCompressor:format] is
   * [enum@Gio.ZlibCompressorFormat.GZIP], the compressor will set the OS code of
   * the gzip header to this value.
   *
   * If the value is unset, zlib will set the OS code depending on the platform.
   * This may be undesirable when reproducible output is desired. In that case setting
   * the OS code to `3` (for Unix) is recommended.
   *
   * Since: 2.86
   */
  g_object_class_install_property (gobject_class,
                                   PROP_OS,
                                   g_param_spec_int  ("os", NULL, NULL,
                                                      -1, 255,
                                                      -1,
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS |
                                                      G_PARAM_EXPLICIT_NOTIFY));
}

/**
 * g_zlib_compressor_new:
 * @format: the format to use for the compressed data
 * @level: compression level (`0`-`9`), `-1` for default
 *
 * Creates a compressor.
 *
 * Returns: a new [class@Gio.ZlibCompressor]
 * Since: 2.24
 **/
GZlibCompressor *
g_zlib_compressor_new (GZlibCompressorFormat format,
		       int level)
{
  GZlibCompressor *compressor;

  compressor = g_object_new (G_TYPE_ZLIB_COMPRESSOR,
			     "format", format,
			     "level", level,
			     NULL);

  return compressor;
}

/**
 * g_zlib_compressor_get_file_info:
 * @compressor: a compressor
 *
 * Gets the [property@Gio.ZlibCompressor:file-info] property.
 *
 * Returns: (nullable) (transfer none): file info for the gzip header, if set
 * Since: 2.26
 */
GFileInfo *
g_zlib_compressor_get_file_info (GZlibCompressor *compressor)
{
  g_return_val_if_fail (G_IS_ZLIB_COMPRESSOR (compressor), NULL);

  return compressor->file_info;
}

/**
 * g_zlib_compressor_set_file_info:
 * @compressor: a compressor
 * @file_info: (nullable): file info for the gzip header
 *
 * Sets the [property@Gio.ZlibCompressor:file-info] property.
 *
 * Note: it is an error to call this function while a compression is in
 * progress; it may only be called immediately after creation of @compressor,
 * or after resetting it with [method@Gio.Converter.reset].
 *
 * Since: 2.26
 */
void
g_zlib_compressor_set_file_info (GZlibCompressor *compressor,
                                 GFileInfo       *file_info)
{
  g_return_if_fail (G_IS_ZLIB_COMPRESSOR (compressor));

  if (file_info == compressor->file_info)
    return;

  if (compressor->file_info)
    g_object_unref (compressor->file_info);
  if (file_info)
    g_object_ref (file_info);
  compressor->file_info = file_info;
  g_object_notify (G_OBJECT (compressor), "file-info");

  g_zlib_compressor_set_gzheader (compressor);
}

/**
 * g_zlib_compressor_get_os:
 * @compressor: a compressor
 *
 * Gets the [property@Gio.ZlibCompressor:os] property.
 *
 * Returns: the previously set OS value, or `-1` if unset
 * Since: 2.86
 */
int
g_zlib_compressor_get_os (GZlibCompressor *compressor)
{
  g_return_val_if_fail (G_IS_ZLIB_COMPRESSOR (compressor), -1);

  return compressor->os;
}

/**
 * g_zlib_compressor_set_os:
 * @compressor: a compressor
 * @os: the OS code to use, or `-1` to unset
 *
 * Sets the [property@Gio.ZlibCompressor:os] property.
 *
 * Note: it is an error to call this function while a compression is in
 * progress; it may only be called immediately after creation of @compressor,
 * or after resetting it with [method@Gio.Converter.reset].
 *
 * Since: 2.86
 */
void
g_zlib_compressor_set_os (GZlibCompressor *compressor,
                          int              os)
{
  g_return_if_fail (G_IS_ZLIB_COMPRESSOR (compressor));
  g_return_if_fail (os >= -1 && os < 256);

  if (os == compressor->os)
    return;

  compressor->os = os;

  g_object_notify (G_OBJECT (compressor), "os");

  g_zlib_compressor_set_gzheader (compressor);
}

static void
g_zlib_compressor_reset (GConverter *converter)
{
  GZlibCompressor *compressor = G_ZLIB_COMPRESSOR (converter);
  int res;

  res = deflateReset (&compressor->zstream);
  if (res != Z_OK)
    g_warning ("unexpected zlib error: %s", compressor->zstream.msg);

  /* deflateReset reset the header too, so re-set it */
  g_zlib_compressor_set_gzheader (compressor);
}

static GConverterResult
g_zlib_compressor_convert (GConverter *converter,
			   const void *inbuf,
			   gsize       inbuf_size,
			   void       *outbuf,
			   gsize       outbuf_size,
			   GConverterFlags flags,
			   gsize      *bytes_read,
			   gsize      *bytes_written,
			   GError    **error)
{
  GZlibCompressor *compressor;
  int res;
  int flush;

  compressor = G_ZLIB_COMPRESSOR (converter);

  compressor->zstream.next_in = (void *)inbuf;
  compressor->zstream.avail_in = inbuf_size;

  compressor->zstream.next_out = outbuf;
  compressor->zstream.avail_out = outbuf_size;

  flush = Z_NO_FLUSH;
  if (flags & G_CONVERTER_INPUT_AT_END)
    flush = Z_FINISH;
  else if (flags & G_CONVERTER_FLUSH)
    flush = Z_SYNC_FLUSH;

  res = deflate (&compressor->zstream, flush);

  if (res == Z_MEM_ERROR)
    {
      g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_FAILED,
			   _("Not enough memory"));
      return G_CONVERTER_ERROR;
    }

    if (res == Z_STREAM_ERROR)
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
		   _("Internal error: %s"), compressor->zstream.msg);
      return G_CONVERTER_ERROR;
    }

    if (res == Z_BUF_ERROR)
      {
	if (flags & G_CONVERTER_FLUSH)
	  return G_CONVERTER_FLUSHED;

	/* We do have output space, so this should only happen if we
	   have no input but need some */

	g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_PARTIAL_INPUT,
			     _("Need more input"));
	return G_CONVERTER_ERROR;
      }

  if (res == Z_OK || res == Z_STREAM_END)
    {
      *bytes_read = inbuf_size - compressor->zstream.avail_in;
      *bytes_written = outbuf_size - compressor->zstream.avail_out;

      if (res == Z_STREAM_END)
	return G_CONVERTER_FINISHED;
      return G_CONVERTER_CONVERTED;
    }

  g_assert_not_reached ();
}

static void
g_zlib_compressor_iface_init (GConverterIface *iface)
{
  iface->convert = g_zlib_compressor_convert;
  iface->reset = g_zlib_compressor_reset;
}
