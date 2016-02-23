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

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"

#include "bt_common.h"

#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"
#include "bt_osal_qp.h"
#include "bt_osal_vcard.h"


#define __MMI_PHB_INFO_FIELD__ /* here we define __MMI_PHB_INFO_FIELD__ always */

/* vcard field enum */
typedef enum
{
    VCARD_FIELD_BEGIN,      /* vcard begin feild */
    VCARD_FIELD_VERSION,    /* vcard version feild */
    VCARD_FIELD_FN,         /* vcard FN feild */
    VCARD_FIELD_N,          /* vcard name feild */
    VCARD_FIELD_TEL,        /* vcard tel feild */
    VCARD_FIELD_EMAIL,      /* vcard eamil feild */
    VCARD_FIELD_ORG,        /* vcard org feild */
    VCARD_FIELD_BDAY,       /* vcard birthday feild */
    VCARD_FIELD_TITLE,      /* vcard title feild */
    VCARD_FIELD_URL,        /* vcard url feild */
    VCARD_FIELD_NOTE,       /* vcard note feild */
    VCARD_FIELD_ADR,        /* vcard address feild */
    VCARD_FIELD_PHOTO,      /* vcard photo feild */
    VCARD_FIELD_REV,        /* vcard rev feild */
    VCARD_FIELD_LABEL,      /* vcard label feild */
    VCARD_FIELD_NICKNAME,   /* vcard nike name feild */
    VCARD_FIELD_MAILER,     /* vcard mailer feild */
    VCARD_FIELD_TZ,         /* vcard tz feild */
    VCARD_FIELD_GEO,        /* vcard geo feild */
    VCARD_FIELD_ROLE,       /* vcard role feild */
    VCARD_FIELD_LOGO,       /* vcard logo feild */
    VCARD_FIELD_AGENT,      /* vcard agent feild */
    VCARD_FIELD_CATEGORIES, /* vcard categories feild */
    VCARD_FIELD_PRODID,     /* vcard prodid feild */
    VCARD_FIELD_SORT_STRING,/* vcard sort string feild */
    VCARD_FIELD_SOUND,      /* vcard sound feild */
    VCARD_FIELD_UID,        /* vcard UID feild */
    VCARD_FIELD_CLASS,      /* vcard class feild */
    VCARD_FIELD_KEY,        /* vcard key feild */
    VCARD_FIELD_X,          /* vcard X feild */
    VCARD_FIELD_END,        /* vcard end feild */
    VCARD_FIELD_TOTAL       /* enum total */
} VCARD_FILED_ENUM;

typedef enum
{
    VCARD_NAME_FIRST,                 /* vcard name field - family name */
    VCARD_NAME_SECOND,                /* vcard name field - given name */
    VCARD_NAME_THIRD,                 /* vcard name field - additional name */
    VCARD_NAME_FORTH,                 /* vcard name field - name prefix */
    MMI_VCARD_NAME_FIFTH,             /* vcard name field - name suffix */
    VCARD_NAME_ENUM_TOTAL             /* vcard name enum total */
} VCARD_NAME_ENUM;

typedef enum
{
    VCARD_ADDRESS_POBOX,
    VCARD_ADDRESS_EXTENSION,
    VCARD_ADDRESS_STREET,
    VCARD_ADDRESS_CITY,
    VCARD_ADDRESS_STATE,
    VCARD_ADDRESS_POSTALCODE,
    VCARD_ADDRESS_COUNTRY,
    VCARD_ADDRESS_ENUM_TOTAL
} VCARD_ADDRESS_ENUM;

/*
typedef enum
{
    VCARD_VERSION_21,
    VCARD_VERSION_30
} VCARD_VERSION_ENUM;
*/
typedef enum
{
    VOBJ_PARA_LANGUAGE,               /* Not used */
    VOBJ_PARA_CHARSET,                /* Charset for parsing */
    VOBJ_PARA_ENCODING,               /* Encoding for parsing */
    VOBJ_PARA_TOTAL
} vobj_desc_para_type_enum;

typedef enum
{
    VOBJ_EN_7BIT = 0,                 /* 7-bit encoding */
    VOBJ_EN_8BIT,                     /* 8-bit encoding */
    VOBJ_EN_QP,                       /* Quoted-Printable */
    VOBJ_EN_BASE64,                   /* Base64 */
    VOBJ_EN_OTHERS,                   /* X-xxxx */
    VOBJ_EN_TOTAL
} vobj_ecoding_type_enum;

typedef enum
{
    VOBJ_CH_UTF8,
    VOBJ_CH_BIG5,
    VOBJ_CH_UTF7,
    VOBJ_CH_TOTAL
} vobj_charset_type_enum;

typedef enum
{
    VCARD_PARA_CELL,
    VCARD_PARA_HOME,
    VCARD_PARA_WORK,
    VCARD_PARA_VOICE,
    VCARD_PARA_FAX,
    VCARD_PARA_PAGER,
    VCARD_PARA_PREF,
    VCARD_PARA_TOTAL
} vcard_para_enum;


typedef enum
{
    VCARD_EMAIL_PARA_INTERNET,
    VCARD_EMAIL_PARA_TOTAL
} vcard_email_para_enum;

/* for vcard 3.0 number type */
typedef enum
{
    VCARD_TYPE_PARA_TYPE,
    VCARD_TYPE_PARA_TOTAL
}vcard_type_para_enum;

