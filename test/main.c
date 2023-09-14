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
/* File Includes                                                             */
/*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef X86_MINGW
#include <signal.h>
#endif

#include <malloc.h>
#include "fgcr_typedefs.h"

#include "fgcr.h"

#ifdef WINDOWS_TIMER
#include <windows.h>
#else
#include <sys/time.h>
#endif

#define H264_MAX_WD 3840
#define H264_MAX_HT 2176

#define ALIGN8(x) ((((x) + 7) >> 3) << 3)
#define DEFAULT_FPS         30

#define STRLENGTH 1000
#define ERRLENGTH 2000

#ifdef PROFILE_ENABLE
#ifdef WINDOWS_TIMER
typedef  LARGE_INTEGER TIMER;
#else
typedef struct timeval TIMER;
#endif
#else
typedef WORD32 TIMER;
#endif

#ifdef PROFILE_ENABLE
#ifdef WINDOWS_TIMER
#define GETTIME(timer) QueryPerformanceCounter(timer);
#else
#define GETTIME(timer) gettimeofday(timer,NULL);
#endif

#ifdef WINDOWS_TIMER
#define ELAPSEDTIME(s_start_timer,s_end_timer, s_elapsed_time, frequency) \
                  { \
                   TIMER s_temp_time;   \
                   s_temp_time.LowPart = s_end_timer.LowPart - s_start_timer.LowPart ; \
                   s_elapsed_time = (UWORD32) ( ((DOUBLE)s_temp_time.LowPart / (DOUBLE)frequency.LowPart )  * 1000000); \
                }
#else
#define ELAPSEDTIME(s_start_timer,s_end_timer, s_elapsed_time, frequency) \
                   s_elapsed_time = ((s_end_timer.tv_sec - s_start_timer.tv_sec) * 1000000) + (s_end_timer.tv_usec - s_start_timer.tv_usec);
#endif

#else
#define GETTIME(timer)
#define ELAPSEDTIME(s_start_timer,s_end_timer, s_elapsed_time, frequency)
#endif

typedef struct
{
	UWORD32 u4_file_save_flag;
	UWORD8  u1_enable_logs;
	UWORD32 u4_max_frm_ts;

	void *cocodec_obj;
	CHAR ac_ip_fname[STRLENGTH];
	CHAR ac_op_fgs_fname[STRLENGTH];
	CHAR ac_fgc_export_fname[STRLENGTH];
	CHAR ac_fgc_file_fname[STRLENGTH];
	UWORD32 fps;

	//FGS related parameters
	UWORD32 u4_fgs_enable;
	UWORD32 u4_fgs_blk_size;
	UWORD32 u4_fgs_transform_type;
	UWORD32 u4_fgs_disable_chroma;
	UWORD32 u4_fgs_enable_rewriter;
	UWORD32 u4_fgs_overide_fgc_cancel_flag;
	UWORD32 u4_fgs_overide_log2_scale_factor;
	UWORD32 u4_fgs_overide_dep_comp_model_values;

	fgcr_ctl_set_fgc_params_t s_fgs_prms[MAX_FGC];
	UWORD8 u1_codec;
	UWORD8 u1_mode;
	UWORD8 u1_num_fgc;
	UWORD u1_fgc_count;
} vid_dec_ctx_t;

typedef enum
{
	INVALID,
	HELP,
	VERSION,
	INPUT_FILE,
	//FGS_UPD_STRM,
	OUTPUT,
	CHKSUM,
	SAVE_OUTPUT,
	ENABLE_LOGS,
	SAVE_CHKSUM,
	CHROMA_FORMAT,
	NUM_FRAMES,
	//NUM_CORES,
	MAX_WD,
	MAX_HT,
	PROFILE,
	FPS,
	TRACE,
	CONFIG,
	FGS_ENABLE,
	FGS_BLK_SIZE,
	FGS_TRANSFORM_TYPE,
	FGS_DISABLE_CHROMA,
	FGS_ENABLE_REWRITER,
	FGS_OVERIDE_FGC_CANCEL_FLAG,
	FGS_OVERIDE_LOG2_SCALE_FACTOR,
	FGS_OVERIDE_DEP_COMP_MODEL_VALUES,
	FILM_GRAIN_CHARACTERISTICS_CANCEL_FLAG,
	FILM_GRAIN_MODEL_ID,
	SEPARATE_COLOUR_DESCRIPTION_PRESENT_FLAG,
	FILM_GRAIN_BIT_DEPTH_LUMA_MINUS8,
	FILM_GRAIN_BIT_DEPTH_CHROMA_MINUS8,
	FILM_GRAIN_FULL_RANGE_FLAG,
	FILM_GRAIN_COLOUR_PRIMARIES,
	FILM_GRAIN_TRANSFER_CHARACTERISTICS,
	FILM_GRAIN_MATRIX_COEFFICIENTS,
	BLENDING_MODE_ID,
	LOG2_SCALE_FACTOR,
	COMP_MODEL_PRESENT_FLAG,
	NUM_INTENSITY_INTERVALS_MINUS1,
	NUM_MODEL_VALUES_MINUS1,
	INTENSITY_INTERVAL_LOWER_BOUND,
	INTENSITY_INTERVAL_UPPER_BOUND,
	COMP_MODEL_VALUE,
	FILM_GRAIN_CHARACTERISTICS_REPETITION_PERIOD,
	FILM_GRAIN_CHARACTERISTICS_PERSISTENCE_FLAG,
	CODEC,
	MODE,
	NUM_FGC,
	FGC_FILE,
} ARGUMENT_T;

typedef struct
{
	CHAR argument_shortname[4];
	CHAR argument_name[128];
	ARGUMENT_T argument;
	CHAR description[512];
} argument_t;

static const argument_t argument_mapping[] =
{
	{"-h",  "--help",                   HELP,
		 "Print this help\n"},
	{ "-c", "--config",      CONFIG,
		 "config file (Default: test.cfg)\n" },
	{ "--", "--fgc_file",      FGC_FILE,
		 "file containing list of film grain characteristics\n" },

	{"-v",  "--version",                VERSION,
		 "Version information\n"},
	{"-i",  "--input",                  INPUT_FILE,
		 "Input file\n"},
		 {"-o",  "--output",                 OUTPUT,
			  "Output file\n"},
			  {"--", "--enable_logs",             ENABLE_LOGS,
					"Enable Log prints\n"},
					{ "-n", "--num_frames",             NUM_FRAMES,
						 "Number of frames to be decoded\n" },
						 { "--", "--fgs_enable_rewriter", FGS_ENABLE_REWRITER,
							   "Film Grain synthesis rewriter FGS : 0:Disable, 1:Enable(default)\n" },
						 { "--", "--fgs_overide_fgc_cancel_flag", FGS_OVERIDE_FGC_CANCEL_FLAG,
							   "Film Grain synthesis overide flag for  film_grain_characteristics_cancel_flag: 0:Disable, 1:Enable(default)\n" },
						 { "--", "--fgs_overide_log2_scale_factor", FGS_OVERIDE_LOG2_SCALE_FACTOR,
							   "Film Grain synthesis overide flag for log2scalefactor : 0:Disable, 1:Enable(default)\n" },
						 { "--", "--fgs_overide_dep_comp_model_values", FGS_OVERIDE_DEP_COMP_MODEL_VALUES,
							   "Film Grain synthesis overide flag for comp_model_value dependencies : 0:Disable, 1:Enable(default)\n" },

						 {"--",          "--film_grain_characteristics_cancel_flag", FILM_GRAIN_CHARACTERISTICS_CANCEL_FLAG, "FGS enable parameter. 1-enable 0-disable {0}\n"},

						 {"--",         "--film_grain_model_id",                 FILM_GRAIN_MODEL_ID, "0: [Frequency Filtering] 1: [Auto Regression]\n"},

						 {"--",   "--separate_colour_description_present_flag", SEPARATE_COLOUR_DESCRIPTION_PRESENT_FLAG , "Range: [0,1]\n"},

						 {"--",            "--blending_mode_id",                  BLENDING_MODE_ID,            "0: [additive blending] 1: [multiplicative blending]\n" },

						 {"--",             "--log2_scale_factor",                 LOG2_SCALE_FACTOR,            "supported range: [2,7]\n"},

						 {"--",     "--comp_model_present_flag",           COMP_MODEL_PRESENT_FLAG,      "Comma-separated values for Y,U & V; Range: [0,1] \n"},

						 {"--",       "--num_intensity_intervals_minus1",    NUM_INTENSITY_INTERVALS_MINUS1, "Comma-separated values for Y,U & V; Range: [0,255]\n" },

						 {"--",  "--num_model_values_minus1",           NUM_MODEL_VALUES_MINUS1,       "Comma-separated values for Y,U & V; Range: [0,5]\n"},

						 {"--",        "--intensity_interval_lower_bound",    INTENSITY_INTERVAL_LOWER_BOUND, "Comma-separated lower bound values for y,u,v for every \"ith\" \
											intensity interval (y_i1,y_i2,y_i3,..,u_i1,ui2,..,v_i1,..,etc.); Range: [0,2^(bitdepth)-1]\n" },

	{"--",         "--intensity_interval_upper_bound",    INTENSITY_INTERVAL_UPPER_BOUND, "Comma-separated upper bound values for y,u,v for every \"ith\" \
											intensity interval (y_i1,y_i2,y_i3,..,u_i1,ui2,..,v_i1,..,etc.); Range: [0,2^(bitdepth)-1]\n" },

	{"--",       "--comp_model_value",                  COMP_MODEL_VALUE,                "Comma-separated 'j' component model values  for y,u,v for every \"ith\" intensity interval\n"},

	{"--",     "--film_grain_characteristics_repetition_period",           FILM_GRAIN_CHARACTERISTICS_REPETITION_PERIOD,      "repetition period \n"},

	{"--",     "--film_grain_characteristics_persistence_flag",           FILM_GRAIN_CHARACTERISTICS_PERSISTENCE_FLAG,      "persistence flag \n"},

	{"--",       "--codec",                  CODEC,                "AVC or HEVC codec"},

	{"--",       "--mode",                  MODE,                "Rewrite or Export mode"},

	{"--",       "--num_fgc",                  NUM_FGC,                "Number of film grain characteristics"},

};

#define PEAK_WINDOW_SIZE            8
#define DEFAULT_FGS_ENABLE_FLAG     1
#define DEFAULT_FGS_BLK_SIZE        8
#define DEFAULT_FGS_TRANSFORM_TYPE  0 //RDD5
#define DEFAULT_FGS_DISABLE_CHROMA  0 //enable chroma FGS
#define DEFAULT_FGS_ENABLE_REWRITER 1
#define DEFAULT_FGS_OVERIDE_FGC_CANCEL_FLAG 1
#define DEFAULT_FGS_OVERIDE_LOG2_SCALE_FACTOR 1
#define DEFAULT_FGS_OVERIDE_DEP_COMP_MODEL_VALUES 1

/* modes */
#define FGC_REWRITER  1
#define FGC_EXPORT    2

