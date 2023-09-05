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
/*  File Name         : ih264d_api.c                                         */
/*                                                                           */
/*  Description       : Has all  API related functions                       */
/*                                                                           */
/*  List of Functions : api_check_struct_sanity                              */
/*          fgcr_create                                                      */
/*          fgcr_delete                                                      */
/*          fgcr_init                                                        */
/*          fgcr_map_error                                                   */
/*          fgcr_get_version                                                 */
/*          fgcr_get_status                                                  */
/*          fgcr_set_params                                                  */
/*          fgcr_ctl                                                         */
/*          fgcr_fill_output_struct_from_context                             */
/*          fgcr_api_function                                                */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*****************************************************************************/
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "fgcr_typedefs.h"
#include "fgcr_macros.h"
#include "fgcr_platform_macros.h"
#include "fgcr.h"
#include "fgcr_defs.h"
#include "fgcr_structs.h"
#include "fgcr_nal.h"
#include "fgcr_error_handler.h"
#include "fgcr_defs.h"
#include "fgcr_parse_slice.h"
#include "fgcr_parse_cavlc.h"
#include "fgcr_utils.h"
#include "fgcr_parse_headers.h"

/*********************/
/* Codec Versioning  */
/*********************/
#define CODEC_NAME              "REWRITER"
#define CODEC_RELEASE_TYPE      "production"
#define CODEC_RELEASE_VER       "1.0"
#define CODEC_VENDOR            "ITTIAM"
#define MAXVERSION_STRLEN       511

#ifdef X86_MSVC
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif
#define VERSION(version_string, codec_name, codec_release_type, codec_release_ver, codec_vendor)    \
    SNPRINTF(version_string, MAXVERSION_STRLEN,                                                     \
             "@(#)Id:%s_%s Ver:%s Released by %s Build: %s @ %s",                                   \
             codec_name, codec_release_type, codec_release_ver, codec_vendor, __DATE__, __TIME__)

#define DEFAULT_WD 1920
#define DEFAULT_HT 1080

IV_API_CALL_STATUS_T fgcr_set_fgs_rewrite_params(
	iv_obj_t *dec_hdl,
	void *pv_api_ip,
	void *pv_api_op);

IV_API_CALL_STATUS_T fgcr_set_fgs_params(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op);

void fgcr_fill_output_struct_from_context(dec_struct_t *ps_dec,
	fgcr_video_rewrite_ip_t *ps_h264_dec_ip,
	fgcr_video_rewrite_op_t *ps_h264_dec_op);

void fgcr_reset_sei(dec_struct_t *ps_dec)
{
	memset(ps_dec->ps_sei, 0, sizeof(sei));
}

