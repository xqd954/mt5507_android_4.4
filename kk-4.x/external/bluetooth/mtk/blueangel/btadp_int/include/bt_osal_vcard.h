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

#ifndef __BT_OSAL_VCARD_H__
#define __BT_OSAL_VCARD_H__

/* error code */
#define BTMTK_VCD_ERR_OK        0   /* success */
#define BTMTK_VCD_ERR_FAILED    -1  /* common error */
#define BTMTK_VCD_ERR_IO        -2  /* IO error, such as FS error */
#define BTMTK_VCD_ERR_VERSION   -3  /* version error */
#define BTMTK_VCD_ERR_OVERSIZE  -4  /* size over limiation */
#define BTMTK_VCD_ERR_HEADER    -5  /* invalid vCard header */
#define BTMTK_VCD_ERR_PARAM     -6  /* parameter is invalid */

/* PHB field type */
#define SRV_PHB_ENTRY_FIELD_NAME       0x00001      /* phonebook name field */
#define SRV_PHB_ENTRY_FIELD_NUMBER     0x00002      /* phonebook mobile number field */
#define SRV_PHB_ENTRY_FIELD_HOME       0x00004      /* phonebook home number field */
#define SRV_PHB_ENTRY_FIELD_OFFICE     0x00008      /* phonebook office number field */
#define SRV_PHB_ENTRY_FIELD_FAX        0x00010      /* phonebook fax number field */
#define SRV_PHB_ENTRY_FIELD_EMAIL1     0x00020      /* phonebook email1 address field */
#define SRV_PHB_ENTRY_FIELD_EMAIL2     0x00040      /* phonebook email2 address field */
#define SRV_PHB_ENTRY_FIELD_COMPANY    0x00080      /* phonebook company name field */
#define SRV_PHB_ENTRY_FIELD_ADDRESS    0x00100     /* phonebook address field */
#define SRV_PHB_ENTRY_FIELD_VOIP       0x00200     /* phonebook voip number field */
#define SRV_PHB_ENTRY_FIELD_IMPS       0x00400     /* phonebook imps field */
#define SRV_PHB_ENTRY_FIELD_TITLE      0x00800     /* phonebook title field */
#define SRV_PHB_ENTRY_FIELD_URL        0x01000    /* phonebook url field */
#define SRV_PHB_ENTRY_FIELD_NOTE       0x02000    /* phonebook note field */
#define SRV_PHB_ENTRY_FIELD_GROUP      0x04000    /* phonebook caller group field */
#define SRV_PHB_ENTRY_FIELD_BDAY       0x08000    /* phonebook birthday field */
#define SRV_PHB_ENTRY_FIELD_RING       0x10000   /* phonebook caller ringtione field */
#define SRV_PHB_ENTRY_FIELD_PIC        0x20000   /* phonebook caller video field */
#define SRV_PHB_ENTRY_FIELD_VIDEO      0x40000   /* phonebook caller pic field */
#define SRV_PHB_ENTRY_FIELD_DEF        0x80000   /* phonebook default number field */
#define SRV_PHB_ENTRY_FIELD_ALL        0xFFFFF   /* phonebook default number field */


#define BTMTK_VCARD_MAX_NAME_LEN             40
#define BTMTK_VCARD_MAX_NUMBER_LEN           44
#define BTMTK_VCARD_MAX_EMAIL_LEN            60
#define BTMTK_VCARD_MAX_TITLE_LEN            20
#define BTMTK_VCARD_MAX_COMPANY_LEN          20
#define BTMTK_VCARD_MAX_URL_LEN              128
#define BTMTK_VCARD_MAX_ADDRESS_LEN          25
#define BTMTK_VCARD_MAX_NOTE_LEN             30
#define BTMTK_VCARD_MAX_BIRTHDAY_LEN         12
#define BTMTK_VCARD_MAX_FILEPATH_LEN         256

/* utf8 encoding */
typedef struct
{
    U8 first_name[BTMTK_VCARD_MAX_NAME_LEN + 1];                /* family name field */
    U8 second_name[BTMTK_VCARD_MAX_NAME_LEN + 1];               /* given name field */
    U8 third_name[BTMTK_VCARD_MAX_NAME_LEN + 1];                /* Additional Name */
    U8 fourth_name[BTMTK_VCARD_MAX_NAME_LEN + 1];               /* Name Prefix */
    U8 fifth_name[BTMTK_VCARD_MAX_NAME_LEN + 1];                /* Name Suffix */
    
    U8 cell_num[BTMTK_VCARD_MAX_NUMBER_LEN + 1];                /*  cellular number */
    U8 home_num[BTMTK_VCARD_MAX_NUMBER_LEN + 1];                /*  home number field */
    U8 office_num[BTMTK_VCARD_MAX_NUMBER_LEN + 1];              /*  offfice number field */
    U8 fax_num[BTMTK_VCARD_MAX_NUMBER_LEN + 1];                 /*  fax number field */
    /* U8 pref_num[BTMTK_VCARD_MAX_NUMBER_LEN + 1]; */          /* preferred number */
    
    U8 email[BTMTK_VCARD_MAX_EMAIL_LEN + 1];                    /*  email address field */
    U8 email2[BTMTK_VCARD_MAX_EMAIL_LEN + 1];                   /*  email address2 field */
    
    U8 company[BTMTK_VCARD_MAX_COMPANY_LEN + 1];                /*  company name field */
    
    U8 birthday[BTMTK_VCARD_MAX_BIRTHDAY_LEN + 1];              /* birthday year field */
    
    U8 url[BTMTK_VCARD_MAX_URL_LEN + 1];                        /* url field */
    
    U8 title[BTMTK_VCARD_MAX_TITLE_LEN + 1];                    /* title field */
    
    U8 note[BTMTK_VCARD_MAX_NOTE_LEN + 1];                      /* note field */
    
    /* address info */
    U8 postbox[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];                /* post office address field */
    U8 extension[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];              /* extension field */
    U8 street[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];                 /* street field */
    U8 city[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];                   /* city field */
    U8 state[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];                  /* state field */
    U8 postcode[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];             /* postalcode field */
    U8 country[BTMTK_VCARD_MAX_ADDRESS_LEN + 1];	            /* country field */
    
    /* picture file name with path */
    U16 picture_filename[BTMTK_VCARD_MAX_FILEPATH_LEN];      /* picture path */
    
} btmtk_vcard_data_struct;


S32 btmtk_vcard_parse_file_to_struct(U16* filename, btmtk_vcard_data_struct* vcard_struct);

#endif