typedef enum
{
    MMI_VCARD_FIELDS_NAME_FIRST,                 /* vcard name field - family name */
    MMI_VCARD_FIELDS_NAME_SECOND,                /* vcard name field - given name */
    MMI_VCARD_FIELDS_NAME_THIRD,                 /* vcard name field - additional name */
    MMI_VCARD_FIELDS_NAME_FORTH,                 /* vcard name field - name prefix */
    MMI_VCARD_FIELDS_NAME_FIFTH,                 /* vcard name field - name suffix */
    MMI_VCARD_FIELDS_NUM_CELL,
    MMI_VCARD_FIELDS_NUM_HOME,
    MMI_VCARD_FIELDS_COMPANY,
    MMI_VCARD_FIELDS_EMAIL,
    MMI_VCARD_FIELDS_EMAIL2,
    MMI_VCARD_FIELDS_NUM_OFFICE,
    MMI_VCARD_FIELDS_NUM_FAX,
    MMI_VCARD_FIELDS_BDAY,
    MMI_VCARD_FIELDS_TITLE,
    MMI_VCARD_FIELDS_URL,
    MMI_VCARD_FIELDS_ADDRESS_POBOX,
    MMI_VCARD_FIELDS_ADDRESS_EXTENSION,
    MMI_VCARD_FIELDS_ADDRESS_STREET,
    MMI_VCARD_FIELDS_ADDRESS_CITY,
    MMI_VCARD_FIELDS_ADDRESS_STATE,
    MMI_VCARD_FIELDS_ADDRESS_POSTALCODE,
    MMI_VCARD_FIELDS_ADDRESS_COUNTRY,
    MMI_VCARD_FIELDS_NOTE,
    MMI_VCARD_FIELDS_CALLER_IMAGE,
    MMI_VCARD_FIELDS_ENUM_TOTAL             /* enum total */
} mmi_vcard_fields_enum;

typedef enum
{
    MMI_VCARD_ADDRESS_POBOX,
    MMI_VCARD_ADDRESS_EXTENSION,
    MMI_VCARD_ADDRESS_STREET,
    MMI_VCARD_ADDRESS_CITY,
    MMI_VCARD_ADDRESS_STATE,
    MMI_VCARD_ADDRESS_POSTALCODE,
    MMI_VCARD_ADDRESS_COUNTRY,
    MMI_VCARD_ADDRESS_ENUM_TOTAL
} mmi_vcard_address_enum;

#define VCARD_WRITER_FIELD_BUFFER  500 /* Writer Buffer for single field. over 50 x 9 */
#define VCARD_FILE_SIZE_LIMIT 200000

#ifdef __MMI_PHB_INFO_FIELD__
#define VCARD_WRITE_BUFF_LEN      3500          /* vcard write buffer size */
#define VCARD_READ_BUFF_LEN        2048         /* vcard read buffer size */
#else /* __MMI_PHB_INFO_FIELD__ */
#define VCARD_WRITE_BUFF_LEN      1024          /* vcard write buffer size */
#define VCARD_READ_BUFF_LEN          512        /* vcard read buffer size */
#endif /* __MMI_PHB_INFO_FIELD__ */

#define MMI_PHB_BDAY_MIN_YEAR_INT       1900        /* Min Year */
#define MMI_PHB_BDAY_MAX_YEAR_INT       2030        /* Max Year */

#define  VOBJ_CR_CHAR         0x0D
#define  VOBJ_LF_CHAR         0x0A
#define  VOBJ_NULL            '\0'
#define  VOBJ_CRLF_STR     "\r\n"
#define  VOBJ_LF_STR       "\n"
#define  VOBJ_VALUE_SEP_STR   ":"
#define VCARD_HEADER "VCARD"
#define VCARD_VERSION_21 "2.1"
#define VCARD_VERSION_30 "3.0"

typedef struct
{
    kal_uint16 nYear;
    kal_uint8 nMonth;
    kal_uint8 nDay;
    kal_uint8 nHour;
    kal_uint8 nMin;
    kal_uint8 nSec;
    kal_uint8 DayIndex; /* 0=Sunday */
} applib_time_struct;

#ifdef __MMI_PHB_INFO_FIELD__
static S8 g_vcard_write_buff[VCARD_WRITE_BUFF_LEN];
static S8 g_vcard_read_buff[VCARD_READ_BUFF_LEN];
#endif /* __MMI_PHB_INFO_FIELD__ */

static const S8* const g_vcard_field[VCARD_FIELD_TOTAL] = 
{
    "BEGIN",
    "VERSION",
    "FN",
    "N",
    "TEL",
    "EMAIL",
    "ORG",
    "BDAY",
    "TITLE",
    "URL",
    "NOTE",
    "ADR",
    "PHOTO",
    "REV",
    "LABEL",
    "NICKNAME",
    "MAILER",
    "TZ",
    "GEO",
    "ROLE",
    "LOGO",
    "AGENT",
    "CATEGORIES",
    "PRODID",
    "SORT-STRING",
    "SOUND",
    "UID",
    "CLASS",
    "KEY",
    "X-",
    "END"
};
static const S8* const g_vcard_para[] = 
{
    "CELL",
    "HOME",
    "WORK",
    "VOICE",
    "FAX",
    "PAGER",
    "PREF"
};
static const S8* const g_vobj_encoding[] = 
{
    "7BIT",
    "8BIT",
    "QUOTED-PRINTABLE",
    "BASE64",
    "X-"
};

