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
 * bt_bip_porting.c
 *
 * Project:
 * --------
 * YuSu
 *
 * Description:
 * ------------
 * APIs
 *
 * Author:
 * -------
 * Paul Chuang
 *
 ****************************************************************************/
#ifdef  __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_ext_debug.h"
#else
#include <cutils/xlog.h>
#endif
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>


#include "bt_bip_comm.h"
#include "bt_bipi_struct.h"
#include "bt_bipi_api.h"

#include "bt_bipr_struct.h"
#include "bt_bipr_api.h"

#include "bluetooth_struct.h"
#include "bluetooth_bipi_struct.h"
#include "bt_message.h"
#include "bt_mmi.h"

#include "bt_ext_utility.h"
//#include "ext_osal.h"

#include "bt_bip_porting.h"
#include "bt_bip_internal.h"







#define BIP_PORTING_LAYER_DEBUG 1

#ifdef BIP_PORTING_LAYER_DEBUG
#define BIP_DBG(fmt, ...) XLOGD("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#else
#define BIP_DBG(fmt, ...) {}
#endif
#define BIP_ERR(fmt, ...) XLOGE("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)

extern S32 ext_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size,const U8 *src);
extern S32 ext_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size,const U8 *src);

static int g_apisock;
BTMTK_BIP_JNI_CALLBACK g_bip_jni_callback = NULL;


BT_BOOL btmtk_bip_set_jni_callback(BTMTK_BIP_JNI_CALLBACK pfCallback, int apisock)
{
    g_bip_jni_callback = pfCallback;
    g_apisock = apisock;
    return TRUE;
}


void bip_postevent(U8 event, void* parameters, U8 count)
{
    if( NULL !=  g_bip_jni_callback)
    {
        g_bip_jni_callback(event, parameters, count);
    }
}


