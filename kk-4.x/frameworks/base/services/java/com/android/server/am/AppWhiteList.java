/*
 * Copyright (C) 2013 The TCL Android TV
 *
 */
 package com.android.server.am;

import java.io.File;
import java.io.FileInputStream;
import java.io.StringReader;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.xml.parsers.SAXParserFactory;
import org.apache.http.util.EncodingUtils;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import org.xmlpull.v1.XmlSerializer;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.util.Xml;

public class AppWhiteList {
	private final static String TAG = "AppWhiteList";
	private final static String PATCH_WHITE_LIST = "/system/etc/appwhitelist.xml";
	private final static boolean DBUG = true;
	private static List<String> mAppWhiteList= new ArrayList<String>();
	private final Context mContext;
	private final static String mSanDUI = "com.tcl.ui6";

	private DefaultHandler mDefaultHandler = new DefaultHandler() {
		public void startElement(String uri, String localName, String qName,
				Attributes attributes) throws SAXException {
			super.startElement(uri, localName, qName, attributes);
			if(localName.trim().equals("appItem")){
				String value =attributes.getValue(0).trim();
				AppWhiteList.mAppWhiteList.add(value);
			}
		}
	};

	public AppWhiteList(Context context) {
		mContext = context;
	//	createAppWhitelist(PATCH_WHITE_LIST);
		onParseWhiteListXml(PATCH_WHITE_LIST);
	}

	public void onKillBackProcess () {
		if(mAppWhiteList .size() <=0) {
	//		createAppWhitelist(PATCH_WHITE_LIST);
			onParseWhiteListXml(PATCH_WHITE_LIST);
		}
		if(mAppWhiteList.size() <=0)  {
                        if(DBUG) Log.i(TAG, "AppWhiteList size is null, do nothing!");
                        return;
                }
		
	/*	ComponentName cn = ((ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE))
				.getRunningTasks(1).get(0).topActivity;
		if(DBUG) Log.i(TAG, "AppWhiteList Size:" + mAppWhiteList .size());
		if(DBUG) Log.i(TAG, "ComponentName:" +cn.getPackageName());
		if(!cn.getPackageName().equals(mSanDUI))
			return;
	*/	
//		if(DBUG) Log.i(TAG, "Killing back process!");
		ExecutorService exec = Executors.newCachedThreadPool();
		exec.execute(new Runnable() {
			@Override
			public void run() {
				onKillProcessAction ();
			}
		});
		exec.shutdown();
	}

	private void onParseWhiteListXml(String xmlFileName) {
		if(DBUG) Log.i(TAG, "Parse white list from xml's file!");
		File file = new File(xmlFileName);
		String res = "";
		if (file.exists()) {
			try {
				FileInputStream fin = new FileInputStream(file);
				int length = fin.available();
				byte[] buffer = new byte[length];
				fin.read(buffer);
				res = EncodingUtils.getString(buffer, "UTF-8");
				fin.close();
				
				if("".equals(res)) return;
				SAXParserFactory factory = SAXParserFactory.newInstance(); 
				XMLReader reader= factory.newSAXParser().getXMLReader();
				reader.setContentHandler(mDefaultHandler);
				reader.parse(new InputSource(new StringReader(res)));
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private void onKillProcessAction () {
		try {
			
            /*
            ActivityManager am = (ActivityManager)mContext.getSystemService(Context.ACTIVITY_SERVICE);
	 		ComponentName cn = am.getRunningTasks(1).get(0).topActivity;
			if(cn.getPackageName() != null){
				if (!(mAppWhiteList.contains(cn.getPackageName()))) {
					Log.d(TAG,"######Killing ===>   " + cn.getPackageName() + "    <===");
					Method forceStopPackage = am.getClass().getDeclaredMethod("forceStopPackage", String.class);
					forceStopPackage.setAccessible(true);
					forceStopPackage.invoke(am, cn.getPackageName());
				}
			}
            */
            
            ActivityManager am = (ActivityManager)mContext.getSystemService(Context.ACTIVITY_SERVICE);
			List<RunningAppProcessInfo> appProcessInfos = am.getRunningAppProcesses();
			for (RunningAppProcessInfo appProcessInfo : appProcessInfos) {
				String processName = appProcessInfo.processName;
                //Log.d(TAG,"######getRunningAppProcesses : ===>   " + processName + "    <===");
                
				if (!(mAppWhiteList.contains(processName))) {
					Log.d(TAG,"######forceStopPackage Killing ===>   " + processName + "    <===");
					Method forceStopPackage = am.getClass().getDeclaredMethod("forceStopPackage", String.class);
					forceStopPackage.setAccessible(true);
					forceStopPackage.invoke(am, processName);
                    System.gc();
				}
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void createAppWhitelist(String xmlFileName) {
		
		File xmlFile = new File(xmlFileName);
		String res = "";

		Log.d(TAG,"enter createAppWhitelist");
		if(xmlFile.exists()) {
			try {
				if(DBUG) Log.i(TAG, "App white list file is exists!");
				FileInputStream fin = new FileInputStream(xmlFile);
				int length = fin.available();
				byte[] buffer = new byte[length];
				fin.read(buffer);
				res = EncodingUtils.getString(buffer, "UTF-8");
				fin.close();
			} catch (Exception e) {
			}
			
			if(!("".equals(res))) {
				if(DBUG) Log.i(TAG, "App white list file is OK!");
				return;
			}
		}
		
		try {
			if(DBUG) Log.i(TAG, "Does'nt exist or empty,create app white list now!");
			xmlFile.createNewFile();
			xmlFile.setReadable(true);
			xmlFile.setWritable(true);
		} catch (IOException e) {
			e.printStackTrace();
		}

		FileOutputStream fileos = null;
		try {
			fileos = new FileOutputStream(xmlFile);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		XmlSerializer serializer = Xml.newSerializer();
		try {
			serializer.setOutput(fileos, "UTF-8");
			serializer.startDocument(null, Boolean.valueOf(true));
			serializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
			serializer.startTag(null, "appWhiteList");
			
			ActivityManager am = (ActivityManager)mContext.getSystemService(Context.ACTIVITY_SERVICE);
			List<RunningAppProcessInfo> appProcessInfos = am.getRunningAppProcesses();
			List<String> mProcesses= new ArrayList<String>();
			for (RunningAppProcessInfo appProcessInfo : appProcessInfos) {
				mProcesses.add(appProcessInfo.processName);
			}
			for(int i=0;i<mProcesses.size();i++) { 
				String mProcesse = mProcesses.get(i);
				serializer.startTag(null, "appItem");
				serializer.attribute(null, "packageName", mProcesse);
				serializer.endTag(null, "appItem");
			}
			
			List<PackageInfo> packages = mContext.getPackageManager().getInstalledPackages(0);
			for(int i=0;i<packages.size();i++) { 
				PackageInfo packageInfo = packages.get(i);
				if (!(mProcesses.contains(packageInfo.packageName))) {
					serializer.startTag(null, "appItem");
					serializer.attribute(null, "packageName", packageInfo.packageName);
					serializer.endTag(null, "appItem");
				}
			}
			serializer.endTag(null, "appWhiteList");
			serializer.endDocument();
			serializer.flush();
			fileos.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}	
}
