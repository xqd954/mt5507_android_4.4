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

#include "hid.h"
#include "bluetooth_hid_struct.h"
#include "bttypes.h"
#include "hidi.h"
#include "bt_os_api.h"
#include "bt_mmi.h"
#include <math.h>

#define MOUSE_SUPPORT 
#define CONSUMER_SUPPORT


#define KEYBOARD_USAGEPAGE   0x07
#define BUTTON_USAGEPAGE   0x09
#define GENERIC_DESKTOP_USAGEPAGE   0x01
#define CONSUMER_USAGEPAGE 0x0C
#ifdef __NORDIC_VOICE_COMMAND__
#define VENDOR_DEFINED_USAGEPAGE 0xFF00
#endif


#define USAGE_POWER	0x30
#define USAGE_PLAYPAUSE	0xCD
#define USAGE_STOPCD	0xB7
#define USAGE_PREVIOUSSONG	0xB6
#define USAGE_NEXTSONG		0xB5
#define USAGE_EJECTCD	0xB8
#define USAGE_VOLUMEUP 	0xE9
#define USAGE_VOLUMEDOWN 	0xEA
#define USAGE_MUTE 		0xE2
#define USAGE_WWW 		0x0196
#define USAGE_BACK 		0x0224
#define USAGE_FORWARD 	0x0225
#define USAGE_STOP 		0x0226
#define USAGE_SEARCH 	0x0221
#define USAGE_HOME 	0x0223
#define USAGE_SCROLLUP 		0x0233	
#define USAGE_SCROLLDOWN 	0x0234
#define USAGE_MAIL 	0x018A
#define USAGE_CONFIG 	0x0183
#define USAGE_IMAGES 	0x01B6
#define USAGE_AUDIO 	0x01B7
#define USAGE_MOVIE 	0x01B8
#define USAGE_MESSENGER 	0x01BC
#define USAGE_SCREENSAVER 	0x01B1
#define USAGE_ZOOMIN 	0x022D
#define USAGE_ZOOMOUT 	0x022E
#define USAGE_FULLSCREEN	0x0230
#define USAGE_MENU	0x40
#define USAGE_COFFEE	0x019E



#define CONSUMER_USAGE_SIZE 29
#define MAX_VAR_TABLE_SIZE 16
#define MAX_KEY_ARRAY_SIZE 10
#define MAX_ARY_TABLE_SIZE 16

#define HID_GAMEPAD_INDEX_IN_DRIVER	150


typedef struct
{
	U16	usage;
	U8	keyStatus;
}keyEvent;

typedef struct
{
	keyEvent*	keyAry;
	U8			reportID;
	U8			keyAryIndex;
	U8 			firstAry;
}keyArrayStruct;

U16 CONSUMER_EVENT_MAP[CONSUMER_USAGE_SIZE][2] = {
	{USAGE_PLAYPAUSE,232},
	{USAGE_STOPCD,233},
	{USAGE_PREVIOUSSONG,234},
	{USAGE_NEXTSONG,235},
	{USAGE_EJECTCD,236},
	{USAGE_VOLUMEUP,237},
	{USAGE_VOLUMEDOWN,238},
	{USAGE_MUTE,239},
	{USAGE_WWW,240},
	{USAGE_BACK,241},
	{USAGE_FORWARD,242},
	{USAGE_STOP,243},
	{USAGE_SEARCH,244},
	{USAGE_SCROLLUP,245},
	{USAGE_SCROLLDOWN,246},
	{USAGE_HOME,252},
	{USAGE_POWER,102},
	{USAGE_MAIL,253},
	{USAGE_CONFIG,254},
	{USAGE_IMAGES,221},
	{USAGE_AUDIO,222},
	{USAGE_MOVIE,223},
	{USAGE_MESSENGER,220},
	{USAGE_SCREENSAVER,219},
	{USAGE_ZOOMIN,217},
	{USAGE_ZOOMOUT,218},
	{USAGE_FULLSCREEN,216},
	{USAGE_MENU,215},
	{USAGE_COFFEE,219}
};
char	ItemSize[4] = {0,1,2,4};
static keyArrayStruct keyArySet[MAX_KEY_ARRAY_SIZE];	
static U8 firstAry = 0;

void Hid_Init_KeyArray()
{
	int i ;
	for(i = 0; i < MAX_KEY_ARRAY_SIZE; i++)
	{
		keyArySet[i].firstAry = 0;
		keyArySet[i].keyAry = NULL;
		keyArySet[i].keyAryIndex = 0;
		keyArySet[i].reportID = 0;
	}
}

void Hid_Free_Mem()
{
	int i;
	for(i = 0; i < MAX_KEY_ARRAY_SIZE; i++)
	{
		if(keyArySet[i].firstAry == 1)
		{
			bt_prompt_trace(MOD_BT,"[HID]Free the array table");
			hid_free(keyArySet[i].keyAry);
			keyArySet[i].firstAry = 0;
		}
		keyArySet[i].keyAry = NULL;
		keyArySet[i].keyAryIndex = 0;
		keyArySet[i].reportID = 0;
	}
}

void  ShowBytes( void *  s,  int  n)
{
    U8 *  start  =  (U8* )s;
	int i=0;
	bt_prompt_trace(MOD_BT,"[HID][OFFSET] ADDRESS: VALUE\n\n");
 
 for  (  i  =   0 ; i  <  n; i ++ )
 {
        bt_prompt_trace(MOD_BT, "[HID][%4d] %8x: %2x\n ",i,start+i,*(start+i));
  
  if  ((i  +   1 )  %   4   ==   0 )
  {
           bt_prompt_trace(MOD_BT, " ----------------------\n " );
        } 
    } // for 
 } 