static IV_API_CALL_STATUS_T api_check_struct_sanity(iv_obj_t *ps_handle,
	void *pv_api_ip,
	void *pv_api_op)
{
	FGCR_API_COMMAND_TYPE_T e_cmd;
	UWORD32 *pu4_api_ip;
	UWORD32 *pu4_api_op;
	//UWORD32 i;

	if (NULL == pv_api_op)
	{
		return (IV_FAIL);
	}

	if (NULL == pv_api_ip)
	{
		return (IV_FAIL);
	}

	pu4_api_ip = (UWORD32 *)pv_api_ip;
	pu4_api_op = (UWORD32 *)pv_api_op;
	e_cmd = (FGCR_API_COMMAND_TYPE_T)*(pu4_api_ip + 1);

	/* error checks on handle */
	switch ((WORD32)e_cmd)
	{
	case FGCR_CMD_CREATE:
		break;

	case FGCR_CMD_VIDEO_REWRITE:
	case FGCR_CMD_EXPORT:
	case FGCR_CMD_DELETE:
	case FGCR_CMD_VIDEO_CTL:
		if (ps_handle == NULL)
		{
			*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
			*(pu4_api_op + 1) |= FGCR_HANDLE_NULL;
			return IV_FAIL;
		}

		if (ps_handle->u4_size != sizeof(iv_obj_t))
		{
			*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
			*(pu4_api_op + 1) |= FGCR_HANDLE_STRUCT_SIZE_INCORRECT;
			return IV_FAIL;
		}

		if (ps_handle->pv_fxns != fgcr_api_function)
		{
			*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
			*(pu4_api_op + 1) |= FGCR_INVALID_HANDLE_NULL;
			return IV_FAIL;
		}

		if (ps_handle->pv_codec_handle == NULL)
		{
			*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
			*(pu4_api_op + 1) |= FGCR_INVALID_HANDLE_NULL;
			return IV_FAIL;
		}
		break;

	default:
		*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
		*(pu4_api_op + 1) |= FGCR_INVALID_API_CMD;
		return IV_FAIL;
	}

	switch ((WORD32)e_cmd)
	{
	case FGCR_CMD_CREATE:
	{
		fgcr_create_ip_t *ps_ip = (fgcr_create_ip_t *)pv_api_ip;
		fgcr_create_op_t *ps_op = (fgcr_create_op_t *)pv_api_op;

		ps_op->u4_error_code = 0;

		if (ps_ip->u4_size != sizeof(fgcr_create_ip_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_IP_API_STRUCT_SIZE_INCORRECT;
			H264_DEC_DEBUG_PRINT("\n");
			return (IV_FAIL);
		}

		if (ps_op->u4_size != sizeof(fgcr_create_op_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_OP_API_STRUCT_SIZE_INCORRECT;
			H264_DEC_DEBUG_PRINT("\n");
			return (IV_FAIL);
		}
	}
	break;

	case FGCR_CMD_VIDEO_REWRITE:
	{
		fgcr_video_rewrite_ip_t *ps_ip =
			(fgcr_video_rewrite_ip_t *)pv_api_ip;
		fgcr_video_rewrite_op_t *ps_op =
			(fgcr_video_rewrite_op_t *)pv_api_op;

		H264_DEC_DEBUG_PRINT("The input bytes is: %d",
			ps_ip->u4_num_Bytes);
		ps_op->u4_error_code = 0;

		if (ps_ip->u4_size
			!= sizeof(fgcr_video_rewrite_ip_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_IP_API_STRUCT_SIZE_INCORRECT;
			return (IV_FAIL);
		}

		if (ps_op->u4_size
			!= sizeof(fgcr_video_rewrite_op_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_OP_API_STRUCT_SIZE_INCORRECT;
			return (IV_FAIL);
		}
	}
	break;

	case FGCR_CMD_DELETE:
	{
		fgcr_delete_ip_t *ps_ip = (fgcr_delete_ip_t *)pv_api_ip;
		fgcr_delete_op_t *ps_op = (fgcr_delete_op_t *)pv_api_op;

		ps_op->u4_error_code = 0;

		if (ps_ip->u4_size != sizeof(fgcr_delete_ip_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_IP_API_STRUCT_SIZE_INCORRECT;
			return (IV_FAIL);
		}

		if (ps_op->u4_size != sizeof(fgcr_delete_op_t))
		{
			ps_op->u4_error_code |= 1
				<< FGCR_UNSUPPORTEDPARAM;
			ps_op->u4_error_code |=
				FGCR_OP_API_STRUCT_SIZE_INCORRECT;
			return (IV_FAIL);
		}
	}
	break;

	case FGCR_CMD_VIDEO_CTL:
	{
		UWORD32 *pu4_ptr_cmd;
		UWORD32 sub_command;

		pu4_ptr_cmd = (UWORD32 *)pv_api_ip;
		pu4_ptr_cmd += 2;
		sub_command = *pu4_ptr_cmd;

		switch (sub_command)
		{
		case FGCR_CMD_CTL_SETPARAMS:
		{
			fgcr_ctl_set_config_ip_t *ps_ip;
			fgcr_ctl_set_config_op_t *ps_op;
			ps_ip = (fgcr_ctl_set_config_ip_t *)pv_api_ip;
			ps_op = (fgcr_ctl_set_config_op_t *)pv_api_op;

			if (ps_ip->u4_size
				!= sizeof(fgcr_ctl_set_config_ip_t))
			{
				ps_op->u4_error_code |= 1
					<< FGCR_UNSUPPORTEDPARAM;
				ps_op->u4_error_code |=
					FGCR_IP_API_STRUCT_SIZE_INCORRECT;
				return IV_FAIL;
			}
		}

		break;

		case FGCR_CMD_EXPORT:
		case FGCR_CMD_CTL_SETCODEC:
			break;

		case FGCR_CMD_CTL_GETVERSION:
		{
			fgcr_ctl_getversioninfo_ip_t *ps_ip;
			fgcr_ctl_getversioninfo_op_t *ps_op;
			ps_ip = (fgcr_ctl_getversioninfo_ip_t *)pv_api_ip;
			ps_op = (fgcr_ctl_getversioninfo_op_t *)pv_api_op;
			if (ps_ip->u4_size
				!= sizeof(fgcr_ctl_getversioninfo_ip_t))
			{
				ps_op->u4_error_code |= 1
					<< FGCR_UNSUPPORTEDPARAM;
				ps_op->u4_error_code |=
					FGCR_IP_API_STRUCT_SIZE_INCORRECT;
				return IV_FAIL;
			}
			if (ps_op->u4_size
				!= sizeof(fgcr_ctl_getversioninfo_op_t))
			{
				ps_op->u4_error_code |= 1
					<< FGCR_UNSUPPORTEDPARAM;
				ps_op->u4_error_code |=
					FGCR_OP_API_STRUCT_SIZE_INCORRECT;
				return IV_FAIL;
			}
		}
		break;

		case FGCR_CMD_CTL_SET_FGS_ENABLE_REWRITER:
		case FGCR_CMD_CTL_SET_FGS_OVERIDE_FGC_CANCEL_FLAG:
		case FGCR_CMD_CTL_SET_FGS_OVERIDE_LOG2_SCALE_FACTOR:
		case FGCR_CMD_CTL_SET_FGS_OVERIDE_DEP_COMP_MODEL_VALUES:
		{
			fgcr_ctl_set_fgs_params_ip_t *ps_ip;
			fgcr_ctl_set_fgs_params_op_t *ps_op;

			ps_ip = (fgcr_ctl_set_fgs_params_ip_t *)pv_api_ip;
			ps_op = (fgcr_ctl_set_fgs_params_op_t *)pv_api_op;

			if (ps_ip->u4_size != sizeof(fgcr_ctl_set_fgs_params_ip_t))
			{
				ps_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
				ps_op->u4_error_code |=
					FGCR_IP_API_STRUCT_SIZE_INCORRECT;
				return IV_FAIL;
			}

			if (ps_op->u4_size != sizeof(fgcr_ctl_set_fgs_params_op_t))
			{
				ps_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
				ps_op->u4_error_code |=
					FGCR_OP_API_STRUCT_SIZE_INCORRECT;
				return IV_FAIL;
			}
			break;
		}
		case FGCR_CMD_CTL_SET_FGS_FOR_REWRITE:
			break;
		default:
			*(pu4_api_op + 1) |= 1 << FGCR_UNSUPPORTEDPARAM;
			*(pu4_api_op + 1) |= FGCR_UNSUPPORTED_API_CMD;
			return IV_FAIL;
			break;
		}
	}
	break;
	}

	return IV_SUCCESS;
}


/**************************************************************************
  * \if Function name : fgcr_init_decoder \endif
  *
  *
  * \brief
  *    Initializes the decoder
  *
  * \param apiVersion               : Version of the api being used.
  * \param errorHandlingMechanism   : Mechanism to be used for errror handling.
  * \param postFilteringType: Type of post filtering operation to be used.
  * \param uc_outputFormat: Format of the decoded picture [default 4:2:0].
  * \param uc_dispBufs: Number of Display Buffers.
  * \param p_NALBufAPI: Pointer to NAL Buffer API.
  * \param p_DispBufAPI: Pointer to Display Buffer API.
  * \param ih264d_dec_mem_manager  :Pointer to the function that will be called by decoder
  *                        for memory allocation and freeing.
  *
  * \return
  *    0 on Success and -1 on error
  *
  **************************************************************************
  */
void fgcr_init_decoder(void * ps_dec_params)
{
	dec_struct_t * ps_dec = (dec_struct_t *)ps_dec_params;
	WORD32 size;

	// Initialize all static buffers pointing to any structures to 0

	size = sizeof(sei);
	memset(ps_dec->ps_sei, 0, size);

	size = sizeof(dec_bit_stream_t);
	memset(ps_dec->ps_bitstrm, 0, size);
	memset(ps_dec->ps_upd_bitstrm, 0, size);

	ps_dec->init_done = 0;

	/* Initialize the sei validity u4_flag with zero indiacting sei is not valid*/
	ps_dec->ps_sei->u1_is_valid = 0;

	/* decParams Initializations */
	ps_dec->u1_init_dec_flag = 0;
	ps_dec->i4_header_decoded = 0;

	ps_dec->i4_error_code = 0;

	ps_dec->u2_total_mbs_coded = 0;

	/* ! */
	/* Initializing flush frame u4_flag */
	ps_dec->u1_flushfrm = 0;

	{
		ps_dec->ps_bitstrm->pv_codec_handle = (void*)ps_dec;
	}

	ps_dec->init_done = 1;
}

WORD32 fgcr_free_bufs(iv_obj_t *dec_hdl)
{
	dec_struct_t *ps_dec;

	void(*pf_aligned_free)(void *pv_mem_ctxt, void *pv_buf);
	void *pv_mem_ctxt;

	ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;
	pf_aligned_free = ps_dec->pf_aligned_free;
	pv_mem_ctxt = ps_dec->pv_mem_ctxt;

	PS_DEC_ALIGNED_FREE(ps_dec, ps_dec->ps_sei);
	PS_DEC_ALIGNED_FREE(ps_dec, ps_dec->ps_bitstrm);
	PS_DEC_ALIGNED_FREE(ps_dec, ps_dec->pu1_bits_buf);
	PS_DEC_ALIGNED_FREE(ps_dec, ps_dec->temp_mem_holder);

	PS_DEC_ALIGNED_FREE(ps_dec, dec_hdl->pv_codec_handle);

	if (dec_hdl)
	{
		pf_aligned_free(pv_mem_ctxt, dec_hdl);
	}
	return IV_SUCCESS;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_allocate_bufs			                             */
/*                                                                           */
/*  Description   : Allocates static buffers                                 */
/*                                                                           */
/*  Inputs        :iv_obj_t decoder handle                                   */
/*                :pv_api_ip pointer to input structure                      */
/*                :pv_api_op pointer to output structure                     */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
WORD32 fgcr_allocate_bufs(iv_obj_t **dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	fgcr_create_ip_t *ps_create_ip;
	fgcr_create_op_t *ps_create_op;
	void *pv_buf;
	dec_struct_t *ps_dec;
	void *(*pf_aligned_alloc)(void *pv_mem_ctxt, WORD32 alignment, WORD32 size);
	void(*pf_aligned_free)(void *pv_mem_ctxt, void *pv_buf);
	void *pv_mem_ctxt;
	WORD32 size;

	ps_create_ip = (fgcr_create_ip_t *)pv_api_ip;
	ps_create_op = (fgcr_create_op_t *)pv_api_op;

	ps_create_op->u4_error_code = 0;

	pf_aligned_alloc = ps_create_ip->pf_aligned_alloc;
	pf_aligned_free = ps_create_ip->pf_aligned_free;
	pv_mem_ctxt = ps_create_ip->pv_mem_ctxt;

	/* Initialize return handle to NULL */
	ps_create_op->pv_handle = NULL;
	size = sizeof(iv_obj_t);
	pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
	RETURN_IF((NULL == pv_buf), IV_FAIL);
	*dec_hdl = (iv_obj_t *)pv_buf;
	ps_create_op->pv_handle = *dec_hdl;

	(*dec_hdl)->pv_codec_handle = NULL;
	size = sizeof(dec_struct_t);
	pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
	RETURN_IF((NULL == pv_buf), IV_FAIL);
	memset(pv_buf, 0, size);
	(*dec_hdl)->pv_codec_handle = (dec_struct_t *)pv_buf;
	ps_dec = (dec_struct_t *)pv_buf;

	ps_dec->u4_max_wd = DEFAULT_WD;
	ps_dec->u4_max_ht = DEFAULT_HT;
	ps_dec->u4_max_wd = ALIGN64(ps_dec->u4_max_wd);
	ps_dec->u4_max_ht = ALIGN64(ps_dec->u4_max_ht);

	ps_dec->pf_aligned_alloc = pf_aligned_alloc;
	ps_dec->pf_aligned_free = pf_aligned_free;
	ps_dec->pv_mem_ctxt = pv_mem_ctxt;

	size = sizeof(sei);
	pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
	RETURN_IF((NULL == pv_buf), IV_FAIL);
	ps_dec->ps_sei = (sei *)pv_buf;

	size = sizeof(dec_bit_stream_t);
	pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
	RETURN_IF((NULL == pv_buf), IV_FAIL);
	ps_dec->ps_bitstrm = (dec_bit_stream_t *)pv_buf;

	size = sizeof(dec_bit_stream_t);
	pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
	RETURN_IF((NULL == pv_buf), IV_FAIL);
	ps_dec->ps_upd_bitstrm = (dec_bit_stream_t *)pv_buf;

	{
		UWORD8 chroma_idc = 1;
		UWORD32 ui_size = 0;
		//UWORD8 *pu1_buf;
		WORD32 size;
		void *pv_buf;
		WORD32 i4_pixel_size = 1;
		void *pv_mem_ctxt = ps_dec->pv_mem_ctxt;

		/************************************************************/
		/* Post allocation Initialisations                          */
		/************************************************************/

		/* Post allocation Increment Actions */

		size = sizeof(UWORD8) * ps_dec->u4_max_wd * ps_dec->u4_max_ht *
			i4_pixel_size * ((2 + chroma_idc) / 2);
		ps_dec->u4_bits_buf_size = MAX(256000, size);
		pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, ps_dec->u4_bits_buf_size);
		RETURN_IF((NULL == pv_buf), IV_FAIL);
		ps_dec->pu1_bits_buf = (UWORD8 *)pv_buf;
		memset(ps_dec->pu1_bits_buf, 0, ps_dec->u4_bits_buf_size);

		size = sizeof(UWORD8) * ps_dec->u4_max_wd * ps_dec->u4_max_ht;
		pv_buf = pf_aligned_alloc(pv_mem_ctxt, 128, size);
		RETURN_IF((NULL == pv_buf), IV_FAIL);
		ps_dec->temp_mem_holder = pv_buf;
		memset(ps_dec->temp_mem_holder, 0, size);
	}

	fgcr_init_decoder(ps_dec);

	return IV_SUCCESS;
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_create                                              */
/*                                                                           */
/*  Description   : creates decoder                                          */
/*                                                                           */
/*  Inputs        :iv_obj_t decoder handle                                   */
/*                :pv_api_ip pointer to input structure                      */
/*                :pv_api_op pointer to output structure                     */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_create(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	fgcr_create_op_t *ps_create_op;

	WORD32 ret;

	ps_create_op = (fgcr_create_op_t *)pv_api_op;

	ps_create_op->u4_error_code = 0;

	ret = fgcr_allocate_bufs(&dec_hdl, pv_api_ip, pv_api_op);

	/* If allocation of some buffer fails, then free buffers allocated till then */
	if ((IV_FAIL == ret) && (NULL != dec_hdl))
	{
		fgcr_free_bufs(dec_hdl);
		ps_create_op->u4_error_code = FGCR_MEM_ALLOC_FAILED;
		ps_create_op->u4_error_code = 1 << FGCR_FATALERROR;

		return IV_FAIL;
	}

	return IV_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_map_error                                           */
/*                                                                           */
/*  Description   : Maps error codes to IVD error groups                     */
/*                                                                           */
/*  Inputs        : error status                                             */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
UWORD32 fgcr_map_error(UWORD32 i4_err_status)
{
	UWORD32 temp = 0;

	switch (i4_err_status)
	{
	case FGCR_MEM_ALLOC_FAILED:
		temp = 1 << FGCR_FATALERROR;
		H264_DEC_DEBUG_PRINT("\nFatal Error\n");
		break;

	case ERROR_EOB_FLUSHBITS_T:
		temp = 1 << FGCR_INSUFFICIENTDATA;
		break;
	}

	return temp;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_video_rewrite                                       */
/*                                                                           */
/*  Description   : handle video decode API command                          */
/*                                                                           */
/*  Inputs        : iv_obj_t decoder handle                                  */
/*                : pv_api_ip pointer to input structure                     */
/*                : pv_api_op pointer to output structure                    */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/

IV_API_CALL_STATUS_T fgcr_video_rewrite(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	dec_struct_t * ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);
	WORD32 i4_err_status = 0;
	UWORD8 *pu1_buf = NULL;
	WORD32 buflen;
	UWORD32 u4_max_ofst, u4_length_of_start_code = 0;
	UWORD32 bytes_consumed = 0;
	UWORD32 bytes_consumed_bf_upd = 0;
	UWORD32 cur_slice_is_nonref = 0;
	UWORD32 u4_next_is_aud;
	UWORD32 u4_first_start_code_found = 0;
	WORD32 ret = 0;
	IV_API_CALL_STATUS_T api_ret_value = IV_SUCCESS;
	WORD32 header_data_left = 0, frame_data_left = 0;
	UWORD8 codec = ps_dec->codec;
	UWORD8 *pu1_bitstrm_buf;
	UWORD8 *pu1_upd_bitstrm_buf;
	fgcr_video_rewrite_ip_t    *ps_h264_dec_ip;
	fgcr_video_rewrite_op_t    *ps_h264_dec_op;

	fgcr_video_rewrite_ip_t *ps_dec_ip;
	fgcr_video_rewrite_op_t *ps_dec_op;

	ps_h264_dec_ip = (fgcr_video_rewrite_ip_t *)pv_api_ip;
	ps_h264_dec_op = (fgcr_video_rewrite_op_t *)pv_api_op;

	ps_dec_ip = (fgcr_video_rewrite_ip_t *)ps_h264_dec_ip;
	ps_dec_op = (fgcr_video_rewrite_op_t *)ps_h264_dec_op;

	{
		UWORD32 u4_size;
		u4_size = ps_dec_op->u4_size;
		memset(ps_dec_op, 0, sizeof(fgcr_video_rewrite_op_t));
		ps_dec_op->u4_size = u4_size;
	}

	if (ps_dec->init_done != 1)
	{
		return IV_FAIL;
	}

	if (0 == ps_dec->u1_flushfrm)
	{
		//Checks for new Buffer
		if (ps_dec_ip->pv_stream_buffer == NULL)
		{
			ps_dec_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
			ps_dec_op->u4_error_code |= FGCR_DEC_FRM_BS_BUF_NULL;
			return IV_FAIL;
		}
		if (ps_dec_ip->u4_num_Bytes <= 0)
		{
			ps_dec_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
			ps_dec_op->u4_error_code |= FGCR_DEC_NUMBYTES_INV;
			return IV_FAIL;
		}
	}

	ps_dec->u1_pic_decode_done = 0;

	ps_dec_op->u4_num_bytes_consumed = 0;

	ps_dec_op->u4_num_bytes_generated = 0;

	ps_dec_op->u4_error_code = 0;

	ps_dec->ps_sei->u1_is_valid = 0;

	ps_dec->u4_slice_start_code_found = 0;

	/**
	 * In case the deocder is not in flush mode(in shared mode),
	 * then decoder has to pick up a buffer to write current frame.
	 * Check if a frame is available in such cases
	 */

	if (ps_dec->u1_flushfrm && ps_dec->u1_init_dec_flag)
	{
		/*In the case of flush ,since no frame is decoded set pic type as invalid*/

		return (IV_FAIL);
	}

	ps_dec->u2_total_mbs_coded = 0;
	ps_dec->u4_first_slice_in_pic = 2;

	fgcr_reset_sei(ps_dec);
	ps_dec_op->pv_stream_out_buffer = ps_dec_ip->pv_stream_out_buffer;
	pu1_upd_bitstrm_buf = (UWORD8 *)ps_dec_ip->pv_stream_out_buffer;
	ps_dec->is_fgs_rewrite_succ = 0;
	do
	{
		WORD32 buf_size;

		pu1_buf = (UWORD8*)ps_dec_ip->pv_stream_buffer
			+ ps_dec_op->u4_num_bytes_consumed;

		u4_max_ofst = ps_dec_ip->u4_num_Bytes
			- ps_dec_op->u4_num_bytes_consumed;

		pu1_bitstrm_buf = ps_dec->pu1_bits_buf;
		buf_size = ps_dec->u4_bits_buf_size;

		u4_next_is_aud = 0;

		buflen = ih264d_find_start_code(pu1_buf, 0, u4_max_ofst,
			&u4_length_of_start_code,
			&u4_next_is_aud, codec);

		if (buflen == -1)
		{
			buflen = 0;
		}

		/* Ignore bytes beyond the allocated size of intermediate buffer */
		buflen = MIN(buflen, buf_size);

		bytes_consumed = buflen + u4_length_of_start_code;
		ps_dec_op->u4_num_bytes_consumed += bytes_consumed;
		bytes_consumed_bf_upd = ps_dec_op->u4_num_bytes_generated;

		if (buflen)
		{
			memcpy(pu1_bitstrm_buf, pu1_buf + u4_length_of_start_code,
				buflen);
			memcpy(pu1_upd_bitstrm_buf, pu1_buf, u4_length_of_start_code + buflen);
			pu1_upd_bitstrm_buf += u4_length_of_start_code;
			ps_dec_op->u4_num_bytes_generated += u4_length_of_start_code;
			/* Decoder may read extra 8 bytes near end of the frame */
			if ((buflen + 8) < buf_size)
			{
				memset(pu1_bitstrm_buf + buflen, 0, 8);
			}
			u4_first_start_code_found = 1;
		}
		else
		{
			if (u4_first_start_code_found == 0)
			{
				/*no start codes found in current process call*/

				ps_dec->i4_error_code = ERROR_START_CODE_NOT_FOUND;
				ps_dec_op->u4_error_code |= 1 << FGCR_INSUFFICIENTDATA;

				fgcr_fill_output_struct_from_context(ps_dec, ps_h264_dec_ip, ps_h264_dec_op);

				ps_dec_op->u4_error_code = ps_dec->i4_error_code;

				return (IV_FAIL);
			}
			else
			{
				/* a start code has already been found earlier in the same process call*/
				frame_data_left = 0;
				header_data_left = 0;				//for live streaming
				continue;
			}
		}

		ps_dec_op->pv_stream_out_buffer = ps_dec_ip->pv_stream_out_buffer;

		ret = ih264d_parse_nal_unit_for_rewriter(dec_hdl, ps_dec_op, pu1_bitstrm_buf, &pu1_upd_bitstrm_buf, buflen, u4_length_of_start_code, codec);

		if (ret != OK)
		{
			UWORD32 error = fgcr_map_error(ret);
			ps_dec_op->u4_error_code = error | ret;
			api_ret_value = IV_FAIL;

			if (ret == FGCR_MEM_ALLOC_FAILED)
			{
				ps_dec->u4_slice_start_code_found = 0;
				break;
			}

			if (ret == ERROR_INCOMPLETE_FRAME)
			{
				ps_dec_op->u4_num_bytes_consumed -= bytes_consumed;
				pu1_upd_bitstrm_buf -= (ps_dec_op->u4_num_bytes_generated - bytes_consumed_bf_upd);
				ps_dec_op->u4_num_bytes_generated = bytes_consumed_bf_upd;
				api_ret_value = IV_FAIL;
				break;
			}
		}

		header_data_left = ((ps_dec->i4_decode_header == 1)
			&& (ps_dec->i4_header_decoded != 3)
			&& (ps_dec_op->u4_num_bytes_consumed < ps_dec_ip->u4_num_Bytes));

		frame_data_left = (((ps_dec->i4_decode_header == 0)
			&& ((ps_dec->u1_pic_decode_done == 0)
				|| (u4_next_is_aud == 1)))
			&& (ps_dec_op->u4_num_bytes_consumed < ps_dec_ip->u4_num_Bytes));
	} while ((header_data_left == 1) || (frame_data_left == 1) && (!ps_dec->is_fgs_rewrite_succ));


	H264_DEC_DEBUG_PRINT("The num bytes consumed: %d\n",
		ps_dec_op->u4_num_bytes_consumed);
	return api_ret_value;
}

IV_API_CALL_STATUS_T fgcr_export(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	IV_API_CALL_STATUS_T api_ret_value = IV_SUCCESS;

	dec_struct_t * ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);
	fgcr_ctl_fgc_export_ip_t *ps_h264_dec_ip;
	fgcr_ctl_fgc_export_op_t *ps_h264_dec_op;
	
	ps_h264_dec_ip = (fgcr_ctl_fgc_export_ip_t *)pv_api_ip;
	ps_h264_dec_op = (fgcr_ctl_fgc_export_ip_t *)pv_api_op;


	sei *ps_sei = ps_dec->ps_sei;

	fgcr_ctl_set_fgc_params_t *fgc_export_params;
	fgc_export_params = (fgcr_ctl_set_fgc_params_t *)ps_h264_dec_ip->ps_fgc_export_prms;

	fgc_export_params->u1_film_grain_characteristics_cancel_flag = ps_sei->s_fgc_params.u1_film_grain_characteristics_cancel_flag;
	fgc_export_params->u1_film_grain_model_id = ps_sei->s_fgc_params.u1_film_grain_model_id;
	fgc_export_params->u1_separate_colour_description_present_flag = ps_sei->s_fgc_params.u1_separate_colour_description_present_flag;
	if (fgc_export_params->u1_separate_colour_description_present_flag)
	{
		fgc_export_params->u1_film_grain_bit_depth_luma_minus8 = ps_sei->s_fgc_params.u1_film_grain_bit_depth_luma_minus8;
		fgc_export_params->u1_film_grain_bit_depth_chroma_minus8 = ps_sei->s_fgc_params.u1_film_grain_bit_depth_chroma_minus8;
		fgc_export_params->u1_film_grain_full_range_flag = ps_sei->s_fgc_params.u1_film_grain_full_range_flag;
		fgc_export_params->u1_film_grain_colour_primaries = ps_sei->s_fgc_params.u1_film_grain_colour_primaries;
		fgc_export_params->u1_film_grain_transfer_characteristics = ps_sei->s_fgc_params.u1_film_grain_transfer_characteristics;
		fgc_export_params->u1_film_grain_matrix_coefficients = ps_sei->s_fgc_params.u1_film_grain_matrix_coefficients;
	}
    fgc_export_params->u1_blending_mode_id = ps_sei->s_fgc_params.u1_blending_mode_id;
	fgc_export_params->u1_log2_scale_factor = ps_sei->s_fgc_params.u1_log2_scale_factor;

	for (int c = 0; c < 3; c++)
	{
		fgc_export_params->u1_comp_model_present_flag[c] = ps_sei->s_fgc_params.au1_comp_model_present_flag[c];
	}
	for (int c = 0; c < 3; c++)
	{
		if (fgc_export_params->u1_comp_model_present_flag[c])
		{
			fgc_export_params->u1_num_intensity_intervals_minus1[c] = ps_sei->s_fgc_params.au1_num_intensity_intervals_minus1[c];
			fgc_export_params->u1_num_model_values_minus1[c] = ps_sei->s_fgc_params.au1_num_model_values_minus1[c];
			for (int i = 0; i <= fgc_export_params->u1_num_intensity_intervals_minus1[c]; i++)
			{
				fgc_export_params->u1_intensity_interval_lower_bound[c][i] = ps_sei->s_fgc_params.au1_intensity_interval_lower_bound[c][i];
				fgc_export_params->u1_intensity_interval_upper_bound[c][i] = ps_sei->s_fgc_params.au1_intensity_interval_upper_bound[c][i];
				for(int j=0; j<=fgc_export_params->u1_num_model_values_minus1[c]; j++)\
				{
					fgc_export_params->u4_comp_model_value[c][i][j] = ps_sei->s_fgc_params.ai4_comp_model_value[c][i][j];
				}
			}
		}
	}

	fgc_export_params->u1_film_grain_characteristics_persistence_flag = ps_sei->s_fgc_params.u1_film_grain_characteristics_persistence_flag;
	

	return api_ret_value;
	
/////////////////////////////////////////////////////////////////////////////////	
//	dec_struct_t * ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);
//	WORD32 i4_err_status = 0;
//	UWORD8 *pu1_buf = NULL;
//	WORD32 buflen;
//	UWORD32 u4_max_ofst, u4_length_of_start_code = 0;
//	UWORD32 bytes_consumed = 0;
//	UWORD32 bytes_consumed_bf_upd = 0;
//	UWORD32 cur_slice_is_nonref = 0;
//	UWORD32 u4_next_is_aud;
//	UWORD32 u4_first_start_code_found = 0;
//	WORD32 ret = 0;
//	IV_API_CALL_STATUS_T api_ret_value = IV_SUCCESS;
//	WORD32 header_data_left = 0, frame_data_left = 0;
//	UWORD8 codec = ps_dec->codec;
//	UWORD8 *pu1_bitstrm_buf;
//	UWORD8 *pu1_upd_bitstrm_buf;
//	fgcr_video_rewrite_ip_t    *ps_h264_dec_ip;
//	fgcr_video_rewrite_op_t    *ps_h264_dec_op;
//
//	fgcr_video_rewrite_ip_t *ps_dec_ip;
//	fgcr_video_rewrite_op_t *ps_dec_op;
//
//	ps_h264_dec_ip = (fgcr_video_rewrite_ip_t *)pv_api_ip;
//	ps_h264_dec_op = (fgcr_video_rewrite_op_t *)pv_api_op;
//
//	ps_dec_ip = (fgcr_video_rewrite_ip_t *)ps_h264_dec_ip;
//	ps_dec_op = (fgcr_video_rewrite_op_t *)ps_h264_dec_op;
//
//	{
//		UWORD32 u4_size;
//		u4_size = ps_dec_op->u4_size;
//		memset(ps_dec_op, 0, sizeof(fgcr_video_rewrite_op_t));
//		ps_dec_op->u4_size = u4_size;
//	}
//
//	if (ps_dec->init_done != 1)
//	{
//		return IV_FAIL;
//	}
//
//	if (0 == ps_dec->u1_flushfrm)
//	{
//		//Checks for new Buffer
//		if (ps_dec_ip->pv_stream_buffer == NULL)
//		{
//			ps_dec_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
//			ps_dec_op->u4_error_code |= FGCR_DEC_FRM_BS_BUF_NULL;
//			return IV_FAIL;
//		}
//		if (ps_dec_ip->u4_num_Bytes <= 0)
//		{
//			ps_dec_op->u4_error_code |= 1 << FGCR_UNSUPPORTEDPARAM;
//			ps_dec_op->u4_error_code |= FGCR_DEC_NUMBYTES_INV;
//			return IV_FAIL;
//		}
//	}
//
//	ps_dec->u1_pic_decode_done = 0;
//
//	ps_dec_op->u4_num_bytes_consumed = 0;
//
//	ps_dec_op->u4_num_bytes_generated = 0;
//
//	ps_dec_op->u4_error_code = 0;
//
//	ps_dec->ps_sei->u1_is_valid = 0;
//
//	ps_dec->u4_slice_start_code_found = 0;
//
//	/**
//	 * In case the deocder is not in flush mode(in shared mode),
//	 * then decoder has to pick up a buffer to write current frame.
//	 * Check if a frame is available in such cases
//	 */
//
//	if (ps_dec->u1_flushfrm && ps_dec->u1_init_dec_flag)
//	{
//		/*In the case of flush ,since no frame is decoded set pic type as invalid*/
//
//		return (IV_FAIL);
//	}
//
//	ps_dec->u2_total_mbs_coded = 0;
//	ps_dec->u4_first_slice_in_pic = 2;
//
//	fgcr_reset_sei(ps_dec);
//	ps_dec_op->pv_stream_out_buffer = ps_dec_ip->pv_stream_out_buffer;
//	pu1_upd_bitstrm_buf = (UWORD8 *)ps_dec_ip->pv_stream_out_buffer;
//	ps_dec->is_fgs_rewrite_succ = 0;
//	do
//	{
//		WORD32 buf_size;
//
//		pu1_buf = (UWORD8*)ps_dec_ip->pv_stream_buffer
//			+ ps_dec_op->u4_num_bytes_consumed;
//
//		u4_max_ofst = ps_dec_ip->u4_num_Bytes
//			- ps_dec_op->u4_num_bytes_consumed;
//
//		pu1_bitstrm_buf = ps_dec->pu1_bits_buf;
//		buf_size = ps_dec->u4_bits_buf_size;
//
//		u4_next_is_aud = 0;
//
//		buflen = ih264d_find_start_code(pu1_buf, 0, u4_max_ofst,
//			&u4_length_of_start_code,
//			&u4_next_is_aud, codec);
//
//		if (buflen == -1)
//		{
//			buflen = 0;
//		}
//
//		/* Ignore bytes beyond the allocated size of intermediate buffer */
//		buflen = MIN(buflen, buf_size);
//
//		bytes_consumed = buflen + u4_length_of_start_code;
//		ps_dec_op->u4_num_bytes_consumed += bytes_consumed;
//		bytes_consumed_bf_upd = ps_dec_op->u4_num_bytes_generated;
//
//		if (buflen)
//		{
//			memcpy(pu1_bitstrm_buf, pu1_buf + u4_length_of_start_code,
//				buflen);
//			memcpy(pu1_upd_bitstrm_buf, pu1_buf, u4_length_of_start_code + buflen);
//			pu1_upd_bitstrm_buf += u4_length_of_start_code;
//			ps_dec_op->u4_num_bytes_generated += u4_length_of_start_code;
//			/* Decoder may read extra 8 bytes near end of the frame */
//			if ((buflen + 8) < buf_size)
//			{
//				memset(pu1_bitstrm_buf + buflen, 0, 8);
//			}
//			u4_first_start_code_found = 1;
//		}
//		else
//		{
//			if (u4_first_start_code_found == 0)
//			{
//				/*no start codes found in current process call*/
//
//				ps_dec->i4_error_code = ERROR_START_CODE_NOT_FOUND;
//				ps_dec_op->u4_error_code |= 1 << FGCR_INSUFFICIENTDATA;
//
//				fgcr_fill_output_struct_from_context(ps_dec, ps_h264_dec_ip, ps_h264_dec_op);
//
//				ps_dec_op->u4_error_code = ps_dec->i4_error_code;
//
//				return (IV_FAIL);
//			}
//			else
//			{
//				/* a start code has already been found earlier in the same process call*/
//				frame_data_left = 0;
//				header_data_left = 0;				//for live streaming
//				continue;
//			}
//		}
//
//		ps_dec_op->pv_stream_out_buffer = ps_dec_ip->pv_stream_out_buffer;
//
//		ret = ih264d_parse_nal_unit_for_fgc_export(dec_hdl, ps_dec_op, pu1_bitstrm_buf, &pu1_upd_bitstrm_buf, buflen, u4_length_of_start_code, codec);
//
//		if (ret != OK)
//		{
//			UWORD32 error = fgcr_map_error(ret);
//			ps_dec_op->u4_error_code = error | ret;
//			api_ret_value = IV_FAIL;
//
//			if (ret == FGCR_MEM_ALLOC_FAILED)
//			{
//				ps_dec->u4_slice_start_code_found = 0;
//				break;
//			}
//
//			if (ret == ERROR_INCOMPLETE_FRAME)
//			{
//				ps_dec_op->u4_num_bytes_consumed -= bytes_consumed;
//				pu1_upd_bitstrm_buf -= (ps_dec_op->u4_num_bytes_generated - bytes_consumed_bf_upd);
//				ps_dec_op->u4_num_bytes_generated = bytes_consumed_bf_upd;
//				api_ret_value = IV_FAIL;
//				break;
//			}
//		}
//
//		header_data_left = ((ps_dec->i4_decode_header == 1)
//			&& (ps_dec->i4_header_decoded != 3)
//			&& (ps_dec_op->u4_num_bytes_consumed < ps_dec_ip->u4_num_Bytes));
//
//		frame_data_left = (((ps_dec->i4_decode_header == 0)
//			&& ((ps_dec->u1_pic_decode_done == 0)
//				|| (u4_next_is_aud == 1)))
//			&& (ps_dec_op->u4_num_bytes_consumed < ps_dec_ip->u4_num_Bytes));
//	} while ((header_data_left == 1) || (frame_data_left == 1) && (!ps_dec->is_fgs_rewrite_succ));
//
//
//	H264_DEC_DEBUG_PRINT("The num bytes consumed: %d\n",
//		ps_dec_op->u4_num_bytes_consumed);
//	return api_ret_value;
}


IV_API_CALL_STATUS_T fgcr_get_version(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	char version_string[MAXVERSION_STRLEN + 1];
	UWORD32 version_string_len;

	fgcr_ctl_getversioninfo_ip_t *ps_ip;
	fgcr_ctl_getversioninfo_op_t *ps_op;

	ps_ip = (fgcr_ctl_getversioninfo_ip_t *)pv_api_ip;
	ps_op = (fgcr_ctl_getversioninfo_op_t *)pv_api_op;
	UNUSED(dec_hdl);
	ps_op->u4_error_code = IV_SUCCESS;

	VERSION(version_string, CODEC_NAME, CODEC_RELEASE_TYPE, CODEC_RELEASE_VER,
		CODEC_VENDOR);

	if ((WORD32)ps_ip->u4_version_buffer_size <= 0)
	{
		ps_op->u4_error_code = ERROR_VERS_BUF_INSUFFICIENT;
		return (IV_FAIL);
	}

	version_string_len = (UWORD32)strnlen(version_string, MAXVERSION_STRLEN) + 1;

	if (ps_ip->u4_version_buffer_size >= version_string_len)
	{
		memcpy(ps_ip->pv_version_buffer, version_string, version_string_len);
		ps_op->u4_error_code = IV_SUCCESS;
	}
	else
	{
		ps_op->u4_error_code = ERROR_VERS_BUF_INSUFFICIENT;
		return IV_FAIL;
	}
	return (IV_SUCCESS);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_set_params                                          */
/*                                                                           */
/*  Description   :                                                          */
/*                                                                           */
/*  Inputs        : iv_obj_t decoder handle                                  */
/*                : pv_api_ip pointer to input structure                     */
/*                : pv_api_op pointer to output structure                    */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_set_params(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	dec_struct_t * ps_dec;
	IV_API_CALL_STATUS_T ret = IV_SUCCESS;

	fgcr_ctl_set_config_ip_t *ps_ctl_ip = (fgcr_ctl_set_config_ip_t *)pv_api_ip;
	fgcr_ctl_set_config_op_t *ps_ctl_op = (fgcr_ctl_set_config_op_t *)pv_api_op;

	ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);

	ps_ctl_op->u4_error_code = 0;

	ps_dec->i4_decode_header = 0;

	return ret;
}

IV_API_CALL_STATUS_T fgcr_set_codec(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	dec_struct_t * ps_dec;
	IV_API_CALL_STATUS_T ret = IV_SUCCESS;

	fgcr_ctl_set_codec_ip_t *ps_ctl_ip = (fgcr_ctl_set_codec_ip_t *)pv_api_ip;
	fgcr_ctl_set_codec_op_t *ps_ctl_op = (fgcr_ctl_set_codec_op_t *)pv_api_op;

	ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);

	ps_dec->codec = ps_ctl_ip->u1_codec;

	return ret;
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_delete                                              */
/*                                                                           */
/*  Description   :                                                          */
/*                                                                           */
/*  Inputs        : iv_obj_t decoder handle                                  */
/*                : pv_api_ip pointer to input structure                     */
/*                : pv_api_op pointer to output structure                    */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_delete(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	fgcr_delete_ip_t *ps_ip = (fgcr_delete_ip_t *)pv_api_ip;
	fgcr_delete_op_t *ps_op = (fgcr_delete_op_t *)pv_api_op;

	UNUSED(ps_ip);
	ps_op->u4_error_code = 0;

	fgcr_free_bufs(dec_hdl);

	return IV_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name :  fgcr_ctl                                                */
/*                                                                           */
/*  Description   :                                                          */
/*                                                                           */
/*  Inputs        : iv_obj_t decoder handle                                  */
/*                : pv_api_ip pointer to input structure                     */
/*                : pv_api_op pointer to output structure                    */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_ctl(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
	fgcr_ctl_set_config_ip_t *ps_ctl_ip;
	fgcr_ctl_set_config_op_t *ps_ctl_op;
	IV_API_CALL_STATUS_T ret = IV_SUCCESS;
	UWORD32 subcommand;
	dec_struct_t *ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;

	if (ps_dec->init_done != 1)
	{
		//Return proper Error Code
		return IV_FAIL;
	}
	ps_ctl_ip = (fgcr_ctl_set_config_ip_t*)pv_api_ip;
	ps_ctl_op = (fgcr_ctl_set_config_op_t*)pv_api_op;
	ps_ctl_op->u4_error_code = 0;
	subcommand = ps_ctl_ip->e_sub_cmd;

	switch (subcommand)
	{
	case FGCR_CMD_CTL_SETPARAMS:
		ret = fgcr_set_params(dec_hdl, (void *)pv_api_ip, (void *)pv_api_op);
		break;
	case FGCR_CMD_CTL_SETCODEC:
		ret = fgcr_set_codec(dec_hdl, (void *)pv_api_ip, (void *)pv_api_op);
		break;
	case FGCR_CMD_CTL_GETVERSION:
		ret = fgcr_get_version(dec_hdl, (void *)pv_api_ip, (void *)pv_api_op);
		break;
	case FGCR_CMD_CTL_SET_FGS_ENABLE_REWRITER:
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_FGC_CANCEL_FLAG:
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_LOG2_SCALE_FACTOR:
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_DEP_COMP_MODEL_VALUES:
		ret = fgcr_set_fgs_params(dec_hdl, (void *)pv_api_ip, (void *)pv_api_op);
		break;
	case FGCR_CMD_CTL_SET_FGS_FOR_REWRITE:
		ret = fgcr_set_fgs_rewrite_params(dec_hdl, (void *)pv_api_ip, (void *)pv_api_op);
		break;
	default:
		H264_DEC_DEBUG_PRINT("\ndo nothing\n");
		break;
	}

	return ret;
}

/**
 *******************************************************************************
 *
 * @brief
 *  Sets degrade params
 *
 * @par Description:
 *  Sets FGS params.
 *  Refer to fgcr_ctl_set_fgs_params_ip_t definition for details
 *
 * @param[in] ps_codec_obj
 *  Pointer to codec object at API level
 *
 * @param[in] pv_api_ip
 *  Pointer to input argument structure
 *
 * @param[out] pv_api_op
 *  Pointer to output argument structure
 *
 * @returns  Status
 *
 * @remarks
 *
 *
 *******************************************************************************
 */

IV_API_CALL_STATUS_T fgcr_set_fgs_params(
	iv_obj_t *dec_hdl,
	void *pv_api_ip,
	void *pv_api_op) {

	fgcr_ctl_set_fgs_params_ip_t *ps_ip;
	fgcr_ctl_set_fgs_params_op_t *ps_op;
	dec_struct_t *ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;

	ps_ip = (fgcr_ctl_set_fgs_params_ip_t *)pv_api_ip;
	ps_op = (fgcr_ctl_set_fgs_params_op_t *)pv_api_op;

	ps_op->u4_error_code = ERROR_UNSUPPORTED_SEI_FGS_PARAMS;

	switch ((FGCR_CMD_CTL_SUB_CMDS)ps_ip->e_sub_cmd)
	{
	case FGCR_CMD_CTL_SET_FGS_ENABLE_REWRITER:
		if ((0 == ps_ip->u4_param_value) ||
			(1 == ps_ip->u4_param_value)) {
			ps_dec->u4_fgs_enable_rewriter = ps_ip->u4_param_value;
			ps_op->u4_error_code = 0;
		}
		else
		{
			return ERROR_UNSUPPORTED_SEI_FGS_PARAMS;
		}
		break;
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_FGC_CANCEL_FLAG:
		if ((0 == ps_ip->u4_param_value) ||
			(1 == ps_ip->u4_param_value)) {
			ps_dec->u4_fgs_overide_fgc_cancel_flag = ps_ip->u4_param_value;
			ps_op->u4_error_code = 0;
		}
		else
		{
			return ERROR_UNSUPPORTED_SEI_FGS_PARAMS;
		}
		break;
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_LOG2_SCALE_FACTOR:
		if ((0 == ps_ip->u4_param_value) ||
			(1 == ps_ip->u4_param_value)) {
			ps_dec->u4_fgs_overide_log2_scale_factor = ps_ip->u4_param_value;
			ps_op->u4_error_code = 0;
		}
		else
		{
			return ERROR_UNSUPPORTED_SEI_FGS_PARAMS;
		}
		break;
	case FGCR_CMD_CTL_SET_FGS_OVERIDE_DEP_COMP_MODEL_VALUES:
		if ((0 == ps_ip->u4_param_value) ||
			(1 == ps_ip->u4_param_value)) {
			ps_dec->u4_fgs_overide_dep_comp_model_values = ps_ip->u4_param_value;
			ps_op->u4_error_code = 0;
		}
		else
		{
			return ERROR_UNSUPPORTED_SEI_FGS_PARAMS;
		}
		break;
	default:
		break;
	}
	return IV_SUCCESS;
}

WORD32 error_check_FGS(fgcr_set_fgc_params_t *ps_fgc_param)
{
	{
		UWORD8 numComp = MAX_NUM_COMP; /* number of color components */
		UWORD8 compCtr, intensityCtr, multiGrainCheck[MAX_NUM_COMP][MAX_NUM_INTENSITIES] = { 0 };
		UWORD16 multiGrainCtr;
		UWORD8 limitCompModelVal1[10] = { 0 }, limitCompModelVal2[10] = { 0 };
		UWORD8 num_comp_model_pairs = 0, limitCompModelCtr, compPairMatch;

		if ((0 != ps_fgc_param->u1_film_grain_characteristics_cancel_flag) &&
			(1 != ps_fgc_param->u1_film_grain_characteristics_cancel_flag))
		{
			printf("Error Film Grain Characteristc Cancel Flag invalid value");
			return IV_FAIL;
		}

		if (FILM_GRAIN_MODEL_ID_VALUE != ps_fgc_param->u1_film_grain_model_id)
		{
			printf("Error Film Grain Model ID invalid value");
			return IV_FAIL;
		}

		if (0 != ps_fgc_param->u1_separate_colour_description_present_flag)
		{
			printf("Error separate_colour_description_present_flag invalid value");
			return IV_FAIL;
		}

		if (BLENDING_MODE_VALUE != ps_fgc_param->u1_blending_mode_id)
		{
			printf("Error blending_mode_id invalid value");
			return IV_FAIL;
		}

		if ((ps_fgc_param->u1_log2_scale_factor < MIN_LOG2SCALE_VALUE) ||
			(ps_fgc_param->u1_log2_scale_factor > MAX_LOG2SCALE_VALUE))
		{
			printf("Error log2_scale_factor invalid value");
			return IV_FAIL;
		}

		/* validation of component model present flag */
		for (compCtr = 0; compCtr < numComp; compCtr++)
		{
			if ((ps_fgc_param->u1_comp_model_present_flag[compCtr] != 0) &&
				(ps_fgc_param->u1_comp_model_present_flag[compCtr] != 1))
			{
				printf("Error comp_model_present_flag invalid value");
				return IV_FAIL;
			}
			if (ps_fgc_param->u1_comp_model_present_flag[compCtr] &&
				(ps_fgc_param->u1_num_model_values_minus1[compCtr] > MAX_ALLOWED_MODEL_VALUES_MINUS1))
			{
				printf("Error num_model_values_minus1 invalid value");
				return IV_FAIL;
			}
		}

		/* validation of intensity intervals and  */
		for (compCtr = 0; compCtr < numComp; compCtr++)
		{
			if (1 == ps_fgc_param->u1_comp_model_present_flag[compCtr])
			{
				for (intensityCtr = 0; intensityCtr <= ps_fgc_param->u1_num_intensity_intervals_minus1[compCtr]; intensityCtr++)
				{
					if (ps_fgc_param->u1_intensity_interval_lower_bound[compCtr][intensityCtr] >
						ps_fgc_param->u1_intensity_interval_upper_bound[compCtr][intensityCtr])
					{
						printf("Error: Invalid value for intensity_interval_lower_bound & intensity_interval_upper_bound");
						return IV_FAIL;
					}

					for (multiGrainCtr = ps_fgc_param->u1_intensity_interval_lower_bound[compCtr][intensityCtr];
						multiGrainCtr <= ps_fgc_param->u1_intensity_interval_upper_bound[compCtr][intensityCtr]; multiGrainCtr++)
					{
						if (multiGrainCheck[compCtr][multiGrainCtr]) /* Non over lap */
						{
							printf("Error: Invalid value for intensity_interval_lower_bound & intensity_interval_upper_bound");
							return IV_FAIL;
						}
						else
						{
							multiGrainCheck[compCtr][multiGrainCtr] = 1;
						}
					}

					/* Error check on model component value */
					if (ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][0] >= (1 << 8))
					{
						printf("Error: Invalid value for standard deviation (comp_model_value[0])");
						return IV_FAIL;
					}
					else if (((ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][1] < MIN_CUT_OFF_FREQUENCY) ||
						(ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][1] > MAX_CUT_OFF_FREQUENCY)) &&
						(ps_fgc_param->u1_num_model_values_minus1[compCtr] > 0))
					{
						printf("Error: Invalid value for cut off frequencies");
						return IV_FAIL;
					}
					else if (((ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][2] < MIN_CUT_OFF_FREQUENCY) ||
						(ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][2] > MAX_CUT_OFF_FREQUENCY)) &&
						(ps_fgc_param->u1_num_model_values_minus1[compCtr] > 1))
					{
						printf("Error: Invalid value for cut off frequencies");
						return IV_FAIL;
					}

					compPairMatch = 0;
					for (limitCompModelCtr = 0; limitCompModelCtr <= num_comp_model_pairs; limitCompModelCtr++)
					{
						if ((limitCompModelVal1[limitCompModelCtr] ==
							ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][1]) &&
							(limitCompModelVal2[limitCompModelCtr] ==
								ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][2]))
						{
							compPairMatch = 1;
						}
					}

					if (0 == compPairMatch)
					{
						num_comp_model_pairs++;
						/* max allowed pairs are 10 as per SMPTE -RDD5*/
						if (num_comp_model_pairs > MAX_ALLOWED_COMP_MODEL_PAIRS)
						{
							printf("Error: Invalid value for cut off frequencies");
							return IV_FAIL;
						}
						limitCompModelVal1[num_comp_model_pairs - 1] =
							ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][1];
						limitCompModelVal2[num_comp_model_pairs - 1] =
							ps_fgc_param->u4_comp_model_value[compCtr][intensityCtr][2];
					}
				}
			}
		}

		//if (0 != ps_fgc_param->u4_film_grain_characteristics_repetition_period)
		//{
		//	printf("Error: Invalid value for film_grain_characteristics_repetition_period");
		//	return IV_FAIL;
		//}
	}
	return 0;
}

