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
/*!
 **************************************************************************
 * \file ih264d_parse_headers.c
 *
 * \brief
 *    Contains High level syntax[above slice] parsing routines
 *
 **************************************************************************
 */
#include "fgcr_typedefs.h"
#include "fgcr_macros.h"
#include "fgcr_platform_macros.h"
#include "fgcr_bitstrm.h"
#include "fgcr_structs.h"
#include "fgcr_parse_cavlc.h"
#include "fgcr_defs.h"
#include "fgcr_parse_slice.h"
#include "fgcr_utils.h"
#include "fgcr_nal.h"

#include "fgcr_debug.h"
#include "fgcr_error_handler.h"
#include "fgcr_sei.h"
#include "fgcr_defs.h"
#include "fgcr.h"

#include <string.h>
#include <stddef.h>
#include <assert.h>

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_slice_partition                             */
/*                                                                           */
/*  Description   : This function is intended to parse and decode slice part */
/*                  itions. Currently it's not implemented. Decoder will     */
/*                  print a message, skips this NAL and continues            */
/*  Inputs        : ps_dec    Decoder parameters                             */
/*                  ps_bitstrm    Bitstream                                  */
/*  Globals       : None                                                     */
/*  Processing    : This functionality needs to be implemented               */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/

WORD32 ih264d_parse_slice_partition(dec_struct_t * ps_dec,
                                    dec_bit_stream_t * ps_bitstrm)
{
    H264_DEC_DEBUG_PRINT("\nSlice partition not supported");
    UNUSED(ps_dec);
    UNUSED(ps_bitstrm);
    return (0);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_filler_data                                 */
/*                                                                           */
/*  Description   : This function is intended to parse and decode filler     */
/*                  data NAL. Currently it's not implemented. Decoder will   */
/*                  print a message, skips this NAL and continues            */
/*  Inputs        : ps_dec    Decoder parameters                             */
/*                  ps_bitstrm    Bitstream                                  */
/*  Globals       : None                                                     */
/*  Processing    : This functionality needs to be implemented               */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/
WORD32 ih264d_parse_filler_data(dec_struct_t * ps_dec,
                                dec_bit_stream_t * ps_bitstrm)
{
    UNUSED(ps_dec);
    UNUSED(ps_bitstrm);
    return (0);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_parse_end_of_stream                               */
/*                                                                           */
/*  Description   : This function is intended to parse and decode end of     */
/*                  sequence. Currently it's not implemented. Decoder will   */
/*                  print a message, skips this NAL and continues            */
/*  Inputs        : ps_dec    Decoder parameters                             */
/*  Globals       : None                                                     */
/*  Processing    : This functionality needs to be implemented               */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/
void ih264d_parse_end_of_stream(dec_struct_t * ps_dec)
{
    UNUSED(ps_dec);
    return;
}

/*!
 **************************************************************************
 * \if Function name : AcessUnitDelimiterRbsp \endif
 *
 * \brief
 *    Decodes AcessUnitDelimiterRbsp.
 *
 * \param ps_bitstrm   : Pointer to bit ps_bitstrm containing the NAL unit
 *
 * \return
 *    0 on Success and error code otherwise
 **************************************************************************
 */

WORD32 ih264d_access_unit_delimiter_rbsp(dec_struct_t * ps_dec)
{
    UWORD8 u1_primary_pic_type;
    u1_primary_pic_type = ih264d_get_bits_h264(ps_dec->ps_bitstrm, 3);
    switch(u1_primary_pic_type)
    {
        case I_PIC:
        case SI_PIC:
        case ISI_PIC:
            break;
        default:
            break;
    }
    return (0);
}
/*!
 **************************************************************************
 * \if Function name : ih264d_parse_nal_unit_for_rewriter \endif
 *
 * \brief
 *    Decodes NAL unit
 *
 * \return
 *    0 on Success and error code otherwise
 **************************************************************************
 */

WORD32 ih264d_parse_nal_unit_for_rewriter(iv_obj_t *dec_hdl,
    fgcr_video_rewrite_op_t *ps_dec_op,
    UWORD8 *pu1_buf,
    UWORD8 **pu1_upd_buf,
    UWORD32 u4_length)
{
    dec_bit_stream_t *ps_bitstrm;
    static WORD32 i4_bits_left_in_cw = WORD_SIZE;
    dec_struct_t *ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;
    UWORD8 u1_first_byte, u1_nal_ref_idc;
    UWORD8 u1_nal_unit_type;
    WORD32 i_status = OK;
    ps_bitstrm = ps_dec->ps_bitstrm;
    UWORD8 *ps_upd_bitstrm = (UWORD8 *)ps_dec_op->pv_stream_out_buffer;
    if (pu1_buf)
    {
        if (u4_length)
        {
            ih264d_process_nal_unit(ps_dec->ps_bitstrm, pu1_buf,
                u4_length);
            u1_first_byte = ih264d_get_bits_h264(ps_bitstrm, 8);

            if (NAL_FORBIDDEN_BIT(u1_first_byte))
            {
                H264_DEC_DEBUG_PRINT("\nForbidden bit set in Nal Unit, Let's try\n");
            }
            u1_nal_unit_type = NAL_UNIT_TYPE(u1_first_byte);
            // if any other nal unit other than slice nal is encountered in between a
            // frame break out of loop without consuming header
            if ((ps_dec->u2_total_mbs_coded < (ps_dec->u2_total_mbs_coded)) &&
				(u1_nal_unit_type > IDR_SLICE_NAL))
			{
                return ERROR_INCOMPLETE_FRAME;
            }
            ps_dec->u1_nal_unit_type = u1_nal_unit_type;
            u1_nal_ref_idc = (UWORD8)(NAL_REF_IDC(u1_first_byte));
            //Skip all NALUs if SPS and PPS are not decoded
            {
                switch (u1_nal_unit_type)
                {
                case SEI_NAL:
                    break;
                case IDR_SLICE_NAL:
                case SLICE_NAL:
                    break;
                default:

                    *pu1_upd_buf += u4_length;
                    ps_dec_op->u4_num_bytes_generated += u4_length;
                    i4_bits_left_in_cw -= (u4_length << 3);
                    while (i4_bits_left_in_cw <= 0)
                    {
                        i4_bits_left_in_cw += WORD_SIZE;
                    }
                    if (i4_bits_left_in_cw > 32)
                    {
                        //Do nothing
                    }
                    break;
                }
            }
            //else
            {
                switch (u1_nal_unit_type)
                {
                case SLICE_DATA_PARTITION_A_NAL:
                case SLICE_DATA_PARTITION_B_NAL:
                case SLICE_DATA_PARTITION_C_NAL:
                    if (!ps_dec->i4_decode_header)
                        ih264d_parse_slice_partition(ps_dec, ps_bitstrm);

                    break;

                case IDR_SLICE_NAL:
                case SLICE_NAL:

                    /* ! */
                    if (!ps_dec->i4_decode_header)
                    {
                        if (ps_dec->i4_header_decoded == 3)
                        {
                            /* ! */
                            ps_dec->u4_slice_start_code_found = 1;

                            ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);

                            i_status = ih264d_detect_first_mb_slice(
                                (UWORD8)(u1_nal_unit_type
                                    == IDR_SLICE_NAL),
                                u1_nal_ref_idc, ps_dec);
                            if (ps_dec->u4_first_slice_in_pic != 0)
                            {
                                /*  if the first slice header was not valid set to 1 */
                                ps_dec->u4_first_slice_in_pic = 1;
                            }

                            if (i_status != OK)
                            {
                                return i_status;
                            }
                            if ((ps_dec->u2_first_mb_in_slice == 0) && (ps_dec->u4_fgs_enable_rewriter))
                            {
                                ps_dec->frm_counts++;
                                if (ps_dec->frm_counts == ps_dec->frm_SEI_counts)
                                {
                                    *pu1_upd_buf += u4_length;
                                    ps_dec_op->u4_num_bytes_generated += u4_length;
                                    i4_bits_left_in_cw -= (u4_length << 3);
                                    while (i4_bits_left_in_cw <= 0)
                                    {
                                        i4_bits_left_in_cw += WORD_SIZE;
                                    }
                                }
                                //Either SEI or FGS is not present
                                else if ((ps_dec->frm_counts - 1) == ps_dec->frm_SEI_counts)
                                {
                                    UWORD8 *temp_holder = ps_dec->temp_mem_holder;
                                    memcpy(temp_holder, *pu1_upd_buf - 4, u4_length + 4);
                                    bitstrm_t ps_bitstream;
                                    ps_bitstream.pu1_strm_buffer = *pu1_upd_buf;
                                    ps_bitstream.u4_strm_buf_offset = 0;
                                    ps_bitstream.u4_cur_word = 0;
                                    ps_bitstream.i4_bits_left_in_cw = WORD_SIZE;
                                    ps_bitstream.i4_zero_bytes_run = 0;
                                    ps_bitstream.u4_max_strm_size = 10000;
                                    i_status = i264_generate_sei_message(&ps_bitstream,
										ps_dec->s_fgs_rewrite_prms);
                                    if (i_status != OK)
                                    {
                                        return i_status;
                                    }
                                    *pu1_upd_buf += ps_bitstream.u4_strm_buf_offset;
                                    ps_dec->frm_SEI_counts++;
                                    memcpy(*pu1_upd_buf, temp_holder, u4_length + 4);
                                    *pu1_upd_buf += u4_length + 4;
                                    ps_dec_op->u4_num_bytes_generated +=
										u4_length + 4 + ps_bitstream.u4_strm_buf_offset;
                                }
                                else
                                {
                                    return NOT_OK;
                                }
                            }
                            else
                            {
                                ps_dec_op->u4_num_bytes_generated += u4_length;
                                *pu1_upd_buf += u4_length;
                            }
                            if (ps_dec->u2_first_mb_in_slice == 0)
                            {
                                ps_dec->is_fgs_rewrite_succ = 1;
                            }
                        }
                        else
                        {
                            H264_DEC_DEBUG_PRINT(
                                "\nSlice NAL Supplied but no header has been supplied\n");
                            *pu1_upd_buf += u4_length;
                            ps_dec_op->u4_num_bytes_generated += u4_length;
                        }
                    }
                    else
                    {
                        *pu1_upd_buf += u4_length;
                        ps_dec_op->u4_num_bytes_generated += u4_length;
                    }
                    break;

                case SEI_NAL:
                    if (!ps_dec->i4_decode_header)
                    {
                        ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);
                        ps_dec->ps_u1_upd_buf = *pu1_upd_buf;
                        i_status = ih264d_parse_sei_message(ps_dec, ps_bitstrm);
                        if (i_status != OK)
                            return i_status;

                        if (ps_dec->u4_num_bytes_updated != 0)
                        {
                            *pu1_upd_buf += ps_dec->u4_num_bytes_updated;
                            ps_dec_op->u4_num_bytes_generated += ps_dec->u4_num_bytes_updated;
                        }
                        else
                        {
                            *pu1_upd_buf += u4_length;
                            ps_dec_op->u4_num_bytes_generated += u4_length;
                        }
                    }
                    else
                    {
                        *pu1_upd_buf += u4_length;
                        ps_dec_op->u4_num_bytes_generated += u4_length;
                    }
                    break;

                case SEQ_PARAM_NAL:
                        ps_dec->i4_header_decoded |= 0x1;
                    break;

                case PIC_PARAM_NAL:
                        ps_dec->i4_header_decoded |= 0x2;
                    break;
                case ACCESS_UNIT_DELIMITER_RBSP:
                    if (!ps_dec->i4_decode_header)
                    {
                        ih264d_access_unit_delimiter_rbsp(ps_dec);
                    }
                    break;
                    //Let us ignore the END_OF_SEQ_RBSP NAL and decode even after this NAL
                case END_OF_STREAM_RBSP:
                    if (!ps_dec->i4_decode_header)
                    {
                        ih264d_parse_end_of_stream(ps_dec);
                    }
                    break;
                case FILLER_DATA_NAL:
                    if (!ps_dec->i4_decode_header)
                    {
                        ih264d_parse_filler_data(ps_dec, ps_bitstrm);
                    }
                    break;
                default:
                    H264_DEC_DEBUG_PRINT("\nUnknown NAL type %d\n", u1_nal_unit_type);
                    break;
                }
            }
        }
    }

    return i_status;
}
