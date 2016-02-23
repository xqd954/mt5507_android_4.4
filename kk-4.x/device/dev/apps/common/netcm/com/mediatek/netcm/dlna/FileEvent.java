package com.mediatek.netcm.dlna;

import java.util.LinkedList;

import com.mediatek.dlna.FoundDeviceEvent;
import com.mediatek.dlna.LeftDeviceEvent;
import com.mediatek.dlna.object.Content;
import com.mediatek.dlna.object.ContentType;
import com.mediatek.dlna.object.DLNADevice;
import com.mediatek.dlna.object.FailedContentEvent;
import com.mediatek.dlna.object.NormalContentEvent;
import com.mediatek.netcm.util.NetLog;


/**
 * Auxiliary class for DLNA manager.
 * 
 */
public class FileEvent {
	public static final int FILTER_TYPE_ALL = 0;
	public static final int FILTER_TYPE_VIDEO = 1;
	public static final int FILTER_TYPE_AUDIO = 2;
	public static final int FILTER_TYPE_IMAGE = 3;
	public static final int FILTER_TYPE_TEXT = 4;

	private String TAG = "CM_FileEvent";
	// public static final int EVENT_TYPE_UNKNOW = 0;
	// public static final int EVENT_TYPE_FOUND_DEVICE = 1;
	// public static final int EVENT_TYPE_LEFT_DEVICE = 2;
	// public static final int EVENT_TYPE_NORMAL_CONTENT = 3;
	// public static final int EVENT_TYPE_FAILED_CONTENT = 4;

	// private int mFileType = EVENT_TYPE_UNKNOW;
	private FoundDeviceEvent mFileFoundEvent;
	private LeftDeviceEvent mFileLeftEvent;
	private NormalContentEvent mFileNormalEvent;
	private FailedContentEvent mFileFailEvent;
	private LinkedList<DLNAFile> mFileNameList = null;
	private LinkedList<DLNAFile> mContentList = null;

	public FileEvent(FoundDeviceEvent fileFoundEvent,
			LinkedList<DLNAFile> nameList) {
		NetLog.d(TAG, "new FoundDeviceEvent");
		this.mFileFoundEvent = fileFoundEvent;
		// this.mFileType = EVENT_TYPE_FOUND_DEVICE;
		this.mFileNameList = new LinkedList<DLNAFile>();
		this.mFileNameList = nameList;
	}

	public FileEvent(LeftDeviceEvent fileLeftEvent,
			LinkedList<DLNAFile> nameList) {
		NetLog.d(TAG, "new LeftDeviceEvent");
		this.mFileLeftEvent = fileLeftEvent;
		// this.mFileType = EVENT_TYPE_LEFT_DEVICE;
		this.mFileNameList = new LinkedList<DLNAFile>();
		this.mFileNameList = nameList;
	}

	public FileEvent(NormalContentEvent fileNormalEvent,
			LinkedList<DLNAFile> contentList) {
		NetLog.d(TAG, "new NormalContentEvent");
		this.mFileNormalEvent = fileNormalEvent;
		// this.mFileType = EVENT_TYPE_NORMAL_CONTENT;
		this.mFileNameList = new LinkedList<DLNAFile>();
		this.mContentList = contentList;
		// for (int i = 0; i < fileNormalEvent.getList().size(); i++){
		// mFileNameList.add(fileNormalEvent.getList().get(i).getTitle());
		// }
		for (int i = 0; i < contentList.size(); i++) {
			this.mFileNameList.add(contentList.get(i));
		}
	}

	public FileEvent(FailedContentEvent fileFailEvent) {
		NetLog.d(TAG, "new FailedContentEvent");
		this.mFileFailEvent = fileFailEvent;
		// this.mFileType = EVENT_TYPE_FAILED_CONTENT;
	}

	// public int getEventType(){
	// return this.mFileType;
	// }

