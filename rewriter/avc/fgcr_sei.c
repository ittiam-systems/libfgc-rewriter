/* 	Copyright (c) [2020]-[2023] Ittiam Systems Pvt. Ltd.
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted (subject to the limitations in the
   disclaimer below) provided that the following conditions are met:
   •	Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   •	Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   •	None of the names of Ittiam Systems Pvt. Ltd., its affiliates,
   investors, business partners, nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
   BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   This Software is an implementation of the AVC/H.264
   standard by Ittiam Systems Pvt. Ltd. (“Ittiam”).
   Additional patent licenses may be required for this Software,
   including, but not limited to, a license from MPEG LA’s AVC/H.264
   licensing program (see https://www.mpegla.com/programs/avc-h-264/).

   NOTWITHSTANDING ANYTHING TO THE CONTRARY, THIS DOES NOT GRANT ANY
   EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS OF ANY AFFILIATE
   (TO THE EXTENT NOT IN THE LEGAL ENTITY), INVESTOR, OR OTHER
   BUSINESS PARTNER OF ITTIAM. You may only use this software or
   modifications thereto for purposes that are authorized by
   appropriate patent licenses. You should seek legal advice based
   upon your implementation details.

---------------------------------------------------------------
*/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : ih264d_sei.c                                         */
/*                                                                           */
/*  Description       : This file contains routines to parse SEI NAL's       */
/*                                                                           */
/*****************************************************************************/

#include <assert.h>

