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

/**
 * @file
 * Libavfilter multithreading support
 */

#include <stddef.h>

#include "libavutil/error.h"
#include "libavutil/macros.h"
#include "libavutil/mem.h"
#include "libavutil/slicethread.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/avfilter_internal.h"

typedef struct ThreadContext {
    AVFilterGraph *graph;
    AVSliceThread *thread;
    avfilter_action_func *func;

    /* per-execute parameters */
    AVFilterContext *ctx;
    void *arg;
    int   *rets;
} ThreadContext;

static void worker_func(void *priv, int jobnr, int threadnr, int nb_jobs, int nb_threads)
{
    ThreadContext *c = priv;
    int ret = c->func(c->ctx, c->arg, jobnr, nb_jobs);
    if (c->rets)
        c->rets[jobnr] = ret;
}

static void slice_thread_uninit(ThreadContext *c)
{
    avpriv_slicethread_free(&c->thread);
}

static int thread_execute(AVFilterContext *ctx, avfilter_action_func *func,
                          void *arg, int *ret, int nb_jobs)
{
    ThreadContext *c = fffiltergraph(ctx->graph)->thread;

    if (nb_jobs <= 0)
        return 0;
    c->ctx         = ctx;
    c->arg         = arg;
    c->func        = func;
    c->rets        = ret;

    avpriv_slicethread_execute(c->thread, nb_jobs, 0);
    return 0;
}

static int thread_init_internal(ThreadContext *c, int nb_threads)
{
    nb_threads = avpriv_slicethread_create(&c->thread, c, worker_func, NULL, nb_threads);
    if (nb_threads <= 1)
        avpriv_slicethread_free(&c->thread);
    return FFMAX(nb_threads, 1);
}

int ff_graph_thread_init(FFFilterGraph *graphi)
{
    AVFilterGraph *graph = &graphi->p;
    int ret;

    if (graph->nb_threads == 1) {
        graph->thread_type = 0;
        return 0;
    }

    graphi->thread = av_mallocz(sizeof(ThreadContext));
    if (!graphi->thread)
        return AVERROR(ENOMEM);

    ret = thread_init_internal(graphi->thread, graph->nb_threads);
    if (ret <= 1) {
        av_freep(&graphi->thread);
        graph->thread_type = 0;
        graph->nb_threads  = 1;
        return (ret < 0) ? ret : 0;
    }
    graph->nb_threads = ret;

    graphi->thread_execute = thread_execute;

    return 0;
}

void ff_graph_thread_free(FFFilterGraph *graph)
{
    if (graph->thread)
        slice_thread_uninit(graph->thread);
    av_freep(&graph->thread);
}