static const S8* const g_vobj_paras[] = 
{
    "LANGUAGE",
    "CHARSET",
    "ENCODING"
};

static const S8* const g_vobj_charset[] = 
{
    "UTF-8",
    "BIG5",
    "UTF-7"
};

static const U8 g_dt_day_per_mon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static S8* mmi_vobj_uti_get_value(S8 *strString)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strstr(strString, (S8*) VOBJ_VALUE_SEP_STR) != NULL)
    {
	    return (S8*) (strstr(strString, (S8*) VOBJ_VALUE_SEP_STR) + 1);
    }
    else
    {
		return NULL;
    }
}
static S8 mmi_vobj_uti_strnicmp(S8 *src, S8 *dest, S32 maxlen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ch1, ch2;
    S8 *s1, *s2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    s1 = src;
    s2 = dest;

    while (maxlen-- > 0)
    {
        ch1 = tolower(*s1++);
        ch2 = tolower(*s2++);
        if (ch1 != ch2)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static U8 mmi_vobj_uti_cmp_filed(S8 *srcString, const S8 *refString)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 refLen, srcLen;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    srcLen = strlen(srcString);
    refLen = strlen(refString);

    if (srcLen < refLen)
    {
        return FALSE;
    }
    if (mmi_vobj_uti_strnicmp(srcString, (S8*) refString, refLen))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}

static U8 mmi_vcard_reader_get_para_value(S8 *String, const S8 *para, U8 para_enum, U8 *value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *para_pos;
    U16 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strstr(String, ":") == NULL)
    {
        return FALSE;
    }

    para_pos = strstr(String, para);

    if (para_pos != NULL)
    {
        para_pos = strstr(para_pos, "=") + 1;
        if (para_pos != NULL)
        {

            if (para_enum == VOBJ_PARA_CHARSET)
            {
                for (i = 0; i < VOBJ_CH_TOTAL; i++)
                {
                    if (mmi_vobj_uti_cmp_filed(para_pos, g_vobj_charset[i]))
                    {
                        break;
                    }
                }
                *value = (U8)i;
                return TRUE;
            }
            else if (para_enum == VOBJ_PARA_ENCODING)
            {
                for (i = 0; i < VOBJ_EN_TOTAL; i++)
                {
                    if (mmi_vobj_uti_cmp_filed(para_pos, g_vobj_encoding[i]))
                    {
                        break;
                    }
                }
                *value = (U8)i;
                return TRUE;
            }
        }
    }

    return FALSE;
}

static void mmi_vcard_util_remove_field_delimiter(S8 *str, U16 length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8* tmp = str;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (length && *str)
    {
        if (*str == '\\' && (*(str + 1) == ';' || *(str + 1) == '\\'))
        {
            *(tmp++) = *(str + 1);
            str++;
            length--;
        }
        else if (*str != ';')
        {
            *(tmp++) = *str;
        }
        str++;
        length--;
    }
    *tmp = 0;
}

static void mmi_vcard_retrive_address(S8* string, U8* pobox, U8* extension, U8* street, U8* city, U8* state, U8* postalcode, U8* country, U16 max_len, U8 default_charset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *temp_buf;
    S8 *temp_value_buf;
    S8 *cursor;
    U8 *adr_value_buf = NULL;
    U8 value;
    U16 i;
    U8 is_qp_encode = 0;
    U8 num_of_semicolon = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = mmi_vobj_uti_get_value(string);
    if (!cursor || !cursor[0])
    {
        return;
    }

    temp_buf = get_ctrl_buffer(VCARD_WRITER_FIELD_BUFFER);
    temp_value_buf = get_ctrl_buffer(VCARD_WRITER_FIELD_BUFFER);
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_ENCODING], VOBJ_PARA_ENCODING, &value) &&
        value == VOBJ_EN_QP)
    {
        is_qp_encode = 1;
    }
    /* decode */
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_CHARSET], VOBJ_PARA_CHARSET, &value))
    {
		if (value == VOBJ_CH_UTF8)
		{
			// default_charset = MMI_CHSET_UTF8;
		}
    #ifdef __MMI_CHSET_UTF7__	
		else if (value == VOBJ_CH_UTF7)
		{
			// default_charset = MMI_CHSET_UTF7;
		}
    #endif /* __MMI_CHSET_UTF7__ */
    }
    while(*cursor)
    {
        switch (num_of_semicolon)
        {
            case VCARD_ADDRESS_POBOX:
                adr_value_buf = pobox;
                break;

            case VCARD_ADDRESS_EXTENSION:
                adr_value_buf = extension;
                break;

            case VCARD_ADDRESS_STREET:
                adr_value_buf = street;
                break;

            case VCARD_ADDRESS_CITY:
                adr_value_buf = city;
                break;

            case VCARD_ADDRESS_STATE:
                adr_value_buf = state;
                break;

            case VCARD_ADDRESS_POSTALCODE:
                adr_value_buf = postalcode;
                break;

            case VCARD_ADDRESS_COUNTRY:
                adr_value_buf = country;
                break;

            default:
                free_ctrl_buffer(temp_buf);
                free_ctrl_buffer(temp_value_buf);
                return;                                                                           
        }

        if (adr_value_buf == NULL)
        {
            break;
        }

        i = 0;
        while ((*cursor != ';') && (*cursor != '\0'))
        {
            if ((*cursor == '\\') && (*(cursor + 1) == ';'))
            {
                cursor++;
            }
            if (i < VCARD_WRITER_FIELD_BUFFER-1)
            {
                temp_buf[i++] = *cursor;
            }
            cursor++;
        }
        temp_buf[i] = '\0';
        if (*cursor == ';')
        {
            num_of_semicolon++;
            cursor++;
        }
        if (is_qp_encode == 1)
        {
            /* can only pass the dest buff length (total_len - 1); */
            S32 count = applib_qprint_decode_basic(
                            temp_buf, 
                            strlen(temp_buf),
                            temp_value_buf, 
                            VCARD_WRITER_FIELD_BUFFER - 1, 
                            QPOPT_DECODE_SKIP_LEADING_SPACE);
            if(count >= 0){
                temp_value_buf[count] = 0;
            }else{
                free_ctrl_buffer(temp_buf);
                free_ctrl_buffer(temp_value_buf);
                return;
            }
        }
        else
        {
            strcpy(temp_value_buf, temp_buf);
        }

        mmi_vcard_util_remove_field_delimiter(temp_value_buf, VCARD_WRITER_FIELD_BUFFER);
        /* decode */
        strncpy((char*)adr_value_buf, temp_value_buf, BTMTK_VCARD_MAX_ADDRESS_LEN);
        /*
        mmi_chset_convert(
            default_charset,
            MMI_CHSET_UCS2,
            (char*)temp_value_buf,
            (char*)adr_value_buf,
            (max_len + 1) * ENCODING_LENGTH);
        */
    }
    free_ctrl_buffer(temp_buf);
    free_ctrl_buffer(temp_value_buf);
}

