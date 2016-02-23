package com.mediatek.mmpcm.fileimpl;

import java.io.File;
import java.net.URI;

import android.graphics.Bitmap;

public class TextFile extends MtkFile {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public TextFile(MtkFile f) {
		super(f.getPath());
		// TODO Auto-generated constructor stub
	}
	
	public TextFile(URI uri) {
		super(uri);
		// TODO Auto-generated constructor stub
	}

	public TextFile(String dirPath, String name) {
		super(dirPath, name);
		// TODO Auto-generated constructor stub
	}

	public TextFile(String path) {
		super(path);
		// TODO Auto-generated constructor stub
	}

	public TextFile(File dir, String name) {
		super(dir, name);
		// TODO Auto-generated constructor stub
	}

//	@Override
//	String getTest() {
//		// TODO Auto-generated method stub
////		return super.getTest();
//		return new String("Text:"+this.getName());
//	}

	@Override
	public Bitmap getThumbnail(int width,int height,boolean isThumbnail) {
		// TODO Auto-generated method stub
		return super.getThumbnail(width,height,isThumbnail);
	}

}
