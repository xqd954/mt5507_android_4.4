/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MTKOMXPlugin.h"

#include <dlfcn.h>

#include <media/stagefright/HardwareAPI.h>
#include <media/stagefright/MediaDebug.h>
#include <ui/GraphicBuffer.h>

namespace android {
    
OMX_ERRORTYPE _plugin_use_buffer_anb(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBufferHdr,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN OMX_U32                   nSizeBytes,
    OMX_IN OMX_U8                   *pBuffer)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;

    ret = OMX_UseBuffer(
                hComponent, ppBufferHdr, nPortIndex,
                pAppPrivate, nSizeBytes, pBuffer);

    return ret;
}

typedef struct UseAndroidNativeBufferParams3 {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_PTR pAppPrivate;
    OMX_BUFFERHEADERTYPE **bufferHeader;
    const sp<GraphicBuffer>& nativeBuffer;
};

OMX_ERRORTYPE _plugin_use_android_native_buffer(OMX_HANDLETYPE hComponent, UseAndroidNativeBufferParams3* ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    OMX_U32                frameSize = 0;
    OMX_U32                bufWidth, bufHeight;

    sp<GraphicBuffer> nBuf = ComponentParameterStructure->nativeBuffer;

    OMX_U8 *buffer_u8;
    //MTK_OMX_LOGD ("@@ nBuf.get() = 0x%08X", nBuf.get());
    
    status_t err = nBuf->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, (void**)(&buffer_u8));
    if (err != 0) {
        return OMX_ErrorUndefined;
    }
    //MTK_OMX_LOGD ("@@ nBuf.get() = 0x%08X, nBuf->width=%d, nBuf->height=%d, nBuf->stride=%d, nBuf->format=0x%08X, buffer=0x%08X", nBuf.get(), nBuf->width, nBuf->height, nBuf->stride, nBuf->format, buffer);

    // mtk FBM width is 64 alignment.
    bufWidth = ((nBuf->width + 63) / 64) * 64;
    //bufHeight = ((nBuf->height + 15) / 16) * 16;
    // mtk FBM height is 32 alignment.
    bufHeight = ((nBuf->height + 31) / 32) * 32;
    //frameSize = (bufWidth * bufHeight * 3) / 2;
    //frameSize = bufWidth * bufHeight * 4;
    frameSize = (bufWidth * bufHeight * 3) / 2;

    ret = _plugin_use_buffer_anb(hComponent, 
                                 ComponentParameterStructure->bufferHeader, 
                                 ComponentParameterStructure->nPortIndex,
                                 ComponentParameterStructure->pAppPrivate, 
                                 frameSize, (OMX_U8 *)buffer_u8);

    err = nBuf->unlock();
    if (err != 0) {
        return OMX_ErrorUndefined;
    }

    return ret;
}


static OMX_ERRORTYPE _plugin_send_command(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam1,
            OMX_IN  OMX_PTR pCmdData)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->SendCommand(pMTKOMXPlugin->mhComponentConnect,
        Cmd, nParam1, pCmdData);
}


static OMX_ERRORTYPE _plugin_get_parameter(
            OMX_IN  OMX_HANDLETYPE hComponent, 
            OMX_IN  OMX_INDEXTYPE nParamIndex,  
            OMX_INOUT OMX_PTR ComponentParameterStructure)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    OMX_ERRORTYPE                       eError = OMX_ErrorNone;
    
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }

    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    switch ((_MTKOMXPLUGIN_INDEXTYPE)nParamIndex)
    {
        case _MTKOMXPLUGIN_IndexParamGetAndroidNativeBufferUsage:
        {
            GetAndroidNativeBufferUsageParams *pPar =
                (GetAndroidNativeBufferUsageParams *)ComponentParameterStructure;

            if (MTK_OMX_PLUGIN_OUTPUT_PORT_INDEX != pPar->nPortIndex) {
                eError = OMX_ErrorBadPortIndex;
                break;
            }

            pPar->nUsage = GRALLOC_USAGE_SW_WRITE_OFTEN;
            break;
        }
                
        default:
            //LOGE("[OMX_Core][_set_parameter] %s %d: not implement (type=%d) ERROR\n", __FUNCTION__, __LINE__, nParamIndex);
            eError = OMX_ErrorNotImplemented;
            break;
    }

    // OMX_INDEXEXTTYPE, extension part
    if(eError == OMX_ErrorNotImplemented)
    {
        eError = OMX_ErrorNone;
        switch ((OMX_INDEXTYPE)nParamIndex)
        {
            default:
            eError = ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->GetParameter(pMTKOMXPlugin->mhComponentConnect,
                nParamIndex, ComponentParameterStructure);
            break;
        }
    }

    LOGI("OMXPlugin(%d) %s nParamIndex(0x%x) err(%d)", (int)hComponent,
            __FUNCTION__, (int)nParamIndex, eError);

    return eError;

}

