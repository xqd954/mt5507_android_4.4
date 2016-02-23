/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BTSNIFF"

#define bt_tst_err(...) ALOGE(__VA_ARGS__)
#define bt_tst_dbg(...) ALOGD(__VA_ARGS__)

#define MAX_PATH 256


//static unsigned char g_buffer[1024*16] __attribute__ ((aligned (__BIGGEST_ALIGNMENT__)));

static unsigned int g_enableLog = 0;
static int g_hSnifferLogFile = -1;

int init();
void deinit();
static int dbgWrite(const unsigned char *buf, int count);

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to create a log file.
*
* \param none
*
* \retval ERROR_SUCCESS If create the log file successfully
*/
/*----------------------------------------------------------------------------*/
static int dbgFileCreate(void)
{
    int             status = 0;
    char           fileName[MAX_PATH];
    time_t  t = time(NULL);
    struct tm      *pTm = localtime(&t);
	
    sprintf(fileName, "/data/@btmtk/hci_sniffer_log_%04d%02d%02d%02d%02d%02d.log",
        (int)pTm->tm_year+1900,
        (int)pTm->tm_mon+1,
        (int)pTm->tm_mday,
        (int)pTm->tm_hour,
        (int)pTm->tm_min,
        (int)pTm->tm_sec);
    bt_tst_dbg("open file(%s)", fileName);
    // Create the log file always.
    g_hSnifferLogFile = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    if(g_hSnifferLogFile < 0){
    	bt_tst_dbg("Open file failed : errno=%d, %s", errno, strerror(errno));
    	return -1;
    }else{
        int     bytesWritten;
        unsigned char   fileId[] = { 0x62, 0x74, 0x73, 0x6e, 0x6f, 0x6f, 0x70, 0x00};
        unsigned char   fileVer[] = { 0x00, 0x00, 0x00, 0x01};
        unsigned char   fileType[] = { 0x00, 0x00, 0x03, 0xEA};
        
        /* magic word */
        bytesWritten = dbgWrite(fileId, sizeof(fileId));
        if(bytesWritten != sizeof(fileId)){
            bt_tst_dbg("write fileId failed");
            return -1;
        }

        /* version 1 */
        bytesWritten = dbgWrite(fileVer, sizeof(fileVer));
        if(bytesWritten != sizeof(fileVer)){
            bt_tst_dbg("write fileVer failed");
            return -1;
        }

        /* 1001 H4 */
        bytesWritten = dbgWrite(fileType, sizeof(fileType));
        if(bytesWritten != sizeof(fileType)){
            bt_tst_dbg("write fileType failed");
            return -1;
        }
    }
    return status;
} /* dbgFileCreate */


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to close a log file.
*
* \param none
*
* \retval ERROR_SUCCESS If close the log file successfully
*/
/*----------------------------------------------------------------------------*/
static int dbgFileClose ()
{
    int status = 0;

    if (g_hSnifferLogFile >= 0) 
    {
        close(g_hSnifferLogFile);
        g_hSnifferLogFile = -1;
    }
    return status;
} /* dbgFileCreate */

static int dbgWrite(const unsigned char *buf, int count){
    int nWritten = 0;
    int bytesToWrite;

    if(g_hSnifferLogFile < 0)
    {
        bt_tst_err("g_hSnifferLogFile is not opened");
        return -1;
    }

    bytesToWrite = count;    
    while(bytesToWrite > 0)
    {
        //bt_tst_dbg("dbgWrite buffer=%p, bytesToWrite=%d", buf, bytesToWrite);
        nWritten = write(g_hSnifferLogFile, buf, bytesToWrite);
        if (nWritten > 0)
        {
            //bt_tst_dbg("dbgWrite %d bytes are written to uart", nWritten);
            bytesToWrite -= nWritten;
            buf += nWritten;
        }
        else
        {
            bt_tst_err("dbgWrite failed : nWritten=%d, err=%s, errno=%d", nWritten, strerror(errno), errno);
            break;
        }
    }
    //bt_tst_dbg("dbgWrite total wrote %d bytes", count - bytesToWrite);
    return (count - bytesToWrite);

}


