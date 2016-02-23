/**
 * use for connect user interface and common logic
 */
package com.mediatek.ui.nav.util;

import android.content.Context;
import android.graphics.RectF;
import android.view.View;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVCommon;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVStorage;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVInputCommon;
import com.mediatek.tvcommon.TVOptionRange;

public class NavIntegrationZoom {
	private static final String TAG = "NavIntegration";

	private static NavIntegrationZoom instance;
	private static TVContent mTv;
	private static TVConfigurer TvCf;
	private TVInputManager mTVInputManager;
	public TVOptionRange<Integer> dpmsOption;
	private TVOutput mTvOutput;

	public static final int ZOOM_UP = 0;
	public static final int ZOOM_DOWN = 1;
	public static final int ZOOM_LEFT = 2;
	public static final int ZOOM_RIGHT = 3;
	public static final int ZOOM_1 = 1;
	public static final int ZOOM_2 = 2;
	public static final int ZOOM_0_5 = 3;
	private TVStorage mTvStorage;
	private int current_zoom = ZOOM_1;

	/**
	 * when power on TV first time, should start setup wizard
	 */

	private NavIntegrationZoom(Context context) {
		mTv = TVContent.getInstance(context);
		TvCf=mTv.getConfigurer();
		mTVInputManager = mTv.getInputManager();

	}

	public static synchronized NavIntegrationZoom getInstance(Context context) {
		if (instance == null) {
			instance = new NavIntegrationZoom(context);
		}
		return instance;
	}

	public interface iEffectChangeZoomListener {
		void effectChanged(View mView, int value);
	}

	/**
	 * zoom
	 */

	/**
	 * zoom is useful in different screen mode
	 */
	public boolean screenModeZoomShow() {
		TVOptionRange<Integer> mOption = (TVOptionRange<Integer>) TvCf
				.getOption(ConfigType.CFG_SCREEN_MODE);
		 if(null != mOption){
		 int screenMode = mOption.get();
		 if (screenMode == ConfigType.SCREEN_MODE_DOT_BY_DOT
				|| screenMode == ConfigType.SCREEN_MODE_PAN_SCAN) {
			return true;
		 }
		}
			return false;
	}

	/**
	 * 
	 * @param moveType
	 *            ZOOM_UP,ZOOM_DOWN,ZOOM_LEFT,ZOOM_RIGHT.
	 */
	public void iMoveScreenZoom(int moveType) {
		if ((mTVInputManager.getOuputMode() == TVInputCommon.OUTPUT_MODE_NORMAL)
				&& (current_zoom == ZOOM_2)) {
			mTvOutput = mTVInputManager.getDefaultOutput();

			RectF mSrcRectangleRectF = mTvOutput.getSrcRectangle();
			float l = 0;
			float t = 0;
			float r = 0;
			float b = 0;
			
			if(null != mSrcRectangleRectF ){
			   l = mSrcRectangleRectF.left;
		  	 t = mSrcRectangleRectF.top;
			   r = mSrcRectangleRectF.right;
			   b = mSrcRectangleRectF.bottom;			
       
			switch (moveType) {
			case ZOOM_UP:
				t -= 0.02;
				if (t < 0.0) {
					t += 0.02;
				} else {
					b -= 0.02;
				}
				break;
			case ZOOM_DOWN:
				b += 0.02;
				if (b > 1.0) {
					b -= 0.02;
				} else {
					t += 0.02;
				}
				break;
			case ZOOM_LEFT:
				l -= 0.02;
				if (l < 0.0) {
					l += 0.02;
				} else {
					r -= 0.02;
				}
				break;
			case ZOOM_RIGHT:
				r += 0.02;
				if (r > 1.0) {
					r -= 0.02;
				} else {
					l += 0.02;
				}
				break;
			default:
				break;
			}

			mTvOutput.setSrcRectangle(new RectF(l, t, r, b));
			}
		}
	}

	public int iGetCurrentZoom() {
		mTvOutput = mTVInputManager.getDefaultOutput();
    RectF mScreenRectangle = mTvOutput.getScreenRectangle();
    RectF mSrcRectangle = mTvOutput.getSrcRectangle();
     if(null != mScreenRectangle && null != mSrcRectangle ){
		float tmpscr = mScreenRectangle.right - mScreenRectangle.left;
		float tmpsrc = mSrcRectangle.right - mSrcRectangle.left;
		
		if (tmpscr < 0.6 && tmpscr > 0.4) {
			current_zoom = ZOOM_0_5;
		} else {
			if (tmpsrc < 0.6 && tmpsrc > 0.4) {
				current_zoom = ZOOM_2;
			} else {
				current_zoom = ZOOM_1;
			}

		}
  }
		return current_zoom;
	}

	public void iSetZoom(View mView, iEffectChangeZoomListener mZoomListener) {
		if (mTVInputManager.getOuputMode() == TVInputCommon.OUTPUT_MODE_NORMAL) {
			mTvOutput = mTVInputManager.getDefaultOutput();

			current_zoom++;
			if (current_zoom > ZOOM_0_5) {
				current_zoom = ZOOM_1;
			}

			switch (current_zoom) {
			case ZOOM_1:
				mTvOutput.setSrcRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
				mTvOutput.setScreenRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
				break;
			case ZOOM_2:
				mTvOutput
						.setSrcRectangle(new RectF(0.25f, 0.25f, 0.75f, 0.75f));
				mTvOutput.setScreenRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
				break;
			case ZOOM_0_5:
				mTvOutput.setSrcRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
				mTvOutput.setScreenRectangle(new RectF(0.25f, 0.25f, 0.75f,
						0.75f));
				break;
			default:
				break;
			}

			if (mZoomListener != null) {
				current_zoom = iGetCurrentZoom();
				mZoomListener.effectChanged(mView, current_zoom);
			}
		}
	}
	
	public void setZoomMode(int zoomMode) {
		mTvOutput = mTVInputManager.getDefaultOutput();
		switch (zoomMode) {
		case ZOOM_1:
			mTvOutput.setSrcRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
			mTvOutput.setScreenRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
			break;
		case ZOOM_2:
			mTvOutput.setSrcRectangle(new RectF(0.25f, 0.25f, 0.75f, 0.75f));
			mTvOutput.setScreenRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
			break;
		case ZOOM_0_5:
			mTvOutput.setSrcRectangle(new RectF(0.0f, 0.0f, 1.0f, 1.0f));
			mTvOutput.setScreenRectangle(new RectF(0.25f, 0.25f, 0.75f, 0.75f));
			break;
		default:
			break;
		}
	}
}