void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
    int ret = -1;
    int sockfd = -1;

    BIP_DBG("BTCMD_SendMessage msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);

    U16 *field;

    ilm_struct ilm;

    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.sap_id = 0;
    ilm.src_mod_id = MOD_MMI;
    ilm.dest_mod_id = MOD_BT;

    //assert (size <= MAX_ILM_BUFFER_SIZE);

    if(ptr != NULL)
        memcpy(ilm.ilm_data, ptr, size);
    // LOCAL_PARA_HDR 
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

    sockfd = g_apisock;

    if(sockfd >= 0 )
    {
        ret = send(sockfd, (void*)&ilm, size, 0);
        if(ret < 0)
        {
            BIP_ERR("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            BIP_DBG("send msg success : %d", ret);
        }
    }
    else
    {
        BIP_ERR("socket uninitialized");
    }
}

/********************************************************************************
*   
********************************************************************************/

enum BIP_OP_ENUM
{
    BIP_OP_NONE,
    BIP_OP_ACTIVATE,
    BIP_OP_DEACTIVATE,
    BIP_OP_GET_CAPA,
    BIP_OP_PUSH_IMG,
    BIP_OP_PUSH_THUM,
    BIP_OP_DISCONNECT
};

typedef struct
{
    const char * s;
    U32 v;
} bip_encoding_map;

static const bip_encoding_map map[]=
{
    {"JPEG", BT_BIP_TYPE_JPEG},
    {"BMP", BT_BIP_TYPE_BMP},
    {"GIF", BT_BIP_TYPE_GIF},
    {"WBMP", BT_BIP_TYPE_WBMP},
    {"PNG", BT_BIP_TYPE_PNG},
    {"", 0}
};

static const char* const status_str[] =
{
    "",
    "Indicating",
    "Responding",
    "Completed",
    "Requesting",
    "Confirming"
};


static HMTKBIPI g_i_handle;
static U8 g_i_opcode;
static U8 g_i_bchal;
static U8 g_img_handle[8];
static HMTKBIPR g_r_handle;
static U8 g_r_opcode;
static U8 g_r_bchal;


static T_BIP_STATE g_i_state = BIP_STATE_DISABLED;
static T_BIP_STATE g_r_state = BIP_STATE_DISABLED;

static T_BIP_OBJECT g_sending_object;
static int g_sending_len = 0;
static T_BIP_OBJECT g_receiving_object;
static int g_receiving_len = 0;

static char g_root_path[MAX_FILE_NAME_LEN];
/********************************************************************************
*   Initiator
********************************************************************************/
//AUX function
//-------------------------------------------------------------------------------
void bip_util_convert_string2array(const char *source, T_BDADDR dest)
{
    BIP_DBG("+");
    int i;
    char tmp;
    char *ptr = (char*)source;
    for(i = 6;i > 0;)
    {
        dest[--i] = strtoul(ptr, &ptr, 16);
        ptr++;
    }
    BIP_DBG("%s==>0x%02X%02X%02X%02X%02X%02X",
        source, dest[0], dest[1], dest[2], dest[3], dest[4], dest[5]);
    BIP_DBG("-");
}

void bip_util_reverse_array(U8 *dest, U8 *source)
{
    dest[5] = source[0];
    dest[4] = source[1];
    dest[3] = source[2];
    dest[2] = source[3];
    dest[1] = source[4];
    dest[0] = source[5];
}

static void wchar_net_to_local(unsigned short* str)
{
    unsigned short t;

    while(*str)
    {
        t = *((unsigned char*)str) << 8 | *(((unsigned char*)str) + 1);
        *str = t;
        str++;
    }
}

static U32 bip_get_encoding_val(const char* s)
{
    U32 i = 0;

    while(map[i].s[0])
    {
        if (!strcmp(map[i].s, s))
            break;
        i++;
    }
    return map[i].v;
}

static const char* bip_get_encoding_str(U32 v)
{
    U32 i = 0;

    while(map[i].s[0])
    {
        if (map[i].v == v)
            break;
        i++;
    }
    return map[i].s;
}

static U32 bip_get_mtk_trans_val(T_BIP_TRANS v)
{
    switch (v)
    {
    case BIP_TRANS_CROP:            
        return BT_BIP_IMG_TRANS_CROP;
        
    case BIP_TRANS_FILL:
        return BT_BIP_IMG_TRANS_FILL;
        
    case BIP_TRANS_STRETCH:
        return BT_BIP_IMG_TRANS_STRECH;
        
    default:
        break;
    }
    return BT_BIP_IMG_TRANS_NONE;

}

static S32 bip_check_pixel(U16 w, U16 h, bt_bip_pixel* pixel)
{
    if (!pixel->height2 && !pixel->width2)
    {
        if (w == pixel->height2 && h == pixel->width2)
            return 1;
        else
            return 0;
    }
    else if (pixel->height2 && pixel->width2)
    {
        if (pixel->height)
        {
            if ((pixel->width <= w && pixel->width2 >= w) &&
                (pixel->height <= h && pixel->height2 >= h))
                return 1;
            else
                return 0;
        }
        else
        {
            if ((pixel->height2 / pixel->width2) == (h/w))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

/********************************************************************************
* Initator START  
*********************************************************************************/
//-------------------------------------------------------------------------------
//Initator Report START----------------------------------------------------------
//-------------------------------------------------------------------------------
static void bipi_report_enable(S32 result)
{

    if (result == BT_BIP_ERR_OK)
    {
        if (g_i_state == BIP_STATE_DISABLED)
        {
            BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_ENABLED);
            g_i_state = BIP_STATE_ENABLED;
        }

        bip_postevent(BIP_INITIATOR_ENABLE_SUCCESS, NULL, 0);
    }
    else
    {
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_DISABLED);
        g_i_state = BIP_STATE_DISABLED;

        bip_postevent(BIP_INITIATOR_ENABLE_FAIL, NULL, 0);
    }
}


static void bipi_report_disable(S32 result)
{
    if (result == BT_BIP_ERR_OK)
    {
      BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_DISABLED);
      g_i_state = BIP_STATE_DISABLED;

      bip_postevent(BIP_INITIATOR_DISABLE_SUCCESS, NULL, 0);
    }
    else
    {
      bip_postevent(BIP_INITIATOR_DISABLE_FAIL, NULL, 0);
    }
}


static void bipi_report_capabilities(S32 result, bt_bip_capabilities_cnf* cap, S32 b_state)
{
    U32 i;
    S32 ret;

    T_BIP_IMAGING_CAPABILITY ImagingCapability;

    
    if (result == BT_BIP_ERR_INDICATING)
    {
        BIP_DBG("<-- Client : [GET_CAPABILITY], progress indication!");
        
        ret = btmtk_bipi_continue(g_i_handle, 1);
        BIP_DBG("--> BIP_I : bipi_continue() ret = %d", ret);
        return;
    }


    if (result == BT_BIP_ERR_OK)
    {
        int NumImageFormats = 0;
        for (i = 0; i < BT_BIP_MAX_IMAGE_FORMATES && NumImageFormats < MAX_NUM_OF_NESTED_ATTR; i++)
        {
            if (cap->supported_formats[i].encoding )
            {
                NumImageFormats++;
            }
            else
            {
                break;
            }
        }
        S8* para[9 + NumImageFormats*6];

        for (i = 0; i < (U32)(9 + NumImageFormats*6); i++)
        {
            para[i] = (S8*)malloc(sizeof(S8) * MAX_DIGIT_SIZE);
        }

        S8 version[MAX_DIGIT_SIZE];
        S8 encoding[MAX_DIGIT_SIZE];
        S8 width[MAX_DIGIT_SIZE];
        S8 height[MAX_DIGIT_SIZE];
        S8 width2[MAX_DIGIT_SIZE];
        S8 height2[MAX_DIGIT_SIZE];
        S8 size[MAX_DIGIT_SIZE];
        S8 transform[MAX_DIGIT_SIZE];
        S8 numImageFormats[MAX_DIGIT_SIZE]; 

        sprintf(version, "%s", "1.0");
        BIP_DBG("ImagingCapability.PreferFormat.Version : -> (%s)", version);
        sprintf(encoding, "%s", bip_get_encoding_str(cap->preferred_format.encoding));
        BIP_DBG("ImagingCapability.PreferFormat.Encoding : -> (%s)", encoding);

        sprintf(width, "%d", cap->preferred_format.pixel.width);
        BIP_DBG("ImagingCapability.PreferFormat.width : (%s) <- (%u)", width, cap->preferred_format.pixel.width);
        sprintf(height, "%d", cap->preferred_format.pixel.height);
        BIP_DBG("ImagingCapability.PreferFormat.height : (%s) <- (%u)", height, cap->preferred_format.pixel.height);
        sprintf(width2, "%d", cap->preferred_format.pixel.width2);
        BIP_DBG("ImagingCapability.PreferFormat.width2: (%s) <- (%u)", width2, cap->preferred_format.pixel.width2);
        sprintf(height2, "%d", cap->preferred_format.pixel.height2);
        BIP_DBG("ImagingCapability.PreferFormat.height2 : (%s) <- (%u)", height2, cap->preferred_format.pixel.height2);
        sprintf(size, "%lu", cap->preferred_format.size);
        BIP_DBG("ImagingCapability.PreferFormat.size : (%s) <- (%lu)", size, cap->preferred_format.size);
        sprintf(transform, "%lu", cap->preferred_format.transformation);
        BIP_DBG("ImagingCapability.PreferFormat.transform : (%s) <- (%lu)", transform, cap->preferred_format.transformation);
        sprintf(numImageFormats, "%d", NumImageFormats);
        BIP_DBG("numImageFormats :  (%s) <- (%u)", numImageFormats, NumImageFormats);



        int j;
        for(j = 0; j < MAX_DIGIT_SIZE; j++)
        {
            *(para[PreferFormat_Version] + j) =  version[j];
            *(para[PreferFormat_Encoding] + j) =  encoding[j];
            *(para[PreferFormat_Width] + j) =  width[j];
            *(para[PreferFormat_Height] + j) =  height[j];
            *(para[PreferFormat_Width2] + j) =  width2[j];
            *(para[PreferFormat_Height2] + j) = height2[j];
            *(para[PreferFormat_Size] + j) =  size[j];
            *(para[Transform] + j) =  transform[j];
            *(para[ImageFormats_NumImageFormats] + j) = numImageFormats[j];
        }

        for (i = 0; i < (U32)NumImageFormats ; i++)
        {
            S8 encoding[MAX_DIGIT_SIZE];
            S8 width[MAX_DIGIT_SIZE];
            S8 height[MAX_DIGIT_SIZE];
            S8 width2[MAX_DIGIT_SIZE];
            S8 height2[MAX_DIGIT_SIZE];
            S8 size[MAX_DIGIT_SIZE];

            sprintf( encoding, "%s", bip_get_encoding_str(cap->supported_formats[i].encoding));
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Encoding : -> (%s)", i, encoding);
            sprintf( width, "%d", cap->supported_formats[i].pixel.width);
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Width : (%s) <- (%d)", i, width, cap->supported_formats[i].pixel.width);
            sprintf( height, "%d", cap->supported_formats[i].pixel.height);
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Height : (%s) <- (%d)", i, height, cap->supported_formats[i].pixel.height);
            sprintf( width2, "%d", cap->supported_formats[i].pixel.width2);
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Width2 : (%s) <- (%d)", i, width2, cap->supported_formats[i].pixel.width2);
            sprintf( height2, "%d", cap->supported_formats[i].pixel.height2);
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Height2 : (%s) <- (%d)", i, height2, cap->supported_formats[i].pixel.height2);
            sprintf( size, "%lu", cap->supported_formats[i].size);
            BIP_DBG("ImagingCapability.ImageFormats[%lu].Size : (%s) <- (%lu)", i, size, cap->supported_formats[i].size);
            


            for(j = 0; j < MAX_DIGIT_SIZE; j++)
            {
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Encoding] + j) =  encoding[j];
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Width] + j) =  width[j];
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Height] + j) =  height[j];
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Width2] + j) =  width2[j];
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Height2] + j) =  height2[j];
                *(para[ImageFormats_NumImageFormats + i*6 + ImageFormats_Size] + j) =  size[j];
            }
        }


        if (g_i_opcode == BIP_OP_GET_CAPA)
        {
            BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);
            g_i_state = BIP_STATE_CONNECTED;

            g_i_opcode = 0;
            
            bip_postevent(BIP_INITIATOR_GET_CAPABILITY_SUCCESS, para, 9 + NumImageFormats*6 );
            for (i = 0; i < (U32)(9 + NumImageFormats*6) ; i++)
            {
               free(para[i]);
            }

            BIP_DBG("post success event : BIP_INITIATOR_GET_CAPABILITY_SUCCESS");
        }        
        else
        {
            BIP_DBG("get capa success in error opcode = %d", g_i_opcode);
        }
    }
    else if (result == BT_BIP_ERR_USER_ABORT)
    {
        g_i_state = BIP_STATE_DISCONNECTING;
        bip_postevent(BIP_INITIATOR_CANCEL_SUCCESS, NULL, 0);
        BIP_DBG("--> post success event : BIP_INITIATOR_CANCEL_SUCCESS, in case of get-cap abort");
        
        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        BIP_DBG("--> BIP_I : disconnect force in case of get-capa abort, ret = %d, %d ",ret, g_i_opcode);
    }
    else
    {
        if (b_state)
        {
            BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, b_state);
            g_i_state = b_state;

            g_i_opcode = 0;
        }
        
        bip_postevent(BIP_INITIATOR_GET_CAPABILITY_FAIL, NULL, 0);
    }
}


static void bipi_report_progress(bt_bip_continue_ind* ind)
{
    S32 ret;

    BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_SENDING);
    g_i_state = BIP_STATE_SENDING;
    BIP_DBG("g_sending_len: (%d) -> (%lu)", g_sending_len, g_sending_len + ind->data_len);
    g_sending_len += ind->data_len;

    if (ind->obj_len)
        BIP_DBG("<-- BIP_I : [CONTINUE_IND], sent %lu%c", (U32)((g_sending_len * 100)/ind->obj_len), '%');
    else
        BIP_DBG("<-- BIP_I : [CONTINUE_IND], sent : %d bytes", g_sending_len);

    S8* para[1];
    S8 sendingLen[MAX_DIGIT_SIZE];
    sprintf(sendingLen, "%d", g_sending_len);
    para[0] = sendingLen;
    BIP_DBG("para[0]: SendingLen -> (%s)", sendingLen);


    bip_postevent(BIP_INITIATOR_PROGRESS, para, 1);


    ret = btmtk_bipi_continue(g_i_handle, 1);
    BIP_DBG("--> BIP_I : bipi_continue() ret = %d", ret);
}


