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

#ifndef _FGCR_BITSTRM_H_
#define _FGCR_BITSTRM_H_
/*!
 *************************************************************************
 * \file fgcr_bitstrm.h
 *
 * \brief
 *  Contains all the declarations of bitstream reading routines
 *
 *************************************************************************
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"

#define INT_IN_BYTES        4
#define INT_IN_BITS         32

/**
******************************************************************************
 *  @brief      defines the maximum number of bits in a bitstream word
******************************************************************************
 */
#define WORD_SIZE         32

/**
******************************************************************************
*  @brief  The number of consecutive zero bytes for emulation prevention check
******************************************************************************
*/
#define EPB_ZERO_BYTES      2

/**
******************************************************************************
*  @brief  Emulation prevention insertion byte
******************************************************************************
*/
#define EPB_BYTE            0x03

/* Based on level 1.2 of baseline profile */
/* 396[MAX_FS] * 128 * 1.5 [ChromaFormatParameter] / sizeof(UWORD32)
 i.e  396 * 128 * 1.5 / 4 = 19008 */
/* Based on level 3 of main profile */
/* 1620[MAX_FS] * 128 * 1.5 [ChromaFormatParameter] / sizeof(UWORD32)
 i.e  1620 * 128 * 1.5 / 4= 77760 */
#define SIZE_OF_BUFFER      77760
#define MAX_NALS_IN_AU      256
/* Structure for the ps_bitstrm */
typedef struct
{
    UWORD32 u4_ofst; /* Offset in the buffer for the current bit */
    UWORD32 *pu4_buffer; /* Bitstream Buffer  */
    UWORD32 u4_max_ofst; /* Position of the last bit read in the current buffer */
    void * pv_codec_handle; /* For Error Handling */
} dec_bit_stream_t;

typedef struct bitstrm
{
    /** points to start of stream buffer.    */
    UWORD8  *pu1_strm_buffer;

    /**
     *  max bitstream size (in bytes).
     *  Encoded stream shall not exceed this size.
     */
    UWORD32 u4_max_strm_size;

    /**
     *  byte offset (w.r.t pu1_strm_buffer) where next byte would be written
     *  Bitstream engine makes sure it would not corrupt data beyond
     *  u4_max_strm_size bytes
                                 */
    UWORD32 u4_strm_buf_offset;

    /**
     *  current bitstream word; It is a scratch word containing max of
     *  WORD_SIZE bits. Will be copied to stream buffer when the word is
     *  full
                                 */
    UWORD32 u4_cur_word;

    /**
     *  signifies number of bits available in u4_cur_word
     *  bits from MSB to i4_bits_left_in_cw of u4_cur_word have already been
     *  inserted next bits would be inserted from pos [i4_bits_left_in_cw-1]
     *  Range of this variable [1 : WORD_SIZE]
                                 */
    WORD32  i4_bits_left_in_cw;

    /**
     *  signifies the number of consecutive zero bytes propagated from previous
     *  word. It is used for emulation prevention byte insertion in the stream
                                 */
    WORD32  i4_zero_bytes_run;

    /** Total number of NAL units in the output buffer; Shall not exceed MAX_NALS_IN_AU */
    WORD32 i4_num_nal;

    /** Pointer to start of each NAL unit in the output buffer */
    UWORD8  *apu1_nal_start[MAX_NALS_IN_AU];

} bitstrm_t;

/**
******************************************************************************
 *  @brief   Macro to check if emulation prevention byte insertion is required
******************************************************************************
 */
#define INSERT_EPB(zero_run, next_byte)                                       \
    ((zero_run) == EPB_ZERO_BYTES) && (0 == ((next_byte) & 0xFC))

/**
******************************************************************************
 *  @brief  Inserts 1 byte and Emulation Prevention Byte(if any) into bitstream
 *          Increments the stream offset and zero run correspondingly
******************************************************************************
 */
#define PUTBYTE_EPB(ptr,off,byte,zero_run)                      \
{                                                               \
    if( INSERT_EPB(zero_run, byte) )                            \
    {                                                           \
        ptr[off] = EPB_BYTE;                                    \
        off++;                                                  \
        zero_run = 0;                                           \
    }                                                           \
                                                                \
    ptr[off] = byte;                                            \
    off++;                                                      \
    zero_run = byte ? 0 : zero_run+1;                           \
}

/* To read the next bit */
UWORD8 ih264d_get_bit_h264(dec_bit_stream_t *);

