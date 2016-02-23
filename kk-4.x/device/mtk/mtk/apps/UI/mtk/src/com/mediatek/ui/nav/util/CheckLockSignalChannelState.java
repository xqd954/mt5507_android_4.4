package com.mediatek.ui.nav.util;

import android.content.Context;
import android.os.Handler;
import android.view.View;

import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.commonview.BannerView;
import com.mediatek.ui.nav.commonview.SnowTextView;
import com.mediatek.ui.nav.commonview.SnowTextView.ShowType;

public class CheckLockSignalChannelState {
	private static CheckLockSignalChannelState instance;
	private Context mContext;
	private NavIntegration mNavIntegration;
	private static Object lock = new Object();
	private static final String TAG = "CheckLockSignalChannelState";
	private BannerView mBannerView;
	private SnowTextView mSnowTextView;
	private Handler mHandler;
	private static TurnkeyUiMainActivity mTurnkeyUiMainActivity;

	public Handler getHandler() {
		return mHandler;
	}

	public void setBannerView(BannerView mBannerView) {
		this.mBannerView = mBannerView;
	}

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	private CheckLockSignalChannelState(Context context) {
		mNavIntegration = NavIntegration.getInstance(context);
	}

	public static synchronized CheckLockSignalChannelState getInstance(Context context) {
				if (instance == null) {
					instance = new CheckLockSignalChannelState(context);
					if (null != context
							&& context instanceof TurnkeyUiMainActivity) {
						mTurnkeyUiMainActivity = (TurnkeyUiMainActivity) context;
					}
				}
		return instance;
	}

	public void setSnowTextView(SnowTextView mSnowTextView) {
		this.mSnowTextView = mSnowTextView;
	}

	public boolean checkLockedSignStateOrHasChannel(boolean mFirstShowBanner) {		
		if (mFirstShowBanner && null != mSnowTextView) {
			mSnowTextView.setVisibility(View.GONE);
		}
		if (mNavIntegration.isCurrentSourceTv() && null != mSnowTextView && null != mBannerView) {
			if (mNavIntegration.isCurrentSourceBlocking()) {
				if ((mFirstShowBanner == true
						|| mTurnkeyUiMainActivity.mFirstShowInputPwdDialog == true) && null != mHandler) {
					mHandler
							.removeMessages(MessageType.NAV_CURRENT_SOURCE_LOCKED);					
					mTurnkeyUiMainActivity.mFirstShowInputPwdDialog = false;
					mHandler
							.sendEmptyMessage(MessageType.NAV_CURRENT_SOURCE_LOCKED);
				} else {					
					showSnowText(ShowType.SPECIAL_INPUT_LOCKED);						
				}				
				return true;
			}
			if (mNavIntegration.iGetChannelLength() <= 0) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_PLEASE_SCAN);					
				}				
				return true;
			}
			if (mNavIntegration.iGetChannelLength() > 0) {
				if (mNavIntegration.isCurrentChannelBlocking()) {
					if (mFirstShowBanner == true) {
						mBannerView.show(false, -1, false);
					} else {
						showSnowText(ShowType.SPECIAL_CHANNEL_LOCKED);						
					}					
					return true;
				}
			}
			if (mNavIntegration.iCurrentInputSourceHasSignal() == false) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_NO_SIGNAL);
				}				
				return true; 
			}

			if (mNavIntegration.iCurrentChannelHasSignal() == false) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_NO_SIGNAL);					
				}				
				return true;
			}
			if (mNavIntegration.isAudioScrambled()
					|| mNavIntegration.isVideoScrambled()) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_CHANNEL_SCRAMBLED);					
				}				
				return true;
			}
			if (mNavIntegration.isCurrentChannelRadio()) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_RADIO_ONLY);					
				}				
				return true;
			}

		} else if(null != mSnowTextView && null != mBannerView){
			if (mNavIntegration.isCurrentSourceBlocking()) {
				if ((mFirstShowBanner == true
						|| mTurnkeyUiMainActivity.mFirstShowInputPwdDialog == true) && null != mHandler) {
					mHandler
							.removeMessages(MessageType.NAV_CURRENT_SOURCE_LOCKED);					
					mTurnkeyUiMainActivity.mFirstShowInputPwdDialog = false;
					mHandler
							.sendEmptyMessage(MessageType.NAV_CURRENT_SOURCE_LOCKED);
				} else {
					showSnowText(ShowType.SPECIAL_INPUT_LOCKED);					
				}				
				return true;
			}
			if (mNavIntegration.iCurrentInputSourceHasSignal() == false) {
				if (mFirstShowBanner == true) {
					mBannerView.show(false, -1, false);
				} else {
					showSnowText(ShowType.SPECIAL_NO_SIGNAL);					
				}				
				return true;
			}
			 if (mNavIntegration.vgaNotSupport()) {
				 if (mFirstShowBanner == true) {
						mBannerView.show(false, -1, false);
					} else {
						showSnowText(ShowType.SPECIAL_NO_SUPPORT);					
					}
				 return true;
				}
		}
		return false;
	}
	
	private void showSnowText(ShowType showType){
		mTurnkeyUiMainActivity.hideAll();
		mSnowTextView.showSpecialView(showType);
		mSnowTextView.setVisibility(View.VISIBLE);
	}

}
