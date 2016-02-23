package com.mediatek.netcm.samba;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;

import com.mediatek.netcm.util.NetLog;

/**
 * Auxiliary class for Samba manager.
 * it use to save the user's login information into a configure file.
 */
public class SaveUserInfo {
	private static String mPathValue;
	private final static String mUserKey = "User";
	private final static String mPasswdKey = "Password";

	private static String mUserValue;
	private static String mPasswdValue;
	private static int mCount = 0;
	private static String dirName = "/data/misc/smb/";
	
	private static String TAG = "CM_SaveUserInfo";
	
	private static void parseLine(String line) {
		line = line.trim();
		if (line.matches("\\[.*\\]")) {
			mPathValue = line.replaceFirst("\\[(.*)\\]", "$1");
			mCount++;
		} else if (line.matches(".*=.*")) {
			int i = line.indexOf('=');
			String name = line.substring(0, i);
			String value = line.substring(i + 1);
			if (name.equals(mUserKey)) {
				mUserValue = value;
				mCount++;
			} else {
				mPasswdValue = value;
				mCount++;
			}
		}
	}

	/**
	 * Read the configure file, and create a hash table 
	 * according to the configure file.
	 * 
	 * @param fileName  the configure file's name.
	 * @param table     the hash table will be created.
	 * @return
	 */
	public static void readINIFile(String fileName,
			Hashtable<String, LoginInfo> table)  {//throws Exception
		if (fileName == null) {
			return;
		}
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(new InputStreamReader(
					new FileInputStream(fileName), "UTF-8"));
			String line;
			while ((line = reader.readLine()) != null) {
				parseLine(line);
				if (mCount == 3) {
					table.put(mPathValue, new LoginInfo(mUserValue,
							mPasswdValue));
					mCount = 0;
				}
			}
			//reader.close();
		} catch (Exception e) {
			NetLog.e(TAG, "[readINIFile]: read error.");
		}finally{
			if(reader!=null){
				try{
					reader.close();
				}catch(Exception ex){
					ex.printStackTrace();
				}
				
			}
		}
	}

	/**
	 * Write the configure file, according to the hash table.
	 * 
	 * @param fileName  the configure file's name.
	 * @param table     the hash table will be recorded.
	 * @return true if operation success, otherwise false.
	 * 
	 */
	public static boolean writeINIFile(String fileName,
			Hashtable<String, LoginInfo> table)  {//throws Exception
		if (table.isEmpty() || (fileName == null)) {
			NetLog.d(TAG, "[writeXMLFile]: Hashtable is Empty.");
			return false;
		}

		if(!createSmbDir()) {
			return false;
		}
		
		PrintWriter writer = null;
		try {
			File file = new File(fileName);
			if (!file.exists()) {
				file.createNewFile();
			}
			writer = new PrintWriter(new OutputStreamWriter(
					new FileOutputStream(file)));
			StringBuilder builder = new StringBuilder();

			Enumeration<String> ekey = table.keys();
			while (ekey.hasMoreElements()) {
				mPathValue = ekey.nextElement();

				LoginInfo info = table.get(mPathValue);
				builder.append("\n").append("\n").append("[").append(mPathValue).append("]");
				builder.append(getUserString(info));
				builder.append(getPsswdString(info));
			}

			writer.write(builder.toString().trim());
			writer.flush();
			//writer.close();
			return true;
		} catch (Exception e) {
			NetLog.e(TAG, "[writeINIFile]:write error(" + fileName + ").");
			return false;
		}finally{
			if(writer!=null){
				try{
					writer.close();
				}catch(Exception ex){
					ex.printStackTrace();
				}
					
			}
		}
	}

	private static boolean createSmbDir() {
		File dir = new File(dirName);
		if(dir.exists()) {
			return true;
		}
		
		if(!dirName.endsWith(File.separator)) {
			dirName = dirName + File.separator;
		}
		
		if(dir.mkdirs()) {
			NetLog.d(TAG, "Create smb directory success.");
			return true;
		}
		
		NetLog.d(TAG, "Create smb directory failed.");
		return false;
	}
	
	private static String getUserString(LoginInfo info) {
		StringBuilder builder = new StringBuilder();
		builder.append("\n").append(mUserKey).append("=").append(
				info.getUserName());
		return builder.toString();
	}
	
	private static String getPsswdString(LoginInfo info) {
		StringBuilder builder = new StringBuilder();
		builder.append("\n").append(mPasswdKey).append("=").append(
				info.getPasswd());
		return builder.toString();
	}

}
