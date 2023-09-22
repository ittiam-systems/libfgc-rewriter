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
/*  File Name         : fgcr.h                                             */
/*                                                                           */
/*  Description       : This file contains all the necessary structure and   */
/*                      enumeration definitions needed for the Application   */
/*                      Program Interface(API) of the Ittiam H264 ASP        */
/*                      Decoder on Cortex A8 - Neon platform                 */
/*                                                                           */
/*  List of Functions : fgcr_api_function                                    */
/*                                                                           */
/*****************************************************************************/

#ifndef _FGCR_H_
#define _FGCR_H_
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/


/* codec type */
#define AVC 1
#define HEVC 2

/*SEI FGC relaeted macros*/
#define SEI_FGC_NUM_COLOUR_COMPONENTS 3
#define SEI_FGC_MAX_NUM_MODEL_VALUES 6
#define SEI_FGC_MAX_NUM_INTENSITY_INTERVALS 256

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/


/* IV_API_CALL_STATUS_T:This is only to return the FAIL/PASS status to the  */
/* application for the current API call                                     */

typedef enum {
    IV_STATUS_NA                                = 0x7FFFFFFF,
    IV_SUCCESS                                  = 0x0,
    IV_FAIL                                     = 0x1,
}IV_API_CALL_STATUS_T;


/*****************************************************************************/
/* Structure                                                                 */
/*****************************************************************************/

/* IV_OBJ_T: This structure defines the handle for the codec instance        */

typedef struct {
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * Pointer to the API function pointer table of the codec
     */
    void                                        *pv_fxns;

    /**
     * Pointer to the handle of the codec
     */
    void                                        *pv_codec_handle;
}iv_obj_t;

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
//FGS parameters
#define MAX_KEYWORD_LEN                   512
#define MAX_NUM_COMP                      3
#define MAX_NUM_INTENSITIES               256
#define MAX_NUM_MODEL_VALUES              6
#define MAX_ALLOWED_MODEL_VALUES_MINUS1 2
#define MIN_LOG2SCALE_VALUE 2
#define MAX_LOG2SCALE_VALUE 7
#define FILM_GRAIN_MODEL_ID_VALUE 0
#define BLENDING_MODE_VALUE 0
#define MAX_STANDARD_DEVIATION 255
#define MIN_CUT_OFF_FREQUENCY 2
#define MAX_CUT_OFF_FREQUENCY 14
#define DEFAULT_HORZ_CUT_OFF_FREQUENCY 8
#define MAX_ALLOWED_COMP_MODEL_PAIRS 10

/*****************************************************************************/
/* API Function Prototype                                                    */
/*****************************************************************************/
IV_API_CALL_STATUS_T fgcr_api_function(iv_obj_t *ps_handle, void *pv_api_ip,void *pv_api_op);

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

/* FGCR_API_COMMAND_TYPE_T:API command type                                   */
typedef enum {
    FGCR_CMD_VIDEO_NA                          = 0x7FFFFFFF,
    FGCR_CMD_CREATE                            = 1,
    FGCR_CMD_DELETE,
    FGCR_CMD_VIDEO_CTL,
    FGCR_CMD_VIDEO_REWRITE,
    FGCR_CMD_EXPORT
}FGCR_API_COMMAND_TYPE_T;

/* FGCR_CONTROL_API_COMMAND_TYPE_T: Video Control API command type            */

typedef enum {
    FGCR_CMD_NA                          = 0x7FFFFFFF,
    FGCR_CMD_CTL_SETPARAMS               = 0x1,
    FGCR_CMD_CTL_GETVERSION              = 0x6,
    FGCR_CMD_CTL_CODEC_SUBCMD_START      = 0x7
}FGCR_CONTROL_API_COMMAND_TYPE_T;

/* FGCR_ERROR_BITS_T: A UWORD32 container will be used for reporting the error*/
/* code to the application. The first 8 bits starting from LSB have been     */
/* reserved for the codec to report internal error details. The rest of the  */
/* bits will be generic and each bit has an associated meaning as mentioned  */
/* below. The unused bit fields are reserved for future extenstions and will */
/* be zero in the current implementation                */

