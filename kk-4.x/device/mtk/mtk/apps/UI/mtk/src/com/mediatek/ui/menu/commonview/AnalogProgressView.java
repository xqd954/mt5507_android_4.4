package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.model.DtmbScanRF;
import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TVScanner.ScannerListener;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.EditChannel;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.util.MtkLog;

public class AnalogProgressView extends LinearLayout {

	private static final String TAG = "AnalogProgressView";
	private Context mContext;
	// scan status text
	private TextView mStateTextView;
	// anolog channel number
	private TextView mAnaloguechannel;
	// digital channel number
	private TextView mNumberChannel;
	public ProgressBar progressBar;
	// percentage of competion
	public int mPercentage;
	public TextView mFinishpercentage;
	private int progress = 0;
	public boolean mFinish = false;
	private LinearLayout probarLine;
	public Loading loading;
	private boolean scannerStatu = false;// remember progress state value

	private TVContent tvc;
	private TVChannelManager tm;
	private NavIntegration mNavIntegration;
	private int mLastChannelNum = 0;
	private TVChannelSelector selector;
	private int mAddedChannelCount = 0;

	private TVScanner tv_scan;
	private EditChannel editChannel;
	private Handler mHandler;

	public static final int ATV_SCAN_COMPLETED = 1;
	public static final int DTV_SCAN_COMPLETED = 2;
	public boolean mKillThread =false;

	public AnalogProgressView(Context context) {
		super(context);
		mContext = context;
	}

