package com.mediatek.mmpcm.threedimen.photoimpl;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;

public class FileInfo extends Info{
	static private FileInfo mInfo;
	public static FileInfo getInstance() {
		if (mInfo == null) {
			synchronized (FileInfo.class) {
				if (mInfo == null) {
					mInfo = new FileInfo();
				}
			}
		}
		return mInfo;
	}
	
	public MetaData getMetaDataInfo(){
		return null;
	}
	
}
