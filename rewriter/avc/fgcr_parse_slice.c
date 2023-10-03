/* 	Copyright (c) [2020]-[2023] Ittiam Systems Pvt. Ltd.
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted (subject to the limitations in the
   disclaimer below) provided that the following conditions are met:
   �	Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   �	Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   �	None of the names of Ittiam Systems Pvt. Ltd., its affiliates,
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
   standard by Ittiam Systems Pvt. Ltd. (�Ittiam�).
   Additional patent licenses may be required for this Software,
   including, but not limited to, a license from MPEG LA�s AVC/H.264
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
 * \file ih264d_parse_slice.c
 *
 * \brief
 *    Contains routines that decodes a slice NAL unit
 *
 **************************************************************************
 */
#include <string.h>
#include <assert.h>
#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"
#include "fgcr_structs.h"
#include "fgcr_debug.h"
#include "fgcr_bitstrm.h"
#include "fgcr_parse_cavlc.h"
#include "fgcr_utils.h"
#include "fgcr_defs.h"
#include "fgcr_error_handler.h"
#include "fgcr_defs.h"
#include "fgcr_parse_slice.h"

#include "fgcr_defs.h"

#include "fgcr_sei.h"
#include "fgcr.h"
#define RET_LAST_SKIP  0x80000000

WORD32 ih264d_detect_first_mb_slice(UWORD8 u1_is_idr_slice,
    UWORD8 u1_nal_ref_idc,
    dec_struct_t *ps_dec /* Decoder parameters */
       )
{
    dec_bit_stream_t * ps_bitstrm = ps_dec->ps_bitstrm;
    UWORD16 u2_first_mb_in_slice;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;

    /* read FirstMbInSlice  and slice type*/
    u2_first_mb_in_slice = ih264d_uev(pu4_bitstrm_ofst,
        pu4_bitstrm_buf);
    ps_dec->u2_first_mb_in_slice = u2_first_mb_in_slice;

    return 0;
}

WORD32 ihevcd_detect_first_slice_segment_in_pic(
	dec_struct_t *ps_dec /* Decoder parameters */
)
{
	dec_bit_stream_t * ps_bitstrm = ps_dec->ps_bitstrm;
	UWORD8 u1_first_slice_segment_in_pic_flag;
	UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
	UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;

	/* read FirstSliceSegmentInPicFlag*/
	u1_first_slice_segment_in_pic_flag = ih264d_get_bit_h264(ps_bitstrm);

	ps_dec->u1_first_slice_segment_in_pic_flag = u1_first_slice_segment_in_pic_flag;

	return 0;
}
