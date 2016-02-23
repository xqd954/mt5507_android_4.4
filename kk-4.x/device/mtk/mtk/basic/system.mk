ifeq "$(ANDROID_BROWSER_CACHE_POLICY)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.browser.cachePolicy=1
endif

ifeq "$(ANDROID_SWITCH_FBM)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.switchfbm=1
endif

#ifeq "$(ENLARGE_PNG_HW_THRESHOLD)" "true"
# for CTS test, default threshold is 100k, set to 500k for CTS test ok
PRODUCT_PROPERTY_OVERRIDES += png.hw.threshold=512000
#endif

ifeq "$(ANDROID_PNG_SW_DECODE)" "true"
# for PNG HW decode, default threshold is 1024k, set to 9999999 for SW decode
PRODUCT_PROPERTY_OVERRIDES += png.hw.threshold=9999999
endif

ifeq "$(ANDROID_JPG_SW_DECODE)" "true"
# for JPG HW decode, default threshold is ??k, set to 9999999 for SW decode
PRODUCT_PROPERTY_OVERRIDES += jpg.hw.threshold=9999999
endif

ifeq "$(SUPPORT_4K2K)" "true"
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.4k2k.photo=1
else
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.4k2k.photo=0
endif


ifeq "$(SUPPORT_ROTATE)" "true"
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.disable_scrn_rt=0
else
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.disable_scrn_rt=1
endif

ifeq "$(SUPPORT_FULLSCREEN)" "true"
# for pip with activity fullscreen change,default value 0,if ro.mtk.pip.fullscreen=0,pip position will change when activity fullscreen change, =1 not change
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.pip.fullscreen=1
else
	PRODUCT_PROPERTY_OVERRIDES += ro.mtk.pip.fullscreen=0
endif



ifeq "$(ANDROID_MEM_DBG)" "true"
PRODUCT_PROPERTY_OVERRIDES += libc.debug.malloc=1
endif
ifeq  "$(ANDROID_BROWSER_USE_CMPB_PLAYER)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.useCmpbPlayer=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.useCmpbPlayer=0
endif

ifeq  "$(ANDROID_BROWSER_ALWAYS_FULLSCREEN)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.alwaysFullscreen=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.alwaysFullscreen=0
endif

ifeq  "$(ANDROID_BROWSER_OPEN_LINK_SAME_TAB)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.openLinkInSameTab=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.openLinkInSameTab=0
endif

ifeq  "$(ANDROID_BROWSER_RELEASE_PLAYER_WHEN_ON_PAUSE)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.freePlayerWhenPause=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.freePlayerWhenPause=0
endif

ifeq  "$(ANDROID_BROWSER_SKIP_USER_AGENT_BLACK_LIST)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.skipUABlackList=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.skipUABlackList=0
endif

ifeq  "$(ANDROID_BROWSER_LOW_MEMORY_MODE)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.lowMemoryMode=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.browser.lowMemoryMode=0
endif

PRODUCT_PROPERTY_OVERRIDES += ro.mtk.pkg_version=$(VERSION)
ifeq "$(AUTO_TEST)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.auto_test=1
    PRODUCT_PROPERTY_OVERRIDES += btchk.logcat.need=1
	PRODUCT_PROPERTY_OVERRIDES += ro.adb.secure=0
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.auto_test=0
# for cts: android.os.cts.BuildVersionTest.testReleaseVersion
	PRODUCT_PROPERTY_OVERRIDES += ro.adb.secure=0
endif

ifeq "$(SYSTEMUI_EXISTED)" "false"
	PRODUCT_PROPERTY_OVERRIDES += ro.def_systemui_existed=0
else
	PRODUCT_PROPERTY_OVERRIDES += ro.def_systemui_existed=1
endif

ifeq "$(DM_VERITY)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.dmv=1
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.dmv=0
endif

ifeq "$(SUPPORT_CAMERA_BACK)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.camera.facingback=1
else
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.camera.facingback=0
endif
ifeq  "$(CUSTOMER_AP)" "TKUI"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.tkui=1
    PRODUCT_PROPERTY_OVERRIDES += mtk.tkui.suspend=0
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.tkui=0
endif

ifeq "$(USE_CMPB_AS_GALLERY_PLAYER)" "true"
    PRODUCT_PROPERTY_OVERRIDES += use.cmpb.in.gallery=1
else
    PRODUCT_PROPERTY_OVERRIDES += use.cmpb.in.gallery=0
endif

ifeq "$(SUPPORT_SCREEN_LOCK)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.disable.screen.lock=0
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.disable.screen.lock=1
endif

ifeq "$(ANDROID_TWO_WORLDS)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.twoworlds=1
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.twoworlds=0
endif

ifeq "$(DISABLE_LONG_PRESS_DO_STANDBY)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.no_long_press_do_standby=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.no_long_press_do_standby=0
endif

PRODUCT_PROPERTY_OVERRIDES += mtk.pms.no_useraction_timeout=5

PRODUCT_PROPERTY_OVERRIDES += mtk.pms.suspend_wait_timeout=10

