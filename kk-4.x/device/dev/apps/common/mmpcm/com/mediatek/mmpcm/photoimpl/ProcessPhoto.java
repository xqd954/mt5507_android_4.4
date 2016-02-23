package com.mediatek.mmpcm.photoimpl;

import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.mmcimpl.Const;

import android.graphics.Bitmap;
import android.graphics.Camera;
import android.graphics.Matrix;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;
import android.widget.RelativeLayout;

/**
 * 
 * This class represents process photo.
 *
 */
public class ProcessPhoto {

	private static Camera mCamera = new Camera();
	private static Matrix mMatrix = new Matrix();

	public static int ZOOM_OUT = 0;
	public static int ZOOM_IN = 1;


	/**
	 * Rorate a bitmap.
	 * @return
	 */
	public static Bitmap Rotate(Bitmap bm, int[] dg) {
		if (null != bm && dg !=null && dg.length > 0 ) {
			int centerX, centerY;
			int widthOrig = bm.getWidth();
			int heightOrig = bm.getHeight();
			centerX = widthOrig>>1;
		    centerY = heightOrig>>1;
			 mCamera.save();
			 mCamera.rotateX(dg[0]);
			 mCamera.rotateY(dg[1]);
			 mCamera.rotateZ(dg[2]);
			 int arr[] = dg;
			 MmpTool.LOG_INFO("plz Rotate"+"{"+arr[0]+","+arr[1]+","+arr[2]+"}");
	         mCamera.getMatrix(mMatrix);
	         mCamera.restore(); 
	         mMatrix.preTranslate(-centerX, -centerY);
	         mMatrix.postTranslate(centerX, centerY);  
	         mCamera.save(); 
			try {
				Bitmap resizedBitmap = Bitmap.createBitmap(bm, 0, 0, widthOrig,
						heightOrig, mMatrix, true);
				if (bm != null && !bm.equals(resizedBitmap)) {
	                    bm.recycle();
	                    bm = resizedBitmap;
	                }
			} catch (OutOfMemoryError ex) {
				ex.printStackTrace();
			// We have no memory to rotate. Return the original bitmap.
			}	catch(RuntimeException Re){
				MmpTool.LOG_WARN("----use a recycled bitmap--------");
				Re.printStackTrace();
			}			
		}

		// BitmapDrawable myNewRotateBitmapDrawable = new BitmapDrawable(
		// resizedBitmap);
		return bm;
	}


	/**
	 * zoom out or zoom in.
	 * 
	 * @param image
	 * @return
	 */
	public void zoom(ImageView image, int inOrOut, Bitmap bitmap, float size) {

		// this.imageView = (MtkImageView) image;

		int oldWidth, oldHeight;
		int newWidth, newHeight;

		image.setScaleType(ScaleType.FIT_XY);
		/*
		 * if (inOrOut == ZOOM_OUT){ imageView.scaleWithKey(ZOOM_OUT); } else{
		 * imageView.scaleWithKey(ZOOM_IN); }
		 */
		if ((inOrOut != ConstPhoto.ZOOM_OUT) && (inOrOut != ConstPhoto.ZOOM_IN)) {
			if (image != null) {

				newHeight = (int) (bitmap.getHeight() * size);
				newWidth = (int) (bitmap.getWidth() * size);

				LayoutParams params = image.getLayoutParams();
				params.height = newHeight;
				params.width = newWidth;

				image.setImageBitmap(bitmap);
				image.setLayoutParams(params);

			} else {
				MmpTool.LOG_ERROR("(ProcessPhoto zoom())image is null !!!!");
			}
		} else if ((inOrOut == ZOOM_OUT) && (image != null)) {

			if (image.getHeight() == 0) {
				oldWidth = image.getDrawable().getIntrinsicWidth();
				oldHeight = image.getDrawable().getIntrinsicHeight();
			} else {
				oldWidth = image.getWidth();
				oldHeight = image.getHeight();
			}

			newHeight = (int) Math.ceil(oldHeight * 1.25);
			newWidth = (int) Math.ceil(oldWidth * 1.25);

			LayoutParams params = image.getLayoutParams();
			params.height = newHeight;
			params.width = newWidth;
			image.setImageBitmap(bitmap);
			image.setLayoutParams(params);
		} else if ((inOrOut == ZOOM_IN) && (image != null)) {
			if (image.getHeight() == 0) {
				oldWidth = image.getDrawable().getIntrinsicWidth();
				oldHeight = image.getDrawable().getIntrinsicHeight();
			} else {
				oldWidth = image.getWidth();
				oldHeight = image.getHeight();
			}

			newHeight = (int) Math.floor(oldHeight * 0.8);
			newWidth = (int) Math.floor(oldWidth * 0.8);

			LayoutParams params = image.getLayoutParams();
			params.height = newHeight;
			params.width = newWidth;
			image.setImageBitmap(bitmap);
			image.setLayoutParams(params);
		}

	}
	
}