static void mmi_vcard_retrive_noraml_data(S8* string, U8* data, U16 max_len, U8 default_charset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *temp_buf;
    S8 *cursor;
    U8 value;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = mmi_vobj_uti_get_value(string);
    if (!cursor || !cursor[0])
    {
        return;
    }

    temp_buf = get_ctrl_buffer(VCARD_WRITER_FIELD_BUFFER);
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_ENCODING], VOBJ_PARA_ENCODING, &value) &&
        value == VOBJ_EN_QP)
    {
        S32 count = applib_qprint_decode_basic(
                            cursor,
                            strlen(cursor),
                            temp_buf, 
                            VCARD_WRITER_FIELD_BUFFER - 1,
                            QPOPT_DECODE_SKIP_LEADING_SPACE);
	if(count >= 0){
        temp_buf[count] = 0;
	}else{
            free_ctrl_buffer(temp_buf);
	    return;
	}
    }
    else
    {
        strncpy(temp_buf, cursor, (VCARD_WRITER_FIELD_BUFFER - 1));
        temp_buf[VCARD_WRITER_FIELD_BUFFER - 1] = 0;
    }

    /* Remove delimiter. */
    mmi_vcard_util_remove_field_delimiter(temp_buf, VCARD_WRITER_FIELD_BUFFER);

    /* decode */
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_CHARSET], VOBJ_PARA_CHARSET, &value))
    {
        if (value == VOBJ_CH_UTF8)
        {
           // default_charset = MMI_CHSET_UTF8;
        }
    #ifdef __MMI_CHSET_UTF7__   
        else if (value == VOBJ_CH_UTF7)
        {
            // default_charset = MMI_CHSET_UTF7;
        }
    #endif /* __MMI_CHSET_UTF7__ */
    }
    /* anyway, we think it is utf8 */
    strncpy((char*)data, temp_buf, max_len);

    /*
    mmi_chset_convert(
        default_charset,
        MMI_CHSET_UCS2,
        (char*)temp_buf,
        (char*)data,
        (max_len + 1) * ENCODING_LENGTH);
    */
    free_ctrl_buffer(temp_buf);
}
static U8 mmi_vcard_reader_get_para_number(S8 *String, U8 *type, U8 *pref)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *sep_pos;
    U16 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strstr(String, ":") == NULL)
    {
        return 0;
    }

    *pref = 0;
    sep_pos = String;
    while ((sep_pos = strstr(sep_pos, ";")) != NULL)
    {
        sep_pos++;
        for (i = 0; i < VCARD_PARA_TOTAL; i++)
        {
            if (mmi_vobj_uti_cmp_filed(sep_pos, g_vcard_para[i]))
            {
                break;
            }
        }
        switch (i)
        {
            case VCARD_PARA_CELL:
                *type = VCARD_PARA_CELL;
                break;
            case VCARD_PARA_HOME:
                *type = VCARD_PARA_HOME;
                break;
            case VCARD_PARA_WORK:
                *type = VCARD_PARA_WORK;
                break;
            case VCARD_PARA_FAX:
                *type = VCARD_PARA_FAX;
                break;
            case VCARD_PARA_PAGER:
                *type = VCARD_PARA_PAGER;
            case VCARD_PARA_PREF:
                *pref = 1;
                break;
            default:
                break;
        }
    }
    return 1;
}

