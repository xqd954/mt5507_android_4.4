package com.mediatek.gamekit;

import android.view.SurfaceHolder;

public abstract class GKCmd {
	public static interface Listener {
		public void onDone(GKCmd cmd); 
		public void onStart(GKCmd cmd);
	}

	public GKCmd(GKView sender) {
		this(sender, null);
	}

	public GKCmd(GKView sender, Listener listener) {
		this.sender = sender;
		this.listener = listener;
		this.done = false;
	}
	/**
	 * This specifies which is command sender.
	 */
	public final GKView sender;
	
	private Listener listener;

	void doCmd(GKEngine engine) {
		if (listener != null) {
			listener.onStart(this);
		}
		run(engine);		
		notifyDone();
		if (listener != null) {
			listener.onDone(this);
		}
	}

	// abstract boolean accept(GKEngine engine);
	protected abstract boolean run(GKEngine engine);

	private synchronized void notifyDone() {
		done = true;
		notifyAll();
	}

	public synchronized void waitDone() {
		while (!done) {
			try {
				wait();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}

	private boolean done;
}