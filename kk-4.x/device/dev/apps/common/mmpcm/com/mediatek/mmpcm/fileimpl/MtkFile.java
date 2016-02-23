package com.mediatek.mmpcm.fileimpl;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URI;
import java.sql.Time;

import android.content.Context;
import android.graphics.Bitmap;

public class MtkFile extends File {

    private boolean isMarked = false;
	private int mSrcType = -1;

    public boolean getMark() {
        return isMarked;
    }

    public void setMark(boolean mark) {
        isMarked = mark;
    }

    public MtkFile(File f) {
        super(f.getPath());
        // super(f.getAbsolutePath());
    }

    public MtkFile(URI uri) {
        super(uri);
        // TODO Auto-generated constructor stub
    }

    public MtkFile(String dirPath, String name) {
        super(dirPath, name);
        // TODO Auto-generated constructor stub
    }

    public MtkFile(String path) {
        super(path);
        // TODO Auto-generated constructor stub
    }

    public MtkFile(File dir, String name) {
        super(dir, name);
        // TODO Auto-generated constructor stub
    }
    /**
     * filter file by set mode.
     * @param mode
     * @return
     */
    private boolean filterFile(String []mode){
    	if (mode == null){
    		return false;
    	}
        for (String s : mode) {
            if (this.getName().toLowerCase().endsWith(s)) {
                return true;
            }
        }
        return false;
    }
    
    public boolean isPhotoFile() {

        return  filterFile(FileConst.photoSuffix);
    }
	 
    public int getPhotoSuffix(){
    	int i = 0;
        for (String s : FileConst.photoSuffix) {        	
            if (this.getName().toLowerCase().endsWith(s)) {
                return i;
            }
            i++;
        }
        return -1;
    }
    
    public boolean isThrdPhotoFile(){
    	
    	return filterFile(FileConst.thrdPhotoSuffix);
    }
    
	public boolean isIsoFile() {
		if (this.getName().toLowerCase().endsWith("iso")) {
			return true;
		}
		return false;
	}
		
	public boolean isIsoVideoFile() {
		if (this.getName().toLowerCase().endsWith(".ssif")) {
			return true;
		}
		return false;
	}
		

    public boolean isTextFile() {

        return filterFile(FileConst.textSuffix);
    }

    public boolean isAudioFile() {

        return filterFile(FileConst.audioSuffix);
    }

    public boolean isVideoFile() {

        return filterFile(FileConst.videoSuffix);
    }

    public String getLastModified() {
        return new Time(lastModified()).toString();
    }

    public static void openFileInfo(Context context) {

    }

    public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {
        return null;
    }
    
    public void stopThumbnail(){
    	
    }

    public String getResolution() {
    	return null;
    }
    
    public long getFileSize() {
        long lSize = 0;
        if (isDirectory()) {
            String cmd = "du -s " + this.getPath();

            Process pid = null;
            try {
                pid = Runtime.getRuntime().exec(cmd);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            if(pid == null) {
            	return lSize;
            }
            InputStream  ips = pid.getInputStream();
            BufferedReader bufferedReader = new BufferedReader(
                    new InputStreamReader(ips));

            try{
	            try {
	                pid.waitFor();
	            } catch (InterruptedException e) {
	                // TODO Auto-generated catch block
	                e.printStackTrace();
	            }
	
	            try {
	                String line = bufferedReader.readLine();
	                if (line == null) {
	                    lSize = 0;
	                } else {
	                    line = line.split("\t")[0];
	                    if (line.length() == 0) {
	                        lSize = 0;
	                    } else {
	                        lSize = Long.parseLong(line);
	                        lSize *= 1024;
	                    }
	                }
	                ips.close();
	            } catch (IOException e) {
	                // TODO Auto-generated catch block
	                e.printStackTrace();
	            }
            } catch (Exception e) {
            	e.printStackTrace();
            } finally {
            	try {
					ips.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
            }
        } else {
//            String a = this.getAbsolutePath();
            lSize = length();
        }

        return lSize;
    }
    
    public String getSize() {
        long ls = getFileSize();
        
        if (ls / 1024 / 1024 != 0) {
            return getMegaSize(ls);
        } else if (ls / 1024 != 0) {
            return getKiloSize(ls);
        } else
            return ls + " B";
    }

    public String getMegaSize(long lSize) {
    	long tempSizeInt = 0;
    	long tempSizeRemainder = 0;
    	tempSizeInt = lSize / 1024 / 1024;
    	tempSizeRemainder = lSize/1024 % 1024;    	
    	return tempSizeInt + "." + tempSizeRemainder + "MB";
    	
    	
        //return lSize / 1024 / 1024 + "MB";
       
    }

    public String getKiloSize(long lSize) {
    	long tempSizeInt = 0;
    	long tempSizeRemainder = 0;
    	tempSizeInt = lSize / 1024;
    	tempSizeRemainder = lSize % 1024;    	
    	return tempSizeInt + "." + tempSizeRemainder + "KB";
        //return lSize / 1024 + "KB";
    }

    // String getTest() {
    // StringBuffer buffer = new StringBuffer();
    // if (this.isDirectory()) {
    // buffer.append("Diretory: ");
    // } else {
    // buffer.append("Unknow: ");
    // }
    // return buffer.append(getName()).append(" ").append(getKiloSize())
    // .append(" ").append(getLastModified()).toString();
    // }

}