#include "fgcr_typedefs.h"
#include "fgcr_macros.h"
#include "fgcr_platform_macros.h"
#include "fgcr_bitstrm.h"
#include "fgcr_structs.h"
#include "fgcr_error_handler.h"
#include "fgcr_parse_cavlc.h"
#include "fgcr_defs.h"
#include "fgcr_utils.h"

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_fgc_params                                  */
/*                                                                           */
/*  Description   : This function parses SEI message film grain charcaristics*/
/*  Inputs        : ps_bitstrm    Bitstream                                  */
/*                  ps_dec        Poniter decoder context                    */
/*                  ui4_payload_size pay load i4_size                        */
/*  Globals       : None                                                     */
/*  Processing    :                                                          */
/*  Outputs       : None                                                     */
/*  Return        : 0 for successfull parsing, else -1                       */
/*                                                                           */
/*****************************************************************************/
WORD32 ih264d_parse_fgc_params(dec_bit_stream_t *ps_bitstrm,
    dec_struct_t *ps_dec,
    UWORD32 ui4_payload_size)
{
    sei *ps_sei = ps_dec->ps_sei;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    WORD32 i4_luma_bitdepth, i4_chroma_bitdepth;
    UWORD32 c;
    UWORD32 i;
    UWORD32 j;

    UWORD8 codec = ps_dec->codec;

    ps_sei->s_fgc_params.u4_fgc_sei_present_flag = 0;

    if ((ps_dec == NULL) || (ps_sei == NULL))
    {
        return 0;
    }

    // hardcoding to 8 bit. The dependency will be removed.
    i4_luma_bitdepth = 8;
    i4_chroma_bitdepth = 8;

    ps_sei->s_fgc_params.u1_film_grain_characteristics_cancel_flag =
        (UWORD8)ih264d_get_bit_h264(ps_bitstrm);

    if (0 == ps_sei->s_fgc_params.u1_film_grain_characteristics_cancel_flag)
    {
        ps_sei->s_fgc_params.u1_film_grain_model_id = (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 2);
        if (ps_sei->s_fgc_params.u1_film_grain_model_id > 1)
        {
            return 0;
        }

        ps_sei->s_fgc_params.u1_separate_colour_description_present_flag =
            (UWORD8)ih264d_get_bit_h264(ps_bitstrm);

        if (ps_sei->s_fgc_params.u1_separate_colour_description_present_flag)
        {
            ps_sei->s_fgc_params.u1_film_grain_bit_depth_luma_minus8 =
                (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 3);

            i4_luma_bitdepth = ps_sei->s_fgc_params.u1_film_grain_bit_depth_luma_minus8 + 8;

            ps_sei->s_fgc_params.u1_film_grain_bit_depth_chroma_minus8 =
                (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 3);

            i4_chroma_bitdepth = ps_sei->s_fgc_params.u1_film_grain_bit_depth_chroma_minus8 + 8;

            ps_sei->s_fgc_params.u1_film_grain_full_range_flag =
                (UWORD8)ih264d_get_bit_h264(ps_bitstrm);

            ps_sei->s_fgc_params.u1_film_grain_colour_primaries =
                (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);

            ps_sei->s_fgc_params.u1_film_grain_transfer_characteristics =
                (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);

            ps_sei->s_fgc_params.u1_film_grain_matrix_coefficients =
                (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);
        }

        ps_sei->s_fgc_params.u1_blending_mode_id =
            (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 2);

        if (ps_sei->s_fgc_params.u1_blending_mode_id > 1)
        {
            return 0;
        }

        ps_sei->s_fgc_params.u1_log2_scale_factor =
            (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 4);

        for (c = 0; c < SEI_FGC_NUM_COLOUR_COMPONENTS; c++)
        {
            ps_sei->s_fgc_params.au1_comp_model_present_flag[c] =
                (UWORD8)ih264d_get_bit_h264(ps_bitstrm);
        }

        for (c = 0; c < SEI_FGC_NUM_COLOUR_COMPONENTS; c++)
        {
            if (ps_sei->s_fgc_params.au1_comp_model_present_flag[c])
            {
                ps_sei->s_fgc_params.au1_num_intensity_intervals_minus1[c] =
                    (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);

                ps_sei->s_fgc_params.au1_num_model_values_minus1[c] =
                    (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 3);

                if (ps_sei->s_fgc_params.au1_num_model_values_minus1[c] >
                    (SEI_FGC_MAX_NUM_MODEL_VALUES - 1))
                {
                    return 0;
                }

                for (i = 0; i <= ps_sei->s_fgc_params.au1_num_intensity_intervals_minus1[c]; i++)
                {
                    ps_sei->s_fgc_params.au1_intensity_interval_lower_bound[c][i] =
                        (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);

                    ps_sei->s_fgc_params.au1_intensity_interval_upper_bound[c][i] =
                        (UWORD8)ih264d_get_bits_h264(ps_bitstrm, 8);

                    for (j = 0; j <= ps_sei->s_fgc_params.au1_num_model_values_minus1[c]; j++) {

                        ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] =
                            (WORD32)ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf);

                        if (0 == ps_sei->s_fgc_params.u1_film_grain_model_id)
                        {
                            if ((1 == j) || (2 == j))
                            {
                                if ((ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] < 0) ||
                                    (ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] > 16))
                                    return 0;
                            }
                            else if ((3 == j) || (4 == j))
                            {
                                if ((ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] < 0) ||
                                    (ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] >
                                        ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j - 2]))
                                {
                                    return 0;
                                }
                            }
                            else
                            {
                                WORD32 max_lim = (c == 0) ? (1 << i4_luma_bitdepth) - 1 :
                                    (1 << i4_chroma_bitdepth) - 1;

                                if ((ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] < 0) ||
                                    (ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] > max_lim))
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            WORD32 max_lim = (c == 0) ? (1 << (i4_luma_bitdepth - 1)) :
                                (1 << (i4_chroma_bitdepth - 1));

                            if ((ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] < -max_lim) ||
                                (ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j] >= max_lim))
                            {
                                return 0;
                            }
                        }
                    }
                }
            }
        }

        if (codec == AVC)
        {
            ps_sei->s_fgc_params.u4_film_grain_characteristics_repetition_period =
                (UWORD32)ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);

            if (ps_sei->s_fgc_params.u4_film_grain_characteristics_repetition_period < 0 ||
                ps_sei->s_fgc_params.u4_film_grain_characteristics_repetition_period > 16384)
            {
                return 0;
            }
        }
        else if (codec == HEVC)
        {
            ps_sei->s_fgc_params.u1_film_grain_characteristics_persistence_flag = (UWORD8)ih264d_get_bit_h264(ps_bitstrm);
        }

        ps_sei->s_fgc_params.u4_fgc_sei_present_flag = 1;
    }

    return (0);
}