	public AnalogProgressView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
	}

	protected void onFinishInflate() {
		super.onFinishInflate();
		((Activity) getContext()).getLayoutInflater().inflate(
				R.layout.menu_scan_dialog_layout, this);
		init();
	}

	public void init() {
		tv_scan = TVContent.getInstance(mContext).getScanner();
		editChannel = EditChannel.getInstance(mContext);
		mStateTextView = (TextView) findViewById(R.id.state);
		progressBar = (ProgressBar) findViewById(R.id.scanprogressbar);
		mFinishpercentage = (TextView) findViewById(R.id.finishpercentage);
		loading = (Loading) findViewById(R.id.setup_tv_scan_loading);
		mFinishpercentage.setText(mPercentage + "%");
		mStateTextView.setText(mContext
				.getString(R.string.menu_setup_channel_scan));
		mAnaloguechannel = (TextView) findViewById(R.id.analoguechannel);
		mAnaloguechannel.setText(mContext
				.getString(R.string.menu_setup_channel_scan_ana));
		mNumberChannel = (TextView) findViewById(R.id.numberchannel);
		mNumberChannel.setText(mContext
				.getString(R.string.menu_setup_channel_scan_dig));
		probarLine = (LinearLayout) findViewById(R.id.menu_scan_proline);
	}

	public TextView getmStateTextView() {
		return mStateTextView;
	}

	public void setmStateTextView(TextView mStateTextView) {
		this.mStateTextView = mStateTextView;
	}

	public ProgressBar getProgressBar() {
		return progressBar;
	}

	public void setProgressBar(ProgressBar progressBar) {
		this.progressBar = progressBar;
	}

	public TextView getmFinishpercentage() {
		return mFinishpercentage;
	}

	public void setmFinishpercentage(TextView mFinishpercentage) {
		this.mFinishpercentage = mFinishpercentage;
	}

	public int getProgress() {
		return progress;
	}

	public void setProgress(int progress) {
		this.progress = progress;
		progressBar.setProgress(progress);

	}

	public boolean isScannerStatu() {
		return scannerStatu;
	}

	public void setScannerStatu(boolean scannerStatu) {
		this.scannerStatu = scannerStatu;
	}

	public LinearLayout getProbarLine() {
		return probarLine;
	}

	public void setProbarLine(LinearLayout probarLine) {
		this.probarLine = probarLine;
	}

	public void setHandler(Handler handler) {
		mHandler = handler;
	}

	public void scan(final int freFrom, int freTo) {
		tvc = TVContent.getInstance(mContext);
		tm = tvc.getChannelManager();
		selector = tvc.getChannelSelector();
		mNavIntegration = NavIntegration.getInstance(mContext);
		if (selector.getCurrentChannel() != null) {
			TVChannel mCurrentChannel = mNavIntegration.iGetCurrentChannel();
			if (null != mCurrentChannel) {
				mLastChannelNum = mCurrentChannel.getChannelNum();
			}	
		}
		if (!NavIntegration.getInstance(mContext).isCurrentSourceDTV()) {
			MtkLog.i(TAG, "################# Analog Manual Scan Frequency ["
					+ freFrom + ", " + freTo + "]");
			tv_scan.atvRangeScan(freFrom, freTo,
					(TVScanner.ScannerListener) new ScannerListener() {
						public void onCompleted(int completeValue) {
							if (tm.getChannelLength() > 0) {
								selectChannel();
							}
						}

						public void onFrequence(int freq) {
							// rate
							mAnaloguechannel.setText(mContext
									.getString(R.string.menu_tv_freq)
									+ (float) freq / 1000 + "MHz");
						}

						public void onProgress(final int progress,
								final int channels) {
							mHandler.post(new Runnable() {
								public void run() {
									mStateTextView.setVisibility(View.VISIBLE);
									mAnaloguechannel
											.setVisibility(View.VISIBLE);
									loading.setVisibility(View.VISIBLE);
									mNumberChannel.setVisibility(View.VISIBLE);
									mStateTextView
											.setText(mContext
													.getString(R.string.menu_setup_channel_scan));
									mNumberChannel
											.setText(mContext
													.getString(R.string.menu_setup_channel_scan_ana)
													+ channels);
									mFinishpercentage.setText(progress + "%");
									progressBar.setProgress(progress);

									if (progress >= 100) {
										mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE, MessageType.delayMillis2);
										Message msg = new Message();
										msg.what = MessageType.MENU_TV_SCAN_COMPLETED;
										msg.arg1 = ATV_SCAN_COMPLETED;
										mHandler.sendMessage(msg);
									}
								}
							});
						}
					});
		} else {
			MtkLog.i(TAG, "################# Single RF Scan Frequency ["
					+ freFrom + ", " + freTo + "]");
					try {
						tv_scan.dvbcSingleRFScan(freFrom,
								(TVScanner.ScannerListener) new ScannerListener() {
									public void onCompleted(int completeValue) {
							if (tm.getChannelLength() > 0) {
								selectAddChannel(freFrom);
							}

						}

						public void onFrequence(int freq) {
							MtkLog.i(TAG,
									"################# Single RF Scan Frequency ["
											+ freq);
						}

						public void onProgress(final int progress,
								final int channels) {
							mHandler.post(new Runnable() {
								public void run() {
									mStateTextView.setVisibility(View.VISIBLE);
									mAnaloguechannel
											.setVisibility(View.VISIBLE);
									loading.setVisibility(View.VISIBLE);
									mNumberChannel.setVisibility(View.VISIBLE);
									mStateTextView
											.setText(mContext
													.getString(R.string.menu_setup_channel_scan));
									mNumberChannel
											.setText(mContext
													.getString(R.string.menu_tv_rf_signal_level));
									mAnaloguechannel
											.setText(mContext
													.getString(R.string.menu_setup_channel_scan_dig)
													+ channels);
									mAddedChannelCount = channels;

									mFinishpercentage.setText(progress + "%");
									progressBar.setProgress(progress);

									if (progress >= 100) {
										mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE,MessageType.delayMillis2);
										Message msg = new Message();
										msg.what = MessageType.MENU_TV_SCAN_COMPLETED;
										msg.arg1 = DTV_SCAN_COMPLETED;
										msg.arg2 =ConfigType.BS_SRC_CABLE;
										mHandler.sendMessage(msg);
									}
								}
							});
						}
					});
					} catch (IllegalAccessException e) {
						throw new IllegalStateException("Must be invoked in dvb-cable mode.");
					}
				
		}
	}

	private void selectChannel() {
		if (tm.getChannelLength() > 0) {
			TVChannel ch = tm.findChannelByNumber(mLastChannelNum);

			if (ch == null) {
				List<TVChannel> myChennels = tm.getChannels();
				if(null != myChennels){
					ch = myChennels.get(0);
				}
			}

			tvc.getChannelSelector().select(ch);
		}
	}

	/**
	 * select the first new found channel, if there is no channel found, select
	 * the previous selected channel
	 * 
	 * @param freq
	 */
	private void selectAddChannel(int freq) {
		List<TVChannel> mCurrentList = tm.getChannels();
		TVChannel selCh = tm.findChannelByNumber(mLastChannelNum);
		if(null != mCurrentList){
		for (TVChannel ch : mCurrentList) {
			if (ch.getFreq() == freq) {
				selCh = ch;
				break;
				}
			}
		}
		if(selCh !=null){
		tvc.getChannelSelector().select(selCh);
		}
	}
	public void singleRFChannelScan(){
		tvc = TVContent.getInstance(mContext);
		tm = tvc.getChannelManager();
		mKillThread =false;
		selector = tvc.getChannelSelector();
		mNavIntegration = NavIntegration.getInstance(mContext);
		if (selector.getCurrentChannel() != null) {
			TVChannel channel = mNavIntegration.iGetCurrentChannel();
			if (null != channel) {
				mLastChannelNum = channel.getChannelNum();
			}
		}
		final DtmbScanRF  mDtmbScanRF =tv_scan.getCurDtmbScanRF();
		if(mDtmbScanRF!=null && !"".equals(mDtmbScanRF.getRFChannel())){
			try {
				tv_scan.dtmbSingleRFScan(mDtmbScanRF.getRFChannel(),new  ScannerListener() {
					public void onCompleted(int completeValue) {
						if (tm.getChannelLength() > 0) {
								selectAddChannel(mDtmbScanRF.getRFScanFrequency());
						}
						mHandler.sendEmptyMessage(MessageType.MENU_TV_RF_SCAN_REFRESH);
					}

			public void onFrequence(int freq) {
				
				MtkLog.i(TAG,
						"################# Single RF Scan Frequency ["
								+ freq);
			}

			public void onProgress(final int progress,
					final int channels) {
//				mHandler.sendEmptyMessage(MessageType.MENU_TV_RF_SCAN_REFRESH);
				
				mHandler.post(new Runnable() {
					public void run() {
						mStateTextView.setVisibility(View.VISIBLE);
						mAnaloguechannel
								.setVisibility(View.VISIBLE);
						loading.setVisibility(View.VISIBLE);
								
//								if((TVScanner.SCAN_TYPE_DTMB).equals(getScanType())){
//									mNumberChannel.setVisibility(View.INVISIBLE);
//								}else{
//									mNumberChannel.setVisibility(View.VISIBLE);
//									mNumberChannel
//									.setText(mContext
//											.getString(R.string.menu_tv_rf_signal_level));
//								}
								mNumberChannel.setVisibility(View.INVISIBLE);
								mStateTextView
								.setText(mContext
										.getString(R.string.menu_setup_channel_scan));
						mNumberChannel
								.setText(mContext
										.getString(R.string.menu_tv_rf_signal_level));
						mAnaloguechannel
								.setText(mContext
										.getString(R.string.menu_setup_channel_scan_dig)
										+ channels);
						mAddedChannelCount = channels;

						mFinishpercentage.setText(progress + "%");
						progressBar.setProgress(progress);

						if (progress >= 100) {
							
							mHandler
									.sendEmptyMessageDelayed(
											MessageType.MENU_AUTO_EXIT_MESSAGE,
											MessageType.delayMillis2);
							Message msg = new Message();
							msg.what = MessageType.MENU_TV_SCAN_COMPLETED;
							msg.arg1 = DTV_SCAN_COMPLETED;
							msg.arg2 =ConfigType.BS_SRC_AIR;
							mHandler.sendMessage(msg);
						}
					}
				});
			}
				});
			} catch (IllegalAccessException e) {
				// TODO Auto-generated catch block
				throw new IllegalStateException("Must be invoked in dtmb-air mode.");
			}
		}
		
	/*new Thread(new Runnable() {
			
			public void run() {
				while(mKillThread==false){
					try {
						Thread.sleep(500);
						mHandler.sendEmptyMessage(MessageType.MENU_TV_RF_SCAN_REFRESH);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					
				}
			}
		}).start();*/
	}
}
