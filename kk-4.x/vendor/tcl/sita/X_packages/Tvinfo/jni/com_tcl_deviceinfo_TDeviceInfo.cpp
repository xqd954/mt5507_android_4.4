#include "cutils/properties.h"
#include <utils/Log.h>
#include "jni.h"
#include <nativehelper/JNIHelp.h>
#include "DeviceInfo.h"

#define LOG_TAG "DeviceInfoJNI"

static void
tcl_deviceinfo_TDeviceInfo_checkDatabase(JNIEnv* env, jobject obj) {
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_checkDatabase();
}

static jintArray 
tcl_deviceinfo_TDeviceInfo_getProjectList (JNIEnv* env, jobject obj) {
	int prjList[150] = {-1};
	int size = 0;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getProjectList(prjList, size);
	jintArray mRet = env->NewIntArray(size); 
	env->SetIntArrayRegion(mRet, 0, size, prjList);
	return mRet;
}

static jstring 
tcl_deviceinfo_TDeviceInfo_getClientType (JNIEnv* env, jobject obj, jint prjid) {
	char buff[50] = {0};
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getClientType(prjid, buff);
	return env->NewStringUTF(buff);
}

static jstring 
tcl_deviceinfo_TDeviceInfo_getModelName (JNIEnv* env, jobject obj, jint prjid) {
	char buff[50] = {0};
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getModelName(prjid, buff);
	return env->NewStringUTF(buff);
}

static jstring 
tcl_deviceinfo_TDeviceInfo_getPanelName (JNIEnv* env, jobject obj, jint prjid) {
	char buff[50] = {0};
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getPanelName(prjid, buff);
	return env->NewStringUTF(buff);
}

static int 
tcl_deviceinfo_TDeviceInfo_getPanelType (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_getPanelType(prjid);
	return mRet;
}

static jstring 
tcl_deviceinfo_TDeviceInfo_getPSUName (JNIEnv* env, jobject obj, jint prjid) {
	char buff[50] = {0};
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getPSUName(prjid, buff);
	return env->NewStringUTF(buff);
}

static jstring 
tcl_deviceinfo_TDeviceInfo_getRCUName (JNIEnv* env, jobject obj, jint prjid) {
	char buff[50] = {0};
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	pDevInfo->device_info_getRCUName(prjid, buff);
	return env->NewStringUTF(buff);
}

static int
tcl_deviceinfo_TDeviceInfo_get3DStatus (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_get3DStatus(prjid);
	return mRet;
}

static int
tcl_deviceinfo_TDeviceInfo_get3DGlassesType (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_get3DGlassesType(prjid);
	return mRet;
}

static int
tcl_deviceinfo_TDeviceInfo_getWifiStatus (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_getWifiStatus(prjid);
	return mRet;
}

static int
tcl_deviceinfo_TDeviceInfo_getWifiType (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_getWifiType(prjid);
	return mRet;
}

static int
tcl_deviceinfo_TDeviceInfo_getMemc (JNIEnv* env, jobject obj, jint prjid) {
	int mRet = -1;
	DeviceInfo *pDevInfo = DeviceInfo::getInstance();
	mRet = pDevInfo->device_info_getMemc(prjid);
	return mRet;
}

static JNINativeMethod method_table[] = {
{"native_devinfo_checkDatabase", "()V", (void*)tcl_deviceinfo_TDeviceInfo_checkDatabase },
{"native_devinfo_getProjectList", "()[I", (void*)tcl_deviceinfo_TDeviceInfo_getProjectList },
{"native_devinfo_getClientType", "(I)Ljava/lang/String;", (void*)tcl_deviceinfo_TDeviceInfo_getClientType },
{"native_devinfo_getModelName", "(I)Ljava/lang/String;", (void*)tcl_deviceinfo_TDeviceInfo_getModelName },
{"native_devinfo_getPanelName", "(I)Ljava/lang/String;", (void*)tcl_deviceinfo_TDeviceInfo_getPanelName },
{"native_devinfo_getPanelType", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_getPanelType },
{"native_devinfo_getPSUName", "(I)Ljava/lang/String;", (void*)tcl_deviceinfo_TDeviceInfo_getPSUName },
{"native_devinfo_getRCUName", "(I)Ljava/lang/String;", (void*)tcl_deviceinfo_TDeviceInfo_getRCUName },
{"native_devinfo_get3DStatus", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_get3DStatus },
{"native_devinfo_get3DGlassesType", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_get3DGlassesType },
{"native_devinfo_getWifiStatus", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_getWifiStatus },
{"native_devinfo_getWifiType", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_getWifiType },
{"native_devinfo_getMemc", "(I)I", (void*)tcl_deviceinfo_TDeviceInfo_getMemc },
};


int register_tcl_deviceinfo_TDeviceInfo(JNIEnv *env) {
	return jniRegisterNativeMethods(env, "com/tcl/deviceinfo/TDeviceInfo", method_table, NELEM(method_table));
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK) {
        if (NULL != env && register_tcl_deviceinfo_TDeviceInfo(env) == 0) {
            result = JNI_VERSION_1_4;
        }
    }
    return result;
}


