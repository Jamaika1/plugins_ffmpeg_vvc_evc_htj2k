/* GdkPixbuf library - Scaling and compositing functions
 *
 * Copyright (C) 1999 The Free Software Foundation
 *
 * Author: Owen Taylor <otaylor@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include <math.h>
#include <string.h>
#include "gdk-pixbuf-transform.h"
#include "gdk-pixbuf-private.h"
#include "pixops.h"

/**
 * gdk_pixbuf_scale:
 * @src: a #GdkPixbuf
 * @dest: the #GdkPixbuf into which to render the results
 * @dest_x: the left coordinate for region to render
 * @dest_y: the top coordinate for region to render
 * @dest_width: the width of the region to render
 * @dest_height: the height of the region to render
 * @offset_x: the offset in the X direction (currently rounded to an integer)
 * @offset_y: the offset in the Y direction (currently rounded to an integer)
 * @scale_x: the scale factor in the X direction
 * @scale_y: the scale factor in the Y direction
 * @interp_type: the interpolation type for the transformation.
 *
 * Creates a transformation of the source image @src by scaling by
 * @scale_x and @scale_y then translating by @offset_x and @offset_y,
 * then renders the rectangle (@dest_x, @dest_y, @dest_width,
 * @dest_height) of the resulting image onto the destination image
 * replacing the previous contents.
 *
 * Try to use gdk_pixbuf_scale_simple() first; this function is
 * the industrial-strength power tool you can fall back to, if
 * gdk_pixbuf_scale_simple() isn't powerful enough.
 *
 * If the source rectangle overlaps the destination rectangle on the
 * same pixbuf, it will be overwritten during the scaling which
 * results in rendering artifacts.
 **/
void
gdk_pixbuf_scale (const GdkPixbuf *src,
		  GdkPixbuf       *dest,
		  int              dest_x,
		  int              dest_y,
		  int              dest_width,
		  int              dest_height,
		  double           offset_x,
		  double           offset_y,
		  double           scale_x,
		  double           scale_y,
		  GdkInterpType    interp_type)
{
  const guint8 *src_pixels;
  guint8 *dest_pixels;

  g_return_if_fail (GDK_IS_PIXBUF (src));
  g_return_if_fail (GDK_IS_PIXBUF (dest));
  g_return_if_fail (dest_x >= 0 && dest_x + dest_width <= dest->width);
  g_return_if_fail (dest_y >= 0 && dest_y + dest_height <= dest->height);

  offset_x = floor (offset_x + 0.5);
  offset_y = floor (offset_y + 0.5);

  /* Force an implicit copy */
  dest_pixels = gdk_pixbuf_get_pixels (dest);
  src_pixels = gdk_pixbuf_read_pixels (src);

  _pixops_scale (dest_pixels, dest->width, dest->height, dest->rowstride,
                 dest->n_channels, dest->has_alpha, src_pixels, src->width,
                 src->height, src->rowstride, src->n_channels, src->has_alpha,
                 dest_x, dest_y, dest_width, dest_height, offset_x, offset_y,
                 scale_x, scale_y, (PixopsInterpType)interp_type);
}

/**
 * gdk_pixbuf_composite:
 * @src: a #GdkPixbuf
 * @dest: the #GdkPixbuf into which to render the results
 * @dest_x: the left coordinate for region to render
 * @dest_y: the top coordinate for region to render
 * @dest_width: the width of the region to render
 * @dest_height: the height of the region to render
 * @offset_x: the offset in the X direction (currently rounded to an integer)
 * @offset_y: the offset in the Y direction (currently rounded to an integer)
 * @scale_x: the scale factor in the X direction
 * @scale_y: the scale factor in the Y direction
 * @interp_type: the interpolation type for the transformation.
 * @overall_alpha: overall alpha for source image (0..255)
 *
 * Creates a transformation of the source image @src by scaling by
 * @scale_x and @scale_y then translating by @offset_x and @offset_y.
 *
 * This gives an image in the coordinates of the destination pixbuf.
 * The rectangle (@dest_x, @dest_y, @dest_width, @dest_height)
 * is then alpha blended onto the corresponding rectangle of the
 * original destination image.
 *
 * When the destination rectangle contains parts not in the source
 * image, the data at the edges of the source image is replicated
 * to infinity.
 *
 * ![](composite.png)
 */