static void mmi_vcard_util_v30_number_field_preprocess(S8 *str, U16 length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    i = 0;
    while (i < length) 
    {
        if (str[i] == ':') /* End of type */
            return;

        if ((str[i] == 'T' || str[i] == 't') && 
            (str[i + 1] == 'Y' || str[i + 1] == 'y') && 
            (str[i + 2] == 'P' || str[i + 2] == 'p') && 
            (str[i + 3] == 'E' || str[i + 3] == 'e') && 
            str[i + 4] == '=')
        {
            for (j = i; j < length && str[j] != '\0'; j++)
            {
                str[j] = str[j + 5];
            }
            str[j] = '\0';
        }
        else if (str[i] == ',')
        {
            str[i] = ';';
        }
        i++;
    }
}
static void mmi_vcard_retrive_number(S8* string, U8* number, U8* home_number, U8* office_number, U8* fax_number)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* U8 result; */
    U8 type = VCARD_PARA_TOTAL;
    U8 pref;
    S8 temp_buf[BTMTK_VCARD_MAX_NUMBER_LEN + 1];
    S8 *cursor;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    mmi_vcard_util_v30_number_field_preprocess(string, (U16)strlen(string));

    cursor = mmi_vobj_uti_get_value(string);
    if (!cursor || !cursor[0])
    {
        return;
    }

    mmi_vcard_reader_get_para_number(string, &type, &pref);
    memset(temp_buf, 0, sizeof(temp_buf));
    strncpy(temp_buf, cursor, BTMTK_VCARD_MAX_NUMBER_LEN);
    temp_buf[BTMTK_VCARD_MAX_NUMBER_LEN] = 0;

    switch (type)
    {
        case VCARD_PARA_HOME:
            if (home_number && !home_number[0])
            {
                strncpy((char*)home_number, temp_buf, BTMTK_VCARD_MAX_NUMBER_LEN);               
            }
            break;
        case VCARD_PARA_WORK:
            if (office_number && !office_number[0])
            {
                strncpy((char*)office_number, temp_buf, BTMTK_VCARD_MAX_NUMBER_LEN);
            }
            break;
        case VCARD_PARA_FAX:
            if (fax_number && !fax_number[0])
            {
                strncpy((char*)fax_number, temp_buf, BTMTK_VCARD_MAX_NUMBER_LEN);
            }
            break;
        case VCARD_PARA_PAGER:
            break; /* Don't have pager field */
    #ifdef __MMI_VCARD_TEL_VOICE_CELL_DIFFERENT__
        case VCARD_PARA_CELL:
            if (number && !number[0])
            {
                strncpy((char*)number, temp_buf, BTMTK_VCARD_MAX_NUMBER_LEN);                
            }
            break;
        default:
            break;
    #else /* __MMI_VCARD_TEL_VOICE_CELL_DIFFERENT__ */
        /* demo project not define the compile option */
        case VCARD_PARA_CELL:
        default:
            if (number && !number[0])
            {
                strncpy((char*)number, temp_buf, BTMTK_VCARD_MAX_NUMBER_LEN);
            }
            break;
    #endif /* __MMI_VCARD_TEL_VOICE_CELL_DIFFERENT__ */
    }
}


static void mmi_vcard_retrive_name(S8* string, U8* first_name, U8* second_name, U8* third_name, U8* forth_name, U8* fifth_name, U8 default_charset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8* temp_buf;
    S8* temp_value_buf;
    S8* cursor;
    U8* adr_value_buf = NULL;
    U8 value;
    U16 p2;
    U8 is_qp_encode = 0;
    U8 num_of_semicolon = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = mmi_vobj_uti_get_value(string);
    if (!cursor || !cursor[0])
    {
        return;
    }
    temp_buf = get_ctrl_buffer(VCARD_WRITER_FIELD_BUFFER);
    temp_value_buf = get_ctrl_buffer(VCARD_WRITER_FIELD_BUFFER);
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_ENCODING], VOBJ_PARA_ENCODING, &value) &&
        value == VOBJ_EN_QP)
    {
        is_qp_encode = 1;
    }
    /* decode */
    if (mmi_vcard_reader_get_para_value(string, g_vobj_paras[VOBJ_PARA_CHARSET], VOBJ_PARA_CHARSET, &value))
    {
		if (value == VOBJ_CH_UTF8)
		{
			// default_charset = MMI_CHSET_UTF8;
		}
    #ifdef __MMI_CHSET_UTF7__	
		else if (value == VOBJ_CH_UTF7)
		{
			// default_charset = MMI_CHSET_UTF7;
		}
    #endif /* __MMI_CHSET_UTF7__ */
    }
    while(*cursor)
    {
        switch (num_of_semicolon)
        {
            case VCARD_NAME_FIRST:
                adr_value_buf = first_name;
                break;

            case VCARD_NAME_SECOND:
                adr_value_buf = second_name;
                break;

            case VCARD_NAME_THIRD:
                adr_value_buf = third_name;
                break;

            case VCARD_NAME_FORTH:
                adr_value_buf = forth_name;
                break;

            case MMI_VCARD_NAME_FIFTH:
                adr_value_buf = fifth_name;
                break;

            default:
                free_ctrl_buffer(temp_buf);
                free_ctrl_buffer(temp_value_buf);
                return;                                                                            
        }
        p2 = 0;
        while ((*cursor != ';') && (*cursor != '\0'))
        {
            if ((*cursor == '\\') && (*(cursor + 1) == ';'))
            {
                cursor++;
            }
            if (p2 < (VCARD_WRITER_FIELD_BUFFER-1))
            {
                temp_buf[p2++] = *cursor;
            }
            cursor++;
        }
        temp_buf[p2] = '\0';
        if (adr_value_buf || ((num_of_semicolon == VCARD_NAME_SECOND) && first_name))
        {
            if (is_qp_encode == 1)
            {
                /* can only pass the dest buff length (total_len - 1); */
                S32 count = applib_qprint_decode_basic(
                                temp_buf,
                                strlen(temp_buf),
                                temp_value_buf, 
                                VCARD_WRITER_FIELD_BUFFER - 1, 
                                QPOPT_DECODE_SKIP_LEADING_SPACE);
                if(count >= 0){
                temp_value_buf[count] = 0;
                }else{
                    free_ctrl_buffer(temp_buf);
                    free_ctrl_buffer(temp_value_buf);
                    return;
                }
            }
            else
            {
                strncpy(temp_value_buf, temp_buf, VCARD_WRITER_FIELD_BUFFER - 1);
            }

            mmi_vcard_util_remove_field_delimiter(temp_value_buf, VCARD_WRITER_FIELD_BUFFER);
            /* decode */
            if (!adr_value_buf)
            {
                adr_value_buf = (U8*)temp_buf;
            }
            strncpy((char*)adr_value_buf, (const char*)temp_value_buf, BTMTK_VCARD_MAX_NAME_LEN);
            /*
            mmi_chset_convert(
                default_charset,
                MMI_CHSET_UCS2,
                (char*)temp_value_buf,
                (char*)adr_value_buf,
                (MMI_PHB_NAME_FIELD_LENGTH + 1) * ENCODING_LENGTH);
            
             if (adr_value_buf == temp_buf)
             {
                srv_phb_convert_to_name(first_name, first_name, adr_value_buf, BTMTK_VCARD_MAX_NAME_LEN);
             }
             */
        }
        if (*cursor == ';')
        {
            num_of_semicolon++;
            cursor++;
        }
    }
    free_ctrl_buffer(temp_buf);
    free_ctrl_buffer(temp_value_buf);
}

