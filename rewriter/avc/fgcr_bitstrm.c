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
 * \file ih264d_bitstrm.c
 *
 * \brief
 *    Bitstream parsing routines
 *
 **************************************************************************
 */

#include <stdlib.h>
#include <assert.h>
#include "fgcr_typedefs.h"
#include "fgcr_platform_macros.h"
#include "fgcr_utils.h"
#include "fgcr_bitstrm.h"
#include "fgcr_error_handler.h"

#include "fgcr_debug.h"
#include "fgcr_structs.h"

/*!
 **************************************************************************
 * \if Function name : ih264d_get_bit_h264 \endif
 *
 * \brief
 *    Read one bit from the bitstream.
 *
 *   This is a Bitstream processing function. It reads the
 *   bit currently pointed by the bit pointer in the
 *   buffer and advances the pointer by one. It returns
 *   the bit (0 or 1) in the form of an unsigned integer.
 *
 * \return
 *    Returns the next bit (0 or 1) in the bitstream.
 *
 **************************************************************************
 */
UWORD8 ih264d_get_bit_h264(dec_bit_stream_t *ps_stream)
{
    UWORD32 u4_code;

    GETBIT(u4_code, ps_stream->u4_ofst, ps_stream->pu4_buffer);
    return (u4_code);
}

/*!
 **************************************************************************
 * \if Function name : ih264d_get_bits_h264 \endif
 *
 * \brief
 *    Read specified number of bits from the bitstream.
 *
 *   This is a Bitstream processing function. It reads the
 *   number specified number of bits from the current bit
 *   position and advances the bit and byte pointers
 *   appropriately.
 *
 * \return
 *    An unsigned 32 bit integer with its least significant bits
 *    containing the bits in order of their occurence in the bitstream.
 *
 **************************************************************************
 */

UWORD32 ih264d_get_bits_h264(dec_bit_stream_t *ps_bitstrm, UWORD32 u4_num_bits)
{
    UWORD32 u4_code = 0;
    if(u4_num_bits)
        GETBITS(u4_code, ps_bitstrm->u4_ofst, ps_bitstrm->pu4_buffer, u4_num_bits);
    return (u4_code);
}

/*!
 **************************************************************************
 * \if Function name : ih264d_next_bits_h264 \endif
 *
 * \brief
 *    Peek specified number of bits from the bitstream.
 *
 *   This is a Bitstream processing function. It gets the
 *   specified number of bits from the buffer without
 *   altering the current pointers. It is equivalent to
 *   next_bits() function in the standard.
 *
 * \return
 *    An unsigned 32 bit integer with its least significant bits
 *    containing the bits in order of their occurence in the bitstream.
 **************************************************************************
 */
UWORD32 ih264d_next_bits_h264(dec_bit_stream_t *ps_bitstrm, UWORD32 u4_num_bits)
{
    UWORD32 u4_word_off = (ps_bitstrm->u4_ofst >> 5);
    UWORD32 u4_bit_off = ps_bitstrm->u4_ofst & 0x1F;
    UWORD32 *pu4_bitstream = ps_bitstrm->pu4_buffer;
    UWORD32 u4_bits = pu4_bitstream[u4_word_off++] << u4_bit_off;

    /*************************************************************************/
    /* Test if number of bits to be read exceeds the number of bits in the   */
    /* current word. If yes, read from the next word of the buffer, The bits */
    /* from both the words are concatenated to get next 32 bits in 'u4_bits' */
    /*************************************************************************/
    if(u4_bit_off > (INT_IN_BITS - u4_num_bits))
        u4_bits |= (pu4_bitstream[u4_word_off] >> (INT_IN_BITS - u4_bit_off));

    return ((u4_bits >> (INT_IN_BITS - u4_num_bits)));
}

/*!
 **************************************************************************
 * \if Function name : ih264d_flush_bits_h264 \endif
 *
 * \brief
 *    Flush specified number of bits from the bitstream.
 *
 *   This function flushes the specified number of bits (marks
 *   as read) from the buffer.
 *
 * \return
 *     A 8 bit unsigned integer with value
 *    '1' on successful flush
 *    '0' on failure.
 *
 **************************************************************************
 */
WORD32 ih264d_flush_bits_h264(dec_bit_stream_t *ps_bitstrm, WORD32 u4_num_bits)
{
    ps_bitstrm->u4_ofst += u4_num_bits;

    if(ps_bitstrm->u4_ofst > ps_bitstrm->u4_max_ofst)
    {
        return ERROR_EOB_FLUSHBITS_T;
    }
    return OK;
}

