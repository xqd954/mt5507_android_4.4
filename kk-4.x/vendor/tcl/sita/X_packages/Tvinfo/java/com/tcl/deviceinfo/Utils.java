package com.tcl.deviceinfo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

import com.tcl.deviceinfo.TDeviceInfo;

import java.io.InputStream;
import java.io.OutputStream;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;

import android.util.Log;

import android.os.FileUtils;

class Utils {
	private static final String TAG = "Utils";
	private static final boolean LOGI = true;
	private static final String DEVINFO_FILENAME = "/data/devinfo.txt";
	private static final String CITY_FILENAME = "/data/cityname.txt";
	private static final String DEVINFO_FILE_PERMISSION = "chmod 777 /data/devinfo.txt";
	private static final String CITY_FILE_PERMISSION = "chmod 777 /data/cityname.txt";
	
	private static int rootSeeker(String cmd) {
		// TODO Auto-generated method stub
		LocalSocketAddress lsa = new android.net.LocalSocketAddress("sserver", LocalSocketAddress.Namespace.RESERVED);
		LocalSocket ls = new LocalSocket();
		byte[] buffer = new byte[16]; // receive result "ok" or "fail"
		InputStream is = null;
		OutputStream os = null;
		
		
		try {
			Log.d("RootSeeker","start connect");
			ls.connect(lsa);
			is = ls.getInputStream();
			os = ls.getOutputStream();
			os.write(cmd.getBytes());
			int rlen = is.read(buffer);
			if(rlen != -1){

			String result = new String(buffer, 0, rlen);
				if (result.equals("ok")) {
					Log.d("localSocketDemo", "OK");
				} else if (result.equals("fail")) {
					Log.d("localSocketDemo", "FAIL");
				}
			}else{
				Log.d("localSocketdemo"," read failed!!!!!!!!!!!");
			}
			
			is.close();
			os.close();
			ls.close();

			return 0;

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		return 0;
	}

	public static void createDeviceInfoFile() {
		TDeviceInfo mDeviceInfo = TDeviceInfo.getInstance();
		FileWriter fw=null;
		try {
			fw = new FileWriter(Utils.DEVINFO_FILENAME);
			fw.write("devid=" + mDeviceInfo.getDeviceID() + "\n");
			fw.write("sv=" + mDeviceInfo.getSoftwareVersion() + "\n"); 
			fw.write("hv=" + mDeviceInfo.getHardwareVersion() + "\n"); 
			fw.write("devmodel=" + mDeviceInfo.getClientType(mDeviceInfo.getProjectID()) + "\n"); 
			fw.flush();
		} catch (Exception e) {
			e.printStackTrace();
		} finally{
			try{
				if(fw!=null){
					fw.close();
				}
	     FileUtils.setPermissions(Utils.DEVINFO_FILENAME, 
                                      FileUtils.S_IRUSR | 
                                      FileUtils.S_IWUSR | 
                                      FileUtils.S_IRGRP | 
                                      FileUtils.S_IROTH, -1, -1);
                                
				//Utils.rootSeeker( Utils.DEVINFO_FILE_PERMISSION );
			}catch(Exception e){
				e.printStackTrace();
			}
		}   
	}
	
	public static void setCityName(String mCityName){
		FileWriter fw=null;
		try {
			fw = new FileWriter(Utils.CITY_FILENAME);
			fw.write(mCityName);
			fw.flush();
		} catch (Exception e) {
			e.printStackTrace();
		} finally{
			try{
				if(fw!=null){
					fw.close();
				}
				Utils.rootSeeker(Utils.CITY_FILE_PERMISSION);
			}catch(Exception e){
				e.printStackTrace();
			}
		}   
	}
	
	public static String getCityName(){
		synchronized (Utils.class) {
			File type = new File(Utils.CITY_FILENAME);
			String mCityName = "";
			FileReader read;
			if(type.exists()&&type.canRead()){
				try {
					read = new FileReader(type);
					BufferedReader br = new BufferedReader(read);
					if (br.ready()) {
						mCityName = br.readLine();
						br.close();
						return mCityName;
					} else {
						if(LOGI) Log.i(TAG,"can't read /data/cityname.txt");
						return mCityName;
					}
				} catch (Exception e) {
					e.printStackTrace();
					return mCityName;
				}
			}else{
				return mCityName;
			}
		}
	}
}
