package com.mediatek.ui.mmp.model;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Time;

import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.mmpcm.fileimpl.FileConst;

import com.mediatek.ui.mmp.util.TextUtils;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.media.ThumbnailUtils;
import android.util.Log;

public abstract class FileAdapter {
	private static final String TAG = "FileAdapter";

	public static final int SMALL = 1;
	public static final int MEDIUM = 2;
	public static final int LARGE = 3;

	public static final int FILE_TYPE_PHOTO = 0;
	public static final int FILE_TYPE_VIDEO = 1;
	public static final int FILE_TYPE_AUDIO = 2;
	public static final int FILE_TYPE_TEXT = 3;

	private int mWidth = -1;
	private int mHeight = -1;

	private BitmapFactory.Options mBitmapOptions;

	
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

	public boolean isValidSizePhoto(){
		long size = length();
		if(size > 0 && size < FileConst.MAX_PHOTO_SIZE){
			return true;
		}
		return true;
	}
	
    public boolean isThrdPhotoFile(){
    	
    	return filterFile(FileConst.thrdPhotoSuffix);
    }
    
	public boolean isPhotoFile() {

		return filterFile(FileConst.photoSuffix);
	}

	public boolean isTextFile() {

		return filterFile(FileConst.textSuffix);
	}

	public boolean isDevice() {
		return false;
	}

	public boolean isAudioFile() {

		return filterFile(FileConst.audioSuffix);
	}

	public boolean isVideoFile() {

		return filterFile(FileConst.videoSuffix);
	}
	
	public boolean isIsoFile() {
		if (this.getName().toLowerCase().endsWith("iso")) {
			return true;
		}
		return false;
	}

	public String getLastModified() {
		return new Time(this.lastModified()).toString();
	}

	protected String getSize(long length) {
		if(length <= 0){
			return "";
		}
		
		if (length / 1024 / 1024 != 0) {
			return getMegaSize(length);
		} else if (length / 1024 != 0) {
			return getKiloSize(length);
		} else
			return length + " B";
	};

	protected String getTextSize(long length) {
		return length + " Byte";
	}

	protected String getMegaSize(long lSize) {
		return lSize / 1024 / 1024 + "MB";
	}

	protected String getKiloSize(long lSize) {
		return lSize / 1024 + "KB";
	}

	protected InputStream getInputStream() {
		return null;
	}

	public void stopDecode() {
		if (null != mBitmapOptions) {
			MtkLog.i(TAG, " Bitmap  stopDecode --------------");
			mBitmapOptions.requestCancelDecode();
		}
	}

	public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {
		return null;
	}
	
	public void stopThumbnail(){
		
	}
	
	private static final int BUFFER_SIZE = 1024 * 16;
	private static File sTempFolder;

	static {
		sTempFolder = new File("/tmp");// /data/data/com.mediatek.ui/cache

		if (!sTempFolder.exists()) {
			sTempFolder.mkdir();
		}
	}

	protected Bitmap decodeBitmap(InputStream in, int width, int height) {
		Bitmap bmp = null;
		if (in != null) {
			if (null == mBitmapOptions) {
				mBitmapOptions = new BitmapFactory.Options();
			}
			mBitmapOptions.inSampleSize = computeSampleSize(in, width,height);

			mBitmapOptions.inScaled = true;
			mBitmapOptions.inDensity = mWidth/mBitmapOptions.inSampleSize;
			if((mWidth*1000 / width) > (mHeight*1000 /height)){

				mBitmapOptions.inTargetDensity = width;
			}else{
				mBitmapOptions.inTargetDensity = mWidth*width/mHeight;
			}

			bmp = decodeBitmap(getInputStream(), mBitmapOptions);

			try {
				in.close();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				// in = null;
				// System.gc();
			}
			//int size = width > height? width : height;
		
		}

		return bmp;
	}

	protected String getResolution(InputStream in) {
		String resolution = null;

		if (mWidth < 0 && mHeight < 0) {
			decodeInJustBounds(in);
		}

		if (mWidth > 0 && mHeight > 0) {
			resolution = new StringBuffer().append(mWidth).append("*").append(
					mHeight).toString();
		}

		return resolution;
	}

	private Bitmap decodeBitmap(InputStream in, Options opts) {
		Bitmap bitmap = null;

		if (in != null) {
			//when decode thumbnial use the next BitmapFactory.decodeStream() way in dlna,the function cannot return.
//			bitmap = BitmapFactory.decodeStream(in, null, opts);
//			try {
//				in.close();
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
//			if (bitmap == null) {
			FileInputStream fileInput = null;
			File cache = null;
				try {
					in = getInputStream();
					if(null != in){
						cache = saveAsTemp(in);
						if(null != cache){
							fileInput = new FileInputStream(cache);
						}
						if(null != fileInput){
							bitmap = BitmapFactory.decodeStream(fileInput, null, opts);
						}
					}
				} catch (IOException e) {
					e.printStackTrace();
				}finally{
				if(null != cache){
					cache.delete();
				}
                     if(null != fileInput){
                         try{
						 	fileInput.close();
							}catch(IOException e){
							e.printStackTrace();
								}
					 }
				}
//			}
		}

		return bitmap;
	}


