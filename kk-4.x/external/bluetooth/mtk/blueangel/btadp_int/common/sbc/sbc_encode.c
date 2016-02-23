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

/****************************************************************************
 *
 * File:
 *     $Workfile:sbc_encode.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the SBC encoder application.
 *             
 * Created:     January 18, 2005
 *
 * Copyright 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/
#include "sbcconfig.h"
#include "sbc.h"
#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

/* Function Prototypes */     
static XaStatus ParseStandardOptions(char **opt, SbcStreamInfo *info, U8 *parmFile);
static void PrintHelp(void);
static XaStatus ParseParms(U8 *fNameParms, SbcStreamInfo *info);

/* Constants */
#define SBC_BUFF_SIZE 2048
#define PCM_BUFF_SIZE 8192

#define BT_DB_PATH "\\@btmtk\\data"
#define BT_A2DP_SRC48_FILE BT_DB_PATH"\\a2dp48.wav"
#define BT_A2DP_SRC44_FILE BT_DB_PATH"\\a2dp44.wav"
#define BT_A2DP_SRC32_FILE BT_DB_PATH"\\a2dp32.wav"
#define BT_A2DP_SRC16_FILE BT_DB_PATH"\\a2dp16.wav"

#define BT_A2DP_SBC_FILE BT_DB_PATH"\\a2dp.sbc"

extern void OS_MemSet(U8 *dest, U8 byte, U32 len);
extern void translateFilePath(const U8 *refPath, U8 *absPathBuf);

/*---------------------------------------------------------------------------
 * main()
 *
 *     Main routine
 */
U8          sbcBuffer[SBC_BUFF_SIZE];
U8          pcmBuffer[PCM_BUFF_SIZE];


