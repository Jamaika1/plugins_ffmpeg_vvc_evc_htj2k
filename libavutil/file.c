/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/config.h"
#include "libavutil/error.h"
#include "libavutil/file.h"
#include "libavutil/file_open.h"
#include "libavutil/internal.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include <fcntl.h>
#include <sys/stat.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_MMAP
#include <sys/mman.h>
#elif HAVE_MAPVIEWOFFILE
#include <windows.h>
#endif

typedef struct FileLogContext {
    const AVClass *class;
    int   log_offset;
    void *log_ctx;
} FileLogContext;

static const AVClass file_log_ctx_class = {
    .class_name                = "FILE",
    .item_name                 = av_default_item_name,
    .option                    = NULL,
    .version                   = LIBAVUTIL_VERSION_INT,
    .log_level_offset_offset   = offsetof(FileLogContext, log_offset),
    .parent_log_context_offset = offsetof(FileLogContext, log_ctx),
};

int av_file_map(const char *filename, uint8_t **bufptr, size_t *size,
                int log_offset, void *log_ctx)
{
    FileLogContext file_log_ctx = { &file_log_ctx_class, log_offset, log_ctx };
    int err, fd = avpriv_open(filename, O_RDONLY);
    struct stat st;
    av_unused void *ptr;
    off_t off_size;
    *bufptr = NULL;
    *size = 0;

    if (fd < 0) {
        err = AVERROR(errno);
        av_log(&file_log_ctx, AV_LOG_ERROR, "Cannot read file '%s': %s\n", filename, av_err2str(err));
        return err;
    }

    if (fstat(fd, &st) < 0) {
        err = AVERROR(errno);
        av_log(&file_log_ctx, AV_LOG_ERROR, "Error occurred in fstat(): %s\n", av_err2str(err));
        close(fd);
        return err;
    }

    off_size = st.st_size;
    if (off_size > SIZE_MAX) {
        av_log(&file_log_ctx, AV_LOG_ERROR,
               "File size for file '%s' is too big\n", filename);
        close(fd);
        return AVERROR(EINVAL);
    }
    *size = off_size;

    if (!*size) {
        *bufptr = NULL;
        goto out;
    }

#if HAVE_MMAP
    ptr = mmap(NULL, *size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        err = AVERROR(errno);
        av_log(&file_log_ctx, AV_LOG_ERROR, "Error occurred in mmap(): %s\n", av_err2str(err));
        close(fd);
        *size = 0;
        return err;
    }
    *bufptr = ptr;
#elif HAVE_MAPVIEWOFFILE
    {
        HANDLE mh, fh = (HANDLE)_get_osfhandle(fd);

        mh = CreateFileMapping(fh, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!mh) {
            av_log(&file_log_ctx, AV_LOG_ERROR, "Error occurred in CreateFileMapping()\n");
            close(fd);
            *size = 0;
            return -1;
        }

        ptr = MapViewOfFile(mh, FILE_MAP_COPY, 0, 0, *size);
        CloseHandle(mh);
        if (!ptr) {
            av_log(&file_log_ctx, AV_LOG_ERROR, "Error occurred in MapViewOfFile()\n");
            close(fd);
            *size = 0;
            return -1;
        }

        *bufptr = ptr;
    }
#else
    *bufptr = av_malloc(*size);
    if (!*bufptr) {
        av_log(&file_log_ctx, AV_LOG_ERROR, "Memory allocation error occurred\n");
        close(fd);
        *size = 0;
        return AVERROR(ENOMEM);
    }
    read(fd, *bufptr, *size);
#endif

out:
    close(fd);
    return 0;
}

void av_file_unmap(uint8_t *bufptr, size_t size)
{
    if (!size || !bufptr)
        return;
#if HAVE_MMAP
    munmap(bufptr, size);
#elif HAVE_MAPVIEWOFFILE
    UnmapViewOfFile(bufptr);
#else
    av_free(bufptr);
#endif
}
