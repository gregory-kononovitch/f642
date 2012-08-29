/*
 * file    : f642_util.cc
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f642_x264.hh"

void t508::f642::X264::setLogLevel(int logLevel) {
    this->logLevel = logLevel;
    param.i_log_level = logLevel;
}

void t508::f642::X264::dumpConfig() {
    dumpConfig(&param);
}

void t508::f642::X264::dumpConfig(x264_param_t *prm) {
    /* CPU flags */
    fprintf(stderr, "cpu = %d\n", prm->cpu);
    fprintf(stderr, "i_threads = %d\n", prm->i_threads);
    fprintf(stderr, "b_sliced_threads = %d\n", prm->b_sliced_threads);
    fprintf(stderr, "b_deterministic = %d\n", prm->b_deterministic);
    fprintf(stderr, "b_cpu_independent = %d\n", prm->b_cpu_independent);
    fprintf(stderr, "i_sync_lookahead = %d\n", prm->i_sync_lookahead);

    /* Video Properties */
    fprintf(stderr, "i_width = %d\n", prm->i_width);
    fprintf(stderr, "i_height = %d\n", prm->i_height);
    fprintf(stderr, "i_csp = %d\n", prm->i_csp);
    fprintf(stderr, "i_level_idc = %d\n", prm->i_level_idc);
    fprintf(stderr, "i_frame_total = %d\n", prm->i_frame_total);

    /* NAL HRD*/
    fprintf(stderr, "i_nal_hrd = %d\n", prm->i_nal_hrd);


    fprintf(stderr, "i_sar_height = %d\n", prm->vui.i_sar_height);
    fprintf(stderr, "i_sar_width = %d\n", prm->vui.i_sar_width);
    fprintf(stderr, "i_overscan = %d\n", prm->vui.i_overscan);
    fprintf(stderr, "i_vidformat = %d\n", prm->vui.i_vidformat);
    fprintf(stderr, "b_fullrange = %d\n", prm->vui.b_fullrange);
    fprintf(stderr, "i_colorprim = %d\n", prm->vui.i_colorprim);
    fprintf(stderr, "i_transfer = %d\n", prm->vui.i_transfer);
    fprintf(stderr, "i_colmatrix = %d\n", prm->vui.i_colmatrix);
    fprintf(stderr, "i_chroma_loc = %d\n", prm->vui.i_chroma_loc);

    /* Bitstream parameters */
    fprintf(stderr, "i_frame_reference = %d\n", prm->i_frame_reference);
    fprintf(stderr, "i_dpb_size = %d\n", prm->i_dpb_size);
    fprintf(stderr, "i_keyint_max = %d\n", prm->i_keyint_max);
    fprintf(stderr, "i_keyint_min = %d\n", prm->i_keyint_min);
    fprintf(stderr, "i_scenecut_threshold = %d\n", prm->i_scenecut_threshold);
    fprintf(stderr, "b_intra_refresh = %d\n", prm->b_intra_refresh);

    fprintf(stderr, "i_bframe = %d\n", prm->i_bframe);
    fprintf(stderr, "i_bframe_adaptive = %d\n", prm->i_bframe_adaptive);
    fprintf(stderr, "i_bframe_bias = %d\n", prm->i_bframe_bias);
    fprintf(stderr, "i_bframe_pyramid = %d\n", prm->i_bframe_pyramid);
    fprintf(stderr, "b_open_gop = %d\n", prm->b_open_gop);
    fprintf(stderr, "b_bluray_compat = %d\n", prm->b_bluray_compat);

    fprintf(stderr, "b_deblocking_filter = %d\n", prm->b_deblocking_filter);
    fprintf(stderr, "i_deblocking_filter_alphac0 = %d\n", prm->i_deblocking_filter_alphac0);
    fprintf(stderr, "i_deblocking_filter_beta = %d\n", prm->i_deblocking_filter_beta);

    fprintf(stderr, "b_cabac = %d\n", prm->b_cabac);
    fprintf(stderr, "i_cabac_init_idc = %d\n", prm->i_cabac_init_idc);

    fprintf(stderr, "b_interlaced = %d\n", prm->b_interlaced);
    fprintf(stderr, "b_constrained_intra = %d\n", prm->b_constrained_intra);

    fprintf(stderr, "i_cqm_preset = %d\n", prm->i_cqm_preset);


    /* Log */
    fprintf(stderr, "i_log_level = %d\n", prm->i_log_level);
    fprintf(stderr, "b_visualize = %d\n", prm->b_visualize);
    fprintf(stderr, "b_full_recon = %d\n", prm->b_full_recon);

    /* Encoder analyser parameters */
        fprintf(stderr, "intra = %u\n", prm->analyse.intra);
        fprintf(stderr, "inter = %u\n", prm->analyse.inter);

        fprintf(stderr, "b_transform_8x8 = %d\n", prm->analyse.b_transform_8x8);
        fprintf(stderr, "i_weighted_pred = %d\n", prm->analyse.i_weighted_pred);
        fprintf(stderr, "b_weighted_bipred = %d\n", prm->analyse.b_weighted_bipred);
        fprintf(stderr, "i_direct_mv_pred = %d\n", prm->analyse.i_direct_mv_pred);
        fprintf(stderr, "i_chroma_qp_offset = %d\n", prm->analyse.i_chroma_qp_offset);

        fprintf(stderr, "i_me_method = %d\n", prm->analyse.i_me_method);
        fprintf(stderr, "i_me_range = %d\n", prm->analyse.i_me_range);
        fprintf(stderr, "i_mv_range = %d\n", prm->analyse.i_mv_range);
        fprintf(stderr, "i_mv_range_thread = %d\n", prm->analyse.i_mv_range_thread);
        fprintf(stderr, "i_subpel_refine = %d\n", prm->analyse.i_subpel_refine);
        fprintf(stderr, "b_chroma_me = %d\n", prm->analyse.b_chroma_me);
        fprintf(stderr, "b_mixed_references = %d\n", prm->analyse.b_mixed_references);
        fprintf(stderr, "i_trellis = %d\n", prm->analyse.i_trellis);
        fprintf(stderr, "b_fast_pskip = %d\n", prm->analyse.b_fast_pskip);
        fprintf(stderr, "b_dct_decimate = %d\n", prm->analyse.b_dct_decimate);
        fprintf(stderr, "i_noise_reduction = %d\n", prm->analyse.i_noise_reduction);
        fprintf(stderr, "f_psy_rd = %f\n", prm->analyse.f_psy_rd);
        fprintf(stderr, "f_psy_trellis = %f\n", prm->analyse.f_psy_trellis);
        fprintf(stderr, "b_psy = %d\n", prm->analyse.b_psy);

        /* the deadzone size that will be used in luma quantization */
        fprintf(stderr, "i_luma_deadzone[2] = %d ; %d\n", prm->analyse.i_luma_deadzone[0], prm->analyse.i_luma_deadzone[1]);

        fprintf(stderr, "b_psnr = %d\n", prm->analyse.b_psnr);
        fprintf(stderr, "b_ssim = %d\n", prm->analyse.b_ssim);

    /* Rate control parameters */

        fprintf(stderr, "i_rc_method = %d\n", prm->rc.i_rc_method);

        fprintf(stderr, "i_qp_constant = %d\n", prm->rc.i_qp_constant);
        fprintf(stderr, "i_qp_min = %d\n", prm->rc.i_qp_min);
        fprintf(stderr, "i_qp_max = %d\n", prm->rc.i_qp_max);
        fprintf(stderr, "i_qp_step = %d\n", prm->rc.i_qp_step);

        fprintf(stderr, "i_bitrate = %d\n", prm->rc.i_bitrate);
        fprintf(stderr, "f_rf_constant = %f\n", prm->rc.f_rf_constant);
        fprintf(stderr, "f_rf_constant_max = %f\n", prm->rc.f_rf_constant_max);
        fprintf(stderr, "f_rate_tolerance = %f\n", prm->rc.f_rate_tolerance);
        fprintf(stderr, "i_vbv_max_bitrate = %d\n", prm->rc.i_vbv_max_bitrate);
        fprintf(stderr, "i_vbv_buffer_size = %d\n", prm->rc.i_vbv_buffer_size);
        fprintf(stderr, "f_vbv_buffer_init = %f\n", prm->rc.f_vbv_buffer_init);
        fprintf(stderr, "f_ip_factor = %f\n", prm->rc.f_ip_factor);
        fprintf(stderr, "f_pb_factor = %f\n", prm->rc.f_pb_factor);

        fprintf(stderr, "i_aq_mode = %d\n", prm->rc.i_aq_mode);
        fprintf(stderr, "f_aq_strength = %f\n", prm->rc.f_aq_strength);
        fprintf(stderr, "b_mb_tree = %d\n", prm->rc.b_mb_tree);
        fprintf(stderr, "i_lookahead = %d\n", prm->rc.i_lookahead);

        /* 2pass */
        fprintf(stderr, "b_stat_write = %d\n", prm->rc.b_stat_write);
        fprintf(stderr, "psz_stat_out = %s\n", prm->rc.psz_stat_out);
        fprintf(stderr, "b_stat_read = %d\n", prm->rc.b_stat_read);
        fprintf(stderr, "psz_stat_in = %s\n", prm->rc.psz_stat_in);

        /* 2pass params (same as ffmpeg ones) */
        fprintf(stderr, "f_qcompress = %f\n", prm->rc.f_qcompress);
        fprintf(stderr, "f_qblur = %f\n", prm->rc.f_qblur);
        fprintf(stderr, "f_complexity_blur = %f\n", prm->rc.f_complexity_blur);
        fprintf(stderr, "zones = %p\n", prm->rc.zones);
        fprintf(stderr, "i_zones = %d\n", prm->rc.i_zones);
        fprintf(stderr, "psz_zones = %s\n", prm->rc.psz_zones);


    /* Cropping Rectangle parameters: added to those implicitly defined by
       non-mod16 video resolutions. */
        fprintf(stderr, "i_left = %u\n", prm->crop_rect.i_left);
        fprintf(stderr, "i_top = %u\n", prm->crop_rect.i_top);
        fprintf(stderr, "i_right = %u\n", prm->crop_rect.i_right);
        fprintf(stderr, "i_bottom = %u\n", prm->crop_rect.i_bottom);

    /* frame packing arrangement flag */
    fprintf(stderr, "i_frame_packing = %d\n", prm->i_frame_packing);

    /* Muxing parameters */
    fprintf(stderr, "b_aud = %d\n", prm->b_aud);
    fprintf(stderr, "b_repeat_headers = %d\n", prm->b_repeat_headers);
    fprintf(stderr, "b_annexb = %d\n", prm->b_annexb);
    fprintf(stderr, "i_sps_id = %d\n", prm->i_sps_id);
    fprintf(stderr, "b_vfr_input = %d\n", prm->b_vfr_input);

    fprintf(stderr, "b_pulldown = %d\n", prm->b_pulldown);
    fprintf(stderr, "i_fps_num = %u\n", prm->i_fps_num);
    fprintf(stderr, "i_fps_den = %u\n", prm->i_fps_den);
    fprintf(stderr, "i_timebase_num = %u\n", prm->i_timebase_num);
    fprintf(stderr, "i_timebase_den = %u\n", prm->i_timebase_den);

    fprintf(stderr, "b_tff = %d\n", prm->b_tff);

    fprintf(stderr, "b_pic_struct = %d\n", prm->b_pic_struct);

    fprintf(stderr, "b_fake_interlaced = %d\n", prm->b_fake_interlaced);

    fprintf(stderr, "i_slice_max_size = %d\n", prm->i_slice_max_size);
    fprintf(stderr, "i_slice_max_mbs = %d\n", prm->i_slice_max_mbs);
    fprintf(stderr, "i_slice_count = %d\n", prm->i_slice_count);
}