static U8 mmi_vcard_check_version(S8* String)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *cursor;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = mmi_vobj_uti_get_value(String);
    if (cursor && (!mmi_vobj_uti_strnicmp(cursor, VCARD_VERSION_21, strlen(VCARD_VERSION_21)) ||
        !mmi_vobj_uti_strnicmp(cursor, VCARD_VERSION_30, strlen(VCARD_VERSION_30))))
    {
        return 1;
    }
    else
    {
        return 0;    
    }
}

static void mmi_vcard_init_data(btmtk_vcard_data_struct* vcard_data)
{
    /* low efficiency */
    memset(vcard_data, 0, sizeof(btmtk_vcard_data_struct));
}

static U8 mmi_vcard_check_header(S8* String)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *cursor;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = (S8*)strstr(String, (S8*)VOBJ_VALUE_SEP_STR);
    if (!cursor || !mmi_vobj_uti_strnicmp((cursor + 1), VCARD_HEADER, strlen(VCARD_HEADER)))
    {
        return 1;
    }
    else
    {
        return 0;    
    }
}

static U8 mmi_vcard_get_field_type(S8* String)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < VCARD_FIELD_TOTAL; i++)
    {
        if (mmi_vobj_uti_cmp_filed(String, g_vcard_field[i]))
        {
            break;
        }
    }
    /* Add check for tag "N" here. Because the above check will confuse tag "N" and "NOTE" */
    if ((i == VCARD_FIELD_N) && !((String[1] == ':') || (String[1] == ';')))
    {
    #if defined(__MMI_PHB_INFO_FIELD__)
        return VCARD_FIELD_NOTE;
    #else /* defined(__MMI_PHB_INFO_FIELD__) */
        return VCARD_FIELD_TOTAL;
    #endif /* defined(__MMI_PHB_INFO_FIELD__) */
    }
    return i;
}
static S32 mmi_vcard_uti_get_single_line(S8 *strOutput, BTMTK_FS_HANDLE fh)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 nBytes = 0;
    S8 strCRLF[4];
    S8 strLF[2];
    S8 *currCursor;
    S8 str_photo_end[12];
    S8 *photo_end_cursor;
    S32 strLen = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    sprintf(strCRLF, "%c%c", VOBJ_CR_CHAR, VOBJ_LF_CHAR);
    sprintf(strLF, "%c", VOBJ_LF_CHAR);
    sprintf(str_photo_end, "//Z%s%s", strCRLF, strCRLF);
    btmtk_fs_read_ucs2(fh, strOutput, VCARD_READ_BUFF_LEN - 1, &nBytes);

    strOutput[nBytes] = VOBJ_NULL;

    if (!nBytes)
    {
        return 0;
    }

    if ((currCursor = strstr(strOutput, strLF)) != NULL)  /* find out first LF */
    {
        if ((photo_end_cursor = strstr(strOutput, str_photo_end)) != NULL)  /* find out photo end */
        {
            if (currCursor > photo_end_cursor)
            {
                currCursor = photo_end_cursor + 6;
            }
        }
        strLen = (S32) (currCursor - strOutput);
        if (strOutput[strLen - 1] == VOBJ_CR_CHAR)
        {
            strOutput[strLen - 1] = VOBJ_NULL;
        }
        else
        {
            strOutput[strLen] = VOBJ_NULL;
        }


        btmtk_fs_seek_ucs2(fh, (S32) (strLen - nBytes + 1), BTMTK_FS_SEEK_CURRENT);
        nBytes = strLen;
    }
    else    /* CRLF not found, read until CRLF */
    {
        strOutput[nBytes] = VOBJ_NULL;
    }

    return nBytes;
}

static S32 mmi_vcard_uti_get_line(S8 *strOutput, BTMTK_FS_HANDLE fh)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 nBytes = 0;
#ifdef __MMI_PHB_INFO_FIELD__
    S8 *buffer = g_vcard_read_buff;