#define DEFAULT_NUM_FGC 1

#define ivd_api_function        fgcr_api_function

#ifdef _WIN32
/*****************************************************************************/
/* Function to print library calls                                           */
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*  Function Name : memalign                                                 */
/*                                                                           */
/*  Description   : Returns malloc data. Ideally should return aligned memory*/
/*                  support alignment will be added later                    */
/*                                                                           */
/*  Inputs        : alignment                                                */
/*                  i4_size                                                  */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*                                                                           */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*                                                                           */
/*****************************************************************************/

void * fgcr_aligned_malloc(void *pv_ctxt, WORD32 alignment, WORD32 i4_size)
{
	(void)pv_ctxt;
	return (void *)_aligned_malloc(i4_size, alignment);
}

void fgcr_aligned_free(void *pv_ctxt, void *pv_buf)
{
	(void)pv_ctxt;
	_aligned_free(pv_buf);
	return;
}
#endif

#if (!defined(IOS)) && (!defined(_WIN32))
void * fgcr_aligned_malloc(void *pv_ctxt, WORD32 alignment, WORD32 i4_size)
{
	(void)pv_ctxt;
	return memalign(alignment, i4_size);
}

void fgcr_aligned_free(void *pv_ctxt, void *pv_buf)
{
	(void)pv_ctxt;
	free(pv_buf);
	return;
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  Function Name : get_version                                              */
/*                                                                           */
/*  Description   : Control call to get codec version                        */
/*                                                                           */
/*                                                                           */
/*  Inputs        : codec_obj : Codec handle                                 */
/*  Globals       :                                                          */
/*  Processing    : Calls enable skip B frames control                       */
/*                                                                           */
/*  Outputs       :                                                          */
/*  Returns       : Control call return i4_status                            */
/*                                                                           */
/*****************************************************************************/

IV_API_CALL_STATUS_T get_version(void *codec_obj)
{
	fgcr_ctl_getversioninfo_ip_t ps_ctl_ip;
	fgcr_ctl_getversioninfo_op_t ps_ctl_op;
	UWORD8 au1_buf[512];
	IV_API_CALL_STATUS_T i4_status;
	ps_ctl_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
	ps_ctl_ip.e_sub_cmd = FGCR_CMD_CTL_GETVERSION;
	ps_ctl_ip.u4_size = sizeof(fgcr_ctl_getversioninfo_ip_t);
	ps_ctl_op.u4_size = sizeof(fgcr_ctl_getversioninfo_op_t);
	ps_ctl_ip.pv_version_buffer = au1_buf;
	ps_ctl_ip.u4_version_buffer_size = sizeof(au1_buf);

	i4_status = ivd_api_function((iv_obj_t *)codec_obj,
		(void *)&(ps_ctl_ip),
		(void *)&(ps_ctl_op));

	if (i4_status != IV_SUCCESS)
	{
		printf("Error in Getting Version number e_dec_status = %d u4_error_code = %x\n",
			i4_status, ps_ctl_op.u4_error_code);
	}
	else
	{
		printf("Ittiam Rewriter Version number: %s\n",
			(char *)ps_ctl_ip.pv_version_buffer);
	}
	return i4_status;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : codec_exit                                               */
/*                                                                           */
/*  Description   : handles unrecoverable errors                             */
/*  Inputs        : Error message                                            */
/*  Globals       : None                                                     */
/*  Processing    : Prints error message to console and exits.               */
/*  Outputs       : Error mesage to the console                              */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/
void codec_exit(CHAR *pc_err_message)
{
	printf("%s\n", pc_err_message);
	exit(-1);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : print_usage                                              */
/*                                                                           */
/*  Description   : Prints argument format                                   */
/*                                                                           */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    : Prints argument format                                   */
/*                                                                           */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*                                                                           */
/*****************************************************************************/

void print_usage(void)
{
	WORD32 i = 0;
	WORD32 num_entries = sizeof(argument_mapping) / sizeof(argument_t);
	printf("\nUsage:\n");
	while (i < num_entries)
	{
		printf("%-32s\t %s", argument_mapping[i].argument_name,
			argument_mapping[i].description);
		i++;
	}
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : get_argument                                             */
/*                                                                           */
/*  Description   : Gets argument for a given string                         */
/*                                                                           */
/*  Inputs        : name                                                     */
/*  Globals       :                                                          */
/*  Processing    : Searches the given string in the array and returns       */
/*                  appropriate argument ID                                  */
/*                                                                           */
/*  Outputs       : Argument ID                                              */
/*  Returns       : Argument ID                                              */
/*                                                                           */
/*****************************************************************************/

ARGUMENT_T get_argument(CHAR *name)
{
	WORD32 i = 0;
	WORD32 num_entries = sizeof(argument_mapping) / sizeof(argument_t);
	while (i < num_entries)
	{
		if ((0 == strcmp(argument_mapping[i].argument_name, name)) ||
			((0 == strcmp(argument_mapping[i].argument_shortname, name)) &&
			(0 != strcmp(argument_mapping[i].argument_shortname, "--"))))
		{
			return argument_mapping[i].argument;
		}
		i++;
	}
	return INVALID;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : get_argument                                             */
/*                                                                           */
/*  Description   : Gets argument for a given string                         */
/*                                                                           */
/*  Inputs        : name                                                     */
/*  Globals       :                                                          */
/*  Processing    : Searches the given string in the array and returns       */
/*                  appropriate argument ID                                  */
/*                                                                           */
/*  Outputs       : Argument ID                                              */
/*  Returns       : Argument ID                                              */
/*                                                                           */
/*****************************************************************************/

void parse_argument(vid_dec_ctx_t *ps_app_ctx, CHAR *argument, CHAR *value)
{
    ARGUMENT_T arg;
    UWORD8      *pu1_keywd_str;
    UWORD8      au1_keywd_str[MAX_KEYWORD_LEN];
    CHAR ac_error_str[ERRLENGTH];
    pu1_keywd_str = &au1_keywd_str[0];
    //Updating the parser for new arguments
    arg = get_argument(argument);
    switch (arg)
    {
    case HELP:
        print_usage();
        exit(-1);
    case VERSION:
        break;
    case INPUT_FILE:
        sscanf(value, "%s", ps_app_ctx->ac_ip_fname);
        break;
    case OUTPUT:
        sscanf(value, "%s", ps_app_ctx->ac_op_fgs_fname);
        break;
    case ENABLE_LOGS:
        sscanf(value, "%hhu", &ps_app_ctx->u1_enable_logs);
        break;
    case NUM_FRAMES:
        sscanf(value, "%d", &ps_app_ctx->u4_max_frm_ts);
        break;
    case FGS_ENABLE_REWRITER:
        sscanf(value, "%d", &ps_app_ctx->u4_fgs_enable_rewriter);
        break;
    case FGS_OVERIDE_FGC_CANCEL_FLAG:
        sscanf(value, "%d", &ps_app_ctx->u4_fgs_overide_fgc_cancel_flag);
        break;
    case FGS_OVERIDE_LOG2_SCALE_FACTOR:
        sscanf(value, "%d", &ps_app_ctx->u4_fgs_overide_log2_scale_factor);
        break;
    case FGS_OVERIDE_DEP_COMP_MODEL_VALUES:
        sscanf(value, "%d", &ps_app_ctx->u4_fgs_overide_dep_comp_model_values);
        break;

    case FILM_GRAIN_CHARACTERISTICS_CANCEL_FLAG:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_characteristics_cancel_flag);
        break;

    case FILM_GRAIN_MODEL_ID:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_model_id);
        break;

    case SEPARATE_COLOUR_DESCRIPTION_PRESENT_FLAG:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_separate_colour_description_present_flag);
        break;

    case FILM_GRAIN_BIT_DEPTH_LUMA_MINUS8:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_bit_depth_luma_minus8);
        break;

    case FILM_GRAIN_BIT_DEPTH_CHROMA_MINUS8:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_bit_depth_chroma_minus8);
        break;

    case FILM_GRAIN_FULL_RANGE_FLAG:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_full_range_flag);
        break;

    case FILM_GRAIN_COLOUR_PRIMARIES:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_colour_primaries);
        break;

    case FILM_GRAIN_TRANSFER_CHARACTERISTICS:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_transfer_characteristics);
        break;

    case FILM_GRAIN_MATRIX_COEFFICIENTS:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_matrix_coefficients);
        break;

    case BLENDING_MODE_ID:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_blending_mode_id);
        break;

    case LOG2_SCALE_FACTOR:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_log2_scale_factor);
        break;

    case COMP_MODEL_PRESENT_FLAG:
    {
        char *token;
        char *str;
        const char s[2] = ",";
        WORD32 i = 0;
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);
        while (token != NULL)
        {
            if (i < MAX_NUM_COMP)
            {
                sscanf(token, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[i]);
                i++;
                token = strtok(NULL, s);
            }
        }
        if (i < MAX_NUM_COMP)
        {
            sprintf(ac_error_str, "Invalid number of values [%d] being passed to parameter: [%s]\n",
                i, argument);
            codec_exit(ac_error_str);
        }
    }
    break;

    case NUM_INTENSITY_INTERVALS_MINUS1:
    {
        char *token;
        char *str;
        const char s[2] = ",";
        WORD32 i = 0;
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);
        while (token != NULL)
        {
            if (i < MAX_NUM_COMP)
            {
                sscanf(token, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[i]);
                i++;
                token = strtok(NULL, s);
            }
        }
        if (i < MAX_NUM_COMP)
        {
            sprintf(ac_error_str, "Invalid number of values [%d] being passed to parameter: [%s]\n",
                i, argument);
            codec_exit(ac_error_str);
        }
    }
    break;

    case NUM_MODEL_VALUES_MINUS1:
    {
        char *token;
        char *str;
        const char s[2] = ",";
        WORD32 i = 0;
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);
        while (token != NULL)
        {
            if (i < MAX_NUM_COMP)
            {
                sscanf(token, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_model_values_minus1[i]);
                i++;
                token = strtok(NULL, s);
            }
        }
        if (i < MAX_NUM_COMP)
        {
            sprintf(ac_error_str, "\nInvalid number of values [%d] being passed to parameter: [%s]\n",
                i, argument);
            codec_exit(ac_error_str);
        }
    }
    break;

    case INTENSITY_INTERVAL_LOWER_BOUND:
    {
        WORD32 max_num_of_values = 0;
        WORD32 num_intensity_itvls = 0;
        char *token;
        char *str;
        const char s[2] = ",";
        WORD32 i, j, k;
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);

        for (i = 0; i < MAX_NUM_COMP; i++)
        {
            max_num_of_values += (ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[i] * (ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[i] + 1));
        }

        i = 0;
        for (j = 0; j < MAX_NUM_COMP; j++)
        {
            num_intensity_itvls = (1 + ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[j])*ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[j];
            for (k = 0; k < (num_intensity_itvls); k++)
            {
                if (token == NULL)
                {
                    sprintf(ac_error_str, "Invalid number of values [%d] being passed to parameter: [%s]\n",
                        i, argument);
                    codec_exit(ac_error_str);
                }
                else
                {
                    sscanf(token, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_intensity_interval_lower_bound[j][k]);
                    token = strtok(NULL, s);
                    i++;
                }
            }
        }
    }
    break;

    case INTENSITY_INTERVAL_UPPER_BOUND:
    {
        WORD32 max_num_of_values = 0;
        WORD32 num_intensity_itvls = 0;
        char *token;
        char *str;
        const char s[2] = ",";
        WORD32 i, j, k;
        i = 0;
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);
        for (j = 0; j < MAX_NUM_COMP; j++)
        {
            num_intensity_itvls = (1 + ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[j])*ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[j];
            for (k = 0; k < num_intensity_itvls; k++)
            {
                if (token == NULL)
                {
                    sprintf(ac_error_str, "Invalid number of values [%d] being passed to parameter: [%s]\n",
                        i, argument);
                    codec_exit(ac_error_str);
                }
                else
                {
                    sscanf(token, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_intensity_interval_upper_bound[j][k]);
                    token = strtok(NULL, s);
                    i++;
                }
            }
        }
    }
    break;

    case COMP_MODEL_VALUE:
    {
        WORD32 max_num_of_values = 0;
        WORD32 num_intensity_itvls = 0;
        WORD32 num_comp_model_values = 0;
        WORD32 c = 0;
        WORD32 i, j, k, v;
        char *token;
        char *str;
        const char s[2] = ",";
        sscanf(value, "%s", pu1_keywd_str);
        str = (char *)pu1_keywd_str;
        token = strtok(str, s);

        for (i = 0; i < MAX_NUM_COMP; i++)
        {
            max_num_of_values += (ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[i] * (ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[i] + 1));
        }
        i = 0; j = 0;

        if (i < max_num_of_values)
        {
            for (j = 0; j < MAX_NUM_COMP; j++)
            {
                num_intensity_itvls = (1 + ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_intensity_intervals_minus1[j])* ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[j];
                for (k = 0; k < (num_intensity_itvls); k++)
                {
                    num_comp_model_values = (1 + ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_num_model_values_minus1[j]) *
                        ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_comp_model_present_flag[j];
                    for (v = 0; v < (num_comp_model_values); v++)
                    {
                        if (token == NULL)
                        {
                            sprintf(ac_error_str, "Invalid number of values [%d] being passed to parameter: [%s]\n",
                                i, argument);
                            codec_exit(ac_error_str);
                        }
                        else
                        {
                            sscanf(token, "%d", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u4_comp_model_value[j][k][v]);
                            token = strtok(NULL, s);
                            i++;
                        }
                    }
                }
            }
        }
    }
    break;

    case FILM_GRAIN_CHARACTERISTICS_REPETITION_PERIOD:
        sscanf(value, "%d", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u4_film_grain_characteristics_repetition_period);
        break;

    case FILM_GRAIN_CHARACTERISTICS_PERSISTENCE_FLAG:
        sscanf(value, "%hhu", &ps_app_ctx->s_fgs_prms[ps_app_ctx->u1_fgc_count].u1_film_grain_characteristics_persistence_flag);
        break;

    case CODEC:
        sscanf(value, "%d", &ps_app_ctx->u1_codec);
        break;

    case MODE:
        sscanf(value, "%d", &ps_app_ctx->u1_mode);
        break;

    case NUM_FGC:
        sscanf(value, "%d", &ps_app_ctx->u1_num_fgc);
        break;

    case FGC_FILE:
        sscanf(value, "%s", ps_app_ctx->ac_fgc_file_fname);

    case INVALID:
    default:
        printf("Ignoring argument :  %s\n", argument);
        break;
    }
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : read_cfg_file                                            */
/*                                                                           */
/*  Description   : Reads arguments from a configuration file                */
/*                                                                           */
/*                                                                           */
/*  Inputs        : ps_app_ctx  : Application context                        */
/*                  fp_cfg_file : Configuration file handle                  */
/*  Globals       :                                                          */
/*  Processing    : Parses the arguments and fills in the application context*/
/*                                                                           */
/*  Outputs       : Arguments parsed                                         */
/*  Returns       : None                                                     */
/*                                                                           */
/*****************************************************************************/