/*!
 **************************************************************************
 * \if Function name : ih264d_check_byte_aligned \endif
 *
 * \brief
 *    Checks whether the bit ps_bitstrm u4_ofst is at byte boundary.
 *
 * \param ps_bitstrm : Pointer to bitstream
 *
 * \return
 *    Returns 1 if bit ps_bitstrm u4_ofst is at byte alligned position else zero.
 **************************************************************************
 */

UWORD8 ih264d_check_byte_aligned(dec_bit_stream_t * ps_bitstrm)
{
    if(ps_bitstrm->u4_ofst & 0x07)
        return (0);
    else
        return (1);
}

/**
******************************************************************************
*
*  @brief Initializes the encoder bitstream engine
*
*  @par   Description
*  This routine needs to be called at start of slice/frame encode
*
*  @param[in]   ps_bitstrm
*  pointer to bitstream context (handle)
*
*  @param[in]   p1_bitstrm_buf
*  bitstream buffer pointer where the encoded stream is generated in byte order
*
*  @param[in]   u4_max_bitstrm_size
*  indicates maximum bitstream buffer size. (in bytes)
*  If actual stream size exceeds the maximum size, encoder should
*   1. Not corrupt data beyond u4_max_bitstrm_size bytes
*   2. Report an error back to application indicating overflow
*
*  @return      success or failure error code
*
******************************************************************************
*/
WORD32  i264_bitstrm_init(bitstrm_t   *ps_bitstrm,
    UWORD8     *pu1_bitstrm_buf,
    UWORD32     u4_max_bitstrm_size)
{
    ps_bitstrm->pu1_strm_buffer = pu1_bitstrm_buf;
    ps_bitstrm->u4_max_strm_size = u4_max_bitstrm_size;

    /* Default init values for other members of bitstream context */
    ps_bitstrm->u4_strm_buf_offset = 0;
    ps_bitstrm->u4_cur_word = 0;
    ps_bitstrm->i4_bits_left_in_cw = WORD_SIZE;
    ps_bitstrm->i4_zero_bytes_run = 0;

    return 0;
}

/**
******************************************************************************
*
*  @brief insert NAL start code prefix (0x000001) into bitstream with an option
*  of inserting leading_zero_8bits (which makes startcode prefix as 0x00000001)
*
*  @par   Description
*  Although start code prefix could have been put by calling ihevce_put_bits(),
*  ihevce_put_nal_start_code_prefix() is specially added to make sure emulation
*  prevention insertion is not done for the NAL start code prefix which will
*  surely happen otherwise by calling ihevce_put_bits() interface.
*
*  @param[in]    ps_bitstrm
*  pointer to bitstream context (handle)
*
*  @param[in]    insert_leading_zero_8bits
*  flag indicating if one more zero bytes needs to prefixed before start code
*
*  @return      success or failure error code
*
******************************************************************************
*/
WORD32    i264_put_nal_start_code_prefix(bitstrm_t   *ps_bitstrm,
    WORD32      insert_leading_zero_8bits)
{
    UWORD32  u4_strm_buf_offset = ps_bitstrm->u4_strm_buf_offset;
    UWORD8* pu1_strm_buf = ps_bitstrm->pu1_strm_buffer;
    WORD32  num_nals = ps_bitstrm->i4_num_nal;

    /* Bitstream buffer overflow check assuming worst case of 4 bytes */
    if ((u4_strm_buf_offset + 4) >= ps_bitstrm->u4_max_strm_size)
    {
        return ERROR_BITSTRM_OVERFLOW;
    }

    /* Update the current NAL start ptr and increment counter */
    ASSERT(num_nals >= 0);
    ASSERT(num_nals < MAX_NALS_IN_AU);
    if (num_nals < MAX_NALS_IN_AU)
    {
        ps_bitstrm->apu1_nal_start[num_nals] = pu1_strm_buf + u4_strm_buf_offset;
        ps_bitstrm->i4_num_nal++;
    }

    /* Insert leading zero 8 bits conditionally */
    if (insert_leading_zero_8bits)
    {
        pu1_strm_buf[u4_strm_buf_offset] = 0x00;
        u4_strm_buf_offset++;
    }

    /* Insert NAL start code prefix 0x00 00 01 */
    pu1_strm_buf[u4_strm_buf_offset] = 0x00;
    u4_strm_buf_offset++;

    pu1_strm_buf[u4_strm_buf_offset] = 0x00;
    u4_strm_buf_offset++;

    pu1_strm_buf[u4_strm_buf_offset] = 0x01;
    u4_strm_buf_offset++;

    /* update the stream offset */
    ps_bitstrm->u4_strm_buf_offset = u4_strm_buf_offset;

    return 0;
}