/* To read the next specified number of bits */
UWORD32 ih264d_get_bits_h264(dec_bit_stream_t *, UWORD32);

/* To see the next specified number of bits */
UWORD32 ih264d_next_bits_h264(dec_bit_stream_t *, UWORD32);

/* To flush a specified number of bits*/
WORD32 ih264d_flush_bits_h264(dec_bit_stream_t *, WORD32);

/*!
 **************************************************************************
 * \if Function name : MoreRbspData \endif
 *
 * \brief
 *    Determines whether there is more data in RBSP or not.
 *
 * \param ps_bitstrm : Pointer to bitstream
 *
 * \return
 *    Returns 1 if there is more data in RBSP before rbsp_trailing_bits().
 *    Otherwise it returns FALSE.
 **************************************************************************
 */

#define MORE_RBSP_DATA(ps_bitstrm) \
  (ps_bitstrm->u4_ofst < ps_bitstrm->u4_max_ofst)
#define EXCEED_OFFSET(ps_bitstrm) \
  (ps_bitstrm->u4_ofst > ps_bitstrm->u4_max_ofst)

void GoToByteBoundary(dec_bit_stream_t * ps_bitstrm);
UWORD8 ih264d_check_byte_aligned(dec_bit_stream_t * ps_bitstrm);


/******FGS rewriter functions********/
WORD32  i264_bitstrm_init(bitstrm_t   *ps_bitstrm,
    UWORD8     *pu1_bitstrm_buf,
    UWORD32     u4_max_bitstrm_size);

WORD32    i264_put_nal_start_code_prefix(bitstrm_t   *ps_bitstrm,
    WORD32      insert_leading_zero_8bits);

WORD32 i264_byte_align(bitstrm_t  *ps_bitstream_buf);

WORD32 i264_put_bits(bitstrm_t  *ps_bitstream_buf,
    UWORD32    u4_code_val,
    UWORD32    u4_code_len,
    UWORD32     eos);

WORD32 i264_put_rbsp_trailing_bits(bitstrm_t   *ps_bitstrm);

WORD32 i264_put_sev(bitstrm_t   *ps_bitstrm,
    WORD32  i4_code_val);

WORD32 i264_put_uev(bitstrm_t    *ps_bitstream_buf,
    UWORD32 u4_code_val);

/*****************************************************************************/
/* Define a macro for inlining of GETBIT:                                    */
/*****************************************************************************/
#define   GETBIT(u4_code, u4_offset, pu4_bitstream)                         \
{                                                                           \
    UWORD32 *pu4_buf =  (pu4_bitstream);                                    \
    UWORD32 u4_word_off = ((u4_offset) >> 5);                               \
    UWORD32 u4_bit_off = (u4_offset) & 0x1F;                                \
    u4_code = pu4_buf[u4_word_off] << u4_bit_off;                           \
    (u4_offset)++;                                                          \
    u4_code = (u4_code >> 31);                                              \
}

/*****************************************************************************/
/* Define a macro for inlining of GETBITS: u4_no_bits shall not exceed 32    */
/*****************************************************************************/
#define     GETBITS(u4_code, u4_offset, pu4_bitstream, u4_no_bits)          \
{                                                                           \
    UWORD32 *pu4_buf =  (pu4_bitstream);                                    \
    UWORD32 u4_word_off = ((u4_offset) >> 5);                               \
    UWORD32 u4_bit_off = (u4_offset) & 0x1F;                                \
    u4_code = pu4_buf[u4_word_off++] << u4_bit_off;                         \
                                                                            \
    if(u4_bit_off)                                                          \
        u4_code |= (pu4_buf[u4_word_off] >> (INT_IN_BITS - u4_bit_off));    \
    u4_code = u4_code >> (INT_IN_BITS - u4_no_bits);                        \
    (u4_offset) += u4_no_bits;                                              \
}                                                                           \
                                                                            \

/*****************************************************************************/
/* Define a macro for inlining of NEXTBITS                                   */
/*****************************************************************************/
#define     NEXTBITS(u4_word, u4_offset, pu4_bitstream, u4_no_bits)         \
{                                                                           \
    UWORD32 *pu4_buf =  (pu4_bitstream);                                    \
    UWORD32 u4_word_off = ((u4_offset) >> 5);                               \
    UWORD32 u4_bit_off = (u4_offset) & 0x1F;                                \
    u4_word = pu4_buf[u4_word_off++] << u4_bit_off;                         \
    if(u4_bit_off)                                                          \
        u4_word |= (pu4_buf[u4_word_off] >> (INT_IN_BITS - u4_bit_off));    \
    u4_word = u4_word >> (INT_IN_BITS - u4_no_bits);                        \
}

