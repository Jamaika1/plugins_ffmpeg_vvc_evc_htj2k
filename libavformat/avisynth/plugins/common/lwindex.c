/*****************************************************************************
 * lwindex.c / lwindex.cpp
 *****************************************************************************
 * Copyright (C) 2012-2015 L-SMASH Works project
 *
 * Authors: Yusuke Nakamura <muken.the.vfrmaniac@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *****************************************************************************/

/* This file is available under an ISC license. */

#ifndef _WIN32
// GNU SOURCE macro for realpath
#define _GNU_SOURCE
#endif

#include <sys/stat.h>
#include "xxhash.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <libavutil/pixdesc.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "decode.h"
#include "lwindex_parser.h"
#include "lwindex_utils.h"

typedef struct {
    lwlibav_extradata_handler_t exh;
    AVCodecContext* codec_ctx;
    AVCodecParserContext* parser_ctx;
    const AVBitStreamFilter* bsf;
    AVBSFContext* bsf_ctx;
    AVFrame* picture;
    AVPacket pkt;
    uint32_t delay_count;
    lw_field_info_t last_field_info;
    int mpeg12_video; /* 0: neither MPEG-1 Video nor MPEG-2 Video
                       * 1: either MPEG-1 Video or MPEG-2 Video */
    int vc1_wmv3; /* 0: neither VC-1 nor WMV3
                   * 1: either VC-1 or WMV3
                   * 2: either VC-1 or WMV3 encapsulated in ASF */
    int already_decoded;
    int (*decode)(AVCodecContext*, AVFrame*, int*, AVPacket*);
} lwindex_helper_t;

typedef struct {
    int number_of_helpers;
    lwindex_helper_t** helpers;
    const char** preferred_video_decoder_names;
    int* prefer_video_hw_decoder;
    const char** preferred_audio_decoder_names;
    int thread_count;
    char* format_name;
    AVBufferRef* hw_device_ctx;
} lwindex_indexer_t;

typedef struct {
    int codec_type;
    int codec_id;
    AVRational time_base;
    int width;
    int height;
    char fmt[64];
    int colorspace;
    int channels;
    uint64_t layout;
    int sample_rate;
    int bits_per_sample;
} lwindex_stream_info_t;

typedef struct {
    int64_t pts;
    int64_t dts;
} video_timestamp_t;

typedef struct {
    video_timestamp_t temp;
    video_timestamp_t core;
} video_timestamp_temp_t;

static inline int check_frame_reordering(video_frame_info_t* info, uint32_t sample_count)
{
    for (uint32_t i = 2; i <= sample_count; i++)
        if (info[i].pts < info[i - 1].pts)
            return 1;
    return 0;
}

static int compare_info_pts(const video_frame_info_t* a, const video_frame_info_t* b)
{
    int64_t diff = (int64_t)(a->pts - b->pts);
    return diff > 0 ? 1 : (diff == 0 ? 0 : -1);
}

static int compare_pts(const video_timestamp_t* a, const video_timestamp_t* b)
{
    int64_t diff = (int64_t)(a->pts - b->pts);
    return diff > 0 ? 1 : (diff == 0 ? 0 : -1);
}

static int compare_dts(const video_timestamp_t* a, const video_timestamp_t* b)
{
    int64_t diff = (int64_t)(a->dts - b->dts);
    return diff > 0 ? 1 : (diff == 0 ? 0 : -1);
}

static inline void sort_info_presentation_order(video_frame_info_t* info, uint32_t sample_count)
{
    qsort(info, sample_count, sizeof(video_frame_info_t), (int (*)(const void*, const void*))compare_info_pts);
}

static inline void sort_presentation_order(video_timestamp_t* timestamp, uint32_t sample_count, size_t size)
{
    qsort(timestamp, sample_count, size, (int (*)(const void*, const void*))compare_pts);
}

static inline void sort_decoding_order(video_timestamp_t* timestamp, uint32_t sample_count, size_t size)
{
    qsort(timestamp, sample_count, size, (int (*)(const void*, const void*))compare_dts);
}

static inline int lineup_seek_base_candidates(lwlibav_file_handler_t* lwhp)
{
    return !strcmp(lwhp->format_name, "mpeg") || !strcmp(lwhp->format_name, "mpegts") || lwhp->raw_demuxer
        ? SEEK_DTS_BASED | SEEK_PTS_BASED | SEEK_POS_BASED | SEEK_POS_CORRECTION
        : SEEK_DTS_BASED | SEEK_PTS_BASED | SEEK_POS_CORRECTION;
}

/* This function generates PTSs from DTSs by picture types.
 * Note that this function does not work for MPEG-4 Video Part2 with
 * packed bitstream since P-picture precedes B-pictures whithin packet
 * and the libavcodec's parser recognizes the packet as a P-picture.
 * The bitstream filter mpeg4_unpack_bframes unpacks B-pictures from
 * packed bitstream and is a solution to this problem. */
static void mpeg124_video_vc1_genarate_pts(lwlibav_video_decode_handler_t* vdhp)
{
    video_frame_info_t* info = vdhp->frame_list;
    int reordered_stream = 0;
    uint32_t num_consecutive_b = 0;
    for (uint32_t i = 1; i <= vdhp->frame_count; i++) {
        /* In the case where B-pictures exist
         * Decode order
         *      I[1]P[2]P[3]B[4]B[5]P[6]...
         * DTS
         *        0   1   2   3   4   5 ...
         * Presentation order
         *      I[1]P[2]B[4]B[5]P[3]P[6]...
         * PTS
         *        1   2   3   4   5   6 ...
         * We assume B-pictures always be present in the stream here. */
        if ((enum AVPictureType)info[i].pict_type == AV_PICTURE_TYPE_B) {
            /* B-pictures shall be output or displayed in the same order as they are encoded. */
            info[i].pts = info[i].dts;
            ++num_consecutive_b;
            reordered_stream = 1;
        } else {
            /* Apply DTS of the current picture to PTS of the last I- or P-picture. */
            if (i > num_consecutive_b + 1)
                info[i - num_consecutive_b - 1].pts = info[i].dts;
            num_consecutive_b = 0;
        }
    }
    if (reordered_stream && num_consecutive_b != vdhp->frame_count) {
        /* Check if any duplicated PTS. */
        uint32_t flush_number = vdhp->frame_count - num_consecutive_b;
        int64_t* last_pts = &info[flush_number].pts;
        if (*last_pts != AV_NOPTS_VALUE)
            for (uint32_t i = vdhp->frame_count; i && *last_pts >= info[i].dts; i--)
                if (*last_pts == info[i].pts && i != flush_number)
                    *last_pts = AV_NOPTS_VALUE;
        if (*last_pts == AV_NOPTS_VALUE) {
            /* Estimate PTS of the last displayed picture. */
            int64_t duration = info[vdhp->frame_count].dts - info[vdhp->frame_count - 1].dts;
            *last_pts = info[vdhp->frame_count].dts + duration;
        }
        /* Check leading B-pictures. */
        int64_t last_keyframe_pts = AV_NOPTS_VALUE;
        for (uint32_t i = 1; i <= vdhp->frame_count; i++) {
            if (info[i].pts != AV_NOPTS_VALUE && last_keyframe_pts != AV_NOPTS_VALUE && info[i].pts < last_keyframe_pts)
                info[i].flags |= LW_VFRAME_FLAG_LEADING;
            if (info[i].flags & LW_VFRAME_FLAG_KEY)
                last_keyframe_pts = info[i].pts;
        }
    } else
        for (uint32_t i = 1; i <= vdhp->frame_count; i++)
            info[i].pts = info[i].dts;
}

static void interpolate_pts(video_frame_info_t* info, /* 0-origin */
    video_timestamp_temp_t* timestamp, /* 0-origin */
    uint32_t frame_count, AVRational time_base, uint64_t max_composition_delay)
{
    /* Find the first valid PTS. */
    uint32_t valid_start = UINT32_MAX;
    for (uint32_t i = 0; i < frame_count; i++)
        if (timestamp[i].core.pts != AV_NOPTS_VALUE)
            valid_start = i;
    if (valid_start != UINT32_MAX) {
        /* Generate PTSs. */
        for (uint32_t i = valid_start; i; i--)
            timestamp[i - 1].core.pts = timestamp[i].core.pts - time_base.num;
        while (valid_start < frame_count) {
            /* Find the next valid PTS. */
            uint32_t valid_end = UINT32_MAX;
            for (uint32_t i = valid_start + 1; i < frame_count; i++)
                if (timestamp[i].core.pts != AV_NOPTS_VALUE && timestamp[i].core.pts != timestamp[i - 1].core.pts)
                    valid_end = i;
            /* Interpolate PTSs roughly. */
            if (valid_end != UINT32_MAX)
                for (uint32_t i = valid_end; i > valid_start + 1; i--)
                    timestamp[i - 1].core.pts = timestamp[i].core.pts - time_base.num;
            else
                for (uint32_t i = valid_start + 1; i < frame_count; i++)
                    timestamp[i].core.pts = timestamp[i - 1].core.pts + time_base.num;
            valid_start = valid_end;
        }
    } else {
        if (max_composition_delay)
            /* Get the maximum composition delay derived from reordering. */
            for (uint32_t i = 0; i < frame_count; i++)
                if (i < timestamp[i].core.dts) {
                    uint64_t composition_delay = timestamp[i].core.dts - i;
                    max_composition_delay = MAX(max_composition_delay, composition_delay);
                }
        /* Generate PTSs. */
        timestamp[0].core.pts = max_composition_delay * time_base.num;
        for (uint32_t i = 1; i < frame_count; i++)
            timestamp[i].core.pts = timestamp[i - 1].core.pts + (info[i - 1].repeat_pict == 0 ? 1 : 2) * time_base.num;
    }
}

static void interpolate_dts(video_frame_info_t* info, /* 0-origin */
    uint32_t frame_count, AVRational time_base)
{
    /* Find the first valid DTS. */
    uint32_t valid_start = UINT32_MAX;
    for (uint32_t i = 0; i < frame_count; i++)
        if (info[i].dts != AV_NOPTS_VALUE)
            valid_start = i;
    if (valid_start != UINT32_MAX) {
        /* Generate DTSs. */
        for (uint32_t i = valid_start; i; i--)
            info[i - 1].dts = info[i].dts - time_base.num;
        while (valid_start < frame_count) {
            /* Find the next valid DTS. */
            uint32_t valid_end = UINT32_MAX;
            for (uint32_t i = valid_start + 1; i < frame_count; i++)
                if (info[i].dts != AV_NOPTS_VALUE && info[i].dts != info[i - 1].dts)
                    valid_end = i;
            /* Interpolate DTSs roughly. */
            if (valid_end != UINT32_MAX)
                for (uint32_t i = valid_end; i > valid_start + 1; i--)
                    info[i - 1].dts = info[i].dts - time_base.num;
            else
                for (uint32_t i = valid_start + 1; i < frame_count; i++)
                    info[i].dts = info[i - 1].dts + time_base.num;
            valid_start = valid_end;
        }
    } else {
        /* Generate DTSs. */
        info[0].dts = 0;
        for (uint32_t i = 1; i < frame_count; i++)
            info[i].dts = info[i - 1].dts + (info[i - 1].repeat_pict == 0 ? 1 : 2) * time_base.num;
    }
}

static int poc_genarate_pts(lwlibav_video_decode_handler_t* vdhp, int max_num_reorder_pics)
{
    video_frame_info_t* info = &vdhp->frame_list[1];
    /* Deduplicate POCs. */
    int64_t poc_offset = 0;
    int64_t poc_min = 0;
    int64_t invalid_poc_min = 0;
    uint32_t last_idr = UINT32_MAX;
    uint32_t invalid_poc_start = 0;
    uint32_t max_composition_delay = 0;
    int invalid_poc_present = 0;
    for (uint32_t i = 0;; i++) {
        if (i < vdhp->frame_count && info[i].poc != 0) {
            /* poc_offset is not added to each POC here.
             * It is done when we encounter the next coded video sequence. */
            if (info[i].poc < 0) {
                /* Pictures with negative POC shall precede IDR-picture in composition order.
                 * The minimum POC is added to poc_offset when we encounter the next coded video sequence. */
                if (last_idr == UINT32_MAX || i > last_idr + max_num_reorder_pics) {
                    if (!invalid_poc_present) {
                        invalid_poc_present = 1;
                        invalid_poc_start = i;
                    }
                    if (invalid_poc_min > info[i].poc)
                        invalid_poc_min = info[i].poc;
                } else if (poc_min > info[i].poc) {
                    poc_min = info[i].poc;
                    max_composition_delay = MAX(max_composition_delay, i - last_idr);
                }
            }
            continue;
        }
        /* Encountered a new coded video sequence or no more POCs.
         * Add poc_offset to each POC of the previous coded video sequence. */
        poc_offset -= poc_min;
        int64_t poc_max = 0;
        for (uint32_t j = last_idr; j < i; j++)
            if (info[j].poc >= 0 || (j <= last_idr + max_num_reorder_pics)) {
                info[j].poc += poc_offset;
                if (poc_max < info[j].poc)
                    poc_max = info[j].poc;
            }
        poc_offset = poc_max + 1;
        if (invalid_poc_present) {
            /* Pictures with invalid negative POC is probably supposed to be composited
             * both before the next coded video sequence and after the current one. */
            poc_offset -= invalid_poc_min;
            for (uint32_t j = invalid_poc_start; j < i; j++)
                if (info[j].poc < 0) {
                    info[j].poc += poc_offset;
                    if (poc_max < info[j].poc)
                        poc_max = info[j].poc;
                }
            invalid_poc_present = 0;
            invalid_poc_start = 0;
            invalid_poc_min = 0;
            poc_offset = poc_max + 1;
        }
        if (i < vdhp->frame_count) {
            poc_min = 0;
            last_idr = i;
        } else
            break; /* no more POCs */
    }
    /* Check if composition delay derived from reordering is present. */
    int composition_reordering_present;
    if (max_composition_delay == 0) {
        composition_reordering_present = 0;
        for (uint32_t i = 1; i < vdhp->frame_count; i++)
            if (info[i].poc < info[i - 1].poc) {
                composition_reordering_present = 1;
                break;
            }
    } else
        composition_reordering_present = 1;
    /* Generate timestamps. */
    video_timestamp_temp_t* timestamp = (video_timestamp_temp_t*)malloc(vdhp->frame_count * sizeof(video_timestamp_temp_t));
    if (!timestamp)
        return -1;
    for (uint32_t i = 0; i < vdhp->frame_count; i++) {
        timestamp[i].core.pts = info[i].pts;
        timestamp[i].core.dts = info[i].dts;
    }
    if (composition_reordering_present) {
        /* Here, PTSs are temporary values for sort. */
        for (uint32_t i = 0; i < vdhp->frame_count; i++) {
            timestamp[i].temp.pts = info[i].poc;
            timestamp[i].temp.dts = i;
        }
        sort_presentation_order(&timestamp[0].temp, vdhp->frame_count, sizeof(video_timestamp_temp_t));
        interpolate_pts(info, timestamp, vdhp->frame_count, vdhp->time_base, max_composition_delay);
        sort_decoding_order(&timestamp[0].temp, vdhp->frame_count, sizeof(video_timestamp_temp_t));
        /* Check leading pictures. */
        int64_t last_keyframe_pts = AV_NOPTS_VALUE;
        for (uint32_t i = 0; i < vdhp->frame_count; i++) {
            if (last_keyframe_pts != AV_NOPTS_VALUE && timestamp[i].core.pts < last_keyframe_pts)
                info[i].flags |= LW_VFRAME_FLAG_LEADING;
            if (info[i].flags & LW_VFRAME_FLAG_KEY)
                last_keyframe_pts = timestamp[i].core.pts;
        }
    } else
        interpolate_pts(info, timestamp, vdhp->frame_count, vdhp->time_base, 0);
    /* Set generated timestamps. */
    for (uint32_t i = 0; i < vdhp->frame_count; i++) {
        info[i].pts = timestamp[i].core.pts;
        info[i].dts = timestamp[i].core.dts;
    }
    free(timestamp);
    return 0;
}

