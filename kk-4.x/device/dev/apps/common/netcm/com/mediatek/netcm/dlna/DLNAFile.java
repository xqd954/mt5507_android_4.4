package com.mediatek.netcm.dlna;

import com.mediatek.dlna.object.DLNADevice;
import com.mediatek.dlna.object.Content;
import android.util.Log;

/**
 * This Class is use to store the content and device which found in DMS. 
 * <ul>
 * <li> DLNA Device object.</li>
 * <li> DLNA Content object.</li>
 * </ul>
 */
public class DLNAFile {
	public static final String TAG = "DLNAFile";
	public static final int TYPE_DEVICE = 0;
	public static final int TYPE_CONTENT = 1;
	public static final int TYPE_UNKNOW = 2;
	private int fileType = TYPE_UNKNOW;
	private DLNADevice device = null;
	private Content content = null;
	private String path = null;
	private String parent = null;

	/**
	 * Constructs a DLNAFile object with device, path and parent.
	 * 
	 * @param device   The DLNA device object.
	 * @param path     The path of device.
	 * @param parent   The parent path of device.
	 */
	public DLNAFile(DLNADevice device, String path, String parent) {
		this.device = device;
		this.fileType = TYPE_DEVICE;
		this.path = path;
		this.parent = parent;
	}
	
	/**
	 * Constructs a DLNAFile object with content, path and parent.
	 * 
	 * @param content   The DLNA content object.
	 * @param path     The path of content.
	 * @param parent   The parent path of device.
	 */
	public DLNAFile(Content content, String path, String parent) {
		this.content = content;
		this.fileType = TYPE_CONTENT;
		this.path = path;
		this.parent = parent;
	}

	/**
	 * Get type of DLNAFile's object.
	 * 
	 * @return one of TYPE_DEVICE, TYPE_CONTENT, TYPE_UNKNOW.
	 */
	public int getType() {
		return fileType;
	}

	/**
	 * Get DLNADevice's object.
	 * 
	 * @return DLNADevice's object.
	 */
	public DLNADevice getDevice() {
		return this.device;
	}
	
	/**
	 * Check if the DLNAFile object is device or not.
	 * 
	 * @return true if the DLNAFile object is device, else false.
	 */
	public boolean isDevice(){
		if(getType() == TYPE_DEVICE) {
			return true;
		}
		
		return false;
	}
	
	/**
	 * Get device name.
	 * 
	 * @param path  The path which will be parsed.
	 * @return the device name.
	 */
	public String getDevice(String path){
		if (path == null || path.length() <= 1){
			return null;
		}
		String s = path.substring(1);
		return s.substring(0, s.indexOf("/"));
	}
	
	/**
	 * Check if the content is directory or not.
	 * 
	 * @return true if the content is directory, otherwise false.
	 */
	public boolean isDirectory() {
		int type = this.getType();
		if(type == TYPE_DEVICE) {
			return false;
		}
		else if (type == TYPE_CONTENT) {
			return this.content.isDirectory();
		}
		
		return false;
	}
	
	/**
	 * Check if the content is file or not.
	 * 
	 * @return true if the content is file, otherwise false.
	 */
	public boolean isFile() {
		int type = this.getType();
		if(type == TYPE_DEVICE) {
			return false;
		}
		else if (type == TYPE_CONTENT) {
			return this.content.isFile();
		}
		
		return false;
	}

	/**
	 * Get the object of Content.
	 * 
	 * @return the object of content.
	 */
	public Content getContent() {
		return this.content;
	}

	/**
	 * Get the name of DNAFile object.
	 * 
	 * @return the name of object.
	 */
	public String getName() {
		int type = this.getType();
		if (type == TYPE_DEVICE)
			return this.getDevice().getName();
		else if (type == TYPE_CONTENT)
			return this.getContent().getTitle();
		return null;
	}
	
	/**
	 * Get the parent path of this DNAFile object.
	 * 
	 * @return the parent path of this DNAFile object.
	 */
	public String getParent(){
		return this.parent;
	}
	
