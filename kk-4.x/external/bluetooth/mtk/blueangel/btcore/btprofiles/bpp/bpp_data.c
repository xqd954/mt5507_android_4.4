/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *  bpp_data.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for BPP SOAP composing/parsing.
 *
 * Author:
 * -------
 *   Dexiong Xu, revised by Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 09 22 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integrate bluetooth code from //ALPS_SW_PERSONAL/sh.lai/10YW1040OF_CB/ into //ALPS_SW/TRUNK/ALPS/.
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifdef __ON_MAUI__
#include "kal_release.h"        /* Basic data type */
#endif

#include "stdio.h"      /* Basic C I/O functions */
#include "stdlib.h"     /* standard lib */

#include "bttypes.h"

#ifdef __ON_MAUI__
#include "fat_fs.h"     /* FS */
#else
#include "osapi.h"
#include "bt_adp_fs.h"  /* FS APIs */
#include "bt_adp_fs_ucs2.h"  /* FS APIs */
#include "bt_osal.h"  /* str APIs */

#define  FHANDLE kal_int32 /* used in xml_def.h */
#endif

#include "xml_def.h"    /* XML_PARSER */

#ifdef    __DRM_SUPPORT__  
#include "drm_def.h"
#include "Drm_gprot.h"
#endif

#include "goep.h"


#include "bluetooth_bpp_common.h"

#include "bpp.h"
#include "bppi.h"

/* for DEBUG : write the sending data to temp file */
#define  BPP_DEGUB_WRITE_SENDDOC_2FILE  1


#define FOUR_BITS_MASK          0x0f
#define FIVE_BITS_MASK          0x1F
#define SIX_BITS_MASK           0x3F
#define UTF8_FLW_BYTE_MASK   0x80
#define UTF8_FLW_BYTE_BITS   0x06
#define UTF8_TWO_BYTES_FIRST_MASK   0xC0
#define UTF8_THREE_BYTES_FIRST_MASK  0xE0


const kal_char bt_bpp_inline_item[] = "bpp-inline";
const kal_char bt_bpp_attr_ref_src[] = "src";
const kal_char bt_bpp_attr_ref_data[] = "data";
const kal_char bt_bpp_attr_type[] = "type";
const kal_char bt_bpp_attr_size[] = "inline-size";
const kal_char bt_bpp_mem_drv[] = "bpp_mem/";

#define BPP_PRINTER_ERROR_CODE_MAX 12


#define BPP_GET_PDU_LEN(buff_len,free_head,free_tail) \
	(kal_uint16)((buff_len)-(free_head)-(free_tail)-sizeof(bpp_head_buff_struct))   

#define BPP_GET_PULL_PDU_LEN(buff_len)  \
	BPP_GET_PDU_LEN((buff_len),BPP_PDU_FREE_HEADER,0)

#define BPP_GET_PUSH_PDU_LEN(buff_len,printer_mru,free_head,free_tail)\
	BPP_GET_PDU_LEN((printer_mru>(buff_len)? (buff_len):printer_mru),(free_head),(free_tail))


/* string */

#define BPP_CONSTANT_STRING

bpp_mime_id_str_struct bpp_mime_type_table[BPP_MIME_TYPE_MAX_ENUM + 1] = 
{
{BPP_MIME_TYPE_UNKNOWN, ""},
{BPP_MIME_APPLICATION_XHTML_PRINT_095,"application/vnd.pwg-xhtml-print+xml:0.95"},
{BPP_MIME_APPLICATION_XHTML_PRINT_100,"application/vnd.pwg-xhtml-print+xml:1.0"},
{BPP_MIME_APPLICATION_MULTIPLEXED,    "application/vnd.pwg-multiplexed"},
{BPP_MIME_TEXT_PLAIN,      "text/plain"},
{BPP_MIME_TEXT_VCARD,      "text/x-vcard:2.1"},
{BPP_MIME_TEXT_VCARD30,    "text/x-vcard:3.0"},
{BPP_MIME_TEXT_VCALENDAR,  "text/x-vcalendar:1.0"},
{BPP_MIME_TEXT_ICALENDAR20,"text/calendar:2.0"},
{BPP_MIME_TEXT_VMESSAGE,   "text/x-vmessage:1.1"},
{BPP_MIME_TEXT_VNOTE,      "text/x-vnote:1.1"},
{BPP_MIME_IMAGE_JPEG,"image/jpeg"},
{BPP_MIME_IMAGE_GIF, "image/gif"},
{BPP_MIME_IMAGE_BMP, "image/bmp"},
{BPP_MIME_IMAGE_WBMP,"image/vnd.wap.wbmp"},
{BPP_MIME_IMAGE_PNG, "image/png"},
{BPP_MIME_IMAGE_SVG, "image/svg+xml"},
{BPP_MIME_TYPE_MAX_ENUM, NULL}
};

#define BPP_MAX_ATTR_SIDED_NUM  3
struct
{
	bt_bpp_sided_enum sided_id;
	const kal_char * sided_str;

}bpp_sided_enum_str[BPP_MAX_ATTR_SIDED_NUM] = 
{
    {BPP_ONE_SIDED,            "one-sided"},
    {BPP_TWO_SIDED_LONG_EDGE,  "two-sided-long-edge"},
    {BPP_TWO_SIDED_SHORT_EDGE, "two-sided-short-edge"}
};

#define BPP_MAX_ATTR_ORIENT_NUM  4
struct
{
	bt_bpp_orient_enum orient_id;
	const kal_char * orient_str;

}bpp_orientation_enum_str[BPP_MAX_ATTR_ORIENT_NUM] = 
{
    {BPP_ORIENT_PORTRAIT,          "portrait"},
    {BPP_ORIENT_LANDSCAPE,         "landscape"},
    {BPP_ORIENT_REVERSE_PORTRAIT,  "reverse-portrait"},
    {BPP_ORIENT_REVERSE_LANDSCAPE, "reverse-landscape"}
};

#define BPP_MAX_ATTR_QUALITY_NUM  3
struct 
{
	bt_bpp_quality_enum quality_id;
	const kal_char * quality_str;
	
} bpp_print_quality_enum_str[BPP_MAX_ATTR_QUALITY_NUM] = 
{
    {BPP_QUALITY_DRAFT,  "draft"},
    {BPP_QUALITY_NORMAL, "normal"},
    {BPP_QUALITY_HIGH,   "high"}
};

struct bpp_attr_str
{
	U32 			 	attr_mask;
	const kal_char * 	main_attr_str;
	const kal_char * 	sub_attr_str;

}bpp_attr_str_table[BPP_ATTR_MAX_ATTRIBUTE + 1]=
{
	{BPP_PRINTER_NAME_MASK,					"PrinterName", NULL},
	{BPP_PRINTER_LOCATION_MASK,				"PrinterLocation", NULL},
	{BPP_PRINTER_STATE_MASK,				"PrinterState", NULL},
	{BPP_PRINTER_STATEREASONS_MASK,			"PrinterStateReasons", NULL},
	{BPP_DOCUMENT_FORMATS_SUPPORTED_MASK,	"DocumentFormatsSupported", "DocumentFormat"},
	{BPP_COLOR_SUPPORTED_MASK,				"ColorSupported", NULL},	
	{BPP_MAX_COPIES_SUPPORTED_MASK,			"MaxCopiesSupported", NULL},
	{BPP_SIDES_SUPPORTED_MASK,				"SidesSupported", "Sides"},
	{BPP_NUMBERUP_SUPPORTED_MASK,			"NumberUpSupported", NULL},
	{BPP_ORIENTATIONS_SUPPORTED_MASK,		"OrientationsSupported", "Orientation"},
	{BPP_MEDIA_SIZES_SUPPORTED_MASK,		"MediaSizesSupported", "MediaSize"},
	{BPP_MEDIA_TYPES_SUPPORTED_MASK,		"MediaTypesSupported", "MediaType"},
	{BPP_MEDIA_LOADED_MASK,					"MediaLoaded", NULL},
	{BPP_PRINT_QUALITY_SUPPORTED_MASK,		"PrintQualitySupported", "PrintQuality"}, 
	{BPP_QUEUED_JOB_COUNT_MASK,				"QueuedJobCount", NULL},
	{BPP_IMAGE_FORMATS_SUPPORTED_MASK,		"ImageFormatsSupported", "ImageFormat"},
	{BPP_BASIC_TEXT_PAGE_WIDTH_MASK,		"BasicTextPageWidth", NULL},
	{BPP_BASIC_TEXT_PAGE_HEIGHT_MASK,		"BasicTextPageHeight", NULL},
	{BPP_PRINTER_GENERALCURRENTOPERATOR_MASK,"PrinterGeneralCurrentOperator", NULL},

	{0,		"OperationStatus", NULL}
};


static const kal_char *bpp_media_type_enum_str[] = 
{
	"un-defined", /* undefined*/
    "stationery",
    "stationery-coated",
    "stationery-inkjet",
    "stationery-preprinted",
    "stationery-letterhead",
    "stationery-prepunched",
    "stationery-fine",
    "stationery-heavyweight",
    "stationery-lightweight",
    "transparency",
    "envelope",
    "envelope-plain",
    "envelope-window",
    "continuous",
    "continuous-long",
    "continuous-short",
    "tab-stock",
    "pre-cut-tabs",
    "full-cut-tabs",
    "multi-part-form",
    "labels",
    "multi-layer",
    "screen",
    "screen-paged",
    "photographic",
    "photographic-glossy",
    "photographic-high-gloss",
    "photographic-semi-gloss",
    "photographic-satin",
    "photographic-matte",
    "photographic-film",
    "back-print-film",
    "cardstock",
    "roll"
};

const kal_char* bpp_media_size_self_desc_name[BPP_MEDIA_SIZE_MAX_ENUM]=
{
  "iso_a10_26x37mm",
  "iso_a9_37x52mm",
  "iso_a8_52x74mm",
  "iso_a7_74x105mm",
  "iso_a6_105x148mm",
  "iso_a5_148x210mm",
  "iso_a5-extra_174x235mm",
  "iso_a4_210x297mm",
  "iso_a4-tab_225x297mm",
  "iso_a4-extra_235.5x322.3mm",
  "iso_a3_297x420mm",
  "iso_a2_420x594mm",
  "iso_a1_594x841mm",
  "iso_a0_841x1189mm",
  "iso_2a0_1189x1682mm",
  "iso_b10_31x44mm",
  "iso_b9_44x62mm",
  "iso_b8_62x88mm",
  "iso_b7_88x125mm",
  "iso_b6_125x176mm",
  "iso_b6c4_125x324mm",
  "iso_b5_176x250mm",
  "iso_b5-extra_201x276mm",
  "iso_b4_250x353mm",
  "iso_b3_353x500mm",
  "iso_b2_500x707mm",
  "iso_b1_707x1000mm",
  "iso_b0_1000x1414mm",
  "iso_c10_28x40mm",
  "iso_c9_40x57mm",
  "iso_c8_57x81mm",
  "iso_c7_81x114mm",
  "iso_c7c6_81x162mm",
  "iso_c6_114x162mm",
  "iso_c6c5_114x229mm",
  "iso_c5_162x229mm",
  "iso_c4_229x324mm",
  "iso_c3_324x458mm",
  "iso_c2_458x648mm",
  "iso_c1_648x917mm",
  "iso_c0_917x1297mm",       
  "na_index-4x6_4x6in",
  "na_letter_8.5x11in"
};

static const kal_char *bpp_oper_status_err_code[] = 
{
    "0x0000",
    "0x0001",
    "0x0002",
    "0x0400",
    "0x0401",
    "0x0402",
    "0x0403",
    "0x0404",
    "0x0405",
    "0x0406",
    "0x0407",
    "0x0408",
    "0x0409",
    "0x040A",
    "0x040B",
    "0x040C",
    "0x040D",
    "0x040E",
    "0x040F",
    "0x0410",
    "0x0411",
    "0x0412",
    "0x0418",
    "0x0500",
    "0x0501",
    "0x0502",
    "0x0503",
    "0x0504",
    "0x0505",
    "0x0506",
    "0x0507",
    "0x0508",
    "0x0509",
};

/*
const kal_char *bpp_oper_status_err_code_desc[] = 
{
    "successful-ok",
    "successful-ok-ignored-or-substituted-attributes",
    "successful-ok-conflicting-attributes",
    "client-error-bad-request",
    "client-error-forbidden",
    "client-error-not-authenticated",
    "client-error-not-authorized",
    "client-error-not-possible",
    "client-error-timeout",
    "client-error-not-found",
    "client-error-gone",
    "client-error-request-entity-too-large",
    "client-error-request-value-too-long",
    "client-error-document-format-not-supported",
    "client-error-attributes-or-values-not-supported",
    "client-error-uri-scheme-not-supported",
    "client-error-charset-not-supported",
    "client-error-conflicting-attributes",
    "client-error-compression-not-supported",
    "client-error-compression-error",
    "client-error-document-format-error",
    "client-error-document-access-error",
    "client-error-media-not-loaded",
    "server-error-internal-error",
    "server-error-operation-not-supported",
    "server-error-service-unavailable",
    "server-error-version-not-supported",
    "server-error-device-error",
    "server-error-temporary-error",
    "server-error-not-accepting-jobs",
    "server-error-busy",
    "server-error-job-canceled",
    "server-error-multiple-document-jobs-not-supported",
};
*/


const kal_char *bpp_state_reason_enum_str[BPP_PRINTER_ERROR_CODE_MAX] = 
{
    "none",
    "attention-required",
    "media-jam",
    "paused",
    "door-open",
    "media-low",
    "media-empty",
    "output-area-almost-full",
    "output-area-full",
    "marker-supply-low",
    "marker-supply-empty",
    "marker-failure",
};

static const kal_char *bpp_printer_state_enum_str[] = 
{
    "unknown",
    "idle",
    "processing",
    "stopped",
};

static const kal_char *bpp_job_state_enum_str[] = 
{
    "unknown",
    "printing",
    "waiting",
    "stopped",
    "completed",
    "aborted",
    "cancelled",
};


static const kal_char bpp_xhtml_print_mime_type[] = "application/vnd.pwg-xhtml-print+xml";

#ifdef __BPP_DEBUG__
//static const kal_char bpp_mime_type_xhtm_raw[] = "application/vnd.pwg-xhtml-print+xml:Raw";
#endif 

static const kal_char bpp_soap_mime_type_str[] = "x-obex/bt-SOAP";
//static const kal_char BPP_SOAP_ACTION_CREATE_JOB[] =
//    "SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#CreateJob\"\r\n\r\n";

static const kal_char bpp_content_length[] = "CONTENT-LENGTH: %d\r\n";
static const kal_char bpp_content_type[] = "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n";

static const kal_char bpp_require_soap_header[] =
    "SOAPACTION: \"urn:schemas-bluetooth-org:service:Printer:1#%s\"\r\n"
    "\r\n"
    "<s:Envelope\r\n"
    "  xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n"
    "  s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
    "<s:Body>\r\n" "<u:%s xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\r\n";

static const kal_char bpp_soap_body_end_template[] = "</u:%s>\r\n" "</s:Body>\r\n" "</s:Envelope>";

//static const kal_char bpp_soap_action_get_event[] = "GetEvent";
static const kal_char bpp_soap_action_create_job[] = "CreateJob";
static const kal_char bpp_soap_action_get_attr[] = "GetPrinterAttributes";

/* GetPrinterAttr contents */
static const kal_char  BPP_SOAP_GETPRTATTR_REQ_START                    []="<RequestedPrinterAttributes>\n";
static const kal_char  BPP_SOAP_GETPRTATTR_REQ_END                      []="</RequestedPrinterAttributes>\n";
static const kal_char  BPP_SOAP_GETPRTATTR_TAG_START                    []="<PrinterAttribute>";
static const kal_char  BPP_SOAP_GETPRTATTR_TAG_END                      []="</PrinterAttribute>\r\n";