static void bipi_report_push_img(S32 result, S32 b_state, bt_bipi_para* para)
{
    if (result == BT_BIP_ERR_INDICATING)
    {
        BIP_DBG("<-- Client : [PUT_IMAGE], progress indication, obj = %lu, data = %lu.",
            para->cnf.ind.obj_len, para->cnf.ind.data_len);
        bipi_report_progress(&para->cnf.ind);
        return;
    }

    /* other result meaning completion of the transaction */
    if (result == BT_BIP_ERR_THUMBNAIL_NEEDED || result == BT_BIP_ERR_OK)
    {
        BIP_DBG("<-- BIP_I : [PUSH_IMAGE] success!");
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);

        g_i_state = BIP_STATE_CONNECTED;
        g_i_opcode = 0;
        strcpy( (char*)g_img_handle,(char*) para->cnf.put_img.handle);


        S8* para[1];
        S8 imageHandle[MAX_DIGIT_SIZE];
        sprintf(imageHandle, "%s", g_img_handle);
        para[0] = imageHandle;
        BIP_DBG("para[0]: imageHandle -> (%s)", imageHandle);

        //bip_postevent(BIP_INITIATOR_IMAGE_PUSH_SUCCESS, para, 1);


        if (result == BT_BIP_ERR_THUMBNAIL_NEEDED)
        {
            BIP_DBG("<-- BIP_I : [PUSH_IMAGE] success, and thumbnail is needed!");
            bip_postevent(BIP_INITIATOR_THUMBNAIL_REQ, para, 1);
        }
        else 
        {
            bip_postevent(BIP_INITIATOR_IMAGE_PUSH_SUCCESS, para, 1);
        }

    }
    else
    {
        BIP_DBG("<-- BIP_I : [PUSH_IMAGE] failed !");

        if (b_state)
        {
            BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, b_state);
            g_i_state = b_state;
            g_i_opcode = 0;
        }

        bip_postevent(BIP_INITIATOR_IMAGE_PUSH_FAIL, NULL, 0);
    }
}


static void bipi_report_push_thumbnail(S32 result, S32 b_state, bt_bipi_para* para)
{
    if (result == BT_BIP_ERR_INDICATING)
    {
        BIP_DBG("<-- Client : [PUT_IMAGE], progress indication, obj = %lu, data = %lu.", para->cnf.ind.obj_len, para->cnf.ind.data_len);
        bipi_report_progress(&para->cnf.ind);

        return;
    }

    g_i_opcode = 0;
    if (result == BT_BIP_ERR_OK)
    {
        BIP_DBG("<-- BIP_I : [PUSH_THUMBNAIL] success!");
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);
        g_i_state = BIP_STATE_CONNECTED;
        bip_postevent(BIP_INITIATOR_THUMBNAIL_PUSH_SUCCESS, NULL, 0);
    }
    else
    {
        BIP_DBG("<-- BIP_I : [PUSH_THUMBNAIL] failed !");

        if (b_state)
        {
            BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, b_state);
            g_i_state = b_state;
        }

        bip_postevent(BIP_INITIATOR_THUMBNAIL_PUSH_FAIL, NULL, 0);
    }
}


static void bipi_report_auth_req(bt_bipi_para* para)
{
    S8* info[1];
    S8 needUid[2];

    if (para->cnf.obauth_ind.option)
    {
        sprintf(needUid, "%s", "1");
    }
    else
    {
        sprintf(needUid, "%s", "0");
    }
    info[0] = needUid;
    BIP_DBG(" <-- Client : [OBEX_AUTH], option = %s.", needUid);

    g_i_bchal = 1;

    bip_postevent(BIP_INITIATOR_OBEX_AUTHREQ, info, 1);
}


static void bipi_report_connect_failure()
{
    switch(g_i_opcode)
    {
    case BIP_OP_GET_CAPA:
        bipi_report_capabilities(BT_BIP_ERR_FAILED, NULL, BIP_STATE_ENABLED);
        break;

    case BIP_OP_PUSH_IMG:
        bipi_report_push_img(BT_BIP_ERR_FAILED, BIP_STATE_ENABLED, NULL);
        break;

    case BIP_OP_PUSH_THUM:
        bipi_report_push_thumbnail(BT_BIP_ERR_FAILED, BIP_STATE_ENABLED, NULL);
        break;

    case BIP_OP_DISCONNECT:
        g_i_opcode = 0;
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_ENABLED);
        g_i_state = BIP_STATE_ENABLED;

        bip_postevent(BIP_INITIATOR_DISCONNECT_SUCCESS, NULL, 0);
        break;

    default:
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_ENABLED);
        g_i_state = BIP_STATE_ENABLED;
        break;

    }
}

static void bipi_report_disconnect(S32 result)
{
    bipi_report_connect_failure();
    g_i_bchal = 0;
	memset(&g_sending_object, 0, sizeof(T_BIP_OBJECT));
	g_sending_len = 0;
}
//-------------------------------------------------------------------------------
//Initator Report END------------------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//Initator Request START---------------------------------------------------------
//-------------------------------------------------------------------------------
static S32 bipi_send_connect_cmd(T_BDADDR BdAddr)
{
    bt_bip_connect_req req;
    memset(&req, 0, sizeof(bt_bip_connect_req));
    
    req.service = BT_BIP_SRV_IMG_PUSH;
    req.addr.lap = BdAddr[3]<< 16 | BdAddr[4] << 8 | BdAddr[5];
    req.addr.uap = BdAddr[2];
    req.addr.nap = BdAddr[0] << 8 | BdAddr[1];
    BIP_DBG("lap: %3lx, uap: %x , nap: %2x", req.addr.lap, req.addr.uap, req.addr.nap);
    return btmtk_bipi_connect(g_i_handle, &req);
}

// this function used in request complete confirm only
static U32 bipi_check_disconnect(S32 result)
{
    S32 ret;

    if (g_i_opcode == BIP_OP_DISCONNECT)
    {
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_DISCONNECTING);
        g_i_state = BIP_STATE_DISCONNECTING;

        if (result == BT_BIP_ERR_OK)
        {
            bip_postevent(BIP_INITIATOR_CANCEL_FAIL, NULL, 0);
        }
        else
        {
            bip_postevent(BIP_INITIATOR_CANCEL_SUCCESS, NULL, 0);
        }

        BIP_DBG("post success event : BIP_INITIATOR_CANCEL event, result = %d", result);
        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        BIP_DBG("disconnect force in case of check_disconnect(), ret = %d ", ret);
        return 1;
    }
    return 0;
}


static S32 bipi_send_put_img_cmd(T_BIP_OBJECT* pobj)
{
    bt_bip_put_img_req req;
    memset(&req, 0, sizeof(bt_bip_put_img_req));
    char* p;

    bip_postevent(BIP_INITIATOR_IMAGE_PUSH_START, NULL, 0);

    p = pobj->DirName + strlen(pobj->DirName);
#ifdef BTMTK_ON_LINUX
    *p = '/';
#else
    *p = '\\';
#endif
    strcpy(p + 1, pobj->FileName);

#if defined(BTMTK_ON_WISE)
    wsConvertFileNameToNativeFormat(req.img_path, pobj->DirName);
#else
    ext_chset_utf8_to_ucs2_string((U8*)req.img_path, 520, (const U8 *)pobj->DirName);
#endif
    *p = 0;
    ext_chset_utf8_to_ucs2_string((U8*)req.img_name, 512,  (const U8 *)pobj->FileName);

    req.img_desc.size = (U32)pobj->ObjectSize;
    //req.img_desc.maxsize = (U32)pobj->AcceptableFileSize;
    req.img_desc.pixel.height = pobj->ImageDesc.Height;
    req.img_desc.pixel.width = pobj->ImageDesc.Width;
    req.img_desc.pixel.height2 = pobj->ImageDesc.Height2;
    req.img_desc.pixel.width2 = pobj->ImageDesc.Width2;
    req.img_desc.encoding = bip_get_encoding_val(pobj->ImageDesc.Encoding);
    req.img_desc.transformation = bip_get_mtk_trans_val(pobj->ImageDesc.Transform);

    return btmtk_bipi_put_image(g_i_handle, &req);
}


static S32 bipi_send_put_thumbnail_cmd(T_BIP_OBJECT* pobj)
{
    bt_bip_put_linked_thum_req req;
    char* p;

    memset((void*)&req, 0, sizeof(bt_bip_put_linked_thum_req));


    bip_postevent(BIP_INITIATOR_THUMBNAIL_PUSH_START, NULL, 0);

    p = pobj->DirName + strlen(pobj->DirName);

#if defined(BTMTK_ON_LINUX)
    *p = '/';
#else
    *p = '\\';
#endif
    strcpy(p + 1, pobj->FileName);

#if defined(BTMTK_ON_WISE)
    wsConvertFileNameToNativeFormat(req.img_path, pobj->DirName);
#elif defined(BTMTK_ON_LINUX)
    ext_chset_utf8_to_ucs2_string((U8*)req.img_path, 520,  (const U8 *)pobj->ThumbnailFullPath);
#else
    ext_chset_utf8_to_ucs2_string((U8*)req.img_path, 520, pobj->DirName);
#endif
    *p = 0;
    strcpy((char*)req.handle, (char*)g_img_handle);

    return btmtk_bipi_put_linked_thumbnail(g_i_handle, &req);
}