void read_cfg_file(vid_dec_ctx_t *ps_app_ctx, FILE *fp_cfg_file)
{
    CHAR line[STRLENGTH];
    CHAR description[STRLENGTH];
    CHAR value[STRLENGTH];
    CHAR argument[STRLENGTH];
    void *ret;
    while (0 == feof(fp_cfg_file))
    {
        line[0] = '\0';
        ret = fgets(line, STRLENGTH, fp_cfg_file);
        if (NULL == ret)
            break;
        argument[0] = '\0';
        /* Reading Input File Name */
        sscanf(line, "%s %s %s", argument, value, description);
        if (argument[0] == '\0')
            continue;

        parse_argument(ps_app_ctx, argument, value);
    }
}

void read_fgc_file(vid_dec_ctx_t *ps_app_ctx, FILE *fp_cfg_file)
{
    CHAR line[STRLENGTH];
    CHAR description[STRLENGTH];
    CHAR value[STRLENGTH];
    CHAR argument[STRLENGTH];
    void *ret;
    while (0 == feof(fp_cfg_file))
    {
        line[0] = '\0';
        ret = fgets(line, STRLENGTH, fp_cfg_file);
        if (NULL == ret)
            break;
        argument[0] = '\0';
        /* Reading Input File Name */
        sscanf(line, "%s %s %s", argument, value, description);
        if (argument[0] == '\0')
            continue;
        else if ((0 == strcmp(argument, "FGC")) && (0 != strcmp(value, "1")))
        {
            ps_app_ctx->u1_fgc_count++;
            continue;
        }

        parse_argument(ps_app_ctx, argument, value);
    }
}

