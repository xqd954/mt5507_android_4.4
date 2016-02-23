/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.test;

import java.io.File;
import java.util.Date;

/**
 *
 */
public interface IPVRoperation<T> {

	// For PVR-Record
	boolean initPVRRecord();

	boolean startPVRrecord();

	boolean stopPVRRecord();

	boolean pausePVRRecord();

	// Set PVR Info before recording.
	void setDuration(int duration);

	void setStartTime(Date date);

	T getPVRFileInfo(File file);

	boolean createPVRFile(String name);

	// Play PVR files
	boolean initPVRplay();

	boolean startPVRPlay();

	boolean stopPVRPlay();

	boolean pausePVRPlay();

	boolean forwardPVRplay(int speed);

	boolean backwardPVRplay(int speed);

	boolean deletePVRFile(File file);
}