static void bipi_start_req()
{
    S32 ret;

    switch(g_i_opcode)
    {
    case BIP_OP_GET_CAPA:
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);
        g_i_state = BIP_STATE_CONNECTED;

        ret = btmtk_bipi_get_capabilities(g_i_handle);
        BIP_DBG("start_req, bipi_get_capabilities return = %d", ret);
        break;

    case BIP_OP_PUSH_IMG:
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);
        g_i_state = BIP_STATE_CONNECTED;

        ret = bipi_send_put_img_cmd(&g_sending_object);
        BIP_DBG("--> BIP_I: start_req, bipi_put_img return = %d", ret);
        break;

    case BIP_OP_PUSH_THUM:
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTED);
        g_i_state = BIP_STATE_CONNECTED;

        ret = bipi_send_put_thumbnail_cmd(&g_sending_object);
        BIP_DBG("--> BIP_I: start_req, bipi_put_thumbnail() ret = %d", ret);
        break;
        
    case BIP_OP_DISCONNECT:
        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_DISCONNECTING);
        g_i_state = BIP_STATE_DISCONNECTING;
                
        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        BIP_DBG("--> BIP_I : bipi_disconnect(1) in case of connect success, ret = %d, g_i_opcode = %d", ret, g_i_opcode);

        break;

    default:
        break;
    }
}
//-------------------------------------------------------------------------------
//Initator Request END-----------------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//Initator Callback START----------------------------callback from bt_bipi_comm.c
//-------------------------------------------------------------------------------
static void bipi_callback(void* p, bt_bipi_para* para)
{

    switch(para->opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        BIP_DBG("[ACTIVATE] : result = %d", para->result);	
        
        if (para->result == BT_BIP_ERR_OK)
        {
            g_i_handle = para->handle;
        }
        //memset(&g_sending_object, 0, sizeof(T_BIP_OBJECT));
        //g_sending_len = 0;

        g_i_opcode = 0;
        bipi_report_enable(para->result);
        break;
        
    case BT_BIP_OP_DEACTIVATE:
        BIP_DBG("[DEACTIVATE] : result = %d", para->result);	
       
        if (para->result == BT_BIP_ERR_OK)
        {
            g_i_handle = NULL;
        }        
        g_i_opcode = 0;
        bipi_report_disable(para->result);
        break;
       
    case BT_BIP_OP_CONNECT:
        BIP_DBG("<--- Client : [CONNECT] : result =%d", para->result);

        if (para->result == BT_BIP_ERR_OK)
        {
            bipi_start_req();
        }
        else
        {
            bipi_report_connect_failure();
        }
        break;

    case BT_BIP_OP_GET_CAPABILITIES:
        BIP_DBG("<-- Client : [GET_CAPABILITIES] : result = %d", para->result);
        // check if disconnect... 
        if (!bipi_check_disconnect(BT_BIP_ERR_FAILED))
        {
            bipi_report_capabilities(para->result, &para->cnf.capa, BIP_STATE_CONNECTED);            
        }        
        break;

    case BT_BIP_OP_PUT_IMG:
        BIP_DBG("<-- Client : [PUT_IMAGE] : result = %d", para->result);
        if (!bipi_check_disconnect(para->result))
        {
            bipi_report_push_img(para->result, BIP_STATE_CONNECTED, para);
        }
        break;

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        BIP_DBG("<-- Client : [PUT_THUMBNAIL] : result = %d", para->result);
        if (!bipi_check_disconnect(para->result))
        {
            bipi_report_push_thumbnail(para->result, BIP_STATE_CONNECTED, para);
        }
        break;

    case BT_BIP_OP_DISCONNECT:
        BIP_DBG("<-- Client : [DISCONNECT] : result = %d.", para->result);
        bipi_report_disconnect(para->result);
        break;

    case BT_BIP_OP_CONTINUE:
        BIP_DBG("<-- Client : [CONTINUE_IND]");
        break;
        
    case BT_BIP_OP_OBEX_AUTH:
        {            
            switch (para->result)
            {
            case BT_BIP_ERR_OBAUTH_NEEDED:
                BIP_DBG("<--Client : [AUTHENTICATION] : indicating");
                bipi_report_auth_req(para);
                break;
            default:
                BIP_DBG("<--Client : [AUTHENTICATION] : result = %d", para->result);
            }            
        }
        break;
        
    case BT_BIP_OP_ABORT:
        // in case of abort failure 
        bipi_start_req();
        break;
        
    }

}
//-------------------------------------------------------------------------------
//Initator Callback END----------------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//Initator API START-------------------------------------------------------------
//-------------------------------------------------------------------------------

void bip_initiator_enable()
{
    int ret;
    bt_bipi_active_info act;
    memset(&act, 0, sizeof(bt_bipi_active_info));


    if (g_i_handle)
    {
    
        bt_bipi_context* cntx = (bt_bipi_context*)g_i_handle;
        cntx->status = BIPI_STATUS_IDLE;
        memset(cntx, 0, sizeof(bt_bipi_context));

        g_i_state = BIP_STATE_DISABLED;
        g_i_handle = NULL;
        g_i_opcode = BIP_OP_NONE;
        g_i_bchal = 0;
	
        //BIP_ERR("bipi_activate in error state!, return ok");
        //bipi_report_enable(BT_BIP_ERR_OK);
        //return;
    }

    if (g_i_opcode == BIP_OP_ACTIVATE)
    {
        BIP_ERR("bipi_activate in error state!, return failed");
        bipi_report_enable(BT_BIP_ERR_FAILED);
        return;
    }

    g_i_opcode = BIP_OP_ACTIVATE;

    act.callback = bipi_callback;
    ret = btmtk_bipi_activate(&act);
    BIP_DBG("bipi_activate, ret = %d", ret);
}


void bip_initiator_disable()
{
    S32 ret;

    if (!g_i_handle)
    {
        BIP_ERR("bipi_deactivate in error state, response OK!");
        bipi_report_disable(BT_BIP_ERR_OK);
        return;
    }
    if (g_i_opcode == BIP_OP_DEACTIVATE)
    {
        BIP_ERR("bipi_deactivate in error state, response failed!");
        bipi_report_disable(BT_BIP_ERR_FAILED);
        return;
    }
    g_i_opcode = BIP_OP_DEACTIVATE;
    ret = btmtk_bipi_deactivate(g_i_handle);
    BIP_DBG("bipi_deactivate(), ret = %d", ret);
}


void bip_initiator_disconnect(T_BDADDR BdAddr)
{
    S32 ret;


    switch(g_i_state)
    {
    case BIP_STATE_DISABLED:
    case BIP_STATE_ENABLED:
        BIP_ERR("--> BIP_I : disconnect in disconnect state!");
        bip_postevent(BIP_INITIATOR_DISCONNECT_SUCCESS, NULL, 0);
        return;

    case BIP_STATE_CONNECTING:
        // send abort 
        bip_postevent(BIP_INITIATOR_CANCEL_SUCCESS, NULL, 0);
        // continue 
    case BIP_STATE_CONNECTED:
        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        BIP_DBG("--> BIP_I : disconnect force, ret = %d ", ret);

    case BIP_STATE_DISCONNECTING:
        g_i_opcode = BIP_OP_DISCONNECT;
        break;

    default:
        g_i_opcode = BIP_OP_DISCONNECT;
        ret = btmtk_bipi_abort(g_i_handle);
        BIP_DBG("--> BIP_I : disconnect, start bipi_abort() ret = %d", ret);
        break;
    }

    g_i_state = BIP_STATE_DISCONNECTING;

    //mtk03036 20100807
    memset(&g_sending_object, 0, sizeof(T_BIP_OBJECT));
    g_sending_len = 0;
    //mtk03036 20100807 end

    BIP_DBG("--> BIP_I : disconnect, set state == DISCONNECTING");
}