/* CreateJob contents */
static const kal_char  BPP_SOAP_ATTR_JOB_NAME_START                      []="<JobName>";
static const kal_char  BPP_SOAP_ATTR_JOB_NAME_END                        []="</JobName>\n";
static const kal_char  BPP_SOAP_ATTR_DOCUMENT_FORMAT_START               []="<DocumentFormat>";
static const kal_char  BPP_SOAP_ATTR_DOCUMENT_FORMAT_END                 []="</DocumentFormat>\n";
static const kal_char  BPP_SOAP_ATTR_COPIES_START                        []="<Copies>";
static const kal_char  BPP_SOAP_ATTR_COPIES_END                          []="</Copies>\n";
static const kal_char  BPP_SOAP_ATTR_NUMBER_UP_START                     []="<NumberUp>";
static const kal_char  BPP_SOAP_ATTR_NUMBER_UP_END                       []="</NumberUp>\n";
static const kal_char  BPP_SOAP_ATTR_SIDES_START                         []="<Sides>";
static const kal_char  BPP_SOAP_ATTR_SIDES_END                           []="</Sides>\n";
static const kal_char  BPP_SOAP_ATTR_ORIENTATION_REQUESTED_START         []="<OrientationRequested>";
static const kal_char  BPP_SOAP_ATTR_ORIENTATION_REQUESTED_END           []="</OrientationRequested>\n";
static const kal_char  BPP_SOAP_ATTR_MEDIA_SIZE_START                    []="<MediaSize>";
static const kal_char  BPP_SOAP_ATTR_MEDIA_SIZE_END                      []="</MediaSize>\n";
static const kal_char  BPP_SOAP_ATTR_MEDIA_TYPE_START                    []="<MediaType>";
static const kal_char  BPP_SOAP_ATTR_MEDIA_TYPE_END                      []="</MediaType>\n";
static const kal_char  BPP_SOAP_ATTR_QUALITY_START                       []="<PrintQuality>";
static const kal_char  BPP_SOAP_ATTR_QUALITY_END                         []="</PrintQuality>\n";

static const kal_char BPP_XHTML_CONTENT_TYPE_STR[] =
    "Content-Type: application/vnd.pwg-multiplexed; type=application/vnd.pwg-xhtml-print+xml\r\n";

static const kal_char bpp_xhtml_multiplexed_type[] = "application/vnd.pwg-multiplexed";

static const kal_char BPP_SOAP_ATTR_NOT_AVAIL[] = "NA";
static const kal_char bpp_last_chk[] = "\r\nCHK 0 0 LAST\r\n";

