package com.mediatek.ui.menu.util;

public class IPData {

	/**
	 * IP string.
	 */
	private StringBuffer sb_ip;
	/**
	 * IP address family.
	 */
	private int ip_family;
	/**
	 * Net address length.
	 */
	private int net_length;
	/**
	 * Machine address length.
	 */
	private int machine_length;

	/**
	 * 
	 * @param str
	 *            IP String.
	 */
	public IPData(StringBuffer str) {
		sb_ip = new StringBuffer();
		sb_ip.append(str);
		if (str.toString().length() > 0) {
			initIPData();
		}
	}

	/**
	 * Check if the sub net mask is fit for the ip.
	 * 
	 * @param ip
	 *            The sub net mask.
	 * @return
	 */
	public boolean checkSubnetMask(StringBuffer ip) {
		IPData compare = new IPData(ip);
		int i=0;
		for (i = 0; i < net_length; i++) {
			if (!compare.getSection(i).equalsIgnoreCase(IPBoxSetting.SMAX_IP)) {
				return false;
			}
		}
		for(i=net_length;i<4;i++){
			if(! checkMaskMachine(compare.getSection(i))){
				return false;
			}
		}
		return true;
	}

	/**
	 * Check if the sub net mask is fit for the ip.
	 * 
	 * @param ip
	 *            The sub net mask IPData.
	 */
	public boolean checkSubnetMask(IPData ip) {
		int i=0;
		for (i = 0; i < net_length; i++) {
			if (!ip.getSection(i).equalsIgnoreCase(IPBoxSetting.SMAX_IP)) {
				return false;
			}
		}
		
		for(i=net_length;i<4;i++){
			if(! checkMaskMachine(ip.getSection(i))){
				return false;
			}
		}
		return true;
	}

	private boolean checkMaskMachine(String str) {
		int sec = parseInt(str);
		int temp = 128;		
		boolean ret_pre = true;
		while (temp >= 1) {
			if ((temp & sec) == temp && ret_pre == true) {
				ret_pre = true;
			} else if ((temp & sec) == temp && ret_pre == false) {
				return false;
			} else if ((temp & sec) != temp) {
				ret_pre = false;
			}
			temp/=2;
		}
		return true;
	}

	/**
	 * Check if current ip is in the same sub net with route according to the
	 * sub net mask.
	 * 
	 * @param subnetMask
	 *            The sub net mask.
	 * @param sroute
	 *            The route address.
	 */
	public boolean checkRoute(StringBuffer subnetMask, StringBuffer sroute) {
		IPData route = new IPData(sroute);
		if (route.getSubnetCode(subnetMask).toString().equalsIgnoreCase(
				getSubnetCode(subnetMask).toString())) {
			return true;
		}
		return false;
	}

	/**
	 * Get the sub net address according to the sub net mask.
	 * 
	 * @param subnetMask
	 *            The sub net mask.
	 * @return The sub net address
	 */
	public StringBuffer getSubnetCode(StringBuffer subnetMask) {
		StringBuffer sb_result = new StringBuffer();
		IPData mask = new IPData(subnetMask);
		for (int i = 0; i < 4; i++) {
			int mask_byte = parseInt(mask.getSection(i));
			int ip_byte = parseInt(getSection(i));
			int ip_code_byte = mask_byte & ip_byte;
			sb_result.append(ip_code_byte + "");
			sb_result.append('.');
		}
		sb_result = sb_result.deleteCharAt(sb_result.length() - 1);
		return sb_result;
	}

	/**
	 * Parse the string into integer.
	 * 
	 * @param sbSection
	 */
	private int parseInt(String sbSection) {
		int result = 0;
		for (int i = 0; i < sbSection.length(); i++) {
			result *= 10;
			result += Integer.parseInt(sbSection.substring(i, i + 1));
		}
		return result;
	}

	/**
	 * Get sub IP section string.
	 * 
	 * @param section
	 *            section index,must less than 4 ,more than -1
	 * @return
	 */
	private String getSection(int section) {
		return sb_ip.substring(4 * section, 4 * section + 3);
	}

	/**
	 * Initial the ip family and net address length.
	 */
	private void initIPData() {

		if (-1 == (ip_family = getIPFamily())) {
			ip_family = 0;
		}
		getNetandMachine();
	}

	/**
	 * Get the current ip address family.
	 * 
	 * @return 0:IP address of type A. 1:IP address of type B. 2:IP address of
	 *         type C.
	 */
	private int getIPFamily() {
		String sPre = sb_ip.substring(0, 3);
		int iPre = 0;

		try {
			iPre = Integer.parseInt(sPre);
		} catch (Exception e) {
			return -1;
		}

		if (iPre > IPBoxSetting.ADDRESS_FAMILY_A_START
				&& iPre < IPBoxSetting.ADDRESS_FAMILY_A_END) {
			return IPBoxSetting.ADDRESS_FAMILY_A;
		} else if (iPre > IPBoxSetting.ADDRESS_FAMILY_B_START
				&& iPre < IPBoxSetting.ADDRESS_FAMILY_B_END) {
			return IPBoxSetting.ADDRESS_FAMILY_B;
		} else if (iPre > IPBoxSetting.ADDRESS_FAMILY_C_START
				&& iPre < IPBoxSetting.ADDRESS_FAMILY_C_END) {
			return IPBoxSetting.ADDRESS_FAMILY_C;
		} else
			return -1;
	}

	/**
	 * Get the number of ip section of current address.
	 */
	private void getNetandMachine() {
		net_length = ip_family + 1;
		machine_length = 4 - net_length;
	}
	
	public static final class IPBoxSetting {
		public static String DEFAULT_STRING = "---";
		public static String DEFAULT_VALUE = "0";
		public static String CURRENT_STRING = "__";
		public static String IP_DOT = ".";

		public static int IPLINE_NUM = 12;
		public static int IPSEC_NUM = 3;
		public static int MIN_NUM = 7;
		public static int MAX_NUM = 16;
		public static int BOX_NUM = 48;
		public static int MAX_IP = 255;
		public static String SMAX_IP = "255";
		public static int FMAX_IP = 224;

		public static int BOX_HEIGHT = 47;
		public static int BOX_WIDTH = 32;
		public static int DOT_WIDTH = 25;
		
		public final static int FORMAT_ERROR_NUM=0;
		public final static int FORMAT_ERROR_NET=1;
		public final static int CONNECT_FAIL=2;
		public final static int CONNECT_SUCCESS=3;
		
		public final static int ADDRESS_FAMILY_A=0;
		public final static int ADDRESS_FAMILY_A_START=1;
		public final static int ADDRESS_FAMILY_A_END=126;
		public final static int ADDRESS_FAMILY_B=1;
		public final static int ADDRESS_FAMILY_B_START=128;
		public final static int ADDRESS_FAMILY_B_END=191;
		public final static int ADDRESS_FAMILY_C=2;
		public final static int ADDRESS_FAMILY_C_START=192;
		public final static int ADDRESS_FAMILY_C_END=223;
		
		public final static String MANUAL_IPS = "manual_ips";	
	}
}
