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
 *  bpp_vcard.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for BPP vcard composing.
 *
 * Author:
 * -------
 *   From MAUI, revised by Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/


#include "stdio.h"      /* Basic C I/O functions */
#include "stdlib.h"     /* standard lib */

#include "bttypes.h"

#include "osapi.h"
#include "bt_adp_fs.h"  /* FS APIs */
#include "bt_adp_fs_ucs2.h"
#include "bt_osal.h"  /* str APIs */

#define  FHANDLE S32 /* used in xml_def.h */

#include "xml_def.h"    /* XML_PARSER */

#include "goep.h"

#include "bluetooth_bpp_common.h"

#include "bpp.h"
#include "bppi.h"

#include "bt_osal.h"

typedef enum
{
    BPP_VCARD_FIELD_NAME,
    BPP_VCARD_FIELD_TEL_CELL,
    BPP_VCARD_FIELD_TEL_HOME,
    BPP_VCARD_FIELD_TEL_WORK,
    BPP_VCARD_FIELD_TEL_FAX,
    BPP_VCARD_FIELD_CMPNY,
    BPP_VCARD_FIELD_EMAIL,
    BPP_VCARD_FIELD_BIRTHDAY,
    BPP_VCARD_FIELD_TITLE,
    BPP_VCARD_FIELD_URL,                    
    BPP_VCARD_FIELD_ADR_POBOX,
    BPP_VCARD_FIELD_ADR_EXTENSION,
    BPP_VCARD_FIELD_ADR_STREET,
    BPP_VCARD_FIELD_ADR_CITY,
    BPP_VCARD_FIELD_ADR_STATE,
    BPP_VCARD_FIELD_ADR_POSTALCODE,
    BPP_VCARD_FIELD_ADR_COUNTRY,
    BPP_VCARD_FIELD_NOTE,
    //BPP_VCARD_FIELD_IMPS,
    //BPP_VCARD_FIELD_VOIP,
    BPP_VCARD_FIELD_TOTAL
} bpp_vcard_field_enum;

static const S8 *bpp_vcard_field_str[BPP_VCARD_FIELD_TOTAL] = 
{
    "Name",
    "Mobile",
    "Home Number",
    "Office Number",
    "Fax",
    "Company",
    "Email",
    "Birthday",
    "Title",
    "Url",
    "P.O. Box",
    "Extension",
    "Street",
    "City",
    "State/Province",
    "Postal/ZIP Code",
    "Country/Region",
    "Note",
    //"Imps",
    //"Voip",
};

//extern BT_BOOL btmtk_vcard_parse_file_to_struct(U16* filename, btmtk_vcard_data_struct* vcard_struct);


static const S8 vcard_xhtml_head_template[]=    
	"<head>\r\n"
	"<title></title>\r\n"
	"<style type=\"text/css\">\r\n"
	"table {border-collapse: collapse; border-spacing: 2pt;}\r\n"
	"div.center {text-align: center; }\r\n"
	".title {padding: 4px; border-left: solid 1px Gray; border-right: solid 1px Gray; border-top: solid 1px Gray; border-bottom: solid 1px Gray; color: Black; vertical-align: middle; text-align: left; }\r\n"
	".value {padding: 4px; border-left: solid 1px Gray; border-right: solid 1px Gray; border-top: solid 1px Gray; border-bottom: solid 1px Gray; vertical-align: bottom; text-align: left; }\r\n"
	"</style>\r\n"
	"</head>\r\n";


static const S8 vcard_xhtml_body_start[]=
	"<body>\r\n"
	"<div align=\"center\">\r\n"
	"<table>\r\n";
	//"<caption>vCard</caption>\r\n";

static const S8 vcard_xhtml_attr_template[]=
	"<tr>\r\n"
	"<td class=\"title\">%s</td>\r\n"
	"<td class=\"value\">%s</td>\r\n"
	"</tr>\r\n";

static const S8 vcard_xhtml_body_end[]=
	"</table>\r\n"
	"</div>\r\n"
	"</body>\r\n"
	"</html>\r\n";
	




