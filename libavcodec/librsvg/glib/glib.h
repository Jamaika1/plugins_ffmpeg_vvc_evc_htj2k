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

#ifndef __G_LIB_H__
#define __G_LIB_H__

#define __GLIB_H_INSIDE__

#include "galloca.h"
#include "garray.h"
#include "gasyncqueue.h"
#include "gatomic.h"
#include "gbacktrace.h"
#include "gbase64.h"
#include "gbitlock.h"
#include "gbookmarkfile.h"
#include "gbytes.h"
#include "gcharset.h"
#include "gchecksum.h"
#include "gconvert.h"
#include "gdataset.h"
#include "gdate.h"
#include "gdatetime.h"
#include "gdir.h"
#include "genviron.h"
#include "gerror.h"
#include "gfileutils.h"
#include "ggettext.h"
#include "ghash.h"
#include "ghmac.h"
#include "ghook.h"
#include "ghostutils.h"
#include "giochannel.h"
#include "gkeyfile.h"
#include "glist.h"
#include "gmacros.h"
#include "gmain.h"
#include "gmappedfile.h"
#include "gmarkup.h"
#include "gmem.h"
#include "gmessages.h"
#include "gnode.h"
#include "goption.h"
#include "gpathbuf.h"
#include "gpattern.h"
#include "gpoll.h"
#include "gprimes.h"
#include "gqsort.h"
#include "gquark.h"
#include "gqueue.h"
#include "grand.h"
#include "grcbox.h"
#include "grefcount.h"
#include "grefstring.h"
#include "gregex.h"
#include "gscanner.h"
#include "gsequence.h"
#include "gshell.h"
#include "gslice.h"
#include "gslist.h"
#include "gspawn.h"
#include "gstrfuncs.h"
#include "gstringchunk.h"
#include "gstring.h"
#include "gstrvbuilder.h"
#include "gtestutils.h"
#include "gthread.h"
#include "gthreadpool.h"
#include "gtimer.h"
#include "gtimezone.h"
#include "gtrashstack.h"
#include "gtree.h"
#include "gtypes.h"
#include "gunicode.h"
#include "guri.h"
#include "gutils.h"
#include "guuid.h"
#include "gvariant.h"
#include "gvarianttype.h"
#include "gversion.h"
#include "gversionmacros.h"

#ifdef G_PLATFORM_WIN32
#include "gwin32.h"
#endif

#include "deprecated/gallocator.h"
#include "deprecated/gcache.h"
#include "deprecated/gcompletion.h"
#include "deprecated/gmain.h"
#include "deprecated/grel.h"
#include "deprecated/gthread.h"

#include "glib-autocleanups.h"
#include "glib-typeof.h"

#undef __GLIB_H_INSIDE__

#endif /* __G_LIB_H__ */
