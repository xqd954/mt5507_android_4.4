package com.mediatek.mmpcm.fileimpl;

import java.io.File;
import java.io.IOException;
import java.net.URI;

import com.mediatek.mmpcm.MmpTool;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Bitmap.Config;
import android.media.ExifInterface;
import android.util.Log;
import com.mediatek.mmpcm.photoimpl.ProcessPhoto;
import com.mediatek.mmpcm.mmcimpl.Const;


public class ThrDPhotoFile extends MtkFile{

	/**
	 * 
	 */
	private static final String TAG = "ThrDPhotoFile";
	private static final long serialVersionUID = 1L;

	public ThrDPhotoFile(MtkFile f) {
		super(f.getPath());
		
	}
	public ThrDPhotoFile(URI uri) {
		super(uri);
	}

	public ThrDPhotoFile(String dirPath, String name) {
		super(dirPath, name);
	}

	public ThrDPhotoFile(String path) {
		super(path);
	}

	public ThrDPhotoFile(File dir, String name) {
		super(dir, name);
	}
	
	public String getResolution(){

		/*if (!isValidPhoto()){
			return null;
		}*/
		if (width == 0 || height == 0) {
			BitmapFactory.Options o = new BitmapFactory.Options();
			o.inJustDecodeBounds = true;
			o.inSampleSize = 1;
			BitmapFactory.decodeFile(this.getAbsolutePath(), o);
			width = o.outWidth;
			height = o.outHeight;
		}

		return new StringBuffer().append(width).append("*").append(height)
				.toString();
	}
	
	private int width;
	private int height;
	
	/**
	 * if photo larger than 20M, unvalid.
	 * @return if true, valide. else unvalid
	 */
	/*private boolean isValidPhoto(){
		if (this.getFileSize() > 20*1024*1024){
			return false;
		}
		return true;
	}*/
	
	
	public Bitmap getThumbnail(int width, int height,boolean isThumbnail){

		Bitmap bmp = null;

		try {

			MmpTool.LOG_INFO("starting--------");
			/*if (!isValidPhoto()){
				return null;
			}*/
			bmp = decodeBitmap(this, width);
//			bmp = ThumbnailUtils.extractThumbnail(bmp, width, height,
//					ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
			if (hisense) {
				bmp = scaleBitmap(bmp, width);
			}
		} catch (OutOfMemoryError e) {
			MmpTool.LOG_ERROR("OutOfMemoryError!!!");
		}
		return bmp;
	}
	
	private static BitmapFactory.Options opt = new BitmapFactory.Options();
	private Bitmap decodeBitmap(ThrDPhotoFile thrDPhotoFile, int requiredSize) {
		Bitmap bmp = null;

		opt.mCancel = false;
		Log.d(TAG, "opt.mCancel" + opt.mCancel);
		if (thrDPhotoFile != null) {
			// Decode image size
			if (width == 0 || height == 0) {
				opt.inJustDecodeBounds = true;
				opt.inSampleSize = 1;
				MmpTool.LOG_INFO("thumbnial_decoding 2222222222222222222");
				Log.d(TAG, thrDPhotoFile.getAbsolutePath());
				BitmapFactory.decodeFile(this.getAbsolutePath(), opt);
				width = opt.outWidth;
				height = opt.outHeight;
				MmpTool.LOG_INFO("width = " + opt.outWidth +";" + " height = "+ opt.outHeight);
			}
			// Find the correct scale value. It should be the power of 2.
			if (height > requiredSize || width > requiredSize) {
				int scale = (int) Math.pow(2.0, (int) Math.round(Math
						.log(requiredSize / (double) Math.max(height, width))
						/ Math.log(0.5)));
				// Decode with inSampleSize
				opt.inJustDecodeBounds = false;
				opt.inSampleSize = scale;
				MmpTool.LOG_INFO("thumbnial_decoding 3333333333333333");
				MmpTool.LOG_INFO(thrDPhotoFile.getAbsolutePath());
				bmp = BitmapFactory.decodeFile(thrDPhotoFile.getAbsolutePath(), opt);
			} else {
				opt.inJustDecodeBounds = false;
				opt.inSampleSize = 1;
				MmpTool.LOG_INFO("thumbnial_decoding 4444444444444444444");
				MmpTool.LOG_INFO(thrDPhotoFile.getAbsolutePath());
				bmp = BitmapFactory.decodeFile(thrDPhotoFile.getAbsolutePath(), opt);
			}
		}

		if (getOrientation() != -1) {

			if (getOrientation() == 1) {
				return bmp;
			} else {
				int orientation = getOrientation();
				if (null != bmp) {
					bmp = ProcessPhoto.Rotate(bmp,Const.ORIENTATION_ARRAY[orientation]);//rotate(bmp, orientation);
				}			
			}
		}

		return bmp;
	}