void bip_initiator_getcapability_request(T_BDADDR BdAddr)
{
    S32 ret;

    if (g_i_opcode || g_i_state == BIP_STATE_DISABLED)
    {
        BIP_ERR("invoke get capabilitiy in error state!");
        bipi_report_capabilities(BT_BIP_ERR_FAILED,NULL, 0);
        return;
    }
    
    g_i_opcode = BIP_OP_GET_CAPA;

    if (g_i_state == BIP_STATE_CONNECTED)
    {
        ret = btmtk_bipi_get_capabilities(g_i_handle);
        BIP_DBG("invoke get capabilitiy ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);

        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTING);
        g_i_state = BIP_STATE_CONNECTING;

        BIP_DBG("invoke get capabilitiy, start to connect, ret = %d ", ret);
    }
    
}


void bip_initiator_pushimage(T_BDADDR BdAddr, T_BIP_OBJECT* BipObject ) 
{
    S32 ret;
    BIP_DBG("--> BIP_I : push_image, state = %d", g_i_state);


    if (g_i_opcode || g_i_state == BIP_STATE_DISABLED)
    {
        BIP_ERR("--> BIP_I : invoke put_img in error state!");
        bipi_report_push_img(BT_BIP_ERR_FAILED, 0, NULL);
        return;
    }

    g_i_opcode = BIP_OP_PUSH_IMG;

    g_sending_object = *BipObject;

    if (g_i_state == BIP_STATE_CONNECTED)
    {
        g_i_state = BIP_STATE_SENDING;
 
        ret = bipi_send_put_img_cmd(BipObject);

        BIP_DBG("--> BIP_I : invoke put_image ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);


        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTING);
        g_i_state = BIP_STATE_CONNECTING;
        BIP_DBG("--> BIP_I : invoke put_image, start to connect, ret = %d ", ret);
    }
}


void bip_initiator_pushthumbnail(T_BDADDR BdAddr, T_BIP_OBJECT *BipObject)
{
    S32 ret;
    BIP_DBG("--> BIP_I : push_thumbnail, state = %d", g_i_state);

    if (g_i_opcode || g_i_state == BIP_STATE_DISABLED)
    {
        BIP_ERR("--> BIP_I : invoke put_img in error state!");
        bipi_report_push_thumbnail(BT_BIP_ERR_FAILED, 0, NULL);
        return;
    }

    g_i_opcode = BIP_OP_PUSH_THUM;

    g_sending_object = *BipObject;

    if (g_i_state == BIP_STATE_CONNECTED)
    {
        g_i_state = BIP_STATE_SENDING; 
        ret = bipi_send_put_thumbnail_cmd(BipObject);
        BIP_DBG("--> BIP_I : invoke bipi_put_thumbnail ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);

        BIP_DBG("g_i_state : (%d) -> (%d)", g_i_state, BIP_STATE_CONNECTING);
        g_i_state = BIP_STATE_CONNECTING;
        BIP_DBG("--> BIP_I : invoke put_thumbnail, start to connect, ret = %d ", ret);
    }
}


void bip_initiator_auth_response(T_OBEX_AUTH *auth_reply)
{
    bt_bip_obex_auth_rsp rsp;
    S32 ret;

    if (!g_i_bchal)
    {
        BIP_ERR("--> BIP_I : invoke mbt_bip_initiator_auth_response in error case, cur op = %d", g_i_opcode);
        return;
    }

    if (auth_reply->bAuth == TRUE)
    {
        rsp.result = BT_BIP_ERR_OK;

        strcpy((char*)rsp.pwd, (char*)auth_reply->Passwd);
        rsp.pwd_len = strlen((const char*)rsp.pwd);
        rsp.uid_len = strlen(auth_reply->UserId);
        if (rsp.uid_len)
            strcpy((char*)rsp.uid, (char*)auth_reply->UserId);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    g_i_bchal = 0;

    ret = btmtk_bipi_obex_auth_rsp(g_i_handle, &rsp);
    BIP_DBG("--> BIP_I : bipi_obex_auth_rsp() ret = %d", ret);
}
//-------------------------------------------------------------------------------
//Initator API END---------------------------------------------------------------
//-------------------------------------------------------------------------------
/********************************************************************************
* Initator END  
*********************************************************************************/





/********************************************************************************
* Responder 
*********************************************************************************/

//-------------------------------------------------------------------------------
//Responder Report START---------------------------------------------------------
//-------------------------------------------------------------------------------
static void bipr_report_enable(S32 result)
{
    if (result == BT_BIP_ERR_OK)
    {
        if (g_r_state == BIP_STATE_DISABLED)
        {
            BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_ENABLED);
            g_r_state = BIP_STATE_ENABLED;
        }

        bip_postevent(BIP_RESPONDER_ENABLE_SUCCESS, NULL, 0);
    }
    else
    {
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_DISABLED);
        g_r_state = BIP_STATE_DISABLED;
        bip_postevent(BIP_RESPONDER_ENABLE_FAIL, NULL, 0);
    }
}


static void bipr_report_disable(S32 result)
{
    if (result == BT_BIP_ERR_OK)
    {
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_DISABLED);
        g_r_state = BIP_STATE_DISABLED;
        bip_postevent(BIP_RESPONDER_DISABLE_SUCCESS, NULL, 0);
    }
    else
    {
        bip_postevent(BIP_RESPONDER_DISABLE_FAIL, NULL, 0);
    }
}


static void bipr_report_disconnect(void)
{
    BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_ENABLED);
    g_r_state = BIP_STATE_ENABLED;

    switch(g_r_opcode)
    {
    case BIP_OP_GET_CAPA:
        BIP_ERR("<-- BIP_R : [DISCONNECT_IND], in case of GET_CAPA!");
        break;

    case BIP_OP_PUSH_IMG:
        bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_FAIL, NULL, 0);
        break;

    case BIP_OP_PUSH_THUM:
        bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, NULL, 0);
        break;

    case BIP_OP_DISCONNECT:
        bip_postevent(BIP_RESPONDER_DISCONNECT_SUCCESS, NULL, 0);
        break;

    default:
        break;
    }
    g_r_opcode = 0;
    g_r_bchal = 0;
}


static void bipr_report_progress(bt_bip_continue_ind* ind)
{
    S32 ret;


    g_r_state = BIP_STATE_RECEIVING;
    g_receiving_len += ind->data_len;

    S8* para[2];
    S8 receivingLen[MAX_DIGIT_SIZE];
    S8 objLen[MAX_DIGIT_SIZE];
    sprintf(receivingLen, "%d", g_receiving_len);
    para[0] = receivingLen;


    BIP_DBG("para[0]: ReceivingLen -> (%s)", receivingLen);


    if (ind->obj_len) {
        BIP_DBG("<-- BIP_R : [CONTINUE_IND], received %lu%c", (U32)((g_receiving_len * 100)/ind->obj_len), '%');
        sprintf(objLen, "%lu", ind->obj_len);
        para[1] = objLen;
    }
    else {
        BIP_DBG("<-- BIP_R : [CONTINUE_IND], received : %d bytes", g_receiving_len);
        sprintf(objLen, "%d", 0);
        para[1] = objLen;
    }


    bip_postevent(BIP_RESPONDER_PROGRESS, para, 2);
    ret = btmtk_bipr_continue(g_r_handle, 1);
    BIP_DBG(" --> BIP_R : bipr_continue() ret = %d", ret);
}


U32 bipr_check_disconnect(bt_bipr_para* para, U32 opcode)
{
    S32 ret;

    if (g_r_opcode == BIP_OP_DISCONNECT)
    {

        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_DISCONNECTING);
        g_r_state = BIP_STATE_DISCONNECTING;

        if (para->status == BT_BIPR_OPS_COMPLETE && para->data.result == BT_BIP_ERR_OK)
        {
            if (opcode == BT_BIP_OP_PUT_IMG)
                bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_SUCCESS, NULL, 0);
            else if (opcode == BT_BIP_OP_PUT_LINKED_THUMBNAIL)
                bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_SUCCESS, NULL, 0);
            else if (opcode == BT_BIP_OP_GET_CAPABILITIES)
                bip_postevent(BIP_RESPONDER_CAPABILITY_RES_SUCCESS, NULL, 0);
        }
        else
        {
            if (opcode == BT_BIP_OP_PUT_IMG)
                bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_FAIL, NULL, 0);
            else if (opcode == BT_BIP_OP_PUT_LINKED_THUMBNAIL)
                bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, NULL, 0);
            else if (opcode == BT_BIP_OP_GET_CAPABILITIES)
                bip_postevent(BIP_RESPONDER_CAPABILITY_RES_SUCCESS, NULL, 0);
        }
        BIP_DBG("--> BIP post success event : BIPEVT_BIP_RESPONDER_CANCEL event");

        ret = btmtk_bipr_disconnect(g_r_handle, 1);
        BIP_DBG("--> BIP_R : disconnect force in case of check_disconnect(), ret = %d ", ret);
        return 1;
    }
    return 0;
}