#else
    S8 *buffer = get_ctrl_buffer(VCARD_READ_BUFF_LEN);
#endif
    S32 i;
    S32 strLen;
    S8 *curr_cursor;
    S8 *cursor = NULL;
    U16 buffer_offset;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btmtk_fs_read_ucs2(fh, buffer, VCARD_READ_BUFF_LEN - 1, &nBytes);
    buffer[nBytes] = '\0';

    if (!nBytes)
    {
    #ifndef __MMI_PHB_INFO_FIELD__
        free_ctrl_buffer(buffer);
    #endif /* __MMI_PHB_INFO_FIELD__ */
        return 0;
    }

    // only the VERSION and END are sure to be single line
    if ((strncmp(g_vcard_field[VCARD_FIELD_END], buffer, strlen(g_vcard_field[VCARD_FIELD_END])) == 0) ||
        (strncmp(g_vcard_field[VCARD_FIELD_VERSION], buffer, strlen(g_vcard_field[VCARD_FIELD_VERSION])) == 0))
    {
        btmtk_fs_seek_ucs2(fh, 0 - nBytes, BTMTK_FS_SEEK_CURRENT);
    #ifndef __MMI_PHB_INFO_FIELD__
        free_ctrl_buffer(buffer);
    #endif /* __MMI_PHB_INFO_FIELD__ */
        return mmi_vcard_uti_get_single_line(strOutput, fh);
    }

    /* find next filed */
    for (i = VCARD_FIELD_FN; i < VCARD_FIELD_TOTAL; i++)
    {
        buffer_offset = 1;
        do
        {
            curr_cursor = strstr(buffer + buffer_offset, g_vcard_field[i]);
            if (curr_cursor != (S8*) NULL)
            {
                buffer_offset += (curr_cursor - (buffer + buffer_offset) + 1);//strlen(g_vcard_field[i]);
            }
        }while(curr_cursor != (S8*) NULL && *(curr_cursor - 1) != 0x0A);

        if (curr_cursor == (S8*) NULL)
        {
            continue;
        }
        else if (cursor == (S8*) NULL && (*(curr_cursor - 1) == 0x0A))
        {
            cursor = curr_cursor;
        }
        else if (((S32) cursor > (S32) curr_cursor) && (*(curr_cursor - 1) == 0x0A))
        {
            cursor = curr_cursor;
        }
    }
    if (cursor == NULL)
    {
        btmtk_fs_seek_ucs2(fh, 0 - nBytes, BTMTK_FS_SEEK_CURRENT);
    #ifndef __MMI_PHB_INFO_FIELD__
        free_ctrl_buffer(buffer);
    #endif /* __MMI_PHB_INFO_FIELD__ */
        return mmi_vcard_uti_get_single_line(strOutput, fh);
        /* recursively to get single line to parse */
    }

    strLen = (S32) cursor - (S32) buffer;
    strncpy(strOutput, buffer, strLen);
    strOutput[strLen] = VOBJ_NULL;

    /* Remove end of line "\r\n" */
    if (strncmp((S8*) ((strOutput + strLen) - strlen(VOBJ_LF_STR)), (S8*) VOBJ_LF_STR, strlen(VOBJ_LF_STR)) ==
        0)
    {
        if (strOutput[strLen - strlen(VOBJ_LF_STR) - 1] == VOBJ_CR_CHAR)
        {
            strOutput[strLen - strlen(VOBJ_LF_STR) - 1] = VOBJ_NULL;
        }
        else
        {
            strOutput[strLen - strlen(VOBJ_LF_STR)] = VOBJ_NULL;
        }
    }

    btmtk_fs_seek_ucs2(fh, (S32) (strLen - nBytes), BTMTK_FS_SEEK_CURRENT);

#ifndef __MMI_PHB_INFO_FIELD__
    free_ctrl_buffer(buffer);
#endif

    return strLen;
}

static void mmi_vcard_retrive_bday(S8 *string, U8* birthday)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *cursor;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cursor = mmi_vobj_uti_get_value(string);
    if (cursor && cursor[0])
    {
        strncpy((char*)birthday, cursor, BTMTK_VCARD_MAX_BIRTHDAY_LEN);
    }
}

