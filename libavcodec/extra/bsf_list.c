static const FFBitStreamFilter * const bitstream_filters[] = {
&ff_aac_adtstoasc_bsf,
&ff_apv_metadata_bsf,
&ff_av1_frame_merge_bsf,
&ff_av1_frame_split_bsf,
&ff_av1_metadata_bsf,
&ff_chomp_bsf,
&ff_dump_extradata_bsf,
&ff_dca_core_bsf,
&ff_dovi_rpu_bsf,
&ff_dts2pts_bsf,
&ff_dv_error_marker_bsf,
&ff_eac3_core_bsf,
&ff_evc_frame_merge_bsf,
&ff_extract_extradata_bsf,
&ff_filter_units_bsf,
&ff_h264_metadata_bsf,
&ff_h264_mp4toannexb_bsf,
&ff_h264_redundant_pps_bsf,
&ff_hapqa_extract_bsf,
&ff_hevc_metadata_bsf,
&ff_hevc_mp4toannexb_bsf,
&ff_imx_dump_header_bsf,
&ff_media100_to_mjpegb_bsf,
&ff_mjpeg2jpeg_bsf,
&ff_mjpega_dump_header_bsf,
&ff_mpeg2_metadata_bsf,
&ff_mpeg4_unpack_bframes_bsf,
&ff_mov2textsub_bsf,
&ff_noise_bsf,
&ff_null_bsf,
&ff_opus_metadata_bsf,
&ff_pcm_rechunk_bsf,
&ff_pgs_frame_merge_bsf,
&ff_prores_metadata_bsf,
&ff_remove_extradata_bsf,
&ff_setts_bsf,
&ff_showinfo_bsf,
&ff_text2movsub_bsf,
&ff_trace_headers_bsf,
&ff_truehd_core_bsf,
&ff_vp9_metadata_bsf,
&ff_vp9_raw_reorder_bsf,
&ff_vp9_superframe_bsf,
&ff_vp9_superframe_split_bsf,
&ff_vvc_metadata_bsf,
&ff_vvc_mp4toannexb_bsf,
    NULL };