static int decide_video_seek_method(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, uint32_t sample_count)
{
    vdhp->lw_seek_flags = lineup_seek_base_candidates(lwhp);
    video_frame_info_t* info = vdhp->frame_list;
    /* Decide seek base. */
    for (uint32_t i = 1; i <= sample_count; i++)
        if (info[i].pts == AV_NOPTS_VALUE) {
            vdhp->lw_seek_flags &= ~SEEK_PTS_BASED;
            break;
        }
    if (info[1].dts == AV_NOPTS_VALUE)
        vdhp->lw_seek_flags &= ~SEEK_DTS_BASED;
    else
        for (uint32_t i = 2; i <= sample_count; i++)
            if (!(info[i].flags & LW_VFRAME_FLAG_INVISIBLE) && (info[i].dts == AV_NOPTS_VALUE || info[i].dts <= info[i - 1].dts)) {
                vdhp->lw_seek_flags &= ~SEEK_DTS_BASED;
                break;
            }
    if (info[1].file_offset == -1)
        vdhp->lw_seek_flags &= ~SEEK_POS_CORRECTION;
    else
        for (uint32_t i = 2; i <= sample_count; i++)
            if (info[i].file_offset == -1 || info[i].file_offset <= info[i - 1].file_offset) {
                vdhp->lw_seek_flags &= ~SEEK_POS_CORRECTION;
                break;
            }
    if (vdhp->lw_seek_flags & SEEK_POS_BASED) {
        if (lwhp->format_flags & AVFMT_NO_BYTE_SEEK)
            vdhp->lw_seek_flags &= ~SEEK_POS_BASED;
        else {
            uint32_t error_count = 0;
            for (uint32_t i = 1; i <= sample_count; i++)
                error_count += (info[i].file_offset == -1);
            if (error_count == sample_count)
                vdhp->lw_seek_flags &= ~SEEK_POS_BASED;
        }
    }
    /* Construct frame info about timestamp. */
    int no_pts_loss = !!(vdhp->lw_seek_flags & SEEK_PTS_BASED);
    /* Assumption: Generating pts from dts for those codecs is always safe.
     * Previously, there is one additional condtion of the form:
        (lwhp->raw_demuxer || ((vdhp->lw_seek_flags & SEEK_DTS_BASED) && !(vdhp->lw_seek_flags & SEEK_PTS_BASED)))
     * However, this is unnecessarily strict. Why only interpolate pts for raw mpeg2 video, but
     * not video from mpeg2 program stream containers?
     * To avoid breaking too much, let's additionally allow mpeg2 program streams here. */
    if ((lwhp->raw_demuxer || !strcmp(lwhp->format_name, "mpeg") || ((vdhp->lw_seek_flags & SEEK_DTS_BASED) && !no_pts_loss))
        && (vdhp->codec_id == AV_CODEC_ID_MPEG1VIDEO || vdhp->codec_id == AV_CODEC_ID_MPEG2VIDEO
            || vdhp->codec_id == AV_CODEC_ID_MPEG4 /* MPEG-4 Video (Part2) */
            || vdhp->codec_id == AV_CODEC_ID_VC1 || vdhp->codec_id == AV_CODEC_ID_WMV3 || vdhp->codec_id == AV_CODEC_ID_VC1IMAGE
            || vdhp->codec_id == AV_CODEC_ID_WMV3IMAGE)) {
        /* Generate or interpolate DTS if any invalid DTS for each frame. */
        if (!(vdhp->lw_seek_flags & SEEK_DTS_BASED))
            interpolate_dts(&info[1], vdhp->frame_count, vdhp->time_base);
        /* Generate PTS from DTS. */
        mpeg124_video_vc1_genarate_pts(vdhp);
        vdhp->lw_seek_flags |= SEEK_PTS_GENERATED;
        no_pts_loss = 1;
    } else if ((lwhp->raw_demuxer || !no_pts_loss) && (vdhp->codec_id == AV_CODEC_ID_H264 || vdhp->codec_id == AV_CODEC_ID_HEVC)) {
        /* Generate PTS. */
        if (poc_genarate_pts(vdhp, vdhp->codec_id == AV_CODEC_ID_H264 ? 32 : 15) < 0) {
            lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory for PTS generation.");
            return -1;
        }
        vdhp->lw_seek_flags |= SEEK_PTS_GENERATED;
        no_pts_loss = 1;
    }
    /* Reorder in presentation order. */
    if (no_pts_loss && check_frame_reordering(info, sample_count)) {
        /* Consider presentation order for keyframe detection.
         * Note: sample number is 1-origin. */
        vdhp->order_converter = (order_converter_t*)lw_malloc_zero((sample_count + 1) * sizeof(order_converter_t));
        if (!vdhp->order_converter) {
            lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory.");
            return -1;
        }
        sort_info_presentation_order(&info[1], sample_count);
        video_timestamp_t* timestamp = (video_timestamp_t*)lw_malloc_zero((sample_count + 1) * sizeof(video_timestamp_t));
        if (!timestamp) {
            lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory of video timestamps.");
            return -1;
        }
        for (uint32_t i = 1; i <= sample_count; i++) {
            timestamp[i].pts = (int64_t)i;
            timestamp[i].dts = (int64_t)info[i].sample_number;
        }
        sort_decoding_order(&timestamp[1], sample_count, sizeof(video_timestamp_t));
        for (uint32_t i = 1; i <= sample_count; i++)
            vdhp->order_converter[i].decoding_to_presentation = (uint32_t)timestamp[i].pts;
        free(timestamp);
    } else if (vdhp->lw_seek_flags & SEEK_DTS_BASED)
        for (uint32_t i = 1; i <= sample_count; i++)
            info[i].pts = info[i].dts;
    /* Set the minimum timestamp. */
    vdhp->min_ts = (vdhp->lw_seek_flags & (SEEK_PTS_GENERATED | SEEK_PTS_BASED)) ? info[1].pts
        : (vdhp->lw_seek_flags & SEEK_DTS_BASED)                                 ? info[1].dts
                                                                                 : AV_NOPTS_VALUE;
    /* Treat video frames with unique value as keyframe. */
    if (vdhp->lw_seek_flags & SEEK_POS_BASED) {
        if (info[info[1].sample_number].file_offset == -1)
            info[info[1].sample_number].flags &= ~LW_VFRAME_FLAG_KEY;
        for (uint32_t i = 2; i <= sample_count; i++) {
            uint32_t j = info[i].sample_number;
            uint32_t k = info[i - 1].sample_number;
            if (info[j].file_offset == -1)
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
            else if (info[j].file_offset == info[k].file_offset) {
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
                info[k].flags &= ~LW_VFRAME_FLAG_KEY;
            }
        }
    } else if (vdhp->lw_seek_flags & SEEK_PTS_BASED) {
        if (info[info[1].sample_number].pts == AV_NOPTS_VALUE)
            info[info[1].sample_number].flags &= ~LW_VFRAME_FLAG_KEY;
        for (uint32_t i = 2; i <= sample_count; i++) {
            uint32_t j = info[i].sample_number;
            uint32_t k = info[i - 1].sample_number;
            if (info[j].pts == AV_NOPTS_VALUE)
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
            else if (info[j].pts == info[k].pts) {
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
                info[k].flags &= ~LW_VFRAME_FLAG_KEY;
            }
        }
    } else if (vdhp->lw_seek_flags & SEEK_DTS_BASED) {
        if (info[info[1].sample_number].dts == AV_NOPTS_VALUE)
            info[info[1].sample_number].flags &= ~LW_VFRAME_FLAG_KEY;
        for (uint32_t i = 2; i <= sample_count; i++) {
            uint32_t j = info[i].sample_number;
            uint32_t k = info[i - 1].sample_number;
            if (info[j].dts == AV_NOPTS_VALUE)
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
            else if (info[j].dts == info[k].dts) {
                info[j].flags &= ~LW_VFRAME_FLAG_KEY;
                info[k].flags &= ~LW_VFRAME_FLAG_KEY;
            }
        }
    }
    /* Set up keyframe list: presentation order (info) -> decoding order (keyframe_list) */
    for (uint32_t i = 1; i <= sample_count; i++)
        vdhp->keyframe_list[info[i].sample_number] = !!(info[i].flags & LW_VFRAME_FLAG_KEY);
    return 0;
}

static void decide_audio_seek_method(lwlibav_file_handler_t* lwhp, lwlibav_audio_decode_handler_t* adhp, uint32_t sample_count)
{
    adhp->lw_seek_flags = lineup_seek_base_candidates(lwhp);
    audio_frame_info_t* info = adhp->frame_list;
    for (uint32_t i = 1; i <= sample_count; i++)
        if (info[i].pts == AV_NOPTS_VALUE) {
            adhp->lw_seek_flags &= ~SEEK_PTS_BASED;
            break;
        }
    for (uint32_t i = 1; i <= sample_count; i++)
        if (info[i].dts == AV_NOPTS_VALUE) {
            adhp->lw_seek_flags &= ~SEEK_DTS_BASED;
            break;
        }
    if (adhp->lw_seek_flags & SEEK_POS_BASED) {
        if (lwhp->format_flags & AVFMT_NO_BYTE_SEEK)
            adhp->lw_seek_flags &= ~SEEK_POS_BASED;
        else {
            uint32_t error_count = 0;
            for (uint32_t i = 1; i <= sample_count; i++)
                error_count += (info[i].file_offset == -1);
            if (error_count == sample_count)
                adhp->lw_seek_flags &= ~SEEK_POS_BASED;
        }
    }
    if (!(adhp->lw_seek_flags & SEEK_PTS_BASED) && (adhp->lw_seek_flags & SEEK_DTS_BASED))
        for (uint32_t i = 1; i <= sample_count; i++)
            info[i].pts = info[i].dts;
    /* Treat audio frames with unique value as a keyframe. */
    if (adhp->lw_seek_flags & SEEK_POS_BASED) {
        info[1].keyframe = (info[1].file_offset != -1);
        for (uint32_t i = 2; i <= sample_count; i++)
            if (info[i].file_offset == -1)
                info[i].keyframe = 0;
            else if (info[i].file_offset == info[i - 1].file_offset)
                info[i].keyframe = info[i - 1].keyframe = 0;
            else
                info[i].keyframe = 1;
    } else if (adhp->lw_seek_flags & SEEK_PTS_BASED) {
        info[1].keyframe = (info[1].pts != AV_NOPTS_VALUE);
        for (uint32_t i = 2; i <= sample_count; i++)
            if (info[i].pts == AV_NOPTS_VALUE)
                info[i].keyframe = 0;
            else if (info[i].pts == info[i - 1].pts)
                info[i].keyframe = info[i - 1].keyframe = 0;
            else
                info[i].keyframe = 1;
    } else if (adhp->lw_seek_flags & SEEK_DTS_BASED) {
        info[1].keyframe = (info[1].dts != AV_NOPTS_VALUE);
        for (uint32_t i = 2; i <= sample_count; i++)
            if (info[i].dts == AV_NOPTS_VALUE)
                info[i].keyframe = 0;
            else if (info[i].dts == info[i - 1].dts)
                info[i].keyframe = info[i - 1].keyframe = 0;
            else
                info[i].keyframe = 1;
    } else
        for (uint32_t i = 1; i <= sample_count; i++)
            info[i].keyframe = 1;
}

static int64_t calculate_av_gap(
    lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp, lwlibav_audio_decode_handler_t* adhp, int sample_rate)
{
    /* Pick the first video timestamp.
     * If invalid, skip A/V gap calculation. */
    int64_t video_ts = (vdhp->lw_seek_flags & SEEK_PTS_BASED) ? vdhp->frame_list[1].pts : vdhp->frame_list[1].dts;
    if (video_ts == AV_NOPTS_VALUE)
        return 0;
    /* Pick the first valid audio timestamp.
     * If not found, skip A/V gap calculation. */
    int64_t audio_ts = 0;
    uint32_t audio_ts_number = 0;
    if (adhp->lw_seek_flags & SEEK_PTS_BASED) {
        for (uint32_t i = 1; i <= adhp->frame_count; i++)
            if (adhp->frame_list[i].pts != AV_NOPTS_VALUE) {
                audio_ts = adhp->frame_list[i].pts;
                audio_ts_number = i;
                break;
            }
    } else
        for (uint32_t i = 1; i <= adhp->frame_count; i++)
            if (adhp->frame_list[i].dts != AV_NOPTS_VALUE) {
                audio_ts = adhp->frame_list[i].dts;
                audio_ts_number = i;
                break;
            }
    if (audio_ts_number == 0)
        return 0;
    /* Estimate the first audio timestamp if invalid. */
    AVRational audio_sample_base = { 1, sample_rate };
    for (uint32_t i = 1, delay_count = 0; i < MIN(audio_ts_number + delay_count, adhp->frame_count); i++)
        if (adhp->frame_list[i].length != -1)
            audio_ts -= av_rescale_q(adhp->frame_list[i].length, audio_sample_base, adhp->time_base);
        else
            ++delay_count;
    /* Calculate A/V gap in audio samplerate. */
    if (video_ts || audio_ts) {
        int64_t av_gap
            = av_rescale_q(audio_ts, adhp->time_base, audio_sample_base) - av_rescale_q(video_ts, vdhp->time_base, audio_sample_base);
        if (vohp->repeat_control && vohp->repeat_correction_ts)
            av_gap += av_rescale_q(vohp->repeat_correction_ts, vdhp->time_base, audio_sample_base);
        return av_gap;
    }
    return 0;
}

static void compute_stream_duration(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, int64_t stream_duration)
{
    video_frame_info_t* info = vdhp->frame_list;
    int64_t first_ts;
    int64_t largest_ts;
    int64_t second_largest_ts;
    uint64_t first_duration;
    uint64_t stream_timebase;
    if (!(lwhp->format_flags & AVFMT_TS_DISCONT) && (vdhp->lw_seek_flags & (SEEK_PTS_BASED | SEEK_PTS_GENERATED))) {
        if (vdhp->lw_seek_flags & SEEK_PTS_GENERATED)
            goto fail;
        first_ts = info[1].pts;
        largest_ts = first_ts;
        second_largest_ts = first_ts;
        first_duration = info[2].pts - info[1].pts;
        stream_timebase = first_duration;
        vdhp->strict_cfr = (first_duration != 0);
        for (uint32_t i = 2; i <= vdhp->frame_count; i++) {
            uint64_t duration = info[i].pts - info[i - 1].pts;
            if (duration == 0) {
                lw_log_show(&vdhp->lh, LW_LOG_WARNING, "Detected PTS %" PRId64 " duplication at frame %" PRIu32, info[i].pts, i);
                goto fail;
            }
            if (vdhp->strict_cfr && duration != first_duration)
                vdhp->strict_cfr = 0;
            stream_timebase = get_gcd(stream_timebase, duration);
            second_largest_ts = largest_ts;
            largest_ts = info[i].pts;
        }
    } else if (vdhp->lw_seek_flags & (SEEK_DTS_BASED | SEEK_PTS_BASED | SEEK_PTS_GENERATED)) {
        uint32_t prev = 0;
        uint32_t curr = 0;
        uint32_t i = 0;
        for (++i; i <= vdhp->frame_count; i++) {
            prev = vdhp->order_converter ? vdhp->order_converter[i].decoding_to_presentation : i;
            if (!(info[prev].flags & LW_VFRAME_FLAG_INVISIBLE))
                break;
        }
        for (++i; i <= vdhp->frame_count; i++) {
            curr = vdhp->order_converter ? vdhp->order_converter[i].decoding_to_presentation : i;
            if (!(info[curr].flags & LW_VFRAME_FLAG_INVISIBLE))
                break;
        }
        if (i > vdhp->frame_count)
            goto fail;
        first_ts = info[prev].dts;
        largest_ts = first_ts;
        second_largest_ts = first_ts;
        first_duration = info[curr].dts - info[prev].dts;
        stream_timebase = first_duration;
        vdhp->strict_cfr = (first_duration != 0);
        curr = prev;
        while (1) {
            prev = curr;
            for (; i <= vdhp->frame_count; i++) {
                curr = vdhp->order_converter ? vdhp->order_converter[i].decoding_to_presentation : i;
                if (!(info[curr].flags & LW_VFRAME_FLAG_INVISIBLE))
                    break;
            }
            if (i > vdhp->frame_count)
                break;
            uint64_t duration = info[curr].dts - info[prev].dts;
            if (duration == 0) {
                lw_log_show(&vdhp->lh, LW_LOG_WARNING, "Detected DTS %" PRId64 " duplication at frame %" PRIu32, info[curr].dts, curr);
                goto fail;
            }
            if (vdhp->strict_cfr && duration != first_duration)
                vdhp->strict_cfr = 0;
            stream_timebase = get_gcd(stream_timebase, duration);
            second_largest_ts = largest_ts;
            largest_ts = info[curr].dts;
            ++i;
        }
    } else
        goto fail;
    vdhp->actual_time_base.num = (int)(vdhp->time_base.num * stream_timebase);
    vdhp->actual_time_base.den = vdhp->time_base.den;
    vdhp->stream_duration = (largest_ts - first_ts) + (largest_ts - second_largest_ts);
    return;
fail:
    vdhp->stream_duration = stream_duration;
    return;
}

static int allocate_frame_cache_buffers(lwlibav_video_output_handler_t* vohp)
{
    for (int i = 0; i < REPEAT_CONTROL_CACHE_NUM; i++) {
        vohp->frame_cache_buffers[i] = av_frame_alloc();
        if (!vohp->frame_cache_buffers[i])
            return -1;
        vohp->frame_cache_numbers[i] = 0;
    }
    return 0;
}

static void vfr2cfr_settings(lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp, lwlibav_option_t* opt)
{
    if (vdhp->stream_duration > 0 && (vdhp->lw_seek_flags & (SEEK_DTS_BASED | SEEK_PTS_BASED | SEEK_PTS_GENERATED))) {
        vohp->vfr2cfr = opt->vfr2cfr.active;
        vohp->cfr_num = opt->vfr2cfr.fps_num;
        vohp->cfr_den = opt->vfr2cfr.fps_den;
        vohp->frame_count = (uint32_t)(((double)vohp->cfr_num / vohp->cfr_den)
                * ((double)vdhp->stream_duration * vdhp->time_base.num / vdhp->time_base.den)
            + 0.5);
    } else
        vohp->vfr2cfr = 0;
}