IV_API_CALL_STATUS_T fgcr_set_fgs_rewrite_params(
	iv_obj_t *dec_hdl,
	void *pv_api_ip,
	void *pv_api_op)
{
	fgcr_ctl_fgs_rewrite_params_ip_t *ps_ip;
	fgcr_ctl_set_fgs_params_op_t     *ps_op;
	dec_struct_t *ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;
	WORD32 i_status = IV_SUCCESS;
	ps_ip = (fgcr_ctl_fgs_rewrite_params_ip_t *)pv_api_ip;
	ps_op = (fgcr_ctl_set_fgs_params_op_t *)pv_api_op;

	ps_op->u4_error_code = ERROR_UNSUPPORTED_SEI_FGS_REWRITE_PARAMS;
	switch ((FGCR_CMD_CTL_SUB_CMDS)ps_ip->e_sub_cmd)
	{
	case FGCR_CMD_CTL_SET_FGS_FOR_REWRITE:
	{
		ps_dec->s_fgs_rewrite_prms = (fgcr_set_fgc_params_t*)ps_ip->ps_fgs_rewrite_prms;
		i_status = error_check_FGS(ps_dec->s_fgs_rewrite_prms);
		ps_op->u4_error_code = i_status;
	}
		break;
	default:
		break;
	}
	return i_status;
}

