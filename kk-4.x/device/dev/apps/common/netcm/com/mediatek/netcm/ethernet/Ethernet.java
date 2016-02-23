package com.mediatek.netcm.ethernet;

public interface Ethernet {
	
	public void closeEthernet();
	public boolean openEthernet();
	public String getMacAddr();
	public int getState(); 
	public String getConnectMode();
	
	/*API's for DHCP.*/
	public void setConnectDhcp();
	public String getIpAddress(); 
	public String getNetMask(); 
	public String getRouteAddr(); 
	public String getDnsAddr();

	/*API's for manual.*/
	public boolean manualSetting(String ip, String NetMask, String RouteAddr, String DnsAddr);
}