typedef enum {
    /* Bit 9 - Insufficient input data.                                     */
    FGCR_INSUFFICIENTDATA                        = 0x9,
    /* Bit 13 - Unsupported input parameter orconfiguration.                 */
    FGCR_UNSUPPORTEDPARAM                        = 0xd,
    /* Bit 14 - Fatal error (stop the codec).If there is an                  */
    /* error and this bit is not set, the error is a recoverable one.        */
    FGCR_FATALERROR                              = 0xe,
    FGCR_ERROR_BITS_T_DUMMY_ELEMENT              = 0x7FFFFFFF
}FGCR_ERROR_BITS_T;

/* FGCR_CONTROL_API_COMMAND_TYPE_T: Video Control API command type            */
typedef enum {
    FGCR_IP_API_STRUCT_SIZE_INCORRECT            = 0x13,
    FGCR_OP_API_STRUCT_SIZE_INCORRECT            = 0x14,
    FGCR_HANDLE_NULL                             = 0x15,
    FGCR_HANDLE_STRUCT_SIZE_INCORRECT            = 0x16,
    FGCR_INVALID_HANDLE_NULL                     = 0x17,
    FGCR_INVALID_API_CMD                         = 0x18,
    FGCR_UNSUPPORTED_API_CMD                     = 0x19,
    FGCR_DEC_FRM_BS_BUF_NULL                     = 0x1e,
    FGCR_DEC_FRM_TEMP_BUF_NULL                   = 0x1f,
    FGCR_DEC_NUMBYTES_INV                        = 0x27,
    FGCR_MEM_ALLOC_FAILED                        = 0x2b,
    FGCR_DUMMY_ELEMENT_FOR_EXTENSIONS            = 0xD0,
}FGCR_ERROR_CODES_T;

/*****************************************************************************/
/*  Delete Rewriter                                                          */
/*****************************************************************************/

typedef struct {
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * cmd
     */
    FGCR_API_COMMAND_TYPE_T                       e_cmd;

}fgcr_delete_ip_t;

typedef struct{
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * error_code
     */
    UWORD32                                     u4_error_code;

}fgcr_delete_op_t;

/*****************************************************************************/
/*   Initialize Rewriter                                                     */
/*****************************************************************************/

typedef struct {
    /**
     * u4_size of the structure
     */
    UWORD32                                 u4_size;

    /**
     *  e_cmd
     */
    FGCR_API_COMMAND_TYPE_T                  e_cmd;

    /**
     * Pointer to a function for aligned allocation.
     */
    void    *(*pf_aligned_alloc)(void *pv_mem_ctxt, WORD32 alignment, WORD32 size);

    /**
     * Pointer to a function for aligned free.
     */
    void   (*pf_aligned_free)(void *pv_mem_ctxt, void *pv_buf);

    /**
     * Pointer to memory context that is needed during alloc/free for custom
     * memory managers. This will be passed as first argument to pf_aligned_alloc and
     * pf_aligned_free.
     * If application is using standard memory functions like
     * malloc/aligned_malloc/memalign/free/aligned_free,
     * then this is not needed and can be set to NULL
     */
    void    *pv_mem_ctxt;

}fgcr_create_ip_t;

typedef struct{
	/**
	 * u4_size of the structure
	 */
	UWORD32                                 u4_size;

	/**
	 * u4_error_code
	 */
	UWORD32                                 u4_error_code;

	/**
	 * Codec Handle
	 */
	void                                    *pv_handle;
}fgcr_create_op_t;

/*****************************************************************************/
/*   Video Rewrite                                                           */
/*****************************************************************************/

typedef struct {
    /**
     * u4_size of the structure
     */
    UWORD32                                 u4_size;

    /**
     * e_cmd
     */
    FGCR_API_COMMAND_TYPE_T                  e_cmd;

    /**
     * u4_num_Bytes
     */
    UWORD32                                 u4_num_Bytes;

    /**
     * pv_stream_buffer
     */
    void                                    *pv_stream_buffer;

    /**
    * Buffer to store updated stream
    */
    void                                    *pv_stream_out_buffer;

}fgcr_video_rewrite_ip_t;


typedef struct{
    /**
     * u4_size of the structure
     */
    UWORD32                                 u4_size;

    /**
     * u4_error_code
     */
    UWORD32                                 u4_error_code;

    /**
     * num_bytes_consumed
     */
    UWORD32                                 u4_num_bytes_consumed;

    /**
     * num_bytes_consumed by updated stream
     */
    UWORD32                                 u4_num_bytes_generated;

    /**
     * output_present
     */
    UWORD32                                 u4_output_present;

    /**
    * Buffer to store updated stream
    */
    void                                    *pv_stream_out_buffer;

}fgcr_video_rewrite_op_t;