static void create_video_frame_order_list(
    lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp, lwlibav_option_t* opt, int consistent_field_and_repeat)
{
    /* Eliminate guesswork: first determine if repeat is requested in the source. */
    for (uint32_t i = 1; i <= vdhp->frame_count; i++) {
        if (vdhp->frame_list[i].repeat_pict > 1)
            vohp->repeat_requested++;
    }

    if (!(vdhp->lw_seek_flags & (SEEK_PTS_BASED | SEEK_PTS_GENERATED)))
        goto disable_repeat;
    if (opt->vfr2cfr.active)
        opt->apply_repeat_flag = 0;
    video_frame_info_t* info = vdhp->frame_list;
    uint32_t frame_count = vdhp->frame_count;
    uint32_t order_count = 0;
    int no_support_frame_tripling = (vdhp->codec_id != AV_CODEC_ID_MPEG2VIDEO);
    int specified_field_dominance = opt->field_dominance == 0 ? LW_FIELD_INFO_UNKNOWN /* Obey source flags. */
        : opt->field_dominance == 1                           ? LW_FIELD_INFO_TOP /* TFF: Top -> Bottom */
                                                              : LW_FIELD_INFO_BOTTOM; /* BFF: Bottom -> Top */
    /* Check repeat_pict and order_count. */
    if (specified_field_dominance > 0 && (lw_field_info_t)specified_field_dominance != info[1].field_info)
        ++order_count;
    if (consistent_field_and_repeat && !info[1].repeat_pict) {
        for (uint32_t i = 1; i < vdhp->frame_count; ++i)
            info[i].repeat_pict = 1;
    }
    int enable_repeat = 0;
    int complete_frame = 1;
    int repeat_field = 1;
    lw_field_info_t next_field_info = info[1].field_info;
    for (uint32_t i = 1; i <= frame_count; i++, order_count++) {
        int repeat_pict = info[i].repeat_pict;
        lw_field_info_t field_info = info[i].field_info;
        int field_shift = !(repeat_pict & 1);
        if (field_info == LW_FIELD_INFO_UNKNOWN) {
            /* Override with TFF or BFF. */
            field_info = next_field_info;
            info[i].field_info = field_info;
        } else if (field_info != next_field_info) {
            if (!complete_frame) {
                /* The previous picture in output order fails to make a pair to construct a frame.
                 *    coded order: {I[0],P[1]},{P[4],P[5]},{B[2],}
                 *   output order: {I[0],P[1]},{B[2],},{P[4],P[5]}
                 * We exclude this picture from the output buffer. */
                info[i - 1].flags |= LW_VFRAME_FLAG_COUNTERPART_MISSING;
                complete_frame ^= 1;
                order_count -= 1;
            } else if (!repeat_field) {
                opt->apply_repeat_flag = i;
                goto disable_repeat;
            }
        }
        if (opt->apply_repeat_flag)
            switch (repeat_pict) {
            case 5: /* frame tripling */
                if (no_support_frame_tripling)
                    ++order_count;
            case 3: /* frame doubling */
                ++order_count;
                enable_repeat |= 1;
                break;
            case 2: /* field tripling */
                repeat_field ^= 1;
                order_count += repeat_field;
                enable_repeat |= 1;
                break;
            default:
                break;
            }
        if (repeat_pict == 0 && !(info[i].flags & (LW_VFRAME_FLAG_CORRUPT | LW_VFRAME_FLAG_COUNTERPART_MISSING))) {
            /* PAFF field coded picture */
            complete_frame ^= 1;
            order_count -= complete_frame;
            enable_repeat |= 1;
        }
        if (field_shift)
            next_field_info = field_info == LW_FIELD_INFO_TOP ? LW_FIELD_INFO_BOTTOM : LW_FIELD_INFO_TOP;
    }
    if (!enable_repeat || allocate_frame_cache_buffers(vohp) < 0)
        goto disable_repeat;
    /* Create order list. */
    lw_video_frame_order_t* order_list = (lw_video_frame_order_t*)lw_malloc_zero((order_count + 2) * sizeof(lw_video_frame_order_t));
    if (!order_list) {
        lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory to the frame order list for video.");
        goto disable_repeat;
    }
    int64_t correction_ts = 0;
    uint32_t t_count = 1;
    uint32_t b_count = 1;
    if (specified_field_dominance > 0) {
        if ((lw_field_info_t)specified_field_dominance == LW_FIELD_INFO_TOP && info[1].field_info == LW_FIELD_INFO_BOTTOM)
            order_list[t_count++].top = 1;
        else if ((lw_field_info_t)specified_field_dominance == LW_FIELD_INFO_BOTTOM && info[1].field_info == LW_FIELD_INFO_TOP)
            order_list[b_count++].bottom = 1;
        if (t_count > 1 || b_count > 1)
            correction_ts = (info[2].pts - info[1].pts) / (info[1].repeat_pict + 1);
    }
    complete_frame = 1;
    for (uint32_t i = 1; i <= frame_count; i++) {
        /* Check repeat_pict and field dominance. */
        int repeat_pict = info[i].repeat_pict;
        lw_field_info_t field_info = info[i].field_info;
        order_list[t_count++].top = i;
        order_list[b_count++].bottom = i;
        if (opt->apply_repeat_flag)
            switch (repeat_pict) {
            case 5: /* frame tripling */
                if (no_support_frame_tripling) {
                    order_list[t_count++].top = i;
                    order_list[b_count++].bottom = i;
                }
            case 3: /* frame doubling */
                order_list[t_count++].top = i;
                order_list[b_count++].bottom = i;
                break;
            case 2: /* field tripling */
                if (field_info == LW_FIELD_INFO_TOP)
                    order_list[t_count++].top = i;
                else if (field_info == LW_FIELD_INFO_BOTTOM)
                    order_list[b_count++].bottom = i;
                break;
            default:
                break;
            }
        if (repeat_pict == 0) {
            /* PAFF field coded picture */
            if (info[i].flags & LW_VFRAME_FLAG_COUNTERPART_MISSING) {
                /* Exclude this picture from the output buffer. */
                --t_count;
                --b_count;
                complete_frame = 1;
            } else if (!(info[i].flags & LW_VFRAME_FLAG_CORRUPT)) {
                if (field_info == LW_FIELD_INFO_BOTTOM)
                    --t_count;
                else
                    --b_count;
                complete_frame ^= 1;
            }
        }
    }
    --t_count;
    --b_count;
    if (t_count != b_count) {
        order_list[order_count].top = frame_count;
        order_list[order_count].bottom = frame_count;
    }
    memset(&order_list[order_count + 1], 0, sizeof(lw_video_frame_order_t));
    /* Set up repeat control info. */
    vohp->repeat_control = 1;
    vohp->repeat_correction_ts = correction_ts;
    vohp->frame_order_count = order_count;
    vohp->frame_order_list = order_list;
    vohp->frame_count = vohp->frame_order_count;
    lw_log_show(&vdhp->lh, LW_LOG_INFO, "Enable repeat control. frame_count = %u, order_count = %u, t_count = %u, b_count = %u",
        frame_count, order_count, t_count, b_count);
    return;
disable_repeat:
    vohp->repeat_control = 0;
    vohp->repeat_correction_ts = 0;
    vohp->frame_order_count = 0;
    vohp->frame_order_list = NULL;
    vohp->frame_count = vdhp->frame_count;
    if (opt->vfr2cfr.active)
        vfr2cfr_settings(vdhp, vohp, opt);
    if (opt->apply_repeat_flag)
        lw_log_show(&vdhp->lh, LW_LOG_INFO, "Disable repeat control.");
    return;
}

static inline int check_vp8_invisible_frame(const AVPacket* pkt)
{
    return !(pkt->data[0] & 0x10);
}

static inline int is_vp9_superframe(const AVPacket* pkt)
{
    if (!pkt->data || pkt->size < 3)
        return 0;
    if ((pkt->data[pkt->size - 1] & 0xe0) == 0xc0)
        return 1;
    return 0;
}

static void create_video_visible_frame_list(
    lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp, uint32_t invisible_count)
{
    if (vohp->repeat_control || invisible_count == 0)
        return;
    lw_video_frame_order_t* order_list = NULL;
    video_frame_info_t* info = vdhp->frame_list;
    if (vohp->vfr2cfr) {
        /* Duplicated frame numbers could be occured, so frame cache buffers are needed. */
        if (allocate_frame_cache_buffers(vohp) < 0)
            goto disable_repeat;
        order_list = (lw_video_frame_order_t*)lw_malloc_zero((vdhp->frame_count + 1) * sizeof(lw_video_frame_order_t));
        if (!order_list) {
            lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory to the frame order list for video.");
            goto disable_repeat;
        }
        uint32_t visible_number = 0;
        for (uint32_t i = 1; i <= vdhp->frame_count; i++) {
            if (!(info[i].flags & LW_VFRAME_FLAG_INVISIBLE))
                ++visible_number;
            order_list[i].top = visible_number;
            order_list[i].bottom = visible_number;
        }
        vohp->frame_order_count = vdhp->frame_count;
    } else {
        uint32_t visible_count = vdhp->frame_count - invisible_count;
        order_list = (lw_video_frame_order_t*)lw_malloc_zero((visible_count + 1) * sizeof(lw_video_frame_order_t));
        if (!order_list) {
            lw_log_show(&vdhp->lh, LW_LOG_FATAL, "Failed to allocate memory to the frame order list for video.");
            goto disable_repeat;
        }
        uint32_t order_count = 0;
        for (uint32_t i = 1; i <= vdhp->frame_count; i++)
            if (!(info[i].flags & LW_VFRAME_FLAG_INVISIBLE)) {
                ++order_count;
                order_list[order_count].top = i;
                order_list[order_count].bottom = i;
            }
        if (visible_count != order_count)
            goto disable_repeat;
        vohp->frame_order_count = visible_count;
        vohp->frame_count = vohp->frame_order_count;
    }
    vohp->repeat_control = 1;
    vohp->repeat_correction_ts = 0;
    vohp->frame_order_list = order_list;
    return;
disable_repeat:
    free(order_list);
    vohp->repeat_control = 0;
    vohp->repeat_correction_ts = 0;
    vohp->frame_order_count = 0;
    vohp->frame_order_list = NULL;
    if (!vohp->vfr2cfr)
        vohp->frame_count = vdhp->frame_count;
    lw_log_show(&vdhp->lh, LW_LOG_INFO, "Failed to create invisible frame control.");
    return;
}

static lwlibav_extradata_t* alloc_extradata_entries(lwlibav_extradata_handler_t* exhp, int count)
{
    assert(count > 0 && count > exhp->entry_count);
    lwlibav_extradata_t* temp = (lwlibav_extradata_t*)realloc(exhp->entries, count * sizeof(lwlibav_extradata_t));
    if (!temp)
        return NULL;
    exhp->entries = temp;
    temp = &exhp->entries[exhp->entry_count];
    for (int i = exhp->entry_count; i < count; i++) {
        lwlibav_extradata_t* entry = &exhp->entries[i];
        entry->extradata = NULL;
        entry->extradata_size = 0;
        entry->codec_id = AV_CODEC_ID_NONE;
        entry->codec_tag = 0;
        entry->width = 0;
        entry->height = 0;
        entry->pixel_format = AV_PIX_FMT_NONE;
        entry->channel_layout = 0;
        entry->sample_format = AV_SAMPLE_FMT_NONE;
        entry->sample_rate = 0;
        entry->bits_per_sample = 0;
        entry->block_align = 0;
    }
    exhp->entry_count = count;
    return temp;
}

/* The libavcodec VC-1 parser does not support VC-1 and WMV3 packets without start code. This function makes them
 * parsable by adding start code, and convert RBDU into EBDU if needed. */