static uint64_t gettimestamp() {
    struct timeval tv;
    unsigned long long result = 0/*EPOCH_DIFF*/;
    gettimeofday(&tv,NULL);
    result += (tv.tv_sec-946684800ll);
    result *= 1000000LL;
    result += tv.tv_usec;
    return result;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to write HCI packets to a log file.
*
* \param[in] debugStr printf style format string
* \param[in] ... value for format string
*
* \retval ERROR_SUCCESS If log the message successfully
*/
/*----------------------------------------------------------------------------*/
static int dbgLogRaw (unsigned int fgSend, unsigned char *debugRaw, int logLen, int totallen)
{
    int bytesWritten = 0;
    int result = 0;
    unsigned char   ucTemp[24];
    uint64_t fileTime;
    uint64_t ulResultTime;
    uint64_t time2000J1, timeCurrent, diff;

    if (g_hSnifferLogFile < 0) 
    {
        return -1;    
    }

    //Original Length
    ucTemp[0] = (unsigned char)(totallen >> 24); 
    ucTemp[1] = (unsigned char)(totallen >> 16);
    ucTemp[2] = (unsigned char)(totallen >> 8);
    ucTemp[3] = (unsigned char)(totallen);
    
    //Included Length
    ucTemp[4] = (unsigned char)(totallen >> 24); 
    ucTemp[5] = (unsigned char)(totallen >> 16);
    ucTemp[6] = (unsigned char)(totallen >> 8);
    ucTemp[7] = (unsigned char)(totallen);

    //Packet Flags
    ucTemp[8] = 0;                        
    ucTemp[9] = 0;
    ucTemp[10] = 0;
    if(fgSend){
        ucTemp[11] = 0;
    }else{
        ucTemp[11] = 1;
    }

    //Cumulative Drops
    ucTemp[12] = 0;                        
    ucTemp[13] = 0;
    ucTemp[14] = 0;
    ucTemp[15] = 0;

    //in us
    //time2000J1.u.LowPart = 0x83be2000;
    //time2000J1.u.HighPart = 0x002cbb97;
    //time2000J1 = 0x002cbb9783be2000ll;

    //current time
    timeCurrent = gettimestamp();
    bt_tst_dbg("ts=%llu", timeCurrent);
    //diff.QuadPart = timeCurrent.QuadPart - time2000J1.QuadPart;
    //diff = timeCurrent - time2000J1;
	
    //ulResultTime.u.LowPart = 0x4A676000;//since Jan. 1, 0, AD in unit of us
    //ulResultTime.u.HighPart = 0x00E03AB4;
    ulResultTime = 0x00E03AB44A676000ll + timeCurrent;
    //ulResultTime.QuadPart += diff.QuadPart;
    //ulResultTime += diff;

    //Timestamp Microseconds high part
    /*
    ucTemp[16] = (unsigned char)(ulResultTime.u.HighPart >> 24);  
    ucTemp[17] = (unsigned char)(ulResultTime.u.HighPart >> 16);
    ucTemp[18] = (unsigned char)(ulResultTime.u.HighPart >> 8);
    ucTemp[19] = (unsigned char)(ulResultTime.u.HighPart);
    */
    ucTemp[16] = (unsigned char)((ulResultTime >> 56) & 0xFF);
    ucTemp[17] = (unsigned char)((ulResultTime >> 48) & 0xFF);
    ucTemp[18] = (unsigned char)((ulResultTime >> 40) & 0xFF);
    ucTemp[19] = (unsigned char)((ulResultTime >> 32) & 0xFF);
    
    //Timestamp Microseconds low part
    /*
    ucTemp[20] = (unsigned char)(ulResultTime.u.LowPart >> 24);       
    ucTemp[21] = (unsigned char)(ulResultTime.u.LowPart >> 16);
    ucTemp[22] = (unsigned char)(ulResultTime.u.LowPart >> 8);
    ucTemp[23] = (unsigned char)(ulResultTime.u.LowPart);
    */
    ucTemp[20] = (unsigned char)((ulResultTime >> 24) & 0xFF);
    ucTemp[21] = (unsigned char)((ulResultTime >> 16) & 0xFF);
    ucTemp[22] = (unsigned char)((ulResultTime >> 8) & 0xFF);
    ucTemp[23] = (unsigned char)((ulResultTime) & 0xFF);
    bt_tst_dbg("ucTemp[16~23]=%u, %u, %u, %u, %u, %u, %u, %u", 
                       ucTemp[16],
                       ucTemp[17],
                       ucTemp[18],
                       ucTemp[19],
                       ucTemp[20],
                       ucTemp[21],
                       ucTemp[22],
                       ucTemp[23]);

    bytesWritten = dbgWrite(ucTemp, sizeof(ucTemp));
    if(bytesWritten < 0){
        deinit();
    }

    /* Write message log to log file */
    bytesWritten = dbgWrite(debugRaw, logLen);
    if(bytesWritten < 0){
        deinit();
    }

    //FlushFileBuffers(g_hSnifferLogFile);//if cache loss is critical, can be turn on.
    return 0;
} /* dbgLogWr */

int init(void){
    //char val[PATH_MAX] = {0};
    //int vallen = property_get("persist.bt.virtualsniff", val, "0");
    //bt_tst_dbg("persist.bt.virtualsniff : len=%d, %s", vallen, val);
    //if(vallen > 0 && val[0] != '0'){
    if(dbgFileCreate() < 0){
        bt_tst_err("dbgFileCreate failed");
        deinit();
        return -1;
    }else{
        g_enableLog = 1;
    }
    //}
    return 0;
}

void deinit(void){
    bt_tst_err("deinit");
    dbgFileClose();
    g_enableLog = 0;
}

int writePacket(unsigned char *pBuff, unsigned int len){
    int ret = -1;
    if(g_enableLog){
        ret = dbgLogRaw(1, pBuff, len, len);
    }
    return ret;
}

int readPacket(unsigned char type, unsigned char *pBuff, unsigned int len)
{
    int ret = -1;
    /* including UART hdr */
    if(g_enableLog){
        ret = dbgLogRaw(0, &type, 1, len+1);
        if(ret >= 0){
            ret = dbgWrite(pBuff,len);
        }
    }
    return ret;
}

