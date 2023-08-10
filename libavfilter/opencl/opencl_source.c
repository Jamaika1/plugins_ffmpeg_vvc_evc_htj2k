//#include "libavfilter/avfilter.h"
#include <stddef.h>
#include "opencl_source.h"

const char *ff_source_avgblur_cl;
const char *ff_source_colorkey_cl;
const char *ff_source_colorspace_common_cl;
const char *ff_source_convolution_cl;
const char *ff_source_deshake_cl;
const char *ff_source_neighbor_cl;
const char *ff_source_nlmeans_cl;
const char *ff_source_overlay_cl;
const char *ff_source_pad_cl;
const char *ff_source_remap_cl;
const char *ff_source_tonemap_cl;
const char *ff_source_transpose_cl;
const char *ff_source_unsharp_cl;
const char *ff_source_xfade_cl;

/*static const char** const opencl_source[] = {
&ff_source_avgblur_cl,
&ff_source_colorkey_cl,
&ff_source_colorspace_common_cl,
&ff_source_convolution_cl,
&ff_source_deshake_cl,
&ff_source_neighbor_cl,
&ff_source_nlmeans_cl,
&ff_source_overlay_cl,
&ff_source_pad_cl,
&ff_source_remap_cl,
&ff_source_tonemap_cl,
&ff_source_transpose_cl,
&ff_source_unsharp_cl,
&ff_source_xfade_cl,
NULL};*/

/*const AVFilter *av_filter_iterate(void **opaque)
{
    uintptr_t i = (uintptr_t)*opaque;
    const AVFilter *f = opencl_source[i];

    if (f)
        *opaque = (void*)(i + 1);

    return f;
}

const AVFilter *avfilter_get_by_name(const char *name)
{
    const AVFilter *f = NULL;
    void *opaque = 0;

    if (!name)
        return NULL;

    while ((f = av_filter_iterate(&opaque)))
        if (!strcmp(f->name, name))
            return f;

    return NULL;
}*/
