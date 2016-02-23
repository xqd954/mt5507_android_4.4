package com.mediatek.ui.util;

import android.view.KeyEvent;

public class KeyMap {
	/**
	 * The following keys correspond to the 58 keys on the remote control, remote control button on the order from top to bottom, left to right, for example,
       in onKeyDown() Function to determine if(keyCode = =
	 * KEYCODE_POWER) or if(keyCode = = KEYCODE_MTKIR_SOURCE) POWER Usage
     * KEYCODE_POWER SOURCE  Usage KEYCODE_MTKIR_SOURCE
	 */

	// key 1 POWER
	public static final int KEYCODE_POWER = KeyEvent.KEYCODE_POWER;
	// key 2 HOME
	public static final int KEYCODE_HOME = KeyEvent.KEYCODE_HOME;
	// key 3 MTKIR_SOURCE
	public static final int KEYCODE_MTKIR_SOURCE = KeyEvent.KEYCODE_MTKIR_SOURCE;
	// key 4 MTKIR_TIMER
	public static final int KEYCODE_MTKIR_TIMER = KeyEvent.KEYCODE_MTKIR_TIMER;
	// key 5 MTKIR_SLEEP
	public static final int KEYCODE_MTKIR_SLEEP = KeyEvent.KEYCODE_MTKIR_SLEEP;
	// key 6 MTKIR_ZOOM
	public static final int KEYCODE_MTKIR_ZOOM = KeyEvent.KEYCODE_MTKIR_ZOOM;
	// key 7 MTKIR_PEFFECT
	public static final int KEYCODE_MTKIR_PEFFECT = KeyEvent.KEYCODE_MTKIR_PEFFECT;
	// key 8 MTKIR_SEFFECT
	public static final int KEYCODE_MTKIR_SEFFECT = KeyEvent.KEYCODE_MTKIR_SEFFECT;
	// key 9 MTKIR_ASPECT
	public static final int KEYCODE_MTKIR_ASPECT = KeyEvent.KEYCODE_MTKIR_ASPECT;
	// key 10 MTKIR_PIPPOP
	public static final int KEYCODE_MTKIR_PIPPOP = KeyEvent.KEYCODE_MTKIR_PIPPOP;
	// key 11 MTKIR_POPPOS
	public static final int KEYCODE_MTKIR_PIPPOS = KeyEvent.KEYCODE_MTKIR_POPPOS;
	// key 12 MTKIR_PIPSIZE
	public static final int KEYCODE_MTKIR_PIPSIZE = KeyEvent.KEYCODE_MTKIR_PIPSIZE;
	// key 13 MTKIR_MTSAUDIO
	public static final int KEYCODE_MTKIR_MTSAUDIO = KeyEvent.KEYCODE_MTKIR_MTSAUDIO;
	// key 14 MTKIR_CC
	public static final int KEYCODE_MTKIR_MTKIR_CC = KeyEvent.KEYCODE_MTKIR_CC;
	// key 15 MTKIR_SWAP
	public static final int KEYCODE_MTKIR_MTKIR_SWAP = KeyEvent.KEYCODE_MTKIR_SWAP;
	// key 16 1
	public static final int KEYCODE_1 = KeyEvent.KEYCODE_1;
	// key 17 2
	public static final int KEYCODE_2 = KeyEvent.KEYCODE_2;
	// key 18 3
	public static final int KEYCODE_3 = KeyEvent.KEYCODE_3;
	// key 19 4
	public static final int KEYCODE_4 = KeyEvent.KEYCODE_4;
	// key 20 5
	public static final int KEYCODE_5 = KeyEvent.KEYCODE_5;
	// key 21 6
	public static final int KEYCODE_6 = KeyEvent.KEYCODE_6;
	// key 22 7
	public static final int KEYCODE_7 = KeyEvent.KEYCODE_7;
	// key 23 8
	public static final int KEYCODE_8 = KeyEvent.KEYCODE_8;
	// key 24 9
	public static final int KEYCODE_9 = KeyEvent.KEYCODE_9;
	// key 25 MTKIR_FREEZE
	public static final int KEYCODE_MTKIR_FREEZE = KeyEvent.KEYCODE_MTKIR_FREEZE;
	// key 26 0
	public static final int KEYCODE_0 = KeyEvent.KEYCODE_0;
	// key 27 PERIOD
	public static final int KEYCODE_PERIOD = KeyEvent.KEYCODE_PERIOD;
	// key 28 MTKIR_CHUP
	public static final int KEYCODE_MTKIR_CHUP = KeyEvent.KEYCODE_MTKIR_CHUP;
	// key 29 MTKIR_PRECH
	public static final int KEYCODE_MTKIR_PRECH = KeyEvent.KEYCODE_MTKIR_PRECH;
	// key 30 VOLUME_UP
	public static final int KEYCODE_VOLUME_UP = KeyEvent.KEYCODE_VOLUME_UP;
	// key 31 MTKIR_CHDN
	public static final int KEYCODE_MTKIR_CHDN = KeyEvent.KEYCODE_MTKIR_CHDN;
	// key 32 MTKIR_MUTE
	public static final int KEYCODE_MTKIR_MUTE = KeyEvent.KEYCODE_VOLUME_MUTE;
	// key 33 VOLUME_DOWN
	public static final int KEYCODE_VOLUME_DOWN = KeyEvent.KEYCODE_VOLUME_DOWN;
	// key 34 MENU
	public static final int KEYCODE_MENU = KeyEvent.KEYCODE_MENU;
	// key 35 DPAD_UP
	public static final int KEYCODE_DPAD_UP = KeyEvent.KEYCODE_DPAD_UP;
	// key 36 MTKIR_INFO
	public static final int KEYCODE_MTKIR_INFO = KeyEvent.KEYCODE_MTKIR_INFO;
	// key 37 DPAD_LEFT
	public static final int KEYCODE_DPAD_LEFT = KeyEvent.KEYCODE_DPAD_LEFT;
	// key 38 DPAD_CENTER
	public static final int KEYCODE_DPAD_CENTER = KeyEvent.KEYCODE_DPAD_CENTER;
	// key 39 DPAD_RIGHT
	public static final int KEYCODE_DPAD_RIGHT = KeyEvent.KEYCODE_DPAD_RIGHT;
	// key 40 MTKIR_GUIDE
	public static final int KEYCODE_MTKIR_GUIDE = KeyEvent.KEYCODE_MTKIR_GUIDE;
	// key 41 DPAD_DOWN
	public static final int KEYCODE_DPAD_DOWN = KeyEvent.KEYCODE_DPAD_DOWN;
	// key 42 BACK
	public static final int KEYCODE_BACK = KeyEvent.KEYCODE_BACK;
	// key 43 MTKIR_EJECT
	public static final int KEYCODE_MTKIR_EJECT = KeyEvent.KEYCODE_MEDIA_EJECT;
	// key 44 MTKIR_PLAYPAUSE
	public static final int KEYCODE_MTKIR_PLAYPAUSE = KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE;
	// key 45 MTKIR_STOP
	public static final int KEYCODE_MTKIR_STOP = KeyEvent.KEYCODE_MEDIA_STOP;
	// key 46 MTKIR_RECORD
	public static final int KEYCODE_MTKIR_RECORD = KeyEvent.KEYCODE_MEDIA_RECORD;
	// key 47 MTKIR_REWIND
	public static final int KEYCODE_MTKIR_REWIND = KeyEvent.KEYCODE_MEDIA_REWIND;
	// key 48 MTKIR_FASTFORWARD
	public static final int KEYCODE_MTKIR_FASTFORWARD = KeyEvent.KEYCODE_MEDIA_FAST_FORWARD;
	// key 49 MTKIR_PREVIOUS
	public static final int KEYCODE_MTKIR_PREVIOUS = KeyEvent.KEYCODE_MEDIA_PREVIOUS;
	// key 50 MTKIR_NEXT
	public static final int KEYCODE_MTKIR_NEXT = KeyEvent.KEYCODE_MEDIA_NEXT;
	// key 51 MTKIR_TITLEPBC
	public static final int KEYCODE_MTKIR_TITLEPBC = KeyEvent.KEYCODE_MTKIR_TITLEPBC;
	// key 52 MTKIR_SUBTITLE
	public static final int KEYCODE_MTKIR_SUBTITLE = KeyEvent.KEYCODE_MTKIR_SUBTITLE;
	// key 53 MTKIR_REPEAT
	public static final int KEYCODE_MTKIR_REPEAT = KeyEvent.KEYCODE_MTKIR_REPEAT;
	// key 54 MTKIR_ANGLE
	public static final int KEYCODE_MTKIR_ANGLE = KeyEvent.KEYCODE_MTKIR_ANGLE;
	// key 55 MTKIR_RED
	public static final int KEYCODE_MTKIR_RED = KeyEvent.KEYCODE_MTKIR_RED;
	// key 56 MTKIR_GREEN
	public static final int KEYCODE_MTKIR_GREEN = KeyEvent.KEYCODE_MTKIR_GREEN;
	// key 57 MTKIR_YELLOW
	public static final int KEYCODE_MTKIR_YELLOW = KeyEvent.KEYCODE_MTKIR_YELLOW;
	// key 58 MTKIR_BLUE
	public static final int KEYCODE_MTKIR_BLUE = KeyEvent.KEYCODE_MTKIR_BLUE;
	public static final int KEYCODE_MTKIR_CC = KeyEvent.KEYCODE_MTKIR_CC;

}
