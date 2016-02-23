package com.mediatek.gamekit;

import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.opengl.GLES20;
import android.util.Log;

/**
 * 
 * GKEngine is designed as a single "game engine" in a process, but we may have
 * several GKViews in a process. So we make GKEnine as a 'server' which provides
 * native engine support. Once a GKView is connected as a 'client', existing
 * connected view is disconnected automatically.
 * 
 * 
 * 
 * @author chun
 * 
 */
class GKEngine implements Runnable {
	final static String TAG = "GKEngine";
	final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

	static {
		System.loadLibrary("ogrekit");
	}

	private static GKEngine inst;

	/**
	 * Thread for engine.
	 */
	private Thread gkThread;

	/**
	 * Current connected view.
	 */
	private GKView curView = null;

	/**
	 * Get engine instance.
	 * 
	 * @return
	 */
	public static synchronized GKEngine getInst() {
		if (inst == null) {
			inst = new GKEngine();
		}
		return inst;
	}

	protected GKEngine() {

		Log.d("OgreKit", "Create GKEngine");
		gkThread = new Thread(this);
		gkThread.setName("GKThread");
		gkThread.start();
	}

	public static final int STATE_IDLE = 0;
	public static final int STATE_CONNECTED = 1;
	public static final int STATE_RUNNING = 2;
	int state = STATE_IDLE;

	/**
	 * Get engine state, should be STATE_IDLE, STATE_CONNECTED or STATE_RUNNING.
	 * STATE_IDLE: Not connected to any GKView instance. STATE_CONNECTED:
	 * Connected STATE_RUNNING: A blend file is loaded for current connected
	 * view, engine is running to render and perform physics frame by frame. Any
	 * user defined operations can only be runned in this state.
	 * 
	 * @return
	 */
	int getState() {
		return state;
	}

	protected void toState(int state) {
		Log.d("OgreKit", "State from " + this.state + " to " + state);
		this.state = state;
	}

	BlockingQueue<GKCmd> queue = new ArrayBlockingQueue<GKCmd>(32);

	/**
	 * Post a command.
	 * 
	 * @param cmd
	 * @param wait
	 */
	void post(GKCmd cmd, boolean wait) {
//		if(canOmmit) {
//			queue.offer(cmd); 
//		} else {
			try {
				queue.add(cmd);
			} catch (IllegalStateException e) {
				e.printStackTrace();
				GKCmd c;
				while((c =queue.poll()) != null){
					Log.e(TAG, c.toString());
				}
			}
//		}
		if (wait) {
			cmd.waitDone();
		}
	}

	/**
	 * Post a command without waiting and not important.
	 * 
	 * @param cmd
	 */
	void post(GKCmd cmd) {
		post(cmd, false);
	}

	public void run() {

		GKCmd cmd;
		try {
			while (true /* FIX */) {
				cmd = queue.take();
				cmd.doCmd(this);
			}
		} catch (InterruptedException e) {
			// If this is interrupted , make sure all resources are cleaned..
			e.printStackTrace();
			if (state != STATE_IDLE /* && curView != null */) {
				new DisconnectCmd(curView).doCmd(this);
			}
		}
	}

	private EGLDisplay eglDisplay;
	private EGLContext eglContext;
	private EGLSurface eglSurface;

	public final static int EGL_OPENGL_ES2_BIT = 4;

	// Try this list till success
	static int[][] configs = new int[][] {
			new int[] { EGL10.EGL_RED_SIZE, 8, EGL10.EGL_BLUE_SIZE, 8,
					EGL10.EGL_GREEN_SIZE, 8, EGL10.EGL_ALPHA_SIZE, 8,
					EGL10.EGL_SAMPLE_BUFFERS, 1, EGL10.EGL_SAMPLES, 4,
					EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL10.EGL_NONE },
			new int[] { EGL10.EGL_RED_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
					EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_ALPHA_SIZE, 4,
					EGL10.EGL_SAMPLE_BUFFERS, 1, EGL10.EGL_SAMPLES, 4,
					EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL10.EGL_NONE },
			new int[] { EGL10.EGL_RED_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
					EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_ALPHA_SIZE, 4,
					EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL10.EGL_NONE },
			new int[] { EGL10.EGL_RED_SIZE, 5, EGL10.EGL_BLUE_SIZE, 6,
					EGL10.EGL_GREEN_SIZE, 5, EGL10.EGL_RENDERABLE_TYPE,
					EGL_OPENGL_ES2_BIT, EGL10.EGL_NONE },
			new int[] { EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL10.EGL_NONE }

	};

	class ConnectCmd extends GKCmd {