static int make_vc1_ebdu(lwindex_helper_t* helper, AVPacket* in_pkt, AVPacket* out_pkt, uint8_t bdu_type, int is_vc1)
{
    int enough_packet_size = (1 + !is_vc1) * (in_pkt->size + 4);
    if (enough_packet_size > helper->pkt.size) {
        int ret = av_grow_packet(&helper->pkt, enough_packet_size - helper->pkt.size);
        if (ret < 0)
            return ret;
    }
    av_packet_free_side_data(&helper->pkt);
    int ret = av_packet_copy_props(&helper->pkt, in_pkt);
    if (ret < 0)
        return ret;
    int* size = &helper->pkt.size;
    uint8_t* data = helper->pkt.data;
    /* start code */
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x01;
    data[3] = bdu_type;
    if (is_vc1) {
        *size = in_pkt->size + 4;
        memcpy(data + 4, in_pkt->data, in_pkt->size);
    } else {
        /* RBDU to EBDU */
        uint8_t* pos = in_pkt->data;
        uint8_t* end = in_pkt->data + in_pkt->size;
        *size = 4;
        if (pos < end)
            data[(*size)++] = *(pos++);
        if (pos < end)
            data[(*size)++] = *(pos++); /* No need to check emulation since bdu_type == 0 is reserved. */
        while (pos < end) {
            if (pos[-2] == 0x00 && pos[-1] == 0x00 && pos[0] <= 0x03)
                data[(*size)++] = 0x03;
            data[(*size)++] = (pos++)[0];
        }
    }
    memset(data + *size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    return av_packet_ref(out_pkt, &helper->pkt);
}

static lwindex_helper_t* get_index_helper(lwindex_indexer_t* indexer, AVStream* stream, const int rap_verification)
{
    if (indexer->number_of_helpers <= stream->index) {
        const size_t old_alloc_size = indexer->number_of_helpers * sizeof(lwindex_indexer_t*);
        const size_t new_alloc_size = (stream->index + 1) * sizeof(lwindex_indexer_t*);
        lwindex_helper_t** temp = (lwindex_helper_t**)av_realloc(indexer->helpers, new_alloc_size);
        if (!temp)
            return NULL;
        memset((char*)temp + old_alloc_size, 0, new_alloc_size - old_alloc_size);
        indexer->helpers = temp;
        indexer->number_of_helpers = stream->index + 1;
    }
    lwindex_helper_t* helper = indexer->helpers[stream->index];
    if (!helper) {
        /* Allocate the index helper. */
        helper = (lwindex_helper_t*)lw_malloc_zero(sizeof(lwindex_helper_t));
        if (!helper)
            return NULL;
        indexer->helpers[stream->index] = helper;
        /* Set up the decoder. */
        AVCodecParameters* codecpar = stream->codecpar;
        const int is_codec_type_video = codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
        const char** preferred_decoder_names = is_codec_type_video ? rap_verification ? NULL : indexer->preferred_video_decoder_names
                                                                   : indexer->preferred_audio_decoder_names;
        int indexing_prefer_hw = indexer->prefer_video_hw_decoder ? rap_verification ? 0 : *indexer->prefer_video_hw_decoder : -1;
        int* indexing_prefer_hw_ptr = (indexing_prefer_hw == -1) ? NULL : &indexing_prefer_hw;
        if (find_and_open_decoder(&helper->codec_ctx, codecpar, preferred_decoder_names, indexing_prefer_hw_ptr, indexer->thread_count,
                -1.0, 0, indexer->hw_device_ctx)
            < 0)
            /* Failed to find and open an appropriate decoder, but do not abort indexing. */
            return helper;
        helper->mpeg12_video = (codecpar->codec_id == AV_CODEC_ID_MPEG1VIDEO || codecpar->codec_id == AV_CODEC_ID_MPEG2VIDEO);
        helper->vc1_wmv3 = (codecpar->codec_id == AV_CODEC_ID_VC1 || codecpar->codec_id == AV_CODEC_ID_VC1IMAGE
            || codecpar->codec_id == AV_CODEC_ID_WMV3 || codecpar->codec_id == AV_CODEC_ID_WMV3IMAGE);
        if (helper->vc1_wmv3 && !strcmp(indexer->format_name, "asf"))
            helper->vc1_wmv3 = 2;
        /* Set up the parser externally.
         * We don't trust parameters returned by the internal parser. */
        helper->parser_ctx = av_parser_init(helper->vc1_wmv3 ? AV_CODEC_ID_VC1 : codecpar->codec_id);
        if (helper->parser_ctx) {
            helper->parser_ctx->flags |= PARSER_FLAG_COMPLETE_FRAMES;
            /* Set up bitstream filter if needed. */
            if (codecpar->codec_id == AV_CODEC_ID_H264
                && codecpar->extradata_size >= 8 /* 8 is the offset of the first byte of the first SPS in AVCConfigurationRecord. */
                && codecpar->extradata[0] == 1 /* configurationVersion == 1 */
                && helper->parser_ctx->parser && helper->parser_ctx->parser->split
                && helper->parser_ctx->parser->split(helper->codec_ctx, codecpar->extradata + 8, codecpar->extradata_size - 8) <= 0) {
                /* Since a SPS shall have no start code and no its emulation,
                 * therefore, this stream is not encapsulated as byte stream format. */
                helper->bsf = av_bsf_get_by_name("h264_mp4toannexb");
                if (!helper->bsf)
                    return NULL;
            } else if (codecpar->codec_id == AV_CODEC_ID_AAC && stream->nb_frames == 0)
                /* To generate and export AudioSpecificConfig as an extradata from ADTS header. */
                helper->bsf = av_bsf_get_by_name("aac_adtstoasc");
            else if (codecpar->codec_id == AV_CODEC_ID_MPEG4)
                /* This is needed to make mpeg124_video_vc1_genarate_pts() work properly for packed bitstream. */
                helper->bsf = av_bsf_get_by_name("mpeg4_unpack_bframes");
        }
        if (!is_codec_type_video || rap_verification) {
            helper->decode = !is_codec_type_video ? decode_audio_packet : decode_video_packet;
            helper->picture = av_frame_alloc();
            if (!helper->picture)
                return NULL;
        }
        if (helper->parser_ctx && helper->vc1_wmv3 == 2) {
            /* Initialize the VC-1/WMV3 parser by extradata. */
            int ret;
            AVPacket parsable_pkt = { 0 };
            if (codecpar->codec_id == AV_CODEC_ID_WMV3 || codecpar->codec_id == AV_CODEC_ID_WMV3IMAGE) {
                /* Make a sequence header EBDU (0x0000010F). */
                AVPacket pkt = { 0 };
                pkt.data = codecpar->extradata;
                pkt.size = codecpar->extradata_size;
                ret = make_vc1_ebdu(helper, &pkt, &parsable_pkt, 0x0F, 0);
            } else {
                /* For WVC1, the first byte is its size. */
                AVPacket pkt = { 0 };
                pkt.data = codecpar->extradata + 1;
                pkt.size = codecpar->extradata_size - 1;
                ret = av_packet_ref(&parsable_pkt, &pkt);
            }
            if (ret < 0)
                return NULL;
            uint8_t* dummy;
            int dummy_size;
            av_parser_parse2(helper->parser_ctx, helper->codec_ctx, &dummy, &dummy_size, parsable_pkt.data, parsable_pkt.size,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, -1);
            av_packet_unref(&parsable_pkt);
        }
    } else
        helper->already_decoded = 0;
    return helper;
}

/* Apply bistream filter input packet. Allocate or reallocate AVBSFContext if needed.
 *
 * Return 0 and set out_pkt to the filtered packet on success.
 * Otherwise return a negative value. */
static int apply_bsf(lwindex_helper_t* helper, AVCodecContext* ctx, AVPacket* out_pkt, AVPacket* in_pkt, const char* bsf_name)
{
    int ret = -1;
    assert(helper->bsf);
    if (!helper->bsf_ctx || helper->bsf_ctx->par_in->codec_id != ctx->codec_id) {
        /* Allocate AVBSFContext or reallocate it for CODEC change. */
        av_bsf_free(&helper->bsf_ctx);
        if ((helper->bsf = av_bsf_get_by_name(bsf_name ? bsf_name : helper->bsf->name)) == NULL
            || (ret = av_bsf_alloc(helper->bsf, &helper->bsf_ctx)) < 0
            || (ret = avcodec_parameters_from_context(helper->bsf_ctx->par_in, ctx)) < 0)
            return ret;
        helper->bsf_ctx->time_base_in = ctx->time_base;
        if ((ret = av_bsf_init(helper->bsf_ctx)) < 0)
            return ret;
    }
    /* Clone input packet since av_bsf_send_packet() moves sent packet to the internal packet buffer.
     * The allocated packet will be reused for draining the remaining packets. */
    AVPacket* pkt = av_packet_clone(in_pkt);
    if (!pkt)
        return -1;
    in_pkt = pkt; /* Don't send the original input packet to the bitstream filter. */
    /* Apply the filter actually here.
     * Note that ffmpeg's av_bsf_send_packet() does not set EOF by sending NULL payload packet while libav's does.
     * Therefore, not using the same AVPacket pointer for both sending and receiving here. */
    while (1) {
        if ((ret = av_bsf_send_packet(helper->bsf_ctx, in_pkt)) < 0)
            goto fail;
        ret = av_bsf_receive_packet(helper->bsf_ctx, out_pkt);
        if (ret == AVERROR(EAGAIN) || (in_pkt && ret == AVERROR_EOF))
            in_pkt = NULL; /* Send a null packet at the next. */
        else if (ret < 0)
            goto fail;
        else if (ret == 0)
            break;
    }
    /* Update extradata of AVCodecContext if changed. */
    if (ctx->extradata_size != helper->bsf_ctx->par_out->extradata_size
        || memcmp(ctx->extradata, helper->bsf_ctx->par_out->extradata, helper->bsf_ctx->par_out->extradata_size)) {
        av_free(ctx->extradata);
        ctx->extradata_size = 0;
        ctx->extradata = (uint8_t*)av_mallocz(helper->bsf_ctx->par_out->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!ctx->extradata)
            return -1;
        memcpy(ctx->extradata, helper->bsf_ctx->par_out->extradata, helper->bsf_ctx->par_out->extradata_size);
        ctx->extradata_size = helper->bsf_ctx->par_out->extradata_size;
    }
    /* Drain all the remaining packets. */
    while (ret >= 0) {
        ret = av_bsf_receive_packet(helper->bsf_ctx, pkt);
        av_packet_unref(pkt);
    }
    ret = 0;
fail:
    av_packet_free(&pkt);
    return ret;
}

#define IF_START_CODE(ptr) if ((ptr)[0] == 0x00 && (ptr)[1] == 0x00 && (ptr)[2] == 0x01)
static int get_offset_to_h264_parameter_sets(AVPacket* pkt)
{
    int offset = 0;
    while (offset < pkt->size - 4) {
        uint8_t* data = pkt->data + (uintptr_t)offset;
        IF_START_CODE(data)
        {
            /* nal_unit_type == 7 : Sequence Parameter Set
             * nal_unit_type == 8 : Picture Parameter Set
             * Parameter sets shall follow long 4 byte start codes but there may be illegal ones, so do workaround here. */
            uint8_t nal_unit_type = data[3] & 0x1f;
            if (nal_unit_type == 7 || nal_unit_type == 8)
                return offset - (offset > 0 && data[-1] == 0 ? 1 : 0);
            offset += 4; /* Skip start code and NAL Unit header. */
        }
        else offset++;
    }
    return 0;
}

static int get_offset_to_hevc_parameter_sets(AVPacket* pkt)
{
    int offset = 0;
    while (offset < pkt->size - 5) {
        uint8_t* data = pkt->data + (uintptr_t)offset;
        IF_START_CODE(data)
        {
            /* nal_unit_type == 32 : Video Parameter Set
             * nal_unit_type == 33 : Sequence Parameter Set
             * nal_unit_type == 34 : Picture Parameter Set
             * Parameter sets shall follow long 4 byte start codes but there may be illegal ones, so do workaround here. */
            uint8_t nal_unit_type = (data[3] >> 1) & 0x3f;
            if (nal_unit_type == 32 || nal_unit_type == 33 || nal_unit_type == 34)
                return offset - (offset > 0 && data[-1] == 0 ? 1 : 0);
            offset += 5; /* Skip start code and NAL Unit header. */
        }
        else offset++;
    }
    return 0;
}
#undef IF_START_CODE

static int get_offset_to_significant_extradata(AVCodecContext* ctx, AVPacket* pkt)
{
    return ctx->codec_id == AV_CODEC_ID_H264 ? get_offset_to_h264_parameter_sets(pkt)
        : ctx->codec_id == AV_CODEC_ID_HEVC  ? get_offset_to_hevc_parameter_sets(pkt)
                                             : 0;
}

static int append_extradata_if_new(lwindex_helper_t* helper, AVCodecContext* ctx, AVPacket* pkt)
{
    lwlibav_extradata_handler_t* list = &helper->exh;
    if (!(pkt->flags & AV_PKT_FLAG_KEY) && list->entry_count > 0)
        /* Some decoders might not change AVCodecContext.extradata even if a new extradata occurs.
         * Here, we assume non-keyframes reference the latest extradata. */
        return list->current_index;
    /* Anyway, import extradata from AVCodecContext. */
    lwlibav_extradata_t current = { ctx->extradata, ctx->extradata_size };
    /* Import extradata from a side data in the packet if present. */
    for (int i = 0; i < pkt->side_data_elems; i++)
        if (pkt->side_data[i].type == AV_PKT_DATA_NEW_EXTRADATA) {
            current.extradata = pkt->side_data[i].data;
            current.extradata_size = pkt->side_data[i].size;
            break;
        }
    /* Try to import extradata from the packet by splitting if no extradata is present in side data. */
    if (current.extradata == ctx->extradata) {
        AVCodecParserContext* parser_ctx = helper->parser_ctx;
        if (parser_ctx && parser_ctx->parser) {
            if (parser_ctx->parser->split) {
                /* For H.264 stream without start codes, don't split extradata from pkt->data.
                 * Its extradata is stored as global header. so, pkt->data shall contain no extradata.
                 * Libavcodec may not remove meaningless data which precedes data actually needed for decoding,
                 * so get the offset to such significant data here to deduplicate extradata as much as possible. */
                int extradata_size = helper->bsf ? 0 : parser_ctx->parser->split(ctx, pkt->data, pkt->size);
                if (extradata_size > 0) {
                    int offset = get_offset_to_significant_extradata(ctx, pkt);
                    current.extradata = pkt->data + (uintptr_t)offset;
                    current.extradata_size = extradata_size - offset;
                } else if (list->entry_count > 0)
                    /* Probably, this frame is a keyframe in CODEC level
                     * but should not be a random accessible point in container level.
                     * For instance, an IDR-picture which corresponding SPSs and PPSs
                     * do not precede immediately might not be decodable correctly
                     * when decoding from there in MPEG-2 transport stream. */
                    return list->current_index;
            } else if (helper->bsf && ctx->codec_id == AV_CODEC_ID_AAC) {
                /* Try to generate AudioSpecificConfig for each ADTS AAC frame by reopening the bitstream filter.
                 * AVCodecContext.sample_rate is initialized by already upsampled AVCodecParameters.sample_rate and no SBR
                 * signalling, so if you initialize the AAC decoder without actual decoding, then the AAC decoder will
                 * initialize the internal sample_rate by already upsampled sample_rate and double its sample_rate when
                 * finding SBR. As the result, AVCodecContext.sample_rate is doubled.
                 * Note that in order to avoid AVCodecContext.sample_rate doubling when decoding ADTS HE-AAC, this bitstream
                 * filtering is for exporting the extradata only and the filtered packet is not sent to the decoder. */
                av_bsf_free(&helper->bsf_ctx);
                AVPacket filtered_pkt = { 0 };
                (void)apply_bsf(helper, ctx, &filtered_pkt, pkt, "aac_adtstoasc");
                /* Decode actually to get output channels and sampling rate of AAC frame.
                 * Note that this is a side effect of this function. */
                int decode_complete;
                int ret = helper->decode(ctx, helper->picture, &decode_complete, pkt);
                if (ret > 0 && !decode_complete) {
                    AVPacket null_pkt = { 0 };
                    ret = helper->decode(ctx, helper->picture, &decode_complete, &null_pkt);
                    /* Reset the draining state. */
                    avcodec_flush_buffers(ctx);
                }
                helper->already_decoded = 1;
                current.extradata = ctx->extradata;
                current.extradata_size = ctx->extradata_size;
                av_packet_unref(&filtered_pkt);
            }
        }
    }
    if (list->entry_count == 0) {
        lwlibav_extradata_t* entry = alloc_extradata_entries(list, 1);
        if (!entry)
            return -1;
        list->current_index = 0;
        if (current.extradata && current.extradata_size > 0) {
            entry->extradata_size = current.extradata_size;
            entry->extradata = (uint8_t*)av_malloc(current.extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!entry->extradata)
                return -1;
            memcpy(entry->extradata, current.extradata, entry->extradata_size);
            memset(entry->extradata + entry->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
        }
    } else {
        lwlibav_extradata_t* entry = &list->entries[list->current_index];
        if (current.extradata_size != entry->extradata_size || memcmp(current.extradata, entry->extradata, current.extradata_size)) {
            /* Check if this extradata is a new one. If so, append it to the list. */
            for (int i = 0; i < list->entry_count; i++) {
                if (i == list->current_index)
                    continue; /* already compared */
                entry = &list->entries[i];
                if (current.extradata_size == entry->extradata_size
                    && (current.extradata_size == 0 || !memcmp(current.extradata, entry->extradata, current.extradata_size))) {
                    /* The same extradata is found. */
                    list->current_index = i;
                    return list->current_index;
                }
            }
            /* Append a new extradata. */
            entry = alloc_extradata_entries(list, list->entry_count + 1);
            if (!entry)
                return -1;
            if (current.extradata && current.extradata_size > 0) {
                entry->extradata_size = current.extradata_size;
                entry->extradata = (uint8_t*)av_malloc(current.extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
                if (!entry->extradata)
                    return -1;
                memcpy(entry->extradata, current.extradata, entry->extradata_size);
                memset(entry->extradata + entry->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
            }
            list->current_index = list->entry_count - 1;
        }
    }
    return list->current_index;
}

static void investigate_pix_fmt_by_decoding(AVCodecContext* video_ctx, AVPacket* pkt, AVFrame* picture)
{
    int got_picture;
    decode_video_packet(video_ctx, picture, &got_picture, pkt);
}

static int make_packet_parsable(lwindex_helper_t* helper, AVCodecContext* ctx, AVPacket* out_pkt, AVPacket* in_pkt)
{
    if (helper->vc1_wmv3 == 2)
        /* Make a frame EBDU (0x0000010D). */
        return make_vc1_ebdu(helper, in_pkt, out_pkt, 0x0D, ctx->codec_id == AV_CODEC_ID_VC1 || ctx->codec_id == AV_CODEC_ID_VC1IMAGE);
    if (!helper->bsf)
        /* Just use input packet since no bitstream filters are defined for this packet. */
        return av_packet_ref(out_pkt, in_pkt);
    /* Convert frame data into parsable bitstream format. */
    return apply_bsf(helper, ctx, out_pkt, in_pkt, NULL);
}

static int get_picture_type(lwindex_helper_t* helper, AVCodecContext* ctx, AVPacket* pkt, const int rap_verification)
{
    if (!helper->parser_ctx)
        return 0;
    /* Get by the parser. */
    AVPacket filtered_pkt = { 0 };
    int ret = make_packet_parsable(helper, ctx, &filtered_pkt, pkt);
    if (ret < 0)
        return ret;
    uint8_t* dummy;
    int dummy_size;
    av_parser_parse2(helper->parser_ctx, ctx, &dummy, &dummy_size, filtered_pkt.data, filtered_pkt.size, pkt->pts, pkt->dts, pkt->pos);
    const int parser_pict_type = helper->parser_ctx->pict_type > 0 ? helper->parser_ctx->pict_type : 0;
    if (rap_verification) {
        if (parser_pict_type != AV_PICTURE_TYPE_I) {
            pkt->flags &= ~AV_PKT_FLAG_KEY;
            av_packet_unref(&filtered_pkt);
            return parser_pict_type;
        }
        // The parser thinks it's an I-frame. Let's perform a decode test to be sure it's a valid RAP.
        av_frame_unref(helper->picture);
        int decode_complete;
        helper->decode(ctx, helper->picture, &decode_complete, &filtered_pkt);
        if (!decode_complete) {
            AVPacket null_pkt = { 0 };
            helper->decode(ctx, helper->picture, &decode_complete, &null_pkt);
        }
        int pict_type_to_return = parser_pict_type;
        if (decode_complete) {
            if (helper->picture->pict_type == AV_PICTURE_TYPE_I && (helper->picture->flags & AV_FRAME_FLAG_KEY))
                pkt->flags |= AV_PKT_FLAG_KEY;
            else
                pkt->flags &= ~AV_PKT_FLAG_KEY;
            pict_type_to_return = helper->picture->pict_type > 0 ? helper->picture->pict_type : parser_pict_type;
        } else
            pkt->flags &= ~AV_PKT_FLAG_KEY;
        avcodec_flush_buffers(ctx);
        av_packet_unref(&filtered_pkt);
        return pict_type_to_return;
    } else {
        // If the demuxer flag contradicts the parser, always trust the parser.
        // The parser is more reliable as it inspects the bitstream.
        if ((pkt->flags & AV_PKT_FLAG_KEY) && (parser_pict_type != AV_PICTURE_TYPE_I))
            pkt->flags &= ~AV_PKT_FLAG_KEY;
        av_packet_unref(&filtered_pkt);
        return parser_pict_type;
    }
}

/* Return ticks_per_frame.
 *
 * This function is a workaround mainly for lagged ticks_per_frame determination of the libavcodec MPEG-1/2 decoder. Apparently,
 * the libavcodec VC-1 decoder handles both 1 and 2 ticks_per_frame patterns and it can be determined after encountering the
 * sequence header, but it is set up by extradata, which you get from AVCodecParameters, at the decoder initialization, so it
 * should be available safely. */
static int get_ticks_per_frame(AVCodecContext* ctx)
{
    if (ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO || ctx->codec_id == AV_CODEC_ID_H264)
        return 2;
    else if (ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO)
        return 1;
    return (ctx->codec_descriptor->props & AV_CODEC_PROP_FIELDS) ? 2 : 1;
}

static int get_audio_frame_length(lwindex_helper_t* helper, AVCodecContext* ctx, AVPacket* pkt)
{
    int frame_length;
    if (helper->parser_ctx) {
        /* Try to get from the parser. */
        uint8_t* dummy;
        int dummy_size;
        av_parser_parse2(helper->parser_ctx, ctx, &dummy, &dummy_size, pkt->data, pkt->size, pkt->pts, pkt->dts, pkt->pos);
        frame_length = helper->parser_ctx->duration;
    } else
        frame_length = 0;
    if (frame_length == 0 && helper->delay_count == 0)
        frame_length = ctx->frame_size;
    if (frame_length == 0) {
        if (helper->already_decoded)
            frame_length += helper->picture->nb_samples;
        else {
            /* Try to get by actual decoding. */
            AVPacket temp = *pkt;
            int ret = 0;
            int output_audio = 0;
            int draining = 0;
            do {
                if (temp.size == 0)
                    draining = 1;
                ret = helper->decode(ctx, helper->picture, &output_audio, &temp);
                if (ret < 0) {
                    av_channel_layout_copy(&ctx->ch_layout, &helper->picture->ch_layout);
                    ctx->sample_rate = helper->picture->sample_rate;
                }
                if (output_audio)
                    frame_length += helper->picture->nb_samples;
                if (ctx->codec_id == AV_CODEC_ID_WMAV2)
                    break;
                if (!draining) {
                    /* Send the null packet at the next decoding. */
                    temp.data = NULL;
                    temp.size = 0;
                }
            } while (ret > 0 || output_audio);
            if (draining)
                /* Reset the draining state. */
                avcodec_flush_buffers(ctx);
        }
        if (frame_length == 0) {
            frame_length = -1;
            ++helper->delay_count;
            if (helper->parser_ctx) {
                /* Don't use the parser anymore because of asynchronization. */
                av_parser_close(helper->parser_ctx);
                helper->parser_ctx = NULL;
            }
        }
    }
    return frame_length;
}

static enum AVSampleFormat select_better_sample_format(enum AVSampleFormat a, enum AVSampleFormat b)
{
    switch (a) {
    case AV_SAMPLE_FMT_NONE:
        if (b != AV_SAMPLE_FMT_NONE)
            a = b;
        break;
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        if (b != AV_SAMPLE_FMT_U8 && b != AV_SAMPLE_FMT_U8P)
            a = b;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        if (b != AV_SAMPLE_FMT_U8 && b != AV_SAMPLE_FMT_U8P && b != AV_SAMPLE_FMT_S16 && b != AV_SAMPLE_FMT_S16P)
            a = b;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        if (b != AV_SAMPLE_FMT_U8 && b != AV_SAMPLE_FMT_U8P && b != AV_SAMPLE_FMT_S16 && b != AV_SAMPLE_FMT_S16P && b != AV_SAMPLE_FMT_S32
            && b != AV_SAMPLE_FMT_S32P)
            a = b;
        break;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        if (b == AV_SAMPLE_FMT_DBL || b == AV_SAMPLE_FMT_DBLP)
            a = b;
        break;
    default:
        break;
    }
    return a;
}

static inline void write_av_index_entry(FILE* index, const AVIndexEntry* ie)
{
    print_index(index, "POS=%" PRId64 ",TS=%" PRId64 ",Flags=%x,Size=%d,Distance=%d\n", ie->pos, ie->timestamp, ie->flags, ie->size,
        ie->min_distance);
}

static void write_video_extradata(FILE* index, lwlibav_extradata_t* entry)
{
    if (!index)
        return;
    fprintf(index, "Size=%d,Codec=%d,4CC=0x%x,Width=%d,Height=%d,Format=%s,BPS=%d\n", entry->extradata_size, entry->codec_id,
        entry->codec_tag, entry->width, entry->height,
        av_get_pix_fmt_name(entry->pixel_format) ? av_get_pix_fmt_name(entry->pixel_format) : "none", entry->bits_per_sample);
    if (entry->extradata_size > 0)
        fwrite(entry->extradata, 1, entry->extradata_size, index);
    fprintf(index, "\n");
}

static void write_audio_extradata(FILE* index, lwlibav_extradata_t* entry)
{
    if (!index)
        return;
    fprintf(index, "Size=%d,Codec=%d,4CC=0x%x,Layout=0x%" PRIx64 ",Rate=%d,Format=%s,BPS=%d,Align=%d\n", entry->extradata_size,
        entry->codec_id, entry->codec_tag, entry->channel_layout, entry->sample_rate,
        av_get_sample_fmt_name(entry->sample_format) ? av_get_sample_fmt_name(entry->sample_format) : "none", entry->bits_per_sample,
        entry->block_align);
    if (entry->extradata_size > 0)
        fwrite(entry->extradata, 1, entry->extradata_size, index);
    fprintf(index, "\n");
}

static void disable_video_stream(lwlibav_video_decode_handler_t* vdhp)
{
    lw_freep(&vdhp->frame_list);
    lw_freep(&vdhp->keyframe_list);
    lw_freep(&vdhp->order_converter);
    av_freep(&vdhp->index_entries);
    vdhp->stream_index = -1;
    vdhp->index_entries_count = 0;
    vdhp->frame_count = 0;
}

static void cleanup_index_helpers(lwindex_indexer_t* indexer, AVFormatContext* format_ctx, const int rap_verification)
{
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        lwindex_helper_t* helper = get_index_helper(indexer, format_ctx->streams[stream_index], rap_verification);
        if (!helper)
            continue;
        avcodec_free_context(&helper->codec_ctx);
        av_parser_close(helper->parser_ctx);
        av_bsf_free(&helper->bsf_ctx);
        av_frame_free(&helper->picture);
        av_packet_unref(&helper->pkt);
        lwlibav_extradata_handler_t* list = &helper->exh;
        if (list->entries) {
            for (int i = 0; i < list->entry_count; i++)
                av_freep(&list->entries[i].extradata);
            free(list->entries);
        }
        /* Free an index helper. */
        lw_free(helper);
    }
    av_freep(&indexer->helpers);
}

static int create_index(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp,
    lwlibav_audio_decode_handler_t* adhp, lwlibav_audio_output_handler_t* aohp, AVFormatContext* format_ctx, lwlibav_option_t* opt,
    progress_indicator_t* indicator, progress_handler_t* php)
{
    uint32_t video_info_count = 1 << 16;
    uint32_t audio_info_count = 1 << 16;
    video_frame_info_t* video_info = (video_frame_info_t*)malloc(video_info_count * sizeof(video_frame_info_t));
    if (!video_info)
        return -1;
    audio_frame_info_t* audio_info = (audio_frame_info_t*)malloc(audio_info_count * sizeof(audio_frame_info_t));
    if (!audio_info) {
        free(video_info);
        return -1;
    }
    /*
        # Structure of Libav reader index file
        <LibavReaderIndexFile=18>
        <InputFilePath>foobar.omo</InputFilePath>
        <FileSize=1048576>
        <FileLastModificationTime=000>
        <FileHash=0x0123456789abcdef>
        <LibavReaderIndex=0x00000208,0,marumoska>
        <ActiveVideoStreamIndex>+0000000000</ActiveVideoStreamIndex>
        <ActiveAudioStreamIndex>-0000000001</ActiveAudioStreamIndex>
        <DefaultAudioStreamIndex>-0000000001</DefaultAudioStreamIndex>
        <FillAudioGaps>0</FillAudioGaps>
        <StreamInfo=0,0>
        Codec=2,TimeBase=1001/24000,Width=1920,Height=1080,Format=yuv420p,ColorSpace=5
        </StreamInfo>
        Index=0,POS=0,PTS=2002,DTS=0,EDI=0
        Key=1,Pic=1,POC=0,Repeat=1,Field=0,Super=0
        </LibavReaderIndex>
        <VideoConsistentFieldRepeatPict>1</VideoConsistentFieldRepeatPict>
        <StreamDuration=0,0>5000</StreamDuration>
        <StreamIndexEntries=0,0,1>
        POS=0,TS=2002,Flags=1,Size=1024,Distance=0
        </StreamIndexEntries>
        <ExtraDataList=0,0,1>
        Size=252,Codec=28,4CC=0x564d4448,Width=1920,Height=1080,Format=yuv420p,BPS=0
        ... binary string ...
        </ExtraDataList>
        </LibavReaderIndexFile>
     */
    FILE* index = NULL;
    if (opt->index_file_path)
        index = !opt->no_create_index ? lw_fopen(opt->index_file_path, "wb") : NULL;
    else if (!opt->no_create_index) {
        char* index_path = create_lwi_path(opt);
        index = lw_fopen(index_path, "wb");
        if (!index)
            fprintf(stderr, "lsmas: unable to create index file %s\n", index_path);
        lw_free(index_path);
    }
    if (!index && !opt->no_create_index) {
        free(video_info);
        free(audio_info);
        return -1;
    }
    lwhp->format_name = (char*)format_ctx->iformat->name;
    lwhp->format_flags = format_ctx->iformat->flags;
    char lname[4];
    snprintf(lname, sizeof lname, "%.3s", format_ctx->iformat->long_name);
    lwhp->raw_demuxer = !strcmp(lname, "raw");
    vdhp->format = format_ctx;
    adhp->format = format_ctx;
    adhp->dv_in_avi = !strcmp(lwhp->format_name, "avi") ? -1 : 0;
    int32_t video_index_pos = 0;
    int32_t audio_index_pos = 0;
#ifdef _WIN32
    wchar_t* wname = NULL;
#endif // _WIN32
    if (index) {
        /* Write Index file header. */
        uint8_t lwindex_version[4]
            = { (LWINDEX_VERSION >> 24) & 0xff, (LWINDEX_VERSION >> 16) & 0xff, (LWINDEX_VERSION >> 8) & 0xff, LWINDEX_VERSION & 0xff };
        fprintf(index, "<LSMASHWorksIndexVersion=%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "-39b2b47>\n", lwindex_version[0], lwindex_version[1],
            lwindex_version[2], lwindex_version[3]);
        fprintf(index, "<LibavReaderIndexFile=%d>\n", LWINDEX_INDEX_FILE_VERSION);
        fprintf(index, "<InputFilePath>%s</InputFilePath>\n", lwhp->file_path);
#ifdef _WIN32
        struct _stat64 file_stat;
        if (lw_string_to_wchar(CP_UTF8, lwhp->file_path, &wname))
            _wstat64(wname, &file_stat);
        else
            _stat64(lwhp->file_path, &file_stat);
#else
        struct stat file_stat;
        stat(lwhp->file_path, &file_stat);
#endif
        fprintf(index, "<FileSize=%" PRId64 ">\n", file_stat.st_size);
        fprintf(index, "<FileLastModificationTime=%" PRId64 ">\n", file_stat.st_mtime);
        fprintf(index, "<FileHash=0x%016" PRIx64 ">\n", xxhash_file(lwhp->file_path, file_stat.st_size));
        fprintf(index, "<LibavReaderIndex=0x%08x,%d,%s>\n", lwhp->format_flags, lwhp->raw_demuxer, lwhp->format_name);
        video_index_pos = ftell(index);
        fprintf(index, "<ActiveVideoStreamIndex>%+011d</ActiveVideoStreamIndex>\n", -1);
        audio_index_pos = ftell(index);
        fprintf(index, "<ActiveAudioStreamIndex>%+011d</ActiveAudioStreamIndex>\n", adhp->stream_index);
        fprintf(index, "<DefaultAudioStreamIndex>%+011d</DefaultAudioStreamIndex>\n", -1);
        fprintf(index, "<FillAudioGaps>%d</FillAudioGaps>\n", aohp->fill_audio_gaps);
    }
    AVPacket pkt = { 0 };
    int pix_fmt_investigated = 0;
    int video_resolution = 0;
    int is_attached_pic = 0;
    uint32_t video_sample_count = 0;
    uint32_t invisible_count = 0;
    uint32_t video_keyframe_count = 0;
    int64_t last_keyframe_pts = AV_NOPTS_VALUE;
    uint32_t audio_sample_count = 0;
    int audio_sample_rate = 0;
    int constant_frame_length = 1;
    uint64_t audio_duration = 0;
    int64_t first_dts = AV_NOPTS_VALUE;
    int64_t filesize = avio_size(format_ctx->pb);
    int consistent_repeat_pict = 1;
    int consistent_field_order = 1;
    const int rap_verification = opt->rap_verification;
    if (indicator->open)
        indicator->open(php);
    /* Start to read frames and write the index file. */
    lwindex_indexer_t indexer = {
        0, /* number_of_helpers */
        NULL, /* helpers */
        vdhp->preferred_decoder_names, /* preferred_video_decoder_names */
        vdhp->prefer_hw_decoder, /* prefer_video_hw_decoder */
        adhp->preferred_decoder_names, /* preferred_audio_decoder_names */
        lwhp->threads, /* thread_count */
        lwhp->format_name, /* format_name */
        vdhp->hw_device_ctx /* hw device buffer */
    };
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        AVStream* stream = format_ctx->streams[stream_index];
        enum AVMediaType codec_type = stream->codecpar->codec_type;
        if (codec_type != AVMEDIA_TYPE_VIDEO && (codec_type != AVMEDIA_TYPE_AUDIO || adhp->stream_index == -2))
            continue;
        lwindex_helper_t* helper = get_index_helper(&indexer, stream, rap_verification);
        if (!helper || !helper->codec_ctx)
            continue;
        AVCodecContext* pkt_ctx = helper->codec_ctx;
        print_index(index, "<StreamInfo=%d,%d>\n", stream_index, codec_type);
        if (codec_type == AVMEDIA_TYPE_VIDEO)
            print_index(index, "Codec=%d,TimeBase=%d/%d,Width=%d,Height=%d,Format=%s,ColorSpace=%d\n", pkt_ctx->codec_id,
                stream->time_base.num, stream->time_base.den, pkt_ctx->width, pkt_ctx->height,
                av_get_pix_fmt_name(pkt_ctx->pix_fmt) ? av_get_pix_fmt_name(pkt_ctx->pix_fmt) : "none", pkt_ctx->colorspace);
        else {
            if (pkt_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
                av_channel_layout_default(&pkt_ctx->ch_layout, pkt_ctx->ch_layout.nb_channels);
            int bits_per_sample = pkt_ctx->bits_per_raw_sample > 0 ? pkt_ctx->bits_per_raw_sample
                : pkt_ctx->bits_per_coded_sample > 0               ? pkt_ctx->bits_per_coded_sample
                                                                   : av_get_bytes_per_sample(pkt_ctx->sample_fmt) << 3;
            print_index(index, "Codec=%d,TimeBase=%d/%d,Channels=%d:0x%" PRIx64 ",Rate=%d,Format=%s,BPS=%d\n", pkt_ctx->codec_id,
                stream->time_base.num, stream->time_base.den, pkt_ctx->ch_layout.nb_channels, pkt_ctx->ch_layout.u.mask,
                pkt_ctx->sample_rate, av_get_sample_fmt_name(pkt_ctx->sample_fmt) ? av_get_sample_fmt_name(pkt_ctx->sample_fmt) : "none",
                bits_per_sample);
        }
        print_index(index, "</StreamInfo>\n");
    }
    while (read_av_frame(format_ctx, &pkt) >= 0) {
        AVStream* stream = format_ctx->streams[pkt.stream_index];
        AVCodecParameters* codecpar = stream->codecpar;
        if (codecpar->codec_type != AVMEDIA_TYPE_VIDEO && codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
            stream->discard = AVDISCARD_ALL;
            continue;
        }
        if (codecpar->codec_id == AV_CODEC_ID_NONE) {
            stream->discard = AVDISCARD_ALL;
            continue;
        }
        lwindex_helper_t* helper = get_index_helper(&indexer, stream, rap_verification);
        if (!helper) {
            av_packet_unref(&pkt);
            goto fail_index;
        }
        AVCodecContext* pkt_ctx = helper->codec_ctx;
        if (!pkt_ctx) {
            stream->discard = AVDISCARD_ALL;
            continue;
        }
        int extradata_index = append_extradata_if_new(helper, pkt_ctx, &pkt);
        if (extradata_index < 0) {
            av_packet_unref(&pkt);
            goto fail_index;
        }
        if (pkt_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (pkt_ctx->pix_fmt == AV_PIX_FMT_NONE || (pkt_ctx->codec->wrapper_name && !pix_fmt_investigated)) {
                investigate_pix_fmt_by_decoding(pkt_ctx, &pkt, vdhp->frame_buffer);
                pix_fmt_investigated = 1;
            }
            int dv_in_avi_init = 0;
            if (adhp->dv_in_avi == -1 && vdhp->stream_index == -1 && pkt_ctx->codec_id == AV_CODEC_ID_DVVIDEO && opt->force_audio == 0) {
                dv_in_avi_init = 1;
                adhp->dv_in_avi = 1;
                vdhp->stream_index = pkt.stream_index;
            }
            /* Replace lower resolution stream with higher. Override attached picture. */
            int higher_priority = ((pkt_ctx->width * pkt_ctx->height > video_resolution)
                || (is_attached_pic && !(stream->disposition & AV_DISPOSITION_ATTACHED_PIC)));
            if (dv_in_avi_init
                || (!opt->force_video && (vdhp->stream_index == -1 || (pkt.stream_index != vdhp->stream_index && higher_priority)))
                || (opt->force_video && vdhp->stream_index == -1 && pkt.stream_index == opt->force_video_index)) {
                /* Update active video stream. */
                if (index) {
                    int32_t current_pos = ftell(index);
                    fseek(index, video_index_pos, SEEK_SET);
                    fprintf(index, "<ActiveVideoStreamIndex>%+011d</ActiveVideoStreamIndex>\n", pkt.stream_index);
                    fseek(index, current_pos, SEEK_SET);
                }
                memset(video_info, 0, (video_sample_count + 1) * sizeof(video_frame_info_t));
                vdhp->ctx = pkt_ctx;
                vdhp->codec_id = pkt_ctx->codec_id;
                vdhp->stream_index = pkt.stream_index;
                video_resolution = pkt_ctx->width * pkt_ctx->height;
                is_attached_pic = !!(stream->disposition & AV_DISPOSITION_ATTACHED_PIC);
                video_sample_count = 0;
                last_keyframe_pts = AV_NOPTS_VALUE;
                vdhp->max_width = pkt_ctx->width;
                vdhp->max_height = pkt_ctx->height;
                vdhp->initial_width = pkt_ctx->width;
                vdhp->initial_height = pkt_ctx->height;
                vdhp->initial_colorspace = pkt_ctx->colorspace;
            }
            /* Get picture type. */
            int pict_type = get_picture_type(helper, pkt_ctx, &pkt, rap_verification);
            if (pict_type < 0) {
                av_packet_unref(&pkt);
                goto fail_index;
            }
            /* Get Picture Order Count. */
            int poc = helper->parser_ctx ? helper->parser_ctx->output_picture_number : 0;
            /* Get field information. */
            int repeat_pict;
            lw_field_info_t field_info;
            int is_superframe = 0;
            if (helper->parser_ctx) {
                if (helper->parser_ctx->picture_structure == AV_PICTURE_STRUCTURE_TOP_FIELD
                    || helper->parser_ctx->picture_structure == AV_PICTURE_STRUCTURE_BOTTOM_FIELD) {
                    /* field coded picture */
                    if (helper->parser_ctx->picture_structure == AV_PICTURE_STRUCTURE_TOP_FIELD)
                        field_info = LW_FIELD_INFO_TOP;
                    else
                        field_info = LW_FIELD_INFO_BOTTOM;
                    repeat_pict = helper->parser_ctx->repeat_pict;
                } else {
                    /* frame coded picture */
                    if (helper->parser_ctx->field_order == AV_FIELD_TT || helper->parser_ctx->field_order == AV_FIELD_TB)
                        field_info = LW_FIELD_INFO_TOP;
                    else if (helper->parser_ctx->field_order == AV_FIELD_BB || helper->parser_ctx->field_order == AV_FIELD_BT)
                        field_info = LW_FIELD_INFO_BOTTOM;
                    else
                        field_info = helper->last_field_info;
                    if (get_ticks_per_frame(pkt_ctx) == 2 && helper->parser_ctx->repeat_pict != 0)
                        repeat_pict = helper->parser_ctx->repeat_pict;
                    else
                        repeat_pict = 2 * helper->parser_ctx->repeat_pict + 1;
                }
                helper->last_field_info = field_info;
            } else {
                repeat_pict = 1;
                field_info = helper->last_field_info;
            }
            /* Set video frame info if this stream is active. */
            if (pkt.stream_index == vdhp->stream_index) {
                ++video_sample_count;
                video_frame_info_t* info = &video_info[video_sample_count];
                if (pkt_ctx->codec_id != AV_CODEC_ID_VP8 && pkt_ctx->codec_id != AV_CODEC_ID_VP9) {
                    if (video_sample_count > 1) {
                        const video_frame_info_t* const first_info = &video_info[1];
                        if (consistent_repeat_pict && repeat_pict != first_info->repeat_pict)
                            consistent_repeat_pict = 0;
                        if (consistent_field_order && field_info != first_info->field_info)
                            consistent_field_order = 0;
                    }
                }
                memset(info, 0, sizeof(video_frame_info_t));
                info->pts = pkt.pts;
                info->dts = pkt.dts;
                info->file_offset = pkt.pos;
                info->sample_number = video_sample_count;
                info->extradata_index = extradata_index;
                info->pict_type = pict_type;
                info->poc = poc;
                info->repeat_pict = repeat_pict;
                info->field_info = field_info;
                if (pkt.pts != AV_NOPTS_VALUE && last_keyframe_pts != AV_NOPTS_VALUE && pkt.pts < last_keyframe_pts)
                    info->flags |= LW_VFRAME_FLAG_LEADING;
                if (pkt.flags & AV_PKT_FLAG_KEY) {
                    /* For the present, treat this frame as a keyframe. */
                    info->flags |= LW_VFRAME_FLAG_KEY;
                    last_keyframe_pts = pkt.pts;
                    ++video_keyframe_count;
                }
                if (repeat_pict == 0 && field_info == LW_FIELD_INFO_UNKNOWN && pkt_ctx->pix_fmt == AV_PIX_FMT_NONE
                    && (pkt_ctx->codec_id == AV_CODEC_ID_H264 || pkt_ctx->codec_id == AV_CODEC_ID_HEVC)
                    && (pkt_ctx->width == 0 || pkt_ctx->height == 0))
                    info->flags |= LW_VFRAME_FLAG_CORRUPT;
                if (pkt_ctx->codec_id == AV_CODEC_ID_VP8 && check_vp8_invisible_frame(&pkt)) {
                    /* VPx invisible altref frame. */
                    info->pts = AV_NOPTS_VALUE;
                    info->dts = AV_NOPTS_VALUE;
                    info->file_offset = -1;
                    info->flags |= LW_VFRAME_FLAG_INVISIBLE;
                    ++invisible_count;
                    /* backward compatible hack for the index */
                    pkt.pts = AV_NOPTS_VALUE;
                    pkt.dts = AV_NOPTS_VALUE;
                    pkt.pos = -1;
                }
                if (pkt_ctx->codec_id == AV_CODEC_ID_VP9 && is_vp9_superframe(&pkt))
                    is_superframe = 1;
                info->is_superframe = is_superframe;
                if (vdhp->time_base.num == 0 || vdhp->time_base.den == 0) {
                    vdhp->time_base.num = stream->time_base.num;
                    vdhp->time_base.den = stream->time_base.den;
                }
                /* Set maximum resolution. */
                if (vdhp->max_width < pkt_ctx->width)
                    vdhp->max_width = pkt_ctx->width;
                if (vdhp->max_height < pkt_ctx->height)
                    vdhp->max_height = pkt_ctx->height;
                if (video_sample_count + 1 == video_info_count) {
                    video_info_count <<= 1;
                    video_frame_info_t* temp = (video_frame_info_t*)realloc(video_info, video_info_count * sizeof(video_frame_info_t));
                    if (!temp) {
                        av_packet_unref(&pkt);
                        goto fail_index;
                    }
                    video_info = temp;
                }
            }
            /* Set width, height and pixel_format for the current extradata. */
            if (extradata_index >= 0) {
                lwlibav_extradata_handler_t* list = &helper->exh;
                lwlibav_extradata_t* entry = &list->entries[list->current_index];
                if (entry->width < pkt_ctx->width)
                    entry->width = pkt_ctx->width;
                if (entry->height < pkt_ctx->height)
                    entry->height = pkt_ctx->height;
                if (entry->pixel_format == AV_PIX_FMT_NONE)
                    entry->pixel_format = pkt_ctx->pix_fmt;
                if (entry->bits_per_sample == 0)
                    entry->bits_per_sample = pkt_ctx->bits_per_coded_sample;
                if (entry->codec_id == AV_CODEC_ID_NONE)
                    entry->codec_id = pkt_ctx->codec_id;
                if (entry->codec_tag == 0)
                    entry->codec_tag = pkt_ctx->codec_tag;
            }
            /* Write a video packet info to the index file. */
            print_index(index,
                "Index=%d,POS=%" PRId64 ",PTS=%" PRId64 ",DTS=%" PRId64 ",EDI=%d\n"
                "Key=%d,Pic=%d,POC=%d,Repeat=%d,Field=%d,Super=%d\n",
                pkt.stream_index, pkt.pos, pkt.pts, pkt.dts, extradata_index, !!(pkt.flags & AV_PKT_FLAG_KEY), pict_type, poc, repeat_pict,
                field_info, is_superframe);
        } else if (adhp->stream_index != -2) {
            if (adhp->stream_index == -1 && (!opt->force_audio || (opt->force_audio && pkt.stream_index == opt->force_audio_index))) {
                /* Update active audio stream. */
                if (index) {
                    int32_t current_pos = ftell(index);
                    fseek(index, audio_index_pos, SEEK_SET);
                    fprintf(index, "<ActiveAudioStreamIndex>%+011d</ActiveAudioStreamIndex>\n", pkt.stream_index);
                    fprintf(index, "<DefaultAudioStreamIndex>%+011d</DefaultAudioStreamIndex>\n", pkt.stream_index);
                    fseek(index, current_pos, SEEK_SET);
                }
                adhp->ctx = pkt_ctx;
                adhp->codec_id = pkt_ctx->codec_id;
                adhp->stream_index = pkt.stream_index;
            }
            int bits_per_sample = pkt_ctx->bits_per_raw_sample > 0 ? pkt_ctx->bits_per_raw_sample
                : pkt_ctx->bits_per_coded_sample > 0               ? pkt_ctx->bits_per_coded_sample
                                                                   : av_get_bytes_per_sample(pkt_ctx->sample_fmt) << 3;
            if (adhp->time_base.num == 0 || adhp->time_base.den == 0) {
                adhp->time_base.num = stream->time_base.num;
                adhp->time_base.den = stream->time_base.den;
            }
            /* Get audio frame_length. */
            int frame_length = get_audio_frame_length(helper, pkt_ctx, &pkt);
            int gaps = 1;
            int added_gaps = 0;
            while (gaps--) {
                /* Set audio frame info if this stream is active. */
                if (pkt.stream_index == adhp->stream_index) {
                    if (frame_length != -1)
                        audio_duration += frame_length;
                    if (audio_duration <= INT32_MAX) {
                        /* Set up audio frame info. */
                        ++audio_sample_count;
                        audio_frame_info_t* info = &audio_info[audio_sample_count];
                        memset(info, 0, sizeof(audio_frame_info_t));
                        info->pts = pkt.pts;
                        info->dts = pkt.dts;
                        info->file_offset = pkt.pos;
                        info->sample_number = audio_sample_count;
                        info->extradata_index = extradata_index;
                        info->sample_rate = pkt_ctx->sample_rate;
                        if (frame_length != -1 && audio_sample_count > helper->delay_count) {
                            const uint32_t audio_frame_number = audio_sample_count - helper->delay_count;
                            audio_info[audio_frame_number].length = frame_length;
                            if (audio_frame_number > 1
                                && audio_info[audio_frame_number].length != audio_info[audio_frame_number - 1].length)
                                constant_frame_length = 0;
                            if (aohp->fill_audio_gaps && audio_sample_count > 1 && !added_gaps) {
                                const audio_frame_info_t* const prev_info = &audio_info[audio_sample_count - 1];
                                const int64_t prev_pts = prev_info->pts;
                                if (info->pts != AV_NOPTS_VALUE && prev_pts != AV_NOPTS_VALUE) {
                                    const int64_t prev_duration
                                        = av_rescale_q(prev_info->length, (AVRational) { 1, aohp->output_sample_rate }, adhp->time_base);
                                    const int64_t prev_end = prev_pts + prev_duration;
                                    const int64_t time_diff = info->pts - prev_end;
                                    if (time_diff > aohp->fill_audio_gaps) {
                                        info->length
                                            = (int)av_rescale_q(time_diff, adhp->time_base, (AVRational) { 1, aohp->output_sample_rate });
                                        info->pts = prev_end;
                                        info->dts = prev_info->dts + prev_duration;
                                        info->file_offset = -1;
                                        print_index(index,
                                            "Index=%d,POS=%" PRId64 ",PTS=%" PRId64 ",DTS=%" PRId64 ",EDI=%d\n"
                                            "Length=%d\n",
                                            pkt.stream_index, info->file_offset, info->pts, info->dts, extradata_index, info->length);
                                        gaps++;
                                        added_gaps++;
                                    }
                                }
                            }
                        }
                        if (audio_sample_rate == 0)
                            audio_sample_rate = pkt_ctx->sample_rate;
                        if (audio_sample_count + 1 == audio_info_count) {
                            audio_info_count <<= 1;
                            audio_frame_info_t* temp
                                = (audio_frame_info_t*)realloc(audio_info, audio_info_count * sizeof(audio_frame_info_t));
                            if (!temp) {
                                av_packet_unref(&pkt);
                                goto fail_index;
                            }
                            audio_info = temp;
                        }
                        if (pkt_ctx->ch_layout.nb_channels > aohp->output_channel_layout.nb_channels)
                            av_channel_layout_copy(&aohp->output_channel_layout, &pkt_ctx->ch_layout);
                        aohp->output_sample_format = select_better_sample_format(aohp->output_sample_format, pkt_ctx->sample_fmt);
                        aohp->output_sample_rate = MAX(aohp->output_sample_rate, audio_sample_rate);
                        aohp->output_bits_per_sample = MAX(aohp->output_bits_per_sample, bits_per_sample);
                    }
                }
                /* Set channel_layout, sample_rate, sample_format and bits_per_sample for the current extradata. */
                if (extradata_index >= 0) {
                    lwlibav_extradata_handler_t* list = &helper->exh;
                    lwlibav_extradata_t* entry = &list->entries[list->current_index];
                    if (entry->channel_layout == 0)
                        entry->channel_layout = pkt_ctx->ch_layout.u.mask;
                    if (entry->sample_rate == 0)
                        entry->sample_rate = pkt_ctx->sample_rate;
                    if (entry->sample_format == AV_SAMPLE_FMT_NONE)
                        entry->sample_format = pkt_ctx->sample_fmt;
                    if (entry->bits_per_sample == 0)
                        entry->bits_per_sample = bits_per_sample;
                    if (entry->block_align == 0)
                        entry->block_align = pkt_ctx->block_align;
                    if (entry->codec_id == AV_CODEC_ID_NONE)
                        entry->codec_id = pkt_ctx->codec_id;
                    if (entry->codec_tag == 0)
                        entry->codec_tag = pkt_ctx->codec_tag;
                }
            }
            /* Write an audio packet info to the index file. */
            print_index(index,
                "Index=%d,POS=%" PRId64 ",PTS=%" PRId64 ",DTS=%" PRId64 ",EDI=%d\n"
                "Length=%d\n",
                pkt.stream_index, pkt.pos, pkt.pts, pkt.dts, extradata_index, frame_length);
        } else
            stream->discard = AVDISCARD_ALL;
        if (indicator->update) {
            /* Update progress dialog. */
            int percent = 0;
            if (first_dts == AV_NOPTS_VALUE)
                first_dts = pkt.dts;
            if (filesize > 0 && format_ctx->pb->pos > 0)
                /* Update if I/O context's file offset is valid. */
                percent = (int)(100.0 * ((double)format_ctx->pb->pos / filesize) + 0.5);
            else if (format_ctx->duration > 0 && first_dts != AV_NOPTS_VALUE && pkt.dts != AV_NOPTS_VALUE)
                /* Update if packet's DTS is valid. */
                percent = (int)(100.0 * (pkt.dts - first_dts) * (stream->time_base.num / (double)stream->time_base.den)
                        / (format_ctx->duration / AV_TIME_BASE)
                    + 0.5);
            const char* message = index ? "Creating Index file" : "Parsing input file";
            int abort = indicator->update(php, message, percent);
            av_packet_unref(&pkt);
            if (abort)
                goto fail_index;
        } else
            av_packet_unref(&pkt);
    }
    /* Handle delay derived from the audio decoder. */
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        AVStream* stream = format_ctx->streams[stream_index];
        lwindex_helper_t* helper = get_index_helper(&indexer, stream, 0);
        if (!helper || !helper->codec_ctx || !helper->decode || stream->codecpar->codec_type != AVMEDIA_TYPE_AUDIO
            || adhp->stream_index == -2)
            continue;
        AVCodecContext* pkt_ctx = helper->codec_ctx;
        /* Flush if decoding is delayed. */
        for (uint32_t i = 1; i <= helper->delay_count; i++) {
            AVPacket null_pkt = { 0 };
            int decode_complete;
            if (helper->decode(pkt_ctx, helper->picture, &decode_complete, &null_pkt) >= 0) {
                int frame_length = decode_complete ? helper->picture->nb_samples : 0;
                if (stream_index == adhp->stream_index) {
                    audio_duration += frame_length;
                    if (audio_duration > INT32_MAX)
                        break;
                    uint32_t audio_frame_number = audio_sample_count - helper->delay_count + i;
                    audio_info[audio_frame_number].length = frame_length;
                    if (audio_frame_number > 1 && audio_info[audio_frame_number].length != audio_info[audio_frame_number - 1].length)
                        constant_frame_length = 0;
                }
                print_index(index,
                    "Index=%d,POS=-1,PTS=%" PRId64 ",DTS=%" PRId64 ",EDI=-1\n"
                    "Length=%d\n",
                    stream_index, AV_NOPTS_VALUE, AV_NOPTS_VALUE, frame_length);
            }
        }
    }
    print_index(index, "</LibavReaderIndex>\n");
    const int consistent_field_and_repeat = consistent_field_order && consistent_repeat_pict;
    print_index(index, "<VideoConsistentFieldRepeatPict>%d</VideoConsistentFieldRepeatPict>\n", consistent_field_and_repeat);
    /* Deallocate video frame info if no active video stream. */
    if (vdhp->stream_index < 0)
        lw_freep(&video_info);
    /* Deallocate audio frame info if no active audio stream. */
    if (adhp->stream_index < 0)
        lw_freep(&audio_info);
    else {
        /* Check the active stream is DV in AVI Type-1 or not. */
        if (adhp->dv_in_avi == 1 && avformat_index_get_entries_count(format_ctx->streams[adhp->stream_index]) == 0) {
            /* DV in AVI Type-1 */
            audio_sample_count = video_info ? MIN(video_sample_count, audio_sample_count) : 0;
            for (uint32_t i = 1; i <= audio_sample_count; i++) {
                audio_info[i].keyframe = !!(video_info[i].flags & LW_VFRAME_FLAG_KEY);
                audio_info[i].sample_number = video_info[i].sample_number;
                audio_info[i].pts = video_info[i].pts;
                audio_info[i].dts = video_info[i].dts;
                audio_info[i].file_offset = video_info[i].file_offset;
                audio_info[i].extradata_index = video_info[i].extradata_index;
            }
        } else {
            if (adhp->dv_in_avi == 1 && opt->force_video && opt->force_video_index == -1) {
                /* Disable DV video stream. */
                disable_video_stream(vdhp);
                video_info = NULL;
            }
            adhp->dv_in_avi = 0;
        }
    }
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        AVStream* stream = format_ctx->streams[stream_index];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO
            || (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && adhp->stream_index != -2))
            print_index(index, "<StreamDuration=%d,%d>%" PRId64 "</StreamDuration>\n", stream_index, stream->codecpar->codec_type,
                stream->duration);
    }
    if (!strcmp(lwhp->format_name, "asf")) {
        /* Pretty hackish workaround for the ASF demuxer
         * The Simple Index Object does not always describe all keyframes in corresponding video stream.
         * Since the Simple Index Object cannot indicate PTS offset derived from missing indexes of early keyframes,
         * the Simple Index Object is unreliable on frame-accurate seek. So, this section makes up the indexes from
         * the actual timestamps and file offsets without the Simple Index Object.
         * Here, also construct the indexes for audio stream from the actual timestamps and file offsets if present. */
        for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
            AVStream* stream = format_ctx->streams[stream_index];
            AVIndexEntry* temp = NULL;
            if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                uint32_t i = 0;
                for (uint32_t j = 1; j <= video_sample_count && i < video_keyframe_count; j++)
                    if ((video_info[j].flags & LW_VFRAME_FLAG_KEY)
                        && (video_info[j].pts != AV_NOPTS_VALUE || video_info[j].dts != AV_NOPTS_VALUE)) {
                        av_add_index_entry(stream, video_info[j].file_offset,
                            video_info[j].pts != AV_NOPTS_VALUE ? video_info[j].pts : video_info[j].dts,
                            0, // size
                            0, // distance
                            AVINDEX_KEYFRAME);
                        i++;
                    }
            } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                av_add_index_entry(stream, 0, 0, 0, 0, 0);
                uint32_t i = 0;
                for (uint32_t j = 1; j <= audio_sample_count && i < audio_sample_count; j++) {
                    if (audio_info[j].pts != AV_NOPTS_VALUE || audio_info[j].dts != AV_NOPTS_VALUE) {
                        av_add_index_entry(stream, audio_info[j].file_offset,
                            audio_info[j].pts != AV_NOPTS_VALUE ? audio_info[j].pts : audio_info[j].dts,
                            0, // size
                            0, // distance
                            AVINDEX_KEYFRAME);
                        i++;
                    }
                }
            }
            if (!temp) {
                /* Anyway clear the index entries. */
                av_add_index_entry(stream, 0, 0, 0, 0, 0);
            }
        }
    }
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        AVStream* stream = format_ctx->streams[stream_index];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            print_index(
                index, "<StreamIndexEntries=%d,%d,%d>\n", stream_index, AVMEDIA_TYPE_VIDEO, avformat_index_get_entries_count(stream));
            if (vdhp->stream_index != stream_index)
                for (int i = 0; i < avformat_index_get_entries_count(stream); i++)
                    write_av_index_entry(index, avformat_index_get_entry(stream, i));
            else if (avformat_index_get_entries_count(stream) > 0) {
                vdhp->index_entries = (AVIndexEntry*)av_malloc(avformat_index_get_entries_count(stream) * sizeof(AVIndexEntry));
                if (!vdhp->index_entries)
                    goto fail_index;
                for (int i = 0; i < avformat_index_get_entries_count(stream); i++) {
                    const AVIndexEntry* ie = avformat_index_get_entry(stream, i);
                    vdhp->index_entries[i] = *ie;
                    write_av_index_entry(index, ie);
                }
                vdhp->index_entries_count = avformat_index_get_entries_count(stream);
            }
            print_index(index, "</StreamIndexEntries>\n");
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && adhp->stream_index != -2) {
            print_index(
                index, "<StreamIndexEntries=%d,%d,%d>\n", stream_index, AVMEDIA_TYPE_AUDIO, avformat_index_get_entries_count(stream));
            if (adhp->stream_index != stream_index)
                for (int i = 0; i < avformat_index_get_entries_count(stream); i++)
                    write_av_index_entry(index, avformat_index_get_entry(stream, i));
            else if (avformat_index_get_entries_count(stream) > 0) {
                /* Audio stream in matroska container requires index_entries for seeking.
                 * This avoids for re-reading the file to create index_entries since the file will be closed once. */
                adhp->index_entries = (AVIndexEntry*)av_malloc(avformat_index_get_entries_count(stream) * sizeof(AVIndexEntry));
                if (!adhp->index_entries)
                    goto fail_index;
                for (int i = 0; i < avformat_index_get_entries_count(stream); i++) {
                    const AVIndexEntry* ie = avformat_index_get_entry(stream, i);
                    adhp->index_entries[i] = *ie;
                    write_av_index_entry(index, ie);
                }
                adhp->index_entries_count = avformat_index_get_entries_count(stream);
            }
            print_index(index, "</StreamIndexEntries>\n");
        }
    }
    for (unsigned int stream_index = 0; stream_index < format_ctx->nb_streams; stream_index++) {
        AVStream* stream = format_ctx->streams[stream_index];
        AVCodecParameters* codecpar = stream->codecpar;
        if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO || (codecpar->codec_type == AVMEDIA_TYPE_AUDIO && adhp->stream_index != -2)) {
            lwindex_helper_t* helper = get_index_helper(&indexer, stream, rap_verification);
            if (!helper || !helper->codec_ctx)
                continue;
            lwlibav_extradata_handler_t* list = &helper->exh;
            void (*write_av_extradata)(FILE*, lwlibav_extradata_t*)
                = codecpar->codec_type == AVMEDIA_TYPE_VIDEO ? write_video_extradata : write_audio_extradata;
            print_index(index, "<ExtraDataList=%d,%d,%d>\n", stream_index, codecpar->codec_type, list->entry_count);
            if ((codecpar->codec_type == AVMEDIA_TYPE_VIDEO && stream_index == vdhp->stream_index)
                || (codecpar->codec_type == AVMEDIA_TYPE_AUDIO && stream_index == adhp->stream_index)) {
                for (int i = 0; i < list->entry_count; i++)
                    write_av_extradata(index, &list->entries[i]);
                lwlibav_extradata_handler_t* exhp = codecpar->codec_type == AVMEDIA_TYPE_VIDEO ? &vdhp->exh : &adhp->exh;
                exhp->entry_count = list->entry_count;
                exhp->entries = list->entries;
                exhp->current_index
                    = codecpar->codec_type == AVMEDIA_TYPE_VIDEO ? video_info[1].extradata_index : audio_info[1].extradata_index;
                /* Avoid freeing entries. */
                list->entry_count = 0;
                list->entries = NULL;
            } else
                for (int i = 0; i < list->entry_count; i++)
                    write_av_extradata(index, &list->entries[i]);
            print_index(index, "</ExtraDataList>\n");
        }
    }
    print_index(index, "</LibavReaderIndexFile>\n");
    if (vdhp->stream_index >= 0) {
        vdhp->keyframe_list = (uint8_t*)lw_malloc_zero((video_sample_count + 1) * sizeof(uint8_t));
        if (!vdhp->keyframe_list)
            goto fail_index;
        vdhp->frame_list = video_info;
        vdhp->frame_count = video_sample_count;
        vdhp->initial_pix_fmt = vdhp->ctx->pix_fmt;
        if (decide_video_seek_method(lwhp, vdhp, video_sample_count))
            goto fail_index;
        /* Compute the stream duration. */
        compute_stream_duration(lwhp, vdhp, format_ctx->streams[vdhp->stream_index]->duration);
        /* Create the repeat control info. */
        create_video_frame_order_list(vdhp, vohp, opt, consistent_field_and_repeat);
        /* Exclude invisible frames from the output handler. */
        create_video_visible_frame_list(vdhp, vohp, invisible_count);
    }
    if (adhp->stream_index >= 0) {
        adhp->frame_list = audio_info;
        adhp->frame_count = audio_sample_count;
        adhp->frame_length = constant_frame_length ? adhp->frame_list[1].length : 0;
        decide_audio_seek_method(lwhp, adhp, audio_sample_count);
        if (opt->av_sync && vdhp->stream_index >= 0)
            lwhp->av_gap = calculate_av_gap(vdhp, vohp, adhp, audio_sample_rate);
    }
