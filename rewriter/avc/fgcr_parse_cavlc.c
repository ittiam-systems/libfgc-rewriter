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
 ***************************************************************************
 * \file ih264d_parse_cavlc.c
 *
 * \brief
 *    This file contains UVLC related functions.
 *
 ***************************************************************************
 */

#include <string.h>
#include <stdio.h>

#include "fgcr_bitstrm.h"
#include "fgcr_parse_cavlc.h"
#include "fgcr_error_handler.h"
#include "fgcr_defs.h"
#include "fgcr_debug.h"
#include "fgcr_structs.h"

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_uev                                               */
/*                                                                           */
/*  Description   : Reads the unsigned Exp Golomb codec syntax from the      */
/*                  ps_bitstrm as specified in section 9.1 of H264 standard  */
/*                  It also increases bitstream u4_ofst by the number of bits*/
/*                  parsed for UEV decode operation                          */
/*                                                                           */
/*  Inputs        : bitstream base pointer and bitsream u4_ofst in bits      */
/*  Globals       : None                                                     */
/*  Processing    :                                                          */
/*  Outputs       : UEV decoded syntax element and incremented ps_bitstrm u4_ofst */
/*  Returns       : UEV decoded syntax element                               */
/*                                                                           */
/*****************************************************************************/
UWORD32 ih264d_uev(UWORD32 *pu4_bitstrm_ofst, UWORD32 *pu4_bitstrm_buf)
{
    UWORD32 u4_bitstream_offset = *pu4_bitstrm_ofst;
    UWORD32 u4_word, u4_ldz;

    /***************************************************************/
    /* Find leading zeros in next 32 bits                          */
    /***************************************************************/
    NEXTBITS_32(u4_word, u4_bitstream_offset, pu4_bitstrm_buf);
    u4_ldz = CLZ(u4_word);
    /* Flush the ps_bitstrm */
    u4_bitstream_offset += (u4_ldz + 1);
    /* Read the suffix from the ps_bitstrm */
    u4_word = 0;
    if(u4_ldz)
        GETBITS(u4_word, u4_bitstream_offset, pu4_bitstrm_buf, u4_ldz);
    *pu4_bitstrm_ofst = u4_bitstream_offset;
    return ((1 << u4_ldz) + u4_word - 1);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : ih264d_sev                                               */
/*                                                                           */
/*  Description   : Reads the signed Exp Golomb codec syntax from the ps_bitstrm */
/*                  as specified in section 9.1 of H264 standard.            */
/*                  It also increases bitstream u4_ofst by the number of bits*/
/*                  parsed for SEV decode operation                          */
/*                                                                           */
/*  Inputs        : bitstream base pointer and bitsream u4_ofst in bits      */
/*  Globals       : None                                                     */
/*  Processing    :                                                          */
/*  Outputs       : SEV decoded syntax element and incremented ps_bitstrm u4_ofst */
/*  Returns       : SEV decoded syntax element                               */
/*                                                                           */
/*****************************************************************************/
WORD32 ih264d_sev(UWORD32 *pu4_bitstrm_ofst, UWORD32 *pu4_bitstrm_buf)
{
    UWORD32 u4_bitstream_offset = *pu4_bitstrm_ofst;
    UWORD32 u4_word, u4_ldz, u4_abs_val;

    /***************************************************************/
    /* Find leading zeros in next 32 bits                          */
    /***************************************************************/
    NEXTBITS_32(u4_word, u4_bitstream_offset, pu4_bitstrm_buf);
    u4_ldz = CLZ(u4_word);

    /* Flush the ps_bitstrm */
    u4_bitstream_offset += (u4_ldz + 1);

    /* Read the suffix from the ps_bitstrm */
    u4_word = 0;
    if(u4_ldz)
        GETBITS(u4_word, u4_bitstream_offset, pu4_bitstrm_buf, u4_ldz);

    *pu4_bitstrm_ofst = u4_bitstream_offset;
    u4_abs_val = ((1 << u4_ldz) + u4_word) >> 1;

    if(u4_word & 0x1)
        return (-(WORD32)u4_abs_val);
    else
        return (u4_abs_val);
}

