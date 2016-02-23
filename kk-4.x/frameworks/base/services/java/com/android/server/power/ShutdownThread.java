/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 
package com.android.server.power;

import android.app.ActivityManagerNative;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.IActivityManager;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.IBluetoothManager;
import android.nfc.NfcAdapter;
import android.nfc.INfcAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.Vibrator;
import android.os.SystemVibrator;
import android.os.storage.IMountService;
import android.os.storage.IMountShutdownObserver;

import com.android.internal.telephony.ITelephony;

import android.util.Log;
import android.view.WindowManager;

// Begin added by TCL zhanghangzhi, mail: zhanghangzhi@tcl.com
import com.tcl.tvmanager.TTvFunctionManager;
// End of TCL


// 20130822, Henry.Su, reopened for shutdown dialog - START
import android.view.View;
import android.view.WindowManager;
import android.view.Gravity;
import android.view.View.OnClickListener;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Button;
import android.view.LayoutInflater;
import android.util.TypedValue;
// 20130822, Henry.Su, reopened for shutdown dialog - END
// 20130822, Henry.Su, reopened for shutdown dialog - START
import android.widget.TCLAlertDialogEspecial;

import com.tcl.tvmanager.TvManager;
//import com.tcl.tvmanager.exception.TvCommonException;
// 20130822, Henry.Su, reopened for shutdown dialog - END

import android.content.Intent;
import android.content.DialogInterface.OnKeyListener;
import android.view.KeyEvent;


public final class ShutdownThread extends Thread {
    // constants
    private static final String TAG = "ShutdownThread";
    private static final int PHONE_STATE_POLL_SLEEP_MSEC = 500;
    // maximum time we wait for the shutdown broadcast before going on.
    private static final int MAX_BROADCAST_TIME = 10*1000;
    private static final int MAX_SHUTDOWN_WAIT_TIME = 20*1000;
    private static final int MAX_RADIO_WAIT_TIME = 12*1000;

    // length of vibration before shutting down
    private static final int SHUTDOWN_VIBRATE_MS = 500;
    
    // state tracking
    private static Object sIsStartedGuard = new Object();
    private static boolean sIsStarted = false;
    
    private static boolean mReboot;
    private static boolean mRebootSafeMode;
    private static String mRebootReason;

    // Provides shutdown assurance in case the system_server is killed
    public static final String SHUTDOWN_ACTION_PROPERTY = "sys.shutdown.requested";

    // Indicates whether we are rebooting into safe mode
    public static final String REBOOT_SAFEMODE_PROPERTY = "persist.sys.safemode";

    // static instance of this thread
    private static final ShutdownThread sInstance = new ShutdownThread();
    
    private final Object mActionDoneSync = new Object();
    private boolean mActionDone;
    private Context mContext;
    private PowerManager mPowerManager;
    private PowerManager.WakeLock mCpuWakeLock;
    private PowerManager.WakeLock mScreenWakeLock;
    private Handler mHandler;

    //private static AlertDialog sConfirmDialog;
    
     // 20130822, Henry.Su, reopened for shutdown dialog - START
    //private static AlertDialog sConfirmDialog;
	private static TCLAlertDialogEspecial sConfirmDialog;
    // 20130822, Henry.Su, reopened for shutdown dialog - END
    
    
    
    private ShutdownThread() {
    }
 
    /**
     * Request a clean shutdown, waiting for subsystems to clean up their
     * state etc.  Must be called from a Looper thread in which its UI
     * is shown.
     *
     * @param context Context used to display the shutdown progress dialog.
     * @param confirm true if user confirmation is needed before shutting down.
     */
    public static void shutdown(final Context context, boolean confirm) {
        mReboot = false;
        mRebootSafeMode = false;
        shutdownInner(context, confirm);
    }

