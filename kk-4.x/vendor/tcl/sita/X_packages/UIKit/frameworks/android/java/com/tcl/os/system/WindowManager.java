package com.tcl.os.system;

import android.content.Context;
import android.view.KeyEvent;
import android.view.InputEvent;
import android.os.ServiceManager;
import android.view.IWindowManager;
import java.lang.Exception;
import android.hardware.input.InputManager;
import android.util.Log;
import android.content.ComponentName; //add by tank@tcl.com
public class WindowManager {
	private final static String TAG = "WindowManager";
	private Context mContext;
	private static WindowManager mInstance;
	private static InputManager mInputManager;
	private boolean Result = false;
	private int Psync;

	public WindowManager(Context context) {
		mContext = context;
		mInputManager = InputManager.getInstance();
		if (mInputManager == null) {
			Log.i(TAG, "Error----mInputManager:null----");
		}
	}

	/**
	 * inject the keyevent.
	 * 
	 * @return false if the key inject failed
	 */
	public boolean injectKeyEvent(KeyEvent Event, boolean sync) {
		Log.i(TAG, "----injectKeyEvent----");
		if (sync) {
			Psync = 1;
		} else {
			Psync = 0;
		}
		return mInputManager.injectInputEvent(Event, Psync);

	}

	/**
	 * 设置appKeyguard，截取Home键。默认Home键由系统处理。
	 * 
	 * @param true：应用截取Home键，按键交由应用处理；false：把Home键控制权交给系统，按键交由系统处理
	 * @return 无
	 */
	public void appKeyguard(boolean bAppKeyguard) {
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			wm.appKeyguard(bAppKeyguard);
		} catch (Exception e)  { 
		}
	}

	/**
	 * 获取appKeyguard状态
	 * 
	 * @param 无
	 * @return true：Home键控制权在应用；false：Home键控制权限在系统
	 */
	public boolean isAppKeyguard() {
		boolean mRet = false;
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			mRet = wm.isAppKeyguard();
		} catch (Exception e)  { 
		}
		return mRet;
	}

	/**
	 * 禁止Home键功能
	 * 
	 * @param true：启用Home键功能；false：禁止Home键功能。默认Home键可用
	 * @return 无
	 */
	public void activeHomeKey(boolean bActiveHomeKey) {
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			wm.activeHomeKey(bActiveHomeKey);
		} catch (Exception e)  { 
		}     
	}

	/**
	 * 判断当前Home键功能是否可用
	 * 
	 * @param 无
	 * @return true：Home键功能可用；false：Home键被禁用
	 */
	public boolean isHomeKeyActive() {
		boolean mRet = false;
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			mRet = wm.isHomeKeyActive();
		} catch (Exception e)  { 
		}
		return mRet;
	}

	/**
	 * 设置Home键的响应间隔，小于指定时间间隔的按键将忽略。默认响应间隔为：1000ms。
	 * 
	 * @param 时间间隔
	 * @return 无
	 */
	public void setHomeKeyInterval(int iInterval) {
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			wm.setHomeKeyInterval(iInterval);
		} catch (Exception e)  { 
		}  
	}

	/**
	 * 用于标识主菜单的运行状态
	 * 
	 * @param true：当前运行进程为主菜单；false：当前运行进程非主菜单
	 * @return 无
	 */
	public void showSanDUI(boolean bShowSanDUI) {
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			wm.showSanDUI(bShowSanDUI);
		} catch (Exception e)  { 
		}  
	}

	/**
	 * 判断当前运行进程是否为主菜单
	 * 
	 * @param 无
	 * @return true：当前运行进程为主菜单；false：当前运行进程非主菜单。
	 */
	public boolean isShowSanDUI() {
		boolean mRet = false;
        try {
			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
			mRet = wm.isShowSanDUI();
		} catch (Exception e)  { 
		}
		return mRet;
	}
   
   /**
     * 禁止按键响应功能
     * 
     * @param Keycode: 需要设置的android键值
     *        isActive: true：启用按键功能；false：禁止按键功能。默认启用按键功能。
     * @return 无
     */
    public void activeKeyByKeycode(int Keycode, boolean isActive) {
        try {
            IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
            wm.activeKeyByKeycode(Keycode, isActive);
        } catch (Exception e)  {
        }
    }
    //add by tank@tcl.com
    public void activeKeyByKeycodeCnPn(int num, int[] Keycode, boolean isActive, ComponentName ComName, String packageName) {
        try {
            IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
            wm.activeKeyByKeycodeCnPn(num,Keycode,isActive,ComName,packageName);
        } catch (Exception e)  {
        }
    }
    //end tank@tcl.com
    /**
     * 判断按键功能当前是否可用
     * 
     * @param :Keycode 需要判断的按键键值
     * @return true：按键功能可用；false：按键被禁用
     */
    public  boolean isThisKeyActive(int Keycode) {
        boolean mRet = false;
        try {
            IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
            mRet = wm.isThisKeyActive(Keycode);
        } catch (Exception e)  {
        }
        return mRet;
    }
}
