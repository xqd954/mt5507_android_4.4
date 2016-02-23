package com.mediatek.netcm.samba;

import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.sql.Date;
import java.util.Collections;
import java.util.Comparator;
import java.util.Hashtable;
import java.util.LinkedList;

import com.mediatek.netcm.util.NetLog;

import jcifs.netbios.NbtAddress;
import jcifs.smb.NtlmPasswordAuthentication;
import jcifs.smb.SmbAuthException;
import jcifs.smb.SmbException;
import jcifs.smb.SmbFile;
import jcifs.smb.SmbFileFilter;
import jcifs.smb.SmbFileInputStream;
import com.mediatek.mmpcm.fileimpl.FileConst;

/**
 * This Class find files on samba servers and provide 
 * playback, rendering and get input stream capabilites. 
 * 
 */
public class SambaManager {
	public static final int TYPE_UNKNOW = 0;
	public static final int TYPE_DIRECTORY = 1;
	public static final int TYPE_ALL = 2;
	public static final int TYPE_VIDEO = 3;
	public static final int TYPE_AUDIO = 4;
	public static final int TYPE_IMAGE = 5;
	public static final int TYPE_THRIMAGE = 6;
	public static final int TYPE_TEXT = 7;

	private static String TAG = "CM_SambaManager";
	private static String taglogin = "login";
	private static final boolean localLOGV = false;
	private static SambaManager mSambaManager;
	private static final String mRoot = "smb://";
	private final static String infoFile = "/data/misc/smb/smb_user_info.ini";

	private static NtlmPasswordAuthentication mUserAuth = null;
	private LinkedList<SmbFile> mListSmbFile = new LinkedList<SmbFile>();
	private Hashtable<Integer, SmbFileFilter> mFilter = new Hashtable<Integer, SmbFileFilter>();
	private static Hashtable<String, LoginInfo> mNamePWDTable = new Hashtable<String, LoginInfo>();

	static {
		mSambaManager = new SambaManager();
	}

	private SambaManager() {
		mFilter.put(new Integer(TYPE_ALL), new MultiMediaFileFilter());
		mFilter.put(new Integer(TYPE_VIDEO), new VideoFileFilter());
		mFilter.put(new Integer(TYPE_AUDIO), new AudioFileFilter());
		mFilter.put(new Integer(TYPE_IMAGE), new ImageFileFilter());
		mFilter.put(new Integer(TYPE_THRIMAGE), new ThrdPhotoFileFilter());
		mFilter.put(new Integer(TYPE_TEXT), new TextFileFilter());
	}

	/**
	 * Create a new SambaManager instance.
	 * Applications will use for Samba operation.
	 * 
	 */
	public static SambaManager getInstance() {
		return mSambaManager;
	}

	private static boolean isNeedFile(String name,String[] suffixs){
		boolean need = false;
		try{
			String tmpFileName = name.toLowerCase();

			for (String s : suffixs) {
				if (tmpFileName.endsWith(s)) {
					need = true;
					break;
				}
			}

		}catch(Exception ex){
			ex.printStackTrace();
			

		}

		NetLog.d(TAG, "isNeedFile need = "+ need +"name = "+ name);
        return need;



	}

	

