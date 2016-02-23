package com.mediatek.ui.vssdemo;

import java.io.IOException;

import java.io.InputStream;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import android.os.SystemProperties;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.mediatek.ui.nav.NavIntegration;
import com.opengl.jni.GLJniLib;

import android.R;
import android.opengl.GLES10;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.content.*;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class GLRenderer implements GLSurfaceView.Renderer {

	private int TextureId[] = new int[2];

	private InitBuffer initbuf;

	private int gWidht, gheight;

	private float angle = 0;
	private GL10 mGl;
	Context mContext;

	// ---------------
	private float N = 10;
	private float mPoints[][][] = new float[10][10][3];

	public FloatBuffer mRectBuffer;
	public FloatBuffer mRectTexBuffer;

	private int wiggle_count = 0; /* Counter Used To Control How Fast Flag Waves */
	private boolean bOnshow = true;
	private float xrot = 0;
	private float yrot = 0;
	private float zrot = 0;
	private int TextureWidth = 0;
	private int TextureHeight = 0;
	private int mSceneId = 0;
	private int iIdleCount = 30;
	private int iIdleCount_Timing = 50;
	public float zValue = 0;
	public boolean b_is_changesource = false;

	public GLRenderer(Context context) {
		mContext = context;
	}

	public static final int FRAMERATE_SAMPLEINTERVAL_MS = 1000;
	private long _frameCount = 0;
	private float _fps = 0;
	private long _timeLastSample;
    private boolean bFirstEnter = false;
	private void doFps() {
		_frameCount++;

		long now = System.currentTimeMillis();
		long delta = now - _timeLastSample;
		if (delta >= FRAMERATE_SAMPLEINTERVAL_MS) {
			_fps = _frameCount / (delta / 1000f);
			if (SystemProperties.getInt("mtk.vss.demo.fps", 0) == 1) {
				Log.e("Opengl", "the current fps is :" + _fps);
			}
			_timeLastSample = now;
			_frameCount = 0;
		}
	}

	public void setChangeSourceStatus(boolean b) {
		Log.e("Video_test", "***********************************************");
		this.b_is_changesource = b;
	}

	public void onDrawFrame(GL10 gl) {
		if (bOnshow) {
			// TODO Auto-generated method stub
			int tempH = GLJniLib.nativeGetHeight();
			int tempW = GLJniLib.nativeGetWidth();
			//Log.i("Opengl", "height is : " + tempH + " width is :" + tempW);
			if (tempH != this.TextureHeight || tempW != this.TextureWidth || bFirstEnter) {
				if (iIdleCount > 0) {
					iIdleCount--;
					if (iIdleCount == 10) {
						iIdleCount --;
						GLJniLib.nativeTimingChange();
						gl.glGetError();
						gl.glDeleteTextures(2, TextureId, 0);

					}else if(iIdleCount ==0 ){
						iIdleCount = 30;
						b_is_changesource = true;
						TextureHeight = tempH;
                        TextureWidth = tempW;
                        if (bFirstEnter){
                            bFirstEnter = false;
                        }

					}else if(iIdleCount <= 10){
						gl.glGetError();
                		gl.glDeleteTextures(2, TextureId, 0);
               			if (NavIntegration.getInstance(mContext)
                        			.iCurrentInputSourceHasSignal()) {
                    	GLJniLib.nativeUpdateTexture(TextureId[0]);
                    	if (SystemProperties.getInt("mtk.vss.demo.debug", 0) == 1) {
                     		Log.i("*************","height is : " + tempH +" width is :" + tempW );
                    	}
                		} else {
                    		Log.e("video_test", "Current Input Source has no signal");
                		}
					}
				}
				if (SystemProperties.getInt("mtk.vss.demo.debug", 0) == 1) {
					Log.e("Opengl","Waiting for the timing to stable" + iIdleCount);
				}
				// || b_is_changesource) {
				// // if(b_is_changesource){
				// if (iIdleCount > 0) {
				// iIdleCount--;
				// if (iIdleCount == 50) {
				// b_is_changesource = true;
				// GLJniLib.nativeTimingChange();
				// TextureHeight = tempH;
				// TextureWidth = tempW;
				// }
				// if (iIdleCount < 25) {
				// GLJniLib.nativeUpdateTexture(TextureId[0]);
				// }
				// } else {
				// b_is_changesource = false;
				// iIdleCount = 300;
				// iIdleCount_Timing = 0;
				// }
				// GLES10.glClear(GLES10.GL_DEPTH_BUFFER_BIT
				// | GLES10.GL_COLOR_BUFFER_BIT);
				// } else if (iIdleCount_Timing > 0) {
				// iIdleCount_Timing--;
				// gl.glGetError();
				// gl.glDeleteTextures(2, TextureId, 0);
			} else {
				gl.glGetError();
				gl.glDeleteTextures(2, TextureId, 0);
				if (NavIntegration.getInstance(mContext)
						.iCurrentInputSourceHasSignal()) {
					GLJniLib.nativeUpdateTexture(TextureId[0]);
					if (SystemProperties.getInt("mtk.vss.demo.debug", 0) == 1) {
					 Log.i("############","height is : " + tempH +" width is :" + tempW );
					}
				} else {
					Log.e("video_test", "Current Input Source has no signal");
				}
				GLES10.glClear(GLES10.GL_DEPTH_BUFFER_BIT
						| GLES10.GL_COLOR_BUFFER_BIT);
				GLES10.glClearColor(0, 0, 0, 0);

				GLES10.glMatrixMode(GLES10.GL_PROJECTION);
				GLES10.glLoadIdentity();
				// GLES10.gluPerspective(45, (float)gWidht / gheight, 0.5f,
				// 150);
				GLU.gluPerspective(gl, 45.0f, (float) gWidht / (float) gheight,
						0.1f, 5000.0f);
				// GLES10.glOrthof(-gWidht/2,gWidht/2, -gheight/2,gheight/2,
				// -1.0f,
				// 1.0f);
				GLU.gluLookAt(gl, 0, 0, 5 + this.zValue, 0, 0, 0, 0, 1, 0);

				GLES10.glMatrixMode(GLES10.GL_MODELVIEW);

				GLES10.glLoadIdentity();

				// GLES10.glDisable(GLES10.GL_DEPTH_TEST);
				GLES10.glEnable(GLES10.GL_TEXTURE_2D);
				GLES10.glBindTexture(GLES10.GL_TEXTURE_2D, TextureId[0]);
				GLES10.glEnableClientState(GLES10.GL_VERTEX_ARRAY);
				GLES10.glEnableClientState(GLES10.GL_TEXTURE_COORD_ARRAY);
				if (mSceneId == 1) {
					GLES10.glVertexPointer(3, GLES10.GL_FLOAT, 0,
							initbuf.mRectBuffer);

					GLES10.glTexCoordPointer(2, GLES10.GL_FLOAT, 0,
							initbuf.mRectTexBuffer);

					GLES10.glEnableClientState(GLES10.GL_VERTEX_ARRAY);
					GLES10.glEnableClientState(GLES10.GL_TEXTURE_COORD_ARRAY);

					GLES10.glRotatef(angle, 0, 1, 0);
					angle += 1.0f;
					// glDrawArrays(GL_TRIANGLES, 0, 6);
					GLES10.glDrawArrays(GLES10.GL_TRIANGLE_FAN, 0, 4);
				} else if (mSceneId == 0) {
					xrot += 0.3f; /* Increase The X Rotation Variable */
					yrot += 0.2f; /* Increase The Y Rotation Variable */
					zrot += 0.4f; /* Increase The Z Rotation Variable */

					GLES10.glRotatef(xrot, 1.0f, 0.0f, 0.0f); /*
															 * Rotate On The X
															 * Axis
															 */
					GLES10.glRotatef(yrot, 0.0f, 1.0f, 0.0f); /*
															 * Rotate On The Y
															 * Axis
															 */
					GLES10.glRotatef(zrot, 0.0f, 0.0f, 1.0f); /*
															 * Rotate On The Z
															 * Axis
															 */

					GLES10.glScalef(0.2f, 0.2f, 0.2f);
					anim_flag();
					draw_flag();
				}

				GLES10.glDisableClientState(GLES10.GL_VERTEX_ARRAY);
				GLES10.glDisableClientState(GLES10.GL_TEXTURE_COORD_ARRAY);
			}

			doFps();
		}
	}

	public void onDraw() {
		this.bOnshow = true;
		this.TextureHeight = 0;
		this.TextureWidth = 0;
		bFirstEnter = true;
	}

	public void onPause() {
		this.bOnshow = false;
		mGl.glGetError();
		mGl.glDeleteTextures(2, TextureId, 0);
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		// TODO Auto-generated method stub
		Log.e("Opengl", "onSurfaceChanged");
		// GLJniLib.nativeResize(width, height);
		gl.glViewport(0, 0, width, height);
		float ratio = (float) width / height;
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();
		gl.glFrustumf(-ratio, ratio, -1, 1, 1, 100);
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		gWidht = width;
		gheight = height;
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		// TODO Auto-generated method stub
		mGl = gl;
		Log.e("Opengl", "onSurfaceCreated");
		GLES10.glGenTextures(2, TextureId, 0);
		GLJniLib.nativeInit();

		initbuf = new InitBuffer();

		GLES10.glBindTexture(GL10.GL_TEXTURE_2D, TextureId[0]);

		GLES10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,
				GL10.GL_NEAREST);
		GLES10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,
				GL10.GL_LINEAR);
		GLES10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,
				GL10.GL_CLAMP_TO_EDGE);
		GLES10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,
				GL10.GL_CLAMP_TO_EDGE);
		init_flag();

		TextureWidth = GLJniLib.nativeGetWidth();
		TextureHeight = GLJniLib.nativeGetHeight();
	}

	int init_flag() {
		float a = 5.0f * N / 45.0f;
		float b = 4.5f * N / 45.0f;
		int x, y;
		for (x = 0; x < N; x++) { /* Loop Through The X Plane */
			for (y = 0; y < N; y++) { /* Loop Through The Y Plane */
				/* Apply The Wave To Our Mesh */
				mPoints[x][y][0] = (float) ((x / a) - b);
				mPoints[x][y][1] = (float) ((y / a) - b);
				mPoints[x][y][2] = (float) (Math
						.sin((((x / a) * 40.0f) / 360.0f) * 3.141592654f * 2.0f));
			}
		}

		ByteBuffer rvbb = ByteBuffer.allocateDirect(10 * 10 * 4 * 3 * 4);
		rvbb.order(ByteOrder.nativeOrder());
		mRectBuffer = rvbb.asFloatBuffer();
		mRectBuffer.position(0);

		ByteBuffer tvbb = ByteBuffer.allocateDirect(10 * 10 * 4 * 2 * 4);
		tvbb.order(ByteOrder.nativeOrder());
		mRectTexBuffer = tvbb.asFloatBuffer();
		mRectTexBuffer.position(0);

		return 0;
	}

	int anim_flag() {

		wiggle_count++; /* Increase The Counter */

		float a = 5.0f * N / 45.0f;
		float b = 4.5f * N / 45.0f;
		float speed = 0.1f;
		int x, y;
		for (x = 0; x < N; x++) { /* Loop Through The X Plane */
			for (y = 0; y < N; y++) { /* Loop Through The Y Plane */
				/* Apply The Wave To Our Mesh */
				// data->points[x][y][0] = (GLfloat)((x / a) - b);
				// data->points[x][y][1] = (GLfloat)((y / a) - b);
				mPoints[x][y][2] = (float) (Math
						.sin((((x / a) * 40.0f) / 360.0f) * 3.141592654f * 2.0f
								+ wiggle_count * speed));
			}
		}

		return 0;
	}

	int draw_flag() {

		float f_x, f_y, f_xb, f_yb; /* Used To Break The Flag Into Tiny Quads */
		int x, y; /* Loop Variables */

		/* Loop Through The X Plane 0-N-1 (N Points) */
		for (x = 0; x < N - 1; x++) {
			/* Loop Through The Y Plane 0-N-1 (N Points) */
			for (y = 0; y < N - 1; y++) {
				f_x = (float) x / (N - 1); /* Create A Floating Point X Value */
				f_y = (float) y / (N - 1); /* Create A Floating Point Y Value */
				f_xb = (float) (x + 1) / (N - 1); /*
												 * Create A Floating Point Y
												 * Value+0.0227f
												 */
				f_yb = (float) (y + 1) / (N - 1); /*
												 * Create A Floating Point Y
												 * Value+0.0227f
												 */

				float vertices[] = new float[12]; /* Vertices array */
				float texcoords[] = new float[8]; /* Texture coordinates array */

				/* First Texture Coordinate (Bottom Left) */
				texcoords[0] = f_x;
				texcoords[1] = f_y;
				vertices[0] = mPoints[x][y][0];
				vertices[1] = mPoints[x][y][1];
				vertices[2] = mPoints[x][y][2];

				/* Second Texture Coordinate (Top Left) */
				texcoords[2] = f_x;
				texcoords[3] = f_yb;
				vertices[3] = mPoints[x][y + 1][0];
				vertices[4] = mPoints[x][y + 1][1];
				vertices[5] = mPoints[x][y + 1][2];

				/* Fourth Texture Coordinate (Bottom Right) */
				texcoords[4] = f_xb;
				texcoords[5] = f_y;
				vertices[6] = mPoints[x + 1][y][0];
				vertices[7] = mPoints[x + 1][y][1];
				vertices[8] = mPoints[x + 1][y][2];

				/* Third Texture Coordinate (Top Right) */
				texcoords[6] = f_xb;
				texcoords[7] = f_yb;
				vertices[9] = mPoints[x + 1][y + 1][0];
				vertices[10] = mPoints[x + 1][y + 1][1];
				vertices[11] = mPoints[x + 1][y + 1][2];

				mRectBuffer.clear();
				mRectBuffer.put(vertices);
				mRectBuffer.rewind();

				mRectTexBuffer.clear();
				mRectTexBuffer.put(texcoords);
				mRectTexBuffer.rewind();

				GLES10.glVertexPointer(3, GLES10.GL_FLOAT, 0, mRectBuffer);
				GLES10.glTexCoordPointer(2, GLES10.GL_FLOAT, 0, mRectTexBuffer);

				/* Draw one textured quad using two stripped triangles */
				GLES10.glDrawArrays(GLES10.GL_TRIANGLE_STRIP, 0, 4);
			}
		}

		return 0;
	}

	public void SetNextState(int Id) {
		mSceneId = Id;
	}

	// InitBuffer class
	class InitBuffer {
		public FloatBuffer mRectBuffer;
		public FloatBuffer mRectTexBuffer;

		public InitBuffer() {
			// int one = 0x10000;
			float one = 1.4f;

			float rectangle[] = { -one, -one, 0, one, -one, 0, one, one, 0,
					-one, one, 0, };
			float rectTexcoords[] = { 0, 1, 1, 1, 1, 0, 0, 0, };

			ByteBuffer rvbb = ByteBuffer.allocateDirect(rectangle.length * 4);
			rvbb.order(ByteOrder.nativeOrder());
			mRectBuffer = rvbb.asFloatBuffer();
			mRectBuffer.put(rectangle);
			mRectBuffer.position(0);

			ByteBuffer tvbb = ByteBuffer
					.allocateDirect(rectTexcoords.length * 4);
			tvbb.order(ByteOrder.nativeOrder());
			mRectTexBuffer = tvbb.asFloatBuffer();
			mRectTexBuffer.put(rectTexcoords);
			mRectTexBuffer.position(0);
		}
	}

	public boolean setKeyEvent(KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_DOWN) {

			Log.e("log", "event.getAction() == KeyEvent.ACTION_DOWN");
			switch (event.getKeyCode()) {
			case KeyEvent.KEYCODE_0:
				SetNextState(0);
				return true;
			case KeyEvent.KEYCODE_1:
				SetNextState(1);
				return true;
			case KeyEvent.KEYCODE_DPAD_DOWN:
				zValue = zValue + 0.1f;
				return true;
			case KeyEvent.KEYCODE_DPAD_UP:
				zValue = zValue - 0.1f;
				return true;
			}
		}

		return false;
	}
}


