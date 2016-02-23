package com.mediatek.mmpcm.fileimpl;

import java.io.File;
import java.net.URI;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.audioimpl.AudioInfo;
import com.mediatek.mmpcm.audioimpl.CorverPic;

public class AudioFile extends MtkFile {

    /**
	 * 
	 */
    private static final long serialVersionUID = 1423424L;

    static private AudioInfo info = null;
    static private CorverPic vCorver = null;

    public static void openFileInfo(Context context) {
        info = AudioInfo.getInstance();
        vCorver = CorverPic.getInstance();
    }

    public AudioFile(MtkFile f) {
        super(f.getPath());
    }

    public AudioFile(URI uri) {
        super(uri);
    }

    public AudioFile(String dirPath, String name) {
        super(dirPath, name);
    }

    public AudioFile(String path) {
        super(path);
    }

    public AudioFile(File dir, String name) {
        super(dir, name);
    }
    
    @Override
    public void stopThumbnail() {
    	MmpTool.LOG_INFO("stopThumbnail");
    	if(null!=vCorver)
    	{
    		MmpTool.LOG_INFO("stopThumbnail  run-----");
    		vCorver.stopThumbnail();
    	}
    }
    
    @Override
    public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {

        if (vCorver != null){
            Bitmap bp = null;
            
            bp = vCorver.getAudioCorverPic(FileConst.SRC_USB, this.getAbsolutePath(), width, height);
            
            return bp;  
        }
        
        return null;
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
}
