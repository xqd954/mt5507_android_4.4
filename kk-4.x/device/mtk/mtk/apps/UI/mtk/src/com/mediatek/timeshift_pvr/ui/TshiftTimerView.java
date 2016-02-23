/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.content.Context;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.ui.R;

/**
 *
 */
// public class TshiftTimerView extends CommonInfoBar {
public class TshiftTimerView extends PVRTimerView {
	private StateTimeShift callback;
	private static TshiftTimerView self;
	
	public static TshiftTimerView getInstance(Activity context){
	    if (self==null) {
            self = new TshiftTimerView(context);
        }
	    
	    return self;
	}

	/**
	 * @param context
	 */
	private TshiftTimerView(Activity context) {
		super(context);
		hiddenIcon();
	}

	/**
	 * 
	 */
	private void hiddenIcon() {
      getContentView().findViewById(R.id.pvr_rec_icon_small).setVisibility(View.GONE);
      getContentView().findViewById(R.id.pvr_fast_view).setVisibility(View.VISIBLE);
	}
	
	public void forceHiddenIcons() {
		getContentView().findViewById(R.id.pvr_rec_icon_small).setVisibility(View.GONE);
		getContentView().findViewById(R.id.pvr_fast_view).setVisibility(View.GONE);
	}
	
    public ImageView getIconView() {
        return (ImageView) getContentView().findViewById(R.id.pvr_rec_icon_small);
    }

    public TextView getFastView() {
        return (TextView) getContentView().findViewById(R.id.pvr_fast_view);
    }
	
	@Override
	public void doSomething() {
		super.doSomething();
		callback.refreshUI();
	}

	public StateTimeShift getCallback() {
		return callback;
	}

	public void setCallback(final StateTimeShift callback) {
		this.callback = callback;
	}
}
