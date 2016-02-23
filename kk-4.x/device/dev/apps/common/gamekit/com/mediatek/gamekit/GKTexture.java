package com.mediatek.gamekit;

import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.GLUtils;

public class GKTexture {
	
	private int GLId;
	private int width;
	private int hegiht;
	private int colorMode;
	 
	  GKTexture(int id, int w, int h, int mode){
		GLId = id;
		width = w;
		hegiht = h;
		colorMode = mode;
	}
	
	private int getGLId(){
		return GLId;
	}
	
	public int getWidth(){
		return width;
	}
	
	public int getColorMode(){     
		return colorMode;  
	}
	
	public int getHeight(){
		return hegiht;
	}
 
	/**
	 * This must be called in GKEngine thread.
	 * @param bitmap
	 */
	void upload(Bitmap bitmap){ 
		
		GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, getGLId());
		GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0); 
		//GLUtils.texSubImage2D(GLES20.GL_TEXTURE_2D, 0, 0, 0, bitmap);
		GLES20.glGenerateMipmap(GLES20.GL_TEXTURE_2D);
		
	}
}