WORD32 update_FGS_rewrite_str(dec_struct_t *ps_dec)
{
    fgcr_set_fgc_params_t *ps_fgs_prms = ps_dec->s_fgs_rewrite_prms;

    if (ps_dec->u4_fgs_overide_fgc_cancel_flag == 0)
    {
        ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_film_grain_characteristics_cancel_flag =
            ps_dec->s_fgs_prms.u1_film_grain_characteristics_cancel_flag;
    }

    if (ps_dec->u4_fgs_overide_log2_scale_factor == 0)
    {
        ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_log2_scale_factor =
            ps_dec->s_fgs_prms.u1_log2_scale_factor;
    }

    if (ps_dec->u4_fgs_overide_dep_comp_model_values == 0)
    {
        for (int c = 0; c < MAX_NUM_COMP; c++)
        {
            ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_comp_model_present_flag[c] =
                ps_dec->s_fgs_prms.au1_comp_model_present_flag[c];
        }

        for (int c = 0; c < MAX_NUM_COMP; c++)
        {
            if (ps_dec->s_fgs_prms.au1_comp_model_present_flag[c])
            {
                ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_num_intensity_intervals_minus1[c] =
                    ps_dec->s_fgs_prms.au1_num_intensity_intervals_minus1[c];
                ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_num_model_values_minus1[c] =
                    ps_dec->s_fgs_prms.au1_num_model_values_minus1[c];

                for (int i = 0; i <= ps_dec->s_fgs_prms.au1_num_intensity_intervals_minus1[c]; i++)
                {
                    ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_intensity_interval_lower_bound[c][i] =
                        ps_dec->s_fgs_prms.au1_intensity_interval_lower_bound[c][i];
                    ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u1_intensity_interval_upper_bound[c][i] =
                        ps_dec->s_fgs_prms.au1_intensity_interval_upper_bound[c][i];

                    for (int j = 0; j <= ps_dec->s_fgs_prms.au1_num_model_values_minus1[c]; j++)
                    {
                        ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]->u4_comp_model_value[c][i][j] =
                            ps_dec->s_fgs_prms.ai4_comp_model_value[c][i][j];
                    }
                }
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_sei_payload                                 */
/*                                                                           */
/*  Description   : This function parses SEI pay loads. Currently it's       */
/*                  implemented partially.                                   */
/*  Inputs        : ps_bitstrm    Bitstream                                  */
/*                  ui4_payload_type  SEI payload type                       */
/*                  ui4_payload_size  SEI payload i4_size                    */
/*  Globals       : None                                                     */
/*  Processing    : Parses SEI payloads units and stores the info            */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/

WORD32 ih264d_parse_sei_payload(dec_bit_stream_t *ps_bitstrm,
                                UWORD32 ui4_payload_type,
                                UWORD32 u4_payload_size,
                                dec_struct_t *ps_dec)
{
    WORD32 i4_status = 0;
    UWORD8 codec = ps_dec->codec;

    switch(ui4_payload_type)
    {
        case SEI_FILM_GRAIN_CHARACTERISTICS:
            //Update the new buffer added in ps_dec buffer
            ps_dec->u1_fgc_present_before_idr_cra_flag = 1;
            ih264d_parse_fgc_params(ps_bitstrm, ps_dec,u4_payload_size);
            ps_dec->ps_sei->u1_is_valid = 1;
            if (ps_dec->u4_fgs_enable_rewriter)
            {
                bitstrm_t ps_bitstream;
                WORD32 i_status;
                ps_bitstream.pu1_strm_buffer = ps_dec->ps_u1_upd_buf;
                ps_bitstream.u4_strm_buf_offset = 0;
                ps_bitstream.u4_cur_word = 0;
                ps_bitstream.i4_bits_left_in_cw = WORD_SIZE;
                ps_bitstream.i4_zero_bytes_run = 0;
                ps_bitstream.u4_max_strm_size = 10000;
                i_status = update_FGS_rewrite_str(ps_dec);
                if (codec == AVC)
                {
                    i_status = i264_generate_sei_message(&ps_bitstream, ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc]);
                }
                else if (codec == HEVC)
                {
                    i_status = i265_generate_sei_message(&ps_bitstream, ps_dec->s_fgs_rewrite_prms[ps_dec->frm_SEI_counts%ps_dec->u1_num_fgc], (ps_dec->u1_nuh_temporal_id_plus1 - 1));
                }
                if (i_status != OK)
                {
                    return i_status;
                }
                ps_dec->u4_num_bytes_updated = ps_bitstream.u4_strm_buf_offset;
                ps_dec->frm_SEI_counts++;
            }
            else
            {
                ps_dec->u1_fgc_present_before_idr_cra_flag = 0;
                ps_dec->u4_num_bytes_updated = 0;
            }

            break;
            //parsing code for FGS sei
        default:
            ih264d_flush_bits_h264(ps_bitstrm, (u4_payload_size << 3));
            break;
    }
    if (ui4_payload_type != SEI_FILM_GRAIN_CHARACTERISTICS)
    {
        ps_dec->u4_num_bytes_updated = 0;
    }
    return (i4_status);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_sei_message                                 */
/*                                                                           */
/*  Description   : This function is parses and decode SEI. Currently it's   */
/*                  not implemented fully.                                   */
/*  Inputs        : ps_dec    Decoder parameters                             */
/*                  ps_bitstrm    Bitstream                                  */
/*  Globals       : None                                                     */
/*  Processing    : Parses SEI NAL units and stores the info                 */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/

WORD32 ih264d_parse_sei_message(dec_struct_t *ps_dec,
                                dec_bit_stream_t *ps_bitstrm)
{
    UWORD32 ui4_payload_type, ui4_payload_size;
    UWORD32 u4_bits;
    WORD32 i4_status = 0;

    do
    {
        ui4_payload_type = 0;

        u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
        while(0xff == u4_bits)
        {
            u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
            ui4_payload_type += 255;
        }
        ui4_payload_type += u4_bits;

        ui4_payload_size = 0;
        u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
        while(0xff == u4_bits)
        {
            u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
            ui4_payload_size += 255;
        }
        ui4_payload_size += u4_bits;

        i4_status = ih264d_parse_sei_payload(ps_bitstrm, ui4_payload_type,
                                             ui4_payload_size, ps_dec);
        if(i4_status == -1)
        {
            i4_status = 0;
            break;
        }

        if(i4_status != OK)
            return i4_status;

        if(ih264d_check_byte_aligned(ps_bitstrm) == 0)
        {
            u4_bits = ih264d_get_bit_h264(ps_bitstrm);
            if(0 == u4_bits)
            {
                H264_DEC_DEBUG_PRINT("\nError in parsing SEI message");
            }
            while(0 == ih264d_check_byte_aligned(ps_bitstrm))
            {
                u4_bits = ih264d_get_bit_h264(ps_bitstrm);
                if(u4_bits)
                {
                    H264_DEC_DEBUG_PRINT("\nError in parsing SEI message");
                }
            }
        }
    }
    while(ps_bitstrm->u4_ofst < ps_bitstrm->u4_max_ofst);
    return (i4_status);
}

/**
******************************************************************************
*
* @brief Generate nal unit header in the stream as per section 7.4.1
*
* @par   Description
*  Inserts Nal unit header syntax as per section 7.4.1
*
* @param[inout]   ps_bitstrm
*  pointer to bitstream context (handle)
*
* @param[in]   nal_unit_type
*  nal type to be inserted
*
* @param[in]   nal_ref_idc
*  nal ref idc to be inserted
*
* @return      success or failure error code
*
******************************************************************************
*/

static WORD32 i264_generate_nal_unit_header(bitstrm_t *ps_bitstrm,
    WORD32 nal_unit_type,
    WORD32 nal_ref_idc)
{
    WORD32 return_status = IV_SUCCESS;

    /* sanity checks */
    ASSERT((nal_unit_type > 0) && (nal_unit_type < 32));

    /*1 forbidden zero bit + 2 nal_ref_idc + 5 nal_unit_type */
    PUT_BITS(ps_bitstrm,
        ((nal_ref_idc << 5) + nal_unit_type),
        (1 + 2 + 5),
        return_status,
        "nal_unit_header");

    return(return_status);
}

static WORD32 i265_generate_nal_unit_header(bitstrm_t *ps_bitstrm,
    WORD32 nal_unit_type,
    WORD32 nuh_temporal_id)
{
    WORD32 return_status = IV_SUCCESS;

    /* sanity checks */
    ASSERT((nal_unit_type >= 0) && (nal_unit_type < 64));
    ASSERT((nuh_temporal_id >= 0) && (nuh_temporal_id < 7));

    /* forbidden_zero_bit + nal_unit_type */
    PUT_BITS(ps_bitstrm,
        nal_unit_type,
        (1 + 6),
        return_status,
        "nal_unit_header");

    /* nuh_reserved_zero_6bits */
    PUT_BITS(ps_bitstrm,
        0,
        6,
        return_status,
        "nal_unit_header");

    /* nuh_temporal_id_plus1 */
    PUT_BITS(ps_bitstrm,
        (nuh_temporal_id + 1),
        3,
        return_status,
        "nal_unit_header");

    return(return_status);
}

WORD32 i264_generate_sei_message(bitstrm_t *ps_bitstrm, fgcr_set_fgc_params_t *ps_fgs_prms)
{
    WORD32 return_status = IV_SUCCESS;
    WORD32 i4_payload_size = 0;
    WORD32 c, i, j;

    {
        //Calculating payload size
        /** For film_grain_characteristics_cancel_flag */
        i4_payload_size++;

        if (!ps_fgs_prms->u1_film_grain_characteristics_cancel_flag)
        {
            /** film_grain_model_id*/
            i4_payload_size += 2;
            /** separate_colour_description_present_flag*/
            i4_payload_size += 1;
            if (ps_fgs_prms->u1_separate_colour_description_present_flag)
            {
                /**film_grain_bit_depth_luma_minus8*/
                i4_payload_size += 3;
                /** film_grain_bit_depth_chroma_minus8 */
                i4_payload_size += 3;
                /** film_grain_full_range_flag */
                i4_payload_size += 1;
                /** film_grain_colour_primaries */
                i4_payload_size += 8;
                /** film_grain_transfer_characteristics */
                i4_payload_size += 8;
                /** film_grain_matrix_coefficients */
                i4_payload_size += 8;
            }
            //fgs_blending_mode_id
            i4_payload_size += 2;
            //fgs_log2_scale_factor
            i4_payload_size += 4;
            for (c = 0; c < MAX_NUM_COMP; c++)
            {
                //fgs_comp_model_present_flag
                i4_payload_size += 1;
            }
            for (c = 0; c < MAX_NUM_COMP; c++)
            {
                if (ps_fgs_prms->u1_comp_model_present_flag[c])
                {
                    /** fgs_num_intensity_intervals_minus1 */
                    i4_payload_size += 8;
                    /** fgs_num_model_values_minus1 */
                    i4_payload_size += 3;

                    for (i = 0; i <= ps_fgs_prms->u1_num_intensity_intervals_minus1[c]; i++)
                    {
                        /** fgs_intensity_interval_lower_bound */
                        i4_payload_size += 8;
                        /** fgs_intensity_interval_upper_bound */
                        i4_payload_size += 8;

                        for (j = 0; j <= ps_fgs_prms->u1_num_model_values_minus1[c]; j++)
                        {
                            /** fgs_comp_model_value */
                            UWORD32 u4_range;
                            WORD32  i4_val = (WORD32)(ps_fgs_prms->u4_comp_model_value[c][i][j] << 1);
                            GETRANGE(u4_range, i4_val);
                            i4_payload_size += (((u4_range - 1) << 1) + 1);
                        }
                    }
                }
            }
            /** film_grain_characteristics_repetition_period */
            UWORD32 u4_range;
            GETRANGE(u4_range, ps_fgs_prms->u4_film_grain_characteristics_repetition_period);
            i4_payload_size += (((u4_range - 1) << 1) + 1);
        }
    }
    //Converting to bytes
    i4_payload_size = (i4_payload_size + 7) >> 3;

    return_status |= i264_generate_nal_unit_header(ps_bitstrm, I264_SEI_NAL, 0);

    /** Pass payload type & payload size*/
    PUT_BITS(ps_bitstrm,
        I264_SEI_FILM_GRAIN_CHARACTERISTICS_T,
        8,
        return_status,
        "payload_type");

    PUT_BITS(ps_bitstrm,
        i4_payload_size,
        8,
        return_status,
        "payload_size");

    PUT_BITS(ps_bitstrm,
        ps_fgs_prms->u1_film_grain_characteristics_cancel_flag,
        1,
        return_status,
        "SEI: film_grain_characteristics_cancel_flag");

    if (!ps_fgs_prms->u1_film_grain_characteristics_cancel_flag)
    {
        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_film_grain_model_id,
            2,
            return_status,
            "SEI: film_grain_model_id");

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_separate_colour_description_present_flag,
            1,
            return_status,
            "SEI: separate_colour_description_present_flag");

        if (ps_fgs_prms->u1_separate_colour_description_present_flag)
        {
            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_bit_depth_luma_minus8,
                3,
                return_status,
                "SEI: film_grain_bit_depth_luma_minus8");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_bit_depth_chroma_minus8,
                3,
                return_status,
                "SEI: film_grain_bit_depth_chroma_minus8");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_full_range_flag,
                1,
                return_status,
                "SEI: film_grain_full_range_flag");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_colour_primaries,
                8,
                return_status,
                "SEI: film_grain_colour_primaries");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_transfer_characteristics,
                8,
                return_status,
                "SEI: film_grain_transfer_characteristics");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_matrix_coefficients,
                8,
                return_status,
                "SEI: film_grain_matrix_coefficients");
        }

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_blending_mode_id,
            2,
            return_status,
            "SEI: fgs_blending_mode_id");

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_log2_scale_factor,
            4,
            return_status,
            "SEI: fgs_log2_scale_factor");

        for (c = 0; c < MAX_NUM_COMP; c++)
        {
            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_comp_model_present_flag[c],
                1,
                return_status,
                "SEI: fgs_comp_model_present_flag");
        }

        for (c = 0; c < MAX_NUM_COMP; c++)
        {
            if (ps_fgs_prms->u1_comp_model_present_flag[c])
            {
                PUT_BITS(ps_bitstrm,
                    ps_fgs_prms->u1_num_intensity_intervals_minus1[c],
                    8,
                    return_status,
                    "SEI: fgs_num_intensity_intervals_minus1");

                PUT_BITS(ps_bitstrm,
                    ps_fgs_prms->u1_num_model_values_minus1[c],
                    3,
                    return_status,
                    "SEI: fgs_num_model_values_minus1");

                for (i = 0; i <= ps_fgs_prms->u1_num_intensity_intervals_minus1[c]; i++)
                {
                    PUT_BITS(ps_bitstrm,
                        ps_fgs_prms->u1_intensity_interval_lower_bound[c][i],
                        8,
                        return_status,
                        "SEI: fgs_intensity_interval_lower_bound");

                    PUT_BITS(ps_bitstrm,
                        ps_fgs_prms->u1_intensity_interval_upper_bound[c][i],
                        8,
                        return_status,
                        "SEI: fgs_intensity_interval_upper_bound");

                    for (j = 0; j <= ps_fgs_prms->u1_num_model_values_minus1[c]; j++)
                    {
                        PUT_BITS_SEV(ps_bitstrm,
                            ps_fgs_prms->u4_comp_model_value[c][i][j],
                            return_status,
                            "SEI: fgs_comp_model_value");
                    }
                }
            }
        }

        PUT_BITS_UEV(ps_bitstrm,
            ps_fgs_prms->u4_film_grain_characteristics_repetition_period,
            return_status,
            "SEI: film_grain_characteristics_repetition_period");
    }

    return_status |= i264_byte_align(ps_bitstrm);
    return_status |= i264_put_rbsp_trailing_bits(ps_bitstrm);

    return return_status;
}