static U32 bpp_compose_vcard_attr_to_xhtml(BTMTK_FS_HANDLE fh, btmtk_vcard_data_struct *p_vcard)
{
    U32 total_len = 0;
    //U32 data_len, write_len;
    int data_len, write_len;
    S8* pbuf = NULL;
	U8* pvalue = NULL;
	U8* pname = NULL;
    U8 field;
	int ret;
#define BPP_TMP_BUF_LEN 512	
	
	pbuf = get_ctrl_buffer(BPP_TMP_BUF_LEN);
	
    for (field = 0; field < BPP_VCARD_FIELD_TOTAL; field++)
    {
    	/* get value */
		
    	switch (field)
    	{
	    case BPP_VCARD_FIELD_NAME:
			{
				int fnlen, lnlen;

				fnlen = strlen((char*)p_vcard->first_name);
				lnlen = strlen((char*)p_vcard->second_name);
				if ((fnlen > 0) && (lnlen > 0))
				{
					pname = get_ctrl_buffer(fnlen + lnlen + 2);
					strcpy((char*)pname, (char*)p_vcard->first_name);
					strcat((char*)pname, (char*)p_vcard->second_name);
					pvalue = pname;
				}
				else
				{
					pvalue = fnlen > 0 ? p_vcard->first_name : p_vcard->second_name;
				}
				/* compose */
		    	sprintf(pbuf, vcard_xhtml_attr_template, bpp_vcard_field_str[field], pvalue);
				/* write to file */
			    data_len = strlen((char*)pbuf);
			    ret = bpp_fs_write(fh, pbuf, data_len, &write_len);
				total_len += write_len;
				if (pname)
				{
					free_ctrl_buffer(pname);
					pname = NULL;
				}						
	    	}
		
			/* get the next item */
			continue;
			
	    case BPP_VCARD_FIELD_TEL_CELL:
			pvalue = p_vcard->cell_num;
			break;
	    case BPP_VCARD_FIELD_TEL_HOME:
			pvalue = p_vcard->home_num;
			break;
	    case BPP_VCARD_FIELD_CMPNY:
			pvalue = p_vcard->company;
			break;
	    case BPP_VCARD_FIELD_EMAIL:
			pvalue = p_vcard->email;
			break;
	    case BPP_VCARD_FIELD_TEL_WORK:
			pvalue = p_vcard->office_num;
			break;
	    case BPP_VCARD_FIELD_TEL_FAX:
			pvalue = p_vcard->fax_num;
			break;
	    case BPP_VCARD_FIELD_BIRTHDAY:
			pvalue = p_vcard->birthday;
			break;
	    case BPP_VCARD_FIELD_TITLE:
			pvalue = p_vcard->title;
			break;
	    case BPP_VCARD_FIELD_URL:                    
			pvalue = p_vcard->url;
			break;
	    case BPP_VCARD_FIELD_ADR_POBOX:
			pvalue = p_vcard->postbox;
			break;
	    case BPP_VCARD_FIELD_ADR_EXTENSION:
			pvalue = p_vcard->extension;
			break;
	    case BPP_VCARD_FIELD_ADR_STREET:
			pvalue = p_vcard->street;
			break;
	    case BPP_VCARD_FIELD_ADR_CITY:
			pvalue = p_vcard->city;
			break;
	    case BPP_VCARD_FIELD_ADR_STATE:
			pvalue = p_vcard->state;
			break;
	    case BPP_VCARD_FIELD_ADR_POSTALCODE:
			pvalue = p_vcard->postcode;
			break;
	    case BPP_VCARD_FIELD_ADR_COUNTRY:
			pvalue = p_vcard->country;
			break;			
	    case BPP_VCARD_FIELD_NOTE:
			pvalue = p_vcard->note;
			break;			
			
	    default:
			pvalue = NULL;
	    	break;
    	}

		if (pvalue && (strlen((char*)pvalue) > 0))
		{
			/* compose */
	    	sprintf(pbuf, vcard_xhtml_attr_template, bpp_vcard_field_str[field], pvalue);

			/* write to file */
		    data_len = strlen((char*)pbuf);
		    ret = bpp_fs_write(fh, pbuf, data_len, &write_len);
			total_len += write_len;
		}
    }

	if (pbuf)
		free_ctrl_buffer(pbuf);

#undef BPP_TMP_BUF_LEN

	return total_len;
}


