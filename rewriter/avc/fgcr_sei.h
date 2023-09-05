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
/*  File Name         : fgcr_sei.h                                           */
/*                                                                           */
/*  Description       : This file contains routines to parse SEI NAL's       */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*****************************************************************************/

#ifndef _FGCR_SEI_H_
#define _FGCR_SEI_H_

#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"
#include "fgcr_bitstrm.h"
#include "fgcr_structs.h"

#define SEI_FILM_GRAIN_CHARACTERISTICS 19
/* Declaration of dec_struct_t to avoid CCS compilation Error */
struct _DecStruct;
WORD32 ih264d_parse_sei_message(struct _DecStruct *ps_dec,
                                dec_bit_stream_t *ps_bitstrm);

/**
 * Structure to hold FGS SEI param Info
 */
typedef struct
{
	UWORD32 u4_fgc_sei_present_flag;
	UWORD32 u4_idr_pic_id;
	UWORD8 u1_film_grain_characteristics_cancel_flag;
	UWORD8 u1_film_grain_model_id;
	UWORD8 u1_separate_colour_description_present_flag;
	UWORD8 u1_film_grain_bit_depth_luma_minus8;
	UWORD8 u1_film_grain_bit_depth_chroma_minus8;
	UWORD8 u1_film_grain_full_range_flag;
	UWORD8 u1_film_grain_colour_primaries;
	UWORD8 u1_film_grain_transfer_characteristics;
	UWORD8 u1_film_grain_matrix_coefficients;
	UWORD8 u1_blending_mode_id;
	UWORD8 u1_log2_scale_factor;
	UWORD8 au1_comp_model_present_flag[SEI_FGC_NUM_COLOUR_COMPONENTS];
	UWORD8 au1_num_intensity_intervals_minus1[SEI_FGC_NUM_COLOUR_COMPONENTS];
	UWORD8 au1_num_model_values_minus1[SEI_FGC_NUM_COLOUR_COMPONENTS];
	UWORD8 au1_intensity_interval_lower_bound[SEI_FGC_NUM_COLOUR_COMPONENTS][SEI_FGC_MAX_NUM_INTENSITY_INTERVALS];
	UWORD8 au1_intensity_interval_upper_bound[SEI_FGC_NUM_COLOUR_COMPONENTS][SEI_FGC_MAX_NUM_INTENSITY_INTERVALS];
	WORD32 ai4_comp_model_value[SEI_FGC_NUM_COLOUR_COMPONENTS][SEI_FGC_MAX_NUM_INTENSITY_INTERVALS][SEI_FGC_MAX_NUM_MODEL_VALUES];
	UWORD32 u4_film_grain_characteristics_repetition_period;
	UWORD8 u1_film_grain_characteristics_persistence_flag;

}sei_fgc_params_t;

typedef struct
{
    UWORD8 u1_film_grain_characteristics_cancel_flag;
    /* To be 0 : frequency filtering model */
    UWORD8 u1_film_grain_model_id;
    /* To be 0 : Decoded samples and grain to be in same color space */
    UWORD8 u1_separate_colour_description_present_flag;
    UWORD8 u1_film_grain_bit_depth_luma_minus8;
    UWORD8 u1_film_grain_bit_depth_chroma_minus8;
    UWORD8 u1_film_grain_full_range_flag;
    UWORD8 u1_film_grain_colour_primaries;
    UWORD8 u1_film_grain_transfer_characteristics;
    UWORD8 u1_film_grain_matrix_coefficients;
    /* To be 0 : additive blending */
    UWORD8 u1_blending_mode_id;
    /* To be in range of 2-7 : scale factor used in film grain simulation*/
    UWORD8 u1_log2_scale_factor;
    ///* To be either 8 or 16 : blockSize used in film grain simulation*/
    //UWORD8 blockSize;
    /* Control for component model for each component*/
    UWORD8 u1_comp_model_present_flag[MAX_NUM_COMP];
    /* Number of intensity intervals in each component */
    UWORD8 u1_num_intensity_intervals_minus1[MAX_NUM_COMP];
    /* Number of model values in each component */
    UWORD8 u1_num_model_values_minus1[MAX_NUM_COMP];
    /* Lower bound of intensity interval */
    UWORD8 u1_intensity_interval_lower_bound[MAX_NUM_COMP][MAX_NUM_INTENSITIES];
    /* Upper bound of intensity interval */
    UWORD8 u1_intensity_interval_upper_bound[MAX_NUM_COMP][MAX_NUM_INTENSITIES];
    /* Component model values for each intensity interval */
    UWORD32 u4_comp_model_value[MAX_NUM_COMP][MAX_NUM_INTENSITIES][MAX_NUM_MODEL_VALUES];
    /* To be 0:  Persistence of the film grain characteristics */
	UWORD32 u4_film_grain_characteristics_repetition_period;
    UWORD32 u1_film_grain_characteristics_persistence_flag;
}fgcr_set_fgc_params_t;

WORD32 i264_generate_sei_message(bitstrm_t *ps_bitstrm, fgcr_set_fgc_params_t *ps_fgs_prms);
WORD32 i265_generate_sei_message(bitstrm_t *ps_bitstrm, fgcr_set_fgc_params_t *ps_fgs_prms, UWORD8 num_temporal_id_plus1);

struct _sei
{
    // structure variable for FGS SEI
    sei_fgc_params_t s_fgc_params;
    UWORD8 u1_is_valid;
};
typedef struct _sei sei;
#endif /* _FGCR_SEI_H_ */

