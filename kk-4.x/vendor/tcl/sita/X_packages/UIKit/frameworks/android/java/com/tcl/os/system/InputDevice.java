package com.tcl.os.system;

import android.content.Context;
import android.hardware.input.InputManager;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class InputDevice {
    private Context mContext;
	private static final String TAG = "InputDevice";

    public InputDevice(Context context){
        mContext = context;
    }


	static {
		System.loadLibrary("inputproc_jni");
	}

	static private native int changesinglemod(int a);

	static private native int input_event_in(String a);


	public static int injectEvent(String ev){
		
		return input_event_in(ev); //use jni
		
		/** use java 
		String[] args = new String[2];
		args[0] = "text";
		args[1] = ev;
		InputPrivate.getInstance().InputRunWithCmds(args);
		return 0;
		**/
	}
	
    /**
     * Convert the characters of string text into key event's and send to
     * device.
     *
     * @param text is a string of characters you want to input to the device.
     */
    public static void sendText(String text) {

        StringBuffer buff = new StringBuffer(text);

        boolean escapeFlag = false;
        for (int i=0; i<buff.length(); i++) {
            if (escapeFlag) {
                escapeFlag = false;
                if (buff.charAt(i) == 's') {
                    buff.setCharAt(i, ' ');
                    buff.deleteCharAt(--i);
                }
            } 
            if (buff.charAt(i) == '%') {
                escapeFlag = true;
            }
        }

        char[] chars = buff.toString().toCharArray();

        KeyCharacterMap kcm = KeyCharacterMap.load(KeyCharacterMap.VIRTUAL_KEYBOARD);
        KeyEvent[] events = kcm.getEvents(chars);
        for(int i = 0; i < events.length; i++) {
            injectKeyEvent(events[i]);
        }
    }


    /**
     * send key eventto device.
     *
     * @param keyCode is key code you want to input to the device.
     */
    public static void sendKeyEvent(int keyCode) {
        long now = SystemClock.uptimeMillis();
        injectKeyEvent(new KeyEvent(now, now, KeyEvent.ACTION_DOWN, keyCode, 0, 0,
                KeyCharacterMap.VIRTUAL_KEYBOARD, 0, 0, android.view.InputDevice.SOURCE_KEYBOARD));
        injectKeyEvent(new KeyEvent(now, now, KeyEvent.ACTION_UP, keyCode, 0, 0,
                KeyCharacterMap.VIRTUAL_KEYBOARD, 0, 0, android.view.InputDevice.SOURCE_KEYBOARD));
    }
	

    private static void injectKeyEvent(KeyEvent event) {
        Log.i(TAG, "injectKeyEvent: " + event);
        InputManager.getInstance().injectInputEvent(event,
                InputManager.INJECT_INPUT_EVENT_MODE_ASYNC); /*INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH lvh@tcl modify it, 2014-3-13 */
    }

}