int encodec_sbc(
        U8 min_bit_pool,
        U8 block_len, // b0: 16, b1: 12, b2: 8, b3: 4
        U8 subband_num, // b0: 8, b1: 4
        U8 alloc_method, // b0: loudness, b1: SNR
        U8 sample_rate, // b0: 48000, b1: 44100, b2: 32000, b3: 16000
        U8 channel_mode // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
        )
{
    FILE       *wavIn, *sbcOut;
    SbcPcmData  pcmData;
    SbcEncoder  encoder;
    U16         bytesToRead;
    U16         bytesToEncode;
    U16         bytesEncoded;
    XaStatus    status;
    U8          *fNameIn;

    U8          *fNameOut;
    U32         sampleFreq = 0;
    U32         chunk2Size = 0;
    U16         sbcLen;
    U8 pathBuf[_MAX_PATH] = {0};
    int         encodedFrameLen;

    /* Initialize default encoder settings */
    SBC_InitEncoder(&encoder);
    fNameOut = BT_A2DP_SBC_FILE;
    switch(sample_rate)
    {
        case 0x01: /* 48.0K*/
            fNameIn = (U8 *)&BT_A2DP_SRC48_FILE;
            break;
        case 0x02:
            fNameIn = (U8 *)&BT_A2DP_SRC44_FILE;
            break;
        case 0x04:
            fNameIn = (U8 *)&BT_A2DP_SRC32_FILE;
            break;
        case 0x08:
            fNameIn = (U8 *)&BT_A2DP_SRC16_FILE;
            break;

    }
    switch(block_len)
    {
        case 0x01:
            encoder.streamInfo.numBlocks = 16;
            break;
        case 0x02:
            encoder.streamInfo.numBlocks = 12;
            break;
        case 0x04:
            encoder.streamInfo.numBlocks = 8;
            break;
        case 0x08:
            encoder.streamInfo.numBlocks = 4;
            break;
    }
    switch(subband_num)
    {
        case 0x01:
            encoder.streamInfo.numSubBands = 8;
            break;
        case 0x02:
            encoder.streamInfo.numSubBands = 4;
            break;
    }
    switch(alloc_method)
    {
        case 0x01:
            encoder.streamInfo.allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
            break;
        case 0x02:
            encoder.streamInfo.allocMethod = SBC_ALLOC_METHOD_SNR;
            break;
    }
    switch(channel_mode)
    {
        case 0x01:
            encoder.streamInfo.channelMode = SBC_CHNL_MODE_JOINT_STEREO;
            break;
        case 0x02:
            encoder.streamInfo.channelMode = SBC_CHNL_MODE_STEREO;
            break;
        case 0x04:
            encoder.streamInfo.channelMode = SBC_CHNL_MODE_DUAL_CHNL;
            break;
        case 0x08:
            encoder.streamInfo.channelMode = SBC_CHNL_MODE_MONO;
            break;
    }

    encoder.streamInfo.bitPool = min_bit_pool;    
#if 0        
    encoder.streamInfo.numBlocks = 16;
    encoder.streamInfo.numSubBands = 8;
    encoder.streamInfo.allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
    encoder.streamInfo.channelMode = SBC_CHNL_MODE_JOINT_STEREO;
    encoder.streamInfo.bitPool = 35;
#endif

    /* Open the WAV file */
    memset(pathBuf, 0, sizeof(pathBuf));
    translateFilePath(fNameIn, pathBuf);
    wavIn = fopen(pathBuf, "rb");
    if(wavIn == NULL)
        return -1;
    memset(pathBuf, 0, sizeof(pathBuf));
    OS_MemSet(pathBuf, 0, sizeof(pathBuf));
    translateFilePath(fNameOut, pathBuf);
    sbcOut = fopen(pathBuf, "wb");
    if(sbcOut == NULL)
        return -1;    

    /**** Parse the WAV header ****/
    if (fread(pcmBuffer, 1, 4, wavIn) == 4) {
        if (memcmp(pcmBuffer, "RIFF", 4) != 0) {
            printf("Invalid RIFF file format\n");
            return -1;
        }
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }

    /* Read format */
    fseek(wavIn, 8, 0);
    if (fread(pcmBuffer, 1, 4, wavIn) == 4) {
        if (memcmp(pcmBuffer, "WAVE", 4) != 0) {
            printf("Invalid WAV format\n");
            return -1;
        }
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }

    /**** Start subchunk 1 ****/
    fseek(wavIn, 12, 0);
    if (fread(pcmBuffer, 1, 4, wavIn) == 4) {
        if (memcmp(pcmBuffer, "fmt", 3) != 0) {
            printf("Invalid format chunk\n");
            return -1;
        }
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }
    
    /* PCM */
    fseek(wavIn, 20, 0);
    if (fread(pcmBuffer, 1, 2, wavIn) == 2) {
        if (LEtoHost16(pcmBuffer) != 1) {
            printf("Unsupported audio format\n");
            return -1;
        }
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }

    /* Channels */
    if (fread(pcmBuffer, 1, 2, wavIn) == 2) {
        encoder.streamInfo.numChannels = pcmBuffer[0];
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }

    /* Sample frequency */
    if (fread(pcmBuffer, 1, 4, wavIn) == 4) {

        sampleFreq = LEtoHost32(pcmBuffer);

        switch (sampleFreq) {
        case 48000:
            encoder.streamInfo.sampleFreq = SBC_CHNL_SAMPLE_FREQ_48;
            break;
        case 44100:
            encoder.streamInfo.sampleFreq = SBC_CHNL_SAMPLE_FREQ_44_1;
            break;
        case 32000:
            encoder.streamInfo.sampleFreq = SBC_CHNL_SAMPLE_FREQ_32;
            break;
        case 16000:
            encoder.streamInfo.sampleFreq = SBC_CHNL_SAMPLE_FREQ_16;
            break;
        default:
            printf("Unsupported sampling frequency %d\n", sampleFreq);
            return -1;
        }
    } else {
        printf("Error reading WAV file\n");
        return -1;
    }
    /* Bits per sample */
    fseek(wavIn, 34, 0);
    if (fread(pcmBuffer, 1, 2, wavIn) == 2) {
        if (LEtoHost32(pcmBuffer) != 16) {
            printf("Unsupported sample size\n");
        }
    }

    /**** Data chunk ****/
    fseek(wavIn, 44, 0);

    /* Print out SBC format */
    printf("Start\n");
    printf("Sampling Frequency = %d\n", sampleFreq);

    if (encoder.streamInfo.numChannels == 1) {
        if (encoder.streamInfo.channelMode != SBC_CHNL_MODE_MONO) {
            printf("Switching to mono mode...\n");
        }
        encoder.streamInfo.channelMode = SBC_CHNL_MODE_MONO;
        printf("Mode = Mono\n");
    } else {
        switch (encoder.streamInfo.channelMode) {
        case SBC_CHNL_MODE_JOINT_STEREO:
            printf("Mode = Joint Stereo\n");
            break;
        case SBC_CHNL_MODE_STEREO:
            printf("Mode = Stereo\n");
            break;
        case SBC_CHNL_MODE_DUAL_CHNL:
            printf("Mode = Dual Channel\n");
            break;
        default:
            printf("Mode = Unknown\n");
        }
    }

    printf("Blocks = %d\n", encoder.streamInfo.numBlocks);
    printf("Subbands = %d\n", encoder.streamInfo.numSubBands);

    if (encoder.streamInfo.allocMethod == SBC_ALLOC_METHOD_SNR) {
        printf("Allocation = SNR\n");
    } else if(encoder.streamInfo.allocMethod == SBC_ALLOC_METHOD_LOUDNESS) {
        printf("Allocation = LOUDNESS\n");
    } else {
        printf("Allocation = Unknown\n");
    }

    printf("Bitpool = %d\n", encoder.streamInfo.bitPool);

    /* Initialize PCM data structure */
    pcmData.data = pcmBuffer;
    bytesToRead = encoder.streamInfo.numChannels * 
                  encoder.streamInfo.numSubBands * 
                  encoder.streamInfo.numBlocks * 2;
    bytesToRead = (PCM_BUFF_SIZE / bytesToRead) * bytesToRead;

    /* Start reading the SBC data and encoding */
    while ((bytesToEncode = fread(pcmBuffer, 1, bytesToRead, wavIn))) {

        pcmData.data = pcmBuffer;
        pcmData.dataLen = bytesToEncode;

        while (bytesToEncode) {

            /* Encode the PCM buffer into the SBC buffer */
            status = SBC_EncodeFrames(&encoder, &pcmData, &bytesEncoded, 
                                      sbcBuffer, &sbcLen, SBC_BUFF_SIZE);

            bytesToEncode -= bytesEncoded;
            pcmData.data += bytesEncoded;
            pcmData.dataLen -= bytesEncoded;

            if (status == XA_STATUS_SUCCESS) {

                /* Block encoded */
                if (fwrite(sbcBuffer, 1, sbcLen, sbcOut) != sbcLen) {
                    printf("Error writing SBC data\n");
                    return -1;
                }

            } else if (status != XA_STATUS_CONTINUE) {
                /* Parsing error */
                printf("Error encoding SBC stream\n");
                return -1;
            } else {
                /* Not enough data left to encode a frame */
                break;
            }
        }
    }

    printf("Done\n");
    fclose(wavIn);
    fclose(sbcOut);
    encodedFrameLen = SBC_FrameLen(&encoder.streamInfo);
    return encodedFrameLen;
}

