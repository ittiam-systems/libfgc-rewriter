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
#ifndef _FGCR_DEFS_H_
#define _FGCR_DEFS_H_

/**
 ************************************************************************
 * \file fgcr_defs.h
 *
 * \brief
 *    Type definitions used in the code
 *
 ************************************************************************
 */

/** Bit manipulation macros */
#define CHECKBIT(a,i) ((a) &  (1 << i))

#define NAL_REF_IDC(nal_first_byte)       ((nal_first_byte >> 5) & 0x3)
#define NAL_FORBIDDEN_BIT(nal_first_byte) (nal_first_byte>>7)
#define NAL_UNIT_TYPE(nal_first_byte)     ((nal_first_byte & 0x7E)>>1)
#define NUH_TEMPORAL_ID_PLUS1(nal_second_byte)     (nal_second_byte & 0x7)

#define OK        0
#define NOT_OK    -1

/** NAL Types */
//#define SLICE_NAL                       1
//#define SLICE_DATA_PARTITION_A_NAL      2
//#define SLICE_DATA_PARTITION_B_NAL      3
//#define SLICE_DATA_PARTITION_C_NAL      4
//#define IDR_SLICE_NAL                   5
//#define SEI_NAL                         6
//#define SEQ_PARAM_NAL                   7
//#define PIC_PARAM_NAL                   8
//#define ACCESS_UNIT_DELIMITER_RBSP      9
//#define END_OF_SEQ_RBSP                 10
//#define END_OF_STREAM_RBSP              11
//#define FILLER_DATA_NAL                 12
#define NAL_TRAIL_N                       0
#define NAL_TRAIL_R                       1
#define NAL_TSA_N                         2
#define NAL_TSA_R                         3
#define NAL_STSA_N                        4
#define NAL_STSA_R                        5
#define NAL_RADL_N                        6
#define NAL_RADL_R                        7
#define NAL_RASL_N                        8
#define NAL_RASL_R                        9
#define NAL_RSV_VCL_N10                   10
#define NAL_RSV_VCL_N12                   12
#define NAL_RSV_VCL_N14                   14
#define NAL_RSV_VCL_R11                   11
#define NAL_RSV_VCL_R13                   13
#define NAL_RSV_VCL_R15                   15
#define NAL_BLA_W_LP                      16
#define NAL_BLA_W_DLP                     17
#define NAL_BLA_N_LP                      18
#define NAL_IDR_W_LP                      19
#define NAL_IDR_N_LP                      20
#define NAL_CRA                           21
#define NAL_RSV_RAP_VCL22                 22
#define NAL_RSV_RAP_VCL23                 23
#define NAL_RSV_VCL24                     24
#define NAL_RSV_VCL31                     31
#define NAL_VPS                           32
#define NAL_SPS                           33
#define NAL_PPS                           34
#define NAL_AUD                           35
#define NAL_EOS                           36
#define NAL_EOB                           37
#define NAL_FD                            38
#define NAL_PREFIX_SEI                    39
#define NAL_SUFFIX_SEI                    40
#define NAL_RSV_NVCL41                    41
#define NAL_RSV_NVCL47                    47
#define NAL_UNSPEC48                      48
#define NAL_UNSPEC63                      49


/** Picture Types */
#define I_PIC       0
#define IP_PIC      1
#define IPB_PIC     2
#define SI_PIC      3
#define SIP_PIC     4
#define ISI_PIC     5
#define ISI_PSP_PIC 6
#define ALL_PIC     7

#endif /*_FGCR_DEFS_H_*/