static void bipr_report_authorize_ind(bt_bipr_para* para)
{
    BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_CONNECTING);
    g_r_state = BIP_STATE_CONNECTING;


    S8* info[2];
    S8 remoteDevName[MAX_FILE_NAME_LEN];
    sprintf(remoteDevName, "%s", para->data.auth_ind.dev_name);

    U8 dest[6] = {0};
    dest[5] = (U8) (para->data.auth_ind.addr.lap & 0x000000FF);
    dest[4] = (U8) ((para->data.auth_ind.addr.lap & 0x0000FF00) >> 8);
    dest[3] = (U8) ((para->data.auth_ind.addr.lap & 0x00FF0000) >> 16);
    dest[2] = para->data.auth_ind.addr.uap;
    dest[1] = (U8) (para->data.auth_ind.addr.nap & 0x00FF);
    dest[0] = (U8) ((para->data.auth_ind.addr.nap & 0xFF00) >> 8);
    S8 remoteDevAddr[MAX_FILE_NAME_LEN];
    sprintf(remoteDevAddr, "%02x:%02x:%02x:%02x:%02x:%02x", dest[0], dest[1], dest[2], dest[3], dest[4], dest[5]);

    info[0] = remoteDevName;
    info[1] = remoteDevAddr;

    BIP_DBG("info[0]: remoteDevName -> (%s)", remoteDevName);
    BIP_DBG("info[1]: remoteDevAddr -> (%s)", remoteDevAddr);

    BIP_DBG("<-- BIP_R : receive authorize indication, set state to CONNECTING");
    bip_postevent(BIP_RESPONDER_AUTH_REQ, info, 2);
}


static void bipr_report_obex_auth(bt_bipr_para* para)
{
    S8* info[1];
    S8 needUid[2];

    if (para->data.obauth_ind.option)
    {
        sprintf(needUid, "%s", "1");
    }
    else
    {
        sprintf(needUid, "%s", "0");
    }
    info[0] = needUid;
    BIP_DBG(" <-- BIP_R : [OBEX_AUTH], option = %s.", needUid);

    g_r_bchal = 1;

    bip_postevent(BIP_RESPONDER_OBEX_AUTHREQ, info, 1);
}
//-------------------------------------------------------------------------------
//Responder Report END-----------------------------------------------------------
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//Responder Request START--------------------------------------------------------
//-------------------------------------------------------------------------------
static void bipr_rsp_connect_ind(bt_bipr_para* para)
{
    S32 ret;

    if (para->status == BT_BIPR_OPS_INDICATING)
    {
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_CONNECTING);
        g_r_state = BIP_STATE_CONNECTING;


        ret = btmtk_bipr_connect_rsp(g_r_handle, 1);
        BIP_DBG("--> BIP_R : bipr_connect_rsp(), ret = %d.", ret);
    }
    else
    {
        BIP_DBG("--> BIP_R : [CONNECT_IND], connection established!");
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_CONNECTED);
        g_r_state = BIP_STATE_CONNECTED;
    }
}


static void bipr_rsp_get_capa_ind(bt_bipr_para* para)
{
    S32 ret;

    if (para->status == BT_BIPR_OPS_INDICATING)
    {
        g_r_opcode = BIP_OP_GET_CAPA;

        bip_postevent(BIP_RESPONDER_GET_CAPABILITY_REQ, NULL, 0);
    }
    else if (para->status == BT_BIPR_OPS_COMPLETE)
    {
        bip_postevent(BIP_RESPONDER_CAPABILITY_RES_SUCCESS, NULL, 0);
        if (g_r_opcode == BIP_OP_GET_CAPA)
            g_r_opcode = 0;
        else if (g_r_opcode == BIP_OP_DISCONNECT)
        {
            ret = btmtk_bipr_disconnect(g_r_handle, 1);
            BIP_DBG("--> BIP_R : in case of GetCapa Complete, bipr_disconnect(), ret = %d", ret);
        }
    }
}


static void bipr_rsp_put_img_ind(bt_bipr_para* para)
{

    switch (para->status)
    {
    case BT_BIPR_OPS_INDICATING:
        g_r_opcode = BIP_OP_PUSH_IMG;

        //mtk03036 20100807
        memset(&g_receiving_object, 0, sizeof(T_BIP_OBJECT));
        g_receiving_len = 0;
        //mtk03036 20100807 end

        g_receiving_object.ObjectSize = para->data.put_img.img_desc.size;
        BIP_DBG("g_receiving_object.ObjectSize : -> (%lu)", g_receiving_object.ObjectSize);

        strcpy(g_receiving_object.ImageDesc.Version, "1.0");
        BIP_DBG("g_receiving_object.ImageDesc.Version : -> (%s)", "1.0");

        strcpy(g_receiving_object.ImageDesc.Encoding, bip_get_encoding_str(para->data.put_img.img_desc.encoding));
        BIP_DBG("g_receiving_object.ImageDesc.Encoding : -> (%s)", g_receiving_object.ImageDesc.Encoding);

        g_receiving_object.ImageDesc.Size = para->data.put_img.img_desc.size;
        BIP_DBG("g_receiving_object.ImageDesc.Size : -> (%lu)", g_receiving_object.ImageDesc.Size);

        g_receiving_object.ImageDesc.Transform = bip_get_mtk_trans_val(para->data.put_img.img_desc.transformation);
        BIP_DBG("g_receiving_object.ImageDesc.Transform : -> (%d)", g_receiving_object.ImageDesc.Transform);

        g_receiving_object.ImageDesc.Width = para->data.put_img.img_desc.pixel.width;
        BIP_DBG("g_receiving_object.ImageDesc.Width : -> (%lu)", g_receiving_object.ImageDesc.Width);

        g_receiving_object.ImageDesc.Height= para->data.put_img.img_desc.pixel.height;
        BIP_DBG("g_receiving_object.ImageDesc.Height : -> (%lu)", g_receiving_object.ImageDesc.Height);

        g_receiving_object.ImageDesc.Width2 = para->data.put_img.img_desc.pixel.width2;
        BIP_DBG("g_receiving_object.ImageDesc.Width2 : -> (%lu)", g_receiving_object.ImageDesc.Width2);

        g_receiving_object.ImageDesc.Height2 = para->data.put_img.img_desc.pixel.height2;
        BIP_DBG("g_receiving_object.ImageDesc.Height2 : -> (%lu)", g_receiving_object.ImageDesc.Height2);

        wchar_net_to_local(para->data.put_img.img_name);
        ext_chset_ucs2_to_utf8_string((kal_uint8*)g_receiving_object.FileName, MAX_FILE_NAME_LEN, (U8*)para->data.put_img.img_name);
        BIP_DBG("g_receiving_object.FileName : -> (%s)", g_receiving_object.FileName);


        S8* info[2];
        S8 fileName[MAX_FILE_NAME_LEN];
        S8 fileSize[MAX_DIGIT_SIZE];
        
        sprintf(fileName, "%s", g_receiving_object.FileName);
        info[0] = fileName;
        BIP_DBG("info[0]: fileName -> (%s)", fileName);

        sprintf(fileSize, "%lu", g_receiving_object.ObjectSize);
        info[1] = fileSize;
        BIP_DBG("info[1]: fileSize -> (%s)", fileSize);

        bip_postevent(BIP_RESPONDER_ACCESS_REQ, info, 2);
        break;

    case BT_BIPR_OPS_RESPONDING:
        BIP_DBG("[BTMTK][BIP] <-- BIP_R : [PUT_IMG] : status = %s, image size = %lu, current size = %lu",
            status_str[para->status], para->data.cnt_ind.obj_len, para->data.cnt_ind.data_len);
        bipr_report_progress(&para->data.cnt_ind);
        break;

    case BT_BIPR_OPS_COMPLETE:
        BIP_DBG("<-- BIP_R : [PUT_IMG] : Complete, %d", para->data.result);
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_CONNECTED);
        g_r_state = BIP_STATE_CONNECTED;

        if (para->data.result == BT_BIP_ERR_OK)
            bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_SUCCESS, NULL, 0);
        else
        {
            bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_FAIL, NULL, 0);
        }
        g_r_opcode = 0;
        break;
    }
}