/*---------------------------------------------------------------------------
 * PrintHelp()
 *
 *     Print out help message.
 */
void PrintHelp(void)
{
    printf("Usage: sbc_encode [-b<blocklen>] [-s<subbands>] [-p<bitpool>]\n"
           "                  [-m<STEREO|JOINT|DUAL>] [-a<SNR|LOUD>]\n"
           "                  [-P<parm file>\n"
           "                  <WAV file> [SBC file]\n");
}

/*---------------------------------------------------------------------------
 * ParseStandardOptions()
 *
 *     Parses option pointed to by 'opt' parameter. If the parameter is
 * recognized, the 'opt' pointer is advanced past the option.
 */
static XaStatus ParseStandardOptions(char **opt, SbcStreamInfo *info, U8 *parmFile)
{
    char       *cmd = *opt;
    BOOL        done = FALSE;

    switch (*cmd++) {
    case '/':
    case '-':
        /* Look for a normal switch */
        switch (*cmd++) {
        case 'b':
            info->numBlocks = atoi(cmd);

            switch (info->numBlocks) {
            case 4:
            case 8:
            case 12:
            case 16:
                break;
            default:
                printf("Invalid block length: %d\n", info->numBlocks);
                *opt = cmd;
                return XA_STATUS_FAILED;
            }

            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;

        case 's':
            info->numSubBands = atoi(cmd);

            switch (info->numSubBands) {
            case 4:
            case 8:
                break;
            default:
                printf("Invalid number of subbands: %d\n", info->numSubBands);
                *opt = cmd;
                return XA_STATUS_FAILED;
            }

            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;

        case 'p':
            info->bitPool = atoi(cmd);

            if ((info->bitPool < 2) || (info->bitPool > 250)) {
                printf("Invalid bitpool size: %d\n", info->bitPool);
                *opt = cmd;
                return XA_STATUS_FAILED;
            }

            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;

       case 'm':
            if (*cmd == 'S') {
                info->channelMode = SBC_CHNL_MODE_STEREO;
            } else if (*cmd == 'J') {
                info->channelMode = SBC_CHNL_MODE_JOINT_STEREO;
            } else if (*cmd == 'D') {
                info->channelMode = SBC_CHNL_MODE_DUAL_CHNL;
            } else{
                printf("Invalid mode: %s\n",cmd);
                *opt = cmd;
                return XA_STATUS_FAILED;
            }

            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;

        case 'a':
            if (*cmd == 'S') {
                info->allocMethod = SBC_ALLOC_METHOD_SNR;
            } else if (*cmd == 'L') {
                info->allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
            } else {
                printf("Invalid allocation method: %s\n",cmd);
                *opt = cmd;
                return XA_STATUS_FAILED;
            }

            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;

        case 'P':
            strcpy(parmFile, cmd);

            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;

        default:
            /* Unrecognized - or / switch */
            printf("Unrecognized command line switch: %c\n",*(cmd-1));
            return XA_STATUS_FAILED;
        }
        break;

    default:
        /* Done with args */
        *opt = cmd-1;
        return XA_STATUS_SUCCESS;
    }

    /* We survived the parse, return success */
    *opt = cmd;
    return XA_STATUS_PENDING;
}

