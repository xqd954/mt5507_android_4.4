package com.tcl.device.authentication;

import android.net.Uri;
import android.provider.BaseColumns;

class MyUsers {
	public static final String AUTHORITY  = "com.tcl.xian.StartandroidService.MyContentProvider";

	// BaseColumn类中已经包含了 _id字段
	public static final class User implements BaseColumns {
		public static final Uri CONTENT_URI  = Uri.parse("content://com.tcl.xian.StartandroidService.MyContentProvider/User");
		// 表数据列
		public static final String  USER_NAME ="USER_NAME";
		public static final String  USER_SEX ="USER_SEX";
	} 
	
	public static final class huanid implements BaseColumns {
		public static final Uri CONTENT_URI  = Uri.parse("content://com.tcl.xian.StartandroidService.MyContentProvider/huanid");
		// 表数据列
		public static final String  HUAN_ID ="huanid";
	} 
	
	public static final class devicetoken implements BaseColumns {
		public static final Uri CONTENT_URI  = Uri.parse("content://com.tcl.xian.StartandroidService.MyContentProvider/devicetoken");
		// 表数据列
		public static final String  ACTIVE_FLAG ="activeflag";
		public static final String  DEVICELOGIN_FLAG ="loginflag";
		public static final String  DEVICE_ID ="deviceid";
		public static final String  DUM="dum";
		public static final String  DEVICE_MODEL ="devicemodel";
		public static final String  ACTIVE_KEY ="activekey";
		public static final String  DIDTOKEN="didtoken";
		public static final String  TOKEN ="token";
		public static final String  HUAN_ID ="huanid";
		public static final String  LICENSE_TYPE ="license_type";
		public static final String  LICENSE_DATA ="license_data";
                public static final String  BOOT_TIMES ="boottimes";
	} 
}
