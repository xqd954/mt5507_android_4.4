/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.test;

import com.mediatek.record.RecordSample;
import com.mediatek.timeshift_pvr.manager.Util;

/**
 *
 */
public class TestInterface {

	
	//test recordSample, 2013.03.07
	public static void testRecordSample() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Util.showDLog("testRecordSample() start to run");
				RecordSample sample = new RecordSample();
				sample.test();
				
			}
		}).start();
	}

	
}