void
gdk_pixbuf_composite (const GdkPixbuf *src,
		      GdkPixbuf       *dest,
		      int              dest_x,
		      int              dest_y,
		      int              dest_width,
		      int              dest_height,
		      double           offset_x,
		      double           offset_y,
		      double           scale_x,
		      double           scale_y,
		      GdkInterpType    interp_type,
		      int              overall_alpha)
{
  const guint8 *src_pixels;
  guint8 *dest_pixels;

  g_return_if_fail (GDK_IS_PIXBUF (src));
  g_return_if_fail (GDK_IS_PIXBUF (dest));
  g_return_if_fail (dest_x >= 0 && dest_x + dest_width <= dest->width);
  g_return_if_fail (dest_y >= 0 && dest_y + dest_height <= dest->height);
  g_return_if_fail (overall_alpha >= 0 && overall_alpha <= 255);

  offset_x = floor (offset_x + 0.5);
  offset_y = floor (offset_y + 0.5);

  /* Force an implicit copy */
  dest_pixels = gdk_pixbuf_get_pixels (dest);
  src_pixels = gdk_pixbuf_read_pixels (src);

  _pixops_composite (dest_pixels, dest->width, dest->height, dest->rowstride,
                     dest->n_channels, dest->has_alpha, src_pixels,
                     src->width, src->height, src->rowstride, src->n_channels,
                     src->has_alpha, dest_x, dest_y, dest_width, dest_height,
                     offset_x, offset_y, scale_x, scale_y,
                     (PixopsInterpType)interp_type, overall_alpha);
}

/**
 * gdk_pixbuf_composite_color:
 * @src: a #GdkPixbuf
 * @dest: the #GdkPixbuf into which to render the results
 * @dest_x: the left coordinate for region to render
 * @dest_y: the top coordinate for region to render
 * @dest_width: the width of the region to render
 * @dest_height: the height of the region to render
 * @offset_x: the offset in the X direction (currently rounded to an integer)
 * @offset_y: the offset in the Y direction (currently rounded to an integer)
 * @scale_x: the scale factor in the X direction
 * @scale_y: the scale factor in the Y direction
 * @interp_type: the interpolation type for the transformation.
 * @overall_alpha: overall alpha for source image (0..255)
 * @check_x: the X offset for the checkboard (origin of checkboard is at -@check_x, -@check_y)
 * @check_y: the Y offset for the checkboard
 * @check_size: the size of checks in the checkboard (must be a power of two)
 * @color1: the color of check at upper left
 * @color2: the color of the other check
 *
 * Creates a transformation of the source image @src by scaling by
 * @scale_x and @scale_y then translating by @offset_x and @offset_y,
 * then alpha blends the rectangle (@dest_x ,@dest_y, @dest_width,
 * @dest_height) of the resulting image with a checkboard of the
 * colors @color1 and @color2 and renders it onto the destination
 * image.
 *
 * If the source image has no alpha channel, and @overall_alpha is 255, a fast
 * path is used which omits the alpha blending and just performs the scaling.
 *
 * See gdk_pixbuf_composite_color_simple() for a simpler variant of this
 * function suitable for many tasks.
 **/
void
gdk_pixbuf_composite_color (const GdkPixbuf *src,
			    GdkPixbuf       *dest,
			    int              dest_x,
			    int              dest_y,
			    int              dest_width,
			    int              dest_height,
			    double           offset_x,
			    double           offset_y,
			    double           scale_x,
			    double           scale_y,
			    GdkInterpType    interp_type,
			    int              overall_alpha,
			    int              check_x,
			    int              check_y,
			    int              check_size,
			    guint32          color1,
			    guint32          color2)
{
  const guint8 *src_pixels;
  guint8 *dest_pixels;

  g_return_if_fail (GDK_IS_PIXBUF (src));
  g_return_if_fail (GDK_IS_PIXBUF (dest));
  g_return_if_fail (dest_x >= 0 && dest_x + dest_width <= dest->width);
  g_return_if_fail (dest_y >= 0 && dest_y + dest_height <= dest->height);
  g_return_if_fail (overall_alpha >= 0 && overall_alpha <= 255);

  offset_x = floor (offset_x + 0.5);
  offset_y = floor (offset_y + 0.5);

  /* Force an implicit copy */
  dest_pixels = gdk_pixbuf_get_pixels (dest);
  src_pixels = gdk_pixbuf_read_pixels (src);

  _pixops_composite_color (dest_pixels, dest_width, dest_height,
			   dest->rowstride, dest->n_channels, dest->has_alpha,
			   src_pixels, src->width, src->height,
			   src->rowstride, src->n_channels, src->has_alpha,
			   dest_x, dest_y, dest_width, dest_height, offset_x,
			   offset_y, scale_x, scale_y,
			   (PixopsInterpType)interp_type, overall_alpha,
			   check_x, check_y, check_size, color1, color2);
}