//20120627 added by mtk71255 to rename filename.
void bip_responder_obj_rename(char *newName) {

	memcpy(g_receiving_object.FileName, newName, MAX_FILE_NAME_LEN);
	BIP_DBG("mtk71255 add for object renaming :%s", g_receiving_object.FileName);
}

static void bipr_rsp_put_thumbnail_ind(bt_bipr_para* para)
{
    S32 ret;
    char* p;
    bt_bip_put_linked_thum_rsp rsp;


    switch (para->status)
    {
    case BT_BIPR_OPS_INDICATING:
        g_r_opcode = BIP_OP_PUSH_THUM;

        bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_START, NULL, 0);


        p = g_receiving_object.DirName + strlen(g_receiving_object.DirName);
        if (p > g_receiving_object.DirName)
        {
            rsp.result = BT_BIP_ERR_OK;
#if defined(BTMTK_ON_LINUX)
            *p = '/';
#else
            *p = '\\';
#endif
            if (strlen(g_receiving_object.FileName))
            {
                strcpy(p + 1, g_receiving_object.FileName);
                sprintf(p + 1, "%s.thum.jpg", g_receiving_object.FileName);
            }
            else
            {
                strcpy(p + 1, "1234567.thum.jpg");
            }
#if defined(BTMTK_ON_WISE)
            wsConvertFileNameToNativeFormat(rsp.img_path, sdc->ReceivingFile.DirName);
#else
            ext_chset_utf8_to_ucs2_string((U8*)rsp.img_path, 520,  (const U8 *)g_receiving_object.DirName);
#endif
            *p = 0;
        }
        else
        {
            rsp.result = BT_BIP_ERR_FAILED;
        }
        ret = btmtk_bipr_put_linked_thumbnail(g_r_handle, &rsp);
        BIP_DBG("--> BIP_R : bipr_put_linked_thumbnail(), ret = %d.", ret);
        break;

    case BT_BIPR_OPS_RESPONDING:
        BIP_DBG("<-- BIP_R : [PUT_IMG_THUMNAIL] : status = %s, image size = %lu, current size = %lu",
            status_str[para->status], para->data.cnt_ind.obj_len, para->data.cnt_ind.data_len);
        bipr_report_progress(&para->data.cnt_ind);
        break;

    case BT_BIPR_OPS_COMPLETE:
        BIP_DBG("<-- BIP_R : [PUT_IMG_THUMBNAIL] : Complete, %d", para->data.result);
        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_CONNECTED);
        g_r_state = BIP_STATE_CONNECTED;

        if (para->data.result == BT_BIP_ERR_OK)
            bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_SUCCESS, NULL, 0);
        else
        {
            bip_postevent(BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, NULL, 0);
        }
        g_r_opcode = 0;
        break;
    }
}



//-------------------------------------------------------------------------------
//Responder Request END----------------------------------------------------------
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//Responder Callback START-------------------------------------------------------
//-------------------------------------------------------------------------------

