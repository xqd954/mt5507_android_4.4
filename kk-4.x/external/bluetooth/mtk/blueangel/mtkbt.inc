
#BT_PATH := external/bluetooth/mtk/blueangel
MTK_BT_EXTERNAL_PLATFORM :=HP_WOLVERINE
ifneq ($(MTK_TABLET_HARDWARE), )
MTK_HWC_CHIP = $(shell echo $(MTK_TABLET_HARDWARE) | tr A-Z a-z )
else
MTK_HWC_CHIP = $(shell echo $(MTK_PLATFORM) | tr A-Z a-z )
endif

#Bluetooth config
BT_TRANSPORT_DRV := yes

LOCAL_C_INCLUDES += \
	$(BT_PATH)/include \
	$(BT_PATH)/include/common \
	$(BT_PATH)/include/common/default \
	$(BT_PATH)/include/profiles \
	$(BT_PATH)/include/pal \
	$(BT_PATH)/bt_cust/inc \
	$(BT_PATH)/../driver/inc
	
ifeq ($(MTK_BT_CHIP),MTK_MT6611)
LOCAL_CFLAGS += \
	-D__BTMODULE_MT6611__
else ifeq ($(MTK_BT_CHIP),MTK_MT6612)
LOCAL_CFLAGS += \
	-D__BTMODULE_MT6612__
else ifeq ($(MTK_BT_CHIP),MTK_MT6616)
LOCAL_CFLAGS += \
	-D__BTMODULE_MT6616__
else ifeq ($(MTK_BT_CHIP),MTK_MT6620)
LOCAL_CFLAGS += \
	-D__BTMODULE_MT6620__ \
	-D__BT_BTWIFI_COMBO_CHIP__
else ifeq ($(MTK_BT_CHIP),MTK_MT6628)
LOCAL_CFLAGS += \
	-D__BT_BTWIFI_COMBO_CHIP__	
endif

ifeq ($(strip $(MTK_BT_EXTERNAL_PLATFORM)), HP_WOLVERINE)
LOCAL_CFLAGS += \
    	-D__MTK_BT_EXTERNAL_PLATFORM__ \
    	-DMTK_BSP_PACKAGE
    	#-DDETECT_BTUSB_RESUME \
    	

BT_TRANSPORT_DRV := yes
#BT_LIBUSB:= no
endif

#ifeq ($(BT_LIBUSB), yes)
#LOCAL_CFLAGS += \
#	-D__BT_LIBUSB__
#endif
LOCAL_CFLAGS += \
	-DLINUX \
	-DANDROID \
	-g \
	-fshort-wchar

	
LOCAL_CFLAGS += \
	-D__BTMTK__ \
	-D__BT_VER_30__ \
	-D__BT_GAP_PROFILE__ \
	-D__BT_A2DP_PROFILE__ \
	-D__BT_HFG_PROFILE__ \
	-D__BT_BIP_PROFILE__ \
	-D__BT_BIPI_PROFILE__ \
	-D__BT_BIPR_PROFILE__ \
	-D__XML_SUPPORT__ \
	-D__HF_V15__ \
	-DVR_ENABLE \
	-DNO_SEPARATE_HFG \
	-D__BT_SPP_PROFILE__ \
	-D__BT_GOEP_PROFILE__ \
	-D__BT_OPP_PROFILE__ \
	-D__BT_FTS_PROFILE__ \
	-D__BT_FTC_PROFILE__ \
	-D__BT_PBAP_PROFILE__ \
	-D__BT_BPP_PROFILE__ \
	-D__BT_AVRCP_PROFILE__ \
	-D__BT_SPP_SRV_NO_SCO__ \
	-D__BT_SPP_CLI_NO_SCO__ \
	-D__BT_SPP_GENERAL__ \
	-D__BT_DUN_PROFILE__ \
	-D__BT_HIDD_PROFILE__ \
	-D__BT_HIDH_PROFILE__ \
	-D__BT_GATTC_PROFILE__ \
	-D__BT_GATTS_PROFILE__ \
	-D__SPP_SHARED_MEMORY__ \
	-D__BT_SIM_PROFILE__ \
	-D__BT_JSR82_L2RF__ \
	-D__JSR82_SHARED_MEMORY__ \
	-D__JSR82_DYNAMIC_MEMORY__ \
	-D__JSR82_SESSION_DYNAMIC_MEMORY__ \
	-D__USE_BT_RING_BUF_API__ \
	-D__BT_MAPS_PROFILE__ \
	-D__BT_MAP_MNS_SUPPORT__ \
	-D__BT_JSR82__ \
	-D__BT_PAN_PROFILE__ \
	-D__PAN_PTS_TEST__ \
	-D__ENABLE_BTNET_DEV__ \
	-D__LINUX_SUPPRESS_ERROR__ \
	-DBTMTK_ON_LINUX \
	-D__HFG_BLOCK_SIMULTANEOUS_CONNECTION__ \
	-D__SDAP_REQUEST_QUEUE__ \
	-D__SDAP_MODIFIED_SEARCH_ALL__ \
	-D__USE_CATCHER_LOG__ \
	-D__BT_MCAP__ \
	-D__BT_HDP_PROFILE__ \
	-D__HDP_FD_SUPPORT__ \
	-D__SDP_EXIT_SNIFF_MODE__ \
	-D__BT_HOGP_PTS_TEST__ \

