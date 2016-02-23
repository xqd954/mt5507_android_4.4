package com.mediatek.ui.mmp.gamekit.util;

import java.util.ArrayList;

import com.mediatek.ui.util.MtkLog;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Process;
import android.util.Log;

public class AsyncLoader<Path, Result> {

	private final ArrayList<LoadWork<Result>> mQueue = new ArrayList<LoadWork<Result>>();

	private static AsyncLoader mAsyncLoader = null;

	private Handler mHandler;

	private final static int MSG_ADD_WORK = 1;

	protected static final String TAG = "AsyncLoader";

	private HandlerThread mThread;

	public static AsyncLoader getInstance() {

		if (mAsyncLoader == null) {
			mAsyncLoader = new AsyncLoader("ThunmbnailThread");
		}
		return mAsyncLoader;
	}

	private AsyncLoader(String name) {

		mThread = new HandlerThread(name, Process.THREAD_PRIORITY_DEFAULT);
		mThread.start();
		mHandler = new Handler(mThread.getLooper()) {
			public void handleMessage(Message msg) {

				switch (msg.what) {
				case MSG_ADD_WORK: {
					if (mQueue.size() > 0) {
						LoadWork<Result> workItem = mQueue.remove(0);
						
						  MtkLog.i(TAG, mThread.getName() + ":" +
						  " enter" +
						  " task size:" + mQueue.size()); Long start =
						  System.currentTimeMillis();
						 
						Result result = workItem.load();
						workItem.loaded(result);
						/*
						 * MtkLog.i(TAG, mThread.getName() + ":" +
						 * workItem.mWork.getClass().getName() +
						 * " leave cost time:" + (System.currentTimeMillis() -
						 * start) + " task size:" + mQueue.size());
						 */
					}
					break;
				}
				}

			}
		};
	}

	public void addWork(LoadWork<Result> work) {
		synchronized (mQueue) {
			mQueue.add(work);
			mHandler.sendEmptyMessage(MSG_ADD_WORK);

		}
	}

	public int getTaskSize() {
		return mQueue.size();
	}

	public boolean cancel(final LoadWork<Result> work) {
		synchronized (mQueue) {
			int index = findItem(work);

			if (index >= 0) {
				mQueue.remove(index);
				return true;
			} else {
				return false;
			}
		}
	}

	private int findItem(LoadWork<Result> work) {
		for (int i = 0; i < mQueue.size(); i++) {
			if (mQueue.get(i) == work) {
				return i;
			}
		}

		return -1;
	}

	public void clearQueue() {
		synchronized (mQueue) {
			mQueue.clear();
		}
	}

	public void stop() {

	}

	public void stopLoader() {

	}

	public interface LoadWork<Result> {
		Result load();

		void loaded(Result result);

	}

}