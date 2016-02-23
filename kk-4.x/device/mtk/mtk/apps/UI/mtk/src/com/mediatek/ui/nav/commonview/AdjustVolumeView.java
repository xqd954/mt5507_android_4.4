package com.mediatek.ui.nav.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.cec.CecManager;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.nav.util.VolumeAdjustImp;
import com.mediatek.ui.util.MtkLog;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.view.KeyEvent;

public class AdjustVolumeView extends LinearLayout {

	private static final String TAG = "AdjustVolumeView";

	private Context mContext;

	private VolumeAdjustImp volImp;
	private Handler mHandler;

	//private String[] volumetype;

	private TextView volumeName;
	private ProgressBar volumeShow;
	private TextView volumeValue;

	public AdjustVolumeView(Context context) {
		super(context);
		mContext = context;
		volImp = VolumeAdjustImp.getInstance(context);
		init();

	}

	public AdjustVolumeView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		volImp = VolumeAdjustImp.getInstance(context);
		init();
	}

	public void setHandler(Handler handler) {
		this.mHandler = handler;
	}

	private void init() {

		this.addView(inflate(mContext, R.layout.nav_volume_adjust, null));
		volumeName = (TextView) findViewById(R.id.nav_volume_tv_itemname);
		volumeShow = (ProgressBar) findViewById(R.id.nav_volume_progressbar);
		volumeValue = (TextView) findViewById(R.id.nav_volume_tv_itemvalue);

		volumeShow.setMax(volImp.getMax());
		//volumetype = getResources().getStringArray(R.array.nav_bottom_volume);

	}

	/**
	 * adjust volume up and down
	 * 
	 * @param volumeStep
	 *            up:1 down:-1
	 */
	private void changeVolume(final int volumeStep) {
		mHandler.removeMessages(MessageType.NAV_ADUST_VOLUME_DIMISS);
		mHandler.post(new Runnable() {

			public void run() {
				// TODO Auto-generated method stub

				if (CecManager.getInstance()!=null &&CecManager.getInstance().isAmpVolume()) {
//				    volumeShow.setProgress(value);
//                    volumeValue.setText(String.valueOf(value));
//                    volImp.setVolume(value);
				    Log.e("ampvolume", "---------------------------");
                }else {
                    int changeVolumeFlag = volumeStep;
                    int value = volImp.get() + changeVolumeFlag;
                    MtkLog.d(TAG, "Volume: " + value);
                    if (value >= volImp.getMin() && value <= volImp.getMax()) {
                        volumeShow.setProgress(value);
                        volumeValue.setText(String.valueOf(value));
                        volImp.setVolume(value);
                    }
                }
			}

		});

		mHandler.sendEmptyMessageDelayed(MessageType.NAV_ADUST_VOLUME_DIMISS,
				MessageType.delayMillis4);
	}

	/**
	 * change volume mode
	 * 
	 * @param mode
	 *            next:1 previous:-1
	 */
	private void changeVolumeMode(int mode) {
		int newMode = volImp.getVolumeOption() + mode;
		volImp.setVolumeOption(newMode);
		changeVolume(0);
		String[] volumetype = getResources().getStringArray(R.array.nav_bottom_volume);
		volumeName.setText(volumetype[volImp.getVolumeOption()]);

	}

	public void initVolume() {
		TurnkeyUiMainActivity.cancelMute();
		mHandler.removeMessages(MessageType.NAV_ADUST_VOLUME_DIMISS);
		volImp.initVolume();
		String[] volumetype = getResources().getStringArray(R.array.nav_bottom_volume);
		volumeName.setText(volumetype[0]);
		volumeShow.setProgress(volImp.get());
		volumeValue.setText(String.valueOf(volImp.get()));

		mHandler.sendEmptyMessageDelayed(MessageType.NAV_ADUST_VOLUME_DIMISS,
				MessageType.delayMillis4);

	}

	public void volumeEvent(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch (keyCode) {
		case KeyEvent.KEYCODE_VOLUME_UP:
		    CecManager.getInstance().changeVolume(CecManager.Volume_UP);
			changeVolume(1);
			break;
		case KeyEvent.KEYCODE_VOLUME_DOWN:
		    CecManager.getInstance().changeVolume(CecManager.Volume_Down);
			changeVolume(-1);
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
		    CecManager.getInstance().changeVolume(CecManager.Volume_UP);
			changeVolumeMode(1);
			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
		    CecManager.getInstance().changeVolume(CecManager.Volume_Down);
			changeVolumeMode(-1);
			break;
		}
	}
	
	public void refresh(){
	    String ampVolume = "0";
	    if (CecManager.getInstance()!=null){
	        ampVolume = CecManager.getInstance().getVolumeAmp();
	    }
	    int value = Integer.parseInt(ampVolume);
      volumeShow.setProgress(value);
      volumeValue.setText(String.valueOf(value));
      volImp.setVolume(value);
	}

}