static OMX_ERRORTYPE _plugin_set_parameter(
            OMX_IN  OMX_HANDLETYPE hComponent, 
            OMX_IN  OMX_INDEXTYPE nParamIndex,  
            OMX_INOUT OMX_PTR ComponentParameterStructure)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    OMX_ERRORTYPE                       eError = OMX_ErrorNone;
    
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }

    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    switch ((_MTKOMXPLUGIN_INDEXTYPE)nParamIndex)
    {
        case _MTKOMXPLUGIN_IndexParamEnableAndroidBuffers:
        {
            EnableAndroidNativeBuffersParams *pPar =
                (EnableAndroidNativeBuffersParams*)ComponentParameterStructure;

            if (MTK_OMX_PLUGIN_OUTPUT_PORT_INDEX != pPar->nPortIndex) {
                eError = OMX_ErrorBadPortIndex;
                break;
            }

            pMTKOMXPlugin->mEnableANB = pPar->enable;
            break;
        }

        case _MTKOMXPLUGIN_IndexParamGetAndroidNativeBufferUsage:
        {
            GetAndroidNativeBufferUsageParams *pPar =
                (GetAndroidNativeBufferUsageParams *)ComponentParameterStructure;

            if (MTK_OMX_PLUGIN_OUTPUT_PORT_INDEX != pPar->nPortIndex) {
                eError = OMX_ErrorBadPortIndex;
                break;
            }

            pPar->nUsage = GRALLOC_USAGE_SW_WRITE_OFTEN;
            break;
        }

        case _MTKOMXPLUGIN_IndexParamUseAndroidNativeBuffer:
        {
            UseAndroidNativeBufferParams3 *pPar =
                (UseAndroidNativeBufferParams3*)ComponentParameterStructure;

            if (MTK_OMX_PLUGIN_OUTPUT_PORT_INDEX != pPar->nPortIndex) {
                eError = OMX_ErrorBadPortIndex;
                break;
            }
            eError = _plugin_use_android_native_buffer(hComponent,
                pPar);
            break;
        }
                
        default:
            //LOGE("[OMX_Core][_set_parameter] %s %d: not implement (type=%d) ERROR\n", __FUNCTION__, __LINE__, nParamIndex);
            eError = OMX_ErrorNotImplemented;
            break;
    }

    // OMX_INDEXEXTTYPE, extension part
    if(eError == OMX_ErrorNotImplemented)
    {
        eError = OMX_ErrorNone;
        switch ((OMX_INDEXTYPE)nParamIndex)
        {
            default:
            eError = ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->SetParameter(pMTKOMXPlugin->mhComponentConnect,
                nParamIndex, ComponentParameterStructure);
            break;
        }
    }

    LOGI("OMXPlugin(%d) %s nParamIndex(0x%x) err(%d)", (int)hComponent,
            __FUNCTION__, (int)nParamIndex, eError);

    return eError;
}


static OMX_ERRORTYPE _plugin_get_state(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->GetState(pMTKOMXPlugin->mhComponentConnect,
        pState);
}


static OMX_ERRORTYPE _plugin_use_buffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->UseBuffer(pMTKOMXPlugin->mhComponentConnect,
        ppBuffer, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}


static OMX_ERRORTYPE _plugin_allocate_buffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->AllocateBuffer(pMTKOMXPlugin->mhComponentConnect,
        ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
}


static OMX_ERRORTYPE _plugin_free_buffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->FreeBuffer(pMTKOMXPlugin->mhComponentConnect,
        nPortIndex, pBuffer);
}


static OMX_ERRORTYPE _plugin_empty_this_buffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->EmptyThisBuffer(pMTKOMXPlugin->mhComponentConnect,
        pBuffer);
}


