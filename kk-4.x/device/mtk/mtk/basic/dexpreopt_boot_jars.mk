####################################
# Dexpreopt on the boot jars
#
####################################
# TODO: replace it with device's BOOTCLASSPATH
ifneq "$(HYBRID_MODE)" "true"
ifeq "$(ANDROID_TWO_WORLDS)" "true"                      
DEXPREOPT_BOOT_JARS := core:conscrypt:okhttp:core-junit:bouncycastle:ext:framework:framework2:telephony-common:voip-common:mms-common:android.policy:services:apache-xml:webviewchromium:com.mediatek.util:com.mediatek.dm:com.mediatek.common.capture:com.mediatek.common.PhotoPlayer:com.mediatek.twoworlds.tv:com.mediatek.common.powercontrol:com.extra.eventflow:com.mediatek.eventflow:com.mediatek.wfdsink:com.mediatek.network
else
DEXPREOPT_BOOT_JARS := core:conscrypt:okhttp:core-junit:bouncycastle:ext:framework:framework2:telephony-common:voip-common:mms-common:android.policy:services:apache-xml:webviewchromium:com.mediatek.util:com.mediatek.tv:com.mediatek.tv.custom:com.mediatek.dm:com.mediatek.common.capture:com.mediatek.record:com.mediatek.common.PhotoPlayer:com.mediatek.common.powercontrol:com.mediatek.wfdsink:com.tcl.factory:com.tcl.tvmanager:com.tcl.snapshot:android.tclwidget:com.tcl.device.authentication:com.tcl.deviceinfo:com.tcl.devicemanager:com.tcl.seeker:com.tcl.os.storage:com.tcl.os.system:com.tcl.net.ethernet:com.tcl.media:com.tcl.screenrecorder:com.tcl.adsystem:com.tcl.net.pppoe:com.tcl.tclvoicecontrol
#add by tank@tcl.com
#:com.tcl.snapshot
#end tank@tcl.com
endif
else
DEXPREOPT_BOOT_JARS := core:conscrypt:okhttp:core-junit:bouncycastle:ext:framework:framework2:telephony-common:voip-common:mms-common:android.policy:services:apache-xml:webviewchromium:com.mediatek.util:com.mediatek.dm:com.mediatek.common.capture:com.mediatek.common.PhotoPlayer:com.mediatek.common.powercontrol:com.mediatek.wfdsink
endif