U8 Hid_Init_Parser(HidParseData*	tmpHidParser)
{
	//HidParseData*	tmpHidParser = (HidParseData*)hid_malloc(sizeof(HidParseData));
	if(!tmpHidParser)
	{
		bt_prompt_trace(MOD_BT,"[HID]HidParseData hid_malloc error");
		return 0;
	}
	
	tmpHidParser->reportDesc = NULL;
	tmpHidParser->usageQueue = (HidUsage*)hid_malloc(HID_USAGE_MAX * sizeof(HidUsage));
	if(!tmpHidParser->usageQueue)
	{
		bt_prompt_trace(MOD_BT,"[HID]usageQueue hid_malloc error");
		return 0;
	}

	btmtk_os_memset((U8*) tmpHidParser->usageQueue, 0, sizeof(HID_USAGE_MAX * sizeof(HidUsage)));

	tmpHidParser->usageSize = 0;
	tmpHidParser->usageNum = 0;
	tmpHidParser->reportDescLength = 0;
	tmpHidParser->logicalMAX = 0;
	tmpHidParser->logicalMIN = 0;
	tmpHidParser->physicalMAX = 0;
	tmpHidParser->physicalMIN = 0;
	tmpHidParser->reportCount = 0;
	tmpHidParser->reportSize = 0;
	tmpHidParser->reportID = 0;
	tmpHidParser->usagePage = 0;
	tmpHidParser->unitExponent = 0;
	tmpHidParser->unit = 0;

	tmpHidParser->pos = 0;
	tmpHidParser->collectionArray = (HidUsageCollection*)hid_malloc(HID_COLLECTION_MAX * sizeof(HidUsageCollection));
	btmtk_os_memset((HidUsageCollection*) tmpHidParser->collectionArray, 0, sizeof(HID_COLLECTION_MAX * sizeof(HidUsageCollection)));
	tmpHidParser->collectionIndex = 0;
	//return tmpHidParser;
	Hid_Init_KeyArray();
	return 1;
}

void Hid_Reset_UsageQueue(HidParseData  *hpd)
{
	hpd->usageSize = 0;
	hpd->usageNum = 0;
	btmtk_os_memset((U8*) hpd->usageQueue, 0 ,HID_USAGE_MAX *sizeof(HidUsage));
}

S32 Hid_Format_Value(S32 value, U8 size)
{
	if(size == 1)
		value = (S32)(S8)value;
	else if(size == 2)
		value = (S32)(S16)value;
	return value;
}