    static void shutdownInner(final Context context, boolean confirm) {
        // ensure that only one thread is trying to power down.
        // any additional calls are just returned
        synchronized (sIsStartedGuard) {
            if (sIsStarted) {
                Log.d(TAG, "Request to shutdown already running, returning.");
                return;
            }
        }
        if (confirm) {
        
        // 20130822, Henry.Su, reopened for shutdown dialog - START
        //if (confirm && (sConfirmDialog == null || !sConfirmDialog.isShowing())) {

	if (sConfirmDialog == null || !sConfirmDialog.isShowing()) {
		
		Log.d(TAG, "davild-----shutdown --show shutdown dialog.....");
        
        final int longPressBehavior = context.getResources().getInteger(
                        com.android.internal.R.integer.config_longPressOnPowerBehavior);
        final int resourceId = mRebootSafeMode
                ? com.android.internal.R.string.reboot_safemode_confirm
                : (longPressBehavior == 2
                        ? com.android.internal.R.string.shutdown_confirm_question
                        : com.android.internal.R.string.shutdown_confirm);

        Log.d(TAG, "Notifying thread to start shutdown longPressBehavior=" + longPressBehavior);

        final CloseDialogReceiver closer = new CloseDialogReceiver(context);
/*         
         if (sConfirmDialog != null) {
                sConfirmDialog.dismiss();
            }
            sConfirmDialog = new AlertDialog.Builder(context)
                    .setTitle(mRebootSafeMode
                            ? com.android.internal.R.string.reboot_safemode_title
                            : com.android.internal.R.string.power_off)
                    .setMessage(resourceId)
                    .setPositiveButton(com.android.internal.R.string.yes, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            beginShutdownSequence(context);
                        }
                    })
                    .setNegativeButton(com.android.internal.R.string.no, null)
                    .create();
            */
            
            sConfirmDialog = TCLAlertDialogEspecial.makeFullDialog(context, 0);
            /*
 			LayoutInflater Inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
   			LinearLayout layout = (LinearLayout)Inflater.inflate(com.android.internal.R.layout.tcl_alert_content_text_especial, null);
            //LinearLayout layout = (LinearLayout)Inflater.inflate(tvos.R.layout.tcl_alert_content_text_especial, null);
            
			layout.setGravity(Gravity.CENTER);
			sConfirmDialog.setCustomContentView(layout);		
					
			//TextView textView = (TextView)layout.findViewById(com.android.internal.R.id.label);
            TextView textView = (TextView)layout.findViewById(com.android.internal.R.id.tcl_especial_idlabel);
                        
			textView.setText(resourceId);
			textView.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
			textView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 30);
			*/
            //sConfirmDialog.setContentText(context, context.getResources().getString(resourceId) );
            sConfirmDialog.setContentText(context, context.getResources().getString(com.android.internal.R.string.shutdown_confirm_question) );
			sConfirmDialog.setButton(context, TCLAlertDialogEspecial.THREE_BUTTON);
			
            //added by zengml 2014/02/28
			sConfirmDialog.setOnKeyListener(new OnKeyListener() {  
                @Override  
                public boolean onKey(DialogInterface dialog, int keyCode,  
                        KeyEvent event) {  
                    if (keyCode == KeyEvent.KEYCODE_BACK) {  
                        Log.d(TAG, "davild-----shutdown -----onKey--------");
                        sConfirmDialog.dismiss();
                        return true;
                    }  
                    return false;  
                }  
            });  
        
			Button leftButton = sConfirmDialog.getLeftButton();
			leftButton.setText(com.android.internal.R.string.yes);
            leftButton.setTextSize(TypedValue.COMPLEX_UNIT_SP,24);
			leftButton.setOnClickListener(new OnClickListener(){
				@Override
				public void onClick(View arg0) {
                    Log.d(TAG, "davild-----33333333333333--- confirm shutdown...... ");
                    Log.d(TAG, "davild-----444444444444444--leftButton --tcl_dialog_yes");
	                beginShutdownSequence(context);
				}
			});
			
			Button rightButton = sConfirmDialog.getRightButton();
			rightButton.setText(com.android.internal.R.string.no);
            rightButton.setTextSize(TypedValue.COMPLEX_UNIT_SP,24);
			rightButton.setOnClickListener(new OnClickListener(){

				@Override
				public void onClick(View arg0) {
                
                    Log.d(TAG, "davild-----444444444444444--rightButton --tcl_dialog_no");
					sConfirmDialog.dismiss();
					sConfirmDialog = null ;
					
				}
			});
			
            Button centerButton = sConfirmDialog.getCenterButton();
			//centerButton.setText(com.android.internal.R.string.tcl_dialog_alonelistener);
            centerButton.setText(com.android.internal.R.string.listen_only_mode);
            centerButton.setTextSize(TypedValue.COMPLEX_UNIT_SP,24);
			centerButton.setOnClickListener(new OnClickListener(){

				@Override
				public void onClick(View arg0) {
                    Log.d(TAG, "davild-----555555555555--centerButton --tcl_dialog_alonelistener");
                    // Henry.Su 20130820, turn off backlight before starting shutdown sequence - START
					// Power off backlight immediately
					try{
						//TvManager.getInstance().getPictureManager().disableBacklight();

					 	// Begin added by gaodw@tcl.com
					        TTvFunctionManager tvFuncMgr = TTvFunctionManager.getInstance(context);
					        if (tvFuncMgr != null ) {
							boolean mBacklight=tvFuncMgr.getPowerBacklightSate();
							Log.d(TAG, "Shutdown Current mBacklight is :  " + mBacklight );
						     if(mBacklight){
							      Log.d(TAG, "Call setPowerBacklight false");
					            		tvFuncMgr.setPowerBacklight(false);
						     	}
					        }//end

					}
                    catch (Exception ex){
					}
					// Henry.Su 20130820, turn off backlight before starting shutdown sequence - END	
                    
                    Log.e(TAG, "---david---keyCode==KEYCODE_HOME --->>> set  sys.shutdown.alone  true disable key.except V+-,mute, home. " );	
                    SystemProperties.set("sys.shutdown.alone", "true");
            
					sConfirmDialog.dismiss();
					sConfirmDialog = null ;
				}
			});
            
			leftButton.setFocusable(true);
			leftButton.setFocusableInTouchMode(true);
			leftButton.requestFocus();
			
       		WindowManager.LayoutParams lp = sConfirmDialog.getWindow().getAttributes();
       		lp.width = 700;
       		sConfirmDialog.getWindow().setAttributes(lp);
            //end
            
            closer.dialog = sConfirmDialog;
            sConfirmDialog.setOnDismissListener(closer);
            sConfirmDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);
            sConfirmDialog.show();
        } 
	  else if (sConfirmDialog != null && sConfirmDialog.isShowing()) {

		 Log.d(TAG, "davild-----shutdown -- sConfirmDialog != null && sConfirmDialog.isShowing(). and direct return.");
		 return;
	  	}
        	
	}else {
            // 20130822, Henry.Su, reopened for shutdown dialog - END
            Log.d(TAG, "davild-----shutdown -- not show shutdown dialog.....");
            beginShutdownSequence(context);
        }
    }
    
    //--------------david-----------------------------------------

    private static class CloseDialogReceiver extends BroadcastReceiver
            implements DialogInterface.OnDismissListener {
        private Context mContext;
        public Dialog dialog;

        CloseDialogReceiver(Context context) {
            mContext = context;
            IntentFilter filter = new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
            context.registerReceiver(this, filter);
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            dialog.cancel();
        }

        public void onDismiss(DialogInterface unused) {
            mContext.unregisterReceiver(this);
        }
    }

    /**
     * Request a clean shutdown, waiting for subsystems to clean up their
     * state etc.  Must be called from a Looper thread in which its UI
     * is shown.
     *
     * @param context Context used to display the shutdown progress dialog.
     * @param reason code to pass to the kernel (e.g. "recovery"), or null.
     * @param confirm true if user confirmation is needed before shutting down.
     */
    public static void reboot(final Context context, String reason, boolean confirm) {
        mReboot = true;
        mRebootSafeMode = false;
        mRebootReason = reason;
        shutdownInner(context, confirm);
    }

    /**
     * Request a reboot into safe mode.  Must be called from a Looper thread in which its UI
     * is shown.
     *
     * @param context Context used to display the shutdown progress dialog.
     * @param confirm true if user confirmation is needed before shutting down.
     */
    public static void rebootSafeMode(final Context context, boolean confirm) {
        mReboot = true;
        mRebootSafeMode = true;
        mRebootReason = null;
        shutdownInner(context, confirm);
    }

    private static void beginShutdownSequence(Context context) {
        synchronized (sIsStartedGuard) {
            if (sIsStarted) {
                Log.d(TAG, "Shutdown sequence already running, returning.");
                return;
            }
            sIsStarted = true;
        }

        // throw up an indeterminate system dialog to indicate radio is
        // shutting down.
        /*
        ProgressDialog pd = new ProgressDialog(context);
        pd.setTitle(context.getText(com.android.internal.R.string.power_off));
        pd.setMessage(context.getText(com.android.internal.R.string.shutdown_progress));
        pd.setIndeterminate(true);
        pd.setCancelable(false);
        pd.getWindow().setType(WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);

        pd.show();
        */
        sInstance.mContext = context;
        sInstance.mPowerManager = (PowerManager)context.getSystemService(Context.POWER_SERVICE);

        // make sure we never fall asleep again
        sInstance.mCpuWakeLock = null;
        try {
            sInstance.mCpuWakeLock = sInstance.mPowerManager.newWakeLock(
                    PowerManager.PARTIAL_WAKE_LOCK, TAG + "-cpu");
            sInstance.mCpuWakeLock.setReferenceCounted(false);
            sInstance.mCpuWakeLock.acquire();
        } catch (SecurityException e) {
            Log.w(TAG, "No permission to acquire wake lock", e);
            sInstance.mCpuWakeLock = null;
        }

        // also make sure the screen stays on for better user experience
        sInstance.mScreenWakeLock = null;
        
        // 20130822, Henry.Su, reopened for shutdown dialog - START
		// Do not hold the screen wakelock, otherwise the backlight may be kept turned on
        /*if (sInstance.mPowerManager.isScreenOn()) {
        if (sInstance.mPowerManager.isScreenOn()) {
            try {
                sInstance.mScreenWakeLock = sInstance.mPowerManager.newWakeLock(
                        PowerManager.FULL_WAKE_LOCK, TAG + "-screen");
                sInstance.mScreenWakeLock.setReferenceCounted(false);
                sInstance.mScreenWakeLock.acquire();
            } catch (SecurityException e) {
                Log.w(TAG, "No permission to acquire wake lock", e);
                sInstance.mScreenWakeLock = null;
            }
        }
        */
		// 20130822, Henry.Su, reopened for shutdown dialog - END

        
        // start the thread that initiates shutdown
        sInstance.mHandler = new Handler() {
        };
        sInstance.start();
    }

    void actionDone() {
        synchronized (mActionDoneSync) {
            mActionDone = true;
            mActionDoneSync.notifyAll();
        }
    }

    /**
     * Makes sure we handle the shutdown gracefully.
     * Shuts off power regardless of radio and bluetooth state if the alloted time has passed.
     */
    public void run() {
        BroadcastReceiver br = new BroadcastReceiver() {
            @Override public void onReceive(Context context, Intent intent) {
                // We don't allow apps to cancel this, so ignore the result.
                actionDone();
            }
        };

        /*
         * Write a system property in case the system_server reboots before we
         * get to the actual hardware restart. If that happens, we'll retry at
         * the beginning of the SystemServer startup.
         */
        {
            String reason = (mReboot ? "1" : "0") + (mRebootReason != null ? mRebootReason : "");
            SystemProperties.set(SHUTDOWN_ACTION_PROPERTY, reason);
        }

        /*
         * If we are rebooting into safe mode, write a system property
         * indicating so.
         */
        if (mRebootSafeMode) {
            SystemProperties.set(REBOOT_SAFEMODE_PROPERTY, "1");
        }

        // Begin added by TCL zhanghangzhi, mail: zhanghangzhi@tcl.com
        TTvFunctionManager tvFuncMgr = TTvFunctionManager.getInstance(mContext);
        if (tvFuncMgr != null && mReboot) {
            Log.d(TAG, "Call readyForReboot");
            tvFuncMgr.readyForReboot();
        } else if (tvFuncMgr != null && !mReboot) {
            Log.d(TAG, "Call sitaTerminate");
            tvFuncMgr.sitaTerminate();
        }
        // End of TCL

        Log.i(TAG, "Sending shutdown broadcast...");
        
        // First send the high-level shut down broadcast.
        mActionDone = false;
        Intent intent = new Intent(Intent.ACTION_SHUTDOWN);
        intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        mContext.sendOrderedBroadcastAsUser(intent,
                UserHandle.ALL, null, br, mHandler, 0, null, null);     
        final long endTime = SystemClock.elapsedRealtime() + MAX_BROADCAST_TIME;
        synchronized (mActionDoneSync) {
            while (!mActionDone) {
                long delay = endTime - SystemClock.elapsedRealtime();
                if (delay <= 0) {
                    Log.w(TAG, "Shutdown broadcast timed out");
                    break;
                }
                try {
                    mActionDoneSync.wait(delay);
                } catch (InterruptedException e) {
                }
            }
        }
        
        Log.i(TAG, "Shutting down activity manager...");
        
        final IActivityManager am =
            ActivityManagerNative.asInterface(ServiceManager.checkService("activity"));
        if (am != null) {
            try {
                am.shutdown(MAX_BROADCAST_TIME);
            } catch (RemoteException e) {
            }
        }

        // Shutdown radios.
        //shutdownRadios(MAX_RADIO_WAIT_TIME);

        // Shutdown MountService to ensure media is in a safe state
        IMountShutdownObserver observer = new IMountShutdownObserver.Stub() {
            public void onShutDownComplete(int statusCode) throws RemoteException {
                Log.w(TAG, "Result code " + statusCode + " from MountService.shutdown");
                actionDone();
            }
        };

        Log.i(TAG, "Shutting down MountService");

        // Set initial variables and time out time.
        mActionDone = false;
        final long endShutTime = SystemClock.elapsedRealtime() + MAX_SHUTDOWN_WAIT_TIME;
        synchronized (mActionDoneSync) {
            try {
                final IMountService mount = IMountService.Stub.asInterface(
                        ServiceManager.checkService("mount"));
                if (mount != null) {
                    mount.shutdown(observer);
                } else {
                    Log.w(TAG, "MountService unavailable for shutdown");
                }
            } catch (Exception e) {
                Log.e(TAG, "Exception during MountService shutdown", e);
            }
            while (!mActionDone) {
                long delay = endShutTime - SystemClock.elapsedRealtime();
                if (delay <= 0) {
                    Log.w(TAG, "Shutdown wait timed out");
                    break;
                }
                try {
                    mActionDoneSync.wait(delay);
                } catch (InterruptedException e) {
                }
            }
        }
        rebootOrShutdown(mReboot, mRebootReason);
    }

    private void shutdownRadios(int timeout) {
        // If a radio is wedged, disabling it may hang so we do this work in another thread,
        // just in case.
        final long endTime = SystemClock.elapsedRealtime() + timeout;
        final boolean[] done = new boolean[1];
        Thread t = new Thread() {
            public void run() {
                boolean nfcOff;
                boolean bluetoothOff;
                boolean radioOff;

                final INfcAdapter nfc =
                        INfcAdapter.Stub.asInterface(ServiceManager.checkService("nfc"));
                final ITelephony phone =
                        ITelephony.Stub.asInterface(ServiceManager.checkService("phone"));
                final IBluetoothManager bluetooth =
                        IBluetoothManager.Stub.asInterface(ServiceManager.checkService(
                                BluetoothAdapter.BLUETOOTH_MANAGER_SERVICE));

                try {
                    nfcOff = nfc == null ||
                             nfc.getState() == NfcAdapter.STATE_OFF;
                    if (!nfcOff) {
                        Log.w(TAG, "Turning off NFC...");
                        nfc.disable(false); // Don't persist new state
                    }
                } catch (RemoteException ex) {
                Log.e(TAG, "RemoteException during NFC shutdown", ex);
                    nfcOff = true;
                }

                try {
                    bluetoothOff = bluetooth == null || !bluetooth.isEnabled();
                    if (!bluetoothOff) {
                        Log.w(TAG, "Disabling Bluetooth...");
                        bluetooth.disable(false);  // disable but don't persist new state
                    }
                } catch (RemoteException ex) {
                    Log.e(TAG, "RemoteException during bluetooth shutdown", ex);
                    bluetoothOff = true;
                }

                try {
                    radioOff = phone == null || !phone.isRadioOn();
                    if (!radioOff) {
                        Log.w(TAG, "Turning off radio...");
                        phone.setRadio(false);
                    }
                } catch (RemoteException ex) {
                    Log.e(TAG, "RemoteException during radio shutdown", ex);
                    radioOff = true;
                }

                Log.i(TAG, "Waiting for NFC, Bluetooth and Radio...");

                while (SystemClock.elapsedRealtime() < endTime) {
                    if (!bluetoothOff) {
                        try {
                            bluetoothOff = !bluetooth.isEnabled();
                        } catch (RemoteException ex) {
                            Log.e(TAG, "RemoteException during bluetooth shutdown", ex);
                            bluetoothOff = true;
                        }
                        if (bluetoothOff) {
                            Log.i(TAG, "Bluetooth turned off.");
                        }
                    }
                    if (!radioOff) {
                        try {
                            radioOff = !phone.isRadioOn();
                        } catch (RemoteException ex) {
                            Log.e(TAG, "RemoteException during radio shutdown", ex);
                            radioOff = true;
                        }
                        if (radioOff) {
                            Log.i(TAG, "Radio turned off.");
                        }
                    }
                    if (!nfcOff) {
                        try {
                            nfcOff = nfc.getState() == NfcAdapter.STATE_OFF;
                        } catch (RemoteException ex) {
                            Log.e(TAG, "RemoteException during NFC shutdown", ex);
                            nfcOff = true;
                        }
                        if (radioOff) {
                            Log.i(TAG, "NFC turned off.");
                        }
                    }

                    if (radioOff && bluetoothOff && nfcOff) {
                        Log.i(TAG, "NFC, Radio and Bluetooth shutdown complete.");
                        done[0] = true;
                        break;
                    }
                    SystemClock.sleep(PHONE_STATE_POLL_SLEEP_MSEC);
                }
            }
        };

        t.start();
        try {
            t.join(timeout);
        } catch (InterruptedException ex) {
        }
        if (!done[0]) {
            Log.w(TAG, "Timed out waiting for NFC, Radio and Bluetooth shutdown.");
        }
    }

    /**
     * Do not call this directly. Use {@link #reboot(Context, String, boolean)}
     * or {@link #shutdown(Context, boolean)} instead.
     *
     * @param reboot true to reboot or false to shutdown
     * @param reason reason for reboot
     */
    public static void rebootOrShutdown(boolean reboot, String reason) {
        if (reboot) {
            Log.i(TAG, "Rebooting, reason: " + reason);
            try {
                PowerManagerService.lowLevelReboot(reason);
            } catch (Exception e) {
                Log.e(TAG, "Reboot failed, will attempt shutdown instead");
            }
        } /* else if (SHUTDOWN_VIBRATE_MS > 0) {
            // vibrate before shutting down
            Vibrator vibrator = new SystemVibrator();
            try {
                vibrator.vibrate(SHUTDOWN_VIBRATE_MS);
            } catch (Exception e) {
                // Failure to vibrate shouldn't interrupt shutdown.  Just log it.
                Log.w(TAG, "Failed to vibrate during shutdown.", e);
            }

            // vibrator is asynchronous so we need to wait to avoid shutting down too soon.
            try {
                Thread.sleep(SHUTDOWN_VIBRATE_MS);
            } catch (InterruptedException unused) {
            }
        } */

        // Shutdown power
        Log.i(TAG, "Performing low-level shutdown...");
        PowerManagerService.lowLevelShutdown();
    }
}