/*---------------------------------------------------------------------------
 * ParseParms()
 *
 * Parses a parameter file.
 */
static XaStatus ParseParms(U8 *fNameParms, SbcStreamInfo *info)
{
    FILE     *parmsIn;
    XaStatus  status = XA_STATUS_SUCCESS;
    U8        line[256];
    U8       *ptr;

    parmsIn = fopen(fNameParms, "rb");
    if (parmsIn) {
        /* Ignore all lines till the parms file starts */
        while ((ptr = fgets(line, 256, parmsIn))) {
            if (strncmp(ptr, "Start", 4) == 0) {
                break;
            }
        }

        while ((ptr = fgets(line, 256, parmsIn))) {
            if (strncmp(ptr, "Mode", 4) == 0) {
                /* Get the mode */
                ptr = strchr(line, '=');
                if (ptr++) {
                    while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
                    if (strncmp(ptr, "Mono", 4) == 0) {
                        info->channelMode = SBC_CHNL_MODE_MONO;
                    } else if (strncmp(ptr, "Stereo", 6) == 0) {
                        info->channelMode = SBC_CHNL_MODE_STEREO;
                    } else if (strncmp(ptr, "Joint", 5) == 0) {
                        info->channelMode = SBC_CHNL_MODE_JOINT_STEREO;
                    } else if (strncmp(ptr, "Dual", 4) == 0) {
                        info->channelMode = SBC_CHNL_MODE_DUAL_CHNL;
                    } else {
                        status = XA_STATUS_FAILED;
                        goto exit;
                    }
                } else {
                    status = XA_STATUS_FAILED;
                    goto exit;
                }
            } else if (strncmp(ptr, "Block", 5) == 0) {
                /* Get the Block length */
                ptr = strchr(line, '=');
                if (ptr++) {
                    while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
                    info->numBlocks = atoi(ptr);
                } else {
                    status = XA_STATUS_FAILED;
                    goto exit;
                }
            } else if (strncmp(ptr, "Subbands", 8) == 0) {
                /* Get the number of Subbands */
                ptr = strchr(line, '=');
                if (ptr++) {
                    while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
                    info->numSubBands = atoi(ptr);
                } else {
                    status = XA_STATUS_FAILED;
                    goto exit;
                }
            } else if (strncmp(ptr, "Allocation", 10) == 0) {
                /* Get the Allocation method */
                ptr = strchr(line, '=');
                if (ptr++) {
                    while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
                    if (strncmp(ptr, "SNR", 3) == 0) {
                        info->allocMethod = SBC_ALLOC_METHOD_SNR;
                    } else if (strncmp(ptr, "LOUDNESS", 8) == 0) {
                        info->allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
                    } else {
                        status = XA_STATUS_FAILED;
                        goto exit;
                    }
                } else {
                    status = XA_STATUS_FAILED;
                    goto exit;
                }
            } else if (strncmp(ptr, "Bitpool", 7) == 0) {
                /* Get the Bitpool size */
                ptr = strchr(line, '=');
                if (ptr++) {
                    while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
                    info->bitPool = atoi(ptr);
                } else {
                    status = XA_STATUS_FAILED;
                    goto exit;
                }
            } else if (strncmp(ptr, "Done", 4) == 0) {
                break;
            }
        }
    } else {
        status = XA_STATUS_FAILED;
    }

exit:

    return status;
}



#if XA_DEBUG == XA_ENABLED
/*---------------------------------------------------------------------------
 * Called to indicate a failed condition
 */
void OS_Assert(const char *expression, const char *file, U16 line)
{
    /* Halt the system */
    printf("Assert (%s) in file: %s, line: %d", expression, file, line);
}
#endif /* XA_DEBUG == XA_ENABLED */

#if XA_DEBUG_PRINT == XA_ENABLED
#if !defined(DEMO) || TCP_STACK == XA_ENABLED || IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * Called to report stack information
 *
 * NOTE: On the Windows OS platform XA_DEBUG must be enabled to 
 *       print debug messages.
 */
void OS_Report(const char *format, ...)
{
    char buffer[200];  /* Output buffer */
    va_list     args;

    va_start(args, format);
    _vsnprintf(buffer, 200, format, args);
    va_end(args);

    printf("%s", buffer);
}
#endif /* DEMO */
#endif /* XA_DEBUG_PRINT */

