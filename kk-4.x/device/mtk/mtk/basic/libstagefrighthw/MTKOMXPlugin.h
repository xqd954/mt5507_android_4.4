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

#ifndef MTK_OMX_PLUGIN_H_

#define MTK_OMX_PLUGIN_H_

#include <media/stagefright/OMXPluginBase.h>

#define MTK_OMX_PLUGIN_INPUT_PORT_INDEX     (0)
#define MTK_OMX_PLUGIN_OUTPUT_PORT_INDEX    (1)

typedef enum _MTKOMXPLUGIN_OMX_INDEXTYPE
{
    _MTKOMXPLUGIN_IndexVendorStartUnused = 0x7F000000,
    /* Vendor specific structures should be in the range of 0x7F000000 
       to 0x7FFFFFFE.  This range is not broken out by vendor, so
       private indexes are not guaranteed unique and therefore should
       only be sent to the appropriate component. */
        
    /* android extension start */
    _MTKOMXPLUGIN_IndexAndroidStartUnused        = _MTKOMXPLUGIN_IndexVendorStartUnused + 0x00500000,
    
    /* for Android Native Window */
#define _MTKOMXPLUGIN_INDEX_PARAM_ENABLE_ANB      "OMX.google.android.index.enableAndroidNativeBuffers"
    _MTKOMXPLUGIN_IndexParamEnableAndroidBuffers          = _MTKOMXPLUGIN_IndexAndroidStartUnused + 1,
#define _MTKOMXPLUGIN_INDEX_PARAM_GET_ANB_USAGE   "OMX.google.android.index.getAndroidNativeBufferUsage"
    _MTKOMXPLUGIN_IndexParamGetAndroidNativeBufferUsage   = _MTKOMXPLUGIN_IndexAndroidStartUnused + 2,
#define _MTKOMXPLUGIN_INDEX_PARAM_USE_ANB         "OMX.google.android.index.useAndroidNativeBuffer"
    _MTKOMXPLUGIN_IndexParamUseAndroidNativeBuffer        = _MTKOMXPLUGIN_IndexAndroidStartUnused + 3,

    /* for Android Store Metadata Inbuffer */
#define _MTKOMXPLUGIN_INDEX_PARAM_STORE_METADATA_BUFFER "OMX.google.android.index.storeMetaDataInBuffers"
    _MTKOMXPLUGIN_IndexParamStoreMetaDataBuffer           = _MTKOMXPLUGIN_IndexAndroidStartUnused + 4,

    _MTKOMXPLUGIN_IndexMax = 0x7FFFFFFF

} _MTKOMXPLUGIN_INDEXTYPE;


namespace android {

struct MTKOMXPlugin : public OMXPluginBase {

    bool mEnableANB; // android native buffer
    OMX_HANDLETYPE mhComponentConnect;
    OMX_HANDLETYPE mhComponentThis;

    MTKOMXPlugin();
    virtual ~MTKOMXPlugin();

    virtual OMX_ERRORTYPE makeComponentInstance(
            const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);

    virtual OMX_ERRORTYPE destroyComponentInstance(
            OMX_COMPONENTTYPE *component);

    virtual OMX_ERRORTYPE enumerateComponents(
            OMX_STRING name,
            size_t size,
            OMX_U32 index);

    virtual OMX_ERRORTYPE getRolesOfComponent(
            const char *name,
            Vector<String8> *roles);

	virtual OMX_ERRORTYPE allocatePrivateBuffer(void **ppBuffer);
	virtual OMX_ERRORTYPE freePrivateBuffer(void *pBuffer, size_t size);

private:
    void *mLibHandle;

    typedef OMX_ERRORTYPE (*InitFunc)();
    typedef OMX_ERRORTYPE (*DeinitFunc)();
	typedef OMX_ERRORTYPE (*AllocatePrivateBufferFunc)(void **);
	typedef OMX_ERRORTYPE (*FreePrivateBufferFunc)(void *, OMX_U32);
    typedef OMX_ERRORTYPE (*ComponentNameEnumFunc)(
            OMX_STRING, OMX_U32, OMX_U32);

    typedef OMX_ERRORTYPE (*GetHandleFunc)(
            OMX_HANDLETYPE *, OMX_STRING, OMX_PTR, OMX_CALLBACKTYPE *);

    typedef OMX_ERRORTYPE (*FreeHandleFunc)(OMX_HANDLETYPE *);

    typedef OMX_ERRORTYPE (*GetRolesOfComponentFunc)(
            OMX_STRING, OMX_U32 *, OMX_U8 **);

    InitFunc mInit;
    DeinitFunc mDeinit;
	AllocatePrivateBufferFunc mAllocatePrivateBuffer;
	FreePrivateBufferFunc mFreePrivateBuffer;
    ComponentNameEnumFunc mComponentNameEnum;
    GetHandleFunc mGetHandle;
    FreeHandleFunc mFreeHandle;
    GetRolesOfComponentFunc mGetRolesOfComponentHandle;

    MTKOMXPlugin(const MTKOMXPlugin &);
    MTKOMXPlugin &operator=(const MTKOMXPlugin &);
};

}  // namespace android

#endif  // MTK_OMX_PLUGIN_H_
