package com.tcl.deviceinfo;

import java.lang.Exception;
import android.os.SystemProperties;
import com.tcl.factory.FactoryManager;
import java.util.ArrayList;
import java.util.List;
import android.util.Log;

// Begin added by TCL lulin, mail: lulin@tcl.com
import com.tcl.tvmanager.TvManager;
import com.tcl.utility.property.Property;
import android.content.Context;
// End of TCL

/**
 *  DeviceInfo组件用于标准化管理平台相关的数据。组件负责做五件事： 
 *	<br>&Oslash;&nbsp; 负责创建/data/devinfo.txt文件，包含：DeviceID、软件版本号、ClientType、DeviceID的CRC校验码
 *	<br>&Oslash;&nbsp; 实现DeviceID的双备份，开机对DeviceID进行校验
 *	<br>&Oslash;&nbsp; 为用户提供平台相关的数据并通过ProjectID来区分这些数据，实现一套软件兼容多个ProjectID的目的
 *	<br>&Oslash;&nbsp; 平台数据进行双备份：为了确保平台数据的安全性，对数据进行双备份。系统启动时对数据进行校验，如果数据校验失败，则从备份中恢复
 *	<br>&Oslash;&nbsp; 为应用提供工厂相关的接口。应用要想获取工厂数据，不能直接调用工厂菜单接口，需通过DeviceInfo组件的接口来获取，以确保数据的安全性
 */
public class TDeviceInfo {
	private static final String TAG = "TCL_DeviceInfo";
	private static final boolean LOGI = true;
	private static TDeviceInfo mDeviceInfo;
	private static FactoryManager mFactoryManager;

	static {
		System.loadLibrary("deviceinfo_jni");
	}

    /**
     * 获取DeviceInfo的实例对象
     * @param 无
     * @return 返回一个DeviceInfo的对象
     */
	public static TDeviceInfo getInstance() {
		if(mDeviceInfo == null) {
			mDeviceInfo = new TDeviceInfo();
		}
		return mDeviceInfo;
	}