#ifdef X86_MINGW
void sigsegv_handler()
{
    printf("Segmentation fault, Exiting.. \n");
    exit(-1);
}
#endif

UWORD32 default_fgs_rewriter_params(vid_dec_ctx_t *ps_app_ctx)
{
    ps_app_ctx->u4_fgs_enable_rewriter = DEFAULT_FGS_ENABLE_REWRITER;
    ps_app_ctx->u4_fgs_overide_fgc_cancel_flag = DEFAULT_FGS_OVERIDE_FGC_CANCEL_FLAG;
    ps_app_ctx->u4_fgs_overide_log2_scale_factor = DEFAULT_FGS_OVERIDE_LOG2_SCALE_FACTOR;
    ps_app_ctx->u4_fgs_overide_dep_comp_model_values = DEFAULT_FGS_OVERIDE_DEP_COMP_MODEL_VALUES;

    /** Default FGS values if not given by the config file */
    /* FGS parameters */
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_characteristics_cancel_flag = 1;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_model_id = 0;
    ps_app_ctx->s_fgs_prms[0].u1_separate_colour_description_present_flag = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_bit_depth_luma_minus8 = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_bit_depth_chroma_minus8 = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_full_range_flag = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_colour_primaries = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_transfer_characteristics = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_matrix_coefficients = 0;
    ps_app_ctx->s_fgs_prms[0].u1_blending_mode_id = 0;
    ps_app_ctx->s_fgs_prms[0].u1_log2_scale_factor = 2;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_characteristics_persistence_flag = 0;
    for (int c = 0; c < MAX_NUM_COMP; c++)
    {
        ps_app_ctx->s_fgs_prms[0].u1_comp_model_present_flag[c] = 0;
    }

    for (int c = 0; c < MAX_NUM_COMP; c++)
    {
        ps_app_ctx->s_fgs_prms[0].u1_num_intensity_intervals_minus1[c] = 0;
        ps_app_ctx->s_fgs_prms[0].u1_num_model_values_minus1[c] = 0;
        for (int i = 0; i <= MAX_NUM_INTENSITIES; i++)
        {
            ps_app_ctx->s_fgs_prms[0].u1_intensity_interval_lower_bound[c][i] = 0;
            ps_app_ctx->s_fgs_prms[0].u1_intensity_interval_upper_bound[c][i] = 0;
            for (int j = 0; j <= MAX_NUM_MODEL_VALUES; j++)
            {
                ps_app_ctx->s_fgs_prms[0].u4_comp_model_value[c][i][j] = 0;
            }
        }
    }
    ps_app_ctx->s_fgs_prms[0].u4_film_grain_characteristics_repetition_period = 0;
    ps_app_ctx->s_fgs_prms[0].u1_film_grain_characteristics_persistence_flag = 0;

    return 0;
}
//int gettimeofday(struct timeval * tp, struct timezone * tzp)
//{
//	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
//	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
//	// until 00:00:00 January 1, 1970
//	static const UWORD64 EPOCH = ((UWORD64)116444736000000000ULL);
//
//	SYSTEMTIME  system_time;
//	FILETIME    file_time;
//	UWORD64    time;
//
//	GetSystemTime(&system_time);
//	SystemTimeToFileTime(&system_time, &file_time);
//	time = ((UWORD64)file_time.dwLowDateTime);
//	time += ((UWORD64)file_time.dwHighDateTime) << 32;
//
//	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
//	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
//	return 0;
//}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : main                                                     */
/*                                                                           */
/*  Description   : Application to demonstrate codec API                     */
/*                                                                           */
/*                                                                           */
/*  Inputs        : argc    - Number of arguments                            */
/*                  argv[]  - Arguments                                      */
/*  Globals       :                                                          */
/*  Processing    : Shows how to use create, process, control and delete     */
/*                                                                           */
/*  Outputs       : Codec output in a file                                   */
/*  Returns       :                                                          */
/*                                                                           */
/*****************************************************************************/
int main(WORD32 argc, CHAR *argv[])
{
	CHAR ac_cfg_fname[STRLENGTH];
	CHAR ac_fgc_fname[STRLENGTH];
	FILE *fp_cfg_file = NULL;
	FILE *fp_fgc_file = NULL;
	FILE *ps_ip_file = NULL;
	FILE *ps_fgs_upd_file = NULL;
	FILE *ps_fgc_export_file = NULL;
	WORD32 ret;
	CHAR ac_error_str[ERRLENGTH];
	vid_dec_ctx_t s_app_ctx;
	UWORD8 *pu1_bs_buf = NULL;
	UWORD8 *pu1_bsu_buf = NULL;
	UWORD32 u4_num_bytes_dec = 0;
	UWORD32 u4_num_bytes_generated = 0;
	UWORD32 file_pos = 0;
	UWORD64 u8_ip_frm_ts = 0, u8_op_frm_ts = 0;

	WORD32 u4_bytes_remaining = 0;
	UWORD32 i;
	UWORD32 u4_ip_buf_len;
	UWORD32 frm_cnt = 0;
	WORD32 total_bytes_comsumed;
	WORD32 total_bytes_generated;
	UWORD32 max_op_frm_ts;

#ifdef PROFILE_ENABLE
	UWORD32 u4_tot_cycles = 0;
	UWORD32 u4_tot_fmt_cycles = 0;
	UWORD32 peak_window[PEAK_WINDOW_SIZE];
	UWORD32 peak_window_idx = 0;
	UWORD32 peak_avg_max = 0;
#endif

#ifdef WINDOWS_TIMER
	TIMER frequency;
#endif
	iv_obj_t *codec_obj;

#ifdef X86_MINGW
	//For getting printfs without any delay
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

#ifdef X86_MINGW
	{
		signal(SIGSEGV, sigsegv_handler);
	}
#endif

	/* Usage */
	if (argc < 2)
	{
		printf("Using test.cfg as configuration file \n");
		strcpy(ac_cfg_fname, "test.cfg");
	}
	else if (argc == 2)
	{
		strcpy(ac_cfg_fname, argv[1]);
	}


	/***********************************************************************/
	/*                  Initialize Application parameters                  */
	/***********************************************************************/

	strcpy(s_app_ctx.ac_ip_fname, "\0");
	s_app_ctx.fps = DEFAULT_FPS;
	file_pos = 0;
	total_bytes_comsumed = 0;
	total_bytes_generated = 0;
	u8_ip_frm_ts = 0;
	u8_op_frm_ts = 0;
#ifdef PROFILE_ENABLE
	memset(peak_window, 0, sizeof(WORD32) * PEAK_WINDOW_SIZE);
#endif
	s_app_ctx.u4_fgs_enable = DEFAULT_FGS_ENABLE_FLAG;
	s_app_ctx.u4_fgs_blk_size = DEFAULT_FGS_BLK_SIZE;
	s_app_ctx.u4_fgs_transform_type = DEFAULT_FGS_TRANSFORM_TYPE;
	s_app_ctx.u4_fgs_disable_chroma = DEFAULT_FGS_DISABLE_CHROMA;
	s_app_ctx.u1_mode = FGC_REWRITER;
	s_app_ctx.u1_num_fgc = DEFAULT_NUM_FGC;
	s_app_ctx.u1_fgc_count = 0;

	default_fgs_rewriter_params(&s_app_ctx);

	s_app_ctx.u1_enable_logs = 0;
	/*************************************************************************/
	/* Parse arguments                                                       */
	/*************************************************************************/

	/* Read command line arguments */
	if (argc > 2)
	{
		for (i = 1; i < (UWORD32)argc; i += 2)
		{
			if (CONFIG == get_argument(argv[i]))
			{
				strcpy(ac_cfg_fname, argv[i + 1]);
				if ((fp_cfg_file = fopen(ac_cfg_fname, "r")) == NULL)
				{
					sprintf(ac_error_str, "Could not open Configuration file %s",
						ac_cfg_fname);
					codec_exit(ac_error_str);
				}
				read_cfg_file(&s_app_ctx, fp_cfg_file);
				fclose(fp_cfg_file);
			}
			else
			{
				parse_argument(&s_app_ctx, argv[i], argv[i + 1]);
			}
		}
	}
	else
	{
		if ((fp_cfg_file = fopen(ac_cfg_fname, "r")) == NULL)
		{
			sprintf(ac_error_str, "Could not open Configuration file %s",
				ac_cfg_fname);
			codec_exit(ac_error_str);
		}
		read_cfg_file(&s_app_ctx, fp_cfg_file);
		fclose(fp_cfg_file);
	}

    if (s_app_ctx.u1_num_fgc > 1)
    {
        strcpy(ac_fgc_fname, s_app_ctx.ac_fgc_file_fname);
        if ((fp_fgc_file = fopen(ac_fgc_fname, "r")) == NULL)
        {
            sprintf(ac_error_str, "Could not open Film Grain Characteristics file %s",
                ac_fgc_fname);
            codec_exit(ac_error_str);
        }
        read_fgc_file(&s_app_ctx, fp_fgc_file);
        fclose(fp_fgc_file);
    }

	if (s_app_ctx.u1_enable_logs != 0 && s_app_ctx.u1_enable_logs != 1)
	{
		printf("Error in setting parameter enable_logs\n");
		return -1;
	}
#ifdef PRINT_PICSIZE
	/* If the binary is used for only getting number of bytes in each picture, then disable the following features */
	s_app_ctx.u4_file_save_flag = 0;
#endif

	if (strcmp(s_app_ctx.ac_ip_fname, "\0") == 0)
	{
		printf("\nNo input file given for decoding\n");
		exit(-1);
	}

	/***********************************************************************/
	/*          create the file object for input file                      */
	/***********************************************************************/
	ps_ip_file = fopen(s_app_ctx.ac_ip_fname, "rb");
	ps_fgs_upd_file = fopen(s_app_ctx.ac_op_fgs_fname, "wb");

	strcpy(s_app_ctx.ac_fgc_export_fname, "fgc_export.txt");
	ps_fgc_export_file = fopen(s_app_ctx.ac_fgc_export_fname, "wb");

	if (NULL == ps_ip_file)
	{
		sprintf(ac_error_str, "Could not open input file %s",
			s_app_ctx.ac_ip_fname);
		codec_exit(ac_error_str);
	}

	/***********************************************************************/
	/*          create the file object for output file                     */
	/***********************************************************************/

	/* If the filename does not contain %d, then output will be dumped to
	   a single file and it is opened here */
	   /*if((1 == s_app_ctx.u4_file_save_flag) && (strstr(s_app_ctx.ac_op_fname,"%d") == NULL))
	   {
		   ps_op_file = fopen(s_app_ctx.ac_op_fname, "wb");

		   if(NULL == ps_op_file)
		   {
			   sprintf(ac_error_str, "Could not open output file %s",
					   s_app_ctx.ac_op_fname);
			   codec_exit(ac_error_str);
		   }
	   }*/

	   /***********************************************************************/
	   /*                      Create decoder instance                        */
	   /***********************************************************************/
	{

		/*****************************************************************************/
		/*   API Call: Initialize the Decoder                                        */
		/*****************************************************************************/
		{
			fgcr_create_ip_t s_create_ip;
			fgcr_create_op_t s_create_op;
			IV_API_CALL_STATUS_T(*fxns)(iv_obj_t *ps_handle, void *pv_api_ip, void *pv_api_op) = &ivd_api_function;

			s_create_ip.e_cmd = FGCR_CMD_CREATE;
			s_create_ip.pf_aligned_alloc = fgcr_aligned_malloc;
			s_create_ip.pf_aligned_free = fgcr_aligned_free;
			s_create_ip.pv_mem_ctxt = NULL;
			s_create_ip.u4_size = sizeof(fgcr_create_ip_t);
			s_create_op.u4_size = sizeof(fgcr_create_op_t);

			ret = ivd_api_function(NULL, (void *)&s_create_ip,
				(void *)&s_create_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "Error in Create %8x\n",
					s_create_op.u4_error_code);
				codec_exit(ac_error_str);
			}
			codec_obj = (iv_obj_t*)s_create_op.pv_handle;
			codec_obj->pv_fxns = (void *)fxns;
			codec_obj->u4_size = sizeof(iv_obj_t);
			s_app_ctx.cocodec_obj = codec_obj;
		}
	}

	/*************************************************************************/
	/* set Codec                                                  */
	/*************************************************************************/
	{
		fgcr_ctl_set_codec_ip_t s_ctl_ip;
		fgcr_ctl_set_codec_op_t s_ctl_op;

		s_ctl_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
		s_ctl_ip.e_sub_cmd = FGCR_CMD_CTL_SETCODEC;
		s_ctl_ip.u4_size = sizeof(fgcr_ctl_set_codec_ip_t);
		s_ctl_ip.u1_codec = s_app_ctx.u1_codec;
		s_ctl_ip.u1_num_fgc = s_app_ctx.u1_num_fgc;

		s_ctl_op.u4_size = sizeof(fgcr_ctl_set_codec_op_t);

		ret = ivd_api_function((iv_obj_t *)codec_obj, (void *)&s_ctl_ip, (void *)&s_ctl_op);

		if (IV_SUCCESS != ret)
		{
			sprintf(ac_error_str, "Error in Set Codec");
			//codec_exit(ac_error_str);
		}
	}

	if (s_app_ctx.u1_mode == FGC_EXPORT)
		s_app_ctx.u4_fgs_enable_rewriter = 0;

	/*************************************************************************/
	/* set FGS parameters                                                    */
	/*************************************************************************/
	{
		fgcr_ctl_set_fgs_params_ip_t s_ctl_fgs_ip;
		fgcr_ctl_set_fgs_params_op_t s_ctl_fgs_op;

		/** Set FGS rewrite parameters*/
		{
			s_ctl_fgs_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
			s_ctl_fgs_ip.e_sub_cmd = (FGCR_CONTROL_API_COMMAND_TYPE_T)FGCR_CMD_CTL_SET_FGS_ENABLE_REWRITER;
			s_ctl_fgs_ip.u4_param_value = s_app_ctx.u4_fgs_enable_rewriter;

			s_ctl_fgs_ip.u4_size = sizeof(fgcr_ctl_set_fgs_params_ip_t);
			s_ctl_fgs_op.u4_size = sizeof(fgcr_ctl_set_fgs_params_op_t);

			ret = ivd_api_function((iv_obj_t*)codec_obj, (void *)&s_ctl_fgs_ip,
				(void *)&s_ctl_fgs_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "\nError in setting FGS enable rewriter %x", s_ctl_fgs_op.u4_error_code);
				codec_exit(ac_error_str);
			}

			s_ctl_fgs_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
			s_ctl_fgs_ip.e_sub_cmd = (FGCR_CONTROL_API_COMMAND_TYPE_T)FGCR_CMD_CTL_SET_FGS_OVERIDE_FGC_CANCEL_FLAG;
			s_ctl_fgs_ip.u4_param_value = s_app_ctx.u4_fgs_overide_fgc_cancel_flag;

			ret = ivd_api_function((iv_obj_t*)codec_obj, (void *)&s_ctl_fgs_ip,
				(void *)&s_ctl_fgs_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "\nError in setting FGS overide fgc cancel flag");
				codec_exit(ac_error_str);
			}

			s_ctl_fgs_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
			s_ctl_fgs_ip.e_sub_cmd = (FGCR_CONTROL_API_COMMAND_TYPE_T)FGCR_CMD_CTL_SET_FGS_OVERIDE_LOG2_SCALE_FACTOR;
			s_ctl_fgs_ip.u4_param_value = s_app_ctx.u4_fgs_overide_log2_scale_factor;

			ret = ivd_api_function((iv_obj_t*)codec_obj, (void *)&s_ctl_fgs_ip,
				(void *)&s_ctl_fgs_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "\nError in setting FGS overide flag for log2scalefactor");
				codec_exit(ac_error_str);
			}

			s_ctl_fgs_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
			s_ctl_fgs_ip.e_sub_cmd = (FGCR_CONTROL_API_COMMAND_TYPE_T)FGCR_CMD_CTL_SET_FGS_OVERIDE_DEP_COMP_MODEL_VALUES;
			s_ctl_fgs_ip.u4_param_value = s_app_ctx.u4_fgs_overide_dep_comp_model_values;

			ret = ivd_api_function((iv_obj_t*)codec_obj, (void *)&s_ctl_fgs_ip,
				(void *)&s_ctl_fgs_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "\nError in setting FGS overide dependencies of component model values");
				codec_exit(ac_error_str);
			}

			fgcr_ctl_fgs_rewrite_params_ip_t s_ctl_fgs_rewrite_ip;
			fgcr_ctl_set_fgs_params_op_t     s_ctl_fgs_rewrite_op;
			fgcr_ctl_set_fgc_params_t *ps_fgs_prms[MAX_FGC];

			s_ctl_fgs_rewrite_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
			s_ctl_fgs_rewrite_ip.e_sub_cmd = (FGCR_CONTROL_API_COMMAND_TYPE_T)FGCR_CMD_CTL_SET_FGS_FOR_REWRITE;
			s_ctl_fgs_rewrite_ip.u1_num_fgc = s_app_ctx.u1_num_fgc;
			

			for (int i = 0; i < s_app_ctx.u1_num_fgc; i++)
			{
				ps_fgs_prms[i] = &s_app_ctx.s_fgs_prms[i];
				s_ctl_fgs_rewrite_ip.ps_fgs_rewrite_prms[i] = (void *)ps_fgs_prms[i];
			}
			ret = ivd_api_function((iv_obj_t*)codec_obj, (void *)&s_ctl_fgs_rewrite_ip,
				(void *)&s_ctl_fgs_rewrite_op);
			if (ret != IV_SUCCESS)
			{
				sprintf(ac_error_str, "\nError in setting FGS overide values\n");
				codec_exit(ac_error_str);
			}
		}
	}

	/*************************************************************************/
	/* Set the decoder in frame decode mode. It was set in header decode     */
	/* mode earlier                                                          */
	/*************************************************************************/
	{
		fgcr_ctl_set_config_ip_t s_ctl_ip;
		fgcr_ctl_set_config_op_t s_ctl_op;

		s_ctl_ip.e_cmd = FGCR_CMD_VIDEO_CTL;
		s_ctl_ip.e_sub_cmd = FGCR_CMD_CTL_SETPARAMS;
		s_ctl_ip.u4_size = sizeof(fgcr_ctl_set_config_ip_t);

		s_ctl_op.u4_size = sizeof(fgcr_ctl_set_config_op_t);

		ret = ivd_api_function((iv_obj_t *)codec_obj, (void *)&s_ctl_ip, (void *)&s_ctl_op);

		if (IV_SUCCESS != ret)
		{
			sprintf(ac_error_str, "Error in Set Parameters");
			//codec_exit(ac_error_str);
		}
	}