static OMX_ERRORTYPE _plugin_fill_this_buffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->FillThisBuffer(pMTKOMXPlugin->mhComponentConnect,
        pBuffer);
}


static OMX_ERRORTYPE _plugin_get_comp_version(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STRING pComponentName,
            OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
            OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
            OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->GetComponentVersion(pMTKOMXPlugin->mhComponentConnect,
        pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
}


static OMX_ERRORTYPE _plugin_get_config(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex, 
            OMX_INOUT OMX_PTR pComponentConfigStructure)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->GetConfig(pMTKOMXPlugin->mhComponentConnect,
        nIndex, pComponentConfigStructure);
}


static OMX_ERRORTYPE _plugin_set_config(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex, 
            OMX_IN  OMX_PTR pComponentConfigStructure)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->SetConfig(pMTKOMXPlugin->mhComponentConnect,
        nIndex, pComponentConfigStructure);
}


static OMX_ERRORTYPE _plugin_get_extension_idx(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE                       eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    
    if (strncmp(cParameterName, _MTKOMXPLUGIN_INDEX_PARAM_ENABLE_ANB,
        strlen(_MTKOMXPLUGIN_INDEX_PARAM_ENABLE_ANB)) == 0)
    {
        LOGV("OMXPlugin(%d) %s enable ANB(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        *pIndexType = (OMX_INDEXTYPE)_MTKOMXPLUGIN_IndexParamEnableAndroidBuffers;
        eError = OMX_ErrorNone;
    }
    else if (strncmp(cParameterName, _MTKOMXPLUGIN_INDEX_PARAM_GET_ANB_USAGE,
        strlen(_MTKOMXPLUGIN_INDEX_PARAM_GET_ANB_USAGE)) == 0)
    {
        LOGV("OMXPlugin(%d) %s get usage of ANB(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        *pIndexType = (OMX_INDEXTYPE)_MTKOMXPLUGIN_IndexParamGetAndroidNativeBufferUsage;
        eError = OMX_ErrorNone;
    }
    else if (strcmp(cParameterName, _MTKOMXPLUGIN_INDEX_PARAM_USE_ANB) == 0)
    {
        LOGV("OMXPlugin(%d) %s use ANB(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        *pIndexType = (OMX_INDEXTYPE)_MTKOMXPLUGIN_IndexParamUseAndroidNativeBuffer;
        eError = OMX_ErrorNone;
    }
    else if (strcmp(cParameterName, "OMX.google.android.index.useAndroidNativeBuffer2") == 0)
    {
        eError = OMX_ErrorUndefined;
    }
    else
    {
        eError = ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->GetExtensionIndex(pMTKOMXPlugin->mhComponentConnect,
            cParameterName, pIndexType);
    }

    LOGI("OMXPlugin(%d) %s cParameterName(%s) err(%d)", (int)hComponent,
            __FUNCTION__, cParameterName, eError);

    return eError;
}


static OMX_ERRORTYPE _plugin_comp_tunnel_req(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_U32 nPort,
        OMX_IN  OMX_HANDLETYPE hTunneledComp,
        OMX_IN  OMX_U32 nTunneledPort,
        OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->ComponentTunnelRequest(pMTKOMXPlugin->mhComponentConnect,
        nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
}


static OMX_ERRORTYPE _plugin_set_callback(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
            OMX_IN  OMX_PTR pAppData)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                        *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if(((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect)) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->SetCallbacks(pMTKOMXPlugin->mhComponentConnect,
        pCallbacks, pAppData);
}


static OMX_ERRORTYPE _plugin_comp_deinit(
            OMX_IN  OMX_HANDLETYPE hComponent)
{
    OMX_COMPONENTTYPE                   *pComp;
    MTKOMXPlugin                 *pMTKOMXPlugin;
    if(NULL == hComponent)
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorBadParameter;
    }
    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pMTKOMXPlugin = (MTKOMXPlugin *)pComp->pComponentPrivate;
    if((NULL == pMTKOMXPlugin) || (NULL == pMTKOMXPlugin->mhComponentConnect))
    {
        LOGE("OMXPlugin(%d) %s ERROR(L:%d)\n", (int)hComponent, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }
    return ((OMX_COMPONENTTYPE*)pMTKOMXPlugin->mhComponentConnect)->ComponentDeInit(pMTKOMXPlugin->mhComponentConnect);
}

OMXPluginBase *createOMXPlugin() {
    return new MTKOMXPlugin;
}

MTKOMXPlugin::MTKOMXPlugin()
    : mLibHandle(dlopen("libmtal.so", RTLD_NOW)),
      mInit(NULL),
      mDeinit(NULL),
      mComponentNameEnum(NULL),
      mGetHandle(NULL),
      mFreeHandle(NULL),
      mGetRolesOfComponentHandle(NULL) {

      mhComponentThis = NULL;
      mhComponentConnect = NULL;
      mEnableANB = false;
    if (mLibHandle != NULL) {
        mInit = (InitFunc)dlsym(mLibHandle, "OMX_Init");
        mDeinit = (DeinitFunc)dlsym(mLibHandle, "OMX_DeInit");

        mAllocatePrivateBuffer = (AllocatePrivateBufferFunc)dlsym(mLibHandle, "OMX_AllocatePrivateBuffer");
        mFreePrivateBuffer = (FreePrivateBufferFunc)dlsym(mLibHandle, "OMX_FreePrivateBuffer");

        mComponentNameEnum =
            (ComponentNameEnumFunc)dlsym(mLibHandle, "OMX_ComponentNameEnum");

        mGetHandle = (GetHandleFunc)dlsym(mLibHandle, "OMX_GetHandle");
        mFreeHandle = (FreeHandleFunc)dlsym(mLibHandle, "OMX_FreeHandle");

        mGetRolesOfComponentHandle =
            (GetRolesOfComponentFunc)dlsym(
                    mLibHandle, "OMX_GetRolesOfComponent");

        (*mInit)();
    }
    LOGI("MTKOMXPlugin(%d) mLibHandle(%d)", (int)this, (uint32_t)mLibHandle);
}

MTKOMXPlugin::~MTKOMXPlugin() {
    if (mLibHandle != NULL) {
        (*mDeinit)();

        dlclose(mLibHandle);
        mLibHandle = NULL;
    }
    LOGI("MTKOMXPlugin(%d) ~MTKOMXPlugin\n", (int)this);
}

OMX_ERRORTYPE MTKOMXPlugin::makeComponentInstance(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component) {
    OMX_ERRORTYPE eErr;
    if ((mLibHandle == NULL) || (component == NULL)) {
        LOGE("MTKOMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    *component = NULL;

    mhComponentThis = (OMX_COMPONENTTYPE*)malloc(sizeof(OMX_COMPONENTTYPE));
    if(!mhComponentThis)
    {
        LOGE("MTKOMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, __FUNCTION__, __LINE__);
        return OMX_ErrorInsufficientResources;
    }

    eErr = (*mGetHandle)(
            reinterpret_cast<OMX_HANDLETYPE *>(component),
            const_cast<char *>(name),
            appData, const_cast<OMX_CALLBACKTYPE *>(callbacks));

    if(eErr != OMX_ErrorNone)
    {
        free(mhComponentThis);
        mhComponentThis = NULL;
    }

    if((eErr == OMX_ErrorNone) && (*component))
    {

        // return mhComponentThis, keep the real component at mhComponentConnect
        mhComponentConnect = (OMX_HANDLETYPE)*component;
        memcpy(mhComponentThis, mhComponentConnect, sizeof(OMX_COMPONENTTYPE));

        ((OMX_COMPONENTTYPE*)mhComponentThis)->pComponentPrivate        = this;

        ((OMX_COMPONENTTYPE*)mhComponentThis)->SendCommand              = _plugin_send_command;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->GetParameter             = _plugin_get_parameter;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->SetParameter             = _plugin_set_parameter;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->GetState                 = _plugin_get_state;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->UseBuffer                = _plugin_use_buffer;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->AllocateBuffer           = _plugin_allocate_buffer;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->FreeBuffer               = _plugin_free_buffer;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->EmptyThisBuffer          = _plugin_empty_this_buffer;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->FillThisBuffer           = _plugin_fill_this_buffer;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->GetComponentVersion      = _plugin_get_comp_version;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->GetConfig                = _plugin_get_config;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->SetConfig                = _plugin_set_config;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->GetExtensionIndex        = _plugin_get_extension_idx;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->ComponentTunnelRequest   = _plugin_comp_tunnel_req;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->SetCallbacks             = _plugin_set_callback;
        ((OMX_COMPONENTTYPE*)mhComponentThis)->ComponentDeInit          = _plugin_comp_deinit;

        *component = (OMX_COMPONENTTYPE*)mhComponentThis;
    }
    
    LOGI("MTKOMXPlugin(%d) OMXPlugin(%d) %s eErr(%d) name(%s)", (int)this, (int)*component,
        __FUNCTION__, (uint32_t)eErr, name);

    return eErr;
}

OMX_ERRORTYPE MTKOMXPlugin::destroyComponentInstance(
        OMX_COMPONENTTYPE *component) {
    OMX_ERRORTYPE eErr;
    if (mLibHandle == NULL) {
        LOGE("MTKOMXPlugin(%d) OMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, (int)component, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    if(mhComponentConnect)
    {
        if(component != mhComponentThis)
        {
            LOGE("MTKOMXPlugin(%d) OMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, (int)component, __FUNCTION__, __LINE__);
        }
        eErr = (*mFreeHandle)(reinterpret_cast<OMX_HANDLETYPE *>(mhComponentConnect));
        mhComponentConnect = NULL;
    }
    else
    {
        eErr = (*mFreeHandle)(reinterpret_cast<OMX_HANDLETYPE *>(component));
    }
    if(mhComponentThis)
    {
        free(mhComponentThis);
        mhComponentThis = NULL;
    }
    LOGI("MTKOMXPlugin(%d) OMXPlugin(%d) %s eErr(%d)", (int)this, (int)component, __FUNCTION__, (uint32_t)eErr);
    return eErr;
}

OMX_ERRORTYPE MTKOMXPlugin::enumerateComponents(
        OMX_STRING name,
        size_t size,
        OMX_U32 index) {
    OMX_ERRORTYPE eErr;
    if (mLibHandle == NULL) {
        LOGE("MTKOMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    eErr = (*mComponentNameEnum)(name, size, index);
    LOGI("MTKOMXPlugin(%d) %s eErr(%d) name(%s)", (int)this,
        __FUNCTION__, (uint32_t)eErr, name);
    return eErr;
}

OMX_ERRORTYPE MTKOMXPlugin::getRolesOfComponent(
        const char *name,
        Vector<String8> *roles) {
    OMX_ERRORTYPE eErr;
    roles->clear();

    if (mLibHandle == NULL) {
        LOGE("MTKOMXPlugin(%d) %s ERROR(L:%d)\n", (int)this, __FUNCTION__, __LINE__);
        return OMX_ErrorUndefined;
    }

    OMX_U32 numRoles;
    OMX_ERRORTYPE err = (*mGetRolesOfComponentHandle)(
            const_cast<OMX_STRING>(name), &numRoles, NULL);

    if (err != OMX_ErrorNone) {
        LOGE("MTKOMXPlugin(%d) %s eErr(%d) name(%s)", (int)this,
            __FUNCTION__, (uint32_t)eErr, name);
        return err;
    }

    if (numRoles > 0) {
        OMX_U8 **array = new OMX_U8 *[numRoles];
        for (OMX_U32 i = 0; i < numRoles; ++i) {
            array[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
        }

        OMX_U32 numRoles2;
        err = (*mGetRolesOfComponentHandle)(
                const_cast<OMX_STRING>(name), &numRoles2, array);

        LOGI("MTKOMXPlugin(%d) %s name(%s) array[0](%s) err(%d)", (int)this,
            __FUNCTION__, name, array[0], err);

        CHECK_EQ(err, OMX_ErrorNone);
        CHECK_EQ(numRoles, numRoles2);

        for (OMX_U32 i = 0; i < numRoles; ++i) {
            String8 s((const char *)array[i]);
            roles->push(s);

            delete[] array[i];
            array[i] = NULL;
        }

        delete[] array;
        array = NULL;
    }

    LOGI("MTKOMXPlugin(%d) %s err(%d)", (int)this,
            __FUNCTION__, OMX_ErrorNone);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE MTKOMXPlugin::allocatePrivateBuffer(void **ppBuffer)
{
    (* mAllocatePrivateBuffer)(ppBuffer);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MTKOMXPlugin::freePrivateBuffer(void *pBuffer, size_t size)
{
    (* mFreePrivateBuffer)(pBuffer, size);
    return OMX_ErrorNone;
}

}  // namespace android
