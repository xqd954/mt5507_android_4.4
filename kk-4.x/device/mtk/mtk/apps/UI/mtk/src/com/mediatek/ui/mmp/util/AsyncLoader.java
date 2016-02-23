package com.mediatek.ui.mmp.util;

import java.util.ArrayList;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Process;
import android.util.Log;

public class AsyncLoader<Result> {

	private final ArrayList<WorkItem<Result>> mQueue = new ArrayList<WorkItem<Result>>();

	private static AsyncLoader mAsyncLoader = null;

	private Handler mHandler;

	private final static int MSG_ADD_WORK = 1;

	protected static final String TAG = "AsyncLoader";

	private HandlerThread mThread;
	private boolean mQueueClear=false;
	private LoadWork<Result> infoWork;

	public static AsyncLoader getInstance(int num) {

		if (mAsyncLoader == null) {
			mAsyncLoader = new AsyncLoader("ThunmbnailThread");
		}
		return mAsyncLoader;
	}

	public AsyncLoader(String name) {

		mThread = new HandlerThread(name, Process.THREAD_PRIORITY_DEFAULT);
		mThread.start();
		mHandler = new Handler(mThread.getLooper()) {
			public void handleMessage(Message msg) {

				switch (msg.what) {
				case MSG_ADD_WORK: {
					mQueueClear=false;
					if (mQueue.size() > 0) {
						WorkItem<Result> workItem = mQueue.remove(0);
						
						Long start = System.currentTimeMillis();
						Result result = workItem.mWork.load();
						Log.i(TAG, mThread.getName() + ":"
								+ workItem.mWork.getClass().getName()
								+ " enter" + " task size:" + mQueue.size()+"mQueueClear"+mQueueClear);
						if(!mQueueClear){
							workItem.mWork.loaded(result);
						}
						Log.i(TAG,
								mThread.getName() + ":"
										+ workItem.mWork.getClass().getName()
										+ " leave cost time:"
										+ (System.currentTimeMillis() - start)
										+ " task size:" + mQueue.size()+"mQueueClear"+mQueueClear);
					}
					break;
				}
				}

			}
		};
	}

	public void addWork(LoadWork<Result> work) {
		synchronized (mQueue) {
			WorkItem<Result> w = new WorkItem<Result>(work);
			mQueue.add(w);
			mHandler.sendEmptyMessage(MSG_ADD_WORK);

		}
	}
	
	public void addWorkTop(LoadWork<Result> work) {
		synchronized (mQueue) {
			WorkItem<Result> w = new WorkItem<Result>(work);
			mQueue.add(0,w);
			mHandler.sendEmptyMessage(MSG_ADD_WORK);

		}
	}



	public void addSelectedInfoWork(LoadWork<Result> work){
		
		synchronized (mQueue) {
			if(infoWork != null){
				
				cancel(infoWork);
			}
			infoWork = work;
			
			
			WorkItem<Result> w = new WorkItem<Result>(work);
			mQueue.add(0,w);
			mHandler.sendEmptyMessage(MSG_ADD_WORK);

		}
		
	}

	public int getTaskSize() {
		return mQueue.size();
	}

	public boolean cancel(final LoadWork<Result> work) {
		synchronized (mQueue) {
			try{
				int index = findItem(work);

				if (index >= 0) {
					mQueue.remove(index);
					return true;
				} else {
					return false;
				}
			}catch(Exception e){
				
			}
		}
		return false;
	}

	private int findItem(LoadWork<Result> work) {
		for (int i = 0; i < mQueue.size(); i++) {
			if (mQueue.get(i).mWork.equals(work)) {
				return i;
			}
		}

		return -1;
	}

	public void clearQueue() {
		Log.d(TAG,"clearQueue ~~");
		synchronized (mQueue) {
			mQueue.clear();
			mQueueClear=true;
		}
	}

	// public void quit() {
	// if (null != mThread) {
	// mThread.quit();
	// }
	// mAsyncLoader = null;
	// mThread = null;
	// mHandler = null;
	// mQueue.clear();
	// }

	public void stop() {

	}

	public void stopLoader() {
		// synchronized (mQueue) {
		// mDone = true;
		// mQueue.notifyAll();
		// }
		//
		// if (mExecutor != null) {
		// mExecutor.shutdownNow();
		// mExecutor = null;
		// }
		// mAsyncLoader = null;

		// new Exception().printStackTrace();
	}

	// private class LoadThread implements Runnable {
	// public void run() {
	// Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);
	// Log.i("xy", "Thread.currentThread().getName()====="
	// + Thread.currentThread().getName());
	// Thread.currentThread().setName("AsyncLoaderThread");
	//
	// while (true) {
	// WorkItem<Result> workItem = null;
	//
	// synchronized (mQueue) {
	// if (mDone) {
	// // new Exception().printStackTrace();
	// break;
	// }
	//
	// if (!mQueue.isEmpty()) {
	// workItem = mQueue.remove(0);
	// } else {
	// try {
	// mQueue.wait();
	// } catch (InterruptedException ex) {
	// // ignore the exception
	// }
	//
	// continue;
	// }
	// }
	//
	// final Result result = workItem.mWork.load();
	// workItem.mWork.loaded(result);
	// }
	// }
	// }

	public interface LoadWork<Result> {
		Result load();

		void loaded(Result result);
	}

	private static class WorkItem<Result> {
		LoadWork<Result> mWork;

		WorkItem(LoadWork<Result> work) {
			mWork = work;
		}
	}
}