	// public DLNAEvent getEvent(int type){
	// switch(type){
	// case EVENT_TYPE_UNKNOW:
	// return null;
	// case EVENT_TYPE_FOUND_DEVICE:
	// return mFileFoundEvent;
	// case EVENT_TYPE_LEFT_DEVICE:
	// return mFileLeftEvent;
	// case EVENT_TYPE_NORMAL_CONTENT:
	// return mFileNormalEvent;
	// case EVENT_TYPE_FAILED_CONTENT:
	// return mFileFailEvent;
	// default:
	// break;
	// }
	// return null;
	// }

	
	/**
	 * Get the file list of current path.
	 * 
	 * @return the file list of current path.
	 */
	public LinkedList<DLNAFile> getFileList() {
		NetLog.d(TAG, "getFileList");
		return this.mFileNameList;
	}

	private void multiMediaFilter() {
		NetLog.d(TAG, "multiMediaFilter");
		this.mFileNameList.clear();
		for (int i = 0; i < mContentList.size(); i++) {
			Content cnt = mContentList.get(i).getContent();
			ContentType type = cnt.getType();
			if (cnt.isDirectory() || type == ContentType.Video
					|| type == ContentType.Audio || type == ContentType.Photo
					|| type == ContentType.Playlist)
				mFileNameList.add(mContentList.get(i));
		}
	}

	private void videoFilter() {
		NetLog.d(TAG, "videoFilter");
		this.mFileNameList.clear();
		for (int i = 0; i < mContentList.size(); i++) {
			DLNAFile dlnaFile = mContentList.get(i);
			if( dlnaFile!= null ){
				Content cnt = dlnaFile.getContent();
				if (cnt.isDirectory() || cnt.getType() == ContentType.Video)
					mFileNameList.add(dlnaFile);
			}
		}
	}

	private void audioFilter() {
		NetLog.d(TAG, "audioFilter");
		this.mFileNameList.clear();
		for (int i = 0; i < mContentList.size(); i++) {
			Content cnt = mContentList.get(i).getContent();
			if (cnt.isDirectory() || cnt.getType() == ContentType.Audio)
				mFileNameList.add(mContentList.get(i));
		}
	}

	private void imageFilter() {
		NetLog.d(TAG, "imageFilter");
		this.mFileNameList.clear();
		for (int i = 0; i < mContentList.size(); i++) {
			Content cnt = mContentList.get(i).getContent();
			if (cnt.isDirectory() || cnt.getType() == ContentType.Photo)
				mFileNameList.add(mContentList.get(i));
		}
	}

	private void textFilter() {
		NetLog.d(TAG, "textFilter");
		this.mFileNameList.clear();
		for (int i = 0; i < mContentList.size(); i++) {
			Content cnt = mContentList.get(i).getContent();
			if (cnt.isDirectory() || cnt.getType() == ContentType.Playlist)
				mFileNameList.add(mContentList.get(i));
		}
	}

	/**
	 * Get the file list of current path, include a filter.
	 * 
	 * @param filterType  the type of filter.
	 * @return the file list of current path, filter by parameter.
	 */
	public LinkedList<DLNAFile> getFileList(int filterType) {
		NetLog.d(TAG, "getFileList & filter");
		if (mContentList != null) {
			switch (filterType) {
			case FILTER_TYPE_VIDEO:
				videoFilter();
				break;
			case FILTER_TYPE_AUDIO:
				audioFilter();
				break;
			case FILTER_TYPE_IMAGE:
				imageFilter();
				break;
			case FILTER_TYPE_TEXT:
				textFilter();
				break;
			case FILTER_TYPE_ALL:
			default:
				multiMediaFilter();
				break;
			}
		}
		
		return this.mFileNameList;
	}
	
	public DLNADevice getLeftDevice(){
		if (mFileLeftEvent == null){
			return null;
		}
		
		return mFileLeftEvent.getDevice();
	}
	
}