/*****************************************************************************/
/* Define a macro for inlining of NEXTBITS_32                                */
/*****************************************************************************/
#define     NEXTBITS_32(u4_word, u4_offset, pu4_bitstream)                  \
{                                                                           \
    UWORD32 *pu4_buf =  (pu4_bitstream);                                    \
    UWORD32 u4_word_off = ((u4_offset) >> 5);                               \
    UWORD32 u4_bit_off = (u4_offset) & 0x1F;                                \
                                                                            \
    u4_word = pu4_buf[u4_word_off++] << u4_bit_off;                         \
    if(u4_bit_off)                                                          \
    u4_word |= (pu4_buf[u4_word_off] >> (INT_IN_BITS - u4_bit_off));        \
}

/*****************************************************************************/
/* Define a macro for inlining of FIND_ONE_IN_STREAM_32                      */
/*****************************************************************************/
#define   FIND_ONE_IN_STREAM_32(u4_ldz, u4_offset, pu4_bitstream)           \
{                                                                           \
    UWORD32 u4_word;                                                        \
    NEXTBITS_32(u4_word, u4_offset, pu4_bitstream);                         \
    u4_ldz = CLZ(u4_word);													\
    (u4_offset) += (u4_ldz + 1);                                            \
}

/*****************************************************************************/
/* Define a macro for inlining of FIND_ONE_IN_STREAM_LEN                     */
/*****************************************************************************/
#define   FIND_ONE_IN_STREAM_LEN(u4_ldz, u4_offset, pu4_bitstream, u4_len)  \
{                                                                           \
    UWORD32 u4_word;                                                        \
    NEXTBITS_32(u4_word, u4_offset, pu4_bitstream);                         \
    u4_ldz = CLZ(u4_word);                                     \
    if(u4_ldz < u4_len)                                                     \
    (u4_offset) += (u4_ldz + 1);                                            \
    else                                                                    \
    {                                                                       \
        u4_ldz = u4_len;                                                    \
        (u4_offset) += u4_ldz;                                              \
    }                                                                       \
}

/*****************************************************************************/
/* Define a macro for inlining of FLUSHBITS                                  */
/*****************************************************************************/
#define   FLUSHBITS(u4_offset, u4_no_bits)                                  \
{                                                                           \
        (u4_offset) += (u4_no_bits);                                        \
}

#define I264_SEI_FILM_GRAIN_CHARACTERISTICS_T 19
#define I264_SEI_NAL 6

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define ENTROPY_TRACE(syntax_string, value) // no functionality

/**
******************************************************************************
 *  @brief   Macro to put a code with specified number of bits into the
 *           bitstream
******************************************************************************
 */
#define PUT_BITS(ps_bitstrm, code_val, code_len, ret_val, syntax_string) \
         ENTROPY_TRACE(syntax_string, code_val);\
        ret_val |= i264_put_bits((ps_bitstrm), (code_val), (code_len), 0)

/**
******************************************************************************
*  @brief   Macro to put a code with inverse flag into the bitstream
******************************************************************************
*/
#define PUT_BITS_INV(ps_bitstrm, code_val, ret_val, syntax_string) \
        ENTROPY_TRACE(syntax_string, !code_val);\
        ret_val |= i264_put_bits((ps_bitstrm), (!code_val), 1, 0)

/**
******************************************************************************
*  @brief   Macro to put a code with specified number of bits into the
*           bitstream using 0th order exponential Golomb encoding for
*           signed numbers
******************************************************************************
*/
#define PUT_BITS_UEV(ps_bitstrm, code_val, ret_val, syntax_string) \
        ENTROPY_TRACE(syntax_string, code_val);\
        ret_val |= i264_put_uev((ps_bitstrm), (code_val))

/**
******************************************************************************
*  @brief   Macro to put a code with specified number of bits into the
*           bitstream using 0th order exponential Golomb encoding for
*           signed numbers
******************************************************************************
*/
#define PUT_BITS_SEV(ps_bitstrm, code_val, ret_val, syntax_string) \
        ENTROPY_TRACE(syntax_string, code_val);\
        ret_val |= i264_put_sev((ps_bitstrm), (code_val))
#endif  /* _FGCR_BITSTRM_H_ */
