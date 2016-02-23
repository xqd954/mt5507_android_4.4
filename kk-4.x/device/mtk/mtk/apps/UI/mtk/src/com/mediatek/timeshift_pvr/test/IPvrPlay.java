/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.test;

import java.io.File;

/**
 *
 */
public interface IPvrPlay {

	// Play PVR files
	boolean initPvrPlayer();

	boolean startPlayPvr(String path);
	boolean startPlayPvr();

	boolean stopPlayPvr();

	boolean pausePlayPvr();

	boolean forwardPlay(int speed);

	boolean rewindPlay(int speed);

	boolean deletePVRFile(File file);
}
