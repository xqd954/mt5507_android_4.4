package com.mediatek.netcm.ethernet;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.ethernet.EthernetManager;

/**
 * This Class use to network connection test.
 * <ul>
 * <li> Ping gate way.</li>
 * <li> Use google url to Http connection test.</li>
 * </ul>
 */
public class EthernetStatusTest {
	private Context mContext;
	private static EthernetManager mEthManager;
	private static EthernetStatusTest mEthStatusTest = null;
	private static HttpURLConnection connect = null;
	private DhcpInfo mDhcpInfo = null;
	private static URL url = null;
	private static int code = 0;
	private final static String Testlink = "http://www.google.com";

	
	private EthernetStatusTest(Context context) {
		this.mContext = context;
		this.mEthManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
	}
	
	
	/**
	 * Create a new EthernetStatusTest instance.
	 * Applications will use for connect test.
	 * 
	 */
	public static EthernetStatusTest getInstance(Context context) {
		if(mEthStatusTest == null) {
			mEthStatusTest = new EthernetStatusTest(context);
		}
		
		return mEthStatusTest;
	}
	
	private String getGateWay() {
		mDhcpInfo = mEthManager.getDhcpInfo();
		if(mDhcpInfo == null) {
			return null;
		}
		
		int getWayAddr = mDhcpInfo.gateway;
		
		return intToString(getWayAddr);
	}
	
	/**
	 * Ping current network's gate way. Return the result of ping operation.
	 * 
	 * @return Return true if ping operation is success,false otherwise.
	 */
	public boolean conennectTest() {
		String gateWay = getGateWay();
		if(gateWay == null) {
			return false;
		}
		
		Socket server = null;
		try {
			server = new Socket();
			InetSocketAddress address = new InetSocketAddress(gateWay, 80);
			server.connect(address, 5000);
			//server.close();
			return true;
		} catch (UnknownHostException e) {
			return false;
		} catch (IOException e) {
			return false;
		}finally{
				try{
					server.close();
				}catch(Exception ex){
					ex.printStackTrace();
				}
		}
		
	}
	
	/**
	 * Use http connect to Google, test if the network connected or not.
	 * 
	 * @return Return true stands for network connect to Internet, false otherwise.
	 */
	public static boolean httpConnectTest() {
		 if(EthernetImplement.dummyMode){
			 return true;
		 }
		 
		try{
			url = new URL(Testlink);
			connect = (HttpURLConnection)url.openConnection();
			connect.setRequestMethod("GET");
			connect.setConnectTimeout(6*1000);
			code = connect.getResponseCode();
		} catch(Exception e) {
			return false;
		}
		
		if(code == HttpURLConnection.HTTP_OK)
		{
			return true;
		}
		else {
			return false;
		}
	}
	
	private String intToString(int data) {
		StringBuffer StringBuf = new StringBuffer();
		StringBuf.append(data & 0xff).append('.').append((data >>>= 8) & 0xff)
				.append('.').append((data >>>= 8) & 0xff).append('.').append(
						(data >>>= 8) & 0xff);
		return StringBuf.toString();
	}
}