void Hid_Set_Usage_Page(HidParseData *hpd)
{
	if((hpd->item & SIZE_MASK) >2)
		hpd->usageQueue[hpd->usageSize].usagepage = (U16)(hpd->value >>16);
	else
		hpd->usageQueue[hpd->usageSize].usagepage = hpd->usagePage;

	btmtk_os_memcpy((U8*)hpd->usageQueue[hpd->usageSize].usageCollection, hpd->collectionArray, sizeof(HidUsageCollection) * hpd->collectionIndex);

}

	
BtStatus Hid_SdpParse(HidChannel *channel, HidReportConstructor *reportCons, HidParseData  *hpd)
{
	U16		reportOffset[HID_REPORT_ID_MAX][3] = {{0}};
	U16		i;
	U8		once = 0;

	if(!channel || !hpd)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty Channel or Parser data");
		return BT_STATUS_FAILED;
	}			
	
	//channel->reportCons = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
	if(!reportCons)
	{
		bt_prompt_trace(MOD_BT,"[HID]reportCons hid_malloc error");
		return BT_STATUS_FAILED;
	}

    reportCons->deviceType= 0;
    
	hpd->reportDescLength = channel->queryRsp.descriptorLen;
	hpd->reportDesc = (U8*)hid_malloc(hpd->reportDescLength);
	if(!hpd->reportDesc)
	{
		bt_prompt_trace(MOD_BT,"[HID]reportDesc hid_malloc error");
		return BT_STATUS_FAILED;
	}
	btmtk_os_memset(hpd->reportDesc, 0, hpd->reportDescLength);
	
	/* modify for HOGP PTS */
    
    /* report descriptor is positive sequence */
    if(ITEM_UPAGE == (*channel->queryRsp.descriptorList & ITEM_MASK))
    {  
        btmtk_os_memcpy(hpd->reportDesc, channel->queryRsp.descriptorList, hpd->reportDescLength);
        bt_prompt_trace(MOD_BT,"[HID]report descriptor 1:");
        ShowBytes(hpd->reportDesc,hpd->reportDescLength);
    }
    /* report descriptor is nagetive sequence */
    else
    {
        U8 TempBuffer[1024] = {0};
    		
        for(i=0; i < hpd->reportDescLength; i++)
        {
            TempBuffer[hpd->reportDescLength - i - 1] = *(channel->queryRsp.descriptorList + i);
        }
        btmtk_os_memcpy(hpd->reportDesc, TempBuffer, hpd->reportDescLength);
        bt_prompt_trace(MOD_BT,"[HID]report descriptor 2:");
        ShowBytes(hpd->reportDesc,hpd->reportDescLength);
    }
    
	//btmtk_os_memset((U8*) channel->reportCons, 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
	for(i=0; i<HID_REPORT_ID_MAX; i++)
	{
		reportCons[i].reportID = 0;
		//btmtk_os_memset((U8*) channel->reportCons[i].reportFormat, 0, sizeof(HidReportFormat*));
		reportCons[i].reportFormat = NULL;
	}
	
	while(hpd->pos < hpd->reportDescLength)
	{
		hpd->item = hpd->reportDesc[hpd->pos++];
		hpd->value = 0;
		btmtk_os_memcpy((U8*)&hpd->value, &hpd->reportDesc[hpd->pos], ItemSize[hpd->item & SIZE_MASK]);
		//Pos on next item
		hpd->pos += ItemSize[hpd->item & SIZE_MASK];

		switch(hpd->item & ITEM_MASK)
		{
			case ITEM_UPAGE:
			{
				hpd->usagePage = (U16)hpd->value;
				break;
			}
			case ITEM_USAGE:
			{
				if(hpd->usageSize == HID_USAGE_MAX)
				{					
					bt_prompt_trace(MOD_BT,"[HID]realloc usageQueue");
					hpd->usageQueue=(HidUsage*)realloc(hpd->usageQueue, 2*HID_USAGE_MAX * sizeof(HidUsage));
					if(!hpd->usageQueue)
					{
						bt_prompt_trace(MOD_BT,"[HID]usageQueue realloc error");
						break;
					}
				}
				Hid_Set_Usage_Page(hpd);
				hpd->usageQueue[hpd->usageSize].usageID.usageIDValue= (U16)(hpd->value & 0xFFFF);
				hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_SINGLE;
				hpd->usageQueue[hpd->usageSize].usageCountRemain = 0;
				hpd->usageSize++;
				hpd->usageNum++;
				break;
			}
			case ITEM_USAGE_MIN:
			{
				Hid_Set_Usage_Page(hpd);
				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN = (U16)(hpd->value & 0xFFFF);
				hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_RANGE;
				hpd->usageQueue[hpd->usageSize].usageCountRemain = 0;
				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN_Set = TRUE;
				if(hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX_Set == TRUE)
					hpd->usageSize++;
				break;
			}
			case ITEM_USAGE_MAX:
			{
				Hid_Set_Usage_Page(hpd);
				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX = (U16)(hpd->value & 0xFFFF);
				hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_RANGE;
				hpd->usageQueue[hpd->usageSize].usageCountRemain = 0;
				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX_Set = TRUE;
				if(hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN_Set == TRUE)
				{				
					hpd->usageNum += hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX - hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN + 1;
					hpd->usageSize++;
				}
				break;
			}
			case ITEM_COLLECTION:
			{
				if(hpd->collectionIndex < HID_COLLECTION_MAX)
				{
					hpd->collectionArray[hpd->collectionIndex].usagepage =  hpd->usageQueue[hpd->usageSize - 1].usagepage;
					hpd->collectionArray[hpd->collectionIndex].usageIDValue=  hpd->usageQueue[hpd->usageSize - 1].usageID.usageIDValue;
					if(hpd->collectionArray[hpd->collectionIndex].usagepage == GENERIC_DESKTOP_USAGEPAGE)
					{		
						if(reportCons->deviceType != HID_DEVICE_TYPE_KEYBOARD && hpd->collectionArray[hpd->collectionIndex].usageIDValue != HID_DEVICE_TYPE_POINTER)
							reportCons->deviceType = hpd->collectionArray[hpd->collectionIndex].usageIDValue;
					}
					hpd->collectionIndex++;
				}

				Hid_Reset_UsageQueue(hpd);
				break;
			}
			case ITEM_END_COLLECTION:
			{
				hpd->collectionIndex--;
				Hid_Reset_UsageQueue(hpd);
				break;
			}
			case ITEM_REP_ID:
			{
				hpd->reportID = (U8)hpd->value;
				break;
			}
			case ITEM_REP_COUNT:
			{
				hpd->reportCount = (U16)hpd->value;
				break;
			}
			case ITEM_REP_SIZE:
			{
				hpd->reportSize = (U16)hpd->value;
				break;
			}
			case ITEM_LOG_MIN:
			{
				hpd->logicalMIN = Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_LOG_MAX:
			{
				hpd->logicalMAX = Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_PHY_MIN:
			{
				hpd->physicalMIN= Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_PHY_MAX:
			{
				hpd->physicalMAX= Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_UNIT:
			{
				hpd->unit = hpd->value;
				break;
			}
			case ITEM_UNIT_EXP:
			{
				hpd->unitExponent = hpd->value;
				break;
			}
			case ITEM_INPUT:
			case ITEM_OUTPUT:
			case ITEM_FEATURE:
			{
				if(hpd->usageSize > 0 && (hpd->usageNum < hpd->reportCount))
				{
					bt_prompt_trace(MOD_BT,"[HID]hpd->usageNum = %d; hpd->reportCount = %d", hpd->usageNum, hpd->reportCount);
					hpd->usageQueue[hpd->usageSize].usageCountRemain = (hpd->reportCount - hpd->usageNum);
					bt_prompt_trace(MOD_BT,"[HID]hpd->usageQueue[hpd->usageSize-1].usageCountRemain = %d;", hpd->usageQueue[hpd->usageSize-1].usageCountRemain);
				}
				
				U8 	formatIndex = 0;
				if(reportCons[hpd->reportID].reportID != hpd->reportID ||hpd->reportID == 0)
				{
					if(once == 1 && hpd->reportID == 0);
					else
					{
						reportCons[hpd->reportID].reportID = hpd->reportID;

						reportCons[hpd->reportID].reportFormat = (HidReportFormat*)hid_malloc(3 * sizeof(HidReportFormat));
						if(!reportCons[hpd->reportID].reportFormat)
						{
							bt_prompt_trace(MOD_BT,"[HID]reportFormat hid_malloc error");
							return BT_STATUS_FAILED;
						}
					
						btmtk_os_memset((U8*) reportCons[hpd->reportID].reportFormat, 0, 3 * sizeof(HidReportFormat));
						if(hpd->reportID == 0)
							once = 1;
					}
				}
			
				if((hpd->item & ITEM_MASK) == ITEM_INPUT)
				{
					formatIndex = 0;
				}
				else if((hpd->item & ITEM_MASK) == ITEM_OUTPUT)
				{
					formatIndex = 1;
				}
				else if((hpd->item & ITEM_MASK) == ITEM_FEATURE)
				{
					formatIndex = 2;
				}
				
				if(reportCons[hpd->reportID].reportFormat[formatIndex].reportType == HID_REPORT_OTHER)
				{
					if(formatIndex == 0)
						reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_INPUT;
					else if(formatIndex == 1)
						reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_OUTPUT;
					else if(formatIndex == 2)
						reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_FEATURE;

					reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit = (HidReportUnit*)hid_malloc(HID_REPORT_UNIT_MAX_SIZE * sizeof(HidReportUnit));
					if(!reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit)
					{
						bt_prompt_trace(MOD_BT,"[HID]reportUnit hid_malloc error");
						return BT_STATUS_FAILED;
					}
					btmtk_os_memset((U8*) reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit, 0, HID_REPORT_UNIT_MAX_SIZE * sizeof(HidReportUnit));
					
				}					

				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage = (HidUsage*)hid_malloc(HID_USAGE_MAX * sizeof(HidUsage));
				if(!reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage)
				{
					bt_prompt_trace(MOD_BT,"[HID]reportUnit::hidUsage hid_malloc error");
					return BT_STATUS_FAILED;
				}				
				btmtk_os_memset((U8*)reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage, 0, HID_USAGE_MAX*sizeof(HidUsage));

				if(hpd->value & MAIN_ITEM_ARG_VAR)
					reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].arrayAttr = HID_VAR_ATTRIBUTE;	
				else
					reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].arrayAttr = HID_ARRAY_ATTRIBUTE;
				
				btmtk_os_memcpy((U8*)reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage, (U8*)hpd->usageQueue, hpd->usageSize*sizeof(HidUsage));
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].usageSize = hpd->usageSize;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].logicalMIN= hpd->logicalMIN;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].logicalMAX= hpd->logicalMAX;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].physicalMIN= hpd->physicalMIN;							
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].physicalMAX= hpd->physicalMAX;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].unit= hpd->unit;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].unitExponent= hpd->unitExponent;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].offset = reportOffset[hpd->reportID][formatIndex] ;
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].reportCount= hpd->reportCount; 						
				reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].reportSize= hpd->reportSize; 						
						
				reportOffset[hpd->reportID][formatIndex] += (hpd->reportCount) * (hpd->reportSize);
						
				reportCons[hpd->reportID].reportFormat[formatIndex].unitNum++; 				
				Hid_Reset_UsageQueue(hpd);
				break;
			}
			default:
				break;
		}
	}
	return BT_STATUS_SUCCESS;
}