WORD32 i264_byte_align(bitstrm_t  *ps_bitstream_buf)
{
    WORD32 i4_bits_left_in_cw = ps_bitstream_buf->i4_bits_left_in_cw;

    /* Check for byte alignment */
    if (i4_bits_left_in_cw & 0x7)
    {
        UWORD8	u1_len = (i4_bits_left_in_cw & 0x7);
        /* Put cabac_alignment_one_bit to buffer */
        i264_put_bits(ps_bitstream_buf, ((1 << u1_len) - 1), u1_len, 0);
    }

    /* write the valid bytes to stream */
    i4_bits_left_in_cw = ps_bitstream_buf->i4_bits_left_in_cw;
    if (i4_bits_left_in_cw && (i4_bits_left_in_cw != WORD_SIZE))
    {
        UWORD32 i;
        UWORD32 zero_run = ps_bitstream_buf->i4_zero_bytes_run;
        UWORD32 u4_cur_word = ps_bitstream_buf->u4_cur_word;
        WORD32  bits_left_in_cw = ps_bitstream_buf->i4_bits_left_in_cw;
        WORD32  bytes_left_in_cw = (bits_left_in_cw) >> 3;

        for (i = WORD_SIZE; i > (UWORD32)(bytes_left_in_cw * 8); i -= 8)
        {
            /* flush the bits in cur word byte by byte  and copy to stream */
            UWORD8   u1_next_byte = (u4_cur_word >> (i - 8)) & 0xFF;

            PUTBYTE_EPB(ps_bitstream_buf->pu1_strm_buffer, ps_bitstream_buf->u4_strm_buf_offset, u1_next_byte, zero_run);
        }

        /* Default init values for scratch variables of bitstream context */
        ps_bitstream_buf->u4_cur_word = 0;
        ps_bitstream_buf->i4_bits_left_in_cw = WORD_SIZE;
        ps_bitstream_buf->i4_zero_bytes_run = 0;
    }

    return 0;
}