static S32 mmi_vcard_parse_file(BTMTK_FS_HANDLE fh, btmtk_vcard_data_struct* vcard_data, U8 default_charset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifdef __MMI_PHB_INFO_FIELD__
    S8* inBuff = g_vcard_write_buff;
#else
    S8 inBuff[VCARD_WRITE_BUFF_LEN];
#endif
    U8 token;
    U32 f_size;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (fh > 0)
    {
        btmtk_fs_get_filesize_ucs2(fh, &f_size);
        if (f_size > VCARD_FILE_SIZE_LIMIT)
        {
            return BTMTK_VCD_ERR_OVERSIZE;
        }
    }
    else
    {
        return fh;
    }

    /* BEGIN:VCARD */
    if (mmi_vcard_uti_get_single_line(inBuff, fh) != 0)
    {
        if (mmi_vcard_get_field_type(inBuff) != VCARD_FIELD_BEGIN)
        {
            return BTMTK_VCD_ERR_HEADER;
        }
        else if (!mmi_vcard_check_header(inBuff))
        {
            return BTMTK_VCD_ERR_HEADER;
        }
    }
    else
    {
        return BTMTK_VCD_ERR_FAILED;
    }
    mmi_vcard_init_data(vcard_data);

    /* PARSE EACH FIELD */
    while (mmi_vcard_uti_get_line(inBuff, fh) != 0)
    {
        token = mmi_vcard_get_field_type(inBuff);

        switch (token)
        {
            case VCARD_FIELD_END:
                if (!mmi_vcard_check_header(inBuff))
                {
                    return BTMTK_VCD_ERR_FAILED;
                }
                else
                {
                    return BTMTK_VCD_ERR_OK;
                }
                break;                
                
            case VCARD_FIELD_FN:
            case VCARD_FIELD_REV:
            case VCARD_FIELD_PHOTO:
            case VCARD_FIELD_LABEL:
            case VCARD_FIELD_NICKNAME:
            case VCARD_FIELD_MAILER:
            case VCARD_FIELD_TZ:
            case VCARD_FIELD_GEO:
            case VCARD_FIELD_ROLE:
            case VCARD_FIELD_LOGO:
            case VCARD_FIELD_AGENT:
            case VCARD_FIELD_CATEGORIES:
            case VCARD_FIELD_PRODID:
            case VCARD_FIELD_SORT_STRING:
            case VCARD_FIELD_SOUND:
            case VCARD_FIELD_UID:
            case VCARD_FIELD_CLASS:
            case VCARD_FIELD_KEY:
            case VCARD_FIELD_X:
                break;

            /* VERSION */
            case VCARD_FIELD_VERSION:
                if (!mmi_vcard_check_version(inBuff))
                {
                    return BTMTK_VCD_ERR_VERSION;
                }
                break;

            case VCARD_FIELD_N:
                if (vcard_data->first_name
                    || vcard_data->second_name 
                    || vcard_data->third_name
                    || vcard_data->fourth_name
                    || vcard_data->fifth_name)
                {
                    mmi_vcard_retrive_name(
                        inBuff,
                        vcard_data->first_name,
                        vcard_data->second_name,
                        vcard_data->third_name,
                        vcard_data->fourth_name,
                        vcard_data->fifth_name,
                        default_charset);
                }
                break;

            case VCARD_FIELD_TEL:
                if (vcard_data->cell_num
                    || vcard_data->home_num 
                    || vcard_data->office_num
                    || vcard_data->fax_num)
                {
                    mmi_vcard_retrive_number(
                        inBuff,
                        vcard_data->cell_num,
                        vcard_data->home_num,
                        vcard_data->office_num,
                        vcard_data->fax_num);
                }
                break;

            case VCARD_FIELD_EMAIL:
                if (vcard_data->email && !vcard_data->email[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->email, BTMTK_VCARD_MAX_EMAIL_LEN,default_charset);
                }
                else if (vcard_data->email2 && !vcard_data->email2[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->email2, BTMTK_VCARD_MAX_EMAIL_LEN,default_charset);
                }
                break;

            case VCARD_FIELD_ORG:
                if (vcard_data->company && !vcard_data->company[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->company, BTMTK_VCARD_MAX_COMPANY_LEN, default_charset);
               }
                break;

            case VCARD_FIELD_BDAY:
                mmi_vcard_retrive_bday(inBuff, vcard_data->birthday);
                break;

            case VCARD_FIELD_TITLE:
                if (vcard_data->title && !vcard_data->title[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->title, BTMTK_VCARD_MAX_TITLE_LEN,default_charset);
                }
                break;
            case VCARD_FIELD_URL:
                if (vcard_data->url && !vcard_data->url[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->url, BTMTK_VCARD_MAX_URL_LEN,default_charset);
                }
                break;
            case VCARD_FIELD_ADR:
                if (vcard_data->postbox && !vcard_data->postbox[0]
                    && vcard_data->extension && !vcard_data->extension[0]
                    && vcard_data->street && !vcard_data->street[0]
                    && vcard_data->city && !vcard_data->city[0]
                    && vcard_data->state && !vcard_data->state[0]
                    && vcard_data->postcode && !vcard_data->postcode[0]
                    && vcard_data->country && !vcard_data->country[0])
                {
                    mmi_vcard_retrive_address(
                        inBuff,
                        vcard_data->postbox, 
                        vcard_data->extension, 
                        vcard_data->street, 
                        vcard_data->city, 
                        vcard_data->state, 
                        vcard_data->postcode,
                        vcard_data->country,
						BTMTK_VCARD_MAX_ADDRESS_LEN,
                        default_charset);
                }
                break;
            case VCARD_FIELD_NOTE:
                if (vcard_data->note && !vcard_data->note[0])
                {
                    mmi_vcard_retrive_noraml_data(inBuff, vcard_data->note, BTMTK_VCARD_MAX_NOTE_LEN,default_charset);
                }
                break;

            default:
                break;
        }
    }
    return BTMTK_VCD_ERR_FAILED;
}
S32 btmtk_vcard_parse_file_to_struct(U16* filename, btmtk_vcard_data_struct* vcard_data)
{
    S32 ret;
    BTMTK_FS_HANDLE fh;

    if (!filename || !vcard_data)
        return BTMTK_VCD_ERR_PARAM;

    fh = btmtk_fs_open_ucs2(filename, BTMTK_FS_READ_ONLY);
    if (fh <= 0)
        return BTMTK_VCD_ERR_IO;

    ret = mmi_vcard_parse_file(fh, vcard_data, 0);
    btmtk_fs_close_ucs2(fh);
    return ret;
}