LOCAL_LDFLAGS += -Wl,--no-fatal-warnings

LOCAL_CFLAGS += -D__BT_BNEP__
#LOCAL_CFLAGS += -D__BT_GAVDP_SEP_CHNL_DGB__


ifeq ($(MTK_COMBO_QUICK_SLEEP_SUPPORT),yes)
LOCAL_CFLAGS += \
        -D__BT_A2DP_LOW_POWER__
endif


ifeq ($(MTK_BT_POWER_EFFICIENCY_ENHANCEMENT),yes)
LOCAL_CFLAGS += \
        -D__BT_A2DP_LOW_POWER__
endif




ifeq ($(BT_TRANSPORT_DRV), yes)
LOCAL_CFLAGS += \
	-D__BT_TRANSPORT_DRV__
endif

LOCAL_CFLAGS += \
	-D__EXTRA_DEBUG_FEATURE__

LOCAL_CFLAGS += \
	-DWIFI_BB_MT5921 \
	-D__DIRECT_CHNL_MAP__ \

ifeq ($(MTK_WLANBT_SINGLEANT), yes)
LOCAL_CFLAGS += \
	-D__BT_SINGLE_ANTENNA__
endif
#LOCAL_CFLAGS += \
#	-D__BT_HS_30__

ifeq ($(TARGET_PRODUCT),generic)
LOCAL_CFLAGS += \
	-D__ANDROID_EMULATOR__
else

ifeq ($(MTK_BT_CHIP),MTK_MT6620)
else
#LOCAL_CFLAGS += \
#	-D__ENABLE_AFH_PTA_HANDLING__
ifneq ($(BT_TRANSPORT_DRV), yes)
LOCAL_CFLAGS += \	
	-D__ENABLE_SLEEP_MODE__
endif	
endif
endif

ifeq ($(MTK_BT_PROFILE_AVRCP13),yes)
LOCAL_CFLAGS += \
	-D__BT_AVRCP_V13__
endif

#ifeq ($(MTK_BT_30_HS_SUPPORT), yes)
LOCAL_CFLAGS += \
        -D__BT_3_0_HS__ \
        -D__BT_HS_30__\
        -D__BT_GOEP_V2__ 

ifeq ($(MTK_BT_PROFILE_AVRCP14),yes)
LOCAL_CFLAGS += \
	-D__BT_AVRCP_V14__ 
endif
#endif

# always enable LE in mtkbt and let upper layer to decide use LE or not
#ifeq ($(MTK_BT_40_SUPPORT),yes)
LOCAL_CFLAGS += \
	-D__BT_4_0_BLE__ \
	-D__BT_PRXR_PROFILE__ \
	-D__BT_PRXM_PROFILE__ \
#	-D__BT_TIMEC_PROFILE__ \
#	-D__BT_TIMES_PROFILE__
#endif

ifeq ($(MTK_BT_40_LE_STANDALONE),yes)
LOCAL_CFLAGS += \
	-D__BT_LE_STANDALONE__
endif

LOCAL_CFLAGS += \
        -D__MTK_BT_DUAL_PCM_SWITCH_SUPPORT__

#ifeq ($(MTK_BT_SKIP_GATT_SERVICE),yes)
#LOCAL_CFLAGS += \
#  -D__MTK_BT_SKIP_GATT_SERVICE__
#endif

ifeq ($(MTK_EMMC_SUPPORT),yes)
LOCAL_CFLAGS += \
	-D__MTK_EMMC_SUPPORT__
endif

LOCAL_CFLAGS += \
	-D__BT_TEST_MODE_ENABLED__ \
	-D__BTTM_USE_ORIGINAL_PORT__

LOCAL_CFLAGS += \
	-D__BT_USE_CUST_DATA__ \
	-D__ENABLE_HFG_SDP_CACHE__

ifeq ($(MTK_INTERNAL),yes)
LOCAL_CFLAGS += \
	-D__INTERNAL_LOAD__
endif

LOCAL_CFLAGS += \
	-D__INDEPENDENT_INQUIRY_FLOW__

LOCAL_CFLAGS += \
	-Wno-missing-field-initializers \
	-Wno-missing-braces \
	-Wno-strict-aliasing


LOCAL_CFLAGS += \
	-D__KEEP_HOST_AWAKE__

ifeq ($(MTK_BT_AUTOTEST),yes)
LOCAL_CFLAGS += \
	-D__BQE_TEST__ \
	-D__DYNAMIC_CUST_CONFIG__ \
#	-D__ATT_TS_SMALL_DATABASE__ \
#	-D__ATT_TS_LARGE_DATABASE1__ \
#	-D__ATT_TS_LARGE_DATABASE2__ \
#	-D__ATT_TS_LARGE_DATABASE3__

endif
