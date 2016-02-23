package com.mediatek.ui.cec;

import android.R.integer;
import android.R.string;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.pm.PackageParser.NewPermissionInfo;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;

import com.mediatek.ui.util.GetCurrentTask;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.mmpcm.device.DevListener;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.CecCompListener;
import com.mediatek.tv.model.CecDevDiscoveryInfo;
import com.mediatek.tv.model.CecDeviceInfo;
import com.mediatek.tv.model.CompListener;
import com.mediatek.tv.service.CecComponentService;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tv.service.ServiceManager;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem;
import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem.DataType;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.tvcommon.TVCommonManager;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class CecManager implements CecCompListener{
    private static final String TAG = "CecManager";

    public static final int tvWidth = 1920;
    public static final int tvHeight = 1080;
    private Context mContext;
    /***
     * static data used for CEC
     */
    private boolean isCECWorking = false;

    private List<CecListviewAdapter.CecDataItem> deviceGroup =null;
    private List<CecListviewAdapter.CecDataItem> tempDeviceGroup =null;
    private List<CecListviewAdapter.CecDataItem> attributeGroup =null;
    private List<CecListviewAdapter.CecDataItem> setupGroup =null;

    private static CecManager staticManager;
    private CecHandler mHandler;
    private CecComponentService cecService;
    private TVContent mTvContent;
    protected TVManager tvManager;
    private TVInputManager mInputManager ;
     private TVCommonManager mTVCMManager;
    private List<CecDeviceInfo> deviceList = new ArrayList<CecDeviceInfo>();
    private int selectedDevice = 0;
    private MenuConfigManager mConfigManager;

    public static final String CEC_Setup  = "cec_setup";
    public static final String CEC_Standby_ID  = "cec_standby";
    public static final String CEC_Poweron_ID  = "cec_poweron";
    public static final String CEC_Able_ID  = "cec_able";
    private String[] cec_onoff = null;

    private static final int DeviceType_dvdplayer = 1;
    private static final int DeviceType_amplifier = 2;
    private static final int DeviceType_MHL = 3;

    public static final int Volume_UP = 2001;
    public static final int Volume_Down = 2002;
    public static final int Volume_Mute = 2003;
    private String volume_amp = "";
    private boolean volume_amp_mute = false;
    private String sourceString = "";
    private boolean Amp_Connect = false;

    public static final int Refresh_DeviceList = 1001;
    public static final int Show_WaitDialog = 1011;
    public static final int DissMiss_WaitDialog = 1012;
    public static final int Change_Source = 1013;
    public static final int Change_Volume = 1014;
    public static final int Change_Volume_Mute = 1015;
    public static final int LOAD_DEVICE = 1016;

    private boolean amp_conncet = false;
    private boolean showDialog = false;
    private EPGProgressDialog mDialog = null;

    public static CecManager getInstance(Activity activity) {
        if (staticManager == null) {
            staticManager = new CecManager(activity);
        }
        return staticManager;
    }

    public static CecManager getInstance() {
        if (staticManager != null) {
            return staticManager;
        }else {
            CecUtil.log("TimeShiftManager: getInstance, return null;");
            return null;
        }
    }

    public CecManager (Activity activity){
    	showDialog = false;
        mContext = activity;
        mHandler = new CecHandler();
        mTvContent = TVContent.getInstance(mContext);
        tvManager = TVManager.getInstance();
        mInputManager = mTvContent.getInputManager();
        mTVCMManager = TVCommonManager.getInstance(mContext);
        cecService = (CecComponentService)ComponentService.getSpecificComponet(CecComponentService.CecComponentServiceName);
        mConfigManager = MenuConfigManager.getInstance(mContext);
        cec_onoff = mContext.getResources().getStringArray(R.array.menu_cec_on_off);
        Log.e("cecserviec", "cecservice:"+cecService);
        cecService.addListener(this);
        try{
            cecService.updateSysStatus(CecComponentService.CEC_COMP_SET_STATUS_POWER_ON);
        }
        catch(Exception ex){
            ex.printStackTrace();
        }
    }

    @Override
    public void CecCompNotifyInfo(String MsgContent, int dataType, String info) {

        CecUtil.log("MsgContent:"+MsgContent);
        if (MsgContent.equals(CecComponentService.CEC_COMP_POWER_ON)) {
            //info
            //change input source
            CecUtil.log("MsgContent:"+"CecCompNotifyInfo:"+"change source:"+dataType+"_info:"+info);
            sourceString = info;
            mHandler.sendEmptyMessageDelayed(CecManager.Change_Source,500);

        }else if (MsgContent.equals(CecComponentService.CEC_COMP_AMP_CONNECTED)) {
            //dataType 1,on ,
            CecUtil.log("MsgContent:"+"CecCompNotifyInfo:"+"connect:"+dataType+"_info:"+info);
            if (dataType==1) {
                Amp_Connect = true;
            }else {
                Amp_Connect = false;
            }

        }else if (MsgContent.equals(CecComponentService.CEC_COMP_DISCOVERY_FINISH)) {
            // refresh menu,
            CecManager.getInstance().getCecHandler().sendEmptyMessageDelayed(CecManager.Refresh_DeviceList,100);

        }else if (MsgContent.equals(CecComponentService.CEC_COMP_ERROR_MESSAGE)) {
            //info
        }else if (MsgContent.equals(CecComponentService.CEC_COMP_AMP_VOLUME)){
          //dataType ,return volume value .
            CecUtil.log("MsgContent:"+"CecCompNotifyInfo:"+"connect:"+dataType+"_info:"+info);
            volume_amp = dataType+"";
            mHandler.sendEmptyMessage(CecManager.Change_Volume);
        }else if (MsgContent.equals(CecComponentService.CEC_COMP_AMP_MUTE_STATUS)){
         // mute .dataType , on 1.
            if (dataType==1) {
                volume_amp_mute = true;
            }else {
                volume_amp_mute = false;
            }
            mHandler.sendEmptyMessage(CecManager.Change_Volume_Mute);
        }
    }

    public CecComponentService getCecService(){
        if (null!=cecService) {
            return cecService;
        }else {
            return (CecComponentService)ComponentService.getSpecificComponet(CecComponentService.CecComponentServiceName);
        }
    }

    public class CecHandler extends Handler{

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Refresh_DeviceList:
                    loadDeviceDataGroup();
                   /* if (cecMainDialog != null && deviceGroup != null) {
                        cecMainDialog.setAdapter(deviceGroup);
                    }*/
                    mHandler.sendEmptyMessage(CecManager.DissMiss_WaitDialog);
                    break;
                case DissMiss_WaitDialog:
                	showDialog = false;
                    dissmissWaitDialog();
                    break;
                case Show_WaitDialog:
                	showDialog = true;
                    showWaitDialog();
                case 1212:
                    showCecDialog();
                    break;
                case CecManager.Change_Source:
                    changeSource(sourceString);
                    break;
                case CecManager.Change_Volume:
                    if (TurnkeyUiMainActivity.getInstance().getVolumeView()!=null) {
                        TurnkeyUiMainActivity.getInstance().getVolumeView().refresh();
                    }
                    break;
                case CecManager.Change_Volume_Mute:
                     TurnkeyUiMainActivity.getInstance().switchAmpMute();
                    break;
                case LOAD_DEVICE:
                	if(cecMainDialog!=null){
                		CecListviewAdapter cecListviewAdapter = cecMainDialog.getCecListviewAdapter();
                		deviceGroup.clear();
                		for(CecListviewAdapter.CecDataItem item:tempDeviceGroup){
                			deviceGroup.add(item);
                		}
                		tempDeviceGroup.clear();
                		cecListviewAdapter.setDataGroup(deviceGroup);
                		cecListviewAdapter.notifyDataSetChanged();                	 
                		cecMainDialog.getDeviceListView().setSelection(deviceGroup.size()-1);
                	}
                	break;
                default:
                    break;
            }
            super.handleMessage(msg);
        }
    }

    public void onKeyDown(int keyCode){
        switch (keyCode) {
            case KeyMap.KEYCODE_MTKIR_TITLEPBC:
               mHandler.sendEmptyMessage(1212);
                break;
            case KeyEvent.KEYCODE_BACK:
                cecMainDialog.dismiss();
                break;
            default:
                break;
        }
    }
    public void removeMessage(){
    	cecMainDialog.getHandler().removeMessages(CecMainDialog.MSG_DISMISS);
    	cecMainDialog.getHandler().sendEmptyMessageDelayed(CecMainDialog.MSG_DISMISS, CecMainDialog.MSG_DISMISS_DELAY);
    }
    public boolean sendKeyEvent(int keycode,int keyevent){
        int value = mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN);

        Log.i(TAG, "sendKeyEvent " + keycode + "," + cecService + "," + value);

        try {
            if (cecService != null && value == 1) {
            	
            	if(PippopUiLogic.getInstance(mContext).isPipOrPopState() && keycode == KeyMap.KEYCODE_MENU ){
                	return false;
                }
                cecService.sendkeyEvent(keycode, keyevent);

                if(keycode == KeyMap.KEYCODE_MENU ||keycode == KeyMap.KEYCODE_MTKIR_PLAYPAUSE ){                   
                	if(deviceList != null &&
                			mInputManager.getDefaultOutput().hasSignal()) {
	                    //BD
	                    for (int i = 0; i < deviceList.size(); i++) {
	                        Log.i(TAG, "sendKeyEvent: " + deviceList.get(i).toString());
	
	                        if(DeviceType_dvdplayer != getDeviceType(deviceList.get(i).getLogicalAddr())){
	                            continue;//must be DVD
	                        }
	
	                        //find the HDMI port BD connected
	                        String hdmiPort = deviceList.get(i).getHdmiPort();
	                        int port = 0;
	
	                        while(port < 4){
	                            if(hdmiPort.indexOf(String.valueOf(port)) >= 0){
	                                break;
	                            }
	                            port ++;
	                        }
	
	                        String source = mInputManager.getCurrInputSource();
	                        Log.i(TAG, "source:" + source + ",port:" + port);
	
	                        if(source.toLowerCase().contains("hdmi") && source.indexOf(String.valueOf(port)) >= 0){
	                            return true;
	                        }
	                    }//for
                	}//if
                }//if
                
               
            }
        }
        catch (Exception e) {
        }

        return false;
    }

    private CecMainDialog cecMainDialog = null;
    public void showCecDialog(){
        CecUtil.log("cecMainDialog1:"+cecMainDialog);
        if (cecMainDialog==null) {
            cecMainDialog = new CecMainDialog(mContext);
        }
        CecUtil.log("cecMainDialog:"+cecMainDialog);
        loadSetUp();
        loadDeviceDataGroup();
        if (deviceGroup!=null) {
            cecMainDialog.setAdapter(deviceGroup);
        }
        cecMainDialog.show();
        cecMainDialog.setPositon(-400, 200);
        cecMainDialog.getHandler().sendEmptyMessageDelayed(CecMainDialog.MSG_DISMISS,CecMainDialog.MSG_DISMISS_DELAY);
    }

    public void loadDeviceDataGroup(boolean b){
        new Thread(new Runnable() {

            @Override
            public void run() {
                // TODO Auto-generated method stub
                try {
                    if(null!=deviceGroup){
                        deviceGroup.clear();
                    }else {
                        deviceGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
                    }
                    CecUtil.log("deviceList1:"+deviceList);
                    deviceList = cecService.getCecDeviceList(CecComponentService.CEC_SORT_TYPE_DEFAULT, true);
                    CecUtil.log("deviceList:"+deviceList.size());
                    for (int i = 0; i < deviceList.size(); i++) {
                        String deviceName = deviceList.get(i).getOsdName();
                        CecUtil.log("deviceName:"+deviceName);
                        CecUtil.log("deviceinfo:"+deviceList.get(i).toString());
                        CecListviewAdapter.CecDataItem dataItem = new CecListviewAdapter.CecDataItem
                                (deviceName,deviceName, 0,null,DataType.CecMainItemView);
                        dataItem.setDeviceType(deviceList.get(i).getLogicalAddr());
                        if (deviceName!=null) {
                            deviceGroup.add(dataItem);
                        }
                    }
                    CecListviewAdapter.CecDataItem setupItem = new CecListviewAdapter.CecDataItem
                            (CecManager.CEC_Setup,mContext.getResources().getString(R.string.cec_setup), 0,null,DataType.CecMainItemView);
                    deviceGroup.add(setupItem);
                } catch (Exception e) {
                    // TODO: handle exception
                }
            }
        }).start();


    }
    public void loadDeviceDataGroup(){
        /*try {
            if(null!=deviceGroup){
                deviceGroup.clear();
            }else {
                deviceGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
            }
            CecUtil.log("deviceList1:"+deviceList);
            deviceList = cecService.getCecDeviceList(CecComponentService.CEC_SORT_TYPE_DEFAULT, true);
            CecUtil.log("deviceList:"+deviceList.size());
            for (int i = 0; i < deviceList.size(); i++) {
				//getdeviceName is wrong
                String deviceName = deviceList.get(i).getOsdName();
                CecUtil.log("deviceName:"+deviceName);
                CecUtil.log("deviceinfo:"+deviceList.get(i).toString());
                CecListviewAdapter.CecDataItem dataItem = new CecListviewAdapter.CecDataItem
                        (deviceName,deviceName, 0,null,DataType.CecMainItemView);
                dataItem.setDeviceType(deviceList.get(i).getLogicalAddr());
                if (deviceName!=null) {
                    deviceGroup.add(dataItem);
                }
            }
            
        } catch (Exception e) {
            // TODO: handle exception
        }*/
    	new loadDeviceThread().execute("");
        
    }
    
    
    
    public void loadSetUp(){
    	  if(null!=deviceGroup){
              deviceGroup.clear();
          }else {
              deviceGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
          }
    	CecListviewAdapter.CecDataItem setupItem = new CecListviewAdapter.CecDataItem
                (CecManager.CEC_Setup,mContext.getResources().getString(R.string.cec_setup), 0,null,DataType.CecMainItemView);    
        deviceGroup.add(setupItem);
    	MtkLog.e("chengcl", "deviceGroup=222="+deviceGroup.size());
    }

    public List<CecListviewAdapter.CecDataItem> loadSetupDataGroup(){
        if(null!=setupGroup){
            setupGroup.clear();
        }else {
            setupGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
        }

        CecDataItem cecFunction = new  CecDataItem(MenuConfigManager.CEC_CEC_FUN,mContext.getString(R.string.menu_setup_cec_function),
                 mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN), cec_onoff, CecDataItem.DataType.OPTIONVIEW);
        CecDataItem cecAutoTO = new CecDataItem(MenuConfigManager.CEC_AUTO_ON,mContext.getString(R.string.menu_setup_cec_auto_turn),
               mConfigManager.getDefault(MenuConfigManager.CEC_AUTO_ON), cec_onoff,CecDataItem.DataType.OPTIONVIEW);
        CecDataItem cecDevice = new CecDataItem(MenuConfigManager.CEC_DEVICE_DISCOVERY, mContext
                .getString(R.string.menu_setup_cec_device_discovery),0, null, CecDataItem.DataType.CecMainItemView);
        setupGroup.add(cecFunction);
        setupGroup.add(cecAutoTO);
        setupGroup.add(cecDevice);
        return setupGroup;
    }

    public void loadAttributeDataGroup(int deviceType){
        if(null!=attributeGroup){
            attributeGroup.clear();
        }else {
            attributeGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
        }

//        CecDataItem standbyItem = new  CecDataItem(CEC_Standby_ID,mContext.getString(R.string.menu_setup_cec_standby),
//                mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN), cec_onoff, CecDataItem.DataType.OPTIONVIEW);
//        CecDataItem powerItem = new  CecDataItem(CEC_Poweron_ID,mContext.getString(R.string.menu_setup_cec_poweron),
//                mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN), cec_onoff, CecDataItem.DataType.OPTIONVIEW);
        CecListviewAdapter.CecDataItem standbyItem = new CecListviewAdapter.CecDataItem
                (CEC_Standby_ID,mContext.getString(com.mediatek.ui.R.string.menu_setup_cec_standby), 0,null,DataType.CecMainItemView);
        CecListviewAdapter.CecDataItem powerItem = new CecListviewAdapter.CecDataItem
                (CEC_Poweron_ID,mContext.getString(com.mediatek.ui.R.string.menu_setup_cec_poweron), 0,null,DataType.CecMainItemView);
        CecListviewAdapter.CecDataItem ableItem = new CecListviewAdapter.CecDataItem
                (CEC_Able_ID,mContext.getString(com.mediatek.ui.R.string.menu_setup_cec_enablesys),
                        mConfigManager.getDefault(MenuConfigManager.CEC_SAC_OFUN),null,DataType.CecMainItemView);

        if (deviceType==DeviceType_dvdplayer) {
            attributeGroup.add(standbyItem);
            attributeGroup.add(powerItem);
        }else if(deviceType==DeviceType_amplifier){
            attributeGroup.add(standbyItem);
            attributeGroup.add(powerItem);
            attributeGroup.add(ableItem);
        }else if(deviceType==DeviceType_MHL){
            attributeGroup.add(powerItem);
        }
    }

    public int getDeviceType(int value){
        switch (value) {
            case CecComponentService.CEC_LOG_ADDR_TV:

                return 0;
            case CecComponentService.CEC_LOG_ADDR_PLAYBACK_DEV_2:
            case CecComponentService.CEC_LOG_ADDR_PLAYBACK_DEV_3:
            case CecComponentService.CEC_LOG_ADDR_PLAYBACK_DEV_1:

                return 1;
            case CecComponentService.CEC_LOG_ADDR_AUD_SYS:

                return 2;
            case CecComponentService.CEC_LOG_ADDR_RESERVED_2:

                return 3;
            default:
                break;
        }
        return 1;
    }


    public void setSACon(){
        mConfigManager.setValue(MenuConfigManager.CEC_SAC_OFUN, 1);
    }
    public void setSACoff(){
        mConfigManager.setValue(MenuConfigManager.CEC_SAC_OFUN, 0);
    }
    public void setStandby() {
        Amp_Connect = false;
        try {
            if (cecService != null&&deviceList!=null) {
                cecService.setStandbyByLogicAddr(deviceList.get(selectedDevice).getLogicalAddr());
            }
        } catch (TVMException e) {
            e.printStackTrace();
        }
    }

    public void setStandbyToall() {
        Amp_Connect = false;
        try {
            if (cecService != null&&deviceList!=null) {
                cecService.setStandbyToAll(true);
            }
        } catch (TVMException e) {
            e.printStackTrace();
        }
    }

    public void setPowerOn(){
        try {
            if (cecService != null &&deviceList!=null) {
                cecService.powerOnDeviceByLogicAddr(deviceList.get(selectedDevice).getLogicalAddr());
            }
        } catch (Exception e) {
        }
    }

    public void discoveryDevice() {
        if (mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN)==0) {
            return;
        }
        mHandler.sendEmptyMessage(CecManager.Show_WaitDialog);
        new Thread(new Runnable() {

            @Override
            public void run() {
                // TODO Auto-generated method stub
                try {
                    if (cecService != null) {
                        CecDevDiscoveryInfo cInfo = new CecDevDiscoveryInfo();
                        cInfo.setStartLa(CecComponentService.CEC_LOG_ADDR_REC_DEV_1);
                        cInfo.setStopLa(CecComponentService.CEC_LOG_ADDR_FREE_USE);
                        cInfo.setItvl(0);
                        cInfo.setBatchPolling(false);
                        cecService.discoveryDevice(cInfo);
                    }
                } catch (TVMException e) {
                    e.printStackTrace();
                }
            }
        }).start();


    }


    public boolean isAmpVolume(){
        Log.e("isAmpVolume", "isAmpVolume:"+Amp_Connect);
        return Amp_Connect;
    }

    public String getVolumeAmp(){
        return volume_amp;
    }

    public boolean getVolumeAmpMute(){
        return volume_amp_mute;
    }

    public void changeVolume(int value){
        Log.e("CecManager", "changeVolume");
        try {
            switch (value) {
                case CecManager.Volume_UP:
                    cecService.ampVolumeCtrl(CecComponentService.CEC_AMP_CONTROL_VOLUME_UP);
                    Log.e("CecManager", "changeVolumeup");
                    break;
                case CecManager.Volume_Down:
                    cecService.ampVolumeCtrl(CecComponentService.CEC_AMP_CONTROL_VOLUME_DOWN);
                    break;
                case CecManager.Volume_Mute:
                    cecService.ampVolumeCtrl(CecComponentService.CEC_AMP_CONTROL_VOLUME_MUTE);
                    break;
                default:
                    break;
            }

        } catch (Exception e) {
            // TODO: handle exception
        }

    }

    public void showWaitDialog(){
        mDialog = new EPGProgressDialog(mContext,R.style.dialog);
        CecUtil.setPositon(-50, 0, mDialog);
        new Thread(){
            public void run() {
                if (showDialog) {
                    try {
                        MtkLog.e(TAG, "Thread");
                        sleep(1000);
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

            };
        }.start();
        mDialog.setCancelable(false);
        mDialog.show();
    }
    public void dissmissWaitDialog(){
        if (mDialog!=null) {
            mDialog.dismiss();
        }
    }

    public CecHandler getCecHandler(){
        return mHandler;
    }

    public boolean isCecWorking(){
        return isCECWorking;
    }

    public void setCecWorking(boolean work){
        isCECWorking = work;
    }

    public List<CecListviewAdapter.CecDataItem> getAttributeDataGroup(){
        return attributeGroup;
    }

    public List<CecListviewAdapter.CecDataItem> getSetupDataGroup(){
        if (setupGroup==null) {
            loadSetupDataGroup();
        }
        return setupGroup;
    }
    public void setSelectedDevice(int value){
        selectedDevice = value;
    }

    public void changeSource (String value){
    	if(GetCurrentTask.getInstance(mContext).isCurActivityTkuiMainActivity()){
       List<String> mSourceList = Arrays.asList(mInputManager.getInputSourceArray());
       String sourceValue = value.toLowerCase();
//        mInputManager.changeInputSource(value);
       for (int i = 0; i < mSourceList.size(); i++) {
           Log.e("mSourceList", "mSourceList:"+mSourceList.get(i));
       }
       Log.e("mSourceList", "change Source:"+sourceValue);

       mInputManager.changeInputSource(mTVCMManager.getCurrentOutput(),sourceValue);
    	}
    }

    public void exit(){
        try {
            if (mConfigManager.getDefault(MenuConfigManager.CEC_CEC_FUN)==1
                    &&mConfigManager.getDefault(MenuConfigManager.CEC_AUTO_ON)==1) {
                Log.e("Cecmanager", "exit");
                cecService.setStandbyToAll(false);
            }
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
    class loadDeviceThread extends AsyncTask<String,String,String>{

		@Override
		protected String doInBackground(String... arg0) {
			try {
	           if(null!=tempDeviceGroup){
	        	   tempDeviceGroup.clear();
	            }else {
	            	tempDeviceGroup = new ArrayList<CecListviewAdapter.CecDataItem>();
	            }
	            //CecUtil.log("deviceList1:"+deviceList);
	            deviceList = cecService.getCecDeviceList(CecComponentService.CEC_SORT_TYPE_DEFAULT, true);
	            CecUtil.log("deviceList:"+deviceList.size());
	            for (int i = 0; i < deviceList.size(); i++) {
					//getdeviceName is wrong
	                String deviceName = deviceList.get(i).getOsdName();
	                CecUtil.log("deviceName:"+deviceName);
	                CecUtil.log("deviceinfo:"+deviceList.get(i).toString());
	                CecListviewAdapter.CecDataItem dataItem = new CecListviewAdapter.CecDataItem
	                        (deviceName,deviceName, 0,null,DataType.CecMainItemView);
	                dataItem.setDeviceType(deviceList.get(i).getLogicalAddr());
	                if (deviceName!=null) {	                	
	                	tempDeviceGroup.add(dataItem);
	                }
	            }
	            CecListviewAdapter.CecDataItem setupItem = new CecListviewAdapter.CecDataItem
		                (CecManager.CEC_Setup,mContext.getResources().getString(R.string.cec_setup), 0,null,DataType.CecMainItemView);    
		    	tempDeviceGroup.add(setupItem);
		    	MtkLog.d(TAG, "deviceGroup=222="+tempDeviceGroup.size());
	            
	        } catch (Exception e) {
	        }	        
	    
			return null;
		}

		@Override
		protected void onPostExecute(String result) {
			if(tempDeviceGroup!=null && tempDeviceGroup.size()>1){
				mHandler.sendEmptyMessage(LOAD_DEVICE);
			}
			super.onPostExecute(result);
		}

		@Override
		protected void onPreExecute() {
			// TODO Auto-generated method stub
			super.onPreExecute();
		}
    	
    }
    
    
    public void updateSystemPanelOn(){
    	try{
            cecService.updateSysStatus(CecComponentService.CEC_COMP_FAST_BOOT_POWER_ON);
            MtkLog.d(TAG, "fastboot ..... on");
        }
        catch(Exception ex){
        	 MtkLog.e(TAG, "fastboot ..... error");
            ex.printStackTrace();
        }
    }
}
