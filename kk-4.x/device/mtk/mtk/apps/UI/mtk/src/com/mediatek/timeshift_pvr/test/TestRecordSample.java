/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.test;

import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import com.mediatek.record.IRecordListener;
import com.mediatek.record.IRecordRemoteService;
import com.mediatek.record.RecordCreateFileParameter;
import com.mediatek.record.RecordEvent;
import com.mediatek.record.RecordService;
import com.mediatek.record.RecordStartParameter;
import com.mediatek.record.RecordStatus;

/**
 *
 */
public class TestRecordSample {
	private static final String TAG = "[J]RecordListener";
	private IRecordRemoteService mService;
	private RecordListener mListener;

	public TestRecordSample() {
		super();
		IBinder b = (IBinder) android.os.ServiceManager
				.getService(RecordService.SERVICE_NAME);
		mService = IRecordRemoteService.Stub.asInterface(b);
		mListener = new RecordListener();
		try {
			mService.registerListener(mListener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		Log.d(TAG, "RecordSample\n");
	}

	public void test() {
		RecordStartParameter startRecordData = new RecordStartParameter("test",
				0, 1, 2);
		try {
			int recordHandle = mService.startRecord(startRecordData);
			mService.stopRecord(recordHandle);
			RecordStatus recordStatus = new RecordStatus();
			mService.getRecordStatus(recordHandle, recordStatus);

			RecordCreateFileParameter cfPara = new RecordCreateFileParameter();
			mService.createRecordFile(cfPara);
			mService.deleteRecordFile("test");
			mService.unregisterListener(mListener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}
}

class RecordListener extends IRecordListener.Stub {
	private static final String TAG = "[J]RecordListener";

	public void onEvent(RecordEvent event) {
		Log.d(TAG, "onEvent\n");
		Log.d(TAG, event + "\n");
	}
}
