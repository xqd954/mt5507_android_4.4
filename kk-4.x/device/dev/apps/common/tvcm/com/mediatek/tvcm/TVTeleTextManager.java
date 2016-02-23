package com.mediatek.tvcm;

import android.content.Context;
import android.util.Log;

import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.CompListener;
import com.mediatek.tv.service.ComponentService;

public class TVTeleTextManager extends TVComponent {
	private static final String TAG="TVTeleTextManager";
	protected ComponentService cpsService;
	
	private boolean isShowTTXIcon = false;
	
	static private TVTeleTextManager instance;
	
	public TVTeleTextManager(Context context) {
		super(context);
		try {
			Log.e(TAG, "getService"+ComponentService.CompServiceName);
			if(null != tvMgr){
				cpsService=(ComponentService)tvMgr.getService(ComponentService.CompServiceName);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	static public TVTeleTextManager getInstance(Context context){
		if (instance == null) {
			instance = new TVTeleTextManager(context);
		}
		
		return instance;
	}
	
	public void init(){}
	
	
	public void activateComponent(String compName){
		if(cpsService!=null){
			try {
				Log.e(TAG, "ActivateComponent"+ComponentService.CompServiceName);
				cpsService.activateComponent(compName);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
	}
	
	
	public void inActivateCompoent(String compName){
		if(cpsService!=null){
			try {
				Log.e(TAG, "InActivateCompoent:"+ComponentService.CompServiceName);
				cpsService.inActivateCompoent(compName);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	
	public void updateSysStatus(String statusName){
		
		if(cpsService!=null){
			try {
				Log.e(TAG, "updateSysStatus:"+statusName);
				cpsService.updateSysStatus(statusName);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	
	public boolean isTTXAvail(){
		if(cpsService!=null){
			try {
				Log.e(TAG, "IsTTXAvail:"+cpsService.isTTXAvail());
				return cpsService.isTTXAvail();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return false;
	}
	
	
	public void sendkeyEvent(int i4_keycode,int fgKeydown){
		if(cpsService!=null){
			try {
				Log.e(TAG, "sendkeyEvent:"+i4_keycode+"up/down:"+fgKeydown);
				cpsService.sendkeyEvent(i4_keycode, fgKeydown);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	
	
	
	public void  registerNotifyLisenter(CompListener notifyLisenter){
		if(cpsService!=null){
			Log.e(TAG, "add complisenter");
			cpsService.addListener(notifyLisenter);
		}
		
	}
	
	public void  removeNotifyLisenter(CompListener notifyLisenter){
		if(cpsService!=null){
			Log.e(TAG, "remove complisenter");
			cpsService.removeListener(notifyLisenter);
		}
	
	}
		
	public void setTTXIconShow(boolean iconShow){
		Log.e(TAG, "setTTXIconShow(boolean iconShow) == " + iconShow);
		isShowTTXIcon = iconShow;
	}

	public boolean getTTXIconShow(){
		Log.e(TAG, "getTTXIconShow == " + isShowTTXIcon);
		return isShowTTXIcon;
	}
}
