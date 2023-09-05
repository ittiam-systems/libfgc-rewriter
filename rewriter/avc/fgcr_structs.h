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
#ifndef _FGCR_STRUCTS_H_
#define _FGCR_STRUCTS_H_

#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"
#include "fgcr.h"

#include "fgcr_defs.h"
#include "fgcr_bitstrm.h"
#include "fgcr_debug.h"

#include "fgcr_sei.h"

typedef struct _DecStruct dec_struct_t;

struct _DecStruct;

#define ACCEPT_ALL_PICS   (0x00)
#define REJECT_CUR_PIC    (0x01)

#define PIC_TYPE_UNKNOWN  (0xFF)
#define PIC_TYPE_I        (0x00)
#define SYNC_FRM_DEFAULT  (0xFFFFFFFF)
#define INIT_FRAME        (0xFFFFFF)

typedef struct _DecStruct
{
    /* Add below all other static memory allocations and pointers to items
     that are dynamically allocated once per session */
    dec_bit_stream_t *ps_bitstrm;
    dec_bit_stream_t *ps_upd_bitstrm;
    //Add new buffer?
    UWORD16 u2_first_mb_in_slice; /** Address of first MB in slice*/
	UWORD8 u1_first_slice_segment_in_pic_flag;

    /* These things need to be updated at each MbLevel */
    UWORD8 u1_nal_unit_type;

	UWORD8 u1_nuh_temporal_id_plus1;

    /* slice Header Simplification */
    WORD32 i4_error_code;

    /* Variables added to handle field pics */

    WORD32 i4_decode_header;
    WORD32 i4_header_decoded;

    sei_fgc_params_t s_fgs_prms;

    struct _sei *ps_sei;

    UWORD8 u1_init_dec_flag;

    UWORD16 u2_total_num_of_mbs; /** Total number of macroblocks in a coded picture */
    /* variable for finding the no.of mbs decoded in the current picture */
    UWORD16 u2_total_mbs_coded;

    WORD32 e_dec_status;

    UWORD8 u1_flushfrm;

	UWORD32 u4_max_wd;
	UWORD32 u4_max_ht;
    UWORD8 u1_pic_decode_done;
    WORD32 init_done;

    UWORD8 *pu1_bits_buf;

    UWORD32 u4_bits_buf_size;

    UWORD32 u4_slice_start_code_found;

    /* 2 first slice not parsed , 1 :first slice parsed , 0 :first valid slice header parsed*/
    UWORD32 u4_first_slice_in_pic;

    void *(*pf_aligned_alloc)(void *pv_mem_ctxt, WORD32 alignment, WORD32 size);
    void (*pf_aligned_free)(void *pv_mem_ctxt, void *pv_buf);
    void *pv_mem_ctxt;

    WORD32 i4_disp_poc;
    UWORD32 u4_fgs_enable_rewriter;
    UWORD32 u4_fgs_overide_fgc_cancel_flag;
    UWORD32 u4_fgs_overide_log2_scale_factor;
    UWORD32 u4_fgs_overide_dep_comp_model_values;
    fgcr_set_fgc_params_t  *s_fgs_rewrite_prms;
    UWORD8 *temp_mem_holder;
    UWORD8 *ps_u1_upd_buf;
    UWORD32 u4_num_bytes_updated;
    WORD32 frm_counts;
    WORD32 frm_SEI_counts;
	WORD32 FGC_before_IDR_CRA_present;
    WORD8  is_fgs_rewrite_succ;
	UWORD8 codec;
} dec_struct_t;

#endif /* _FGCR_STRUCTS_H */
