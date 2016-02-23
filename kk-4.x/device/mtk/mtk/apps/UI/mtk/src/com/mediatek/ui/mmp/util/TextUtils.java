package com.mediatek.ui.mmp.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.FileOutputStream;

import java.io.FilenameFilter;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import com.mediatek.mmpcm.CommonStorage;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.Log;
import android.content.Context;
import com.mediatek.mmpcm.fileimpl.FileConst;



public class TextUtils {
	
	public static final String TAG = "Utils";
	private static TextUtils mInstance ;
	private CommonStorage mStorage ;

	private static final String localPath = "/tmp/txt_prebuf_temp.txt";

	private static File tmpFile ;

	public static final String TEXT_FONTSIZE = "Text_FontSize";
	
	public static final String TEXT_FONTCOLOR = "Text_FontColor";
	
	public static final String TEXT_FONTSTYLE = "Text_FontStyle";

	


	public static final float SMALLSIZE = 12.0f;
	public static final float MEDSIZE = 18.0f;
	public static final float LARSIZE = 24.0f;


	
	public final static int STATUS_NORMAL = 0;
	public final static int STATUS_ABNORMAL = 1;
	public final static int STATUS_BUSY = 2;
    
    /* Player Mode */ 
    public static final int PLAYER_MODE_LOCAL = FileConst.SRC_USB;
    public static final int PLAYER_MODE_SAMBA = FileConst.SRC_SMB;    
    public static final int PLAYER_MODE_DLNA = FileConst.SRC_DLNA; 
    public static final int PLAYER_MODE_HTTP = FileConst.SRC_HTTP;
    
    

	

	public static final int mColorArray[] = {Color.WHITE,Color.BLUE,Color.RED,Color.GREEN,Color.BLACK};
	public static final int mTypeFaceArray[] = { Typeface.NORMAL,Typeface.ITALIC,Typeface.BOLD,Typeface.BOLD_ITALIC};
	public static final float mSizeArray[] = {SMALLSIZE,MEDSIZE,LARSIZE};
	

	

	private TextUtils(Context context){
		mStorage = CommonStorage.getInstance(context);



	}

	public static TextUtils getInstance(Context context){
		if(mInstance == null){
			mInstance = new TextUtils(context);

		}
		return mInstance;
	}




	public int getFontColorIndex() {
		int colorIndex = 0;

		int mColor = mStorage.get(TEXT_FONTCOLOR, Color.WHITE);
		
		for(int color : mColorArray){
			if(color == mColor){
				return colorIndex;
				
			}

			colorIndex++;
			
		}


		return colorIndex;
	}


	public int getFontSizeIndex() {

		
		int sizeIndex = 0;

		float mSize = mStorage.get(TEXT_FONTSIZE,SMALLSIZE);
		
		for(float size : mSizeArray){
			if(size == mSize){
				return sizeIndex;
				
			}

			sizeIndex++;
			
		}


		return sizeIndex;
			
	}


	public int getFontStyleIndex() {
		
		
		int styleIndex = 0;

		int mFontStyle = getFontStyle();
		
		for(int style : mTypeFaceArray){
			if(style == mFontStyle){
				return styleIndex;
				
			}

			styleIndex++;
			
		}


		return styleIndex;
		
	}








	public void saveFontColor(int color){
		Log.d(TAG,"saveFontColor = "+color);
		mStorage.set(TEXT_FONTCOLOR,color);

	}

	public void saveFontSize(float size){
		Log.d(TAG,"saveFontSize = "+size);
		mStorage.set(TEXT_FONTSIZE,size);

	}
	public void saveFontStyle(int style){
		Log.d(TAG,"saveFontStyle = "+style);
		mStorage.set(TEXT_FONTSTYLE,style);

	}


	public int getFontColor(){
		int mColor = mStorage.get(TEXT_FONTCOLOR, Color.WHITE);
		Log.d(TAG,"getFontColor = "+mColor);
		return mColor;

	}

	public float getFontSize(){
		float mSize = mStorage.get(TEXT_FONTSIZE,SMALLSIZE);
		Log.d(TAG,"getFontSize = "+mSize);
		return mSize;

	}
	public int getFontStyle(){
		int mFontStyle = mStorage.get(TEXT_FONTSTYLE,Typeface.NORMAL);
		Log.d(TAG,"getFontStyle = "+mFontStyle);
		return mFontStyle;
	}



	
	private static void loadLocalPath(InputStream input) {

		
			//String path = localPath;
			int bufSize = 1024;

			try{
				tmpFile= new File(localPath);
		
				//tmpFile.deleteOnExit();
		
				byte[] buffer = new byte[bufSize];
				FileOutputStream output = new FileOutputStream(tmpFile);
		
				for (int i = 0; i <= 2; i++) {
					int ret = input.read(buffer);
					if (ret == -1) {
						break;
					}
		
					output.write(buffer, 0, ret);
				}
		
		
				output.close();
				input.close();
			}catch(Exception e){

				e.printStackTrace();


			}
			
			//return localPath;
		}
	


	/**
	 * gain preview buffer
	 * @param fileuri path
	 */
	public static String getPreviewBuf(InputStream is) {
		Log.d(TAG,"getPreviewBuf ~");

		loadLocalPath(is);
		//File file = new File(localPath);
		final StringBuilder sb = new StringBuilder();
		if(tmpFile!= null && tmpFile.exists()){
			
			try {
				FileInputStream fileIS = new FileInputStream(tmpFile);
				BufferedReader br = new BufferedReader(new InputStreamReader(fileIS, getCharset(tmpFile)));
				String line;
				while(true) {
					if ((line = br.readLine()) != null) {
						Log.d(TAG,"getPreviewBuf line = "+ line);
						sb.append(line);
						sb.append('\n');
					} else{
						break;
					}
				}
				br.close();

			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		return sb.toString();
	}


	
	public static String getCharset(File file) {
		String charset = "GBK";
		byte[] first3Bytes = new byte[3];
		try {
			boolean checked = false;
			FileInputStream fileIS = new FileInputStream(file);
			BufferedInputStream bis = new BufferedInputStream(fileIS);
			bis.mark(1);
			int read = bis.read(first3Bytes, 0, 3);
			if (read == -1){
				bis.close();
				return charset;
			}
			if (first3Bytes[0] == (byte) 0xFF && first3Bytes[1] == (byte) 0xFE) {
				charset = "UTF-16LE";
				checked = true;
			} else if (first3Bytes[0] == (byte) 0xFE
					&& first3Bytes[1] == (byte) 0xFF) {
				charset = "UTF-16BE";
				checked = true;
			} else if (first3Bytes[0] == (byte) 0xEF
					&& first3Bytes[1] == (byte) 0xBB
					&& first3Bytes[2] == (byte) 0xBF) {
				charset = "UTF-8";
				checked = true;
			}
			bis.reset();
			if (!checked) {
				int loc = 0;
				while ((read = bis.read()) != -1) {
					loc++;
					if (read >= 0xF0)
						break;
					if (0x80 <= read && read <= 0xBF)
						break;
					if (0xC0 <= read && read <= 0xDF) {
						read = bis.read();
						if (0x80 <= read && read <= 0xBF)
							continue;
						else
							break;
					} else if (0xE0 <= read && read <= 0xEF) {
						read = bis.read();
						if (0x80 <= read && read <= 0xBF) {
							read = bis.read();
							if (0x80 <= read && read <= 0xBF) {
								charset = "UTF-8";
								break;
							} else
								break;
						} else
							break;
					}
				}
			}
			bis.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return charset;
	}


}