void fgcr_fill_output_struct_from_context(
	dec_struct_t *ps_dec,
	fgcr_video_rewrite_ip_t *ps_h264_dec_ip,
	fgcr_video_rewrite_op_t *ps_h264_dec_op)
{
	fgcr_video_rewrite_ip_t *ps_dec_ip = (fgcr_video_rewrite_ip_t *)ps_h264_dec_ip;
	fgcr_video_rewrite_op_t *ps_dec_op = (fgcr_video_rewrite_op_t *)ps_h264_dec_op;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : fgcr_api_function                                        */
/*                                                                           */
/*  Description   :                                                          */
/*                                                                           */
/*  Inputs        : iv_obj_t decoder handle                                  */
/*                : pv_api_ip pointer to input structure                     */
/*                : pv_api_op pointer to output structure                    */
/*  Outputs       :                                                          */
/*  Returns       : void                                                     */
/*                                                                           */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_api_function(
	iv_obj_t *dec_hdl,
	void *pv_api_ip,
	void *pv_api_op)
{
	UWORD32 command;
	UWORD32 *pu2_ptr_cmd;
	IV_API_CALL_STATUS_T u4_api_ret;
	IV_API_CALL_STATUS_T e_status;
	e_status = api_check_struct_sanity(dec_hdl, pv_api_ip, pv_api_op);

	if (e_status != IV_SUCCESS)
	{
		UWORD32 *ptr_err;

		ptr_err = (UWORD32 *)pv_api_op;
		UNUSED(ptr_err);
		H264_DEC_DEBUG_PRINT("error code = %d\n", *(ptr_err + 1));
		return IV_FAIL;
	}

	pu2_ptr_cmd = (UWORD32 *)pv_api_ip;
	pu2_ptr_cmd++;

	command = *pu2_ptr_cmd;
	switch (command)
	{
	case FGCR_CMD_CREATE:
		u4_api_ret = fgcr_create(dec_hdl, (void *)pv_api_ip,
			(void *)pv_api_op);
		break;

	case FGCR_CMD_DELETE:
		u4_api_ret = fgcr_delete(dec_hdl, (void *)pv_api_ip,
			(void *)pv_api_op);
		break;

	case FGCR_CMD_VIDEO_REWRITE:
		// TODO add new function here  u4_api_ret = 1;
		u4_api_ret = fgcr_video_rewrite(dec_hdl, (void *)pv_api_ip,
			(void *)pv_api_op);
		break;

	case FGCR_CMD_EXPORT:
		u4_api_ret = fgcr_export(dec_hdl, (void *)pv_api_ip,
			(void *)pv_api_op);
		break;

	case FGCR_CMD_VIDEO_CTL:
		u4_api_ret = fgcr_ctl(dec_hdl, (void *)pv_api_ip,
			(void *)pv_api_op);
		break;

	default:
		u4_api_ret = IV_FAIL;
		break;
	}

	return u4_api_ret;
}