PRODUCT_PROPERTY_OVERRIDES += mtk.pms.force_release_wakelock=0

ifeq "$(SYS_DRIVER_ONLY)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.sys_driver_only=1
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.sys_driver_only=0
endif

# Used to distinguish Emulator(Eclipse)and Mtk board. 
# When in MTK board property is 1, When in Emulator(Eclipse),will not include this cfg, so property is 0
PRODUCT_PROPERTY_OVERRIDES += mtk.inside=1

ifeq "$(ANDROID_SW_RENDERING)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.kernel.qemu=1
PRODUCT_PROPERTY_OVERRIDES += ro.kernel.qemu.gles=0
endif

ifeq "$(ANDROID_512MB)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.dram512=1
endif

ifeq "$(CTS_TEST_WITH_SYSTEMUI)" "false"
    PRODUCT_PROPERTY_OVERRIDES += ro.cts_test_with_systemui=0
else
    PRODUCT_PROPERTY_OVERRIDES += ro.cts_test_with_systemui=1
endif

ifeq "$(HIDE_NAVIGATION_BAR)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.hide_navigation_bar=1
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.hide_navigation_bar=0
endif

ifeq "$(CTS_TEST)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.cts_test=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.cts_test=0
endif

ifeq "$(STRESS_TEST)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.stress_test=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.stress_test=0
endif

ifeq "$(TMS_SUPPORT)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.tms_support=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.tms_support=0
endif

ifeq "$(AWESOMEPLAYER_DEBUG)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.awesomePlayer.dumpbs=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.awesomePlayer.dumpbs=0
endif

ifeq "$(OMX_DEBUG)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.omx.verbose.log=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.omx.verbose.log=0
endif

ifeq "$(BGM_CUSTOM_FLOW)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.bgm_custom_flow=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.bgm_custom_flow=0
endif

ifeq "$(MSI)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.multi_screen_interaction=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.multi_screen_interaction=0
endif

ifeq "$(MHL_SUPPORT)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.isSupportMHL=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.isSupportMHL=0
endif

##For WW AP start

ifneq ( ,$(findstring _us, $(MODEL)))
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.marketregion=us
else ifneq ( ,$(findstring _sa, $(MODEL)))
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.marketregion=sa
else ifneq ( ,$(findstring _eu, $(MODEL)))
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.marketregion=eu
else
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.marketregion=cn
endif

ifeq "$(SYS_MHP_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.mhp.existed=1
endif

ifeq "$(SYS_DVBS_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.dvbs.existed=1
endif

ifeq "$(HBBTV_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.hbbtv.existed=1
endif

ifeq "$(SYS_TTX_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.ttx.existed=1
endif

ifeq "$(SYS_SUBTITLE_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.subtitle.existed=1
endif

ifeq "$(SYS_CI_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.ci.existed=1
endif

ifeq "$(SYS_MHEG5_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.mheg5.existed=1
endif

ifeq "$(SYS_OAD_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.oad.existed=1
endif

ifeq "$(GINGA_SUPPORT)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.ginga.existed=1
endif

ifeq "$(AV_RECORD_LIB)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.av_record_lib=1
endif

##For WW AP end

## mtk keyEvent disable ##
PRODUCT_PROPERTY_OVERRIDES += sys.mtk.keyDisable=0

# original flow:    2 different copys of atv, stored in cable and air svl separately
# now default flow: 1 copy atv, stored in cable svl (follow the change of customers)
ifeq "$(ANDROID_AIR_CABLE_TWO_ATV)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.isOnlyCableAtv=0
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.isOnlyCableAtv=1
endif

# HW video encoder support
ifeq "$(HW_VENC_SUPPORT)" "true"
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.isSupportHwVenc=1
else
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk.isSupportHwVenc=0
endif

ifeq "$(BOOT_LOG_NEEDED)" "true"
	PRODUCT_PROPERTY_OVERRIDES += mtk.boot_log_needed=1
else
	PRODUCT_PROPERTY_OVERRIDES += mtk.boot_log_needed=0
endif

PRODUCT_PROPERTY_OVERRIDES += ro.mtk.system.audiosync=1
ifeq "$(ETHERNET_DRV_BUILTIN)" "true"
	PRODUCT_PROPERTY_OVERRIDES += net.ethernet.type=1
else
	PRODUCT_PROPERTY_OVERRIDES += net.ethernet.type=0
endif

ifeq "$(QTAGUID_ENABLE)" "true"
    PRODUCT_PROPERTY_OVERRIDES += net.qtaguid_enabled=1
else
    PRODUCT_PROPERTY_OVERRIDES += net.qtaguid_enabled=0
endif

ifeq "$(ENABLE_TIME_SHIFT)" "true"
    PRODUCT_PROPERTY_OVERRIDES += mtk.tkui.tshift=1
else
    PRODUCT_PROPERTY_OVERRIDES += mtk.tkui.tshift=0
endif

# Include Overlay resource
PRODUCT_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlays
