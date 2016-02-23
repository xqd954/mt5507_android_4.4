package com.mediatek.mmpcm.threedimen.photoimpl;

import com.mediatek.common.PhotoPlayer.MtkPhotoPlayer;
import android.R.integer;
import com.mediatek.common.PhotoPlayer.MtkPhotoHandler;
import com.mediatek.common.PhotoPlayer.NotSupportException;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.mmpcm.threedimen.photo.IPlayback;
import com.mediatek.mmpcm.threedimen.photo.IThrdPhotoEventListener;




import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.UnknownHostException;


import com.mediatek.netcm.dlna.DLNADataSource;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.netcm.samba.SambaManager;

import android.util.Log;
import com.mediatek.mmpcm.fileimpl.FileConst;



/**
* The class manager 3D photo.
**/
public class MPlayback implements IPlayback{
	private final static String TAG = "MPlayback"; 
	private MtkPhotoPlayer mPhotoPlayer;
	private IThrdPhotoEventListener mEventListener;
	
	public boolean dumy = false;

	public static final int LOCAL = FileConst.SRC_USB;
    public static final int SAMBA = FileConst.SRC_SMB;
    public static final int DLNA = FileConst.SRC_DLNA;
	public static final int URL = FileConst.SRC_HTTP;

	private boolean cancelState = false;
	public MPlayback() {
		mPhotoPlayer = new MtkPhotoPlayer();
		
	}


	/**
	 * decode 3Dphoto
	 */
	public void decode3DPhoto(String path,int source){
		
		cancelState = false;
		MmpTool.LOG_INFO("Photo play path = 1" + path);
		if( path == null){
		   if(mEventListener != null){
		   	mEventListener.onCompleted();
		   	return;
		  }
		
		}
		
		if(mPhotoPlayer != null ){

			if(!dumy){
				
				try{
					mPhotoPlayer.ConnetVDP();
				  	String localPath = path;
				  	if(source != LOCAL ){
				  		localPath = getLocalPath(path,source);
				  	}
					Log.d(TAG,"decode3DPhoto localPath ="+ localPath);
					MtkPhotoHandler mtkPhotoHandler = null;
					if(!cancelState){
						synchronized(this) {
							if(!cancelState){
						  		mtkPhotoHandler = mPhotoPlayer.Decode(localPath);
							}
						}
					}
					if(!cancelState){
						synchronized(this) {
							if(null != mtkPhotoHandler && !cancelState){
							   	mEventListener.decodeSuccess();
							   	mPhotoPlayer.Display(mtkPhotoHandler);
								mEventListener.playDone();
							}
						}
					}
					  
				}catch (Exception e){
					if(mEventListener != null && !cancelState){
						mEventListener.onPlayFailed();
					}
					close();
					e.printStackTrace();
				}
			}
		}
		

		if (source != LOCAL) {
			if (image!=null && image.exists()) {
				image.delete();
			}
		}

		 
		
	}

	
	/*
	*add for mark cancel state, if 
	*/
	public void cancel(){
		synchronized(this) {
			cancelState = true;
		}
		
	}
	/**
	 * disconnect to vdp
	 */
	public void close(){
		int result = 0;
		if (mPhotoPlayer != null){
			try{
				if(!dumy){
					result = mPhotoPlayer.DisConnetVDP();
				}
			}catch (NotSupportException e){
				if (mEventListener != null && !cancelState){
					mEventListener.onCloseFailed();
				}
			}
		}
		MmpTool.LOG_INFO("Photo close result = " + result);
	}



		/* For net ,get local path */
	private File image;
	private String tmpPath = "/tmp/photo3d";
	private int bufSize = 64 * 1024;
	private byte[] buffer = null;
	private FileOutputStream output = null;
	/**
	 * Get local play path.
	 * @return
	 * @throws IOException
	 */
	public String getLocalPath(String path,int source) throws IOException {
		InputStream input = getInputStream(path,source);

		if (null == input) {
			Log.d(TAG,"null == input:path:"+path);
			return "";
		}
		String tmpLocalPath = "";
		if(source != DLNA){
			int index = path.lastIndexOf(".");
			if(index > 0){
				tmpLocalPath  = path.substring(index);
			}
		}else{

			tmpLocalPath = DLNAManager.getInstance().getDLNAFileSuffix(path);

		}

		tmpLocalPath = tmpPath + tmpLocalPath;
		Log.d(TAG,"getLocalPath tmpLocalPath ="+ tmpLocalPath);
		image = new File(tmpLocalPath);
		image.deleteOnExit();

		if (null == buffer) {
			buffer = new byte[bufSize];
		}

		output = new FileOutputStream(image);

		int ret = input.read(buffer);

		while (ret > 0 && !cancelState) {
			output.write(buffer, 0, ret);
			ret = input.read(buffer);
		}
		output.close();
		input.close();
		if(cancelState){
			Log.d(TAG,"cancelState path:"+path);
			return null;
		}
		return tmpLocalPath;
	}

	protected InputStream getInputStream(String path ,int source) {
		InputStream input = null;
		Log.d(TAG,"cancelState path:"+path+"   source:"+source);
		if (source == SAMBA) {
			try {
				input = SambaManager.getInstance().getSambaDataSource(path)
						.newInputStream();
			} catch (Exception e) {
				e.printStackTrace();
			} 
		} else if (source == DLNA) {
			DLNADataSource mDLNADataSource = DLNAManager.getInstance()
					.getDLNADataSource(path);
			if (mDLNADataSource != null) {
				input = mDLNADataSource.newContentInputStream();
			}
		} else if (source == URL) {

			try {
				URL url = new URL(path);
				HttpURLConnection httpURLconnection;
				httpURLconnection = (HttpURLConnection) url.openConnection();
				httpURLconnection.setRequestMethod("GET");
				httpURLconnection.setReadTimeout(10 * 1000);

				String responseCode = url.openConnection().getHeaderField(0);

				if (responseCode.indexOf("200") < 0)
					try {
						throw new Exception(
								"Image file is not exit or path is error,error code"
										+ responseCode);
					} catch (Exception e) {
						e.printStackTrace();
					}

				if (httpURLconnection.getResponseCode() == 200) {
					input = url.openStream();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return input;
	}
	

	/**
	 * set event listener, notify caller.
	 * @param listener
	 */
	public void setEventListener(IThrdPhotoEventListener listener){
		mEventListener = listener;
	}
	
	/**
	 * Release source
	 */
	public void onRelease(){
		if (mPhotoPlayer != null){
			mPhotoPlayer = null;
		}
	}
}