/**
 * gdk_pixbuf_scale_simple:
 * @src: a #GdkPixbuf
 * @dest_width: the width of destination image
 * @dest_height: the height of destination image
 * @interp_type: the interpolation type for the transformation.
 *
 * Create a new pixbuf containing a copy of `src` scaled to
 * `dest_width` x `dest_height`.
 *
 * This function leaves `src` unaffected.
 *
 * The `interp_type` should be `GDK_INTERP_NEAREST` if you want maximum
 * speed (but when scaling down `GDK_INTERP_NEAREST` is usually unusably
 * ugly). The default `interp_type` should be `GDK_INTERP_BILINEAR` which
 * offers reasonable quality and speed.
 *
 * You can scale a sub-portion of `src` by creating a sub-pixbuf
 * pointing into `src`; see [method@GdkPixbuf.Pixbuf.new_subpixbuf].
 *
 * If `dest_width` and `dest_height` are equal to the width and height of
 * `src`, this function will return an unscaled copy of `src`.
 *
 * For more complicated scaling/alpha blending see [method@GdkPixbuf.Pixbuf.scale]
 * and [method@GdkPixbuf.Pixbuf.composite].
 *
 * Return value: (nullable) (transfer full): the new pixbuf
 **/
GdkPixbuf *
gdk_pixbuf_scale_simple (const GdkPixbuf *src,
			 int              dest_width,
			 int              dest_height,
			 GdkInterpType    interp_type)
{
  GdkPixbuf *dest;

  g_return_val_if_fail (GDK_IS_PIXBUF (src), NULL);
  g_return_val_if_fail (dest_width > 0, NULL);
  g_return_val_if_fail (dest_height > 0, NULL);

  /* Fast path. */
  if (dest_width == src->width && dest_height == src->height)
    return gdk_pixbuf_copy (src);

  dest = gdk_pixbuf_new (GDK_COLORSPACE_RGB, src->has_alpha, 8, dest_width, dest_height);
  if (!dest)
    return NULL;

  gdk_pixbuf_scale (src, dest,  0, 0, dest_width, dest_height, 0, 0,
		    (double) dest_width / src->width,
		    (double) dest_height / src->height,
		    interp_type);

  return dest;
}

/**
 * gdk_pixbuf_composite_color_simple:
 * @src: a #GdkPixbuf
 * @dest_width: the width of destination image
 * @dest_height: the height of destination image
 * @interp_type: the interpolation type for the transformation.
 * @overall_alpha: overall alpha for source image (0..255)
 * @check_size: the size of checks in the checkboard (must be a power of two)
 * @color1: the color of check at upper left
 * @color2: the color of the other check
 *
 * Creates a new pixbuf by scaling `src` to `dest_width` x `dest_height`
 * and alpha blending the result with a checkboard of colors `color1`
 * and `color2`.
 *
 * Return value: (nullable) (transfer full): the new pixbuf
 **/
GdkPixbuf *
gdk_pixbuf_composite_color_simple (const GdkPixbuf *src,
				   int              dest_width,
				   int              dest_height,
				   GdkInterpType    interp_type,
				   int              overall_alpha,
				   int              check_size,
				   guint32          color1,
				   guint32          color2)
{
  GdkPixbuf *dest;

  g_return_val_if_fail (GDK_IS_PIXBUF (src), NULL);
  g_return_val_if_fail (dest_width > 0, NULL);
  g_return_val_if_fail (dest_height > 0, NULL);
  g_return_val_if_fail (overall_alpha >= 0 && overall_alpha <= 255, NULL);

  dest = gdk_pixbuf_new (GDK_COLORSPACE_RGB, src->has_alpha, 8, dest_width, dest_height);
  if (!dest)
    return NULL;

  gdk_pixbuf_composite_color (src, dest, 0, 0, dest_width, dest_height, 0, 0,
			      (double) dest_width / src->width,
			      (double) dest_height / src->height,
			      interp_type, overall_alpha, 0, 0, check_size, color1, color2);

  return dest;
}

#define OFFSET(pb, x, y) ((x) * (pb)->n_channels + (gsize)(y) * (pb)->rowstride)

/**
 * gdk_pixbuf_rotate_simple:
 * @src: a #GdkPixbuf
 * @angle: the angle to rotate by
 *
 * Rotates a pixbuf by a multiple of 90 degrees, and returns the
 * result in a new pixbuf.
 *
 * If `angle` is 0, this function will return a copy of `src`.
 *
 * Returns: (nullable) (transfer full): the new pixbuf
 *
 * Since: 2.6
 */