		public ConnectCmd(GKView sender) {
			super(sender);
		}

		protected boolean run(GKEngine engine) {
			int state = engine.getState();
			if (state == GKEngine.STATE_RUNNING
					|| state == GKEngine.STATE_CONNECTED) {
				GKCmd exitCmd = new DisconnectCmd(curView);
				exitCmd.doCmd(engine);
			}

			EGL10 egl = (EGL10) EGLContext.getEGL();
			eglDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
			int version[] = new int[2];
			egl.eglInitialize(eglDisplay, version);
			
			for (int i = 0; i < configs.length; i++) {
				EGLConfig cfgs[] = new EGLConfig[1];
				int[] num = new int[]{1};
				if (egl.eglChooseConfig(eglDisplay, configs[i], cfgs, 1, num)) { 
					//Found one
					Log.i(TAG, "Found config \n"+ cfgs[0]);
					int[] eglCtxAttr = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
					eglContext = egl.eglCreateContext(
							eglDisplay, 
							cfgs[0],
							EGL10.EGL_NO_CONTEXT, 
							eglCtxAttr);
					eglSurface = egl.eglCreateWindowSurface(
							eglDisplay, 
							cfgs[0],
							sender, null);
					egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

					String glVer = GLES20.glGetString(GLES20.GL_VERSION);
					Log.d(TAG, "EGL prepared, err: " 
							+ egl.eglGetError()
							+ " GL version : " + glVer);

					Rect rect = sender.getHolder().getSurfaceFrame();
					_createNative(rect.width(), rect.height());

					toState(STATE_CONNECTED);
					curView = sender;

					Log.i(TAG, "Connected a new GKView " + sender);
					return true;
				}
				 
			}
			
			Log.e(TAG, "Cannot find a config");
			return false;
		}
	}

	/**
	 * This must be called in waiting mode
	 * 
	 * @param holder
	 */
	void connect(GKView sender) {
		GKCmd cmd = new ConnectCmd(sender);
		post(cmd, true);
	}

	class DisconnectCmd extends GKCmd {

		public DisconnectCmd(GKView sender) {
			super(sender);
		}

		public boolean run(GKEngine engine) {
			if (engine.getState() == STATE_IDLE) {
				// nothing todo
				return false;
			}
			if (sender != curView) {
				Log.d(TAG, "Command of " + getClass()
						+ " not from current view, ignore");
				return false;
			}

			_destroyNative();

			EGL10 egl = (EGL10) EGLContext.getEGL();

			egl.eglDestroySurface(eglDisplay, eglSurface);
			egl.eglDestroyContext(eglDisplay, eglContext);

			curView = null;
			toState(STATE_IDLE);
			Log.i(TAG, "Disconnected  GKView " + sender);
			return true;
		}
	}

	/**
	 * This must be called in waiting mode
	 * 
	 * @param holder
	 */
	void disconnect(GKView sender) {
		GKCmd cmd = new DisconnectCmd(sender);
		post(cmd, true);
	}

	class LoadCmd extends GKCmd {
		final String fileName;

		public LoadCmd(GKView sender, String fileName, GKCmd.Listener listener) {
			super(sender, listener);
			this.fileName = fileName;

		}

		public boolean run(GKEngine engine) {
			int state = engine.getState();
			if (state == STATE_IDLE) {
				Log.d(TAG, " Unable to load file " + fileName
						+ " when engine is idle for " + sender);
				return false;
			}
			Log.i(TAG, "Start loading " + fileName + " for GKView " + sender);

			if (sender != curView) {
				Log.d(TAG, "Command of " + getClass()
						+ " not from current view, ignore");
				return false;
			}
			EGL10 egl = (EGL10) EGLContext.getEGL();
			egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
			_loadBlendFile(fileName);
			toState(STATE_RUNNING);
			post(new RenderCmd(sender), false);
			Log.i(TAG, "Loaded " + fileName + " for GKView " + sender);
			return true;
		}

	}

	/**
	 * You cannot access directly.
	 * 
	 * @param fileName
	 * @param wait
	 * @param listener
	 */
	void load(GKView sender, String fileName, boolean wait,
			GKCmd.Listener listener) {
		GKCmd cmd = new LoadCmd(sender, fileName, listener);
		post(cmd, wait);
	}

	void load(GKView sender, String fileName) {
		GKCmd cmd = new LoadCmd(sender, fileName, null);
		post(cmd, true);
	}

	class RenderCmd extends GKCmd {

		public RenderCmd(GKView sender) {
			super(sender);
		}

