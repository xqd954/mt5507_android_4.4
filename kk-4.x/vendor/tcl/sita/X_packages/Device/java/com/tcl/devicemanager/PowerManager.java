/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */
package com.tcl.devicemanager;
/* lvh@tcl */
import android.content.Intent;
import android.os.ServiceManager;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.content.Context;

import android.os.SystemClock;
import android.os.IPowerManager;
import android.hardware.input.InputManager;
import android.view.KeyEvent;
import android.content.Intent;
import android.content.ActivityNotFoundException;
/*
	Set auto poweroff timer , power off tv
 */
public class PowerManager{

    private Context context;
	private TimerManager timermanager;

	private boolean _usePM = false;
	public static final int POWER_OFF = 0; //// 关机（不同机型定义不同，可能与POWER_SUSPEND相同）
	public static final int POWER_ON = 1;  ////开机，全速运行，取消省电模式
	public static final int POWER_SUSPEND = 2; //// 待机
	public static final int POWER_SAVE = 3;  //// 省电，用户仍可操作

    public PowerManager(Context _context) {
        context = _context;
		timermanager = TimerManager.getInstance();
    }
    /**     
     * set power off after time.
     * @author TCL TVOS Team
     * @param rel  , time to power off , unit : ms
     */
	public void setPoweroffTimer (long rel) {
		if(timermanager != null){
			timermanager.setPoweroffTimer(rel);
		}
	}
    /**     
     * cancel delay power off
     * @author TCL TVOS Team
     */
	public void cancelPoweroffTimer () {
		if(timermanager != null){
			timermanager.cancelPoweroffTimer();
		}

	}
    /**     
     * get current time to power off
     * @author TCL TVOS Team
     * @return time left , unit : ms
     */
	public long getReminderRelativeTime () {
		if(timermanager != null){
			return timermanager.getReminderRelativeTime();
		}
		return 0;
	}

    private boolean setSleepMode(EnumSleepTimeState eMode) {
       if(timermanager != null){
			return timermanager.setSleepMode(eMode);
		}
        return true;
    }

    /**
     * Get sleep time mode.
     *
    * @return EnumSleepTimeState mode enum
     */
    private EnumSleepTimeState getSleepMode() {
		if(timermanager != null){
			return timermanager.getSleepMode();
		}
		return EnumSleepTimeState.E_OFF;

    }
	/**
     * Enable auto power on feature.
     * This function will load time and event params set by setOnTimer and setOnTimeEvent
     * to enable auto power on feature.
     *
    * @param bEnable true: enable, false: disable
     * @return true: success, or false: fail
     */
    private boolean setOnTimerEnable(boolean bEnable) {
		if(timermanager != null){
			return timermanager.setOnTimerEnable(bEnable);
		}
        return false;
    }
	/**
     * set current power feature.
     * @param _status POWER_OFF : power off TV , same as powerOff()
     *        POWER_ON : return to full run
     *        POWER_SUSPEND: close all application , deep save power 
     *        POWER_SAVE: save power but not close application
     * @return true: success, or false: fail
     */
	public boolean setPowerStatus(int _status){
		switch(_status){
			case POWER_OFF:
				return powerOff();
			case POWER_ON:
			case POWER_SUSPEND:
			case POWER_SAVE:
			default:
				break;
		}
		return false;
	}
	/**
     * Power off TV .
     * @return true: success, or false: fail
     */
	public boolean powerOff(){
		/*
		if(_usePM == true) {
			Thread thr = new Thread("PowerManager") {
		        @Override
		        public void run() {
		            IPowerManager pm = IPowerManager.Stub.asInterface(
		                    ServiceManager.getService(Context.POWER_SERVICE));
		            try {
		                 pm.shutdown(false, false);
		            } catch (RemoteException e) {
		            }
		        }
		    };
		    thr.start();
		    // Wait for us to tell the power manager to shutdown.
		    try {
		        thr.join();
		    } catch (InterruptedException e) {
		    }
		}
		else{
			int eventCode = KeyEvent.KEYCODE_POWER;
			long now = SystemClock.uptimeMillis();
			KeyEvent down = new KeyEvent(now, now, KeyEvent.ACTION_DOWN, eventCode, 0);
			InputManager.getInstance().injectInputEvent(down,
						            InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);

			now = SystemClock.uptimeMillis();
			KeyEvent up = new KeyEvent(now, now, KeyEvent.ACTION_UP, eventCode, 0);
			InputManager.getInstance().injectInputEvent(up,
						            InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
			return true;
		 }
		 */
		 // use timermanager
		if(timermanager == null){
			return false;
		}
		timermanager.setPoweroffTimer(100);
		/*
			// "android.permission.SHUTDOWN"
		if(context == null){
			return false;
		}
		try{
			Intent i = new Intent("android.intent.action.ACTION_REQUEST_SHUTDOWN");		
			i.putExtra("android.intent.extra.KEY_CONFIRM", false);		
			i.putExtra(Intent.EXTRA_ALARM_COUNT,true);
			i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(i);
		}catch(ActivityNotFoundException e){
			e.printStackTrace();
			return false;
		}
		*/
		return true;
	}
}
