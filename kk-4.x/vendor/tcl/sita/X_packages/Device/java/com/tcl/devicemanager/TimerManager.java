/*
 *   Copyright (C) 2006 The Android Open Source Project
 *    
 *   ©2010-2013 TCL CORPORATION All Rights Reserved.
 */


package com.tcl.devicemanager;

import android.content.Intent;
import android.app.IAlarmManager;
import android.os.ServiceManager;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.content.Context;

public class TimerManager{

	static TimerManager mInstance=null;
    private final IAlarmManager mService;

    /**
     * package private on purpose
     */
    TimerManager(IAlarmManager service) {
        mService = service;
    }
	 
	public static TimerManager getInstance() {
	/* Double-checked locking */
		if (mInstance == null) {
			synchronized (TimerManager.class) {
				if (mInstance == null) {
				//           IBinder b = ServiceManager.getService(Context.TV_SERVICE);
				//          mInstance = new TvManager(ITvService.Stub.asInterface(b));
				        IBinder b = ServiceManager.getService(Context.ALARM_SERVICE);
				        IAlarmManager service = IAlarmManager.Stub.asInterface(b);
				   mInstance = new TimerManager(service);
				}
			}
		}
		return mInstance;
	}  

	//Added by zouhaichao: About standby interface in 2013.4.10
	public void setPoweroffTimer (long rel) {
		try {
		    mService.setPoweroffTimer (rel);
		} catch (RemoteException ex) {
		}

	}

	public void cancelPoweroffTimer () {
		try {
		    mService.cancelPoweroffTimer ();
		} catch (RemoteException ex) {
		}

	}

	public long getReminderRelativeTime () {
		try {
			return mService.getReminderRelativeTime ();
		} catch (RemoteException ex) {
		}
	              return 0;
	}
	//End by zouhaichao
        /* Patch Begin by Sarah Zhang(zxia@tcl.com) */
        public void setScreenoffTimer(long rel) {
                try {
                    mService.setScreenoffTimer(rel);
                } catch (RemoteException ex) {
                }

        }

        public void startScreenoffTimer() {
                try {
                    mService.startScreenoffTimer();
                } catch (RemoteException ex) {
                }

        }
        public void stopScreenoffTimer() {
                try {
                    mService.stopScreenoffTimer();
                } catch (RemoteException ex) {
                }

        }

        public long getReminderRelativeScreenTime() {
                try {
                        return mService.getReminderRelativeScreenTime();
                } catch (RemoteException ex) {
                }
                      return 0;
        }

        public void EventResetScreenoffTimer() {
                try {
                    mService.EventResetScreenoffTimer();
                } catch (RemoteException ex) {
                }

        }

        public void ReportIfStartScreenSaver(boolean flag) {
                try {
                    mService.ReportIfStartScreenSaver(flag);
                } catch (RemoteException ex) {
                }

        }

        public long getScreenOffTimer() {
                try {
                        return mService.getScreenOffTimer();
                } catch (RemoteException ex) {
                }
                      return 0;
        }
        /* Patch End by Sarah Zhang(zxia@tcl.com) */
	/////////////////////////////////////////////////////////////////////////////////
	private final long time_unit = 60 * 1000;
	/**
     * Set sleep mode: sleep after certain time.
     *
     * @param eMode
     * @return true: success. false: fail
     */
    public boolean setSleepMode(EnumSleepTimeState eMode) {
        long offms = 0;
		
		switch(eMode) {
		case E_10MIN:
			offms = 10;
			break;
		case E_20MIN:
			offms = 20;
			break;
		case E_30MIN:
			offms = 30;
			break;
		case E_60MIN:
			offms = 60;
			break;
		case E_90MIN:
			offms = 90;
			break;
		case E_120MIN:
			offms = 120;
			break;
		case E_180MIN:
			offms = 180;
			break;
		case E_240MIN:
			offms = 240;
			break;
		case E_OFF:
		default:
				offms = 0;
				break;
		}
		offms = offms * time_unit;
		if(offms == 0){
			cancelPoweroffTimer();
		}else{
			setPoweroffTimer(offms);
		}
        return true;
    }

    /**
     * Get sleep time mode.
     *
    * @return EnumSleepTimeState mode enum
     */
    public EnumSleepTimeState getSleepMode() {
		long offms = 0 ;
		EnumSleepTimeState state = EnumSleepTimeState.E_OFF;
		try {
			offms = mService.getPowerOffTimer();
		} catch (RemoteException ex) {
			offms = 0;
		}
		offms = offms / time_unit;
		if (offms == 0) {
			state = EnumSleepTimeState.E_OFF;
		} else if (0 < offms && offms <= 10) {
			state = EnumSleepTimeState.E_10MIN;
		} else if (10 < offms && offms <= 20) {
			state = EnumSleepTimeState.E_20MIN;
		} else if (20 < offms && offms <= 30) {
			state = EnumSleepTimeState.E_30MIN;
		} else if (30 < offms && offms <= 60) {
			state = EnumSleepTimeState.E_60MIN;
		} else if (60 < offms && offms <= 90) {
			state = EnumSleepTimeState.E_90MIN;
		} else if (90 < offms && offms <= 120) {
			state = EnumSleepTimeState.E_120MIN;
		} else if (120 < offms && offms <= 180) {
			state = EnumSleepTimeState.E_180MIN;
		} else if (180 < offms && offms <= 240) {
			state = EnumSleepTimeState.E_240MIN;
		} else {
			state = EnumSleepTimeState.E_OFF;
		}
		return state;
    }
	/**
     * Enalbe auto power on feature.
     * This function will load time and event params set by setOnTimer and setOnTimeEvent
     * to enable auto power on feature.
     *
     * @param bEnable true: enable, false: disable
     * @return true: success, or false: fail
     */
    public boolean setOnTimerEnable(boolean bEnable) {
		/*
        Log.d(TAG, "setAtvChannel(), paras bEnable = " + bEnable);
        ITvTimer service = getService();
        try {
            return service.setOnTimerEnable(bEnable);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
		*/
        return false;
    }
}