		@Override
		protected boolean run(GKEngine engine) {
			int state = engine.getState();
			if (state == GKEngine.STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view , ignore");
					return false;
				}
				EGL10 egl = (EGL10) EGLContext.getEGL();

				_render();
				// Swap is called in native ?
				egl.eglSwapBuffers(eglDisplay, eglSurface);

				// Re-post
				post(this, false);

				return true;
			} else {
				Log.d(TAG, "Cannot render in state " + state);
				return false;
			}
		}

	}

	static int luaNameSeq = 0;

	class CreateScriptCmd extends GKCmd {
		final String name;
		final String source;

		public CreateScriptCmd(GKView sender, String source) {
			super(sender);
			this.name = "Lua-Created-By-Java-" + luaNameSeq++;
			this.source = source;
		}

		public CreateScriptCmd(GKView sender, String name, String source) {
			super(sender);
			this.name = name;
			this.source = source;
		}

		@Override
		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				script = new GKScript(GKEngine._createScript(
						"Lua-Created-By-Java-" + luaNameSeq++, source)); 
				return true;
			} else {
				Log.d(TAG, "Cannot create script in state " + state);
				return false;
			}
		}

		private GKScript script = null;

		public GKScript getScript() {
			return script;
		}

	}

	// public GKScript sendCreateScript( String src) {
	// CreateScriptCmd cmd = new CreateScriptCmd(src);
	// send(cmd);
	// return cmd.getScript();
	// }
	protected boolean isInGKThread() {
		return gkThread == Thread.currentThread();
	}

	/**
	 * Create a script with name and its source on Lua.
	 * 
	 * @param sender
	 * @param name
	 * @param source
	 * @return
	 */
	GKScript createScript(GKView sender, String name, String source) {

		CreateScriptCmd cmd = new CreateScriptCmd(sender, name, source);
		if (!isInGKThread()) {
			post(cmd, true);
		} else {
			cmd.doCmd(this);
		}
		return cmd.getScript();

	}

	class RunScriptCmd extends GKCmd {
		final GKScript script;

		public RunScriptCmd(GKView sender, GKScript script) {
			super(sender);
			this.script = script;
			// TODO Auto-generated constructor stub
		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				GKEngine._runScript(script.handle); 
				return true;
			} else {
				Log.d(TAG, "Cannot run script in state " + state);
				return false;
			}

		}

	}

	/**
	 * Run created script.
	 * 
	 * @param sender
	 * @param script
	 * @param wait
	 */
	void runScript(GKView sender, GKScript script, boolean wait) {
		RunScriptCmd cmd = new RunScriptCmd(sender, script);
		if (!isInGKThread()) {
			post(cmd, wait);
		} else {
			cmd.doCmd(this);
		}
	}

	class CompileAndExecCmd extends GKCmd {
		final String source;

		public CompileAndExecCmd(GKView sender, String source) {
			super(sender);
			this.source = source;
		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				int handle = GKEngine._createScript(
						"Run-Imediately-Script-Lua", source);
				GKEngine._runScript(handle);
				GKEngine._deleteScript(handle); 
				return true;
			} else {
				Log.d(TAG, "Cannot create and run script in state " + state);
				return false;
			}
		}
	}

	/**
	 * Run script directly specified by source on Lua. Note, this contains
	 * compiling script, so it is better to use runScript with GKScript instance
	 * when it is called frequently.
	 * 
	 * @param sender
	 * @param src
	 * @param wait
	 */
	void runScript(GKView sender, String src, boolean wait) {
		CompileAndExecCmd cmd = new CompileAndExecCmd(sender, src);
		if (!isInGKThread()) {
			post(cmd, wait);
		} else {
			cmd.doCmd(this);
		}
	}

	class GetTextureCmd extends GKCmd {
		final String name;

		public GetTextureCmd(GKView sender, String name) {
			super(sender);
			this.name = name;
		}

		GKTexture texture;

		public GKTexture getTexture() {
			return texture;
		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				texture = _getTexture(name);
				return true;
			} else {
				Log.d(TAG, "Cannot create and run script in state " + state);
				return false;
			}
		}

	}

	/**
	 * Get texture by name. This will wait for instance created if current
	 * thread is not in rendering thread. If caller is in rendering thread, this
	 * returns instance immediatly.
	 * 
	 * @param sender
	 * @param name
	 * @return
	 */
	GKTexture getTexture(GKView sender, String name) {
		GetTextureCmd cmd = new GetTextureCmd(sender, name);
		if (!isInGKThread()) {
			post(cmd, true);
		} else {
			cmd.doCmd(this);
		}
		return cmd.getTexture();
	}

	class UploadImageCmd extends GKCmd {
		final Bitmap bmp;
		final GKTexture texture;

		UploadImageCmd(GKView sender, GKTexture texture, Bitmap bmp,
				GKCmd.Listener listener) {
			super(sender, listener);
			this.texture = texture;
			this.bmp = bmp;

		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				texture.upload(bmp);
				return true;
			} else {
				Log.d(TAG, "Cannot upload image in state " + state);
				return false;
			}
		}

	}

	/**
	 * Upload image to texture.
	 * 
	 * @param sender
	 * @param texture
	 * @param bmp
	 * @param wait
	 * @param listener
	 */
	 void uploadImage(GKView sender, GKTexture texture, Bitmap bmp,
			boolean wait, GKCmd.Listener listener) {
		UploadImageCmd cmd = new UploadImageCmd(sender, texture, bmp, listener);
		if (!isInGKThread()) {
			post(cmd, wait);
		} else {
			cmd.doCmd(this);
		}
	}
	

	class KeyCmd extends GKCmd {
		final int keyCode;
		final int uniChar;
		final int action;

		public KeyCmd(GKView sender, int action, int uniChar, int keyCode) {
			super(sender);
			this.action = action;
			this.uniChar = uniChar;
			this.keyCode = keyCode;
		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				_sendKey(action, uniChar, keyCode);
				return true;
			} else {
				Log.d(TAG, "Cannot process key in state " + state);
				return false;
			}
		}

	}

	/**
	 * 
	 * @param sender
	 * @param action
	 * @param uniChar
	 * @param keyCode
	 * @param wait
	 */
	public void sendKey(GKView sender, int action, int uniChar, int keyCode,
			boolean wait) {
		post(new KeyCmd(sender, action, uniChar, keyCode), wait);
	}

	class MouseCmd extends GKCmd {
		final int buttonId;
		final int action;
		final float x;
		final float y;

		public MouseCmd(GKView sender, int action, float x, float y,
				int buttonId) {
			super(sender);
			this.action = action;
			this.x = x;
			this.y = y;
			this.buttonId = buttonId;
		}

		protected boolean run(GKEngine engine) {
			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				_sendMouse(action, x, y, buttonId);
				return true;
			} else {
				Log.d(TAG, "Cannot process mouse in state " + state);
				return false;
			}
		}

	}

	/**
	 * Send mouse event
	 * 
	 * @param sender
	 * @param action
	 * @param x
	 * @param y
	 * @param buttonId
	 * @param wait
	 */
	public void sendMouse(GKView sender, int action, float x, float y,
			int buttonId, boolean wait) {
		post(new MouseCmd(sender, action, x, y, buttonId), wait);
	}

	public static interface MessageListener {
		public void onMessage(String from, String to, String subject,
				String body);
	}

	 

	// called by native
	static void onMessage(String from, String to, String subject, String body) { 
		GKView view = getInst().curView;
		if(view != null) {
			view.onMessage(from, to, subject, body);
		} else {
			Log.e(TAG, "Message without any view connected");
		}
	}

	class MessageCmd extends GKCmd {
		final String from;
		final String to;
		final String subject;
		final String body;

		public MessageCmd(GKView sender, String from, String to,
				String subject, String body) {
			super(sender);
			this.from = from;
			this.to = to;
			this.subject = subject;
			this.body = body;
		}

		protected boolean run(GKEngine engine) {

			if (state == STATE_RUNNING) {
				if (sender != curView) {
					Log.d("OgreKit", "Command of " + getClass()
							+ " not from current view, ignore");
					return false;
				}
				_sendMessage(from, to, subject, body);
				return true;
			} else {
				Log.d(TAG, "Cannot send message in state " + state);
				return false;
			}
		}

	}

	/**
	 * Send message to engine.
	 * 
	 * @param sender
	 * @param from
	 * @param to
	 * @param subject
	 * @param body
	 * @param wait
	 */
	public void sendMessage(GKView sender, String from, String to,
			String subject, String body, boolean wait) {
		post(new MessageCmd(sender, from, to, subject, body), wait);
	}

	// native functions

	native static void _createNative(int w, int h);

	native static void _destroyNative();

	native static void _loadBlendFile(String file);

	native static void _render();

	native static void _resize(int w, int h);

	native static int _createScript(String name, String src);

	native static void _runScript(int handle);

	native static void _deleteScript(int handle);

	native static GKTexture _getTexture(String name);

	native static void _sendKey(int action, int uniChar, int keyCode);

	native static void _sendMouse(int action, float x, float y, int buttonId);

	native static void _sendMessage(String from, String to, String subject,
			String body);
}
