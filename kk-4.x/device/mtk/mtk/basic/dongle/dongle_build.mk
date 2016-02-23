#
#build control file for bcm bluetooth function by yjw
#

#add for mt5507 project IC_SETTING is mt5882
ifeq "${IC_SETTING}" ""
PALTFORM_NAME_DEF = default
else
PALTFORM_NAME_DEF = ${IC_SETTING}
endif

$(warning PALTFORM_NAME_DEF = $(PALTFORM_NAME_DEF))

DONGLE_BUILD_PATH := device/mtk/mtk/basic/dongle


PRODUCT_COPY_FILES += \
	$(DONGLE_BUILD_PATH)/dongle_commom/libiflytekdongle.so:system/lib/libiflytekdongle.so \
	$(DONGLE_BUILD_PATH)/dongle_commom/libstlport_shared.so:system/lib/libstlport_shared.so \
	$(DONGLE_BUILD_PATH)/dongle_commom/sensors.default.so:system/lib/hw/sensors.$(PALTFORM_NAME_DEF).so \
	$(DONGLE_BUILD_PATH)/dongle_commom/xfsmd:system/xbin/xfsmd \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_assist_10:system/xbin/dongle_assist_10 \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_assist_16:system/xbin/dongle_assist_16 \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_assist_17:system/xbin/dongle_assist_17 \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_assist_nfc:system/xbin/dongle_assist_nfc \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_burn:system/xbin/dongle_burn \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_init:system/xbin/dongle_init \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_version:system/xbin/dongle_version \
	$(DONGLE_BUILD_PATH)/dongle_assist/dongle_manager:system/xbin/dongle_manager \
	$(DONGLE_BUILD_PATH)/motiond/motiond:system/bin/motiond \
	$(DONGLE_BUILD_PATH)/motiond/motiond_to_tcl_libsensors:system/xbin/motiond_to_tcl_libsensors \
	$(DONGLE_BUILD_PATH)/motiond/xfsmdd:system/xbin/xfsmdd \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libfreespace.so:system/lib/libfreespace.so \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libfreespace.so.0:system/lib/libfreespace.so.0 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libfreespace.so.0.7.0:system/lib/libfreespace.so.0.7.0 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-bundle.so:system/lib/libMotionEngine-bundle.so \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-bundle.so.1:system/lib/libMotionEngine-bundle.so.1 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-bundle.so.1.8.2:system/lib/libMotionEngine-bundle.so.1.8.2 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-core.so:system/lib/libMotionEngine-core.so \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-core.so.1:system/lib/libMotionEngine-core.so.1 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-core.so.1.8.2:system/lib/libMotionEngine-core.so.1.8.2 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-ext.so:system/lib/libMotionEngine-ext.so \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-ext.so.1:system/lib/libMotionEngine-ext.so.1 \
	$(DONGLE_BUILD_PATH)/motiond/hillcrestlib/libMotionEngine-ext.so.1.8.2:system/lib/libMotionEngine-ext.so.1.8.2 \


	