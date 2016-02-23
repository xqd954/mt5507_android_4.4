/*
 * Copyright (C) 2011 The Android Open Source Project
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

//#define LOG_NDEBUG 0
#define LOG_TAG "ChromiumHTTPDataSource"
#include <media/stagefright/foundation/ADebug.h>

#include "include/ChromiumHTTPDataSource.h"

#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/MediaErrors.h>

#include "support.h"

#include <cutils/properties.h> // for property_get

namespace android {

ChromiumHTTPDataSource::ChromiumHTTPDataSource(uint32_t flags)
    : mFlags(flags),
      mState(DISCONNECTED),
      mDelegate(new SfDelegate),
      mCurrentOffset(0),
      mIOResult(OK),
      mContentSize(-1),
      mDecryptHandle(NULL),
      mDrmManagerClient(NULL) {
    mDelegate->setOwner(this);
}

ChromiumHTTPDataSource::~ChromiumHTTPDataSource() {
    disconnect();

    delete mDelegate;
    mDelegate = NULL;

    clearDRMState_l();

    if (mDrmManagerClient != NULL) {
        delete mDrmManagerClient;
        mDrmManagerClient = NULL;
    }
}

status_t ChromiumHTTPDataSource::connect(
        const char *uri,
        const KeyedVector<String8, String8> *headers,
        off64_t offset) {
    Mutex::Autolock autoLock(mLock);

    uid_t uid;
    if (getUID(&uid)) {
        mDelegate->setUID(uid);
    }

#if defined(LOG_NDEBUG) && !LOG_NDEBUG
    ALOGV("connect on behalf of uid %d", uid);
#endif

    return connect_l(uri, headers, offset);
}

status_t ChromiumHTTPDataSource::connect_l(
        const char *uri,
        const KeyedVector<String8, String8> *headers,
        off64_t offset) {
#ifndef ANDROID_DEFAULT_CODE
    while (mState == DISCONNECTING) {
        ALOGV("Waiting previous disconnect complete when connect");
        mCondition.wait(mLock);
    }   
#endif
    if (mState != DISCONNECTED) {
        disconnect_l();
    }

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    ALOGD("connect to %s @%lld", uri, offset);
#else
#if defined(LOG_NDEBUG) && !LOG_NDEBUG
    ALOGV("connect to <URL suppressed> @%lld", offset);
#endif
#endif

    mURI = uri;
    mContentType = String8("application/octet-stream");

    if (headers != NULL) {
        mHeaders = *headers;
    } else {
        mHeaders.clear();
    }

    mState = CONNECTING;
    mContentSize = -1;
    mCurrentOffset = offset;

    mDelegate->initiateConnection(mURI.c_str(), &mHeaders, offset);
#ifndef ANDROID_DEFAULT_CODE
    ALOGV("waiting to connect ... ");
#endif

    while (mState == CONNECTING || mState == DISCONNECTING) {
        mCondition.wait(mLock);
    }

#ifndef ANDROID_DEFAULT_CODE
	if( mState == CONNECT_REDIRECT)
	{
		ALOGV("CONNECT_REDIRECT!!! mURI=%s",mURI.c_str());
		return connect_l(mURI.c_str(),&mHeaders,mCurrentOffset);
	}
		
    ALOGV("\t\tconnect done");
#endif
    return mState == CONNECTED ? OK : mIOResult;
}
#ifndef ANDROID_DEFAULT_CODE
void ChromiumHTTPDataSource::OnReceivedRedirect(const char *uri)
{
    Mutex::Autolock autoLock(mLock);
    mURI = uri;
    if (mState == CONNECTING) 
    {
        mState = CONNECT_REDIRECT;
        	 mIOResult = -EINTR;	 
        mCondition.broadcast();
        ALOGI("ChromiumHTTPDataSource::OnReceivedRedirect:initiateDisconnect,mState=%d",mState);
    }
}
#endif

void ChromiumHTTPDataSource::onRedirect(const char *url) {
    Mutex::Autolock autoLock(mLock);
    mURI = url;
}

void ChromiumHTTPDataSource::onConnectionEstablished(
        int64_t contentSize, const char *contentType) {
#ifndef ANDROID_DEFAULT_CODE
    ALOGV("onConnectEstablished, mState = %d", (int)mState);
#endif
    Mutex::Autolock autoLock(mLock);

    if (mState != CONNECTING) {
#ifndef ANDROID_DEFAULT_CODE
        if (mState == CONNECT_REDIRECT) {// for CR ALPS01430522
	    ALOGV("onConnectEstablished, mState = %d, skip this connection and return", (int)mState);
	    //mCondition.broadcast();
            return;
        }	
#endif

        // We may have initiated disconnection.
        CHECK_EQ(mState, DISCONNECTING);
        return;
    }
#ifndef ANDROID_DEFAULT_CODE
    if (mState == DISCONNECTING) {
        ALOGV("onConnectEstablished in DISCONNECTING");
    } else {

        mState = CONNECTED;

    }
#else
    mState = CONNECTED;
#endif//#ifndef ANDROID_DEFAULT_CODE
    mContentSize = (contentSize < 0) ? -1 : contentSize + mCurrentOffset;
    mContentType = String8(contentType);
    mCondition.broadcast();
}

void ChromiumHTTPDataSource::onConnectionFailed(status_t err) {
    Mutex::Autolock autoLock(mLock);
#ifndef ANDROID_DEFAULT_CODE
    ALOGE("onConnectionFailed, err = %d", (int)err);
#endif
    mState = DISCONNECTED;
    mCondition.broadcast();

    // mURI.clear();

    mIOResult = err;
}

void ChromiumHTTPDataSource::disconnect() {
#ifndef ANDROID_DEFAULT_CODE
    ALOGV("disconnect in");
#endif
    Mutex::Autolock autoLock(mLock);
#ifndef ANDROID_DEFAULT_CODE
    ALOGV("disconnect lock ok");
#endif
    disconnect_l();
}

void ChromiumHTTPDataSource::disconnect_l() {
    if (mState == DISCONNECTED) {
        return;
    }
#ifndef ANDROID_DEFAULT_CODE
    if (mState == READING) {
        ALOGV("disconnect when reading");
    }

    if (mState == CONNECTING) {
        ALOGV("disconnect when connecting!!!");
    }
    if (mState == CONNECT_REDIRECT) {
        ALOGV("disconnect when CONNECT_REDIRECT!!!");
    }
#endif

    mState = DISCONNECTING;
    mIOResult = -EINTR;

    mDelegate->initiateDisconnect();

    while (mState == DISCONNECTING) {
        mCondition.wait(mLock);
    }
#ifndef ANDROID_DEFAULT_CODE
    // google design defect, may casue timing problem
    // Case 1: disconnect() and readat() timging.
    //         When A thread disconnecting and wait, B thread readAt and disconnect_l(). So A and B thread both wait for disconnect complete.
    //         When disconnect complete, if B thread get schedule and run, A thread should be wait for mLock. B thread would do connect() and
    //         set mState = CONNECTING, then wait(). At the same time, A thread can get mLock, but mState is uncorrect. So it cause check fail.
    // Case 2: disconnect() and connect() timing.
    if (mState != DISCONNECTED) {
        ALOGE("error: disconnect(), mState = %d", (int)mState);
    }
#else
    CHECK_EQ((int)mState, (int)DISCONNECTED);
#endif

}

status_t ChromiumHTTPDataSource::initCheck() const {
    Mutex::Autolock autoLock(mLock);

    return mState == CONNECTED ? OK : NO_INIT;
}

ssize_t ChromiumHTTPDataSource::readAt(off64_t offset, void *data, size_t size) {
    Mutex::Autolock autoLock(mLock);

    if (mState != CONNECTED) {
        return INVALID_OPERATION;
    }

#if 0
    char value[PROPERTY_VALUE_MAX];
    if (property_get("media.stagefright.disable-net", value, 0)
            && (!strcasecmp(value, "true") || !strcmp(value, "1"))) {
        LOG_PRI(ANDROID_LOG_INFO, LOG_TAG, "Simulating that the network is down.");
        disconnect_l();
        return ERROR_IO;
    }
#endif

    if (offset != mCurrentOffset) {
        AString tmp = mURI;
        KeyedVector<String8, String8> tmpHeaders = mHeaders;

        disconnect_l();

        status_t err = connect_l(tmp.c_str(), &tmpHeaders, offset);

        if (err != OK) {
            return err;
        }
    }

    mState = READING;

    int64_t startTimeUs = ALooper::GetNowUs();

    mDelegate->initiateRead(data, size);

    while (mState == READING) {
        mCondition.wait(mLock);
    }
    
    // for case that got disconnected while reading will cause
    // ANR in NuCachedSource2::fetchInternal
    if (mState != CONNECTED)
    {
        ALOGD("readAt fail: mState = %d, mIOResult = %d", mState, mIOResult);
        return 0;
    }
#if 0    
    if (mIOResult == -1004)     
    {
        ALOGD("readAt fail: mState = %d, mIOResult = %d", mState, mIOResult);
        return 0;
    }
#endif    
    if (mIOResult < OK) {
        return mIOResult;
    }

    if (mState == CONNECTED) {
        int64_t delayUs = ALooper::GetNowUs() - startTimeUs;

        // The read operation was successful, mIOResult contains
        // the number of bytes read.
        addBandwidthMeasurement(mIOResult, delayUs);

        mCurrentOffset += mIOResult;
        return mIOResult;
    }

    return ERROR_IO;
}

void ChromiumHTTPDataSource::onReadCompleted(ssize_t size) {
    Mutex::Autolock autoLock(mLock);

    mIOResult = size;

    if (mState == READING) {
        mState = CONNECTED;
        mCondition.broadcast();
    }
}

status_t ChromiumHTTPDataSource::getSize(off64_t *size) {
    Mutex::Autolock autoLock(mLock);

    if (mContentSize < 0) {
        return ERROR_UNSUPPORTED;
    }

    *size = mContentSize;

    return OK;
}

uint32_t ChromiumHTTPDataSource::flags() {
    return kWantsPrefetching | kIsHTTPBasedSource;
}

// static
void ChromiumHTTPDataSource::InitiateRead(
        ChromiumHTTPDataSource *me, void *data, size_t size) {
    me->initiateRead(data, size);
}

void ChromiumHTTPDataSource::initiateRead(void *data, size_t size) {
    mDelegate->initiateRead(data, size);
}

void ChromiumHTTPDataSource::onDisconnectComplete() {
    Mutex::Autolock autoLock(mLock);
#ifndef ANDROID_DEFAULT_CODE
    ALOGV("onDisconnectComplete()");
#endif
    CHECK_EQ((int)mState, (int)DISCONNECTING);

    mState = DISCONNECTED;
    // mURI.clear();
    mIOResult = -ENOTCONN;

    mCondition.broadcast();
}

sp<DecryptHandle> ChromiumHTTPDataSource::DrmInitialization(const char* mime) {
    Mutex::Autolock autoLock(mLock);

    if (mDrmManagerClient == NULL) {
        mDrmManagerClient = new DrmManagerClient();
    }

    if (mDrmManagerClient == NULL) {
        return NULL;
    }

    if (mDecryptHandle == NULL) {
        /* Note if redirect occurs, mUri is the redirect uri instead of the
         * original one
         */
        mDecryptHandle = mDrmManagerClient->openDecryptSession(
                String8(mURI.c_str()), mime);
    }

    if (mDecryptHandle == NULL) {
        delete mDrmManagerClient;
        mDrmManagerClient = NULL;
    }

    return mDecryptHandle;
}

