package com.mediatek.ui.mmp.gamekit.util;

import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.ui.mmp.gamekit.content.MutilMediaConst;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.FilesManager;

public class FileUtil {

	/**
	 * get type by the name
	 * 
	 * @param name
	 * @return
	 */
	static public int getTypeByName(String name) {
		if (null == name || "".equals(name)) {
			return MutilMediaConst.CONTENT_UNKNOW;
		}
		name = name.toLowerCase();

		for (String s : FileConst.photoSuffix) {
			if (name.endsWith(s)) {
				return MutilMediaConst.CONTENT_PHOTO;
			}
		}
		
		for (String s: FileConst.thrdPhotoSuffix){
			if (name.endsWith(s)){
				return MutilMediaConst.CONTENT_THRDPHOTO;
			}
		}

		for (String s : FileConst.audioSuffix) {
			if (name.endsWith(s)) {
				return MutilMediaConst.CONTENT_AUDIO;
			}
		}

		for (String s : FileConst.videoSuffix) {
			if (name.endsWith(s)) {
				return MutilMediaConst.CONTENT_VIDEO;
			}
		}

		for (String s : FileConst.textSuffix) {
			if (name.endsWith(s)) {
				return MutilMediaConst.CONTENT_TEXT;
			}
		}

		return MutilMediaConst.CONTENT_UNKNOW;
	}

	private static String separator = "/";

	/**
	 * Returns the name of the file or directory represented by this file.
	 * 
	 * @return this file's name or an empty string if there is no name part in
	 *         the file's path.
	 */
	static public String getName(String path) {
		int separatorIndex = path.lastIndexOf(separator);
		return (separatorIndex < 0) ? path : path.substring(separatorIndex + 1,
				path.length());
	}

	static public int getTypeByFile(FileAdapter fAdapter){
		int type = MutilMediaConst.CONTENT_UNKNOW;
		if (fAdapter == null) return type;
		
		if (fAdapter.isVideoFile()) {
			type = MutilMediaConst.CONTENT_VIDEO;
		}
		else if(fAdapter.isAudioFile()){
			type = MutilMediaConst.CONTENT_AUDIO;
		}
		else if(fAdapter.isPhotoFile()){
			type = MutilMediaConst.CONTENT_PHOTO;
		}
		else if(fAdapter.isThrdPhotoFile()){
			type = MutilMediaConst.CONTENT_THRDPHOTO;
		}
		else if(fAdapter.isTextFile()){
			type = MutilMediaConst.CONTENT_TEXT;
		}
		
		return type;
	}
}