typedef enum {
    /** Set processor details */
    FGCR_CMD_CTL_SET_PROCESSOR = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x001,

    /** Enable/disable GPU, supported on select platforms */
    FGCR_CMD_CTL_GPU_ENABLE_DISABLE = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x200,

    /** Set flags to enable FGS Rewriter, other dependent parameters*/
    FGCR_CMD_CTL_SET_FGS_ENABLE_REWRITER = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x404,
    FGCR_CMD_CTL_SET_FGS_OVERIDE_FGC_CANCEL_FLAG = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x405,
    FGCR_CMD_CTL_SET_FGS_OVERIDE_LOG2_SCALE_FACTOR = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x406,
    FGCR_CMD_CTL_SET_FGS_OVERIDE_DEP_COMP_MODEL_VALUES = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x407,

    /** Set flag for Film Grain Synthesis Rewrite parameters */
    FGCR_CMD_CTL_SET_FGS_FOR_REWRITE = FGCR_CMD_CTL_CODEC_SUBCMD_START + 0x408

}FGCR_CMD_CTL_SUB_CMDS;

/*****************************************************************************/
/*   Rewriter control Set Params                                             */
/*****************************************************************************/

typedef struct {
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * cmd
     */
    FGCR_API_COMMAND_TYPE_T                      e_cmd;

    /**
     * sub_cmd
     */
    FGCR_CONTROL_API_COMMAND_TYPE_T              e_sub_cmd;

    /**
     * codec
     */
    UWORD8                                       u1_codec;

    /**
     * number of film grain characteristics
     */
    UWORD8                                       u1_num_fgc;
}fgcr_ctl_set_config_ip_t;

typedef struct{
    /**
     * u4_size of the structure
     */
	UWORD32                                     u4_size;

    /**
     * u4_error_code
     */
	UWORD32                                     u4_error_code;
}fgcr_ctl_set_config_op_t;

/*****************************************************************************/
/*   Rewriter control:Get Version Info                                       */
/*****************************************************************************/

typedef struct{
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * cmd
     */
    FGCR_API_COMMAND_TYPE_T                      e_cmd;

    /**
     * sub_cmd
     */
    FGCR_CONTROL_API_COMMAND_TYPE_T              e_sub_cmd;

    /**
     * pv_version_buffer
     */
    void                                        *pv_version_buffer;

    /**
     * version_buffer_size
     */
    UWORD32                                     u4_version_buffer_size;
}fgcr_ctl_getversioninfo_ip_t;

typedef struct{
    /**
     * u4_size of the structure
     */
    UWORD32                                     u4_size;

    /**
     * error code
     */
    UWORD32                                     u4_error_code;
}fgcr_ctl_getversioninfo_op_t;

typedef struct {
	UWORD32                                     u4_size;
	FGCR_API_COMMAND_TYPE_T                     e_cmd;
	FGCR_CONTROL_API_COMMAND_TYPE_T             e_sub_cmd;
	UWORD32                                     u4_param_value;
}fgcr_ctl_set_fgs_params_ip_t;

typedef struct {
	UWORD32                                     u4_size;
	UWORD32                                     u4_error_code;
}fgcr_ctl_set_fgs_params_op_t;

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
    /* Repetition period of the film grain characteristics in AVC codec*/
    UWORD32 u4_film_grain_characteristics_repetition_period;
    /* To be 0:  Persistence of the film grain characteristics in HEVC codec*/
    UWORD8 u1_film_grain_characteristics_persistence_flag;
}fgcr_ctl_set_fgc_params_t;

typedef struct
{
    UWORD32                                     u4_size;
    FGCR_API_COMMAND_TYPE_T                      e_cmd;
    FGCR_CONTROL_API_COMMAND_TYPE_T              e_sub_cmd;
    UWORD8                                      u1_num_fgc;
    void                                        *ps_fgs_rewrite_prms;
}fgcr_ctl_fgs_rewrite_params_ip_t;

typedef struct
{
    UWORD32                                     u4_size;
    FGCR_API_COMMAND_TYPE_T                      e_cmd;
    void                                        *ps_fgc_export_prms;
}fgcr_ctl_fgc_export_ip_t;

typedef struct {
    UWORD32                                     u4_size;
    UWORD32                                     u4_error_code;
}fgcr_ctl_fgc_export_op_t;

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif
#endif /* _FGCR_H_ */