	/**
	 * Inner class, use to filtering the non-multimedia files.
	 * 
	 */
	static class MultiMediaFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			if(localLOGV)NetLog.d(TAG, "in MultiMediaFileFilter");
			String filename = arg0.getName();
			if (filename.endsWith("/")) {
				filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in MultiMediaFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.audioSuffix) 
				||  isNeedFile(filename,FileConst.videoSuffix)
				||  isNeedFile(filename,FileConst.photoSuffix)
				||  isNeedFile(filename,FileConst.textSuffix)
				||  isNeedFile(filename,FileConst.thrdPhotoSuffix)) {
				if(localLOGV)NetLog.d(TAG, "in MultiMediaFileFilter return true");
				return true;
			}
			return false;
		}
	}

	/**
	 * Inner class, use to filtering the non-video files.
	 * 
	 */
	static class VideoFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			if(localLOGV)NetLog.d(TAG, "in VideoFileFilter");
			String filename = arg0.getName();
			if (filename.endsWith("/")) {
				filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in VideoFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.videoSuffix)) {
				if(localLOGV)NetLog.d(TAG, "in VideoFileFilter return true");
				return true;
			}
			return false;
		}
	}

	/**
	 * Inner class, use to filtering the non-audio files.
	 * 
	 */
	static class AudioFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			if(localLOGV)NetLog.d(TAG, "in AudioFileFilter");
			String filename = arg0.getName();
			if (filename.endsWith("/")) {
				filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in AudioFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.audioSuffix)) {
				if(localLOGV)NetLog.d(TAG, "in AudioFileFilter return true");
				return true;
			}
			return false;
		}
	}

	/**
	 * Inner class, use to filtering the non-image files.
	 * 
	 */
	static class ImageFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			if(localLOGV)NetLog.d(TAG, "in ImageFileFilter");
			String filename = arg0.getName();
			if (filename.endsWith("/")) {
					filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in ImageFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.photoSuffix)) {
				if(localLOGV)	NetLog.d(TAG, "in ImageFileFilter return true");
				return true;
			}
			return false;
		}
	}



		/**
	 * Inner class, use to filtering the 3d-image files.
	 * 
	 */
	static class ThrdPhotoFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			//if(localLOGV)NetLog.d(TAG, "in ThrdPhotoFileFilter");
			
			String filename = arg0.getName();
		
			if (filename.endsWith("/")) {
					filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in ThrdPhotoFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.thrdPhotoSuffix)) {
				if(localLOGV)	NetLog.d(TAG, "in ImageFileFilter return true");
				return true;
			}
			return false;
		}
	}

	/**
	 * Inner class, use to filtering the non-text files.
	 * 
	 */
	static class TextFileFilter implements SmbFileFilter {
		public boolean accept(SmbFile arg0) throws SmbException {
			// TODO Auto-generated method stub
			if(localLOGV)NetLog.d(TAG, "in TextFileFilter");
			String filename = arg0.getName();
			if (filename.endsWith("/")) {
					filename=filename.substring(0, filename.lastIndexOf('/'));//before action:filename.substring(0, filename.lastIndexOf('/'));
			}
			int type = arg0.getType();
			if(localLOGV)NetLog.d(TAG, "in TextFileFilter filename = "+ filename +"type = "+ type);
			if (type == SmbFile.TYPE_PRINTER || type == SmbFile.TYPE_NAMED_PIPE) {
				return false;
			}
			filename = filename.toLowerCase();
			if (arg0.isDirectory() || isNeedFile(filename,FileConst.textSuffix)) {
				if(localLOGV)	NetLog.d(TAG, "in TextFileFilter return true");
				return true;
			}
			return false;
		}
	}


	
	/**
	 * Write the user info into configure file.
	 * 
	 * @return
	 */
	public static void writeUserInfo() {
		try {
			SaveUserInfo.writeINIFile(infoFile, mNamePWDTable);
		} catch (Exception e) {
			return;
		}
	}

	private void readUserInfo() {
		try {
			SaveUserInfo.readINIFile(infoFile, mNamePWDTable);
		} catch (Exception e) {
			return;
		}
	}
	
	/**
	 * Return the user info which store in configure file.
	 * 
	 * @return the hash table of user info.
	 */
	public static Hashtable<String, LoginInfo> getHashTable() {
		return mNamePWDTable;
	} 
	
	/**
	 * Initial operations, init some attribute.
	 * Application need not to understand.
	 * 
	 * @return
	 */
	public void initSamba() {
		jcifs.Config.setProperty("jcifs.resolveOrder", "BCAST,LMHOSTS,WINS,DNS");
		jcifs.Config.setProperty("jcifs.encoding", "Cp1252");
		jcifs.Config.setProperty("jcifs.smb.lmCompatibility", "0");
		jcifs.Config.setProperty("jcifs.smb.client.responseTimeout", "30000");
		jcifs.Config.setProperty("jcifs.netbios.soTimeout", "30000");
		jcifs.Config.setProperty("jcifs.netbios.retryTimeout", "30000");
		jcifs.Config.setProperty("jcifs.netbios.cachePolicy", "24000");
		// jcifs.Config.setProperty("jcifs.smb.client.soTimeout","100");
		// jcifs.Config.setProperty("jcifs.smb.client.useExtenededSecurity","false");
		jcifs.Config.registerSmbURLHandler();
		readUserInfo();
	}

	/**
	 * Check if it only has one subdirectory or not. 
	 * example: 'smb://abc/' and 'smb://abc/def' will return true.
	 * 
	 * @param path the path want to be checked.
	 */
	private static boolean checkFirstLevel(String path) {
		if (path.startsWith(mRoot)) {
			String pathTmp = path.substring(mRoot.length());
			if (pathTmp == null || pathTmp.length() == 0) {
				return false;
			}
			pathTmp = pathTmp.substring(0, pathTmp.lastIndexOf('/'));
			if (pathTmp.contains("/") == false)
				return true;
		}
		return false;
	}

	/**
	 * Get the domin name according to the input parameter.
	 * 
	 * @param path the path will be parsed.
	 */
	private static String getDomainName(String path) {
		if (path.startsWith(mRoot)) {
			int rootLen = mRoot.length();
			if (path.length() > rootLen + 1) {
				String pathTmp = path.substring(mRoot.length());
				pathTmp = pathTmp.substring(0, pathTmp.indexOf("/"));
				return pathTmp;
			}
		}
		return null;
	}

	private boolean isValidFilterType(int type) {
		if (type != TYPE_ALL && type != TYPE_VIDEO && type != TYPE_AUDIO
				&& type != TYPE_IMAGE && type != TYPE_TEXT && type != TYPE_THRIMAGE)
			return false;
		return true;
	}

	/**
	 * Login the specific path.
	 * 
	 * @param path   the path which want to login.
	 * @param username  the user name.
	 * @param password  the password.
	 * @throws MalformedURLException 
	 * @throws SmbException
	 */
	public static void login(String path, String username, String password)
			throws MalformedURLException, SmbException, SmbAuthException {
		if(localLOGV)NetLog.d(taglogin, "[login3][path]: "+path+", [name]: "+username+", [pwd]: "+password);
		if (checkFirstLevel(path) == true) {
			if(localLOGV)	NetLog.d(taglogin,"[login3]: FirstLevel");
			SmbFile smbFile = new SmbFile(path);
			if (smbFile.getType() == SmbFile.TYPE_WORKGROUP) {
				String domainName = getDomainName(path);
				if (domainName != null) {
					NetLog.d(taglogin,"[login3]: FirstLevel, Domain login!");
					setAuthentication(new NtlmPasswordAuthentication(
							domainName, username, password));
					/**
					 * It need not to judge the if the hash table has login info, 
					 * because if login info is wrong, the new login info will replace 
					 * the old one.
					 * 
					 */
					/* if (mNamePWDTable.containsKey(path) == false) */
					mNamePWDTable.put(path, new LoginInfo(username, password));
					return;
				}
				else
				{
					if(localLOGV)	NetLog.d(taglogin, "[login3]: FirstLevel, Domain name NULL!");
				}
			}
		}
		if(localLOGV) NetLog.d(taglogin, "[login3]: Non-FirstLevel!!!");
		/**
		 * The path is not the first level directory, 
		 * it need to set the user name password.
		 * 
		 */
		setAuthentication(new NtlmPasswordAuthentication(null, username,
				password));
		/**
		 * It need not to judge the if the hash table has login info, 
		 * because if login info is wrong, the new login info will replace 
		 * the old one.
		 * 
		 */
		/* if (mNamePWDTable.containsKey(path) == false) */
		mNamePWDTable.put(path, new LoginInfo(username, password));
	}

	/**
	 * Login the specific path.
	 * 
	 * @param path   the path which want to login.
	 * @param username  the user name.
	 * @param password  the password.
	 * @throws MalformedURLException 
	 * @throws SmbException
	 */
	public static void login(String path, String domain, String username,
			String password) throws MalformedURLException, SmbException,
			SmbAuthException {
		if(localLOGV)NetLog.d(taglogin, "[login4][path]: "+path+", [domain]: "+domain+", [name]: "+username+", [pwd]: "+password);
		if ((domain == null) && (username == null) && (password == null)) {
			if(localLOGV)	NetLog.d(taglogin, "[login4]: All 3 filed are NULL");
			setAuthentication(new NtlmPasswordAuthentication(username));
//			mNamePWDTable.put(path, new LoginInfo("", ""));
			return;
		}

		/**
		 * The path is not the first level directory, 
		 * it need to set the user name password.
		 * 
		 */
		if(localLOGV)NetLog.d(taglogin, "[login4][domain]: "+domain+" [name]: "+username+" [pwd]: "+password);
		setAuthentication(new NtlmPasswordAuthentication(domain, username,
				password));
		/**
		 * It need not to judge the if the hash table has login info, 
		 * because if login info is wrong, the new login info will replace 
		 * the old one.
		 * 
		 */
		/* if (mNamePWDTable.containsKey(path) == false) */
		mNamePWDTable.put(path, new LoginInfo(username, password));
		return;
	}

	/**
	 * Get the file list of the input path
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public LinkedList<String> getSmbFileList(String path)
			throws MalformedURLException, SmbException, SmbAuthException,
			UnknownHostException {
		LinkedList<String> listSmbFilePath = new LinkedList<String>();
		if (path == null) {
			path = mRoot;
		}
		if(localLOGV)NetLog.d(taglogin, "[getSmbFileList1][path]: "+path);
		SmbFile smbFile = getSmbFileIns(path);
		switch (smbFile.getType()) {
		case SmbFile.TYPE_FILESYSTEM:
			NetLog.d(TAG, path + " :[TYPE_FILESYSTEM]");
			break;
		case SmbFile.TYPE_WORKGROUP:
			NetLog.d(TAG, path + " :[TYPE_WORKGROUP]");
			break;
		case SmbFile.TYPE_SERVER:
			NetLog.d(TAG, path + " :[TYPE_SERVER]");
			break;
		case SmbFile.TYPE_SHARE:
			NetLog.d(TAG, path + " :[TYPE_SHARE]");
			break;
		case SmbFile.TYPE_NAMED_PIPE:
			NetLog.d(TAG, path + " :[TYPE_NAMEDPIPE]");
			break;
		case SmbFile.TYPE_PRINTER:
			NetLog.d(TAG, path + " :[TYPE_PRINTER]");
			break;
		case SmbFile.TYPE_COMM:
			NetLog.d(TAG, path + " :[TYPE_COMM]");
			break;
		}
		SmbFile[] fList = smbFile.listFiles();
		for (int i = 0; i < fList.length; i++) {
			/* filter the format 'D$/' */
			if (fList[i].getName().endsWith("$/"))
				continue;
			mListSmbFile.add(fList[i]);
			String filePath = fList[i].getPath();
			listSmbFilePath.add(filePath);
		}
		return listSmbFilePath;
	}

	/**
	 * Returns this IP address as a String in the form "%d.%d.%d.%d".
	 * 
	 * @param path
	 *            the path to be parsed, such as 'smb://test/'.
	 * @return the IP address of the host.
	 */
	public String getHostIpAddress(String path) {
		if (path == null) {
			return null;
		}

		NbtAddress address = null;
		String hostName = getDomainName(path);
		NetLog.d(TAG, "[getHostIpAddress]:hostName ->" + hostName);
		if (hostName == null) {
			return null;
		}

		try {
			address = NbtAddress.getByName(hostName);
		} catch (Exception e) {
			NetLog.d(TAG, "[getHostIpAddress]:Exception");
			return null;
		}

		return address.getHostAddress();
	}

	/**
	 * Get the file list of the input path.
	 * 
	 * @param path
	 *            The path waiting to be parsed.
	 * @param filterType
	 *            Tile type filter.
	 * @return The name list including path name of the files under the input
	 *         path.
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public LinkedList<String> getSmbFileList(String path, int filterType)
			throws MalformedURLException, SmbException, SmbAuthException,
			UnknownHostException {
		LinkedList<String> listSmbFilePath = new LinkedList<String>();
		if (path == null) {
			path = mRoot;
		}
		if(localLOGV)NetLog.d(taglogin, "[getSmbFileList2][path]: "+path);
		SmbFile smbFile = getSmbFileIns(path);
		switch (smbFile.getType()) {
		case SmbFile.TYPE_FILESYSTEM:
			NetLog.d(TAG, path + " :[TYPE_FILESYSTEM]");
			break;
		case SmbFile.TYPE_WORKGROUP:
			NetLog.d(TAG, path + " :[TYPE_WORKGROUP]");
			break;
		case SmbFile.TYPE_SERVER:
			NetLog.d(TAG, path + " :[TYPE_SERVER]");
			break;
		case SmbFile.TYPE_SHARE:
			NetLog.d(TAG, path + " :[TYPE_SHARE]");
			break;
		case SmbFile.TYPE_NAMED_PIPE:
			NetLog.d(TAG, path + " :[TYPE_NAMEDPIPE]");
			break;
		case SmbFile.TYPE_PRINTER:
			NetLog.d(TAG, path + " :[TYPE_PRINTER]");
			break;
		case SmbFile.TYPE_COMM:
			NetLog.d(TAG, path + " :[TYPE_COMM]");
			break;
		}
		if (isValidFilterType(filterType) == false)
			filterType = 0;
		SmbFile[] fList = smbFile.listFiles(mFilter
				.get(new Integer(filterType)));
		for (int i = 0; i < fList.length; i++) {

			/* filter the format 'D$/' */
			if (fList[i].getName().endsWith("$/"))
				continue;
			mListSmbFile.add(fList[i]);
			String filePath = fList[i].getPath();
			listSmbFilePath.add(filePath);
		}
		return listSmbFilePath;
	}

	/**
	 * Get the input stream of the input file, it will be played by
	 * media player.
	 * 
	 * @param path the path which want to get.
	 * @return the object of input stream.
	 * @throws MalformedURLException
	 * @throws UnknownHostException
	 * @throws SmbException
	 */
	public InputStream getFileStream(String path) throws MalformedURLException,
			SmbException, UnknownHostException {
		SmbFileInputStream in;
		SmbFile smbFile = getSmbFileIns(path);
		in = new SmbFileInputStream(smbFile);
		NetLog.v("The file url is ", path);
		return in;
	}

	/**
	 * Get the data source of the input file.
	 * Through call newInputStream () method according to SambaDataSource, 
	 * it can get the stream for streaming media player.
	 * 
	 * @param path the path which to get.
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public SambaDataSource getSambaDataSource(String path)
			throws MalformedURLException, SmbException, UnknownHostException {
		SmbFile smbFile = getSmbFileIns(path);
		return new SambaDataSource(smbFile);
	}

	/**
	 * Get type of the input file.
	 * 
	 * @param path
	 * @return SambaManger.TYPE_DIRECTORY SambaManger.TYPE_VIDEO
	 *         SambaManger.TYPE_AUDIO SambaManger.TYPE_IMAGE
	 *         SambaManger.TYPE_TEXT
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public int getType(String path) throws MalformedURLException, SmbException,
			UnknownHostException {
		if (isDir(path) == true)
			return TYPE_DIRECTORY;
		String filename = getFileName(path).toLowerCase();
		if (filename.endsWith(".3gp") || filename.endsWith(".mp4")
				|| filename.endsWith(".ts") || filename.endsWith(".webm")
				|| filename.endsWith(".avi") || filename.endsWith(".wmv")
				|| filename.endsWith(".asf") || filename.endsWith(".mov")
				|| filename.endsWith(".mkv") || filename.endsWith(".mpg")
				|| filename.endsWith(".mpeg") || filename.endsWith(".vro")
				|| filename.endsWith(".rm") || filename.endsWith(".ogm")
				|| filename.endsWith(".flv"))
			return TYPE_VIDEO;
		else if (filename.endsWith(".3gp") || filename.endsWith(".mp4")
				|| filename.endsWith(".m4a") || filename.endsWith(".aac")
				|| filename.endsWith(".flac") || filename.endsWith(".mp3")
				|| filename.endsWith(".mid") || filename.endsWith(".xmf")
				|| filename.endsWith(".mxmf") || filename.endsWith(".rtttl")
				|| filename.endsWith(".rtx") || filename.endsWith(".ota")
				|| filename.endsWith(".ogg") || filename.endsWith(".wav")
				|| filename.endsWith(".wma") || filename.endsWith(".vob")
				|| filename.endsWith(".avi") || filename.endsWith(".wmv")
				|| filename.endsWith(".asf") || filename.endsWith(".mpg")
				|| filename.endsWith(".mpeg") || filename.endsWith(".vro")
				|| filename.endsWith(".ts") || filename.endsWith(".rm")
				|| filename.endsWith(".mkv") || filename.endsWith(".flv"))
			return TYPE_AUDIO;
		else if (filename.endsWith(".jpg") || filename.endsWith(".gif")
				|| filename.endsWith(".png") || filename.endsWith(".bmp"))
			return TYPE_IMAGE;
		else if (filename.endsWith(".txt"))
			return TYPE_TEXT;
		return TYPE_UNKNOW;
	}

	/**
	 * Tests to see if the directory is empty.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public boolean isEmpty(String path) throws MalformedURLException,
			SmbException, UnknownHostException {
		boolean flag = false;
		SmbFile smbFile = getSmbFileIns(path);
		if (smbFile.canRead()) {
			SmbFile[] fList = smbFile.listFiles();
			if (fList.length == 0) {
				flag = true;
			}
		}
		return flag;
	}

	private String longToString(long time) {
		Date date = new Date(time);
		return date.toString();

	}

	/**
	 * Get the file's create time.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public String getCreateTime(String path) throws SmbAuthException,
			MalformedURLException, SmbException, UnknownHostException {
		SmbFile smbFile = getSmbFileIns(path);
		long createTime = smbFile.createTime();
		NetLog.d(TAG, "path: " + path + "createTime: " + createTime);
		return longToString(createTime);
	}

	/**
	 * Tests to see if the file is a directory.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 */
	public boolean isDir(String path) throws MalformedURLException,
			SmbException, UnknownHostException {
		//boolean flag = false;
		SmbFile smbFile = getSmbFileIns(path);
		//flag = smbFile.isDirectory();
		return smbFile.isDirectory();//flag;
	}

	/**
	 * Tests to see if the file is not a directory.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 * @throws java.lang.Exception
	 */
	public boolean isFile(String path) throws MalformedURLException,
			SmbException, UnknownHostException {
		//boolean flag = false;
		SmbFile smbFile = getSmbFileIns(path);
		//flag = smbFile.isFile();
		return smbFile.isFile();//flag;
	}

	/**
	 * Tests to see if the file this SmbFile represents can be read.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 * @throws java.lang.Exception
	 */
	public boolean isCanRead(String path) throws MalformedURLException,
			SmbException, UnknownHostException {
		SmbFile smbFile = getSmbFileIns(path);
		return smbFile.canRead();
	}

	/**
	 * Get the length of this SmbFile in bytes.
	 * 
	 * @param path
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws UnknownHostException
	 * @throws java.lang.Exception
	 */
	public long size(String path) throws MalformedURLException, SmbException,
			UnknownHostException {
	//	long len = 0;
		SmbFile smbFile = getSmbFileIns(path);
		//len = smbFile.length();
		return smbFile.length();//len;
	}

	/**
	 * Get the file name from the input path string.
	 * 
	 * @param path
	 * @return
	 * @throws java.lang.Exception
	 */
	public String getFileName(String path) {
		String name;
		if (path.endsWith("/")) {
			name = path.substring(0, path.lastIndexOf('/'));
			name = name.substring(name.lastIndexOf('/') + 1);
		} else
			name = path.substring(path.lastIndexOf('/') + 1);
		return name;
	}

	/**
	 * @param authentication
	 *            the authentication to set.
	 */
	private static void setAuthentication(
			NtlmPasswordAuthentication authentication) {
		SambaManager.mUserAuth = authentication;
		if(localLOGV)NetLog.d(taglogin,
				"-----[setAuthentication][domain]: " + authentication.getDomain()
						+ " [name]: " + authentication.getName() + " [pwd]: "
						+ authentication.getPassword());
	}
	private LoginInfo getServerInfo(String path){
		if(path==null || mNamePWDTable.keySet().size()==0){
			return null;
		}
		if(mNamePWDTable.containsKey(path) == true){
			return mNamePWDTable.get(path);
		}
		for(String keyPath:mNamePWDTable.keySet()){
			if(keyPath.equals("smb://")){
				continue;
			}else if(path.startsWith(keyPath)){
				return mNamePWDTable.get(keyPath);
			}
		}
		return null;
	}

	/**
	 * Create a SmbFile instance from a path.
	 * This operation may has some process as follows:
	 * 1. First, find the specified path of loginInfo from the HashTable.
	 *    if hash table has logInfo, use the loginInfo to update 
	 *    SambaManager private member 'mUserAuth'.
	 * 2. Then, try create a SmbFile object, if mUserAuth non-empty, 
	 *    input the SmbFile constructor, or just pass path.  
	 * 
	 * @param path the path 
	 * @return
	 * @throws MalformedURLException
	 * @throws SmbException
	 * @throws Exception
	 * @throws java.lang.Exception
	 */
	private SmbFile getSmbFileIns(String path) throws MalformedURLException,
			SmbException, SmbAuthException, UnknownHostException {
		SmbFile smbFile;
	
		if(localLOGV)NetLog.v(taglogin, "[getSmbFileIns] new SMB file : " + path);
		LoginInfo login=getServerInfo(path);
		
		if ( login!= null) {
			if (localLOGV)
				NetLog.d(taglogin,
						"[getSmbFileIns]: mNamePWDTable.containsKey(path) ");
			//LoginInfo login = mNamePWDTable.get(serverPath);
			SambaManager.login(path, login.getUserName(), login.getPasswd());
		}else{
			mUserAuth = null;
		}
	/*	
		if(checkFirstLevel(path)==true && getDomainName(path)!=null)
		{
			//smbFile = new SmbFile(path, new NtlmPasswordAuthentication(getDomainName(path), "", ""));
			//NetLog.d(taglogin, "[getSmbFileIns][Domain]: "+ getDomainName(path) +", new SmbFile(path, new NtlmPasswordAuthentication(getDomainName(path), null, null)");
			smbFile = new SmbFile(path, mUserAuth);
			NetLog.d(taglogin, "[getSmbFileIns]: 1--new SmbFile(path, mUserAuth)");
			if(mUserAuth!=null)
				NetLog.d(taglogin, "[getSmbFileIns]: 1--domain: "+mUserAuth.getDomain()+", name: "+mUserAuth.getName()+", pwd: "+mUserAuth.getPassword());
			return smbFile; 
		}
	*/
		// NetLog.v(TAG, "new SMB file : " + path);
		// NetLog.v(TAG, "new SMB auth : " + mUserAuth);
		if (mUserAuth != null) {
			smbFile = new SmbFile(path, mUserAuth);
			if(localLOGV)	NetLog.d(taglogin,
					"-----[getSmbFileIns][domain]: " + mUserAuth.getDomain()
							+ " [name]: " + mUserAuth.getName() + " [pwd]: "
							+ mUserAuth.getPassword());
		} else {
			smbFile = new SmbFile(path, NtlmPasswordAuthentication.ANONYMOUS);
			if(localLOGV)NetLog.d(taglogin, "[getSmbFileIns]: new SmbFile(path, NtlmPasswordAuthentication.ANONYMOUS)");
		}
		return smbFile;
	}

	/**
	 * Sort the file list by name.
	 * 
	 * @param linkedList the list will be sorted.
	 * @return the file list which has been sorted.
	 * 
	 */
	public LinkedList<String> sortByName(LinkedList<String> linkedList) {
		Collections.sort(linkedList, new Comparator<String>() {
			public int compare(String object1, String object2) {
				return object1.compareTo(object2);
			}
		});
		return linkedList;
	}
}