U32 bpp_compose_vcard_to_xhtml(BTMTK_FS_HANDLE fh, U16* vcard_file)
{
	btmtk_vcard_data_struct *p_vcard = NULL;
	S32 parse_rst;
	U8* cdata;
    int data_len, write_len;        
    S32  ret;
    U32 total_len = 0;

	BT_BPP_FUNC_ENTRY(BPP_COMPOSE_VCARD2XHTML);

	/* Parsing */
	p_vcard = get_ctrl_buffer(sizeof(btmtk_vcard_data_struct));
	OS_MemSet((U8*)p_vcard, 0, sizeof(btmtk_vcard_data_struct));

	parse_rst = btmtk_vcard_parse_file_to_struct(vcard_file, p_vcard);
	BT_BPP_TRACE_INFO_ARG1(BPP_PARSE_VCARD_FILE2STRUCT_RETURN, parse_rst);

	if (parse_rst != BTMTK_VCD_ERR_OK)
	{
		/* failed */
		free_ctrl_buffer(p_vcard);
		return 0;
	}

	/* Composing */

	/* 1. prologue */
	cdata = bpp_get_xhtml_prologue();
    data_len = strlen((char*)cdata);
    ret = bpp_fs_write(fh, cdata, data_len, &write_len);
	total_len += write_len;

	/* 2. head */
	cdata = (U8* )vcard_xhtml_head_template;
    data_len = strlen((char*)cdata);
    ret = bpp_fs_write(fh, cdata, data_len, &write_len);
	total_len += write_len;

	/* 3. body start */
	cdata = (U8* )vcard_xhtml_body_start;
    data_len = strlen((char*)cdata);
    ret = bpp_fs_write(fh, cdata, data_len, &write_len);
	total_len += write_len;

	/* 4. vCard attributes */
	write_len = bpp_compose_vcard_attr_to_xhtml(fh, p_vcard);
	total_len += write_len;

	/* 5. body end */
	cdata = (U8* )vcard_xhtml_body_end;
    data_len = strlen((char*)cdata);
    ret = bpp_fs_write(fh, cdata, data_len, &write_len);
	total_len += write_len;

	
	if (p_vcard)
		free_ctrl_buffer(p_vcard);
	
	return total_len;
}


BT_BOOL bpp_is_file_utf8_encoding(U16* filename)
{
	BT_BOOL ret = TRUE;
	BTMTK_CHSET_ENUM file_chset;	
	
	file_chset =  btmtk_chset_get_file_encoding(filename);
	BT_BPP_TRACE_INFO_ARG1(BPP_GET_FILE_CHARSET_RETURN, file_chset);

	if (file_chset != BTMTK_CHSET_UTF8)
	{
		ret = FALSE;
	}

	BT_BPP_TRACE_INFO_ARG1(BPP_IS_UTF8_TEXT_RETURN, ret);

	return ret;
}

S32 bpp_convert_filecontent_to_utf8(U16* src_file, U16* dst_file)
{
	kal_uint32 uft8_filesize = 0;
	U8 ret;
    BTMTK_FS_HANDLE dst_fh;

	BT_BPP_FUNC_ENTRY(BPP_CONVERT_FILE_TO_UTF8);

	ret = btmtk_chset_set_file_encoding(src_file, BTMTK_CHSET_NONE, dst_file, BTMTK_CHSET_UTF8);
	BT_BPP_TRACE_INFO_ARG1(BPP_CONVERT_TO_UTF8_RETURN, ret);

	/* get UTF8 file size */
	if (ret > 0)
	{
	    dst_fh = btmtk_fs_open_ucs2(dst_file, BTMTK_FS_READ_ONLY);
	    if (dst_fh <= 0)
	        return 0;

		btmtk_fs_get_filesize_ucs2(dst_fh, &uft8_filesize);

		btmtk_fs_close_ucs2(dst_fh);
	}

	return (S32)uft8_filesize;
}

 