void Hid_Free_Parser(HidParseData *hpd)
{	
	hid_free(hpd->usageQueue);
	hid_free(hpd->reportDesc);
	hid_free(hpd->collectionArray);
	hid_free(hpd);
}

U16 Find_Consumer_Usage_Index(U16 usage)
 {
	 U16 i;
	 for(i = 0; i<CONSUMER_USAGE_SIZE; i++)
	 {
		 if(CONSUMER_EVENT_MAP[i][0] == usage)
			 break;
	 }
	 if(i == CONSUMER_USAGE_SIZE)
		 return 0;
	 else
		 return CONSUMER_EVENT_MAP[i][1];
 }

 keyArrayStruct* Find_KeyArray_By_ReportID(U8 reportID)
 {
 	int i;
	for(i = 0; i < MAX_KEY_ARRAY_SIZE; i++)
	{
		if(keyArySet[i].reportID == reportID || keyArySet[i].reportID == 0)
			break;
	}
	return &keyArySet[i];
 }

 BtStatus Hid_Ary_Report_To_Driver(int fd, U8 reportID, U16 usage, U8 reportSize, U16 usagePage)
{
	U16	tmpUsage;
	static int	capsLockPress=1;
	static int	numLockPress=1;
	U8 	i;
	keyArrayStruct* tmpKeyAry;

	if(fd<0)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty hid fd");
		return BT_STATUS_FAILED;
	}

	tmpKeyAry = Find_KeyArray_By_ReportID(reportID);
	
	if(tmpKeyAry->firstAry == 0)
	{
		tmpKeyAry->keyAry = (keyEvent*)hid_malloc(MAX_ARY_TABLE_SIZE*sizeof(keyEvent));
		btmtk_os_memset((U8*)tmpKeyAry->keyAry, 0 ,MAX_ARY_TABLE_SIZE*sizeof(keyEvent));
		tmpKeyAry->firstAry = 1;
		tmpKeyAry->reportID = reportID;
	}

	if(tmpKeyAry->keyAryIndex == 0)
	{
		for(i=0; i<MAX_ARY_TABLE_SIZE; i++)
		{
			if(tmpKeyAry->keyAry[i].usage != 0)
				tmpKeyAry->keyAry[i].keyStatus = HID_KEY_RELEASE;
		}

	}
	

	bt_prompt_trace(MOD_BT,"[HID]Report Array Usage %x",usage);

	if(usage != 0)
	{
		for(i=0; i<MAX_ARY_TABLE_SIZE; i++)
		{
			//find position
			if(usage == tmpKeyAry->keyAry[i].usage)
			{
				tmpKeyAry->keyAry[i].keyStatus=HID_KEY_RESERVE;
				break;
			}
		}
		//not find
		if(i == MAX_ARY_TABLE_SIZE)
		{	i=tmpKeyAry->keyAryIndex;
			while(tmpKeyAry->keyAry[i].usage != 0)
				i++;
			if(i == MAX_ARY_TABLE_SIZE)
			{
				bt_prompt_trace(MOD_BT,"[HID]Too many array events, droped");
				return BT_STATUS_FAILED;
			}
			tmpKeyAry->keyAry[i].usage=usage;
			tmpKeyAry->keyAry[i].keyStatus=HID_KEY_PRESS;
		}
			
	}

	tmpKeyAry->keyAryIndex++;
	
	if(tmpKeyAry->keyAryIndex == reportSize)
	{
		tmpKeyAry->keyAryIndex = 0;
		for(i=0; i<MAX_ARY_TABLE_SIZE; i++)
		{
			if(tmpKeyAry->keyAry[i].usage != 0)
			{
				if(tmpKeyAry->keyAry[i].keyStatus == HID_KEY_PRESS)
				{
					if(usagePage == CONSUMER_USAGEPAGE)
					{
						tmpUsage = Find_Consumer_Usage_Index(tmpKeyAry->keyAry[i].usage);
						if(tmpUsage != 0)
						{
							tmpKeyAry->keyAry[i].usage = tmpUsage;
							bt_prompt_trace(MOD_BT,"[HID]Report Array  new Usage %d",tmpKeyAry->keyAry[i].usage);
						}
					}
					if(ioctl(fd,HID_KEY_PRESS, &tmpKeyAry->keyAry[i].usage)<0)	
					{
						bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
						return BT_STATUS_FAILED;
					}
					/*
					switch(keyAry[i].usage)
					{
						case KEY_CAPS_LOCK_USAGE:
							tmpUsage=KEY_LEFT_SHIFT_USAGE;
							ioctl(fd,HID_KEY_PRESS, &tmpUsage);
							ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							if(capsLockPress%2 == 1)
							{
								ioctl(fd,HID_KEY_PRESS, &tmpUsage);
								ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							}
							capsLockPress++;
							break;
						case KEY_NUM_LOCK_USAGE:
							tmpUsage=KEY_LEFT_ALT_USAGE;
							ioctl(fd,HID_KEY_PRESS, &tmpUsage);
							ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							if(numLockPress%2 == 1)
							{
								ioctl(fd,HID_KEY_PRESS, &tmpUsage);
								ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							}
							numLockPress++;
							break;
						default:
							break;
					}
					*/
				}
				if(tmpKeyAry->keyAry[i].keyStatus == HID_KEY_RELEASE)
				{
					if(ioctl(fd,HID_KEY_RELEASE, &tmpKeyAry->keyAry[i].usage)<0)	
					{
						bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
						return BT_STATUS_FAILED;
					}
#ifdef __NORDIC_VOICE_COMMAND__
					if (tmpKeyAry->keyAry[i].usage == DTV_POWER_OFF_KEY)	
					{
						bPowerOff = TRUE;
					}
#endif
					tmpKeyAry->keyAry[i].usage = 0;
				}
			}
		}
	}
		
	return BT_STATUS_SUCCESS;
}