/**
******************************************************************************
*
*  @brief puts a code with specified number of bits into the bitstream
*
*  @par   Description
*  inserts code_len number of bits from lsb of code_val into the
*  bitstream. updates context members like u4_cur_word, u4_strm_buf_offset and
*  i4_bits_left_in_cw. If the total words (u4_strm_buf_offset) exceeds max
*  available size (u4_max_strm_size), returns error without corrupting data
*  beyond it
*
*  @param[in]    ps_bitstrm
*  pointer to bitstream context (handle)
*
*  @param[in]    u4_code_val
*  code value that needs to be inserted in the stream.
*
*  @param[in]    code_len
*  indicates code length (in bits) of code_val that would be inserted in
*  bitstream buffer size. Range of length[1:WORD_SIZE]
*
*  @remarks     Assumptions: all bits from bit position code_len to msb of
*   code_val shall be zero
*
*  @return      success or failure error code
*
******************************************************************************
*/
WORD32 i264_put_bits(bitstrm_t  *ps_bitstream_buf,
    UWORD32    u4_code_val,
    UWORD32    u4_code_len,
    UWORD32     eos)
{
    /* Local variables */
    UWORD32 u4_cur_word = ps_bitstream_buf->u4_cur_word;
    WORD32  bits_left_in_cw = ps_bitstream_buf->i4_bits_left_in_cw;

    /* check assumptions made in the module */
    ASSERT(u4_code_len > 0 && u4_code_len <= WORD_SIZE);

    if (u4_code_len < WORD_SIZE)
        ASSERT((u4_code_val >> u4_code_len) == 0);

    /* sanity check on the bitstream engine state */
    ASSERT(bits_left_in_cw > 0 && bits_left_in_cw <= WORD_SIZE);

    ASSERT(ps_bitstream_buf->i4_zero_bytes_run <= EPB_ZERO_BYTES);

    ASSERT(ps_bitstream_buf->pu1_strm_buffer != NULL);

    if ((bits_left_in_cw > (WORD32)u4_code_len) && !eos)
    {
        /*******************************************************************/
        /* insert the code in local bitstream word and return              */
        /* code is inserted in position of bits left (post decrement)      */
        /*******************************************************************/
        bits_left_in_cw -= u4_code_len;
        u4_cur_word |= (u4_code_val << bits_left_in_cw);

        ps_bitstream_buf->u4_cur_word = u4_cur_word;
        ps_bitstream_buf->i4_bits_left_in_cw = bits_left_in_cw;

        return(0);
    }
    else if (!eos)
    {
        /********************************************************************/
        /* 1. insert parital code corresponding to bits left in cur word    */
        /* 2. flush all the bits of cur word to bitstream                   */
        /* 3. insert emulation prevention bytes while flushing the bits     */
        /* 4. insert remaining bits of code starting from msb of cur word   */
        /* 5. update bitsleft in current word and stream buffer offset      */
        /********************************************************************/
        UWORD32 u4_strm_buf_offset = ps_bitstream_buf->u4_strm_buf_offset;

        UWORD32 u4_max_strm_size = ps_bitstream_buf->u4_max_strm_size;

        WORD32  zero_run = ps_bitstream_buf->i4_zero_bytes_run;

        UWORD8* pu1_strm_buf = ps_bitstream_buf->pu1_strm_buffer;

        WORD32  i, rem_bits = (u4_code_len - bits_left_in_cw);


        /*********************************************************************/
        /* Bitstream overflow check                                          */
        /* NOTE: corner case of epb bytes (max 2 for 32bit word) not handled */
        /*********************************************************************/
        if ((u4_strm_buf_offset + (WORD_SIZE >> 3)) >= u4_max_strm_size)
        {
            /* return without corrupting the buffer beyond its size */
            return 1;
        }

        /* insert parital code corresponding to bits left in cur word */
        u4_cur_word |= u4_code_val >> rem_bits;

        for (i = WORD_SIZE; i > 0; i -= 8)
        {
            /* flush the bits in cur word byte by byte and copy to stream */
            UWORD8   u1_next_byte = (u4_cur_word >> (i - 8)) & 0xFF;

            PUTBYTE_EPB(pu1_strm_buf, u4_strm_buf_offset, u1_next_byte, zero_run);
        }

        /* insert the remaining bits from code val into current word */
        u4_cur_word = rem_bits ? (u4_code_val << (WORD_SIZE - rem_bits)) : 0;

        /* update the state variables and return success */
        ps_bitstream_buf->u4_cur_word = u4_cur_word;
        ps_bitstream_buf->i4_bits_left_in_cw = WORD_SIZE - rem_bits;
        ps_bitstream_buf->i4_zero_bytes_run = zero_run;
        ps_bitstream_buf->u4_strm_buf_offset = u4_strm_buf_offset;
        return 0;
    }
    if (eos)
    {
        /********************************************************************/
        /* 1. insert parital code corresponding to bits left in cur word    */
        /* 2. flush all the bits of cur word to bitstream                   */
        /* 3. insert emulation prevention bytes while flushing the bits     */
        /* 4. insert remaining bits of code starting from msb of cur word   */
        /* 5. update bitsleft in current word and stream buffer offset      */
        /********************************************************************/
        UWORD32 u4_strm_buf_offset = ps_bitstream_buf->u4_strm_buf_offset;

        UWORD32 u4_max_strm_size = ps_bitstream_buf->u4_max_strm_size;

        WORD32  zero_run = ps_bitstream_buf->i4_zero_bytes_run;

        UWORD8* pu1_strm_buf = ps_bitstream_buf->pu1_strm_buffer;

        WORD32  i, rem_bits;


        /*********************************************************************/
        /* Bitstream overflow check                                          */
        /* NOTE: corner case of epb bytes (max 2 for 32bit word) not handled */
        /*********************************************************************/
        if ((u4_strm_buf_offset + (WORD_SIZE >> 3)) >= u4_max_strm_size)
        {
            /* return without corrupting the buffer beyond its size */
            return 1;
        }

        /* insert parital code corresponding to bits left in cur word */
        if (u4_code_len < (UWORD32)bits_left_in_cw)
        {
            u4_cur_word |= (u4_code_val << (bits_left_in_cw - u4_code_len));
            rem_bits = 0;
        }
        else
        {
            rem_bits = (u4_code_len - bits_left_in_cw);
            u4_cur_word |= u4_code_val >> rem_bits;
        }

        for (i = WORD_SIZE; i > 0; i -= 8)
        {
            /* flush the bits in cur word byte by byte and copy to stream */
            UWORD8   u1_next_byte = (u4_cur_word >> (i - 8)) & 0xFF;

            PUTBYTE_EPB(pu1_strm_buf, u4_strm_buf_offset, u1_next_byte, zero_run);
        }
        if (rem_bits)
        {
            u4_cur_word = (u4_code_val & ((1 << rem_bits) - 1)) << (WORD_SIZE - rem_bits);
            rem_bits = 0;
            for (i = WORD_SIZE; i > 0; i -= 8)
            {
                /* flush the bits in cur word byte by byte and copy to stream */
                UWORD8   u1_next_byte = (u4_cur_word >> (i - 8)) & 0xFF;

                PUTBYTE_EPB(pu1_strm_buf, u4_strm_buf_offset, u1_next_byte, zero_run);
            }
        }

        u4_cur_word = 0;
        /* update the state variables and return success */
        ps_bitstream_buf->u4_cur_word = u4_cur_word;
        ps_bitstream_buf->i4_bits_left_in_cw = WORD_SIZE - rem_bits;
        ps_bitstream_buf->i4_zero_bytes_run = zero_run;
        ps_bitstream_buf->u4_strm_buf_offset = u4_strm_buf_offset;
        return 0;
    }
    ASSERT(0);
    return 0;
} /* End of i264_put_bits */


