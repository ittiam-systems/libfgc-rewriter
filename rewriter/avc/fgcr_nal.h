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

#ifndef _FGCR_NAL_H_
#define _FGCR_NAL_H_

/*!
*************************************************************************
* \file fgcr_nal.h
*
* \brief
*    short_description
*
* Detailed_description
*
*************************************************************************
*/
#include <stdio.h>
#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"
#include "fgcr_bitstrm.h"

WORD32 ih264d_process_nal_unit(dec_bit_stream_t *ps_bitstrm,
                            UWORD8 *pu1_nal_unit,
                            UWORD32 u4_numbytes_in_nal_unit,
                            UWORD8 codec);
void ih264d_rbsp_to_sodb(dec_bit_stream_t *ps_bitstrm);
WORD32 ih264d_find_start_code(UWORD8 *pu1_buf,
                              UWORD32 u4_cur_pos,
                              UWORD32 u4_max_ofst,
                              UWORD32 *pu4_length_of_start_code,
                              UWORD32 *pu4_next_is_aud,
                              UWORD8 codec);


#endif /* _FGCR_NAL_H_ */