#ifdef WINDOWS_TIMER
	QueryPerformanceFrequency(&frequency);
#endif
#ifndef PRINT_PICSIZE
	get_version(codec_obj);
#endif

	max_op_frm_ts = s_app_ctx.u4_max_frm_ts;

	if (max_op_frm_ts < 0)
		max_op_frm_ts = 0xffffffff;/* clip as overflow has occured*/

	max_op_frm_ts = (s_app_ctx.u4_max_frm_ts > 0) ? (max_op_frm_ts) : 0xffffffff;

	u4_ip_buf_len = 2048 * 1024;
	pu1_bs_buf = (UWORD8 *)malloc(u4_ip_buf_len);
	pu1_bsu_buf = (UWORD8 *)malloc(u4_ip_buf_len + (0.25 * u4_ip_buf_len));
	if ((pu1_bs_buf == NULL) || (pu1_bsu_buf == NULL))
	{
		sprintf(ac_error_str,
			"\nAllocation failure for input buffer or updated buffer of i4_size %d",
			u4_ip_buf_len);
		codec_exit(ac_error_str);
	}

	while (u8_op_frm_ts < max_op_frm_ts)
	{
		/***********************************************************************/
		/*   Seek the file to start of current frame, this is equavelent of    */
		/*   having a parcer which tells the start of current frame            */
		/***********************************************************************/
		{
			WORD32 numbytes;

			fseek(ps_ip_file, file_pos, SEEK_SET);
			numbytes = u4_ip_buf_len;

			u4_bytes_remaining = fread(pu1_bs_buf, sizeof(UWORD8),
				numbytes, ps_ip_file);
			if (u4_bytes_remaining == 0)
			{
				break;
			}
		}

		{
			fgcr_video_rewrite_ip_t s_video_decode_ip;
			fgcr_video_rewrite_op_t s_video_decode_op;
#ifdef PROFILE_ENABLE
			UWORD32 s_elapsed_time;
			TIMER s_start_timer;
			TIMER s_end_timer;
#endif

				s_video_decode_ip.e_cmd = FGCR_CMD_VIDEO_REWRITE;
				s_video_decode_ip.pv_stream_buffer = pu1_bs_buf;
				s_video_decode_ip.pv_stream_out_buffer = pu1_bsu_buf;
				s_video_decode_ip.u4_num_Bytes = u4_bytes_remaining;
				s_video_decode_ip.u4_size = sizeof(fgcr_video_rewrite_ip_t);

				s_video_decode_op.u4_size = sizeof(fgcr_video_rewrite_op_t);

				/*****************************************************************************/
				/*   API Call: Video Decode                                                  */
				/*****************************************************************************/

				GETTIME(&s_start_timer);
				ret = ivd_api_function((iv_obj_t *)codec_obj, (void *)&s_video_decode_ip,
					(void *)&s_video_decode_op);

			/*************************************************************************/
			/* Export FGC                                                            */
			/*************************************************************************/
			if (s_app_ctx.u1_mode == FGC_EXPORT)
			{
				static int fgc_count = 0;

				fgcr_ctl_set_fgc_params_t *ps_fgc_export_prms;
				fgcr_ctl_fgc_export_ip_t s_ctl_fgc_export_ip;
				fgcr_ctl_fgc_export_op_t     s_ctl_fgc_export_op;
				

				fgcr_ctl_set_fgc_params_t ps_fgc_export_prms_default;
				ps_fgc_export_prms_default = s_app_ctx.s_fgs_prms[0];
				
				ps_fgc_export_prms = &ps_fgc_export_prms_default;

				s_ctl_fgc_export_ip.e_cmd = FGCR_CMD_EXPORT;
				s_ctl_fgc_export_ip.ps_fgc_export_prms = (void *)ps_fgc_export_prms;
				s_ctl_fgc_export_ip.u4_size = sizeof(fgcr_ctl_fgc_export_ip_t);

				s_ctl_fgc_export_op.u4_size = sizeof(fgcr_ctl_fgc_export_op_t);

				GETTIME(&s_start_timer);
				ret = ivd_api_function((iv_obj_t *)codec_obj, (void *)&s_ctl_fgc_export_ip,
					(void *)&s_ctl_fgc_export_op);

				ps_fgc_export_prms = (fgcr_ctl_set_fgc_params_t *)s_ctl_fgc_export_ip.ps_fgc_export_prms;

				fprintf(ps_fgc_export_file, "----------------------------------------------------------------------\n");
				fprintf(ps_fgc_export_file, "FGC %d \n", ++fgc_count);
				fprintf(ps_fgc_export_file, "----------------------------------------------------------------------\n");
				fprintf(ps_fgc_export_file, "film_grain_characteristics_cancel_flag %d \n", ps_fgc_export_prms->u1_film_grain_characteristics_cancel_flag);
				fprintf(ps_fgc_export_file, "film_grain_model_id %d \n", ps_fgc_export_prms->u1_film_grain_model_id);
				fprintf(ps_fgc_export_file, "separate_colour_description_present_flag %d \n", ps_fgc_export_prms->u1_separate_colour_description_present_flag);
				if (ps_fgc_export_prms->u1_separate_colour_description_present_flag)
				{
					fprintf(ps_fgc_export_file, "film_grain_bit_depth_luma_minus8 %d \n", ps_fgc_export_prms->u1_film_grain_bit_depth_luma_minus8);
					fprintf(ps_fgc_export_file, "film_grain_bit_depth_chroma_minus8 %d \n", ps_fgc_export_prms->u1_film_grain_bit_depth_chroma_minus8);
					fprintf(ps_fgc_export_file, "film_grain_full_range_flag %d \n", ps_fgc_export_prms->u1_film_grain_full_range_flag);
					fprintf(ps_fgc_export_file, "film_grain_colour_primaries %d \n", ps_fgc_export_prms->u1_film_grain_colour_primaries);
					fprintf(ps_fgc_export_file, "film_grain_transfer_characteristics %d \n", ps_fgc_export_prms->u1_film_grain_transfer_characteristics);
					fprintf(ps_fgc_export_file, "film_grain_matrix_coefficients %d \n", ps_fgc_export_prms->u1_film_grain_matrix_coefficients);
				}
				fprintf(ps_fgc_export_file, "u1_blending_mode_id %d \n", ps_fgc_export_prms->u1_blending_mode_id);
				fprintf(ps_fgc_export_file, "u1_log2_scale_factor %d \n", ps_fgc_export_prms->u1_log2_scale_factor);
				
				for (int c = 0; c < 3; c++)
				{
					fprintf(ps_fgc_export_file, "comp_model_present_flag[%d] %d \n", c, ps_fgc_export_prms->u1_comp_model_present_flag[c]);
				}

				for (int c = 0; c < 3; c++)
				{
					if (ps_fgc_export_prms->u1_comp_model_present_flag[c])
					{
						fprintf(ps_fgc_export_file, "num_intensity_intervals_minus1[%d] %d \n", c, ps_fgc_export_prms->u1_num_intensity_intervals_minus1[c]);
						fprintf(ps_fgc_export_file, "num_model_values_minus1[%d] %d \n", c, ps_fgc_export_prms->u1_num_model_values_minus1[c]);
						for (int i = 0; i <= ps_fgc_export_prms->u1_num_intensity_intervals_minus1[c]; i++)
						{
							fprintf(ps_fgc_export_file, "intensity_interval_lower_bound[%d][%d] %d \n", c, i, ps_fgc_export_prms->u1_intensity_interval_lower_bound[c][i]);
							fprintf(ps_fgc_export_file, "intensity_interval_upper_bound[%d][%d] %d \n", c, i, ps_fgc_export_prms->u1_intensity_interval_upper_bound[c][i]);
							for (int j = 0; j <= ps_fgc_export_prms->u1_num_model_values_minus1[c]; j++)\
							{
								fprintf(ps_fgc_export_file, "comp_model_value[%d][%d][%d] %d \n", c, i, j, ps_fgc_export_prms->u4_comp_model_value[c][i][j]);
							}
						}
					}
				}

				fprintf(ps_fgc_export_file, "u1_film_grain_characteristics_persistence_flag %d \n", ps_fgc_export_prms->u1_film_grain_characteristics_persistence_flag);
				
			}

#ifdef PROFILE_ENABLE
			if (s_app_ctx.u1_enable_logs)
			{
				GETTIME(&s_end_timer);
				ELAPSEDTIME(s_start_timer, s_end_timer, s_elapsed_time, frequency);
				UWORD32 peak_avg, id;
				u4_tot_cycles += s_elapsed_time;
				peak_window[peak_window_idx++] = s_elapsed_time;
				if (peak_window_idx == PEAK_WINDOW_SIZE)
					peak_window_idx = 0;
				peak_avg = 0;
				for (id = 0; id < PEAK_WINDOW_SIZE; id++)
				{
					peak_avg += peak_window[id];
				}
				peak_avg /= PEAK_WINDOW_SIZE;
				if (peak_avg > peak_avg_max)
					peak_avg_max = peak_avg;
				frm_cnt++;
				u8_op_frm_ts++;
				printf("framenum: %4d timetaken(microsec): %6d avgtime: %6d peakavgtimemax: %6d numbytesconsumed: %6d numbytesgenerated: %6d \n", frm_cnt, s_elapsed_time, u4_tot_cycles / frm_cnt, peak_avg_max, s_video_decode_op.u4_num_bytes_consumed, s_video_decode_op.u4_num_bytes_generated);
			}
#else
			printf("%d\n", s_video_decode_op.u4_num_bytes_consumed);
#endif

			if (ret != IV_SUCCESS)
			{
				printf("Error in video Frame decode : ret %x Error %x\n", ret,
					s_video_decode_op.u4_error_code);
			}

			u4_num_bytes_dec = s_video_decode_op.u4_num_bytes_consumed;
			u4_num_bytes_generated = s_video_decode_op.u4_num_bytes_generated;
			fwrite(pu1_bsu_buf, sizeof(UWORD8), u4_num_bytes_generated, ps_fgs_upd_file);
			file_pos += u4_num_bytes_dec;
			total_bytes_comsumed += u4_num_bytes_dec;
			total_bytes_generated += u4_num_bytes_generated;
			u8_ip_frm_ts++;
		}
	}

