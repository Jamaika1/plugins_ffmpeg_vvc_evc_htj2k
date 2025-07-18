/*
 * Copyright © 2009, 2010 Codethink Limited
 * Copyright © 2011 Collabora Ltd.
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
 *
 * Author: Ryan Lortie <desrt@desrt.ca>
 *         Stef Walter <stefw@collabora.co.uk>
 */

#include "config.h"

#include "gbytes.h"

#include "garray.h"
#include "gstrfuncs.h"
#include "gatomic.h"
#include "gtestutils.h"
#include "gmem.h"
#include "gmessages.h"
#include "grefcount.h"

#include <string.h>

#if GLIB_SIZEOF_VOID_P == 8
# define G_BYTES_MAX_INLINE (128 - sizeof(GBytesInline))
#else
# define G_BYTES_MAX_INLINE (64 - sizeof(GBytesInline))
#endif

/**
 * GBytes: (copy-func g_bytes_ref) (free-func g_bytes_unref)
 *
 * A simple reference counted data type representing an immutable sequence of
 * zero or more bytes from an unspecified origin.
 *
 * The purpose of a `GBytes` is to keep the memory region that it holds
 * alive for as long as anyone holds a reference to the bytes.  When
 * the last reference count is dropped, the memory is released. Multiple
 * unrelated callers can use byte data in the `GBytes` without coordinating
 * their activities, resting assured that the byte data will not change or
 * move while they hold a reference.
 *
 * A `GBytes` can come from many different origins that may have
 * different procedures for freeing the memory region.  Examples are
 * memory from [func@GLib.malloc], from memory slices, from a
 * [struct@GLib.MappedFile] or memory from other allocators.
 *
 * `GBytes` work well as keys in [struct@GLib.HashTable]. Use
 * [method@GLib.Bytes.equal] and [method@GLib.Bytes.hash] as parameters to
 * [func@GLib.HashTable.new] or [func@GLib.HashTable.new_full].
 * `GBytes` can also be used as keys in a [struct@GLib.Tree] by passing the
 * [method@GLib.Bytes.compare] function to [ctor@GLib.Tree.new].
 *
 * The data pointed to by this bytes must not be modified. For a mutable
 * array of bytes see [struct@GLib.ByteArray]. Use
 * [method@GLib.Bytes.unref_to_array] to create a mutable array for a `GBytes`
 * sequence. To create an immutable `GBytes` from a mutable
 * [struct@GLib.ByteArray], use the [func@GLib.ByteArray.free_to_bytes]
 * function.
 *
 * Since: 2.32
 **/

/* Keep in sync with glib/tests/bytes.c */
struct _GBytes
{
  gconstpointer data;  /* may be NULL iff (size == 0) */
  gsize size;  /* may be 0 */
  gatomicrefcount ref_count;
  GDestroyNotify free_func;
  gpointer user_data;
};

typedef struct
{
  GBytes bytes;
  /* Despite no guarantee about alignment in GBytes, it is nice to
   * provide that to ensure that any code which predates support
   * for inline data continues to work without disruption. malloc()
   * on glibc systems would guarantee 2*sizeof(void*) aligned
   * allocations and this matches that.
   */
  gsize padding;
  guint8 inline_data[];
} GBytesInline;

G_STATIC_ASSERT (G_STRUCT_OFFSET (GBytesInline, inline_data) == (6 * GLIB_SIZEOF_VOID_P));