#ifdef _WIN32
    lw_free(wname);
#endif // _WIN32
    cleanup_index_helpers(&indexer, format_ctx, rap_verification);
    if (index)
        fclose(index);
    if (indicator->close)
        indicator->close(php);
    vdhp->format = NULL;
    adhp->format = NULL;
    return 0;
fail_index:
#ifdef _WIN32
    lw_free(wname);
#endif // _WIN32
    cleanup_index_helpers(&indexer, format_ctx, rap_verification);
    free(video_info);
    free(audio_info);
    if (index)
        fclose(index);
    if (indicator->close)
        indicator->close(php);
    vdhp->format = NULL;
    adhp->format = NULL;
    return -1;
}

static int parse_index_real(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp,
    lwlibav_audio_decode_handler_t* adhp, lwlibav_audio_output_handler_t* aohp, lwlibav_option_t* opt, lwindex_data_t* data, FILE* index)
{
    lwindex_stream_info_t* stream_info = NULL;
    if (!data) {
        return -1;
    }
    /* Test to open the target file. */
    char* file_path = data->input_file_path;
    size_t file_path_length = strlen(opt->file_path);
    const char* ext = file_path_length >= 5 ? &opt->file_path[file_path_length - 4] : NULL;
    if (ext && !strncmp(ext, ".lwi", strlen(".lwi"))) {
        FILE* target = lw_fopen(file_path, "rb");
        if (!target)
            return -1;
        fclose(target);
        file_path_length = strlen(file_path);
        lwhp->file_path = (char*)lw_malloc_zero(file_path_length + 1);
        if (!lwhp->file_path)
            return -1;
        memcpy(lwhp->file_path, file_path, file_path_length);
    } else {
        lwhp->file_path = (char*)lw_malloc_zero(file_path_length + 1);
        if (!lwhp->file_path)
            return -1;
        memcpy(lwhp->file_path, opt->file_path, file_path_length);
    }
    /* Parse the index file. */
#ifdef _WIN32
    wchar_t* wname = NULL;
    struct _stat64 file_stat;
    if (lw_string_to_wchar(CP_UTF8, lwhp->file_path, &wname)) {
        if (_wstat64(wname, &file_stat)) {
            lw_free(wname);
            return -1;
        }
    } else {
        if (_stat64(lwhp->file_path, &file_stat))
            return -1;
    }
#else
    struct stat file_stat;
    if (stat(lwhp->file_path, &file_stat))
        return -1;
#endif
    if (data->file_size != file_stat.st_size)
        return -1;
    if (data->file_last_modification_time != file_stat.st_mtime) {
        // Also check hashsum
        if ((!data->file_hash || data->file_hash != xxhash_file(lwhp->file_path, file_stat.st_size))
            && (!data->file_hash || data->file_hash != xxhash32_file(lwhp->file_path, file_stat.st_size)))
            return -1;
    }
    lwhp->format_flags = data->format_flags;
    lwhp->raw_demuxer = data->raw_demuxer;
    lwhp->format_name = strdup(data->format_name);
    adhp->dv_in_avi = !strcmp(lwhp->format_name, "avi") ? -1 : 0;
    int video_present = (data->active_video_stream_index >= 0);
    int audio_present = (data->active_audio_stream_index >= 0);
    vdhp->stream_index = opt->force_video ? opt->force_video_index : data->active_video_stream_index;
    switch (opt->force_audio_index) {
    case -1: {
        if (data->default_audio_stream_index != data->active_audio_stream_index) {
#ifdef _WIN32
            lw_free(wname);
#endif // _WIN32
            return -1;
        }
    }
    case -2:
        adhp->stream_index = data->active_audio_stream_index;
        break;
    default:
        adhp->stream_index = opt->force_audio_index;
        break;
    }
    uint32_t video_info_count = 1 << 16;
    uint32_t audio_info_count = 1 << 16;
    video_frame_info_t* video_info = NULL;
    audio_frame_info_t* audio_info = NULL;
    if (vdhp->stream_index >= 0) {
        video_info = (video_frame_info_t*)malloc(video_info_count * sizeof(video_frame_info_t));
        if (!video_info)
            goto fail_parsing;
    }
    if (adhp->stream_index >= 0) {
        audio_info = (audio_frame_info_t*)malloc(audio_info_count * sizeof(audio_frame_info_t));
        if (!audio_info)
            goto fail_parsing;
    }
    if (data->active_audio_stream_index == -2 && opt->force_audio_index != -2) // Maybe redundant.
        goto fail_parsing;
    if (opt->force_audio_index != -2 && data->fill_audio_gaps != aohp->fill_audio_gaps)
        goto fail_parsing;
    vdhp->codec_id = AV_CODEC_ID_NONE;
    adhp->codec_id = AV_CODEC_ID_NONE;
    vdhp->initial_pix_fmt = AV_PIX_FMT_NONE;
    vdhp->initial_colorspace = AVCOL_SPC_NB;
    aohp->output_sample_format = AV_SAMPLE_FMT_NONE;
    uint32_t video_sample_count = 0;
    uint32_t invisible_count = 0;
    int64_t last_keyframe_pts = AV_NOPTS_VALUE;
    uint32_t audio_sample_count = 0;
    int audio_sample_rate = 0;
    int constant_frame_length = 1;
    uint64_t audio_duration = 0;

    int max_stream_index = 0;
    for (int i = 0; i < data->num_streams; i++)
        if (data->stream_info[i].stream_index > max_stream_index)
            max_stream_index = data->stream_info[i].stream_index;
    stream_info = (lwindex_stream_info_t*)malloc((max_stream_index + 1) * sizeof(lwindex_stream_info_t));
    if (!stream_info)
        goto fail_parsing;
    for (int i = 0; i < data->num_streams; i++) {
        lwindex_stream_info_t* info = &stream_info[data->stream_info[i].stream_index];
        info->codec_type = data->stream_info[i].codec_type;
        if (info->codec_type == AVMEDIA_TYPE_VIDEO) {
            info->codec_id = data->stream_info[i].codec;
            info->time_base.num = data->stream_info[i].time_base.num;
            info->time_base.den = data->stream_info[i].time_base.den;
            info->width = data->stream_info[i].data.type0.width;
            info->height = data->stream_info[i].data.type0.height;
            strncpy(info->fmt, data->stream_info[i].format, FORMAT_LENGTH);
            info->fmt[FORMAT_LENGTH - 1] = '\0';
            info->colorspace = data->stream_info[i].data.type0.color_space;

            if (data->stream_info[i].stream_index == vdhp->stream_index)
                vdhp->stream_duration = data->stream_info[i].stream_duration;
        } else if (info->codec_type == AVMEDIA_TYPE_AUDIO) {
            info->codec_id = data->stream_info[i].codec;
            info->time_base.num = data->stream_info[i].time_base.num;
            info->time_base.den = data->stream_info[i].time_base.den;
            info->channels = data->stream_info[i].data.type1.channels;
            info->layout = data->stream_info[i].data.type1.layout;
            info->sample_rate = data->stream_info[i].data.type1.sample_rate;
            strncpy(info->fmt, data->stream_info[i].format, FORMAT_LENGTH);
            info->fmt[FORMAT_LENGTH - 1] = '\0';
            info->bits_per_sample = data->stream_info[i].bits_per_sample;
        }
    }

    for (int i = 0; i < data->num_index_entries; i++) {
        index_entry_t* entry = &data->index_entries[i];
        int stream_index = entry->stream_index;
        int codec_type = stream_info[stream_index].codec_type;
        int codec_id = stream_info[stream_index].codec_id;
        AVRational time_base = stream_info[stream_index].time_base;

        if (codec_type == AVMEDIA_TYPE_VIDEO) {
            if (adhp->dv_in_avi == -1 && codec_id == AV_CODEC_ID_DVVIDEO && !opt->force_audio) {
                adhp->dv_in_avi = 1;
                if (vdhp->stream_index == -1) {
                    vdhp->stream_index = stream_index;
                    video_info = (video_frame_info_t*)malloc(video_info_count * sizeof(video_frame_info_t));
                    if (!video_info)
                        goto fail_parsing;
                }
            }
            if (stream_index == vdhp->stream_index) {
                int width = stream_info[stream_index].width;
                int height = stream_info[stream_index].height;
                char* pix_fmt = stream_info[stream_index].fmt;
                int colorspace = stream_info[stream_index].colorspace;

                int key = entry->data.type0.key;
                int pict_type = entry->data.type0.pic;
                if (vdhp->codec_id == AV_CODEC_ID_NONE)
                    vdhp->codec_id = (enum AVCodecID)codec_id;
                if ((key | width | height) || pict_type == -1 || colorspace != AVCOL_SPC_NB) {
                    if (vdhp->initial_width == 0 || vdhp->initial_height == 0) {
                        vdhp->initial_width = width;
                        vdhp->initial_height = height;
                        vdhp->max_width = width;
                        vdhp->max_height = height;
                    } else {
                        if (vdhp->max_width < width)
                            vdhp->max_width = width;
                        if (vdhp->max_height < height)
                            vdhp->max_height = height;
                    }
                    if (vdhp->initial_pix_fmt == AV_PIX_FMT_NONE)
                        vdhp->initial_pix_fmt = av_get_pix_fmt(pix_fmt);
                    if (vdhp->initial_colorspace == AVCOL_SPC_NB)
                        vdhp->initial_colorspace = (enum AVColorSpace)colorspace;
                    if (vdhp->time_base.num == 0 || vdhp->time_base.den == 0) {
                        vdhp->time_base.num = time_base.num;
                        vdhp->time_base.den = time_base.den;
                    }
                    ++video_sample_count;
                    video_frame_info_t* info = &video_info[video_sample_count];
                    memset(info, 0, sizeof(video_frame_info_t));
                    info->pts = entry->pts;
                    info->dts = entry->dts;
                    info->file_offset = entry->pos;
                    info->sample_number = video_sample_count;
                    info->extradata_index = entry->edi;
                    info->pict_type = entry->data.type0.pic;
                    info->poc = entry->data.type0.poc;
                    info->repeat_pict = entry->data.type0.repeat;
                    info->field_info = (lw_field_info_t)entry->data.type0.field;
                    info->is_superframe = entry->data.type0.super;
                    if (entry->pts != AV_NOPTS_VALUE && last_keyframe_pts != AV_NOPTS_VALUE && entry->pts < last_keyframe_pts)
                        info->flags |= LW_VFRAME_FLAG_LEADING;
                    if (key) {
                        info->flags |= LW_VFRAME_FLAG_KEY;
                        last_keyframe_pts = entry->pts;
                    }
                    if (info->repeat_pict == 0 && info->field_info == LW_FIELD_INFO_UNKNOWN && av_get_pix_fmt(pix_fmt) == AV_PIX_FMT_NONE
                        && ((enum AVCodecID)codec_id == AV_CODEC_ID_H264 || (enum AVCodecID)codec_id == AV_CODEC_ID_HEVC)
                        && (width == 0 || height == 0))
                        info->flags |= LW_VFRAME_FLAG_CORRUPT;
                    if ((enum AVCodecID)codec_id == AV_CODEC_ID_VP8 && info->pts == AV_NOPTS_VALUE && info->dts == AV_NOPTS_VALUE
                        && info->file_offset == -1) {
                        /* VPx invisible altref frame. */
                        info->flags |= LW_VFRAME_FLAG_INVISIBLE;
                        ++invisible_count;
                    }
                }
                if (video_sample_count + 1 == video_info_count) {
                    video_info_count <<= 1;
                    video_frame_info_t* temp = (video_frame_info_t*)realloc(video_info, video_info_count * sizeof(video_frame_info_t));
                    if (!temp)
                        goto fail_parsing;
                    video_info = temp;
                }
            }
        } else if (codec_type == AVMEDIA_TYPE_AUDIO) {
            if (stream_index == adhp->stream_index) {
                uint64_t layout = stream_info[stream_index].layout;
                int channels = stream_info[stream_index].channels;
                int sample_rate = stream_info[stream_index].sample_rate;
                char* sample_fmt = stream_info[stream_index].fmt;
                int bits_per_sample = stream_info[stream_index].bits_per_sample;
                int frame_length = entry->data.type1.length;
                if (adhp->codec_id == AV_CODEC_ID_NONE)
                    adhp->codec_id = (enum AVCodecID)codec_id;
                if ((channels | layout | sample_rate | bits_per_sample) && entry->edi != -1 && audio_duration <= INT32_MAX) {
                    if (audio_sample_rate == 0)
                        audio_sample_rate = sample_rate;
                    if (adhp->time_base.num == 0 || adhp->time_base.den == 0) {
                        adhp->time_base.num = time_base.num;
                        adhp->time_base.den = time_base.den;
                    }
                    if (channels > aohp->output_channel_layout.nb_channels)
                        av_channel_layout_from_mask(&aohp->output_channel_layout, layout);
                    aohp->output_sample_format = select_better_sample_format(aohp->output_sample_format, av_get_sample_fmt(sample_fmt));
                    aohp->output_sample_rate = MAX(aohp->output_sample_rate, audio_sample_rate);
                    aohp->output_bits_per_sample = MAX(aohp->output_bits_per_sample, bits_per_sample);
                    ++audio_sample_count;
                    audio_frame_info_t* info = &audio_info[audio_sample_count];
                    memset(info, 0, sizeof(audio_frame_info_t));
                    info->pts = entry->pts;
                    info->dts = entry->dts;
                    info->file_offset = entry->pos;
                    info->sample_number = audio_sample_count;
                    info->extradata_index = entry->edi;
                    info->sample_rate = sample_rate;
                } else
                    for (uint32_t i = 1; i <= adhp->exh.delay_count; i++) {
                        uint32_t audio_frame_number = audio_sample_count - adhp->exh.delay_count + i;
                        if (audio_frame_number > audio_sample_count)
                            goto fail_parsing;
                        audio_info[audio_frame_number].length = frame_length;
                        if (audio_frame_number > 1 && audio_info[audio_frame_number].length != audio_info[audio_frame_number - 1].length)
                            constant_frame_length = 0;
                        audio_duration += frame_length;
                    }
                if (audio_sample_count + 1 == audio_info_count) {
                    audio_info_count <<= 1;
                    audio_frame_info_t* temp = (audio_frame_info_t*)realloc(audio_info, audio_info_count * sizeof(audio_frame_info_t));
                    if (!temp)
                        goto fail_parsing;
                    audio_info = temp;
                }
                if (frame_length == -1)
                    ++adhp->exh.delay_count;
                else if (audio_sample_count > adhp->exh.delay_count) {
                    uint32_t audio_frame_number = audio_sample_count - adhp->exh.delay_count;
                    audio_info[audio_frame_number].length = frame_length;
                    if (audio_frame_number > 1 && audio_info[audio_frame_number].length != audio_info[audio_frame_number - 1].length)
                        constant_frame_length = 0;
                    audio_duration += frame_length;
                }
            }
        }
    }
    if (video_present && opt->force_video && opt->force_video_index != -1
        && (video_sample_count == 0 || vdhp->initial_pix_fmt == AV_PIX_FMT_NONE || vdhp->initial_width == 0 || vdhp->initial_height == 0))
        goto fail_parsing; /* Need to re-create the index file. */
    if (audio_present && opt->force_audio && opt->force_audio_index != -1 && (audio_sample_count == 0 || audio_duration == 0))
        goto fail_parsing; /* Need to re-create the index file. */
    /* Parse stream durations. */

    /* Parse AVIndexEntry. */
    for (int i = 0; i < data->num_streams; i++) {
        stream_info_entry_t* info = &data->stream_info[i];
        int stream_index = info->stream_index;
        int codec_type = info->codec_type;
        int index_entries_count = info->num_stream_index_entries;
        if (index_entries_count > 0) {
            if (codec_type == AVMEDIA_TYPE_VIDEO && stream_index == vdhp->stream_index) {
                vdhp->index_entries_count = index_entries_count;
                vdhp->index_entries = (AVIndexEntry*)av_malloc(vdhp->index_entries_count * sizeof(AVIndexEntry));
                if (!vdhp->index_entries)
                    goto fail_parsing;
                for (int i = 0; i < vdhp->index_entries_count; i++) {
                    vdhp->index_entries[i].pos = info->stream_index_entries[i].pos;
                    vdhp->index_entries[i].timestamp = info->stream_index_entries[i].ts;
                    vdhp->index_entries[i].flags = info->stream_index_entries[i].flags;
                    vdhp->index_entries[i].size = info->stream_index_entries[i].size;
                    vdhp->index_entries[i].min_distance = info->stream_index_entries[i].distance;
                }
            } else if (codec_type == AVMEDIA_TYPE_AUDIO && stream_index == adhp->stream_index) {
                adhp->index_entries_count = index_entries_count;
                adhp->index_entries = (AVIndexEntry*)av_malloc(adhp->index_entries_count * sizeof(AVIndexEntry));
                if (!adhp->index_entries)
                    goto fail_parsing;
                for (int i = 0; i < adhp->index_entries_count; i++) {
                    adhp->index_entries[i].pos = info->stream_index_entries[i].pos;
                    adhp->index_entries[i].timestamp = info->stream_index_entries[i].ts;
                    adhp->index_entries[i].flags = info->stream_index_entries[i].flags;
                    adhp->index_entries[i].size = info->stream_index_entries[i].size;
                    adhp->index_entries[i].min_distance = info->stream_index_entries[i].distance;
                }
            }
        }
    }
    for (int i = 0; i < data->num_extra_data_list; i++) {
        extra_data_list_t* info = &data->extra_data_list[i];
        int stream_index = info->stream_index;
        int codec_type = info->codec_type;
        int entry_count = info->entry_count;
        if (entry_count > 0) {
            if ((codec_type == AVMEDIA_TYPE_VIDEO && stream_index == vdhp->stream_index)
                || (codec_type == AVMEDIA_TYPE_AUDIO && stream_index == adhp->stream_index)) {
                lwlibav_extradata_handler_t* exhp = codec_type == AVMEDIA_TYPE_VIDEO ? &vdhp->exh : &adhp->exh;
                if (!alloc_extradata_entries(exhp, entry_count))
                    goto fail_parsing;
                exhp->current_index = codec_type == AVMEDIA_TYPE_VIDEO ? video_info[1].extradata_index : audio_info[1].extradata_index;
                for (int i = 0; i < exhp->entry_count; i++) {
                    lwlibav_extradata_t* entry = &exhp->entries[i];
                    /* Get extradata size and others. */
                    if (codec_type == AVMEDIA_TYPE_VIDEO) {
                        entry->extradata_size = info->entries[i].size;
                        entry->codec_tag = info->entries[i].fourcc;
                        entry->width = info->entries[i].data.type0.width;
                        entry->height = info->entries[i].data.type0.height;
                        entry->bits_per_sample = info->entries[i].bits_per_sample;
                        entry->pixel_format = av_get_pix_fmt(info->entries[i].format);
                    } else {
                        entry->extradata_size = info->entries[i].size;
                        entry->codec_tag = info->entries[i].fourcc;
                        entry->channel_layout = info->entries[i].data.type1.layout;
                        entry->sample_rate = info->entries[i].data.type1.sample_rate;
                        entry->bits_per_sample = info->entries[i].bits_per_sample;
                        entry->block_align = info->entries[i].data.type1.block_align;
                        entry->sample_format = av_get_sample_fmt(info->entries[i].format);
                    }
                    entry->codec_id = (enum AVCodecID)info->entries[i].codec;
                    /* Get extradata. */
                    if (entry->extradata_size > 0) {
                        entry->extradata = (uint8_t*)av_malloc(entry->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
                        if (!entry->extradata)
                            goto fail_parsing;
                        memcpy(entry->extradata, info->entries[i].binary_data, entry->extradata_size);
                        memset(entry->extradata + entry->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
                    }
                }
            }
        }
    }
    if (vdhp->stream_index >= 0) {
        vdhp->keyframe_list = (uint8_t*)lw_malloc_zero((video_sample_count + 1) * sizeof(uint8_t));
        if (!vdhp->keyframe_list)
            goto fail_parsing;
        vdhp->frame_list = video_info;
        vdhp->frame_count = video_sample_count;
        if (decide_video_seek_method(lwhp, vdhp, video_sample_count))
            goto fail_parsing;
        /* Compute the stream duration. */
        compute_stream_duration(lwhp, vdhp, vdhp->stream_duration);
        /* Create the repeat control info. */
        create_video_frame_order_list(vdhp, vohp, opt, data->consistent_field_and_repeat);
        /* Exclude invisible frames from the output handler. */
        create_video_visible_frame_list(vdhp, vohp, invisible_count);
    }
    if (adhp->stream_index >= 0) {
        if (adhp->dv_in_avi == 1 && adhp->index_entries_count == 0) {
            /* DV in AVI Type-1 */
            audio_sample_count = MIN(video_sample_count, audio_sample_count);
            for (uint32_t i = 0; i <= audio_sample_count; i++) {
                audio_info[i].keyframe = !!(video_info[i].flags & LW_VFRAME_FLAG_KEY);
                audio_info[i].sample_number = video_info[i].sample_number;
                audio_info[i].pts = video_info[i].pts;
                audio_info[i].dts = video_info[i].dts;
                audio_info[i].file_offset = video_info[i].file_offset;
                audio_info[i].extradata_index = video_info[i].extradata_index;
            }
        } else {
            if (adhp->dv_in_avi == 1
                && ((!opt->force_video && data->active_video_stream_index == -1) || (opt->force_video && opt->force_video_index == -1))) {
                /* Disable DV video stream. */
                disable_video_stream(vdhp);
                video_info = NULL;
            }
            adhp->dv_in_avi = 0;
        }
        adhp->frame_list = audio_info;
        adhp->frame_count = audio_sample_count;
        adhp->frame_length = constant_frame_length ? audio_info[1].length : 0;
        decide_audio_seek_method(lwhp, adhp, audio_sample_count);
        if (opt->av_sync && vdhp->stream_index >= 0)
            lwhp->av_gap = calculate_av_gap(vdhp, vohp, adhp, audio_sample_rate);
    }
    int video_index_changed = (vdhp->stream_index != data->active_video_stream_index);
    int audio_index_changed = (adhp->stream_index != data->active_audio_stream_index);
    if (index) {
        if (video_index_changed && data->active_video_stream_index_pos != -1) {
            fflush(index);
            if (fseek(index, data->active_video_stream_index_pos, SEEK_SET) == 0) {
                fprintf(index, "%+011d", vdhp->stream_index);
                fflush(index);
            }
        }
        if (audio_index_changed && data->active_audio_stream_index_pos != -1) {
            fflush(index);
            if (fseek(index, data->active_audio_stream_index_pos, SEEK_SET) == 0) {
                fprintf(index, "%+011d", adhp->stream_index);
                fflush(index);
            }
        }
    }
#ifdef _WIN32
    lw_free(wname);
#endif // _WIN32
    free(stream_info);
    return 0;
fail_parsing:
#ifdef _WIN32
    lw_free(wname);
#endif // _WIN32
    vdhp->frame_list = NULL;
    adhp->frame_list = NULL;
    if (video_info)
        free(video_info);
    if (audio_info)
        free(audio_info);
    free(stream_info);
    return -1;
}

static int parse_index(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp,
    lwlibav_audio_decode_handler_t* adhp, lwlibav_audio_output_handler_t* aohp, lwlibav_option_t* opt, FILE* index)
{
    rewind(index);
    lwindex_data_t* data = lwindex_parse(index, opt->force_audio_index == -2, opt->force_audio_index != -2);
    if (!data) {
        return -1;
    }
    int ret = parse_index_real(lwhp, vdhp, vohp, adhp, aohp, opt, data, index);
    lwindex_free(data);
    return ret;
}

int lwlibav_construct_index(lwlibav_file_handler_t* lwhp, lwlibav_video_decode_handler_t* vdhp, lwlibav_video_output_handler_t* vohp,
    lwlibav_audio_decode_handler_t* adhp, lwlibav_audio_output_handler_t* aohp, lw_log_handler_t* lhp, lwlibav_option_t* opt,
    progress_indicator_t* indicator, progress_handler_t* php)
{
    /* Try to open the index file. */
    size_t file_path_length = strlen(opt->file_path);
    const char* ext = file_path_length >= 5 ? &opt->file_path[file_path_length - 4] : NULL;
    int has_lwi_ext = ext && !strncmp(ext, ".lwi", strlen(".lwi"));
    FILE* index;
    if (has_lwi_ext)
        index = lw_fopen(opt->file_path, (opt->force_video || opt->force_audio) ? "r+b" : "rb");
    else if (opt->index_file_path)
        index = lw_fopen(opt->index_file_path, (opt->force_video || opt->force_audio) ? "r+b" : "rb");
    else {
        char* index_file_path = create_lwi_path(opt);
        if (!index_file_path)
            return -1;
        index = lw_fopen(index_file_path, (opt->force_video || opt->force_audio) ? "r+b" : "rb");
        free(index_file_path);
    }
    if (index) {
        if (parse_index(lwhp, vdhp, vohp, adhp, aohp, opt, index) == 0) {
            /* Opening and parsing the index file succeeded. */
            fclose(index);
            lwhp->threads = opt->threads;
            return 0;
        }
        fclose(index);
    }
    /* Open file. */
    if (!lwhp->file_path) {
        lwhp->file_path = (char*)lw_malloc_zero(file_path_length + 1);
        if (!lwhp->file_path)
            goto fail;
        memcpy(lwhp->file_path, opt->file_path, file_path_length);
        if (has_lwi_ext)
            lwhp->file_path[file_path_length - 4] = '\0';
    }
    AVFormatContext* format_ctx = NULL;
    if (lavf_open_file(&format_ctx, lwhp->file_path, lhp)) {
        if (format_ctx)
            lavf_close_file(&format_ctx);
        goto fail;
    }
    lwhp->threads = opt->threads;
    vdhp->stream_index = -1;
    adhp->stream_index = opt->force_audio_index;
    /* Create the index file. */
    int err = create_index(lwhp, vdhp, vohp, adhp, aohp, format_ctx, opt, indicator, php);
    /* Close file.
     * By opening file for video and audio separately, indecent work about frame reading can be avoidable. */
    lavf_close_file(&format_ctx);
    vdhp->ctx = NULL;
    adhp->ctx = NULL;
    return err;
fail:
    if (lwhp->file_path)
        lw_freep(&lwhp->file_path);
    return -1;
}

int lwlibav_import_av_index_entry(lwlibav_decode_handler_t* dhp)
{
    if (dhp->index_entries) {
        AVStream* stream = dhp->format->streams[dhp->stream_index];
        for (int i = 0; i < dhp->index_entries_count; i++) {
            av_add_index_entry(stream, dhp->index_entries[i].pos, dhp->index_entries[i].timestamp, dhp->index_entries[i].size,
                dhp->index_entries[i].min_distance, dhp->index_entries[i].flags);
        }
        dhp->index_entries = NULL;
        dhp->index_entries_count = 0;
    }
    return 0;
}