#ifdef PROFILE_ENABLE
	if (s_app_ctx.u1_enable_logs)
	{
		printf("Summary\n");
		printf("Input filename                  : %s\n", s_app_ctx.ac_ip_fname);

		if (frm_cnt)
		{
			double avg = u4_tot_cycles / frm_cnt;
			double bytes_avg = total_bytes_comsumed / frm_cnt;
			double bitrate = (bytes_avg * 8 * s_app_ctx.fps) / 1000000;
			printf("Bitrate @ %2d fps(mbps)          : %-6.2f\n", s_app_ctx.fps, bitrate);
			printf("Average FGS-rewriter time(micro sec)  : %-6d\n", (WORD32)avg);
			printf("Avg Peak FGS-rewriter time(%2d frames) : %-6d\n", PEAK_WINDOW_SIZE, (WORD32)peak_avg_max);
			avg = (u4_tot_cycles + u4_tot_fmt_cycles)* 1.0 / frm_cnt;

			printf("FPS achieved (with format conv) : %-3.2f\n", 1000000 / avg);
		}
	}
#endif

	{
		fgcr_delete_ip_t s_delete_dec_ip;
		fgcr_delete_op_t s_delete_dec_op;

		s_delete_dec_ip.e_cmd = FGCR_CMD_DELETE;
		s_delete_dec_ip.u4_size = sizeof(fgcr_delete_ip_t);
		s_delete_dec_op.u4_size = sizeof(fgcr_delete_op_t);

		ret = ivd_api_function((iv_obj_t *)codec_obj, (void *)&s_delete_dec_ip,
			(void *)&s_delete_dec_op);

		if (IV_SUCCESS != ret)
		{
			sprintf(ac_error_str, "Error in Codec delete");
			codec_exit(ac_error_str);
		}
	}
	/***********************************************************************/
	/*              Close all the files and free all the memory            */
	/***********************************************************************/
	{
		fclose(ps_ip_file);
		fclose(ps_fgs_upd_file);
		fclose(ps_fgc_export_file);
	}

	free(pu1_bs_buf);
	free(pu1_bsu_buf);

	return (0);
}