WORD32 i264_put_rbsp_trailing_bits(bitstrm_t   *ps_bitstrm)
{
    WORD32 i;
    UWORD32 u4_cur_word = ps_bitstrm->u4_cur_word;
    WORD32  bits_left_in_cw = ps_bitstrm->i4_bits_left_in_cw;
    WORD32  bytes_left_in_cw = (bits_left_in_cw - 1) >> 3;

    UWORD32 u4_strm_buf_offset = ps_bitstrm->u4_strm_buf_offset;
    UWORD32 u4_max_strm_size = ps_bitstrm->u4_max_strm_size;
    WORD32  zero_run = ps_bitstrm->i4_zero_bytes_run;
    UWORD8* pu1_strm_buf = ps_bitstrm->pu1_strm_buffer;

    /*********************************************************************/
    /* Bitstream overflow check                                          */
    /* NOTE: corner case of epb bytes (max 2 for 32bit word) not handled */
    /*********************************************************************/
    if ((u4_strm_buf_offset + (WORD_SIZE >> 3) - bytes_left_in_cw) >=
        u4_max_strm_size)
    {
        /* return without corrupting the buffer beyond its size */
        return 1;
    }

    /* insert a 1 at the end of current word and flush all the bits */
    u4_cur_word |= (1 << (bits_left_in_cw - 1));

    /* get the bits to be inserted in msbdb of the word */
    //u4_cur_word <<= (WORD_SIZE - bytes_left_in_cw + 1);

    for (i = WORD_SIZE; i > (bytes_left_in_cw * 8); i -= 8)
    {
        /* flush the bits in cur word byte by byte  and copy to stream */
        UWORD8   u1_next_byte = (u4_cur_word >> (i - 8)) & 0xFF;

        PUTBYTE_EPB(pu1_strm_buf, u4_strm_buf_offset, u1_next_byte, zero_run);
    }

    /* update the stream offset */
    ps_bitstrm->u4_strm_buf_offset = u4_strm_buf_offset;

    /* Default init values for scratch variables of bitstream context */
    ps_bitstrm->u4_cur_word = 0;
    ps_bitstrm->i4_bits_left_in_cw = WORD_SIZE;
    ps_bitstrm->i4_zero_bytes_run = 0;

    return 0;

} /* End of i264_put_rbsp_trailing_bits */


WORD32 i264_put_sev(bitstrm_t   *ps_bitstrm,
    WORD32  i4_code_val)
{
    /* Local variables */
    WORD32 i_codeNum, i_zeroes;
    UWORD32 ui_temp;

    i_codeNum = 2 * i4_code_val - 1;
    if (i_codeNum < 0)
    {
        i_codeNum = abs(i_codeNum) - 1;
    }
    ui_temp = i_codeNum + 1;

    GETRANGE(i_zeroes, ui_temp);
    i_zeroes -= 1;

    i264_put_bits(ps_bitstrm,
        (UWORD32)(i_codeNum + 1),
        (UWORD8)(2 * i_zeroes + 1), 0);
    return 0;

} /* End of i264_put_sev */


WORD32 i264_put_uev(bitstrm_t    *ps_bitstream_buf,
    UWORD32 u4_code_val)
{
    /* Local variables */
    UWORD32 ui_codeNum, ui_temp;
    WORD32 i_zeroes;

    /* Derive local variables */
    ui_codeNum = u4_code_val;
    ui_temp = ui_codeNum + 1;
#if !DEBUGUTILS
    GETRANGE(i_zeroes, ui_temp);
    i_zeroes -= 1;
#else
    i_zeroes = -1;
    while (ui_temp)
    {
        i_zeroes++;
        ui_temp >>= 1;
    }
#endif
    i264_put_bits(ps_bitstream_buf,
        (UWORD32)(ui_codeNum + 1),
        (UWORD8)(2 * i_zeroes + 1), 0);
    return 0;

} /* End of i264_put_uev */