static void bipr_callback(void* p, bt_bipr_para* para)
{
    switch(para->opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        BIP_DBG("<-- BIP_R : [ACTIVATE] : result = %d", para->data.result);
        if (para->data.result == BT_BIP_ERR_OK)
        {
            g_r_handle = para->handle;
        }
        //memset(&g_receiving_object, 0, sizeof(T_BIP_OBJECT));
        //g_receiving_len = 0;

        g_r_opcode = 0;
        bipr_report_enable(para->data.result);
        break;

    case BT_BIP_OP_DEACTIVATE:
        BIP_DBG("<-- BIP_R : [DEACTIVATE] : result = %d", para->data.result);
        if (para->data.result == BT_BIP_ERR_OK)
            g_r_handle = 0;
        g_r_opcode = 0;
        bipr_report_disable(para->data.result);
        break;

    case BT_BIP_OP_AUTHORIZE:
        BIP_DBG("<-- BIP_R : [AUTHORIZE] : dev name : %s",  para->data.auth_ind.dev_name);
        BIP_DBG("<-- BIP_R : [AUTHORIZE] : addr: lap = %lx, uap = %x, nap = %x",
            para->data.auth_ind.addr.lap, para->data.auth_ind.addr.uap, para->data.auth_ind.addr.nap);
        bipr_report_authorize_ind(para);
        break;

    case BT_BIP_OP_CONNECT:
        BIP_DBG("<-- BIP_R : [CONNECT] : status = %s, result =%d", status_str[para->status], para->data.result);
        bipr_rsp_connect_ind(para);
        break;

    case BT_BIP_OP_OBEX_AUTH:
        BIP_DBG("<-- BIP_R : [OBEX_AUTH] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (para->status == BT_BIPR_OPS_INDICATING)
        {
            bipr_report_obex_auth(para);
        }
        break;

    case BT_BIP_OP_GET_CAPABILITIES:
        BIP_DBG("<-- BIP_R : [GET_CAPABILITIES] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_GET_CAPABILITIES))
            bipr_rsp_get_capa_ind(para);
        break;

    case BT_BIP_OP_PUT_IMG:
        BIP_DBG("<-- BIP_R : [PUT_IMAGE] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_PUT_IMG))
        {
            bipr_rsp_put_img_ind(para);
        }
        break;

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        BIP_DBG("<-- BIP_R : [PUT_THUMBNAIL] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_PUT_IMG))
        {
            bipr_rsp_put_thumbnail_ind(para);
        }
        break;

    case BT_BIP_OP_GET_IMG:
        break;

    case BT_BIP_OP_DISCONNECT:
        BIP_DBG("<-- BIP_R : [DISCONNECT] : status = %s.", status_str[para->status]);
        bipr_report_disconnect();
        break;

    case BT_BIP_OP_CONTINUE:
        BIP_DBG("<-- BIP_R : [CONTINUE_IND] : status = %s", status_str[para->status]);
        break;

    case BT_BIP_OP_ABORT:
        BIP_DBG("<-- BIP_R : [R_ABORT] status = %s, result = %d", status_str[para->status], para->data.result);

        break;
    }
}



//-------------------------------------------------------------------------------
//Responder Callback END---------------------------------------------------------
//-------------------------------------------------------------------------------






//-------------------------------------------------------------------------------
//Responder API START------------------------------------------------------------
//-------------------------------------------------------------------------------
void bip_responder_enable(const char* rootPath)
{
    S32 ret;
    bt_bipr_active_info inf;

//mtk03036 20100826 
    memcpy(g_root_path, rootPath, MAX_FILE_NAME_LEN);
    BIP_DBG(" --> BIP_R : bipr_activate(), ret = %s", g_root_path);
//mtk03036 20100826 end

    if (g_r_handle)
    {   
        bt_bipr_context* cntx = (bt_bipr_context*)g_r_handle;
        cntx->status = BIPR_STATUS_IDLE;
        memset(cntx, 0, sizeof(bt_bipr_context));

        g_r_state = BIP_STATE_DISABLED;
        g_r_handle = NULL;
        g_r_opcode = BIP_OP_NONE;
        g_r_bchal = 0;
	
        //BIP_ERR("--> BIP_R : inovke enable in error state, repose ok!");
        //bipr_report_enable(BT_BIP_ERR_OK);
        //return;
    }

    if (g_r_opcode == BIP_OP_ACTIVATE)
    {
        BIP_ERR("--> BIP_R : inovke enable in error state, repose failed!");
        bipr_report_enable(BT_BIP_ERR_FAILED);
        return;
    }

    g_r_opcode = BIP_OP_ACTIVATE;

    inf.callback = bipr_callback;
    inf.service = BT_BIP_SRV_IMG_PUSH | BT_BIP_SRV_IMG_PULL;
    ret = btmtk_bipr_activate(&inf);
    BIP_DBG(" --> BIP_R : bipr_activate(), ret = %d", ret);
}


void bip_responder_disable(void)
{
    S32 ret;

    if (!g_r_handle)
    {
        BIP_ERR("--> BIP_R : inovke disenable in error state, repose ok!");
        bipr_report_disable(BT_BIP_ERR_OK);
        return;
    }

    if (g_r_opcode == BIP_OP_DEACTIVATE)
    {
        BIP_ERR("--> BIP_R : inovke disenable in error state, repose failed!");
        bipr_report_disable(BT_BIP_ERR_FAILED);
        return;
    }

    g_r_opcode = BIP_OP_DEACTIVATE;

    ret = btmtk_bipr_deactivate(g_r_handle);
    BIP_DBG(" --> BIP_R : bipr_deactivate(), ret = %d", ret);
}

//20100817 mtk03036
void bip_responder_authorize_response(int auth)
{
    S32 ret;

    if (!g_r_handle)
    {
        BIP_ERR("--> BIP_R : inovke authorize in error state");
        return;
    }

    if ( g_r_state != BIP_STATE_CONNECTING )
    {
        BIP_ERR("--> BIP_R : inovke authorize in error state");
        return;
    }
    ret = btmtk_bipr_authorize(g_r_handle, auth);
    BIP_DBG(" --> BIP_R : bipr_authorize(), ret = %d", ret);
}
//20100817 mtk03036 


void bip_responder_disconnect(void)
{
    S32 ret;
    BIP_DBG("--> BIP_R : disconnect, state = %d", g_r_state);

    switch(g_r_state)
    {
    case BIP_STATE_DISABLED:
    case BIP_STATE_ENABLED:
        BIP_ERR("--> BIP_R : disconnect in disconnect state!");
        bip_postevent(BIP_RESPONDER_DISCONNECT_SUCCESS, NULL, 0);
        return;

    case BIP_STATE_CONNECTING:
    case BIP_STATE_CONNECTED:
        if (g_r_opcode != BIP_OP_GET_CAPA) 
        {
            ret = btmtk_bipr_disconnect(g_r_handle, 1);
            BIP_DBG("--> BIP_R : disconnect force, ret = %d ", ret);
        }

    case BIP_STATE_DISCONNECTING:
        g_r_opcode = BIP_OP_DISCONNECT;
        break;

    default:
        g_r_opcode = BIP_OP_DISCONNECT;
        ret = btmtk_bipr_abort(g_r_handle);
        BIP_DBG("--> BIP_R : disconnect, start bipi_abort() ret = %d", ret);
        break;
    }
    g_r_state = BIP_STATE_DISCONNECTING;
    BIP_DBG("--> BIP_R : disconnect, set state == DISCONNECTING");
}


void bip_responder_getcapability_response(int Reply, T_BIP_IMAGING_CAPABILITY *ImagingCapability)
{
    bt_bip_capabilities_rsp rsp;
    S32 ret;
    S32 f;

    memset(&rsp, 0, sizeof(bt_bip_capabilities_rsp));


    if (g_r_opcode != BIP_OP_GET_CAPA)
    {
        BIP_ERR("--> BIP_R : invoke bip_responder_getcapability_response() in error case, current opcode = %d", g_r_opcode);
        return;
    }
    if (Reply == ALLOW_RES)
    {
        f = 0;

        rsp.result = BT_BIP_ERR_OK;
        rsp.preferred_format.encoding = bip_get_encoding_val(ImagingCapability->PreferFormat.Encoding);
        rsp.preferred_format.size = (U32)ImagingCapability->PreferFormat.Size;
        rsp.preferred_format.transformation = bip_get_mtk_trans_val(ImagingCapability->PreferFormat.Transform);
        rsp.preferred_format.pixel.height = ImagingCapability->PreferFormat.Height;
        rsp.preferred_format.pixel.width = ImagingCapability->PreferFormat.Width;
        rsp.preferred_format.pixel.height2 = ImagingCapability->PreferFormat.Height2;
        rsp.preferred_format.pixel.width2 = ImagingCapability->PreferFormat.Width2;

        if (rsp.preferred_format.encoding == BT_BIP_TYPE_JPEG)
        {
            f = bip_check_pixel(160, 120, &rsp.preferred_format.pixel);
        }

        for (ret = 0; ret < BT_BIP_MAX_IMAGE_FORMATES && (U32)ret < ImagingCapability->NumImageFormats; ret++)
        {
            rsp.supported_formats[ret].encoding  = bip_get_encoding_val(ImagingCapability->ImageFormats[ret].Encoding);
            rsp.supported_formats[ret].size = (U32)ImagingCapability->ImageFormats[ret].Size;
            rsp.supported_formats[ret].pixel.width = ImagingCapability->ImageFormats[ret].Width;
            rsp.supported_formats[ret].pixel.height = ImagingCapability->ImageFormats[ret].Height;
            rsp.supported_formats[ret].pixel.width2 = ImagingCapability->ImageFormats[ret].Width2;
            rsp.supported_formats[ret].pixel.height2 = ImagingCapability->ImageFormats[ret].Height2;

            if (!f && rsp.supported_formats[ret].encoding == BT_BIP_TYPE_JPEG)
                f = bip_check_pixel(160, 120, &rsp.supported_formats[ret].pixel);
        }

        if (!f)
        {
            if (ret == BT_BIP_MAX_IMAGE_FORMATES)
                ret--;

            rsp.supported_formats[ret].encoding  = BT_BIP_TYPE_JPEG;
            rsp.supported_formats[ret].size = 65535;
            rsp.supported_formats[ret].pixel.width = 160;
            rsp.supported_formats[ret].pixel.height = 120;
            rsp.supported_formats[ret].pixel.width2 = 0;
            rsp.supported_formats[ret].pixel.height2 = 0;

        }

    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }
    ret = btmtk_bipr_get_capabilities(g_r_handle, &rsp);
    BIP_DBG("--> BIP_R : bipr_get_capabilities(), ret = %d.", ret);
}


void bip_responder_access_response(int Reply, int Thumbnail, const char* rootPath)
{
    S32 ret;
    char* p;
    T_BIP_OBJECT* pobj;
    bt_bip_put_img_rsp rsp;

    if (g_r_opcode != BIP_OP_PUSH_IMG)
    {
        BIP_ERR("--> BIP_R : invoke mbt_bip_responder_access_response() in error case, current opcode = %d", g_r_opcode);
        bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_FAIL, NULL, 0);
        return;
    }

    if (ALLOW_RES == Reply)
    {
        if ( 1 == Thumbnail )
            rsp.result = BT_BIP_ERR_PARTIAL_CONTENT;
        else
            rsp.result = BT_BIP_ERR_OK;

        pobj = &g_receiving_object;
		
		memcpy(g_root_path, rootPath, MAX_FILE_NAME_LEN);
		BIP_DBG(" --> BIP_R : bipr_activate(), ret = %s", g_root_path);

        //strcpy(pobj->DirName, "/sdcard");
        strcpy(pobj->DirName, g_root_path);
        p = pobj->DirName + strlen(pobj->DirName);

#if defined(BTMTK_ON_LINUX)
        *p = '/';
#else
        *p = '\\';
#endif

        strcpy(p + 1, pobj->FileName);

#if defined(BTMTK_ON_WISE)
        wsConvertFileNameToNativeFormat(rsp.img_path, pobj->DirName);
#else
        ext_chset_utf8_to_ucs2_string((U8*)rsp.img_path, 520,  (const U8 *)pobj->DirName);
#endif
        *p = 0;
        strcpy((char*)rsp.handle, "1234567"); // just set it 

        BIP_DBG("g_r_state : (%d) -> (%d)", g_r_state, BIP_STATE_RECEIVING);
        g_r_state = BIP_STATE_RECEIVING;

        bip_postevent(BIP_RESPONDER_IMAGE_RECEIVE_START, NULL, 0);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    ret = btmtk_bipr_put_image(g_r_handle, &rsp);
    BIP_DBG("--> BIP_R : bipr_put_image(), option = %d, ret = %d", Reply, ret);
}


void bip_responder_auth_response(T_OBEX_AUTH *auth_reply)
{
    S32 ret;
    bt_bip_obex_auth_rsp rsp;

    if (!g_r_bchal)
    {
        BIP_ERR("--> BIP_R : invoke mbt_bip_responder_auth_response() in error case!, curr op = %d", g_r_opcode);
        return;
    }
    if (auth_reply->bAuth && auth_reply->UserId[0])
    {
        rsp.result = BT_BIP_ERR_OK;

        strcpy((char*)rsp.pwd, auth_reply->UserId);
        rsp.pwd_len = strlen(auth_reply->UserId);

        rsp.uid_len = strlen(auth_reply->Passwd);
        if (rsp.uid_len)
            strcpy((char*)rsp.uid, auth_reply->Passwd);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    g_r_bchal = 0;

    ret = btmtk_bipr_obex_auth_rsp(g_r_handle, &rsp);
    BIP_DBG("--> BIP_R : bipr_obex_auth_rsp(), ret = %d", ret);
}


//-------------------------------------------------------------------------------
//Responder API END--------------------------------------------------------------
//-------------------------------------------------------------------------------



/********************************************************************************
* Responder end
*********************************************************************************/