/**
 * g_bytes_new:
 * @data: (transfer none) (array length=size) (element-type guint8) (nullable):
 *   the data to be used for the bytes
 * @size: the size of @data
 *
 * Creates a new [struct@GLib.Bytes] from @data.
 *
 * @data is copied. If @size is 0, @data may be `NULL`.
 *
 * As an optimization, [ctor@GLib.Bytes.new] may avoid an extra allocation by
 * copying the data within the resulting bytes structure if sufficiently small
 * (since GLib 2.84).
 *
 * Returns: (transfer full): a new [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_new (gconstpointer data,
             gsize         size)
{
  g_return_val_if_fail (data != NULL || size == 0, NULL);

  if (data == NULL || size == 0)
    {
      g_assert (data != NULL || size == 0);

      return g_bytes_new_with_free_func (NULL, size, NULL, NULL);
    }

  if (size <= G_BYTES_MAX_INLINE)
    {
      GBytesInline *bytes;

      bytes = g_malloc (sizeof *bytes + size);
      bytes->bytes.data = bytes->inline_data;
      bytes->bytes.size = size;
      bytes->bytes.free_func = NULL;
      bytes->bytes.user_data = NULL;
      g_atomic_ref_count_init (&bytes->bytes.ref_count);

      memcpy (bytes->inline_data, data, size);

      return (GBytes *)bytes;
    }

  return g_bytes_new_take (g_memdup2 (data, size), size);
}

/**
 * g_bytes_new_take:
 * @data: (transfer full) (array length=size) (element-type guint8) (nullable):
 *   the data to be used for the bytes
 * @size: the size of @data
 *
 * Creates a new [struct@GLib.Bytes] from @data.
 *
 * After this call, @data belongs to the `GBytes` and may no longer be
 * modified by the caller. The memory of @data has to be dynamically
 * allocated and will eventually be freed with [func@GLib.free].
 *
 * For creating `GBytes` with memory from other allocators, see
 * [ctor@GLib.Bytes.new_with_free_func].
 *
 * @data may be `NULL` if @size is 0.
 *
 * Returns: (transfer full): a new [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_new_take (gpointer data,
                  gsize    size)
{
  return g_bytes_new_with_free_func (data, size, g_free, data);
}


/**
 * g_bytes_new_static: (skip)
 * @data: (transfer full) (array length=size) (element-type guint8) (nullable):
 *   the data to be used for the bytes
 * @size: the size of @data
 *
 * Creates a new [struct@GLib.Bytes] from static data.
 *
 * @data must be static (ie: never modified or freed). It may be `NULL` if @size
 * is 0.
 *
 * Returns: (transfer full): a new [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_new_static (gconstpointer data,
                    gsize         size)
{
  return g_bytes_new_with_free_func (data, size, NULL, NULL);
}

/**
 * g_bytes_new_with_free_func: (skip)
 * @data: (array length=size) (element-type guint8) (nullable):
 *   the data to be used for the bytes
 * @size: the size of @data
 * @free_func: the function to call to release the data
 * @user_data: data to pass to @free_func
 *
 * Creates a [struct@GLib.Bytes] from @data.
 *
 * When the last reference is dropped, @free_func will be called with the
 * @user_data argument.
 *
 * @data must not be modified after this call is made until @free_func has
 * been called to indicate that the bytes is no longer in use.
 *
 * @data may be `NULL` if @size is 0.
 *
 * Returns: (transfer full): a new [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_new_with_free_func (gconstpointer  data,
                            gsize          size,
                            GDestroyNotify free_func,
                            gpointer       user_data)
{
  GBytes *bytes;

  g_return_val_if_fail (data != NULL || size == 0, NULL);

  bytes = g_new (GBytes, 1);
  bytes->data = data;
  bytes->size = size;
  bytes->free_func = free_func;
  bytes->user_data = user_data;
  g_atomic_ref_count_init (&bytes->ref_count);

  return (GBytes *)bytes;
}

/**
 * g_bytes_new_from_bytes:
 * @bytes: a [struct@GLib.Bytes]
 * @offset: offset which subsection starts at
 * @length: length of subsection
 *
 * Creates a [struct@GLib.Bytes] which is a subsection of another `GBytes`.
 *
 * The @offset + @length may not be longer than the size of @bytes.
 *
 * A reference to @bytes will be held by the newly created `GBytes` until
 * the byte data is no longer needed.
 *
 * Since 2.56, if @offset is 0 and @length matches the size of @bytes, then
 * @bytes will be returned with the reference count incremented by 1. If @bytes
 * is a slice of another `GBytes`, then the resulting `GBytes` will reference
 * the same `GBytes` instead of @bytes. This allows consumers to simplify the
 * usage of `GBytes` when asynchronously writing to streams.
 *
 * Returns: (transfer full): a new [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_new_from_bytes (GBytes  *bytes,
                        gsize    offset,
                        gsize    length)
{
  gchar *base;

  /* Note that length may be 0. */
  g_return_val_if_fail (bytes != NULL, NULL);
  g_return_val_if_fail (offset <= bytes->size, NULL);
  g_return_val_if_fail (offset + length <= bytes->size, NULL);

  /* Avoid an extra GBytes if all bytes were requested */
  if (offset == 0 && length == bytes->size)
    return g_bytes_ref (bytes);

  base = (gchar *)bytes->data + offset;

  /* Avoid referencing intermediate GBytes. In practice, this should
   * only loop once.
   */
  while (bytes->free_func == (gpointer)g_bytes_unref)
    bytes = bytes->user_data;

  g_return_val_if_fail (bytes != NULL, NULL);
  g_return_val_if_fail (base >= (gchar *)bytes->data, NULL);
  g_return_val_if_fail (base <= (gchar *)bytes->data + bytes->size, NULL);
  g_return_val_if_fail (base + length <= (gchar *)bytes->data + bytes->size, NULL);

  return g_bytes_new_with_free_func (base, length,
                                     (GDestroyNotify)g_bytes_unref, g_bytes_ref (bytes));
}

