/*****************************************************************************
 * Copyright (C) 2013-2017 MulticoreWare, Inc
 *
 * Authors: Steve Borho <steve@borho.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at license @ x265.com.
 *****************************************************************************/

#ifndef X265_CONFIG_H
#define X265_CONFIG_H

/* Defines generated at build time */

/* Incremented each time public API is changed, X265_BUILD is used as
 * the shared library SONAME on platforms which support it. It also
 * prevents linking against a different version of the static lib */
#define X265_BUILD 215
#define X265_VERSION 4.1+188-cd4f0d6

#define ENABLE_HDR10_PLUS 0
#define ENABLE_ASSEMBLY 1
#define ENABLE_LIBVMAF 0
#define ENABLE_ALPHA 1
#define ENABLE_MULTIVIEW 1
#define ENABLE_SCC_EXT 1
#define HAVE_STRTOK_R

#define _DEBUG 1

#endif