WORD32 i265_generate_sei_message(bitstrm_t *ps_bitstrm, fgcr_set_fgc_params_t *ps_fgs_prms, UWORD8 num_temporal_id)
{
    WORD32 return_status = IV_SUCCESS;
    WORD32 i4_payload_size = 0;
    WORD32 c, i, j;

    {
        //Calculating payload size
        i4_payload_size++;   //For film_grain_characteristics_cancel_flag

        if (!ps_fgs_prms->u1_film_grain_characteristics_cancel_flag)
        {
            /** film_grain_model_id*/
            i4_payload_size += 2;
            /** separate_colour_description_present_flag*/
            i4_payload_size += 1;
            if (ps_fgs_prms->u1_separate_colour_description_present_flag)
            {
                /**film_grain_bit_depth_luma_minus8*/
                i4_payload_size += 3;
                /** film_grain_bit_depth_chroma_minus8 */
                i4_payload_size += 3;
                /** film_grain_full_range_flag */
                i4_payload_size += 1;
                /** film_grain_colour_primaries */
                i4_payload_size += 8;
                /** film_grain_transfer_characteristics */
                i4_payload_size += 8;
                /** film_grain_matrix_coefficients */
                i4_payload_size += 8;
            }
            //fgs_blending_mode_id
            i4_payload_size += 2;
            //fgs_log2_scale_factor
            i4_payload_size += 4;
            for (c = 0; c < MAX_NUM_COMP; c++)
            {
                //fgs_comp_model_present_flag
                i4_payload_size += 1;
            }
            for (c = 0; c < MAX_NUM_COMP; c++)
            {
                if (ps_fgs_prms->u1_comp_model_present_flag[c])
                {
                    /** fgs_num_intensity_intervals_minus1 */
                    i4_payload_size += 8;
                    /** fgs_num_model_values_minus1 */
                    i4_payload_size += 3;

                    for (i = 0; i <= ps_fgs_prms->u1_num_intensity_intervals_minus1[c]; i++)
                    {
                        /** fgs_intensity_interval_lower_bound */
                        i4_payload_size += 8;
                        /** fgs_intensity_interval_upper_bound */
                        i4_payload_size += 8;

                        for (j = 0; j <= ps_fgs_prms->u1_num_model_values_minus1[c]; j++)
                        {
                            /** fgs_comp_model_value */
                            UWORD32 u4_range;
                            WORD32  i4_val = (WORD32)(ps_fgs_prms->u4_comp_model_value[c][i][j] << 1);
                            GETRANGE(u4_range, i4_val);
                            i4_payload_size += (((u4_range - 1) << 1) + 1);
                        }
                    }
                }
            }
            /** film_grain_characteristics_persistence_flag */
            i4_payload_size++;
        }
    }
    //Converting to bytes
    i4_payload_size = (i4_payload_size + 7) >> 3;

    return_status |= i265_generate_nal_unit_header(ps_bitstrm, NAL_PREFIX_SEI, num_temporal_id);

    /** Pass payload type & payload size*/
    PUT_BITS(ps_bitstrm,
        I264_SEI_FILM_GRAIN_CHARACTERISTICS_T,
        8,
        return_status,
        "payload_type");

    PUT_BITS(ps_bitstrm,
        i4_payload_size,
        8,
        return_status,
        "payload_size");

    PUT_BITS(ps_bitstrm,
        ps_fgs_prms->u1_film_grain_characteristics_cancel_flag,
        1,
        return_status,
        "SEI: film_grain_characteristics_cancel_flag");

    if (!ps_fgs_prms->u1_film_grain_characteristics_cancel_flag)
    {
        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_film_grain_model_id,
            2,
            return_status,
            "SEI: film_grain_model_id");

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_separate_colour_description_present_flag,
            1,
            return_status,
            "SEI: separate_colour_description_present_flag");

        if (ps_fgs_prms->u1_separate_colour_description_present_flag)
        {
            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_bit_depth_luma_minus8,
                3,
                return_status,
                "SEI: film_grain_bit_depth_luma_minus8");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_bit_depth_chroma_minus8,
                3,
                return_status,
                "SEI: film_grain_bit_depth_chroma_minus8");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_full_range_flag,
                1,
                return_status,
                "SEI: film_grain_full_range_flag");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_colour_primaries,
                8,
                return_status,
                "SEI: film_grain_colour_primaries");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_transfer_characteristics,
                8,
                return_status,
                "SEI: film_grain_transfer_characteristics");

            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_film_grain_matrix_coefficients,
                8,
                return_status,
                "SEI: film_grain_matrix_coefficients");
        }

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_blending_mode_id,
            2,
            return_status,
            "SEI: fgs_blending_mode_id");

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_log2_scale_factor,
            4,
            return_status,
            "SEI: fgs_log2_scale_factor");

        for (c = 0; c < MAX_NUM_COMP; c++)
        {
            PUT_BITS(ps_bitstrm,
                ps_fgs_prms->u1_comp_model_present_flag[c],
                1,
                return_status,
                "SEI: fgs_comp_model_present_flag");
        }

        for (c = 0; c < MAX_NUM_COMP; c++)
        {
            if (ps_fgs_prms->u1_comp_model_present_flag[c])
            {
                PUT_BITS(ps_bitstrm,
                    ps_fgs_prms->u1_num_intensity_intervals_minus1[c],
                    8,
                    return_status,
                    "SEI: fgs_num_intensity_intervals_minus1");

                PUT_BITS(ps_bitstrm,
                    ps_fgs_prms->u1_num_model_values_minus1[c],
                    3,
                    return_status,
                    "SEI: fgs_num_model_values_minus1");

                for (i = 0; i <= ps_fgs_prms->u1_num_intensity_intervals_minus1[c]; i++)
                {
                    PUT_BITS(ps_bitstrm,
                        ps_fgs_prms->u1_intensity_interval_lower_bound[c][i],
                        8,
                        return_status,
                        "SEI: fgs_intensity_interval_lower_bound");

                    PUT_BITS(ps_bitstrm,
                        ps_fgs_prms->u1_intensity_interval_upper_bound[c][i],
                        8,
                        return_status,
                        "SEI: fgs_intensity_interval_upper_bound");

                    for (j = 0; j <= ps_fgs_prms->u1_num_model_values_minus1[c]; j++)
                    {
                        PUT_BITS_SEV(ps_bitstrm,
                            ps_fgs_prms->u4_comp_model_value[c][i][j],
                            return_status,
                            "SEI: fgs_comp_model_value");
                    }
                }
            }
        }

        PUT_BITS(ps_bitstrm,
            ps_fgs_prms->u1_film_grain_characteristics_persistence_flag,
            1,
            return_status,
            "SEI: film_grain_characteristics_persistence_flag");
    }

    return_status |= i264_byte_align(ps_bitstrm);
    return_status |= i264_put_rbsp_trailing_bits(ps_bitstrm);

    return return_status;
}