/**
 * g_bytes_get_data:
 * @bytes: a [struct@GLib.Bytes]
 * @size: (out) (optional): location to return size of byte data
 *
 * Get the byte data in the [struct@GLib.Bytes].
 *
 * This data should not be modified.
 *
 * This function will always return the same pointer for a given `GBytes`.
 *
 * `NULL` may be returned if @size is 0. This is not guaranteed, as the `GBytes`
 * may represent an empty string with @data non-`NULL` and @size as 0. `NULL`
 * will not be returned if @size is non-zero.
 *
 * Returns: (transfer none) (array length=size) (element-type guint8) (nullable):
 *   a pointer to the byte data
 * Since: 2.32
 */
gconstpointer
g_bytes_get_data (GBytes *bytes,
                  gsize *size)
{
  g_return_val_if_fail (bytes != NULL, NULL);
  if (size)
    *size = bytes->size;
  return bytes->data;
}

/**
 * g_bytes_get_size:
 * @bytes: a [struct@GLib.Bytes]
 *
 * Get the size of the byte data in the [struct@GLib.Bytes].
 *
 * This function will always return the same value for a given `GBytes`.
 *
 * Returns: the size
 * Since: 2.32
 */
gsize
g_bytes_get_size (GBytes *bytes)
{
  g_return_val_if_fail (bytes != NULL, 0);
  return bytes->size;
}


/**
 * g_bytes_ref:
 * @bytes: a [struct@GLib.Bytes]
 *
 * Increase the reference count on @bytes.
 *
 * Returns: the [struct@GLib.Bytes]
 * Since: 2.32
 */
GBytes *
g_bytes_ref (GBytes *bytes)
{
  g_return_val_if_fail (bytes != NULL, NULL);

  g_atomic_ref_count_inc (&bytes->ref_count);

  return bytes;
}

/**
 * g_bytes_unref:
 * @bytes: (nullable): a [struct@GLib.Bytes]
 *
 * Releases a reference on @bytes.
 *
 * This may result in the bytes being freed. If @bytes is `NULL`, it will
 * return immediately.
 *
 * Since: 2.32
 */
void
g_bytes_unref (GBytes *bytes)
{
  if (bytes == NULL)
    return;

  if (g_atomic_ref_count_dec (&bytes->ref_count))
    {
      if (bytes->free_func != NULL)
        bytes->free_func (bytes->user_data);
      g_free (bytes);
    }
}

/**
 * g_bytes_equal:
 * @bytes1: (type GLib.Bytes): a pointer to a [struct@GLib.Bytes]
 * @bytes2: (type GLib.Bytes): a pointer to a [struct@GLib.Bytes] to compare with @bytes1
 *
 * Compares the two [struct@GLib.Bytes] values being pointed to and returns
 * `TRUE` if they are equal.
 *
 * This function can be passed to [func@GLib.HashTable.new] as the
 * @key_equal_func parameter, when using non-`NULL` `GBytes` pointers as keys in
 * a [struct@GLib.HashTable].
 *
 * Returns: `TRUE` if the two keys match.
 * Since: 2.32
 */
