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

BT_BCM_BUILD_PATH := device/mtk/mtk/basic/bt_bcm


PRODUCT_COPY_FILES += \
	$(BT_BCM_BUILD_PATH)/etc/auto_pair_devlist.conf:system/etc/bluetooth/auto_pair_devlist.conf \
	$(BT_BCM_BUILD_PATH)/etc/bcm20702b0.hcd:system/etc/bluetooth/bcm20702b0.hcd \
	$(BT_BCM_BUILD_PATH)/etc/bt_did.conf:system/etc/bluetooth/bt_did.conf \
	$(BT_BCM_BUILD_PATH)/etc/bt_stack.conf:system/etc/bluetooth/bt_stack.conf \
	$(BT_BCM_BUILD_PATH)/etc/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf \
	$(BT_BCM_BUILD_PATH)/lib/libbt-hci.so:system/lib/libbt-hci.so \
	$(BT_BCM_BUILD_PATH)/lib/libbt-utils.so:system/lib/libbt-utils.so \
	$(BT_BCM_BUILD_PATH)/lib/libbt-vendor.so:system/lib/libbt-vendor.so \
	$(BT_BCM_BUILD_PATH)/lib/hw/bluetooth.default.so:system/lib/hw/bluetooth.default.so \
	$(BT_BCM_BUILD_PATH)/lib/hw/audio.a2dp.default.so:system/lib/hw/audio.a2dp.default.so \
	$(BT_BCM_BUILD_PATH)/modules/btusb.ko:system/lib/modules/btusb.ko \
	