    /**
     * 获取设备的硬件版本 
     * @param 无
     * @return 返回硬件版本号
     */
	public String getHardwareVersion() {
		String mRet = "";
		try {
			mRet = SystemProperties.get("ro.hardware.version_id");
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取系统的软件版本 
     * @param 无
     * @return 返回系统软件版本号
     */
	public String getSoftwareVersion() {
		String mRet = "";
		try {
			mRet = SystemProperties.get("ro.software.version_id");
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取ClientType，建议用getClientType()方法代替 
     * @param  无
     * @return 返回当前平台ClientType
     */
	 @Deprecated
	public String getDeviceModel() {
		return getClientType(getProjectID());
	}

    /**
     * 获取DeviceID 
     * @param  无
     * @return 返回当前平台DeviceID
     */
	public String getDeviceID() {
		String mRet = "";
		try {
			mRet = mFactoryManager.doGetDeviceID();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if(LOGI) Log.i(TAG, "getDeviceID:" + mRet);
		return mRet;
	}

    /**
     * 用于恢复出厂设置清理平台数据，恢复到出厂状态 
	 * 该方法根据不同平台需要单独实现
     * @param  无
     * @return 无
     */
	public void cleanE2promInfo() {
		try {
			mFactoryManager.doCleanChannelList();
			mFactoryManager.doResetUserSettings();
		} catch (Exception e){
			e.printStackTrace();
		}
	}

    /**
     * 获取ProjectID 
     * @param  无
     * @return 返回当前平台的ProjectID，默认值为：0
     */
	public int getProjectID() {
		int mRet = 0;
		try {
			mRet = mFactoryManager.doGetAttribute(FactoryManager.FACTORY_ATTR_PROJ_ID);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取ClientType 
     * @param int prjid，当前平台的ProjectID 
     * @return 返回当前平台ClientType
     */
	public String getClientType(int prjid) {
		String mRet = "";
		try {
			mRet = native_devinfo_getClientType(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取平台的机型名称 
     * @param int prjid，当前平台的ProjectID
     * @return 返回平台的机型名称
     */
	public String getModelName(int prjid) {
		String mRet = "";
		try {
			mRet = native_devinfo_getModelName(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取屏的名称 
     * @param int prjid，当前平台的ProjectID
     * @return 返回电视使用的屏的名称
     */
	public String getPanelName(int prjid) {
		String mRet = "";
		try {
			mRet = native_devinfo_getPanelName(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取屏的类型
     * @param int prjid，当前平台的ProjectID
     * @return 返回屏的类型，4K2K返回：1， 2K1K返回：0
     */
	public int getPanelType(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_getPanelType(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取电源的名称
     * @param int prjid，当前平台的ProjectID
     * @return 返回电源的名称
     */
	public String getPSUName(int prjid) {
		String mRet = "";
		try {
			mRet = native_devinfo_getPSUName(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取遥控器的名称
     * @param int prjid，当前平台的ProjectID
     * @return 返回遥控器的名称
     */
	public String getRCUName(int prjid) {
		String mRet = "";
		try {
			mRet = native_devinfo_getRCUName(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断当前平台支不支持3D功能
     * @param int prjid，当前平台的ProjectID
     * @return 支持返回：1， 否则返回：0
     */
	public int get3DStatus(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_get3DStatus(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取所使用的3D眼镜类型
     * @param int prjid，当前平台的ProjectID
     * @return 快门眼镜返回：1， 偏光眼镜返回：0
     */
	public int get3DGlassesType(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_get3DGlassesType(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断支不支持Wifi功能
     * @param int prjid，当前平台的ProjectID
     * @return 支持返回：1，否则返回：0
     */
	public int getWifiStatus(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_getWifiStatus(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取Wifi类型
     * @param int prjid，当前平台的ProjectID
     * @return 内置Wifi：1，外置Wifi返回：0
     */
	public int getWifiType(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_getWifiType(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断当前平台是否支持运动补偿功能
     * @param int prjid，当前平台的ProjectID
     * @return 支持返回：1，不支持返回：0
     */
	public int getMemc(int prjid) {
		int mRet = -1;
		try {
			mRet = native_devinfo_getMemc(prjid);
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断工厂D模式是否打开
     * @param 无
     * @return 打开返回：<code>true</code>；否则返回：<code>false</code>
     */
	public boolean getFactoryDModeStatus() {
		boolean mRet = false;
		try {
			mRet = (mFactoryManager.doGetAttribute(FactoryManager.FACTORY_ATTR_D_MODE) == 1)?true:false;
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断工厂P模式是否打开
     * @param 无
     * @return 打开返回：<code>true</code>；否则返回：<code>false</code>
     */
	public boolean getFactoryPModeStatus() {
		boolean mRet = false;
		try {
			mRet = (mFactoryManager.doGetAttribute(FactoryManager.FACTORY_ATTR_P_MODE) == 1)?true:false;
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 判断工厂W模式是否打开
     * @param 无
     * @return 打开返回：<code>true</code>；否则返回：<code>false</code>
     */
	public boolean getFactoryWModeStatus() {
		boolean mRet = false;
		try {
			mRet = (mFactoryManager.doGetAttribute(FactoryManager.FACTORY_ATTR_WARM_UP_MODE) == 1)?true:false;
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取产品注册码
     * @param 无
     * @return 返回产品注册码
     */
	public String getRegistrationCode() {
		String mRet = "";
		try {
			mRet = mFactoryManager.doGetRegisterCode();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if(LOGI) Log.i(TAG, "getRegistrationCode:" + mRet);
		return mRet;
	}

	/**
     * 获取TV厂商名称
     * @param 无
     * @return 返回TV厂商名称，默认值为：TCL Multimedia<br>
     * TV厂商名称以“TCL+加公司名称”进行命名
     */
	public String getTVCompany() {
		String mRet = "TCL Multimedia";
		try {
			mRet = SystemProperties.get("ro.build.company");
		} catch (Exception e){
			e.printStackTrace();
		}
		return mRet;
	}

    /**
     * 获取ProjectID数目列表
	 * <br>通过ProjectID数目列表可以知道有都少个ProjectID以及ProjiectID值是多少
     * @param 无
     * @return 返回：List<Integer>类型的ProjectID数目列表
     */
	public List<Integer> getProjectList() {
		List<Integer> mRet = new ArrayList<Integer>();
		int mTemp[] = native_devinfo_getProjectList();
		for(int i=0;i<mTemp.length;i++){
			mRet.add(mTemp[i]);
		} 
		return mRet;
	}

    /**
     * 设置ProjectID
     * @param 需要设置的ProjectID值
     * @return 无
     */
	public void setProjectID(int projectID) {
		try {
			mFactoryManager.doSetAttribute(FactoryManager.FACTORY_ATTR_PROJ_ID, projectID);
		} catch (Exception e){
			e.printStackTrace();
		}
	}

    /**
     * 获取MAC地址
     * @param 无
     * @return 返回MAC地址
     */
	public String getMACAddress() {
		String mRet = "";
		try {
			mRet = mFactoryManager.doGetMacAddress();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if(LOGI) Log.i(TAG, "getMACAddress:" + mRet);
		return mRet;
	}
	
	/**
     * 获取城市名称
     * @param 无
     * @return 返回城市名称
     */
	public String getCityName() {
		String mRet = "";
		try {
			mRet = Utils.getCityName();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if(LOGI) Log.i(TAG, "getCityName:" + mRet);
		return mRet;
	}
	
	/**
     * 设置城市名称
     * @param 城市名称
     * @return 无
     */
	public void setCityName(String mCityName) {
		try {
			Utils.setCityName(mCityName);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}


	/*
        .. SN .....
     * @param .
     * @return String  
     add by lulin mail lul@tcl.com
   */
	public String getSn() {
		String mRet = "";
		try {
			mRet = mFactoryManager.doGetSnCode();
		} catch (Exception e) {
			e.printStackTrace();
		}
		if(LOGI) Log.i(TAG, "getSncode:" + mRet);
		return mRet;
	}
 
		/*
        .. UUID 
     * @param .
     * @return String  
     add by lulin mail lul@tcl.com
   */
       public String getDeviceUUID(Context context) {
            TvManager tvm = TvManager.getInstance(context);
            Property tvmProp = tvm.getProperty();

            String mRet = tvmProp.get("persist.app.auth.UUID","");

            if(LOGI) Log.i(TAG, "get UUID:" + mRet);
            return mRet;
        }
     
     /*
        .. UUID 
     * @param .
     * @return String  
     add by lulin mail lul@tcl.com
   */
        public void setDeviceUUID(Context context, String uuid) {
            TvManager tvm = TvManager.getInstance(context);
            Property tvmProp = tvm.getProperty();

            tvmProp.set("persist.app.auth.UUID", uuid);
   
            if(LOGI) Log.e(TAG, "set UUID");
           
        }
        // End of TCL
	private TDeviceInfo() {
		try {
			mFactoryManager = FactoryManager.getInstance(null);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/*
	 *init generate devinfo.txt 
	 *@hide
	 */
	public void init(){
		try {
			//check device info database
			native_devinfo_checkDatabase();
			//create file "/data/devinfo.txt"
			Utils.createDeviceInfoFile();
		} catch (Exception e){
			e.printStackTrace();
		}
	}

	private static native void native_devinfo_checkDatabase();
	private static native String native_devinfo_getClientType(int projectID);
	private static native String native_devinfo_getModelName(int projectID);
	private static native String native_devinfo_getPanelName(int projectID);
	private static native int native_devinfo_getPanelType(int projectID);
	private static native String native_devinfo_getPSUName(int projectID);
	private static native String native_devinfo_getRCUName(int projectID);
	private static native int native_devinfo_get3DStatus(int projectID);
	private static native int native_devinfo_get3DGlassesType(int projectID);
	private static native int native_devinfo_getWifiStatus(int projectID);
	private static native int native_devinfo_getWifiType(int projectID);
	private static native int native_devinfo_getMemc(int projectID);
	private static native int[] native_devinfo_getProjectList();

}