gboolean
g_bytes_equal (gconstpointer bytes1,
               gconstpointer bytes2)
{
  const GBytes *b1 = bytes1;
  const GBytes *b2 = bytes2;

  g_return_val_if_fail (bytes1 != NULL, FALSE);
  g_return_val_if_fail (bytes2 != NULL, FALSE);

  return b1->size == b2->size &&
         (b1->size == 0 || memcmp (b1->data, b2->data, b1->size) == 0);
}

/**
 * g_bytes_hash:
 * @bytes: (type GLib.Bytes): a pointer to a [struct@GLib.Bytes] key
 *
 * Creates an integer hash code for the byte data in the [struct@GLib.Bytes].
 *
 * This function can be passed to [func@GLib.HashTable.new] as the
 * @key_hash_func parameter, when using non-`NULL` `GBytes` pointers as keys in
 * a [struct@GLib.HashTable].
 *
 * Returns: a hash value corresponding to the key.
 * Since: 2.32
 */
guint
g_bytes_hash (gconstpointer bytes)
{
  const GBytes *a = bytes;
  const signed char *p, *e;
  guint32 h = 5381;

  g_return_val_if_fail (bytes != NULL, 0);

  for (p = (signed char *)a->data, e = (signed char *)a->data + a->size; p != e; p++)
    h = (h << 5) + h + *p;

  return h;
}

/**
 * g_bytes_compare:
 * @bytes1: (type GLib.Bytes): a pointer to a [struct@GLib.Bytes]
 * @bytes2: (type GLib.Bytes): a pointer to a [struct@GLib.Bytes] to compare with @bytes1
 *
 * Compares the two [struct@GLib.Bytes] values.
 *
 * This function can be used to sort `GBytes` instances in lexicographical
 * order.
 *
 * If @bytes1 and @bytes2 have different length but the shorter one is a
 * prefix of the longer one then the shorter one is considered to be less than
 * the longer one. Otherwise the first byte where both differ is used for
 * comparison. If @bytes1 has a smaller value at that position it is
 * considered less, otherwise greater than @bytes2.
 *
 * Returns: a negative value if @bytes1 is less than @bytes2, a positive value
 *   if @bytes1 is greater than @bytes2, and zero if @bytes1 is equal to @bytes2
 * Since: 2.32
 */
gint
g_bytes_compare (gconstpointer bytes1,
                 gconstpointer bytes2)
{
  const GBytes *b1 = bytes1;
  const GBytes *b2 = bytes2;
  gint ret;

  g_return_val_if_fail (bytes1 != NULL, 0);
  g_return_val_if_fail (bytes2 != NULL, 0);

  ret = memcmp (b1->data, b2->data, MIN (b1->size, b2->size));
  if (ret == 0 && b1->size != b2->size)
      ret = b1->size < b2->size ? -1 : 1;
  return ret;
}

static gpointer
try_steal_and_unref (GBytes         *bytes,
                     GDestroyNotify  free_func,
                     gsize          *size)
{
  gpointer result;

  if (bytes->free_func != free_func || bytes->data == NULL ||
      bytes->user_data != bytes->data)
    return NULL;

  /* Are we the only reference? */
  if (g_atomic_ref_count_compare (&bytes->ref_count, 1))
    {
      *size = bytes->size;
      result = (gpointer)bytes->data;
      g_assert (result != NULL);  /* otherwise the case of @bytes being freed can’t be distinguished */
      g_free (bytes);
      return result;
    }

  return NULL;
}


/**
 * g_bytes_unref_to_data:
 * @bytes: (transfer full): a [struct@GLib.Bytes]
 * @size: (out): location to place the length of the returned data
 *
 * Unreferences the bytes, and returns a pointer the same byte data
 * contents.
 *
 * As an optimization, the byte data is returned without copying if this was
 * the last reference to @bytes and @bytes was created with
 * [ctor@GLib.Bytes.new], [ctor@GLib.Bytes.new_take] or
 * [func@GLib.ByteArray.free_to_bytes] and the buffer was larger than the size
 * [struct@GLib.Bytes] may internalize within its allocation. In all other cases
 * the data is copied.
 *
 * Returns: (transfer full) (array length=size) (element-type guint8) (not nullable):
 *   a pointer to the same byte data, which should be freed with [func@GLib.free]
 * Since: 2.32
 */
