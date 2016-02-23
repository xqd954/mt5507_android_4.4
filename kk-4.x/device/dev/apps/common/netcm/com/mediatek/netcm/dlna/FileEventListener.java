package com.mediatek.netcm.dlna;


/**
 * This class use to notify the parse action which called
 * by DLNA manager.
 * 
 */
public interface FileEventListener{
	
	/**
	 * Notify the application some files found.
	 * 
	 * @param event notify the parse action successful.
	 */
	public void onFileFound(FileEvent event);
	
	/**
	 * Notify the application some file left.
	 * 
	 * @param event notify some file left.
	 */
	public void onFileLeft(FileEvent event);
	
	/**
	 * Notify the application the parse action failed.
	 * 
	 * @param event notify the application the parse action failed.
	 */
	public void onFileFailed(FileEvent event);
	
}