void ChromiumHTTPDataSource::getDrmInfo(
        sp<DecryptHandle> &handle, DrmManagerClient **client) {
    Mutex::Autolock autoLock(mLock);

    handle = mDecryptHandle;
    *client = mDrmManagerClient;
}

String8 ChromiumHTTPDataSource::getUri() {
    Mutex::Autolock autoLock(mLock);

    return String8(mURI.c_str());
}

String8 ChromiumHTTPDataSource::getMIMEType() const {
    Mutex::Autolock autoLock(mLock);

    return mContentType;
}

void ChromiumHTTPDataSource::clearDRMState_l() {
    if (mDecryptHandle != NULL) {
        // To release mDecryptHandle
        CHECK(mDrmManagerClient);
        mDrmManagerClient->closeDecryptSession(mDecryptHandle);
        mDecryptHandle = NULL;
    }
}

status_t ChromiumHTTPDataSource::reconnectAtOffset(off64_t offset) {
    Mutex::Autolock autoLock(mLock);

    if (mURI.empty()) {
        return INVALID_OPERATION;
    }

    LOG_PRI(ANDROID_LOG_INFO, LOG_TAG, "Reconnecting...");
    status_t err = connect_l(mURI.c_str(), &mHeaders, offset);
    if (err != OK) {
        LOG_PRI(ANDROID_LOG_INFO, LOG_TAG, "Reconnect failed w/ err 0x%08x", err);
    }

    return err;
}

// static
status_t ChromiumHTTPDataSource::UpdateProxyConfig(
        const char *host, int32_t port, const char *exclusionList) {
    return SfDelegate::UpdateProxyConfig(host, port, exclusionList);
}

}  // namespace android