gpointer
g_bytes_unref_to_data (GBytes *bytes,
                       gsize  *size)
{
  gpointer result;

  g_return_val_if_fail (bytes != NULL, NULL);
  g_return_val_if_fail (size != NULL, NULL);

  /*
   * Optimal path: if this is was the last reference, then we can return
   * the data from this GBytes without copying.
   */

  result = try_steal_and_unref (bytes, g_free, size);
  if (result == NULL)
    {
      /*
       * Copy: Non g_malloc (or compatible) allocator, or static memory,
       * so we have to copy, and then unref.
       */
      result = g_memdup2 (bytes->data, bytes->size);
      *size = bytes->size;
      g_bytes_unref (bytes);
    }

  return result;
}

/**
 * g_bytes_unref_to_array:
 * @bytes: (transfer full): a [struct@GLib.Bytes]
 *
 * Unreferences the bytes, and returns a new mutable [struct@GLib.ByteArray]
 * containing the same byte data.
 *
 * As an optimization, the byte data is transferred to the array without copying
 * if this was the last reference to @bytes and @bytes was created with
 * [ctor@GLib.Bytes.new], [ctor@GLib.Bytes.new_take] or
 * [func@GLib.ByteArray.free_to_bytes] and the buffer was larger than the size
 * [struct@GLib.Bytes] may internalize within its allocation. In all other cases
 * the data is copied.
 *
 * Do not use it if @bytes contains more than %G_MAXUINT
 * bytes. [struct@GLib.ByteArray] stores the length of its data in `guint`,
 * which may be shorter than `gsize`, that @bytes is using.
 *
 * Returns: (transfer full): a new mutable [struct@GLib.ByteArray] containing
 *   the same byte data
 * Since: 2.32
 */
GByteArray *
g_bytes_unref_to_array (GBytes *bytes)
{
  gpointer data;
  gsize size;

  g_return_val_if_fail (bytes != NULL, NULL);

  data = g_bytes_unref_to_data (bytes, &size);
  return g_byte_array_new_take (data, size);
}

/**
 * g_bytes_get_region:
 * @bytes: a [struct@GLib.Bytes]
 * @element_size: a non-zero element size
 * @offset: an offset to the start of the region within the @bytes
 * @n_elements: the number of elements in the region
 *
 * Gets a pointer to a region in @bytes.
 *
 * The region starts at @offset many bytes from the start of the data
 * and contains @n_elements many elements of @element_size size.
 *
 * @n_elements may be zero, but @element_size must always be non-zero.
 * Ideally, @element_size is a static constant (eg: `sizeof` a struct).
 *
 * This function does careful bounds checking (including checking for
 * arithmetic overflows) and returns a non-`NULL` pointer if the
 * specified region lies entirely within the @bytes. If the region is
 * in some way out of range, or if an overflow has occurred, then `NULL`
 * is returned.
 *
 * Note: it is possible to have a valid zero-size region. In this case,
 * the returned pointer will be equal to the base pointer of the data of
 * @bytes, plus @offset.  This will be non-`NULL` except for the case
 * where @bytes itself was a zero-sized region.  Since it is unlikely
 * that you will be using this function to check for a zero-sized region
 * in a zero-sized @bytes, `NULL` effectively always means ‘error’.
 *
 * Returns: (nullable): the requested region, or `NULL` in case of an error
 * Since: 2.70
 */
gconstpointer
g_bytes_get_region (GBytes *bytes,
                    gsize   element_size,
                    gsize   offset,
                    gsize   n_elements)
{
  gsize total_size;
  gsize end_offset;

  g_return_val_if_fail (element_size > 0, NULL);

  /* No other assertion checks here.  If something is wrong then we will
   * simply crash (via NULL dereference or divide-by-zero).
   */

  if (!g_size_checked_mul (&total_size, element_size, n_elements))
    return NULL;

  if (!g_size_checked_add (&end_offset, offset, total_size))
    return NULL;

  /* We now have:
   *
   *   0 <= offset <= end_offset
   *
   * So we need only check that end_offset is within the range of the
   * size of @bytes and we're good to go.
   */

  if (end_offset > bytes->size)
    return NULL;

  /* We now have:
   *
   *   0 <= offset <= end_offset <= bytes->size
   */

  return ((guchar *) bytes->data) + offset;
}
