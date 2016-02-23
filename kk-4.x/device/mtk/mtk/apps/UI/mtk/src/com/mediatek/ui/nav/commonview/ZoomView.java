package com.mediatek.ui.nav.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.nav.util.NavIntegrationZoom;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.nav.util.NavIntegrationZoom.iEffectChangeZoomListener;
import com.mediatek.ui.util.KeyMap;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class ZoomView extends RelativeLayout {
	private Context context;
	private NavIntegrationZoom mNavIntegrationZoom;
	private NavSundryImplement mNavSundryImplement;
	private Handler mHandler;
	RelativeLayout zoomRelativeLayout;
	private String[] screenModeName;

	public Handler getmHandler() {
		return mHandler;
	}

	public void setmHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	public ZoomView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		this.context = context;
		mNavIntegrationZoom = NavIntegrationZoom.getInstance(context);
		init();
	}

	public ZoomView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		mNavIntegrationZoom = NavIntegrationZoom.getInstance(context);
		init();
	}

	public ZoomView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		mNavIntegrationZoom = NavIntegrationZoom.getInstance(context);
		init();
	}

	private void init() {
		// mHandler = new Handler(context.getMainLooper());
		LayoutParams layoutParams = new LayoutParams(LayoutParams.FILL_PARENT,
				LayoutParams.FILL_PARENT);
		addView(inflate(context, R.layout.nav_zoom_layout, null), layoutParams);
		zoomRelativeLayout = (RelativeLayout) findViewById(R.id.nav_channel_zoom);
		screenModeName=context.getResources().getStringArray(R.array.screen_mode_array);
		mNavSundryImplement=NavSundryImplement.getInsNavSundryImplement(context);
	}

	/**
	 * set String Of mModel_ShortTip_TV indifferent zoomMode
	 * 
	 * @param value
	 */
	public void setZoomResetString(int value, TextView mModel_ShortTip_TV) {
		String[] zoomReset = getResources().getStringArray(
				R.array.nav_zoom_mode);
		if(mModel_ShortTip_TV.getVisibility()!= View.VISIBLE){
			mModel_ShortTip_TV.setVisibility(View.VISIBLE);
		}
		if (value < zoomReset.length && value > -1) {
			mModel_ShortTip_TV.setText(zoomReset[value]);
		} else {
			mModel_ShortTip_TV.setText(zoomReset[0]);
		}
	}

	/**
	 * set show zoomMode
	 */
	public void showZoomMode() {
		if (mNavIntegrationZoom.iGetCurrentZoom() == mNavIntegrationZoom.ZOOM_2) {
			mHandler.removeMessages(MessageType.NAV_ZOOMVIEW_DIMISS);
			setVisibility(View.VISIBLE);
//			System.out.println("zoomRelativeLayout----"
//					+ zoomRelativeLayout.getVisibility());
//			System.out.println("shwo zoom2");
			mHandler.sendEmptyMessageDelayed(MessageType.NAV_ZOOMVIEW_DIMISS,
					MessageType.delayMillis4);
		} else {
			setVisibility(View.GONE);
		}
	}

	/**
	 * Move Screen Zoom
	 * 
	 * @param value
	 */
	public void changPicture(int keyCode) {
		int value;
		mHandler.removeMessages(MessageType.NAV_ZOOMVIEW_DIMISS);
		switch (keyCode) {
		case KeyMap.KEYCODE_DPAD_DOWN:
			value = NavIntegrationZoom.ZOOM_DOWN;
			break;
		case KeyMap.KEYCODE_DPAD_UP:
			value = NavIntegrationZoom.ZOOM_UP;
			break;
		case KeyMap.KEYCODE_DPAD_LEFT:
			value = NavIntegrationZoom.ZOOM_LEFT;
			break;
		case KeyMap.KEYCODE_DPAD_RIGHT:
			value = NavIntegrationZoom.ZOOM_RIGHT;
			break;
		default:
			return;
		}
		mNavIntegrationZoom.iMoveScreenZoom(value);
		mHandler.sendEmptyMessageDelayed(MessageType.NAV_ZOOMVIEW_DIMISS,
				MessageType.delayMillis4);
	}

	/**
	 * change zoom
	 */
	public void changeZoom(final TextView mModel_ShortTip_TV) {
		mNavIntegrationZoom.iSetZoom(mModel_ShortTip_TV,
				new iEffectChangeZoomListener() {
					public void effectChanged(View mView, int value) {
						// TODO Auto-generated method stub
						setZoomResetString(value, mModel_ShortTip_TV);
						showZoomMode();
					}
				});
	}

	public void showCurrentZoom(final TextView mModel_ShortTip_TV) {
		setZoomResetString(mNavIntegrationZoom.iGetCurrentZoom(),
				mModel_ShortTip_TV);
		showZoomMode();
	}

	/**
	 * press info key banner info not visible,zoomRelativeLayout set gone
	 */
	public void zoomGone() {
		if (mNavIntegrationZoom.iGetCurrentZoom() == mNavIntegrationZoom.ZOOM_2
				&& zoomRelativeLayout.getVisibility() == View.VISIBLE) {
			setVisibility(View.GONE);
		}
	}

	
	public boolean isNotShow(){
		return mNavIntegrationZoom.screenModeZoomShow();
	}
	
	public void zoomSetfreeze(){
		mNavSundryImplement.setFreeze(false);
	}
}