static U8 XHTML_START_10[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML-Print 1.0//EN\"\r\n"
    "\"http://www.w3.org/MarkUp/DTD/xhtml-print10.dtd\">\r\n"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n" "\r\n";



#define DATA_TYPE_READ_TABLE

static bpp_head_buff_struct *bpp_read_text_in_xhtml(bpp_send_obj_struct *pdata, void *param);

static bpp_head_buff_struct *bpp_read_image_in_xhtml(bpp_send_obj_struct *pdata, void *param);

static bpp_head_buff_struct *bpp_read_bin(bpp_send_obj_struct *pdata, void *param);

static bpp_head_buff_struct *bpp_read_xhtml(bpp_send_obj_struct *pdata, void *param);

/* macro defination */

/* set flag */
#define BPP_SET_INDEXED_FLAG(enum_str,enum_str_array,pflag_array) \
	bpp_set_indexed_flag(enum_str,enum_str_array,sizeof(enum_str_array)/sizeof(kal_char*),pflag_array)

#define BPP_GET_OPER_CONTEXT(pobject)     (bpp_oper_context_struct*)((kal_uint32)pobject-(kal_uint32)&(((bpp_oper_context_struct*)0)->object.send_obj))

/* write UTF8 to buffer */
#define write_utf82buf(pbuf,buf_remain,utf8str)\
do{                                            \
    len = strlen(utf8str);                     \
    if(len < buf_remain)                       \
    {                                          \
        strncpy(pbuf,utf8str,len);             \
        buf_remain-=len;                       \
        pbuf+= len;                            \
    }                                          \
    else                                       \
    {                                          \
        return KAL_FALSE;                      \
    }                                          \
 }while(0);


#define STATIC_FUNC_DECLARARTION


static kal_int32 bpp_util_find_and_replace(kal_char* pbuf, kal_uint32 size, kal_char* src, kal_char* dest);

static kal_int32 bpp_replace_scr_ref(kal_char* pbuf, kal_uint32 size, bpp_send_obj_struct * pchild);

kal_uint32 bpp_cover_ucs2utf8(
            const kal_uint8 *pucs2buf,
            kal_uint32 *pucs2_buf_len,
            kal_uint8 *putf8buf,
            kal_uint32 utf8_buf_len,
            kal_bool big_endian);

static BPP_ERROR_CODE bpp_load_data_from_file(bpp_send_obj_struct *psend_obj);

static kal_bool bpp_pdu_add_chk_header(bpp_head_buff_struct *pdu_out, bpp_send_obj_struct *pdata);

static bpp_head_buff_struct *bpp_compose_multiplexed_data(bpp_send_obj_struct *psend_obj);

static kal_bool bpp_get_soap_content_len(kal_char* pbuf, kal_uint16 size, kal_uint16* pcontent_head_len, kal_uint16* pcontent_len);


static void bpp_xhtml_parser_begin_buf(bpp_token_parser_struct *parser, kal_char *pbuf, kal_uint16 size);

static kal_char *bpp_xhtml_get_token_val(bpp_token_parser_struct *parser);

static bpp_token_enum bpp_xhtml_get_el_token(bpp_token_parser_struct *parser);

static bpp_xhtml_tag_struct *bpp_parser_item(bpp_token_parser_struct *parser, const kal_char *item_name);

static kal_uint32 bpp_set_indexed_flag(
                    const char *enum_str,
                    const char **enum_str_array,
                    kal_uint32 array_size,
                    kal_uint8 *pflag_array);

static bpp_send_obj_struct *bpp_get_inline_item(bpp_token_parser_struct *parser);

static void bpp_destory_buf_obj(bpp_buf_obj_struct *pbuf_obj);

static kal_char *bpp_get_postfix_from_mime_type(bt_bpp_mime_type type);
static kal_char * bpp_get_mime_str_from_filename(kal_char * filename, bt_bpp_mime_type* p_typeid);

static int bpp_HexStrToInt(kal_char * s);


#define INIT_DATA_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_init_get_attr_context
 * DESCRIPTION
 *  This function init get attr context
 * PARAMETERS
 *  get_attr_context     
 *  attr_id_bitmask: attr bitmask
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_get_attr_context(
                    bpp_oper_context_struct *get_attr_context,
                    U32 attr_id_bitmask)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_get_obj_struct *get_obj = &get_attr_context->object.get_obj;
    kal_char *pbuf = NULL;
    kal_uint16 total_len = 0;
    kal_uint16 max_pdu_len = BPP_GET_PULL_PDU_LEN(BPP_MAX_PULL_BUFF_LEN*2);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(max_pdu_len > 0);
    BPP_ASSERT(get_attr_context);

    get_attr_context->current_req = BPP_OPER_GET_ATTRIBUTE;

    get_obj->pdu_in = NULL;
    get_obj->pfile_obj = NULL;
    get_obj->parser_param = NULL;
    strcpy(get_obj->mime_type, bpp_soap_mime_type_str);

    get_attr_context->pdu_out = (bpp_head_buff_struct*) bpp_alloc_head_buf(max_pdu_len, BPP_PDU_FREE_HEADER, 0, TD_RESET);
    pbuf = bpp_get_pdu_ptr(get_attr_context->pdu_out, (kal_uint16*) & total_len);

    if (pbuf)
    {
        kal_char *ptemp = pbuf;
        kal_char content_len_str[64];
        kal_uint16 buf_remain = total_len;
        kal_int16 i = 0;
        kal_uint16 len = 0;

        len += sprintf(pbuf, bpp_require_soap_header, bpp_soap_action_get_attr, bpp_soap_action_get_attr);

        ptemp = pbuf + len;
        buf_remain -= len;

		if (attr_id_bitmask > 0)
		{
	        write_utf82buf(ptemp, buf_remain, BPP_SOAP_GETPRTATTR_REQ_START);

	        for (i = 0; i < BPP_ATTR_MAX_ATTRIBUTE; i++)
	        {
				if (attr_id_bitmask & bpp_attr_str_table[i].attr_mask)
				{
	                write_utf82buf(ptemp, buf_remain, BPP_SOAP_GETPRTATTR_TAG_START);
	                write_utf82buf(ptemp, buf_remain, bpp_attr_str_table[i].main_attr_str);
	                write_utf82buf(ptemp, buf_remain, BPP_SOAP_GETPRTATTR_TAG_END);
				}
	        }

	        write_utf82buf(ptemp, buf_remain, BPP_SOAP_GETPRTATTR_REQ_END);
		}

        len = sprintf(ptemp, bpp_soap_body_end_template, bpp_soap_action_get_attr);
        ASSERT(buf_remain >= len);
        buf_remain -= len;
        
        get_attr_context->pdu_out->free_tail_space += buf_remain;
        get_attr_context->pdu_out->pdu_len -= buf_remain;
        total_len -= buf_remain;

        bpp_pdu_add_header(get_attr_context->pdu_out,bpp_content_type,strlen(bpp_content_type));

        sprintf(content_len_str, bpp_content_length,total_len);
        bpp_pdu_add_header(get_attr_context->pdu_out,content_len_str,strlen(content_len_str));

        return KAL_TRUE;
    }

    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_destory_file_obj
 * DESCRIPTION
 *  This function destory operation context
 * PARAMETERS
 *  pfile_obj       
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_destory_file_obj(bpp_file_obj_struct *pfile_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pfile_obj)
    {
        if (pfile_obj->fh > 0)
        {
            BPP_Debug_x_FS_Close(pfile_obj->fh);
        }
        if (pfile_obj->btemp_file == KAL_TRUE && btmtk_wstrlen(pfile_obj->file_name) > 0)
        {
            bpp_fs_delete((WCHAR*) pfile_obj->file_name);
        }
		
        bpp_mem_free(pfile_obj);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bpp_init_send_obj
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psend_obj       
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_init_send_obj(bpp_send_obj_struct *psend_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_MemSet ((U8*)psend_obj, 0, sizeof(*psend_obj));

    psend_obj->pkt_type = BPP_FIRST_PKT_BIT;

    psend_obj->chk_id = 1;
    psend_obj->total_chk_id = 2;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_deinit_send_obj
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psend_obj  
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_deinit_send_obj(bpp_send_obj_struct *psend_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (psend_obj)
    {
        if (psend_obj->pfile_obj)
        {
            bpp_destory_file_obj(psend_obj->pfile_obj);
        }
        
        if(psend_obj->pbuf_obj)
        {
            bpp_destory_buf_obj(psend_obj->pbuf_obj);
        }
        
        if (psend_obj->pinline_obj)
        {
            bpp_deinit_send_obj(psend_obj->pinline_obj);
            bpp_mem_free(psend_obj->pinline_obj);
        }

		if (psend_obj->simple_data.buf)
		{
			bpp_mem_free(psend_obj->simple_data.buf);
		}
		
        if (psend_obj->pdu_in)
        {
            bpp_free_head_buf(psend_obj->pdu_in);
        }

        if (psend_obj->p_pending_tag)
        {
            bpp_mem_free(psend_obj->p_pending_tag);
        }

		OS_MemSet((U8*)psend_obj, 0, sizeof(*psend_obj));
		
        psend_obj->chk_id = 1;
        psend_obj->total_chk_id = 2;
    }

}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_send_doc_context
 * DESCRIPTION
 *  This function init send doc context
 * PARAMETERS
 *  pcontext        
 *  print_object   
 *  file_flag: if the print doc is temp file
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_send_doc_context(
                    bpp_oper_context_struct *pcontext,
                    bt_bpp_object *print_object,
                    bpp_print_file_flag file_flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_send_obj_struct *psend_obj = &pcontext->object.send_obj;
    kal_bool ret = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pcontext->current_req = BPP_OPER_PRINT_DOC;
    pcontext->data_len = print_object->object_size;
    pcontext->pdu_out = NULL;

    bpp_init_send_obj(psend_obj);

	psend_obj->mime_id = print_object->mime_type;

	if (psend_obj->mime_id == BPP_MIME_APPLICATION_XHTML_PRINT_095 || 
		psend_obj->mime_id == BPP_MIME_APPLICATION_XHTML_PRINT_100 || 
		psend_obj->mime_id == BPP_MIME_APPLICATION_MULTIPLEXED )
	{
		/* xhtml need parse to see if it has in-line object */
		psend_obj->data_type = BPP_XHTML_DATA;
	}
	else
	{
		/* simple data: send the raw data */
		psend_obj->data_type = BPP_SIMPLE_DATA;
	}

	if (print_object->mime_type < BPP_MIME_TYPE_MAX_ENUM)
	{
		psend_obj->mime_str = (kal_char*)bpp_mime_type_table[print_object->mime_type].type_str;

		/* to keep mime_str is same as the "type" in BPP_XHTML_CONTENT_TYPE_STR */
		if (print_object->mime_type == BPP_MIME_APPLICATION_XHTML_PRINT_100 ||
			print_object->mime_type == BPP_MIME_APPLICATION_XHTML_PRINT_095)
		{
			psend_obj->mime_str = (kal_char*)bpp_xhtml_print_mime_type;
		}
	}
	else
	{
		return KAL_FALSE;
	}

    if (btmtk_wstrlen(print_object->file_name))
    {
        bpp_file_obj_struct *pfile_obj;

		psend_obj->pfile_obj = bpp_mem_alloc(sizeof(bpp_file_obj_struct));

		pfile_obj = psend_obj->pfile_obj;

		if (file_flag == BPP_FILE_FLAG_TEMP)
        {
            pfile_obj->btemp_file = KAL_TRUE;
        }
        else
    	{
            pfile_obj->btemp_file = KAL_FALSE;
    	}

        pfile_obj->file_rsize = 0;
        pfile_obj->fh = bpp_fs_open((WCHAR*) print_object->file_name, BTMTK_FS_READ_ONLY);

        if (pfile_obj->fh > 0)
        {
            btmtk_wstrcpy((U16*) pfile_obj->file_name, (U16*) print_object->file_name);
            bpp_fs_getfilesize(pfile_obj->fh, (unsigned int*)&pfile_obj->file_rsize);
        }
        else
        {
            bpp_mem_free(psend_obj->pfile_obj);
            ret = KAL_FALSE;
        }

        if (pcontext->data_len == 0)
        {
            pcontext->data_len = pfile_obj->file_rsize;
        }

    }
    else
    {
        ret = KAL_FALSE;
    }

    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_create_job_context
 * DESCRIPTION
 *  This function init create job context
 * PARAMETERS
 *  create_job_oper_context  
 *  print_object     
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_create_job_context(
                    bpp_oper_context_struct *create_job_oper_context,
                    bt_bpp_object *print_object)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_get_obj_struct *get_obj = &create_job_oper_context->object.get_obj;
    kal_char *pbuf = NULL;
    kal_uint16 total_len = 0;
    kal_uint16 max_pdu_len = BPP_GET_PULL_PDU_LEN(BPP_MAX_PULL_BUFF_LEN); //BPP_MAX_PDU_LEN(create_job_oper_context->printer->printer_mru, BPP_PDU_FREE_HEADER, 0);
    char *p_doc_mime;
	U8 i;
	kal_char *p_str;

    ASSERT(max_pdu_len > 0);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BPP_ASSERT(create_job_oper_context && print_object);

    create_job_oper_context->current_req = BPP_OPER_CREATE_JOB;

    strcpy(get_obj->mime_type, bpp_soap_mime_type_str);
    get_obj->pdu_in = NULL;
    get_obj->pfile_obj = NULL;
    get_obj->parser_param = NULL;

    create_job_oper_context->pdu_out =
        (bpp_head_buff_struct*) bpp_alloc_head_buf(max_pdu_len, BPP_PDU_FREE_HEADER, 0, TD_RESET);
	
    pbuf = bpp_get_pdu_ptr(create_job_oper_context->pdu_out, (kal_uint16*) & total_len);

	ASSERT(print_object->mime_type < BPP_MIME_TYPE_MAX_ENUM);

	p_doc_mime = bpp_get_doc_format(print_object->mime_type);

    if (pbuf)
    {
        kal_char *ptemp = pbuf;
        kal_char temp_buf[64];
        kal_uint16 buf_remain = total_len;
        kal_uint16 len = 0;

        len += sprintf(pbuf, bpp_require_soap_header, bpp_soap_action_create_job, bpp_soap_action_create_job);

        ptemp = pbuf + len;
        buf_remain -= len;
        //if(print_object->print_model==BPP_JOB_BASE_NOT_DEFAULT_PRINT)
        {
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_JOB_NAME_START);
#if 0
            if (print_object->job_name)
            {
                write_utf82buf(ptemp, buf_remain, print_object->job_name);
            }
            else
#endif
            {
                write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_NOT_AVAIL);
            }
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_JOB_NAME_END);

            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_DOCUMENT_FORMAT_START);
            write_utf82buf(ptemp, buf_remain, p_doc_mime);
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_DOCUMENT_FORMAT_END);

            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_COPIES_START);
            sprintf(temp_buf, "%d", print_object->job_config.copies);
            write_utf82buf(ptemp, buf_remain, temp_buf);
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_COPIES_END);

			/* sided attribute */
			p_str = NULL;
			for (i = 0; i < BPP_MAX_ATTR_SIDED_NUM; i++)
			{
				if(print_object->job_config.sides == bpp_sided_enum_str[i].sided_id)
				{
				    p_str = (kal_char*)bpp_sided_enum_str[i].sided_str;
					break;
				}
			}

			if (p_str != NULL)
			{
                write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_SIDES_START);
	            write_utf82buf(ptemp, buf_remain, p_str);
                write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_SIDES_END);

			}
			/* number up */
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_NUMBER_UP_START);
            sprintf(temp_buf, "%d", print_object->job_config.numberup);
            write_utf82buf(ptemp, buf_remain, temp_buf);
            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_NUMBER_UP_END);

			/* orientation */
			p_str = NULL;
			for (i = 0; i < BPP_MAX_ATTR_ORIENT_NUM; i++)
			{
				if(print_object->job_config.orient == bpp_orientation_enum_str[i].orient_id)
				{
				    p_str = (kal_char*)bpp_orientation_enum_str[i].orient_str;
					break;
				}
			}

			if (p_str != NULL)
			{
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_ORIENTATION_REQUESTED_START);
	            write_utf82buf(ptemp, buf_remain, p_str);
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_ORIENTATION_REQUESTED_END);
			}

			/* media size */
			if (print_object->job_config.media_size > 0 &&
				print_object->job_config.media_size < BPP_MEDIA_SIZE_MAX_ENUM)
			{
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_MEDIA_SIZE_START);
	            write_utf82buf(ptemp, buf_remain, bpp_media_size_self_desc_name[print_object->job_config.media_size]);
	            //write_utf82buf(ptemp, buf_remain, print_object->job_config.media_size);
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_MEDIA_SIZE_END);
			}

			/* media type */
			if (print_object->job_config.media_type > 0 &&
				print_object->job_config.media_type < BPP_MEDIA_TYPE_MAX_ENUM)
			{
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_MEDIA_TYPE_START);
	            write_utf82buf(ptemp, buf_remain, bpp_media_type_enum_str[print_object->job_config.media_type]);
	            write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_MEDIA_TYPE_END);
			}
			
			/* quality */
			p_str = NULL;
			for (i = 0; i < BPP_MAX_ATTR_QUALITY_NUM; i++)
			{
				if(print_object->job_config.quality == bpp_print_quality_enum_str[i].quality_id)
				{
				    p_str = (kal_char*)bpp_print_quality_enum_str[i].quality_str;
					break;
				}
			}

			if (p_str != NULL)
			{
                write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_QUALITY_START);
	            write_utf82buf(ptemp, buf_remain, p_str);
                write_utf82buf(ptemp, buf_remain, BPP_SOAP_ATTR_QUALITY_END);
            }
        }

        buf_remain -= sprintf(ptemp, bpp_soap_body_end_template, bpp_soap_action_create_job);
        ASSERT(buf_remain > 0);

        
        create_job_oper_context->pdu_out->free_tail_space += buf_remain;
        create_job_oper_context->pdu_out->pdu_len -= buf_remain;
        total_len -= buf_remain;

        bpp_pdu_add_header(create_job_oper_context->pdu_out,bpp_content_type,strlen(bpp_content_type));

        sprintf(temp_buf, bpp_content_length,total_len);
        bpp_pdu_add_header(create_job_oper_context->pdu_out,temp_buf,strlen(temp_buf));

        return KAL_TRUE;
    }

    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_get_event_context
 * DESCRIPTION
 *  This function init get event context
 * PARAMETERS
 *  get_event_context 
 *  job_id  
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_get_event_context(bpp_oper_context_struct *get_event_context, kal_uint32 job_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    const kal_char get_event_cmd[] = "GetEvent";
	kal_bool ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    get_event_context->current_req = BPP_OPER_GET_EVENT;
    get_event_context->data_len = 0;

	ret = bpp_init_soap_req_with_only_jobid(get_event_context, get_event_cmd, job_id);
	
    return ret;

}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_cancel_job_context
 * DESCRIPTION
 *  This function init cancel job context
 * PARAMETERS
 *  cancel_job_context 
 *  job_id            
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_cancel_job_context(bpp_oper_context_struct *cancel_job_context, kal_uint32 job_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    const kal_char cancel_job_cmd[] = "CancelJob";
	kal_bool ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cancel_job_context->current_req = BPP_OPER_CANCEL_JOB;

	ret = bpp_init_soap_req_with_only_jobid(cancel_job_context, cancel_job_cmd, job_id);
	
    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_soap_req_context
 * DESCRIPTION
 *  This function init scoap req context
 * PARAMETERS
 *  get_context         
 *  pcmdstr             
 *  job_id          
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_init_soap_req_with_only_jobid(
                    bpp_oper_context_struct *get_context,
                    const kal_char *pcmdstr,
                    kal_int32 job_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_char *pbuf = NULL;
    kal_uint16 total_len = 0;
    bpp_get_obj_struct *get_obj = &get_context->object.get_obj;
    kal_uint16 max_pdu_len = BPP_GET_PULL_PDU_LEN(BPP_MAX_PULL_BUFF_LEN); //BPP_MAX_PDU_LEN(get_context->printer->printer_mru, BPP_PDU_FREE_HEADER, 0);;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(max_pdu_len > 0);

    get_obj->pfile_obj = NULL;
    get_obj->pdu_in = NULL;
    get_obj->parser_param = NULL;
    strcpy(get_obj->mime_type, bpp_soap_mime_type_str);

    get_context->pdu_out = (bpp_head_buff_struct*) bpp_alloc_head_buf(max_pdu_len, BPP_PDU_FREE_HEADER, 0, TD_RESET);
    pbuf = bpp_get_pdu_ptr(get_context->pdu_out, (kal_uint16*) & total_len);

    if (pbuf)
    {
        kal_char content_len_str[64];
        kal_uint16 len = 0;

        len += sprintf(pbuf, bpp_require_soap_header, pcmdstr, pcmdstr);
        len += sprintf(pbuf + len, "<JobId>%lu</JobId>\n", job_id);
        len += sprintf(pbuf + len, bpp_soap_body_end_template, pcmdstr);

        ASSERT(total_len >= len);
        
        get_context->pdu_out->free_tail_space = total_len-len;
        get_context->pdu_out->pdu_len = len;

        bpp_pdu_add_header(get_context->pdu_out,bpp_content_type,strlen(bpp_content_type));

        sprintf(content_len_str, bpp_content_length,len);
        bpp_pdu_add_header(get_context->pdu_out,content_len_str,strlen(content_len_str));

        return KAL_TRUE;
    }

    return KAL_FALSE;
}



#define OBS_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_obs_read_doc
 * DESCRIPTION
 *  This function read data to goep buf
 * PARAMETERS
 *  obs    
 *  pbuf   
 *  len      
 * RETURNS
 *  void
 *****************************************************************************/
ObexRespCode bpp_obs_read_doc(void *obs, kal_uint8 *pbuf, kal_uint16 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(0);

    return OBRC_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_obs_write_soaprsp
 * DESCRIPTION
 *  This function write response soap data to file
 * PARAMETERS
 *  obs     
 *  pbuf   
 *  len        
 * RETURNS
 *  void
 *****************************************************************************/
ObexRespCode bpp_obs_write_soaprsp(void *obs, kal_uint8 *pbuf, kal_uint16 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *pcontext = (bpp_oper_context_struct*) obs;
    bpp_get_obj_struct *pobj = pcontext ? &pcontext->object.get_obj : NULL;
//mtk03036 20100506
//move to btcore/btprofiles/bpp/bppi.h
//#define BPP_IN_SOAP_FILE  L"\\BppSoapRsp.xml"
//mtk03036 20100506 end	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_TRACE_INFO_ARG2(BPP_OBS_WRITE_SOAPRSP, (kal_int32)len,(kal_int32)pcontext->current_req);		
    BPP_DB_SOAP_IN((kal_char*)pbuf, len);
	
    if (pobj && pobj->pdu_in && strcmp(pobj->mime_type, bpp_soap_mime_type_str) == 0)
    {
        kal_uint16 pdu_len = 0;
        kal_char *pdata = bpp_get_pdu_ptr(pobj->pdu_in, &pdu_len);
        kal_bool try_again = KAL_TRUE;
        
        /*All the data will be saved to the buf first, If the buf is not too large, 
          it will be saved the existing buf data to the file, and all the current 
          data will save to the buf*/
        while (1)
        {
            if (pobj->pdu_in->free_tail_space > len)
            {
                pdata += pdu_len;
                OS_MemCopy((U8*)pdata, pbuf, len);
                pobj->pdu_in->pdu_len += len;
                pobj->pdu_in->free_tail_space -= len;
                break;
            }
            else
            {
                bpp_file_obj_struct *pfile_obj = pobj->pfile_obj;
//mtk03036 20100510
                //kal_uint16 len;
                int len;
//mtk03036 20100510 end
                kal_int32  f_ret = 0;

                if (pobj->pfile_obj == NULL)
                {
                    pfile_obj = pobj->pfile_obj = bpp_mem_alloc(sizeof(bpp_file_obj_struct));
                    pfile_obj->btemp_file = KAL_TRUE;
                    pfile_obj->file_rsize = 0;
					
                    bpp_get_private_folder((kal_char*) pfile_obj->file_name);

					btmtk_wstrcat(pfile_obj->file_name, BPP_IN_SOAP_FILE);
					
//mtk03036 20100507
                    //pfile_obj->fh = bpp_fs_open((WCHAR*) pfile_obj->file_name, BTMTK_FS_CREATE_ALWAYS);
                    pfile_obj->fh = bpp_fs_open((WCHAR*) pfile_obj->file_name,BTMTK_FS_CREATE_ALWAYS |BTMTK_FS_CREATE | BTMTK_FS_READ_WRITE);
//mtk03036 20100507 end
                }

                if (try_again == KAL_TRUE && pfile_obj->fh > 0 && 
                    (f_ret = bpp_fs_write(pfile_obj->fh, pdata, pdu_len, &len)) >= 0 &&
                    len == pdu_len)
                {
                    pdu_len = 0;
                    pobj->pdu_in->pdu_len -= len;
                    pobj->pdu_in->free_tail_space += len;
                    try_again = KAL_FALSE;
                }
                else
                {
					BT_BPP_TRACE_INFO_ARG3(BPP_WRITE_SOAPRSP_FAILED, try_again,pfile_obj->fh,f_ret);
                    return OBRC_NO_CONTENT;
                }
            }
        }
    }
    else
    {
        BPP_ASSERT(0);
        return OBRC_NO_CONTENT;
    }

    return OBRC_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_obs_get_objlen
 * DESCRIPTION
 *  This function return object len from bpp,which call by obex
 * PARAMETERS
 *  obs 
 * RETURNS
 *  void
 *****************************************************************************/
kal_uint32 bpp_obs_get_objlen(void *obs)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *pcontext = (bpp_oper_context_struct*) obs;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return pcontext->data_len;
}


#define SOAP_COMPOSE_PARSE_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_parse_rsp_soap
 * DESCRIPTION
 *  This function parse rsp soap
 * PARAMETERS
 *  pobj      
 *  data_hdlr       
 *  val_store     
 * RETURNS
 *  void
 *****************************************************************************/
static kal_bool bpp_parse_rsp_soap(bpp_get_obj_struct *pobj, XML_data_hdlr data_hdlr, void *val_store)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    XML_PARSER_STRUCT *xml_parser_p;
	kal_int32 parse_ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pobj && ((pobj->pdu_in && pobj->pdu_in->pdu_len) || (pobj->pfile_obj && pobj->pfile_obj->fh > 0)))
    {
        bt_bpp_xml_parser_parm_struct parser_parm;
        xml_parser_p = bpp_mem_alloc(sizeof(XML_PARSER_STRUCT));
        
        if (xml_parser_p)
        {
            kal_uint32 buf_len = 0;
            kal_char *pbuf = bpp_get_pdu_ptr(pobj->pdu_in, (kal_uint16*) & buf_len);

            OS_MemSet((U8*) xml_parser_p, 0, sizeof(XML_PARSER_STRUCT));

            //if (val_store && xml_new_parser(xml_parser_p) == 0)
            if (val_store)
            {
				xml_new_parser(xml_parser_p);
			
                xml_register_data_handler(xml_parser_p, data_hdlr);

                if (pobj->parser_param)
                {
                    bpp_mem_free(pobj->parser_param);
                }
                
                parser_parm.xml_parser_p = xml_parser_p;
                parser_parm.parser_output = (void*)val_store;
                pobj->parser_param = &parser_parm;
                
                if (pobj->pfile_obj && pobj->pfile_obj->fh > 0)
                {
                	/*If the data is too large, all the remain data will save to this file*/
                    int len;

                    bpp_fs_write(pobj->pfile_obj->fh, pbuf, buf_len, &len);
                    BPP_Debug_x_FS_Close(pobj->pfile_obj->fh);
                    pobj->pfile_obj->fh = -1;
					
                    parse_ret = xml_parse(xml_parser_p, (U8*)pobj->pfile_obj->file_name);

                }
                else
                {
                    parse_ret = xml_parse_buffer(xml_parser_p, pbuf, buf_len);
                }

				BT_BPP_TRACE_INFO_ARG1(BPP_PARSE_SOAPRSP_RETURN, parse_ret);
            }
			
            pobj->parser_param = NULL;
            xml_close_parser(xml_parser_p);
			
            bpp_mem_free(xml_parser_p);

			if(pobj->pfile_obj && (pobj->pfile_obj->btemp_file == KAL_TRUE))
            {
                bpp_fs_delete(pobj->pfile_obj->file_name);
            }
			
            return KAL_TRUE;
        }
    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_get_attr_rsp_data_hdlr
 * DESCRIPTION
 *  This function  parse get arrtr rsp soap
 * PARAMETERS
 *  data    
 *  el          
 *  value       
 *  len        
 *  error         
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_get_attr_rsp_data_hdlr(void *data, const char *el, const char *value, kal_int32 len, kal_int32 error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bpp_get_printer_attr_resp *attr_rsp = NULL;
    bt_bpp_printer_attributes *prtr_attr = NULL;
    bt_bpp_printer_capability *pcapability = NULL;
    const kal_char *attr_name = NULL;
    kal_int32 attr_enum = 0;
    bpp_oper_context_struct * current_oper = NULL;
	U8 i;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    current_oper = bpp_find_oper_context(data,BPP_OPER_GET_ATTRIBUTE);

    if (current_oper != NULL)
    {
        attr_rsp = (bpp_get_printer_attr_resp*) (current_oper->object.get_obj.parser_param->parser_output);
		prtr_attr = attr_rsp->p_attr;
        pcapability = &prtr_attr->capability;
    }

    for (attr_enum = 0; attr_enum < BPP_ATTR_MAX_ATTRIBUTE; attr_enum++)
    {
    	if (bpp_attr_str_table[attr_enum].sub_attr_str != NULL)
    	{
    	    attr_name = bpp_attr_str_table[attr_enum].sub_attr_str;
    	}
		else
    	{
	        attr_name = bpp_attr_str_table[attr_enum].main_attr_str;
    	}

        if (strcmp(attr_name, el) == 0)
        {
            break;
        }
    }

    switch (attr_enum)
    {
        case BPP_ATTR_MAXCOPIESSUPPORTED:
            sscanf(value, "%lu", &pcapability->max_copies_supported);
            break;

        case BPP_ATTR_NUMBERUPSUPPORTED:
            sscanf(value, "%lu", &pcapability->max_numberup_supported);
            break;

        case BPP_ATTR_PRINTERSTATE:
            prtr_attr->printer_state = (bt_bpp_printer_state_enum)BPP_SET_INDEXED_FLAG(value, bpp_printer_state_enum_str, NULL);
            break;

        case BPP_ATTR_PRINTERSTATEREASONS:
            prtr_attr->state_reason = (bt_bpp_printer_state_reason)BPP_SET_INDEXED_FLAG(value, bpp_state_reason_enum_str, NULL);
            break;

        case BPP_ATTR_DOCUMENTFORMATSSUPPORTED:
			if (pcapability->docfmt_num < BPP_MAX_NESTED_ATTR_NUM)
			{
	            for (i = 0; i < BPP_MIME_TYPE_MAX_ENUM; i++)
	            {
	            	if (bpp_mime_type_table[i].type_str != NULL && 
						strcmp(value, (char*)bpp_mime_type_table[i].type_str) == 0)
	            	{
	            		pcapability->doc_format_supported[pcapability->docfmt_num] = bpp_mime_type_table[i].type_id;
						pcapability->docfmt_num++;
	            		break;
	            	}
	            }
			}
				
            break;

        case BPP_ATTR_IMAGEFORMATSSUPPORTED:
			if (pcapability->imgfmt_num < BPP_MAX_IMGFMT_SUPPORTED_NUM)
			{
	            for (i = BPP_MIME_IMAGE_JPEG; i < BPP_MIME_TYPE_MAX_ENUM; i++)
	            {
	            	/* "image/jpeg", or only "jpeg"*/
	            	if (strcmp(value, (char*)bpp_mime_type_table[i].type_str) == 0 ||
						strstr((char*)bpp_mime_type_table[i].type_str, value) != NULL)
	            	{
	            		pcapability->image_format_supported[pcapability->imgfmt_num] = bpp_mime_type_table[i].type_id;
						pcapability->imgfmt_num++;
	            		break;
	            	}
	            }
            }
            break;

        case BPP_ATTR_SIDESSUPPORTED:
            for (i = 0; i < BPP_MAX_ATTR_SIDED_NUM; i++)
            {
                if (strcmp(value, bpp_sided_enum_str[i].sided_str) == 0)
                {
                	pcapability->sides_supported |= bpp_sided_enum_str[i].sided_id;
                	break;
                }
            }			
            break;

        case BPP_ATTR_ORIENTATIONSSUPPORTED:
            for (i = 0; i < BPP_MAX_ATTR_ORIENT_NUM; i++)
            {
                if (strcmp(value, bpp_orientation_enum_str[i].orient_str) == 0)
                {
                	pcapability->orientations_supported |= bpp_orientation_enum_str[i].orient_id;
                	break;
                }
            }
            break;

        case BPP_ATTR_MEDIASIZESSUPPORTED:      

			if (pcapability->mediasize_num < BPP_MAX_NESTED_ATTR_NUM)
			{
	            if(strcmp("na_index_4x6in",value)==0)
	            {
            		pcapability->media_size_supported[pcapability->mediasize_num] = BPP_MEDIA_SIZE_4X6_POSTCARD;
					pcapability->mediasize_num++;
	            }
				else
				for (i = 0; i < BPP_MEDIA_SIZE_MAX_ENUM; i++)
	            {
	            	if (strcmp(value, bpp_media_size_self_desc_name[i]) == 0)
	            	{
	            		pcapability->media_size_supported[pcapability->mediasize_num] = (bt_bpp_media_size_enum)i;
						pcapability->mediasize_num++;
	            		break;
	            	}
	            }
			}

/*
			strcpy(pcapability->media_size_supported[pcapability->mediasize_num], value);
			pcapability->mediasize_num++;
*/
            break;

        case BPP_ATTR_MEDIATYPESSUPPORTED:
			if (pcapability->mediatype_num < BPP_MAX_NESTED_ATTR_NUM)
			{
	            for (i = 0; i < BPP_MEDIA_TYPE_MAX_ENUM; i++)
	            {
	            	if (strcmp(value, bpp_media_type_enum_str[i]) == 0)
	            	{
	            		pcapability->media_type_supported[pcapability->mediatype_num] = (bt_bpp_media_type_enum)i;
						pcapability->mediatype_num++;
	            		break;
	            	}
	            }
			}
            break;

        case BPP_ATTR_PRINTQUALITYSUPPORTED:
            for (i = 0; i < BPP_MAX_ATTR_QUALITY_NUM; i++)
            {
                if (strcmp(value, bpp_print_quality_enum_str[i].quality_str) == 0)
                {
                	pcapability->print_quality_supported |= bpp_print_quality_enum_str[i].quality_id;
                	break;
                }
            }
            break;

        case BPP_ATTR_OPERATIONSTATUS:
            attr_rsp->status_idx = BPP_SET_INDEXED_FLAG(value, bpp_oper_status_err_code, NULL);
			prtr_attr->operation_status = bpp_HexStrToInt((kal_char*)bpp_oper_status_err_code[attr_rsp->status_idx]);
            break;

        case BPP_ATTR_COLORSUPPORTED:
            if (strcmp("true",value) == 0 || strcmp("TRUE",value) == 0 )
            {
				pcapability->color_supported = KAL_TRUE;
            }
			break;

        case BPP_ATTR_QUEUEDJOBCOUNT:
			{
				int cnt;
            	sscanf(value, "%d", &cnt);
				pcapability->queued_job_count = (U16)cnt;
        	}
			break;

        case BPP_ATTR_MEDIALOADED:
			if (pcapability->loaded_mediatype_num < BPP_MAX_LOADED_MEDIA_NUM)
			{
	            for (i = 0; i < BPP_MEDIA_TYPE_MAX_ENUM; i++)
	            {
	            	if (strcmp(value, bpp_media_type_enum_str[i]) == 0)
	            	{
	            		pcapability->loaded_media_type[pcapability->loaded_mediatype_num] = (bt_bpp_media_type_enum)i;
						pcapability->loaded_mediatype_num++;
	            		break;
	            	}
	            }
			}
			break;

		case BPP_ATTR_BASICTEXTPAGEWIDTH:
			sscanf(value, "%lu", &pcapability->basic_text_page_width);
			break;

		case BPP_ATTR_BASICTEXTPAGEHEIGHT:
			sscanf(value, "%lu", &pcapability->basic_text_page_height);
			break;

		case BPP_ATTR_PRINTERNAME:
			{
			unsigned int len = strlen(value);

			if (len >= sizeof(pcapability->printer_name))
				len = sizeof(pcapability->printer_name) - 1;
			
			strncpy((char*)pcapability->printer_name, value, len);
			pcapability->printer_name[len] = 0;
			}
			break;
			
        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_parse_get_attr_rsp_soap
 * DESCRIPTION
 *  This function parse get arrtr rsp soap
 * PARAMETERS
 *  pobj           
 *  get_attr_cnf    
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_parse_get_attr_rsp_soap(
                    bpp_get_obj_struct *pobj,
                    bpp_get_printer_attr_resp *get_attr_rsp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_bool ret = bpp_parse_rsp_soap(pobj, bpp_get_attr_rsp_data_hdlr, (void*)get_attr_rsp);
	
    BPP_DB_SOAP_IN((kal_char*)"\r\n\r\n", 4);

	return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_create_job_rsp_soap_hdlr
 * DESCRIPTION
 *  This function parse create job response soap
 * PARAMETERS
 *  data    
 *  el          [IN]        
 *  value       [IN]        
 *  len         [IN]        
 *  error       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_create_job_rsp_soap_hdlr(void *data, const char *el, const char *value, kal_int32 len, kal_int32 error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_create_job_rst *create_job_rst = NULL;
    bpp_oper_context_struct * current_oper = NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    current_oper = bpp_find_oper_context(data,BPP_OPER_CREATE_JOB);

    if (current_oper != NULL)
    {
        create_job_rst = (bt_bpp_create_job_rst*) (current_oper->object.get_obj.parser_param->parser_output);
    }
    
    if(create_job_rst == NULL)
    {
        BPP_ASSERT(0);
        return ;
    }

    if (!strcmp("JobId", el))
    {   
        sscanf(value, "%lu", &create_job_rst->job_id);
        BT_BPP_TRACE_INFO_ARG1(BPP_CREATEJOB_SOAPRSP_JOB_ID, create_job_rst->job_id);
    }

    else if (!strcmp("OperationStatus", el))
    {
        create_job_rst->status_idx = BPP_SET_INDEXED_FLAG(value, bpp_oper_status_err_code, NULL);
        BT_BPP_TRACE_INFO_ARG2(BPP_CREATE_JOB_INFO_OPERATION_STATUS, value, create_job_rst->status_idx);
    }
    else
    {
        return;
    }

}


/*****************************************************************************
 * FUNCTION
 *  bpp_parse_create_job_rsp_soap
 * DESCRIPTION
 *  This function is to init the bpp context
 * PARAMETERS
 *  pobj       
 *  creat_job_cnf   
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_parse_create_job_rsp_soap(
                    bpp_get_obj_struct *pobj,
                    bt_bpp_create_job_rst *creat_job_rst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	kal_bool ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	ret = bpp_parse_rsp_soap(pobj, 
							bpp_create_job_rsp_soap_hdlr, 
							(void*)creat_job_rst);

	
    BPP_DB_SOAP_IN((kal_char*)"\r\n\r\n", 4);

    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_get_event_rsp_data_hdlr
 * DESCRIPTION
 *  This function parse get event response soap
 * PARAMETERS
 *  data    
 *  el          [IN]        
 *  value       [IN]        
 *  len         [IN]        
 *  error       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_get_event_rsp_data_hdlr(void *data, const char *el, const char *value, kal_int32 len, kal_int32 error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bpp_get_job_status_resp * job_sts_rsp;
    bt_bpp_job_status *prt_status = NULL;
    bpp_oper_context_struct * current_oper = NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    current_oper = bpp_find_oper_context(data,BPP_OPER_GET_EVENT);

    if (current_oper != NULL)
    {
        job_sts_rsp = (bpp_get_job_status_resp*) (current_oper->object.get_obj.parser_param->parser_output);
		prt_status = job_sts_rsp->p_jobstatus;
    }
    
    if(prt_status==NULL)
    {
        BPP_ASSERT(0);
        return ;
    }

    if (!strcmp("PrinterState", el))
    {
        prt_status->printer_state = (bt_bpp_printer_state_enum)BPP_SET_INDEXED_FLAG(value, bpp_printer_state_enum_str, NULL);
    }
    else if (!strcmp("JobState", el))
    {
        prt_status->job_state = (bt_bpp_job_state_enum)BPP_SET_INDEXED_FLAG(value, bpp_job_state_enum_str, NULL);
    }
    else if (!strcmp("PrinterStateReasons", el))
    {
        prt_status->state_reason = (bt_bpp_printer_state_reason)BPP_SET_INDEXED_FLAG(value, bpp_state_reason_enum_str, NULL);
    }
    else if (!strcmp("OperationStatus", el))
    {
        job_sts_rsp->status_idx = BPP_SET_INDEXED_FLAG(value, bpp_oper_status_err_code, NULL);
    }
    else
    {
        return;
    }

}


/*****************************************************************************
 * FUNCTION
 *  bpp_parse_get_event_rsp_soap
 * DESCRIPTION
 *  This function is to init the bpp context
 * PARAMETERS
 *  pobj          
 *  status_ind    
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool bpp_parse_get_event_rsp_soap(bpp_get_obj_struct *pobj, bpp_get_job_status_resp *status_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_bool ret = bpp_parse_rsp_soap(pobj, bpp_get_event_rsp_data_hdlr, (void*)status_ind);

    BPP_DB_SOAP_IN((kal_char*)"\r\n\r\n", 4);

	return ret;
}


#define READ_DATA_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_util_find_and_replace
 * DESCRIPTION
 *  replace the src string in the pbuf which is size long with the dest string
 * PARAMETERS
 *  pbuf       
 *  size        
 * RETURNS
 *  
 *****************************************************************************/
static kal_int32 bpp_util_find_and_replace(kal_char* pbuf, kal_uint32 size, kal_char* src, kal_char* dest)
{
    kal_uint32 i = 0;
    kal_uint32 j = 0;
    kal_uint32 start = 0;
    kal_uint32 src_size  = strlen(src);
    kal_uint32 dest_size = strlen(dest);
    bpp_get_tag_state_enum state= BPP_XHTML_START;
    kal_uint32 output_size = size;
	
    for (i = 0; i < size && j<src_size; i++)
    {
        switch (state)
        {
            case BPP_XHTML_START:
                if (pbuf[i] == src[0])
                {
                    state = BPP_XHTML_TAG_START;
                    start = i;
                    j=1;
                }
                break;

            case BPP_XHTML_TAG_START:
                if (pbuf[i] != src[j++])
                {
                    state = BPP_XHTML_START;
                }
                break;
            default:
                ASSERT(0);
                break;
        }
    }
    
    if(j == src_size)
    {
        OS_MemCopy((U8*)pbuf+(start+dest_size), (U8*)pbuf+(start+src_size), size-(start+src_size));
        OS_MemCopy((U8*)pbuf+start, (U8*)dest, dest_size);
        output_size = size+dest_size-src_size;
    }
    
    return output_size;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_replace_scr_ref
 * DESCRIPTION
 *  Replace the pchild->pfile_obj->file_name in the pbuf which is size long with the string related with pchild->chk_id.
 * PARAMETERS
 *  pbuf       
 *  size     
 * RETURNS
 *  
 *****************************************************************************/
static kal_int32 bpp_replace_scr_ref(kal_char* pbuf, kal_uint32 size, bpp_send_obj_struct * pchild)
{
    kal_uint32 output_size = size;
    kal_char dest_name[32];
    kal_char *postfix;

//20100725 mtk03036
    //if((pchild->mime_id == BPP_MIME_IMAGE_GIF || pchild->mime_id == BPP_MIME_IMAGE_JPEG) && pchild->pfile_obj)
    if( (pchild->mime_id == BPP_MIME_IMAGE_GIF ||
           pchild->mime_id == BPP_MIME_IMAGE_JPEG ||
           pchild->mime_id == BPP_MIME_IMAGE_PNG) && pchild->pfile_obj)
//20100725 mtk03036 end
    {	
		postfix = bpp_get_postfix_from_mime_type(pchild->mime_id);
	
        sprintf(dest_name,"%d",pchild->chk_id);
		strcat(dest_name, postfix);
        
        if(strlen((kal_char*)dest_name) > strlen((kal_char*)pchild->pfile_obj->file_name))
        {
            BPP_ASSERT(strlen((kal_char*)pchild->pfile_obj->file_name)<4);
            strcpy(dest_name,"1");
			strcat(dest_name, postfix);
        }
        
        output_size = bpp_util_find_and_replace(pbuf,size,(kal_char*)pchild->pfile_obj->file_name,dest_name);
        if(output_size < size)
        {
            OS_MemSet((U8*)pbuf+output_size, 0x20, size-output_size);
        }
    }
	
    return output_size;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_read_xhtml
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdata          
 *  param         
 * RETURNS
 *  
 *****************************************************************************/
static bpp_head_buff_struct *bpp_read_xhtml(bpp_send_obj_struct *pdata, void *param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu_in = pdata->pdu_in;
    bpp_head_buff_struct *pdu_out = NULL;
    kal_char *pbuf = NULL;
    kal_uint16 len = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_update_pdu(pdata);

    if (pdu_in && pdu_in->pdu_len > 0)
    {
        kal_uint16  i    = 0;/*The position of the current character*/
        kal_char   c    = '\0';/*The current character*/
        kal_uint16 pos  = 0;
        kal_uint16 read_size        = 0;
        kal_bool   pending_buf      = KAL_FALSE;
        bpp_send_obj_struct *pchild = NULL;
        bpp_get_tag_state_enum state= BPP_XHTML_START;/*The state of the parser machine*/
        bpp_token_parser_struct *parser = (bpp_token_parser_struct*) bpp_mem_alloc(sizeof(bpp_token_parser_struct));

        pbuf = bpp_get_pdu_ptr(pdu_in, (kal_uint16*) & len);

        if (pdata->p_pending_tag)
        {
            bpp_head_buff_struct* ptag = (bpp_head_buff_struct*)pdata->p_pending_tag;
            kal_uint16 tag_size = 0;
            kal_char *ptag_buf = bpp_get_pdu_ptr(ptag, (kal_uint16*) &tag_size);
            
            if(ptag->pdu_len + ptag->free_header_space < pdu_in->free_header_space)
            {
                pdu_in->free_header_space -= ptag->pdu_len;
                pbuf-= ptag->pdu_len;
                pdu_in->pdu_len +=ptag->pdu_len;
                pdata->pdu_len += ptag->pdu_len;
                len += ptag->pdu_len;
                OS_MemCopy((U8*)pbuf, (U8*)ptag_buf, tag_size);
                bpp_free_head_buf(ptag);
            }
            else
            {
                for (i = 0; i < len && i < ptag->free_tail_space; i++)
                {
                    ptag_buf[tag_size++] = pbuf[i];
                    if (pbuf[i] == '>')
                    {
                        break;
                    }
                }
                ptag->pdu_len = tag_size;
                read_size = i + 1;
                
                bpp_xhtml_parser_begin_buf(parser, ptag_buf, tag_size);
                if ((pchild = bpp_get_inline_item(parser)) != NULL)
                {
                    ASSERT(pdata->pinline_obj==NULL);
                    pchild->chk_id = pdata->total_chk_id++;
                    pdata->pinline_obj = pchild;
                    ptag->pdu_len = (U16)bpp_replace_scr_ref(ptag_buf,(U32)tag_size, pchild);
                    pdu_out = ptag;
                }
                else
                {
                    bpp_free_head_buf(ptag);
                }
            }
            pdata->p_pending_tag = NULL;
        }

        
        if(pdu_out)
        {
            bpp_head_buff_struct* pdu_temp = bpp_read_from_pdu(pdata, read_size);
            bpp_free_head_buf(pdu_temp);
        }
        else
        {
            for (i = read_size; i < len && pdata->pinline_obj == NULL; i++)
            {
                c = pbuf[i];
                switch (state)
                {
                    case BPP_XHTML_START:/*The idle state*/
                        if (c == '<')
                        {
                            state = BPP_XHTML_TAG_START;/*The start of a tag*/
                            pending_buf = KAL_TRUE;
                            pos = i;
                        }
                        break;

                    case BPP_XHTML_TAG_START:
                        if (c == '/')
                        {
                            pending_buf = KAL_FALSE;
                            state = BPP_XHTML_START;
                        }
                        else
                        {
                            state = BPP_XHTML_STAG_START;
                        }
                        break;

                    case BPP_XHTML_STAG_START:
                        switch (c)
                        {
                            case '/':
                                state = BPP_XHTML_HUNT_ETAG;
                                break;
                            case '>':
                                pending_buf = KAL_FALSE;
                                state = BPP_XHTML_START;
                            default:
                                break;
                        }
                        break;

                    case BPP_XHTML_HUNT_ETAG:

                        if (c == '>')
                        {
                            pending_buf = KAL_FALSE;
                            bpp_xhtml_parser_begin_buf(parser, &pbuf[pos], (kal_uint16) (i - pos + 1));
                            if ((pchild = bpp_get_inline_item(parser)) != NULL)
                            {
                                read_size = i + 1;
                                pchild->chk_id = pdata->total_chk_id++;
                                pdata->pinline_obj = pchild;
                                bpp_replace_scr_ref(pbuf+pos, i - pos + 1, pchild);
                            }
                            else
                            {
                                pchild = NULL;
                                state = BPP_XHTML_START;
                                if(strcmp(parser->symbol, "img") == 0 || strcmp(parser->symbol, "object") == 0 || strcmp(parser->symbol, bt_bpp_inline_item) == 0)
                                {
                                    BT_BPP_TRACE_INFO_ARG1(BPP_READ_XHTML_MEET_INVLID_IMAGE_FILE_HANDLER, pdu_out);
                                    if (parser)
                                    {
                                        bpp_mem_free(parser);
                                    }
                                    bpp_free_head_buf(pdu_in);
                                    return NULL;
                                }
                            }
                        }
                        else
                        {
                            state = BPP_XHTML_STAG_START;
                        }
                        break;
                    default:
                        ASSERT(0);
                        break;
                }
            }

            if (pending_buf)
            {
                bpp_head_buff_struct* ptag = (bpp_head_buff_struct*) bpp_alloc_head_buf(BPP_MAX_XHTML_TAG_SIZE, BPP_XHTML_TAG_PDU_FREE_HEADER, 0, TD_CTRL);
                kal_uint16 tag_size = 0;
                kal_char *ptag_buf = bpp_get_pdu_ptr(ptag, (kal_uint16*) &tag_size);
                if(tag_size > (U16)(len - pos))
                {
                    tag_size = len - pos;
                    strncpy(ptag_buf, &pbuf[pos], tag_size);
                    ptag->pdu_len = tag_size;
                    ptag->free_tail_space = BPP_MAX_XHTML_TAG_SIZE-tag_size;
                    pdata->p_pending_tag = (void*)ptag;
                    pdata->pdu_len = pos;/*clear pending tag data,it will be send in next frame*/
                    read_size = pos;
                }
                else
                {
                    BPP_ASSERT(0);
                    read_size = len;
                }
                
            }
            else if (pdata->pinline_obj == NULL)
            {
                read_size = len;
            }
            
            if(read_size)
            {
                pdu_out = bpp_read_from_pdu(pdata, read_size);
            }
        }
        
        if (parser)
        {
            bpp_mem_free(parser);
        }
    }
    if (pdu_out)
    {
        if (pdata->pkt_type & BPP_FIRST_PKT_BIT)
        {
            kal_char content_header[BPP_CONTENT_HEADER_LEN];

            sprintf(content_header, "Content-Type: %s\r\n\r\n", pdata->mime_str);
            bpp_pdu_add_header(pdu_out, content_header, strlen(content_header));
        }

        bpp_pdu_add_chk_header(pdu_out, pdata);

        if (pdata->pkt_type & BPP_FIRST_PKT_BIT)
        {
            bpp_pdu_add_header(pdu_out, (kal_char*) BPP_XHTML_CONTENT_TYPE_STR, strlen(BPP_XHTML_CONTENT_TYPE_STR));
            pdata->pkt_type &= (~BPP_FIRST_PKT_BIT);
        }
        if (pdata->pkt_type & BPP_FINAL_PKT_BIT && pdata->pdu_len == 0)
        {
            bpp_pdu_append_tail(pdu_out, (kal_char*) bpp_last_chk, (kal_uint16) strlen(bpp_last_chk));
        }
    }

    return pdu_out;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_cover_ucs2utf8
 * DESCRIPTION
 *  This function covert ucs2 data to utf encoding
 * PARAMETERS
 *  pucs2buf            [IN]        
 *  pucs2_buf_len      
 *  putf8buf        
 *  utf8_buf_len        [IN]        
 *  big_endian          [IN]        
 *  utf8_remain       
 * RETURNS
 *  void
 *****************************************************************************/
kal_uint32 bpp_cover_ucs2utf8(
            const kal_uint8 *pucs2buf,
            kal_uint32 *pucs2_buf_len,
            kal_uint8 *putf8buf,
            kal_uint32 utf8_buf_len,
            kal_bool big_endian)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint16 uchar;
    U32 tlen = 0;
    kal_uint16 i = 0;
    kal_uint16 utf8_offset = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < *pucs2_buf_len / 2 && utf8_offset < utf8_buf_len; i++)
    {
        if (big_endian)
        {
            uchar = pucs2buf[2 * i] << 8 | pucs2buf[2 * i + 1];
        }
        else
        {
            uchar = pucs2buf[2 * i + 1] << 8 | pucs2buf[2 * i];
        }

        if (uchar <= 0x7F)
        {
            kal_uint8 *pdata = NULL;
            kal_char tchar = (kal_char) uchar;

            tlen = 0;
            switch (tchar)
            {
                case '&':
                    pdata = (kal_uint8*)("&amp;");
                    break;

                case '\"':
                    pdata = (kal_uint8*)("&quot;");
                    break;

                /*case '\'':
                    pdata = (kal_uint8*)("&quot;");
                    break;*/

                case '<':
                    pdata = (kal_uint8*)("&lt;");
                    break;

                case '>':
                    pdata = (kal_uint8*)("&gt;");
                    break;
                case '\t':
                    pdata = (kal_uint8*) ("    ");
                    break;
#if 0                    
                case ' ':
                    pdata = (kal_uint8*) ("&nbsp;");
                    break;
#endif
                case '\r':
                    if (i + 1 < (U16)*pucs2_buf_len / 2)
                    {
                        kal_uint16 j = i + 1;
                        kal_uint16 uchar2 = 0;

                        if (big_endian)
                        {
                            uchar2 = pucs2buf[2 * j] << 8 | pucs2buf[2 * j + 1];
                        }
                        else
                        {
                            uchar2 = pucs2buf[2 * j + 1] << 8 | pucs2buf[2 * j];
                        }

                        if (uchar2 == 0x0A /* \n */ )
                        {
                            pdata = (kal_uint8*) "<br/>";
                            i += 1;
                            break;
                        }
                    }
                    break;
                case '\n':
                    pdata = (kal_uint8*) ("<br/>");
                    break;
                default:
                    tlen = 1;
                    pdata = NULL;
                    break;

            }

            if (tlen == 1)
            {
                putf8buf[utf8_offset] = (kal_uint8) uchar;
            }
            else
            {
                if (pdata != NULL)
                {
                    tlen = strlen((kal_char*) pdata);
                }

                if (utf8_offset + tlen <= utf8_buf_len)
                {
                    if (pdata != NULL)
                    {
                        strncpy((kal_char*) putf8buf + utf8_offset, (kal_char*) pdata, tlen);
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    break;
                }
            }

        }
        else if (uchar <= 0x7FF)
        {
            tlen = 2;
            if (utf8_offset + tlen <= utf8_buf_len)
            {
                putf8buf[utf8_offset + 1] = UTF8_FLW_BYTE_MASK | (uchar & SIX_BITS_MASK);
                putf8buf[utf8_offset] = UTF8_TWO_BYTES_FIRST_MASK | ((uchar >> UTF8_FLW_BYTE_BITS) & FIVE_BITS_MASK);
            }
            else
            {
                break;
            }
        }
        else
        {
            tlen = 3;
            if (utf8_offset + tlen <= utf8_buf_len)
            {
                putf8buf[utf8_offset + 2] = UTF8_FLW_BYTE_MASK | (uchar & SIX_BITS_MASK);
                putf8buf[utf8_offset + 1] = UTF8_FLW_BYTE_MASK | ((uchar >> UTF8_FLW_BYTE_BITS) & SIX_BITS_MASK);
                putf8buf[utf8_offset] =
                    UTF8_THREE_BYTES_FIRST_MASK | ((uchar >> (UTF8_FLW_BYTE_BITS * 2)) & FOUR_BITS_MASK);
            }
            else
            {
                break;
            }
        }
        utf8_offset += (U16)tlen;
    }

    *pucs2_buf_len = 2 * i;

    return utf8_offset;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_read_text_in_xhtml
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdata          
 *  param         
 * RETURNS
 *  
 *****************************************************************************/
static bpp_head_buff_struct *bpp_read_text_in_xhtml(bpp_send_obj_struct *pdata, void *param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu_out = NULL;
#ifdef _BPP_TEXT_UCS2_ENCODING 	
    bpp_head_buff_struct *pdu_in = pdata->pdu_in;
    kal_uint8 *pucs2_str = NULL;
    kal_uint8 *putf8_str = NULL;
    kal_uint16 usc2_str_len = 0;
    kal_uint16 utf8_str_len = 0;
    kal_uint16 utf8_buf_len = 0;
    kal_char chk_header[64];
    bpp_oper_context_struct *oper_context = BPP_GET_OPER_CONTEXT((bpp_send_obj_struct*) param);
    kal_uint16 max_pdu_len = BPP_GET_PUSH_PDU_LEN(BPP_MAX_PUSH_BUFF_LEN,oper_context->printer->printer_mru, BPP_PDU_FREE_HEADER, 0);;
#endif	

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    bpp_update_pdu(pdata);

#ifdef _BPP_TEXT_UCS2_ENCODING 	

    pdu_out = (bpp_head_buff_struct*) bpp_alloc_head_buf(max_pdu_len, BPP_PDU_FREE_HEADER, 0, TD_CTRL);

    pucs2_str = bpp_get_pdu_ptr(pdata->pdu_in, (kal_uint16*) & usc2_str_len);
    putf8_str = bpp_get_pdu_ptr(pdu_out, (kal_uint16*) & utf8_buf_len);

    utf8_str_len = (U16)bpp_cover_ucs2utf8((kal_uint8*) pucs2_str, (U32*)&usc2_str_len, putf8_str, (U32)utf8_buf_len, KAL_FALSE);

    pdu_in = bpp_read_from_pdu(pdata, (kal_uint16) usc2_str_len);

    bpp_free_head_buf(pdu_in);

    pdu_out->pdu_len = utf8_str_len;
    pdu_out->free_tail_space = utf8_buf_len - utf8_str_len;


    sprintf(chk_header, "\r\nCHK 1 %d MORE\r\n",utf8_str_len);

    if (bpp_pdu_add_header(pdu_out, chk_header, strlen(chk_header)) == KAL_TRUE)
    {
        return pdu_out;
    }
    else
    {
        bpp_free_head_buf(pdu_out);
        return NULL;
    }

#else /* _BPP_TEXT_UCS2_ENCODING */

	if (pdata->pdu_len > 0)
	{
		pdu_out = bpp_read_from_pdu(pdata, pdata->pdu_len);

		{
		    kal_char chk_header[64];

			sprintf(chk_header, "\r\nCHK 1 %d MORE\r\n",pdu_out->pdu_len);

		    if (bpp_pdu_add_header(pdu_out, chk_header, strlen(chk_header)) == KAL_TRUE)
		    {
		        return pdu_out;
		    }
		}
	}

	return pdu_out;

#endif	
}


/*****************************************************************************
 * FUNCTION
 *  bpp_read_image_in_xhtml
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdata      
 *  param      
 * RETURNS
 *  
 *****************************************************************************/
static bpp_head_buff_struct *bpp_read_image_in_xhtml(bpp_send_obj_struct *pdata, void *param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu_out = NULL;
	kal_char *postfix;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_update_pdu(pdata);

	/* get postfix from mime type */
	postfix = bpp_get_postfix_from_mime_type(pdata->mime_id);

    if (pdata->pdu_len)
    {
        pdu_out = bpp_read_from_pdu(pdata, (kal_uint16) pdata->pdu_len);

        if (pdata->pkt_type & BPP_FIRST_PKT_BIT)
        {
            kal_char content_header[BPP_CONTENT_HEADER_LEN];
			kal_bool ret = KAL_FALSE;

            bpp_pdu_add_header(pdu_out, "\r\n\r\n", 4);
            if (pdata->pfile_obj)
            {
                kal_char ref_name[32];
                sprintf(ref_name,"%d",pdata->chk_id);
				strcat(ref_name, postfix);

                ret = bpp_pdu_add_header(
                        pdu_out,
                        (kal_char*) ref_name,
                        strlen((kal_char*)ref_name));
            }
            else
            {
                if (pdata->pbuf_obj)
                {
                    sprintf(content_header, "%s%lu", bt_bpp_mem_drv, (kal_uint32) pdata->pbuf_obj->pbuf);
					strcat(content_header, postfix);

					ret = bpp_pdu_add_header(pdu_out, (kal_char*) content_header, strlen((kal_char*) content_header));
                }
            }

			if (ret)
			{
	            bpp_pdu_add_header(pdu_out, "Content-Location: ", strlen("Content-Location: "));

	            sprintf(content_header, "Content-Type: %s\r\n", pdata->mime_str);
	            bpp_pdu_add_header(pdu_out, content_header, strlen(content_header));
			}
            pdata->pkt_type &= (~BPP_FIRST_PKT_BIT);
        }
        bpp_pdu_add_chk_header(pdu_out, pdata);
    }
    return pdu_out;

}


/*****************************************************************************
 * FUNCTION
 *  bpp_read_bin
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdata       
 *  param       
 * RETURNS
 *  
 *****************************************************************************/
static bpp_head_buff_struct *bpp_read_bin(bpp_send_obj_struct *pdata, void *param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu_out = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_update_pdu(pdata);

    pdu_out = bpp_read_from_pdu(pdata, pdata->pdu_len);

    return pdu_out;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_load_data_from_file
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psend_obj     
 * RETURNS
 *  
 *****************************************************************************/
static BPP_ERROR_CODE bpp_load_data_from_file(bpp_send_obj_struct *psend_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BPP_ERROR_CODE ret = BPP_SUCCESS;
    bpp_send_obj_struct *pdata = psend_obj;
    bpp_oper_context_struct *oper_context = BPP_GET_OPER_CONTEXT(psend_obj);
    kal_uint32 max_buffer = BPP_MAX_PUSH_BUFF_LEN;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (psend_obj->pinline_obj)
    {
        if (pdata->pdu_len == 0 && pdata->pkt_type & BPP_FINAL_PKT_BIT)
        {
            /* This part seems useless or incorrect, leave this part 
                make code looks little ugly, anyway, we just 
                keep this for safe reason. */
            bpp_deinit_send_obj(pdata);
            bpp_mem_free(pdata);
            psend_obj->pinline_obj = NULL;
        }
        else if (pdata->pinline_obj->pdu_len == 0 &&
                pdata->pinline_obj->pkt_type & BPP_FINAL_PKT_BIT)
        {
            bpp_deinit_send_obj(pdata->pinline_obj);
            bpp_mem_free(pdata->pinline_obj);
            pdata->pinline_obj = NULL;
        }
        else
        {
            pdata = psend_obj->pinline_obj;
        }
    }
    else
    {
        if(max_buffer>1024)
        {
            max_buffer = 1024;
        }
    }

    if (pdata && pdata->pdu_len == 0 && (pdata->pkt_type & BPP_FINAL_PKT_BIT) == 0)
    {
        bpp_file_obj_struct *pfile_obj = pdata->pfile_obj;
        bpp_buf_obj_struct *pbuf_obj = pdata->pbuf_obj;

        if ((pfile_obj && pfile_obj->fh > 0) || (pbuf_obj && pbuf_obj->buf_size != 0))
        {
            kal_char *pbuf = NULL;
            kal_uint16 pdu_len = 0;
            int read_size = 0;
            bpp_head_buff_struct *pdu_in = pdata->pdu_in;

            if (pdu_in == NULL)
            {
                kal_uint16 max_pdu_len = BPP_GET_PUSH_PDU_LEN(max_buffer,oper_context->printer->printer_mru, BPP_PDU_FREE_HEADER, BPP_PDU_FREE_TAIL);

                pdu_in = pdata->pdu_in =
                    (bpp_head_buff_struct*) bpp_alloc_head_buf(
                                            max_pdu_len,
                                            BPP_PDU_FREE_HEADER,
                                            BPP_PDU_FREE_TAIL,
                                            TD_RESET);
            }
            else
            {
                pdu_in->pdu_len = pdu_in->pdu_len + pdu_in->free_header_space + pdu_in->free_tail_space - BPP_PDU_FREE_HEADER -
                BPP_PDU_FREE_TAIL;
                pdu_in->free_header_space = BPP_PDU_FREE_HEADER;
                pdu_in->free_tail_space = BPP_PDU_FREE_TAIL;
            }

            pbuf = bpp_get_pdu_ptr(pdu_in, (kal_uint16*) & pdu_len);

            if (pfile_obj)
            {
                kal_int32 fs_ret  = 0;
                read_size = (U16)(pfile_obj->file_rsize > pdu_len ? pdu_len : pfile_obj->file_rsize);
#ifdef __DRM_SUPPORT__   
                if ((fs_ret = DRM_read_file(pfile_obj->fh, pbuf, read_size, (U32*)&read_size)) >= 0)
#else 
                if ((fs_ret = bpp_fs_read(pfile_obj->fh, pbuf, read_size, &read_size)) >= 0)
#endif
                {             
                    pfile_obj->file_rsize -= read_size;
                    if (pfile_obj->file_rsize == 0)
                    {
                        pdata->pkt_type |= BPP_FINAL_PKT_BIT;
                    }
                    ret = BPP_SUCCESS;
                }
                else
                {
					BT_BPP_TRACE_INFO_ARG2(BPP_LOADDATA_FROMFILE_FAILED,fs_ret,pfile_obj->file_rsize);
                    pdata->pkt_type |= BPP_FINAL_PKT_BIT;
                    ret = BPP_ERROR;
                }

            }
            else if (pbuf_obj)
            {
                U16 size = (U16)(pbuf_obj->buf_size);
                const kal_char* pdata_buf = pbuf_obj->pbuf +pbuf_obj->offset;
                
                read_size = size > pdu_len ? pdu_len : size;
                OS_MemCopy((U8*)pbuf, (U8*)pdata_buf, read_size);
                pbuf_obj->offset += read_size;
                pbuf_obj->buf_size -= read_size;

                if (pbuf_obj->buf_size == 0)
                {
                    pdata->pkt_type |= BPP_FINAL_PKT_BIT;
                }
                ret = BPP_SUCCESS;
            }

            if (ret == BPP_SUCCESS)
            {
                pdu_in->free_tail_space += pdu_in->pdu_len - read_size;
                pdata->pdu_len = read_size;
                pdu_in->pdu_len = 0;
                pdu_in->free_tail_space += read_size;
            }
            else
            {
                pdu_in->free_header_space += pdu_in->pdu_len;
                pdu_in->pdu_len = 0;
                ret = BPP_ERROR;
            }
        }
        else
        {
            if (pdata->pdu_in)
            {
                bpp_free_head_buf(pdata->pdu_in);
                pdata->pdu_in = NULL;
            }
            ret = BPP_PENDING;
        }
    }
    else if (pdata && pdata->pdu_len == 0 && pdata->pkt_type & BPP_FINAL_PKT_BIT)
    {
        bpp_deinit_send_obj(pdata);
        if (psend_obj->pinline_obj == pdata)
        {
            bpp_mem_free(pdata);
            psend_obj->pinline_obj = NULL;
        }
    }

    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_pdu_add_chk_header
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdu_out        
 *  pdata         
 * RETURNS
 *  
 *****************************************************************************/
static kal_bool bpp_pdu_add_chk_header(bpp_head_buff_struct *pdu_out, bpp_send_obj_struct *pdata)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_char chk_header[64];
    kal_uint32 chk_header_len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdata->pkt_type & BPP_FINAL_PKT_BIT && pdata->pdu_len == 0)
    {
        chk_header_len = sprintf(chk_header, "\r\nCHK %d %d LAST\r\n", pdata->chk_id, pdu_out->pdu_len);
    }
    else
    {
        chk_header_len = sprintf(chk_header, "\r\nCHK %d %d MORE\r\n", pdata->chk_id, pdu_out->pdu_len);
    }

    return bpp_pdu_add_header(pdu_out, chk_header, chk_header_len);
}


/*****************************************************************************
 * FUNCTION
 *  bpp_compose_multiplexed_data
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psend_obj        
 * RETURNS
 *  
 *****************************************************************************/
static bpp_head_buff_struct *bpp_compose_multiplexed_data(bpp_send_obj_struct *psend_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_send_obj_struct *pdata = NULL;
    bpp_head_buff_struct *pdu_out = NULL;
    kal_int32 pkt_len = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (psend_obj && psend_obj->pinline_obj)
    {
        pdata = psend_obj->pinline_obj;
    }
    else
    {
        pdata = psend_obj;
    }

    if (pdata && pdata->pdu_len != 0)
    {
        pkt_len = pdata->pdu_len; /* current data len in pdu_in */
    
		if (pdata->mime_id == BPP_MIME_APPLICATION_MULTIPLEXED)
		{
			pdu_out = bpp_read_bin(pdata, psend_obj);
		}
		else if (pdata->mime_id == BPP_MIME_APPLICATION_XHTML_PRINT_095 ||
 				 pdata->mime_id == BPP_MIME_APPLICATION_XHTML_PRINT_100)
		{
			pdu_out = bpp_read_xhtml(pdata, psend_obj);
		}
		else if (pdata->mime_id >= BPP_MIME_TEXT_PLAIN &&
				 pdata->mime_id <= BPP_MIME_TEXT_VNOTE)
		{
			pdu_out = bpp_read_text_in_xhtml(pdata, psend_obj);
		}
                //20100725 mtk03036
		else if (pdata->mime_id == BPP_MIME_IMAGE_JPEG ||
				 pdata->mime_id == BPP_MIME_IMAGE_GIF || 
				 pdata->mime_id == BPP_MIME_IMAGE_PNG ) 
                //20100725 mtk03036 end
		{
			pdu_out = bpp_read_image_in_xhtml(pdata, psend_obj);
		}
		else
		{
		}

        if(pdu_out == NULL)
        {
            psend_obj->total_size_sent = -1;
        }
        else
        {
            psend_obj->total_size_sent += (pkt_len - pdata->pdu_len);
        }                
			
    }

	BT_BPP_TRACE_INFO_ARG2(BPP_RESULT_OF_READ_DATA, pdu_out, psend_obj->total_size_sent);

	return pdu_out;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_send_data_continue
 * DESCRIPTION
 *  
 * PARAMETERS
 *  printer 
 * RETURNS
 *  
 *****************************************************************************/
kal_bool bpp_continue_read_xhtml_data(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_send_obj_struct *psend_obj = (bpp_send_obj_struct*) & printer->current_oper->object.send_obj;
    kal_uint32 size = 0;
    kal_char *pbuf = NULL;
    BPP_ERROR_CODE err = BPP_SUCCESS;
    kal_bool ret = KAL_FALSE;
		
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (printer->current_oper->pdu_out)
    {
        bpp_free_head_buf(printer->current_oper->pdu_out);
        printer->current_oper->pdu_out = NULL;
    }

    while((err = bpp_load_data_from_file(psend_obj)) >= BPP_SUCCESS)
    {
        if (err == BPP_PENDING)
        {			
			bpp_progress_ind(printer);			
        }
        else
        {
			/* we not support buff base to get inline data */
            printer->current_oper->pdu_out = bpp_compose_multiplexed_data(&printer->current_oper->object.send_obj);    
			
            if (printer->current_oper->pdu_out)
            {
                pbuf = bpp_get_pdu_ptr(printer->current_oper->pdu_out, (kal_uint16*) & size);

                if(size == 0)
                {
                    continue;
                }
            }
            else if(printer->current_oper->object.send_obj.p_pending_tag)
            {
                continue;
            }
            else if(psend_obj->total_size_sent < 0)
            {
                return KAL_FALSE;
            }

			/* init send buffer */
			Goep_init_send_buf(&printer->job_channel, pbuf, size);
			
#ifdef __BPP_DEBUG__
            if (BPP_DEGUB_WRITE_SENDDOC_2FILE == 1)
            {
                BPP_DB_OUT(pbuf, size);
            }
#endif     		
			/* continue send */
			GOEP_ClientContinue(&printer->job_channel);

        }
        ret = KAL_TRUE;
		break;
    }


    return ret;
}

kal_bool bpp_continue_read_simple_data(bpp_send_obj_struct *psend_obj)
{
	bpp_file_obj_struct *pfile_obj;
	kal_char *pbuf = NULL;
	//kal_uint16 read_size = 0;	
	int read_size = 0;	
	kal_bool ret = KAL_TRUE;
	kal_int32 fs_ret  = 0;


	if (psend_obj && 
		(psend_obj->pkt_type & BPP_FINAL_PKT_BIT) == 0 &&
		psend_obj->simple_data.buf != NULL)
	{
		pbuf = (kal_char*)psend_obj->simple_data.buf;

		pfile_obj = psend_obj->pfile_obj;

		if ((pfile_obj && pfile_obj->fh > 0))
		{
			//read_size = (U16)min(pfile_obj->file_rsize, psend_obj->simple_data.buf_size);
			read_size = (U32)min(pfile_obj->file_rsize, psend_obj->simple_data.buf_size);
#ifdef __DRM_SUPPORT__   
			if ((fs_ret = DRM_read_file(pfile_obj->fh, pbuf, read_size, (U32*)&read_size)) >= 0)
#else 
			if ((fs_ret = bpp_fs_read(pfile_obj->fh, pbuf, read_size, &read_size)) >= 0)
#endif
			{			  
				psend_obj->simple_data.data_len = read_size;
				
				pfile_obj->file_rsize -= read_size;

                                psend_obj->total_size_sent += read_size;
				
				if (pfile_obj->file_rsize == 0)
				{
					psend_obj->pkt_type |= BPP_FINAL_PKT_BIT;
					bpp_fs_close(pfile_obj->fh);
					pfile_obj->fh = -1;
				}
				ret = KAL_TRUE;
			}
			else
			{
				BT_BPP_TRACE_INFO_ARG2(BPP_LOADDATA_FROMFILE_FAILED,fs_ret,pfile_obj->file_rsize);
				psend_obj->pkt_type |= BPP_FINAL_PKT_BIT;
				bpp_fs_close(pfile_obj->fh);
				pfile_obj->fh = -1;
				ret = KAL_FALSE;
			}
		}
	}
    else if (psend_obj && psend_obj->pkt_type & BPP_FINAL_PKT_BIT)
    {
    	/* finished */
    	psend_obj->simple_data.data_len = 0;
    	ret = KAL_TRUE;
    }	

	return ret;
}


#define CONTINUE____________SENDING

kal_bool bpp_continue_to_read_data(bpp_printer_struct *printer)
{
    bpp_send_obj_struct *psend_obj;
	U8 *pbuf;
	U32 data_len;
	kal_bool ret = KAL_FALSE;

	psend_obj = & printer->current_oper->object.send_obj;

	if (psend_obj->data_type == BPP_SIMPLE_DATA)
	{
		/* simple data */

		/* alloc memory if need */
		if (psend_obj->simple_data.buf == NULL)
		{
			psend_obj->simple_data.buf_size = 
					(U16)(printer->printer_mru > BPP_MAX_PUSH_BUFF_LEN ? BPP_MAX_PUSH_BUFF_LEN : printer->printer_mru);
			
			psend_obj->simple_data.buf = bpp_mem_alloc(psend_obj->simple_data.buf_size);
		}

		ret = bpp_continue_read_simple_data(psend_obj);

		if (ret == KAL_TRUE)
		{
			pbuf = psend_obj->simple_data.buf;
			data_len = psend_obj->simple_data.data_len;

			Goep_init_send_buf(&printer->job_channel, 
								(kal_char*)pbuf, 
								data_len);

			GOEP_ClientContinue(&printer->job_channel);			
		}
	}
	else
	{
		/* xhtml data */
		/* maybe xhtml has inline data */
		ret = bpp_continue_read_xhtml_data(printer);

	}
	

	return ret;
}


#define XHTML_UTIL

/*****************************************************************************
 * FUNCTION
 *  bpp_get_soap_content_len
 * DESCRIPTION
 *  This function get xml soap len
 * PARAMETERS
 *      
 * RETURNS
 *  KAL_BOOL
 *****************************************************************************/
static kal_bool bpp_get_soap_content_len(kal_char* pbuf, kal_uint16 size, kal_uint16* pcontent_head_len, kal_uint16* pcontent_len)
{
    kal_int32 i =0;
    kal_char c;
    bpp_soap_parse_state state = SOAP_INIT;

    for(; i<size; i++)
    {
        c = pbuf[i];
        switch(state)
        {
            case SOAP_INIT:
                if(c=='\n')
                {
                    kal_char content_len[128];
					int len;
					
                    ASSERT(i<128);
                    strncpy(content_len,pbuf,i+1);
                    sscanf(content_len,"CONTENT-LENGTH: %d", &len);
					*pcontent_len = (kal_uint16)len;
                    state = SOAP_HEADER_START;
                }
                break;
            case SOAP_HEADER_START:
                if(c=='\n')
                {
                    state = SOAP_BODY_START;
                }
                break;
           case SOAP_BODY_START:
                if(c=='\n')
                {
                    *pcontent_head_len = i+1;
                    return KAL_TRUE;
                }
                else if(c!='\r')
                {
                    state = SOAP_HEADER_START;
                }
                break;
            default:
                break;
        }
    }
    
	/* workarund SOAP rsp error in PTS 3.3.0 test case TC_SEN_DPS_BC_01_I:
	 * there is only 1 "\r\n" after the header.
	 */
	if (strstr((char*)pbuf, "</s:Envelope>") != NULL)
	{
		return KAL_TRUE;
	}
	
    return KAL_FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_is_soap_complete
 * DESCRIPTION
 *  This function determine is soap recv complete
 * PARAMETERS
 *  bpp_oper_context_struct        [IN]       soap_rsp_oper_context 
 * RETURNS
 *  KAL_BOOL
 *****************************************************************************/
kal_bool bpp_is_soap_complete(bpp_oper_context_struct *oper_context)
{
    kal_bool ret = KAL_FALSE;
    
    if(oper_context && oper_context->object.get_obj.pdu_in && oper_context->object.get_obj.pdu_in->pdu_len > 0)
    {
        kal_uint32 soap_rsp_len = oper_context->data_len;
        kal_uint16 buf_len = 0;
        kal_uint16 content_header_len = 0;
        kal_uint16 body_len = 0;
        kal_char *pbuf = bpp_get_pdu_ptr(oper_context->object.get_obj.pdu_in, (kal_uint16*) & buf_len);
        if(soap_rsp_len == 0)
        {
            if(bpp_get_soap_content_len(pbuf,buf_len,&content_header_len, &body_len)==KAL_TRUE)
            {
                oper_context->data_len = soap_rsp_len= content_header_len+ body_len;
            }
			BT_BPP_TRACE_INFO_ARG3(BPP_IS_SOAP_COMPLETE_INFO1,content_header_len,body_len, buf_len);
        }
		BT_BPP_TRACE_INFO_ARG2(BPP_IS_SOAP_COMPLETE_INFO2,soap_rsp_len,buf_len);
		
        if(soap_rsp_len !=0 && buf_len >= soap_rsp_len)
        {
            ret = KAL_TRUE;
        }
    }
    
    return ret;    
}

/*****************************************************************************
 * FUNCTION
 *  bpp_xhtml_parser_begin_buf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parse         
 *  pbuf       
 *  size        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_xhtml_parser_begin_buf(bpp_token_parser_struct *parser, kal_char *pbuf, kal_uint16 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser)
    {
        parser->pbuf = pbuf;
        parser->size = size;
        parser->pos = 0;
        parser->look_forward = 0;
        parser->psymbol = (kal_char*) & parser->symbol[0];
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_xhtml_get_token_val
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parser     
 * RETURNS
 *  
 *****************************************************************************/
static kal_char *bpp_xhtml_get_token_val(bpp_token_parser_struct *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_char *pval = NULL;
    kal_uint16 val_len = parser->look_forward - parser->pos;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser && parser->psymbol + val_len < &parser->symbol[BPP_TOKEN_BUF])
    {
        pval = parser->psymbol;
        strncpy(pval, &parser->pbuf[parser->pos], parser->look_forward - parser->pos);
        parser->psymbol += parser->look_forward - parser->pos;
        *parser->psymbol = '\0';
        parser->psymbol++;
    }
    return pval;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_xhtml_get_el_token
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parser     
 * RETURNS
 *  
 *****************************************************************************/
static bpp_token_enum bpp_xhtml_get_el_token(bpp_token_parser_struct *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;
    kal_char c;
    bpp_get_token_state_enum state = BPP_XHTML_INIT;
    kal_char *pbuf = NULL;

    /* bpp_token_enum t = BPP_TOKEN_UNKNOW; */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser == NULL || parser->pbuf == NULL || parser->size == 0)
    {
        return BPP_TOKEN_UNKNOW;
    }

    pbuf = parser->pbuf;

    parser->pos = parser->look_forward;

    for (i = parser->pos; i < parser->size; i++)
    {
        c = pbuf[i];
        parser->look_forward++;
        switch (state)
        {
            case BPP_XHTML_INIT:
                switch (c)
                {
                    case '<':
                        if (pbuf[i + 1] == '/')
                        {
                            parser->look_forward++;
                            return BPP_TOKEN_ETAG_START;
                        }
                        else
                        {
                            return BPP_TOKEN_STAG_START;
                        }

                    case '\t':
                    case ' ':
                    case '\r':
                    case '\n':
                        break;

                    case '"':
                        state = BPP_XHTML_BEGIN_VAL;
                        parser->pos = i;
                        break;

                    case '>':
                        return BPP_TOKEN_TAG_END;

                    case '/':
                        if (pbuf[i + 1] == '>')
                        {
                            parser->look_forward++;
                            return BPP_TOKEN_ESTAG_END;
                        }
                        else
                        {
                            return BPP_TOKEN_UNKNOW;
                        }

                    case '=':
                        return BPP_TOKEN_EQUAL;

                    default:
                        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ':' || c == '_')
                        {
                            state = BPP_XHTML_BEGIN_ID;
                            parser->pos = i;
                        }
                        else
                        {
                            return BPP_TOKEN_UNKNOW;
                        }
                        break;
                }
                break;
				
            case BPP_XHTML_BEGIN_VAL:
                if (c == '"')
                {
                    return BPP_TOKEN_VAL;
                }
                break;

            case BPP_XHTML_BEGIN_ID:
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-'
                    || c == ':' || c == '_')
                {
                    continue;
                }
                parser->look_forward--;

                return BPP_TOKEN_ID;

            default:
                return BPP_TOKEN_UNKNOW;
        }
    }
    return BPP_TOKEN_UNKNOW;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_parser_item
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parser       
 *  item_name    
 * RETURNS
 *  
 *****************************************************************************/
static bpp_xhtml_tag_struct *bpp_parser_item(bpp_token_parser_struct *parser, const kal_char *item_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret = KAL_TRUE;
    kal_char *pID = NULL;
    kal_char *pval = NULL;
    bpp_xhtml_tag_struct *pxhtml_tag = NULL;
    bpp_xhtml_tag_enum tag_type = BPP_XHTML_NONE;
    bpp_token_enum token = bpp_xhtml_get_el_token(parser);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (token == BPP_TOKEN_STAG_START)
    {
        tag_type = BPP_XHTML_SETAG;
    }
    else if (token == BPP_TOKEN_ETAG_START)
    {
        tag_type = BPP_XHTML_ETAG;
    }
    else
    {
        ASSERT(0);
        return NULL;
    }
    token = bpp_xhtml_get_el_token(parser);
    pID = bpp_xhtml_get_token_val(parser);

    if (token != BPP_TOKEN_ID /* ||(item_name && (pID == NULL || strcmp(pID,item_name)!=0)) */ )
    {
        return NULL;
    }
    else
    {
        pxhtml_tag = (bpp_xhtml_tag_struct*) bpp_mem_alloc(sizeof(bpp_xhtml_tag_struct));
        pxhtml_tag->tag_type = tag_type;
        pxhtml_tag->attr_num = 0;
    }

    while ((token = bpp_xhtml_get_el_token(parser)) == BPP_TOKEN_ID)
    {
        pID = bpp_xhtml_get_token_val(parser);
        token = bpp_xhtml_get_el_token(parser);
        if (token == BPP_TOKEN_EQUAL)
        {
            token = bpp_xhtml_get_el_token(parser);
            if (token == BPP_TOKEN_VAL)
            {
                pval = bpp_xhtml_get_token_val(parser);
                pxhtml_tag->attr[pxhtml_tag->attr_num].pid = pID;
                pxhtml_tag->attr[pxhtml_tag->attr_num].pval = pval;
                pxhtml_tag->attr_num++;
                ASSERT(pxhtml_tag->attr_num < BPP_XHTML_TAG_MAX_ATTR);
                continue;
            }
        }
        ret = KAL_FALSE;
        break;
    }

    if (ret == KAL_TRUE && BPP_TOKEN_ESTAG_END == token)
    {
        pxhtml_tag->tag_type = BPP_XHTML_SETAG;
    }
    else
    {
        bpp_mem_free(pxhtml_tag);
        pxhtml_tag = NULL;
    }

    return pxhtml_tag;
}

kal_char * bpp_get_mime_id(kal_char * mime_str, bt_bpp_mime_type *type_id)
{
    int i;
	kal_char *p_mime_str = NULL;

	for (i = 0; i < BPP_MIME_TYPE_MAX_ENUM; i++)
	{
	    if (strcmp(mime_str, (char*)bpp_mime_type_table[i].type_str) == 0)
	    {
	    	*type_id = bpp_mime_type_table[i].type_id;
			p_mime_str = (kal_char*)bpp_mime_type_table[i].type_str;
			break;
	    }
	}

	return p_mime_str;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_get_inline_item
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parser    
 * RETURNS
 *  
 *****************************************************************************/
static bpp_send_obj_struct *bpp_get_inline_item(bpp_token_parser_struct *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_char *pitem_name = NULL;
    kal_char *pmime_type = NULL;
    kal_int32 item_size = 0;
    bpp_send_obj_struct *pchild = NULL;
    bpp_xhtml_tag_struct *pxhtml_tag = bpp_parser_item(parser, NULL);
	bt_bpp_mime_type typeid;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pxhtml_tag && (pxhtml_tag->tag_type == BPP_XHTML_ETAG || pxhtml_tag->tag_type == BPP_XHTML_SETAG))
    {
        bpp_xhtml_attr_struct *pattr = NULL;
        kal_uint16 id_len = 0;
        int i = 0;

        for (; i < pxhtml_tag->attr_num; i++)
        {
            pattr = &pxhtml_tag->attr[i];
            id_len = strlen(pattr->pval);
            pattr->pval++;
            pattr->pval[id_len - 2] = '\0'; /* remove '"' */
            if (strcmp(pattr->pid, bt_bpp_attr_ref_src) == 0 || strcmp(pattr->pid, bt_bpp_attr_ref_data) == 0 )
            {
                pitem_name = pattr->pval;
            }
            else if (strcmp(pattr->pid, bt_bpp_attr_type) == 0)
            {
                pmime_type = pattr->pval;
            }
            else if (strcmp(pattr->pid, bt_bpp_attr_size) == 0)
            {
                item_size = atoi(pattr->pval);
            }
        }
        bpp_mem_free(pxhtml_tag);
    }

	 /* check pmime_type */
	if (pmime_type == NULL)
	{
		/* get mime type from filename */
		pmime_type = bpp_get_mime_str_from_filename(pitem_name, &typeid);
	}
	else
	{
	    pmime_type = bpp_get_mime_id(pmime_type, &typeid);
	}
	 	
    if (pitem_name && pmime_type)
    {
        bpp_file_obj_struct *pfile_obj = NULL;
        bpp_buf_obj_struct  *pbuf_obj = NULL;
        
        kal_char *pbuf = NULL;
        kal_uint32 len = (strlen(pitem_name) + 1) * 2;

        pchild = bpp_mem_alloc(sizeof(bpp_send_obj_struct));
        bpp_init_send_obj(pchild);

        if (strncmp(pitem_name, bt_bpp_mem_drv, strlen(bt_bpp_mem_drv)) == 0 && item_size)
        {
            sscanf(pitem_name + strlen(bt_bpp_mem_drv), "%s", pbuf);
            pbuf_obj = bpp_mem_alloc(sizeof(bpp_buf_obj_struct));
            pbuf_obj->pbuf = pbuf;
            pbuf_obj->buf_size = item_size;
            pbuf_obj->offset = 0;            
        }
        else
        {
            kal_uint8 *buf = bpp_mem_alloc(len);

            pfile_obj = bpp_mem_alloc(sizeof(bpp_file_obj_struct));

            bpp_utf8_to_ucs2_string(buf, len, (kal_uint8*) pitem_name);
            pfile_obj->fh = BPP_Debug_x_FS_Open((WCHAR*) buf, BTMTK_FS_READ_ONLY);
            bpp_mem_free(buf);
            BT_BPP_TRACE_INFO_ARG1(BPP_FS_OPEN_HANDLE_INLINE_ITEM, pfile_obj->fh);  
        }

        if (pfile_obj && pfile_obj->fh > 0)
        {
            strcpy((kal_char*) pfile_obj->file_name, (kal_char*) pitem_name);
            if (item_size != 0)
            {
                pfile_obj->file_rsize = item_size;
            }
            else
            {
#ifdef __DRM_SUPPORT__   
                DRM_file_size(pfile_obj->fh, &pfile_obj->file_rsize);
#else
                bpp_fs_getfilesize(pfile_obj->fh, (unsigned int*)&pfile_obj->file_rsize);
#endif
            }

            pfile_obj->btemp_file = KAL_FALSE;

            pchild->pfile_obj = pfile_obj;
            pchild->mime_str = pmime_type;
			pchild->mime_id = typeid;
        }
        else if (pbuf_obj)
        {
            pchild->pbuf_obj = pbuf_obj;
            pchild->mime_str = pmime_type;
			pchild->mime_id = typeid;
        }
        else
        {
            bpp_mem_free(pfile_obj);
            bpp_mem_free(pbuf_obj);
            bpp_mem_free(pchild);
            pchild = NULL;
        }

    }

    return pchild;
}

struct _mime_postfix_struct  
{
   kal_char * postfix;
   bt_bpp_mime_type type;
} bpp_mime_postfix_table[BPP_MIME_TYPE_MAX_ENUM] =
{
	{".xhtml", BPP_MIME_APPLICATION_XHTML_PRINT_100},
	{".txt", BPP_MIME_TEXT_PLAIN},
	{".jpg", BPP_MIME_IMAGE_JPEG},
	{".gif", BPP_MIME_IMAGE_GIF},
	{".png", BPP_MIME_IMAGE_PNG}
};

static kal_char * bpp_get_mime_str_from_filename(kal_char * filename, bt_bpp_mime_type* p_typeid)
{
	kal_char *postfix;
	int i, j;
	bt_bpp_mime_type type_id;
	
    postfix = strrchr(filename, '.');

	if (postfix == NULL)
		return NULL;

	for (i = 0; i < BPP_MIME_TYPE_MAX_ENUM; i++)
	{
		if (strcmp(postfix, bpp_mime_postfix_table[i].postfix) == 0)
		{
		    type_id = bpp_mime_postfix_table[i].type;
			*p_typeid = type_id;
			break;
		}
	}

	if (i < BPP_MIME_TYPE_MAX_ENUM)
	{
	    for (j = 1; j < BPP_MIME_TYPE_MAX_ENUM; j++)
	    {
	    	if (type_id == bpp_mime_type_table[j].type_id)
	    	{
				return (kal_char*)bpp_mime_type_table[j].type_str;
	    	}
	    }
	}

	return NULL;
}

static kal_char *bpp_get_postfix_from_mime_type(bt_bpp_mime_type type)
{
    int i;
	
	ASSERT (type < BPP_MIME_TYPE_MAX_ENUM);

	for (i = 0; i < BPP_MIME_TYPE_MAX_ENUM; i++)
	{
		if (type == bpp_mime_postfix_table[i].type)
		{
			return (kal_char*)bpp_mime_postfix_table[i].postfix;
		}
	}
	
    return NULL;
}

kal_char *bpp_get_doc_format(bt_bpp_mime_type type)
{
	kal_char * doc_format = NULL;

	if (type == BPP_MIME_APPLICATION_XHTML_PRINT_095 || 
		type == BPP_MIME_APPLICATION_XHTML_PRINT_100 || 
		type == BPP_MIME_APPLICATION_MULTIPLEXED )
	{
		doc_format = (kal_char*)bpp_xhtml_multiplexed_type;
	}
	else if (type < BPP_MIME_TYPE_MAX_ENUM)
	{
		doc_format = (kal_char*)bpp_mime_type_table[type].type_str;
	}

	return doc_format;
}

U16 bpp_get_doc_name(WCHAR *pathname, kal_char *name_buf, U16 buf_size)
{
	WCHAR *objname;
    U16 namelen = 0;
	U16 len = 0;
    U16 deter1 = '\\';
    U16 deter2 = '/';

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	/* get object name */
	objname = pathname + btmtk_wstrlen(pathname);

    while (objname > pathname)
    {
        namelen++;
		objname--;
		
        if (OS_MemCmp((U8*)objname, 2, (U8*)&deter1, 2)||
			OS_MemCmp((U8*)objname, 2, (U8*)&deter2, 2))
        {
            break;
        }
    }
	if (objname > pathname)
	{
		objname++; // skip deter
	}
	
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
	/* convert unicode to UTF8 */

#else 
	/* memory copy */
	namelen = namelen * 2;
	len = namelen > buf_size-2 ? buf_size-2 : namelen;
	OS_MemCopy((U8*)name_buf, (U8 *)objname, len);
	name_buf[len-2] = 0;
	name_buf[len-1] = 0;
	
#endif 

	return len;
}


S32 bpp_compose_object_into_xhtml(U16 * dst_xhtml_file, U16 *obj_name, bt_bpp_mime_type obj_type)
{
	int data_len;
    BTMTK_FS_HANDLE fh;	
//mtk03036 20100512
    //fh = bpp_fs_open(dst_xhtml_file, BTMTK_FS_READ_WRITE|BTMTK_FS_CREATE_ALWAYS);
    fh = bpp_fs_open(dst_xhtml_file, BTMTK_FS_READ_WRITE|BTMTK_FS_CREATE_ALWAYS | BTMTK_FS_CREATE);
    if(fh < 0)
    {    
        return -1;
    }
//mtk03036 20100512 end

	if (obj_type == BPP_MIME_TEXT_VCARD || obj_type == BPP_MIME_TEXT_VCARD30)
	{
		/* vCard is special */
		data_len = bpp_compose_vcard_to_xhtml(fh, obj_name);
		
	}
	else
	{
		/* other types: embed */
		data_len = bpp_embed_object_into_xhtml(fh, obj_name, obj_type);
	}

    bpp_fs_close(fh);

	return data_len;
}


U8* bpp_get_xhtml_prologue(void)
{
	return XHTML_START_10;
}

S32 bpp_embed_object_into_xhtml(BTMTK_FS_HANDLE fh, U16 *obj_name, bt_bpp_mime_type obj_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8   *pbuf = NULL;
    U8*   cdata = 0;
    int data_len, write_len;        
    S32  ret;
    U32 total_len = 0;
	U8  *obj_type_str;
	U8*  obj_asc_filename;
	
	static S8 object_xhtml_head_template[]=	
	"<head>\r\n"
	"<title></title>\r\n"
	"<style type=\"text/css\">\r\n"
	"@page {\r\n"
	/*"size: %dmm %dmm ; counter-increment:pages; margin-left: %dmm; margin-right: %dmm; margin-top: %dmm; margin-bottom: %dmm;\r\n"*/
	"}\r\n"
	"div.center { text-align: center; }\r\n"
	".img-css {vertical-align: top; text-align: left;}\r\n"
	"</style>\r\n"
	"</head>\r\n";
	
	static S8 object_xhtml_body_template[]=
	"<body>\r\n"
	"<div align=\"left\">\r\n"
	"<object data=\"%s\" type=\"%s\" />\r\n"
	"</div>\r\n"
	"</body>\r\n"
	"</html>\r\n";


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_TRACE_INFO_ARG1(BPP_EMBED_OBJECT_INTO_XHTML, obj_type); 

	obj_asc_filename = get_ctrl_buffer(BTMTK_MAX_FILE_NAME_LEN);
    btmtk_chset_ucs2_to_utf8_string(obj_asc_filename, BTMTK_MAX_FILE_NAME_LEN, (U8*)obj_name);


    cdata = bpp_get_xhtml_prologue();

    data_len = strlen((char*)cdata);

    ret = bpp_fs_write(fh, cdata, data_len, &write_len);
	total_len += write_len;
	if (ret < 0)
	{
		total_len = 0;
	    goto RET;
	}

    ret = bpp_fs_write(fh, object_xhtml_head_template, strlen((char*)object_xhtml_head_template), &write_len);
	total_len += write_len;
	if (ret < 0)
	{
		total_len = 0;
	    goto RET;
	}

	obj_type_str = (U8 *)bpp_get_doc_format(obj_type);
	if (obj_type_str == NULL)
	{
		total_len = 0;
	    goto RET;
	}

	pbuf = get_ctrl_buffer(strlen((char*)object_xhtml_body_template) 
							+ strlen((char*)obj_asc_filename)
							+ strlen((char*)obj_type_str) + 10);
		
    data_len = sprintf((char*)pbuf, (char*)object_xhtml_body_template, obj_asc_filename, obj_type_str);
    ret = bpp_fs_write(fh, pbuf, data_len, &write_len);
	total_len += write_len;
	if (ret < 0)
	{
		total_len = 0;
	    goto RET;
	}

RET:

	if (obj_asc_filename)
	{
		free_ctrl_buffer(obj_asc_filename);
	}
	if (pbuf)
	{
		free_ctrl_buffer(pbuf);
	}

   return total_len; 

}

S32 bpp_pre_process_object_data(bpp_printer_struct *printer, bt_bpp_object *print_req)
{
	S32 rst = 0;
	U16 *tmp_txt_file = NULL;
	U16 *tmp_xhtml_file = NULL;
	U16 *p_obj_name = print_req->file_name;
	U8 *p_path;
	int path_len = 0;
	int total_len;
#define BPP_MAX_U16_NAME_SIZE (BTMTK_MAX_FILE_NAME_LEN*2)
	
#define BTMTK_BPP_TMP_TXT_FILE  L"btmtkbpp.txt"
#define BTMTK_BPP_TMP_XHTML_FILE   L"btmtkbpp.htm"


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY(BPP_PRE_PROCESS_OBJECT_DATA);


	/* First init the file flag: default is original file */
	printer->printfile_flag = BPP_FILE_FLAG_ORIG; 

	/* get path */
	p_path = (U8*)(print_req->file_name + btmtk_wstrlen(print_req->file_name) - 1);
	while (p_path > (U8*)print_req->file_name)
	{
	    if( (*p_path == '\\' || *p_path == '/') && p_path[1] == 0 )
	    {
	    	path_len = (p_path - (U8*)print_req->file_name) + 2;
			break;
	    }

		p_path -= 2;
	}

	/* first process text encoding */
	if (print_req->mime_type == BPP_MIME_TEXT_PLAIN)
	{
		BT_BOOL utf8_file =	bpp_is_file_utf8_encoding(print_req->file_name);

		if (!utf8_file)
	    {
			S32 utf8_file_size;

		    tmp_txt_file = get_ctrl_buffer(BPP_MAX_U16_NAME_SIZE);
			OS_MemSet((U8 *)tmp_txt_file, 0, BPP_MAX_U16_NAME_SIZE);
			
			//if (path_len > 0)
	    	    //OS_MemCopy((U8*)tmp_txt_file, (U8*)print_req->file_name, path_len);

			BPP_GET_FULL_PATH(tmp_txt_file, BTMTK_BPP_TMP_TXT_FILE);		
		
			/* convert file to utf8 */
			utf8_file_size = bpp_convert_filecontent_to_utf8(print_req->file_name, tmp_txt_file);

			if (utf8_file_size <= 0)
			{
				//BT_BPP_TRACE_ERROR_ARG1(BPP_CONVERT_TEXT2UTF8_FAILED, ret);
			    bpp_fs_delete(tmp_txt_file);
				rst = 0;
				goto RET;
			}
			p_obj_name = tmp_txt_file;

			btmtk_wstrcpy(printer->print_object.file_name, tmp_txt_file);
			printer->print_object.object_size = utf8_file_size;
			printer->printfile_flag = BPP_FILE_FLAG_TEMP; 
			
		}
    }


	/* check if printer support the doc type:
	 * if not, then compose xhtml
	 */

	if (!bpp_printer_support_doctype(printer, print_req->mime_type))
	{
	    tmp_xhtml_file = get_ctrl_buffer(BPP_MAX_U16_NAME_SIZE);
	    OS_MemSet((U8 *)tmp_xhtml_file, 0, BPP_MAX_U16_NAME_SIZE);

	    //if (path_len > 0)
    	        //OS_MemCopy((U8*)tmp_xhtml_file, (U8*)print_req->file_name, path_len);

	    BPP_GET_FULL_PATH(tmp_xhtml_file, BTMTK_BPP_TMP_XHTML_FILE);
        
		// printer not support the doc type, then compose xhtml 
	    total_len = bpp_compose_object_into_xhtml(tmp_xhtml_file, p_obj_name, print_req->mime_type);
	    if (total_len <= 0)
	    {
	        BT_BPP_TRACE_ERROR_ARG1(BPP_COMPOSE_XHTML_FAILED, total_len);
		bpp_fs_delete(tmp_xhtml_file);
		rst = 0;
		goto RET;
	    }

	    btmtk_wstrcpy(printer->print_object.file_name, tmp_xhtml_file);
	    printer->print_object.mime_type = BPP_MIME_APPLICATION_XHTML_PRINT_100;
	    printer->print_object.object_size += total_len;		
	    printer->printfile_flag = BPP_FILE_FLAG_TEMP; 
        }

	rst = print_req->object_size;

RET:
	if (tmp_txt_file)
		free_ctrl_buffer(tmp_txt_file);

	if (tmp_xhtml_file)
		free_ctrl_buffer(tmp_xhtml_file);

	BT_BPP_TRACE_INFO_ARG1(BPP_PRE_PROCESS_OBJECT_DATA_RETURN, rst);

	return rst;
}


#define UTIL_FUNC

/*****************************************************************************
 * FUNCTION
 *  bpp_set_indexed_flag
 * DESCRIPTION
 *  This function set support flag
 * PARAMETERS
 *  enum_str            [IN]        
 *  enum_str_array      [IN]        
 *  array_size          [IN]        
 *  pflag_array         
 * RETURNS
 *  void
 *****************************************************************************/
static kal_uint32 bpp_set_indexed_flag(
                    const char *enum_str,
                    const char **enum_str_array,
                    kal_uint32 array_size,
                    kal_uint8 *pflag_array)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < array_size; i++)
    {
        if (strcmp(enum_str, enum_str_array[i]) == 0)
        {
            break;
        }
    }
    if (i != array_size && pflag_array)
    {
        pflag_array[i] = KAL_TRUE;
    }
    return i;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_destory_buf_obj
 * DESCRIPTION
 *  This function destory operation context
 * PARAMETERS
 *  pbuf_obj  
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_destory_buf_obj(bpp_buf_obj_struct *pbuf_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbuf_obj)
    {
        bpp_mem_free(pbuf_obj);
    }
}

void bpp_progress_ind(bpp_printer_struct *printer)
{
	BppCallbackParms  parms;

	if (printer == NULL)
		return;

    parms.printer_handle = printer->handle;
	parms.oper = BPP_OPER_PRINT_DOC;
	parms.event = BPP_EVENT_PROGRESS_IND;

	if (printer->current_oper)
	{
		parms.info.prog_status.sent_data_len = printer->current_oper->object.send_obj.total_size_sent;
		parms.info.prog_status.total_data_len = printer->current_oper->data_len;
	}
	else
	{
		parms.info.prog_status.sent_data_len = 99;
		parms.info.prog_status.total_data_len = 100;
	}

    BppAppCallback(&parms);

	/* don't continue to send progress ind to app
	 * until we receive progress rsp from app
	 */
	printer->cont_prog_ind = KAL_FALSE;
}
 
static int bpp_HexStrToInt(kal_char * s)
{
	int val = 0;
	kal_char c;
	int i, len;

#define IS_DIGIT(c) ((c)>='0' && (c)<='9')

	len = strlen(s);

	for (i = 0; i < len; i++)
	{
		c = s[i];
		
		if (IS_DIGIT(c))
			val = val * 16 + (c - '0');
		else if (c >= 'a' && c <= 'f')
			val = val * 16 + (c - 'a' + 10);
		else if (c >= 'A' && c <= 'F')
			val = val * 16 + (c - 'A' + 10);
	}

	return val;
}

 
