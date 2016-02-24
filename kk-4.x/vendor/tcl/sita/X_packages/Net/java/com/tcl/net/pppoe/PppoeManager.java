/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package com.tcl.net.pppoe;

import android.content.Context;
import android.os.Binder;
import android.os.IBinder;
import android.os.ServiceManager;
import android.net.pppoe.IPppoeManager;


public class PppoeManager {

	private static final Object mInstanceSync = new Object();
	private static android.net.pppoe.PppoeManager _myinst = null;

 //tmp. modify build error! by gaodw.
 public static final String PPPOE_SERVICE = "pppoe";
 //END

    private PppoeManager(IPppoeManager pppoeService) {
	}

    static public PppoeManager getInstance() {
        /* Double-checked locking */
        if (_myinst == null) {
            synchronized (mInstanceSync) {
                if (_myinst == null) {
                   
		    //tmp. modify build error! bu gaodw.			
		    IBinder b = ServiceManager.getService(PPPOE_SERVICE);
		    //IBinder b = ServiceManager.getService(Context.PPPOE_SERVICE);
                    //end
		    
		    _myinst = new android.net.pppoe.PppoeManager(IPppoeManager.Stub.asInterface(b));
                }
            }
        }

        return new PppoeManager(null);
    }


    public static final java.lang.String PPPOE_STATE_CONNECT = android.net.pppoe.PppoeManager.PPPOE_STATE_CONNECT;
    public static final java.lang.String PPPOE_STATE_DISCONNECT = android.net.pppoe.PppoeManager.PPPOE_STATE_DISCONNECT;
    public static final java.lang.String PPPOE_STATE_CONNECTING = android.net.pppoe.PppoeManager.PPPOE_STATE_DISCONNECTING;
    public static final java.lang.String PPPOE_STATE_DISCONNECTING = android.net.pppoe.PppoeManager.PPPOE_STATE_AUTHFAILED;
    public static final java.lang.String PPPOE_STATE_AUTHFAILED = android.net.pppoe.PppoeManager.PPPOE_STATE_AUTHFAILED;
    public static final java.lang.String PPPOE_STATE_LINKTIMEOUT = android.net.pppoe.PppoeManager.PPPOE_STATE_LINKTIMEOUT;
    public static final java.lang.String PPPOE_STATE_FAILED = android.net.pppoe.PppoeManager.PPPOE_STATE_FAILED;
    public static final int MSG_PPPOE_CONNECT = android.net.pppoe.PppoeManager.MSG_PPPOE_CONNECT;
    public static final int MSG_PPPOE_DISCONNECT = android.net.pppoe.PppoeManager.MSG_PPPOE_DISCONNECT;
    public static final int MSG_PPPOE_CONNECTING = android.net.pppoe.PppoeManager.MSG_PPPOE_CONNECTING;
    public static final int MSG_PPPOE_DISCONNECTING = android.net.pppoe.PppoeManager.MSG_PPPOE_DISCONNECTING;
    public static final int MSG_PPPOE_AUTH_FAILED = android.net.pppoe.PppoeManager.MSG_PPPOE_AUTH_FAILED;
    public static final int MSG_PPPOE_TIME_OUT = android.net.pppoe.PppoeManager.MSG_PPPOE_TIME_OUT;
    public static final int MSG_PPPOE_FAILED = android.net.pppoe.PppoeManager.MSG_PPPOE_FAILED;
    public static final java.lang.String PPPOE_STATE_ACTION = android.net.pppoe.PppoeManager.PPPOE_STATE_ACTION;
    public static final java.lang.String PPPOE_STATE_STATUE = android.net.pppoe.PppoeManager.PPPOE_STATE_STATUE;


    public void connectPppoe(java.lang.String p1, java.lang.String p2){
		_myinst.connectPppoe(p1, p2);
	}
    public void connectPppoe(java.lang.String p1, java.lang.String p2, java.lang.String p3){
		_myinst.connectPppoe(p1, p2, p3);
	}
    public void disconnectPppoe(){
		_myinst.disconnectPppoe();
	}
    public java.lang.String getInterfaceName(){
		return _myinst.getInterfaceName();
	}
    public java.lang.String getIpaddr(){
		return _myinst.getIpaddr();
	}
    public java.lang.String getRoute(){
		return _myinst.getRoute();
	}
    public java.lang.String getMask(){
		return _myinst.getMask();
	}
    public java.lang.String getDns1(){
		return _myinst.getDns1();
	}
    public java.lang.String getDns2(){
		return _myinst.getDns2();
	}
    public java.lang.String getPppoeStatus(){
		return _myinst.getPppoeStatus();
	}
    public boolean PppoeSetInterface(java.lang.String p1){
		return _myinst.PppoeSetInterface(p1);
	}
    public boolean PppoeSetUser(java.lang.String p1){
		return _myinst.PppoeSetUser( p1 );
	}
    public boolean PppoeSetPW(java.lang.String p1 ){
		return _myinst.PppoeSetPW(p1);
	}
    public boolean PppoeDialup(){
		return _myinst.PppoeDialup();
	}
    public void PppoeHangUp(){
		_myinst.PppoeHangUp();
	}
    public void PppoeMonitor(){
		_myinst.PppoeMonitor();
	}
    public com.tcl.net.pppoe.PPPOE_STA PppoeGetStatus(){
		android.net.pppoe.PPPOE_STA _sta = _myinst.PppoeGetStatus();
		com.tcl.net.pppoe.PPPOE_STA _re_sta = PPPOE_STA.DISCONNECTED;
		if(_sta == android.net.pppoe.PPPOE_STA.CONNECTED){
			_re_sta = PPPOE_STA.CONNECTED;
		}else if(_sta == android.net.pppoe.PPPOE_STA.DISCONNECTED){
			_re_sta = PPPOE_STA.DISCONNECTED;
		}else if(_sta == android.net.pppoe.PPPOE_STA.CONNECTING){
			_re_sta = PPPOE_STA.CONNECTING;
		}
		return _re_sta;
	}
    public java.lang.String PppoeGetUser(){
		return _myinst.PppoeGetUser();
	}
    public java.lang.String PppoeGetPW(){
		return _myinst.PppoeGetPW();
	}
    public java.lang.String PppoeGetInterface(){
		return _myinst.PppoeGetInterface();
	}
    public boolean isPppoeRunning(){
		return _myinst.isPppoeRunning();
	}
  
}