	public  String getPreviewBuf(InputStream is){
		return TextUtils.getPreviewBuf(is);


	}

	private void decodeInJustBounds(InputStream in) {
		if (in != null) {
			try {
				BitmapFactory.Options opts = new BitmapFactory.Options();
				opts.inJustDecodeBounds = true;
				BitmapFactory.decodeStream(in, null, opts);

				try {
					in.close();
				} catch (IOException e) {
					e.printStackTrace();
				}

				if (opts.outWidth < 0) {
					FileInputStream fileInput = null;
					File cache = null;
					try {
						in = getInputStream();
						if(null != in){
							cache = saveAsTemp(in);
							if(null != cache){
						    	fileInput = new FileInputStream(cache);
						    }
							if(null != fileInput){
							   BitmapFactory.decodeStream(fileInput, null, opts);
						    }
						}
					} catch (IOException e) {
						e.printStackTrace();
					}finally{
					if(null != cache){
					cache.delete();
				}
                     if(null != fileInput){
                         try{
						 	fileInput.close();
							}catch(IOException e){
							e.printStackTrace();
								}
					 }
				}
				}

				mWidth = opts.outWidth;
				mHeight = opts.outHeight;
			} catch (OutOfMemoryError e) {
				MtkLog.e(TAG, "Out Of Memory!!");
			}
		}
	}

	private int computeSampleSize(InputStream in, int display_width,int display_height ) {
		int sampleSize = -1;

		if (mWidth < 0 && mHeight < 0) {
			decodeInJustBounds(in);
		}

		if(mWidth <= display_width && mHeight<= display_height){
		
			sampleSize = 1;

		}else{
		

			int wSize = mWidth/display_width +(mWidth%display_width > 0 ? 1: 0);

			int hSize = mHeight/display_height + (mHeight%display_width > 0 ? 1: 0);
			
			sampleSize = Math.max(wSize,hSize);

			
		}

		
		int initialSize = sampleSize;


		int roundedSize;
        if (initialSize <= 8 ) {
            roundedSize = 1;
            while (roundedSize < initialSize) {
                roundedSize <<= 1;
            }
        } else {
            roundedSize = (initialSize + 7) / 8 * 8;
        }
		MtkLog.d(TAG, "computeSampleSize Decode Bitmap Sample Size : " + sampleSize +"roundedSize = "+ roundedSize);

        return roundedSize;
	}

	private File saveAsTemp(InputStream in) {
		File temp = null;
        try{
		  temp = File.createTempFile("bmp_", null, sTempFolder);
        }catch(IOException e1){
	        e1.printStackTrace();
        }
		FileOutputStream outputStream = null;
		try{
			if(null != temp){
				outputStream = new FileOutputStream(temp);
			}
		}catch(FileNotFoundException e1){
			e1.printStackTrace();
		}
		BufferedOutputStream out = new BufferedOutputStream(outputStream, BUFFER_SIZE);
		byte[] bytes = new byte[BUFFER_SIZE];
	

		try {
			while (in.read(bytes) >= 0) {
		    out.write(bytes);
	         }
		    out.flush();
		    out.close();
			in.close();
		} catch (IOException e) {
			e.printStackTrace();
		}finally{
            if(null != outputStream){
                  try{
                      outputStream.close();
                     }catch(IOException e){
							e.printStackTrace();
					}
			 }
		}

		return temp;
	}

	public String getDeviceName() {
		return "";
	}

	public abstract String getResolution();

	public abstract String getSize();

	public abstract String getTextSize();

	public abstract String getName();

	public abstract long lastModified();

	public abstract boolean isDirectory();

	public abstract String getPreviewBuf();
	public abstract boolean isFile();

	public abstract String getPath();

	public abstract String getAbsolutePath();

	public abstract long length();

	public abstract boolean delete();

	public abstract String getInfo();
	public abstract String getCacheInfo();

	public abstract String getSuffix();

	protected String assemblyInfos(String... infos) {
		StringBuilder builder = new StringBuilder();

		for (String info : infos) {
			if (info != null && info.length()>0) {
				builder.append(info);
			} else {
				builder.append("-");
			}

			builder.append("\n");
		}

		return builder.toString();
	}
	
	protected String setTime(int mills) {
		if (mills <= 0){
			return "-";
		}
		mills /= 1000;
		int minute = mills / 60;
		int hour = minute / 60;
		int second = mills % 60;
		minute %= 60;
		String text;
		try {
			text = String.format("%02d:%02d:%02d", hour, minute, second);
		} catch (Exception e) {
		    text = "";
			MtkLog.i(TAG, e.getMessage());
		}
		return text;
	}
}
