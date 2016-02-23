package com.mediatek.gamekit;

import java.io.File;
import java.util.ArrayList;

import com.mediatek.gamekit.GKEngine.MessageCmd;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GKView extends SurfaceView implements SurfaceHolder.Callback {

	GKEngine engine;
	// Binding a file to a GKView seems much more easy to use...
	// However, we should also support load file when view is 'active'.
	String blendFileName = null;
	Handler handler = new Handler();

	public GKView(Context context, AttributeSet attrs) {
		this(context, attrs.getAttributeValue(null, "blendPath"));
	}

/*	public GKView(Context context, AttributeSet attrs){
		super(context);
		
		String blendFileName = attrs.getAttributeValue(null, "blendPath");
		
		// Stop doing further work if any blend file name is not correct
		if (blendFileName == null) {
			// We should bind a name with a GKView. If this is
			// null, we would use a empty one or just take an error>>>?

			Log.d("GKView", "Blender file name cannot be null");
			return;
		}
		File file = new File(blendFileName);
		if (!file.exists()) {
			Log.d("GKView", "Blender file name does not exist");
			return;
		}

		engine = GKEngine.getInst();
		this.blendFileName = blendFileName;

		getHolder().addCallback(this);
		
	}*/
	
	public GKView(Context context, String blendFileName) {
		super(context);

		// Stop doing further work if any blend file name is not correct
		if (blendFileName == null) {
			// We should bind a name with a GKView. If this is
			// null, we would use a empty one or just take an error>>>?

			Log.d("GKView", "Blender file name cannot be null");
			return;
		}
		File file = new File(blendFileName);
		if (!file.exists()) {
			Log.d("GKView", "Blender file name does not exist");
			return;
		}

		engine = GKEngine.getInst();
		this.blendFileName = blendFileName;

		getHolder().addCallback(this);

	}

	public static interface LoadListener {
		/**
		 * This will called in user's main thread.
		 */
		public void onLoaded();
	}

	LoadListener loadListener = null;

	public void setLoadListener(LoadListener listener) {
		this.loadListener = listener;
	}

	public void surfaceCreated(SurfaceHolder holder) {
		//
		Log.d("GKView", "Surface created " + this);
		engine.connect(this);
		if (blendFileName != null) {
			if (loadListener != null) {
				engine.load(this, blendFileName, false, new GKCmd.Listener() {
					public void onDone(GKCmd cmd) {
						handler.post(new Runnable() {
							public void run() {
								if (loadListener != null)
									loadListener.onLoaded();
							}
						});
					}

					public void onStart(GKCmd cmd) {

					}
				});
			} else {
				// This will wait
				engine.load(this, blendFileName);
			}
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.d("GKView", "Surface Destroyed " + this);
		engine.disconnect(this);
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		Log.d("GKView", "Surface Changed " + this + "[" + width + ", " + height
				+ "]");
		// engine.sendResize(width, height);
		// Not implemented...
	}

	public static interface MessageListener {
		/**
		 * This will be called in user's main thread.
		 * 
		 * @param from
		 * @param to
		 * @param subject
		 * @param body
		 */
		public void onMessage(String from, String to, String subject,
				String body);
	}

	MessageListener msgListener = null;

	void onMessage(final String from, final String to, final String subject,
			final String body) {
		if (msgListener != null) {
			handler.post(new Runnable() {
				public void run() {
					if (msgListener != null) {
						msgListener.onMessage(from, to, subject, body);
					}
				}
			});
		}
	}

	public void setMessageListener(MessageListener listener) {
		this.msgListener = listener;
	}

	public GKScript createScript(String name, String source) {
		return engine.createScript(this, name, source);
	}

	public void runScript(GKScript script, boolean wait) {
		engine.runScript(this, script, wait);
	}

	public void runScript(String src, boolean wait) {
		engine.runScript(this, src, wait);
	}

	public GKTexture getTexture(GKView sender, String name) {
		return engine.getTexture(this, name);
	}

	public void uploadImage(GKView sender, GKTexture texture, Bitmap bmp,
			boolean wait, GKCmd.Listener listener) {
		engine.uploadImage(this, texture, bmp, wait, listener);
	}

	/**
	 * Upload image to texture and wait for loading.
	 * 
	 * @param sender
	 * @param texture
	 * @param bmp
	 */
	public void uploadImage(GKView sender, GKTexture texture, Bitmap bmp) {
		uploadImage(sender, texture, bmp, true, null);
	}

	public void sendMessage(GKView sender, String from, String to,
			String subject, String body, boolean wait) {
		engine.sendMessage(this, from, to, subject, body, wait);
	}

	// Key and mouse event processing currently are being waited. :)
	// However, this is just for a 'uniform' feeling in Main thread of
	// event dispatching. We can also use none-waiting ...
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		int action = event.getAction();
		int uniChar = event.getUnicodeChar();
		engine.sendKey(this, action, uniChar, keyCode, true);
		return true;
	}

	public boolean onKeyUp(int keyCode, KeyEvent event) {
		int action = event.getAction();
		int uniChar = event.getUnicodeChar();
		engine.sendKey(this, action, uniChar, keyCode, true);
		return true;
	}

	public boolean onTouchEvent(MotionEvent event) {
		int action = event.getAction();
		float x = event.getX();
		float y = event.getY();
		int buttonId = event.getButtonState();

		engine.sendMouse(this, action, x, y, buttonId, true);
		return true;
	}

}
