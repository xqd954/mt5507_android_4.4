package com.tcl.panel.simkey;

public class SimKey {
     native public void openLinuxDevFd();	 
	 native public void closeLinuxDevFd();
	 native public void setkeydown(int keyvalue);
	 native public void setkeyup(int keyvalue);
	 native public void setkey(int keyvalue);
	 static
	 {
	  System.loadLibrary("sim-key");
	 }
}