GdkPixbuf *
gdk_pixbuf_rotate_simple (const GdkPixbuf   *src,
			  GdkPixbufRotation  angle)
{
  const guint8 *src_pixels;
  guint8 *dest_pixels;
  GdkPixbuf *dest;
  const guchar *p;
  guchar *q;
  gint x, y;

  g_return_val_if_fail (GDK_IS_PIXBUF (src), NULL);
  src_pixels = gdk_pixbuf_read_pixels (src);

  switch (angle % 360)
    {
    case 0:
      dest = gdk_pixbuf_copy (src);
      break;
    case 90:
      dest = gdk_pixbuf_new (src->colorspace,
			     src->has_alpha,
			     src->bits_per_sample,
			     src->height,
			     src->width);
      if (!dest)
	return NULL;

      dest_pixels = gdk_pixbuf_get_pixels (dest);

      for (y = 0; y < src->height; y++)
	{
	  for (x = 0; x < src->width; x++)
	    {
	      p = src_pixels + OFFSET (src, x, y);
	      q = dest_pixels + OFFSET (dest, y, src->width - x - 1);
	      memcpy (q, p, dest->n_channels);
	    }
	}
      break;
    case 180:
      dest = gdk_pixbuf_new (src->colorspace,
			     src->has_alpha,
			     src->bits_per_sample,
			     src->width,
			     src->height);
      if (!dest)
	return NULL;

      dest_pixels = gdk_pixbuf_get_pixels (dest);

      for (y = 0; y < src->height; y++)
	{
	  for (x = 0; x < src->width; x++)
	    {
	      p = src_pixels + OFFSET (src, x, y);
	      q = dest_pixels + OFFSET (dest, src->width - x - 1, src->height - y - 1);
	      memcpy (q, p, dest->n_channels);
	    }
	}
      break;
    case 270:
      dest = gdk_pixbuf_new (src->colorspace,
			     src->has_alpha,
			     src->bits_per_sample,
			     src->height,
			     src->width);
      if (!dest)
	return NULL;

      dest_pixels = gdk_pixbuf_get_pixels (dest);

      for (y = 0; y < src->height; y++)
	{
	  for (x = 0; x < src->width; x++)
	    {
	      p = src_pixels + OFFSET (src, x, y);
	      q = dest_pixels + OFFSET (dest, src->height - y - 1, x);
	      memcpy (q, p, dest->n_channels);
	    }
	}
      break;
    default:
      dest = NULL;
      g_warning ("gdk_pixbuf_rotate_simple() can only rotate "
		 "by multiples of 90 degrees");
      g_assert_not_reached ();
  }

  return dest;
}

/**
 * gdk_pixbuf_flip:
 * @src: a #GdkPixbuf
 * @horizontal: `TRUE` to flip horizontally, `FALSE` to flip vertically
 *
 * Flips a pixbuf horizontally or vertically and returns the
 * result in a new pixbuf.
 *
 * Returns: (nullable) (transfer full): the new pixbuf
 *
 * Since: 2.6
 */
GdkPixbuf *
gdk_pixbuf_flip (const GdkPixbuf *src,
		 gboolean         horizontal)
{
  const guint8 *src_pixels;
  guint8 *dest_pixels;
  GdkPixbuf *dest;
  const guchar *p;
  guchar *q;
  gint x, y;

  g_return_val_if_fail (GDK_IS_PIXBUF (src), NULL);
  dest = gdk_pixbuf_new (src->colorspace,
			 src->has_alpha,
			 src->bits_per_sample,
			 src->width,
			 src->height);
  if (!dest)
    return NULL;

  dest_pixels = gdk_pixbuf_get_pixels (dest);
  src_pixels = gdk_pixbuf_read_pixels (src);

  if (!horizontal) /* flip vertical */
    {
      for (y = 0; y < dest->height; y++)
	{
	  p = src_pixels + OFFSET (src, 0, y);
	  q = dest_pixels + OFFSET (dest, 0, dest->height - y - 1);
	  memcpy (q, p, dest->rowstride);
	}
    }
  else /* flip horizontal */
    {
      for (y = 0; y < dest->height; y++)
	{
	  for (x = 0; x < dest->width; x++)
	    {
	      p = src_pixels + OFFSET (src, x, y);
	      q = dest_pixels + OFFSET (dest, dest->width - x - 1, y);
	      memcpy (q, p, dest->n_channels);
	    }
	}
    }

  return dest;
}
