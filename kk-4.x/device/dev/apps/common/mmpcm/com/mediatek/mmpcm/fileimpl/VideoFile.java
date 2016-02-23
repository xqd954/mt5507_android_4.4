package com.mediatek.mmpcm.fileimpl;

import java.io.File;
import java.net.URI;

import android.content.Context;
import android.graphics.Bitmap;

import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.videoimpl.VideoInfo;
import com.mediatek.mmpcm.videoimpl.Thumbnail;

public class VideoFile extends MtkFile {
    static private VideoInfo info = null;
    static private Thumbnail vThumb = null;
    private static Context mContext ;

    /**
	 * 
	 */
    private static final long serialVersionUID = 1L;

    public static void openFileInfo(Context context) {
        info = VideoInfo.getInstance();
        vThumb = Thumbnail.getInstance();
        mContext = context;
    }

    public VideoFile(MtkFile f) {
        super(f.getPath());
    }

    public VideoFile(URI uri) {
        super(uri);
    }

    public VideoFile(String dirPath, String name) {
        super(dirPath, name);
    }

    public VideoFile(String path) {
        super(path);
    }

    public VideoFile(File dir, String name) {
        super(dir, name);
    }

    @Override
    public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {
        if (vThumb != null) {
            Bitmap bp = null;
            bp =  vThumb.getVideoThumbnail(FileConst.SRC_USB, this.getAbsolutePath(), width, height);
            return bp;
        }
        return null;
    }

    public void stopThumbnail(){
        if (vThumb == null)
            return;
        
        vThumb.stopThumbnail();
    }
    
    public MetaData getMetaDataInfo(){
        if (null == info)
            return null;
        
        return info.getMetaDataInfo(this.getAbsolutePath(),FileConst.SRC_USB);
    }   

    public void stopMetaDataInfo(){
    	if (null == info){
    		return;
    	}
    	
    	info.stopMetaData();
    }

    public Bitmap[] getMultiSpecificThumbnail(URI uri[]) {
        return null;
    }

    public Bitmap getFrameThumbnail(int frame) {
        return null;
    }

}