	// add for Hisense
	private static boolean hisense = false;

	public void hisenseOn() {
		hisense = true;
	}
	
	private Bitmap scaleBitmap(Bitmap source, int size) {
		if (source == null) {
			return source;
		}
		Matrix matrix = new Matrix();
		int width = source.getWidth();
		int height = source.getHeight();
		float sx = (float) size / height;

		if (width > height) {
			sx = (float) size / width;
		}
		matrix.setScale(sx, sx);
		try {
			Bitmap bitmap = Bitmap.createBitmap(source, 0, 0, source.getWidth(), source
					.getHeight(), matrix, false);

			if (!bitmap.equals(source)) {				
				source.recycle();
				MmpTool.LOG_INFO("source recycle ? "+source.isRecycled());
			}

			Bitmap bitmap1 = Bitmap.createBitmap((int) size, (int) size,
					Config.ARGB_8888);
			Canvas canvas = new Canvas(bitmap1);
			float left = 0;
			float right = 0;
			int width1 = bitmap.getWidth();
			int height1 = bitmap.getHeight();
			if (width1 < size) {
				left = (size - width1) / 2;
			}
			if (height1 < size) {
				right = (size - height1) / 2;
			}
			canvas.drawBitmap(bitmap, left, right, null);
			
			bitmap.recycle();
			MmpTool.LOG_INFO("bitmap recycle ? "+bitmap.isRecycled());
			
			return bitmap1;
		} catch (OutOfMemoryError e) {
			MmpTool.LOG_ERROR("OutOfMemoryError!!!");
		}
		return null;
	}
	
	

	public void stopDecode() {
		MmpTool.LOG_INFO("STOP Entered 000000000000000");
	if (null != opt) {
		MmpTool.LOG_INFO("STOP Starting:1111111111111");
		opt.requestCancelDecode();
		MmpTool.LOG_INFO("STOP Ending:2222222222222");
		
		opt.mCancel = false;
		MmpTool.LOG_INFO("opt.mCancel" + opt.mCancel);

		}
	}
	
	public void stopThumbnail(){
		//TODO
		stopDecode();
	}
	//private int degree = 0;

	private int getOrientation() {
		//degree = 0;
		ExifInterface exif = null;

		if (null == this.getAbsoluteFile()) {
			Log.d(TAG, "curPath is null!!!");
		}

		try {

			exif = new ExifInterface(this.getAbsolutePath());

			MmpTool.LOG_INFO("curPath = " + this.getAbsolutePath());
		} catch (IOException ex) {
			Log.e(TAG, "cannot read exif", ex);
			return -1;
		}
		int orientation = -1;
		if (exif != null) {
				 orientation = exif.getAttributeInt(
					ExifInterface.TAG_ORIENTATION, -1);
			/*if (orientation != -1) {
				// We only recognize a subset of orientation tag values.
				switch (orientation) {
				case ExifInterface.ORIENTATION_ROTATE_90:
					degree = 90;
					break;
				case ExifInterface.ORIENTATION_ROTATE_180:
					degree = 180;
					break;
				case ExifInterface.ORIENTATION_ROTATE_270:
					degree = 270;
					break;
				}

		}*/
		}

		return orientation;
	}
	private Bitmap rotate(Bitmap bm, int dg) {

		int widthOrig = bm.getWidth();
		int heightOrig = bm.getHeight();

		int newWidth = widthOrig;
		int newHeight = heightOrig;
		float scaleWidth = ((float) newWidth) / widthOrig;
		float scaleHeight = ((float) newHeight) / heightOrig;

		Matrix rotatematrix = new Matrix();
		rotatematrix.postScale(scaleWidth, scaleHeight);

		rotatematrix.setRotate(dg);

		try {
			Bitmap resizedBitmap = Bitmap.createBitmap(bm, 0, 0, widthOrig,
					heightOrig, rotatematrix, true);
			if (!resizedBitmap.equals(bm)) {
				bm.recycle();
				bm = resizedBitmap;

			}
		} catch (OutOfMemoryError e) {
			MmpTool.LOG_ERROR("OutOfMemoryError");
		}

		return bm;
	}
	

}