	/**
	 * Get the path of this DNAFile object.
	 * 
	 * @return the path of this DNAFile object.
	 */
	public String getPath(){
		return this.path;
	}

	public boolean isSubTitleFile(){

		if(this.getType() == TYPE_CONTENT){
			String fileName = this.getContent().getTitle().toLowerCase();
			Log.d(TAG,"isSubTitleFile fileName = "+ fileName);
			if( fileName.endsWith(FileSuffixConst.DLNA_FILE_NAME_EXT_XML) 
				|| fileName.endsWith(FileSuffixConst.DLNA_FILE_NAME_EXT_M4T) ){
				return true;

			}
			
		}
		return false;

	}
	
	/**
	 * Get the suffix name of content.
	 * 
	 * @return the suffix name of content.
	 */
	public String getSuffix() {
		String mimeType = null;
		
		int type = this.getType();
		if (type == TYPE_DEVICE) {
			return null;
		}
		else if (type == TYPE_CONTENT) {
			mimeType = this.content.getMimeType();
			if(mimeType.startsWith(".")){
				if(mimeType.length() == 1){
					return FileSuffixConst.DLNA_FILE_NAME_EXT_UNKONWN;
				}
				else {
					return mimeType;
				}
			}
			
			 /* MUSIC */
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_MPEG)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MP3;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_3GPP)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_3PG;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_WAV)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_WAV;
			}
			
			else if (mimeType.equals(FileSuffixConst. DLNA_MEDIA_MIME_TYPE_AUDIO_X_MS_WMA)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_WMA;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_X_SONY_OMA)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_OMA;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_L16)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_PCM;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_MP4)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MP4;
			}
			
			
			/* IMAGE */
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_IMAGE_BMP)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_BMP;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_IMAGE_GIF)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_GIF;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_IMAGE_JPEG)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_JPG;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_IMAGE_PNG)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_PNG;
			}
			
			/* VIDEO */
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_MPEG)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MPG;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_VND_DLNA_MPEG_TTS)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_TTS;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_X_MS_ASF)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_ASF;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_X_MS_WMV)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_WMV;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_X_MS_AVI) ||
					mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_AVI) ||
					mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_DIVX) ||
					mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_H263) ||
					mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_H264)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_AVI;
			}
			
			else if (mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_MP4)){
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MP4;
			}
			
			else if(mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_QK_TE_MOV)) {
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MOV;
			}
			
			else if(mimeType.equals(FileSuffixConst.DLNA_MEDIA_MIME_TYPE_VIDEO_X_MATROSSKA_MKV)) {
				return FileSuffixConst.DLNA_FILE_NAME_EXT_MKV;
			}
		}
		
		return FileSuffixConst.DLNA_FILE_NAME_EXT_UNKONWN;
	}
	
	/**
	 * Get size of this content.
	 * 
	 * @return size of this content.
	 */
	public long getSize() {
		int type = this.getType();
		if(type == TYPE_DEVICE)
			return 0;
		else if(type == TYPE_CONTENT)
			return this.getContent().getSize();
		return 0;
	}
	
//	public String getDate() {
//		int type = this.getType();
//		if(type == TYPE_DEVICE)
//			return null;
//		else if(type == TYPE_CONTENT)
//			return this.getContent().getResDuration();
//		return null;
//	}

	/**
	 * Check if the paramer equal with this DLNAFile object.
	 * 
	 * @return true if equal, otherwise false.
	 */
	public boolean equals(DLNAFile fileIns) {
		int type = this.getType();
		if (type != fileIns.getType())	
			return false;
		if (type == TYPE_DEVICE) {
			if(this.getDevice() == null || this.getDevice().getName() == null)
				return false;
			if (this.getDevice().getName().equals(fileIns.getDevice().getName()))
				return true;
		} else if (type == TYPE_CONTENT) {
			if(this.getContent().getTitle() == null || this.getContent().getTitle() == null || fileIns.getContent() == null)
				return false;
			if (this.getContent().getTitle().equals(fileIns.getContent()
					.getTitle()))
				return true;
		}
		return false;
	}
}
