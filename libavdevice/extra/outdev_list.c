static const AVOutputFormat * const outdev_list[] = {
//&ff_alsa_muxer,
//&ff_audiotoolbox_muxer,
//&ff_caca_muxer,
//&ff_decklink_muxer,
//&ff_fbdev_muxer,
//&ff_opengl_muxer,
//&ff_oss_muxer,
//&ff_pulse_muxer,
#ifdef FFPLUS
&ff_sdl2_muxer,
#endif
//&ff_sndio_muxer,
//&ff_v4l2_muxer,
//&ff_xv_muxer,
    NULL };