typedef struct
{
	U16		usage;
	U16		usageCollection;
}ReportedData;
 
U8 Find_Used_Usage(ReportedData* usageArray, U16 usage, U16 usageCollection, U8 reportSize)
{
	U8 i;
	for(i = 0; i<reportSize; i++)
	{
		if(usageArray[i].usage == usage && usageArray[i].usageCollection == usageCollection)
			break;
	}
	return i;
}


BtStatus Hid_Var_Report_To_Driver(int fd,U16 usage, S16 value, U16 usagePage, HidUsageCollection* collection)
{
	static ReportedData usageUsed[MAX_VAR_TABLE_SIZE] = {{0,0}};
	static U8 usageVarIndex = 0;
	//static U8 firstVar = 0;
	U8		reportDataIndex = 0;
	U16		tmpUsage;
	/*
	if(firstVar == 0)
	{
		usageUsed = (U8*)hid_malloc(reportSize*sizeof(U16));
		btmtk_os_memset(usageUsed, 0 ,reportSize*sizeof(U16));
		firstVar=1;
	}
	*/
	if(fd<0)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty hid fd");
		return BT_STATUS_FAILED;
	}
	
	if(HID_DEBUG_LEVEL_1)
		bt_prompt_trace(MOD_BT,"[HID]Handle usagePage %x in Hid_Var_Report_To_Driver", usagePage);

	if(usagePage == KEYBOARD_USAGEPAGE || usagePage == BUTTON_USAGEPAGE || usagePage == CONSUMER_USAGEPAGE)
	{
		if(value!=0)
		{
			if(HID_DEBUG_LEVEL_1)
				bt_prompt_trace(MOD_BT,"[HID]Report Var Usage %x",usage);
						
			if(Find_Used_Usage(usageUsed, usage, collection[0].usageIDValue, MAX_VAR_TABLE_SIZE) == MAX_VAR_TABLE_SIZE)
			{
				usageUsed[usageVarIndex].usage= usage;
				usageUsed[usageVarIndex].usageCollection = collection[0].usageIDValue;
				if(++usageVarIndex == MAX_VAR_TABLE_SIZE)
					usageVarIndex = 0;
			}
			
			if(usagePage == CONSUMER_USAGEPAGE)
			{
				tmpUsage = Find_Consumer_Usage_Index(usage);
				usage = tmpUsage;
				if(HID_DEBUG_LEVEL_1)
					bt_prompt_trace(MOD_BT,"[HID]Report Var  new pressed Consumer Usage %d",usage);
			}

			if(usagePage == BUTTON_USAGEPAGE)
			{
				if(collection[0].usageIDValue == HID_DEVICE_TYPE_GAMEPAD ||
					collection[0].usageIDValue == HID_DEVICE_TYPE_JOYSTICK)
				{
					usage += HID_GAMEPAD_INDEX_IN_DRIVER;
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]Send pressed Gamepad Button usage %d", usage);
				}
			}

			if(ioctl(fd,HID_KEY_PRESS, &usage)<0)	
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return BT_STATUS_FAILED;
			}

		}	
		else if(value == 0)
		{
			reportDataIndex = Find_Used_Usage(usageUsed, usage, collection[0].usageIDValue, MAX_VAR_TABLE_SIZE);
			if(reportDataIndex < MAX_VAR_TABLE_SIZE)
			{				
				usageUsed[reportDataIndex].usage = 0;
				usageUsed[reportDataIndex].usageCollection= 0;
				
				if(HID_DEBUG_LEVEL_1)
					bt_prompt_trace(MOD_BT,"[HID]Find_Used_Usage ok");
				
				if(usagePage == CONSUMER_USAGEPAGE)
				{
					tmpUsage = Find_Consumer_Usage_Index(usage);
					usage = tmpUsage;
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]Report Var  new released Consumer Usage %d",usage);
				}

				if(usagePage == BUTTON_USAGEPAGE)
				{
					if(collection[0].usageIDValue == HID_DEVICE_TYPE_GAMEPAD||
					collection[0].usageIDValue == HID_DEVICE_TYPE_JOYSTICK)
					{
						usage += HID_GAMEPAD_INDEX_IN_DRIVER;
						if(HID_DEBUG_LEVEL_1)
							bt_prompt_trace(MOD_BT,"[HID]Send released Gamepad Button usage %d", usage);
					}
				}

	   			if(ioctl(fd,HID_KEY_RELEASE, &usage)<0)	
				{
					bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
					return BT_STATUS_FAILED;
				}
			}
		}
	}
	else if(usagePage == GENERIC_DESKTOP_USAGEPAGE)
	{
		if(HID_DEBUG_LEVEL_1)
			bt_prompt_trace(MOD_BT,"[HID]device type is %d, usage is %x, value is %d", collection[0].usageIDValue, usage, value);
		switch(collection[0].usageIDValue)
		{
		case HID_DEVICE_TYPE_MOUSE:
			{
				switch(usage)
				{
				case POINTER_X_USAGE:
					{
						//value = value *2/3;
						if(ioctl(fd,HID_POINTER_X, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case POINTER_Y_USAGE:
					{
						//value = value *2/3;
						if(ioctl(fd,HID_POINTER_Y, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case WHEEL_USAGE:
					{
						if(ioctl(fd,HID_WHEEL, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
							return BT_STATUS_FAILED;
						}
					}	
					break;
				default:
					break;
				}
			}
			break;
		case HID_DEVICE_TYPE_GAMEPAD:
		case HID_DEVICE_TYPE_JOYSTICK:
			{
				switch(usage)
				{				
				case POINTER_X_USAGE:
					{
						if(ioctl(fd,HID_ABS_X, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_X");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case POINTER_Y_USAGE:
					{
						if(ioctl(fd,HID_ABS_Y, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_Y");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case POINTER_Z_USAGE:
					{
						if(ioctl(fd,HID_ABS_Z, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_Z");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case POINTER_RX_USAGE:
					{
						if(ioctl(fd,HID_ABS_RX, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_RX");
							return BT_STATUS_FAILED;
						}
					}
					break;
				case POINTER_RY_USAGE:
					{
						if(ioctl(fd,HID_ABS_RY, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_RY");
							return BT_STATUS_FAILED; 
						}
					}
					break;
				case POINTER_RZ_USAGE:
					{
						if(ioctl(fd,HID_ABS_RZ, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_RZ");
							return BT_STATUS_FAILED;
						}
					}
					break;
					
				case HAT_USAGE:
					{
						if(ioctl(fd,HID_ABS_HAT, &value)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl HID_ABS_HAT");
							return BT_STATUS_FAILED;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
	return BT_STATUS_SUCCESS;
}


void Hid_Report_Data(U8* data, U16 usage, U16 usagePage, U8 dataLen)
{
#ifdef __NORDIC_VOICE_COMMAND__
	if(usagePage == VENDOR_DEFINED_USAGEPAGE)
	{
		bt_prompt_trace(MOD_BT,"[HID]---------reportData for nordic-----------");
		ShowBytes(data,dataLen);
		//TODO: send data to APP
	}
	else
	{
#endif	
	//TODO: send data to driver
#ifdef __NORDIC_VOICE_COMMAND__
	}
#endif	
}


S32 Hid_Interpreter(int fd,HidReportConstructor *hrc, U8 *reportData, U16 data_len,HidReportType reportType)
{
	U8	reportID;
	U8	unitIndex;
	U8	reportIndex;
	//U8	*reportData;
	U8	*tmpReportData;
	U8	*data_8;
	S16	data_16 = 0;
	S16	value;
	U8 	bitIndex;
	U8 	byteIndex;
	U32	maskBase;
	U8 	byteChange = 1;
	HidReportUnit*	tmpReportUnit;
	U16	usage=0;
	U16 returnUsage=0;
	int	size = 0;
	U8	i = 0;
	U16 LogicalBitSize = 0;
	HidReportFormat *reportFormat;

	//btmtk_os_memset(&tmpReportUnit, 0 , sizeof(tmpReportUnit));
	if(!hrc )
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty report constructor");
		return -1;
	}	
	
	if(!reportData)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty report data");
		return -1;
	}
	if(reportType != HID_REPORT_INPUT &&
		reportType != HID_REPORT_OUTPUT &&
		reportType != HID_REPORT_FEATURE)
	{
		bt_prompt_trace(MOD_BT,"[HID]Other report type");
		return -1;
	}
	//reportData = (U8*)hid_malloc(data_len);
	tmpReportData = (U8*)hid_malloc(data_len);
	//&data_8 = (S8*)hid_malloc(sizeof(S8));
	//&data_16 = (S16*)hid_malloc(sizeof(S16));
	//btmtk_os_memcpy(reportData, data, data_len);
	if(HID_DEBUG_LEVEL_1)
	{
		bt_prompt_trace(MOD_BT,"[HID]---------reportData-----------");
		ShowBytes(reportData,data_len);
	}

	if(hrc[0].reportFormat != NULL)
		reportID = 0;
	else
		btmtk_os_memcpy(&reportID,reportData,1);

	if(HID_DEBUG_LEVEL_1)
	{
		bt_prompt_trace(MOD_BT,"[HID]report ID=%d",reportID);
		bt_prompt_trace(MOD_BT,"[HID]hrc[reportID].report ID=%d",hrc[reportID].reportID);
	}
	if(hrc[reportID].reportID!=reportID)
	{	
		bt_prompt_trace(MOD_BT,"[HID]Invalid report ID");
		goto fail;
	}

	reportFormat = &hrc[reportID].reportFormat[reportType-1];
	if(reportFormat == NULL){
		bt_prompt_trace(MOD_BT,"[HID] &hrc[reportID].reportFormat[reportType-1] == NULL");
		return -1;
	}

	for(unitIndex = 0; unitIndex<reportFormat->unitNum; unitIndex++)
	{		
		tmpReportUnit=&reportFormat->reportUnit[unitIndex];
		
		if(HID_DEBUG_LEVEL_1)
		{
			//bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit=%x",tmpReportUnit);
			bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->hidUsage[0].usagepage:%x",tmpReportUnit->hidUsage[0].usagepage);
		}
		if(tmpReportUnit == NULL)
		{
			bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit == null");
			continue;
		}

		if(tmpReportUnit->hidUsage == NULL)
		{
			bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->hidUsage == null");
			continue;
		}
		
		if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_NONE 
			|| !(tmpReportUnit->hidUsage[0].usagepage ==KEYBOARD_USAGEPAGE
#ifdef MOUSE_SUPPORT
				|| tmpReportUnit->hidUsage[0].usagepage ==BUTTON_USAGEPAGE
				|| tmpReportUnit->hidUsage[0].usagepage ==GENERIC_DESKTOP_USAGEPAGE
#endif
#ifdef CONSUMER_SUPPORT
				|| tmpReportUnit->hidUsage[0].usagepage ==CONSUMER_USAGEPAGE
#endif
#ifdef __NORDIC_VOICE_COMMAND__
				|| tmpReportUnit->hidUsage[0].usagepage ==VENDOR_DEFINED_USAGEPAGE
#endif
				))
		{
			
			if(HID_DEBUG_LEVEL_1)
				bt_prompt_trace(MOD_BT,"[HID]empty usage or not keyboard/button/pointer data");
			continue;
		}
		//btmtk_os_memcpy(&tmpReportUnit, &hrc[reportID].reportFormat[reportType-1].reportUnit[unitIndex], sizeof(HidReportUnit));
		
		bitIndex = 0;
		byteIndex = 0;	
		byteChange = 1;
		size = (int)((tmpReportUnit->reportCount*tmpReportUnit->reportSize +8 -1) /8);
		//maskBase=pow(2, tmpReportUnit->reportSize) -1;
		maskBase = (1 << tmpReportUnit->reportSize) - 1;
		
		if(HID_DEBUG_LEVEL_1)
		{
			bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->reportCount*tmpReportUnit->reportSize:%d", tmpReportUnit->reportCount*tmpReportUnit->reportSize);			
			bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->offset:%d", tmpReportUnit->offset);
		}
		if((tmpReportUnit->reportCount*tmpReportUnit->reportSize) >= 8)
		{
			U8 offsetByteSize = tmpReportUnit->offset / 8;
			//btmtk_os_memcpy(tmpReportData, reportData+1+offsetByteSize, size);
			btmtk_os_memcpy(tmpReportData, reportID == 0?reportData+offsetByteSize:reportData+1+offsetByteSize, size);
		}
		else
		{
			U8 offsetByteSize = tmpReportUnit->offset / 8;
			U32 tmpMask = (1 << tmpReportUnit->reportCount*tmpReportUnit->reportSize) - 1;
			//*tmpReportData = (*(reportData+1+offsetByteSize) >> tmpReportUnit->offset % 8) & tmpMask;
			*tmpReportData = (*(reportID == 0?reportData+offsetByteSize:reportData+1+offsetByteSize) >> tmpReportUnit->offset % 8) & tmpMask;
		}
		
		if(HID_DEBUG_LEVEL_1)
		{
			bt_prompt_trace(MOD_BT,"[HID]---------tmpReportData-----------");
			ShowBytes(tmpReportData, size);
		}
		for(reportIndex = 0; reportIndex<tmpReportUnit->reportCount; reportIndex++)
		{
			if(tmpReportUnit->hidUsage[reportIndex].usageCountRemain != 0)
			{
				if(HID_DEBUG_LEVEL_1)
				{
					bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->hidUsage[reportIndex].usageCountRemain = %d", tmpReportUnit->hidUsage[reportIndex].usageCountRemain);
				}
				int dataLen = (tmpReportUnit->reportSize * (tmpReportUnit->hidUsage[reportIndex].usageCountRemain + 1) -1)/8+1;
				data_8 = (U8*)hid_malloc(dataLen * sizeof(U8));
				btmtk_os_memcpy(data_8, tmpReportData+byteIndex, dataLen);
				usage=tmpReportUnit->hidUsage[reportIndex].usageID.usageIDValue;
				Hid_Report_Data(data_8, usage, tmpReportUnit->hidUsage[0].usagepage, dataLen);

				break;
			}
			if(tmpReportUnit->reportSize>8)
			{			
				if(bitIndex>=8)
				{
					byteIndex+=bitIndex/8;
					bitIndex = 0;
					byteChange = 1;
					hid_free(data_8);
					data_8 = NULL;
				}
				if(byteChange == 1)
				{
					data_8 = (U8*)hid_malloc((tmpReportUnit->reportSize-1)/8+1);
					btmtk_os_memcpy(data_8, tmpReportData+byteIndex,((tmpReportUnit->reportSize-1)/8+1));
					byteChange = 0;
				}			
				
				if(reportIndex == 0)
				{
					//data_16 = (*data_8 & 0x00FF) | ((U16)((*(data_8+1)) & (0xFF << (8 - (tmpReportUnit->reportSize % 8)))) << (tmpReportUnit->reportSize % 8));
					int tmp_offset=((tmpReportUnit->reportSize % 8)==0?8:tmpReportUnit->reportSize % 8);
					//data_16 = ((U16)*data_8 << tmp_offset) | ((U16)((*(data_8+1)) & (0xFF << (8 - tmp_offset))) >> (8 - tmp_offset));
					data_16 = (U16)((*(data_8+1)) & (0xFF >> (8 - tmp_offset))) << 8 | *data_8;
					
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]---------[1]data_16=%x-----------",data_16);
				}
				else
				{
					//data_16 = (*data_8 & (0xFF >> (tmpReportUnit->reportSize % 8)) & 0x00FF) | ((U16)*(data_8+1) << (8 - (tmpReportUnit->reportSize % 8)));
				//	data_16 = ((U16)(*data_8 & (0xFF >> (tmpReportUnit->reportSize % 8))) << 8) | ((U16)*(data_8+1) & 0x00FF);
					data_16 = (*data_8 >> (tmpReportUnit->reportSize % 8)) | ((U16)*(data_8+1) << (8 - (tmpReportUnit->reportSize % 8)));
					
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]---------[2]data_16=%x-----------",data_16);
				}

				value = data_16;
				
				if(HID_DEBUG_LEVEL_1)
					bt_prompt_trace(MOD_BT,"[HID]---------value=%d-----------",value);
			}
			else 	if(tmpReportUnit->reportSize<=8)
			{			
				if(bitIndex>=8)
				{
					bitIndex = 0;
					byteIndex+=1;
					byteChange = 1;
					hid_free(data_8);
					data_8 = NULL;
				}
				if(byteChange == 1)
				{
					data_8 = (U8*)hid_malloc(1);
					btmtk_os_memcpy(data_8, tmpReportData+byteIndex,1);
					
					if(HID_DEBUG_LEVEL_1)
					{
						bt_prompt_trace(MOD_BT,"[HID]---------data_8 = %2x-----------",*data_8);
						//ShowBytes(&data_8,1);
					}
					byteChange = 0;
				}
				value = (*data_8&(maskBase<<bitIndex))>>bitIndex;
			}
			bitIndex+=tmpReportUnit->reportSize;
			if(tmpReportUnit->reportSize >= 8 && tmpReportUnit->hidUsage[0].usagepage != KEYBOARD_USAGEPAGE)
			{
				if(HID_DEBUG_LEVEL_1)
					bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->logicalMIN = %d", tmpReportUnit->logicalMIN);
					
				if(tmpReportUnit->logicalMIN != 0)
				{
					LogicalBitSize = log(tmpReportUnit->logicalMAX)/log(2) + 2;
					if(value >= (1 << (LogicalBitSize -1) ))
					{
						value -= 1 << LogicalBitSize;	
					}
				}
				else
				{
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->logicalMIN == 0");
				}
			}

/*
			if(value>tmpReportUnit->logicalMAX || value<tmpReportUnit->logicalMIN)
			{
				bt_prompt_trace(MOD_BT,"[HID]Wrong report data");
				bt_prompt_trace(MOD_BT,"[HID]Wrong report data is:-----------%d",value);

				continue;
			}
*/
			if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_ID_SINGLE)
			{
				if(tmpReportUnit->arrayAttr == HID_ARRAY_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[value -tmpReportUnit->logicalMIN].usageID.usageIDValue;
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]HID_USAGE_ID_SINGLE value=%d",value);
					Hid_Ary_Report_To_Driver(fd, reportID, usage, tmpReportUnit->reportCount, tmpReportUnit->hidUsage[0].usagepage);
				}
				else if(tmpReportUnit->arrayAttr == HID_VAR_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[reportIndex].usageID.usageIDValue;
					Hid_Var_Report_To_Driver(fd,usage, value, tmpReportUnit->hidUsage[0].usagepage, tmpReportUnit->hidUsage[0].usageCollection);
				}
			}
			else if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_ID_RANGE)
			{					
				if(tmpReportUnit->arrayAttr == HID_ARRAY_ATTRIBUTE)
				{
					if(HID_DEBUG_LEVEL_1)
						bt_prompt_trace(MOD_BT,"[HID]HID_USAGE_ID_RANGE value=%d",value);
					//usage=tmpReportUnit->hidUsage[0].usageID.usageRange.usageMIN + value -tmpReportUnit->logicalMIN;
					Hid_Ary_Report_To_Driver(fd, reportID, value, tmpReportUnit->reportCount, tmpReportUnit->hidUsage[0].usagepage);
				}
				else if(tmpReportUnit->arrayAttr == HID_VAR_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[0].usageID.usageRange.usageMIN + reportIndex;
					Hid_Var_Report_To_Driver(fd,usage, value, tmpReportUnit->hidUsage[0].usagepage, tmpReportUnit->hidUsage[0].usageCollection);
				}
			}
			if(tmpReportUnit->hidUsage[0].usagepage ==KEYBOARD_USAGEPAGE && 
				(usage==KEY_NUM_LOCK_USAGE||usage==KEY_CAPS_LOCK_USAGE||usage==KEY_SCROLL_LOCK_USAGE))
			{
				returnUsage=usage;				
			}
		}
		hid_free(data_8);
		data_8 = NULL;
	}

#ifdef __NORDIC_VOICE_COMMAND__
	if (bPowerOff)
	{
		returnUsage = 0xff;
	}
#endif

fail:
	//btmtk_os_memset(&tmpReportUnit, 0 , sizeof(tmpReportUnit));
	//btmtk_os_memset(reportData, 0, data_len);
	btmtk_os_memset(tmpReportData, 0, data_len);
	//hid_free(reportData);
	hid_free(tmpReportData);
	tmpReportUnit=NULL;

	return returnUsage;
}

