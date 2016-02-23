/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;
import android.widget.PopupWindow.OnDismissListener;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.timeshift_pvr.manager.PvrConstant;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.TshiftControlbar;
import com.mediatek.timeshift_pvr.ui.TshiftTimerView;
import com.mediatek.ui.R;

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 *
 */
public class StateTimeShift extends StateBase {

	private TshiftControlbar mBigCtrlBar;
	private TshiftTimerView mSmallTimerBar;

	private static StateTimeShift mStateSelf;

	private MyHandler mHandler;

	private Long mDuration = 10L * 1000;
	private final Long MAX_DURATION = 24L * 60 * 60;

	private ImageView mPlayIcon;
	private TextView mTshiftTimerView;
	private View[] mTimeLine; // prepare for animation.
	private ProgressBar mProgressbar;

	private Animation mAnim; // prepare for timeline's animation.
	private boolean isAnimationing = false;
    public TextView vVideoRepeata;
    public ImageView vPStatePlay ;

	private int mTiming = 0;
	private int playerOffset=0;
	private boolean isPlaying = false;
	private boolean isStarting = false;
	private int speedStep = 1;
	private int totalTime = 0;

	private final static int SHOW_BIG_CTRL_BAR = 1;
	private final static int SHOW_SMALL_CTRL_BAR = 2;

	private final static int REFRESH_TIMER = 6;
	private final static int REFRESH_TIMER_REDUCE = 61;

	private final static int INIT_PLAYER = 7;
	private final static int START_PLAYER = 8;

	protected static final int START_RECORD = 13;
	protected static final int STOP_RECORD = 14;
	private final static int Clear_allWindow = 15;

	private final static int DEBUGDEBUG = 111111;

	static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity);
		}

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SHOW_BIG_CTRL_BAR:
				if (mStateSelf.isRunning()) {
					mStateSelf.showBigCtrlBar();
					mStateSelf.clearWindow(false);
				}
				break;
			case SHOW_SMALL_CTRL_BAR:
				if (mStateSelf.isRunning()) {
					mStateSelf.showSmallCtrlBar();
					mStateSelf.clearWindow(true);
				}
				break;
			case REFRESH_TIMER:
				removeMessages(REFRESH_TIMER);
				 if (mStateSelf.speedStep!=1) {
                     return;
                 }
				if (mStateSelf.isRunning()) {
				   
					sendEmptyMessageDelayed(REFRESH_TIMER, 1 * 1000);
					mStateSelf.setmTiming(mStateSelf.getmTiming() + 1);
					if (mStateSelf.isStarting) {
					    mStateSelf.setmTiming(mStateSelf.getmTiming() - 1);
					    mStateSelf.isStarting = false;
                    }
					if (!mStateSelf.isPlaying) {
					    mStateSelf.refreshUI();
                    }
	              
				}
				break;
			case REFRESH_TIMER_REDUCE:
              
                removeMessages(REFRESH_TIMER_REDUCE);

                if (mStateSelf.isRunning()&& mStateSelf.isPlaying) {
                    sendEmptyMessageDelayed(REFRESH_TIMER_REDUCE, 1 * 1000);
                    if (mStateSelf.getmTiming()>mStateSelf.totalTime) {
                        mStateSelf.onFast(1, 0, Const.FILTER_VIDEO);
                        mStateSelf.getManager().getController().startVideo();
                        mStateSelf.speedStep = 1;
                        mStateSelf.vVideoRepeata.setVisibility(View.GONE);
                        mStateSelf.setmTiming(mStateSelf.totalTime);
                        sendEmptyMessageDelayed(REFRESH_TIMER, 1 * 1000);
                        mStateSelf.refreshUI();
                       
                        return;
                    }
                    if (mStateSelf.getmTiming() - 1>0) {
                        mStateSelf.refreshUI();
                        mStateSelf.setmTiming(mStateSelf.getmTiming() - 1*mStateSelf.speedStep);
                        
                    }else {
                        removeMessages(REFRESH_TIMER_REDUCE);
                        Toast.makeText(mStateSelf.mContext, "Stop !", Toast.LENGTH_SHORT).show();
                        mStateSelf.stopTimeShift();
                        mStateSelf.getManager().getController().stopPlayPvr();
                    }
                    
                }
                break;
			case INIT_PLAYER:
				removeMessages(START_PLAYER);
				mStateSelf.getManager().getController().initPvrPlayer();

				sendEmptyMessageDelayed(START_PLAYER, 1 * 1000);
				break;

			case START_PLAYER:
				mStateSelf.getManager().getController().startPlayPvr();
				break;

			case START_RECORD:
				mStateSelf.setRunning(true);
				mStateSelf.getController().preparePvr(false, false);
				// sendEmptyMessageDelayed(DEBUGDEBUG, 10*1000);
				Util.showRecordToast(mStateSelf.getActivity());
				break;

			case DEBUGDEBUG:

				// mStateSelf.getController().startPvr();
				break;

			case STOP_RECORD:
				mStateSelf.setRunning(false);
				mStateSelf.getController().stopPvr(false);
				break;
			case PvrConstant.Dissmiss_Tshift_BigCtrlbar:
                Util.Hlog("123456");
                mStateSelf.dissmissBigCtrlBar();
                break;
			case Clear_allWindow:
			    mStateSelf.clearAllWindow();
			    break;
			default:
				break;
			}

			super.handleMessage(msg);
		}

	}
	
	   public static StateTimeShift getInstance(Context mContext, TimeShiftManager manager){
	        if(mStateSelf ==null){
	            mStateSelf = new StateTimeShift(mContext,manager);
	        }
	        return mStateSelf;
	    }
	    
	    public static StateTimeShift getInstance(){
	        if(mStateSelf !=null){
	            return mStateSelf;
	        }else {
	            return null;
	        }
	}

	public StateTimeShift(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);

		setType(StatusType.TIMESHIFT);
		mStateSelf = this;
		mHandler = new MyHandler(getActivity());
		initView();

		startTimeShift();
		setRunning(true);
		onResume();
	}

	/**
	 * 
	 */
	private void initView() {
		initBigCtrlBar();
	}

	/**
	 * 
	 */
	private void initBigCtrlBar() {
		mBigCtrlBar = new TshiftControlbar(getActivity(),
				R.layout.pvr_timeshfit_tshiftworking, mDuration);

		mPlayIcon = (ImageView) mBigCtrlBar.getContentView().findViewById(
				R.id.tshift_plcontorl_btn);
		mProgressbar = (ProgressBar) mBigCtrlBar.getContentView().findViewById(
				R.id.tshfit_timeline_progressBar);
		mTshiftTimerView = (TextView) mBigCtrlBar.getContentView()
				.findViewById(R.id.tshift_offset_timerview);

		TextView tv0 = (TextView) mBigCtrlBar.getContentView().findViewById(
				R.id.tshfit_timeline_time0);
		TextView tv1 = (TextView) mBigCtrlBar.getContentView().findViewById(
				R.id.tshfit_timeline_time1);
		TextView tv2 = (TextView) mBigCtrlBar.getContentView().findViewById(
				R.id.tshfit_timeline_time2);

		ImageView dividLine = (ImageView) mBigCtrlBar.getContentView()
				.findViewById(R.id.tshfit_timeline_dividline1);

		ProgressBar timeLinePb = (ProgressBar) mBigCtrlBar.getContentView().findViewById(
				R.id.tshfit_timeline_dividline1_pb);
		
		timeLinePb.setMax(30*60);	//half an hour.
		mTimeLine = new View[] { tv0, tv1, tv2, dividLine,timeLinePb };

		mBigCtrlBar.setCallback(this);

		mBigCtrlBar.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss() {
				mHandler.sendEmptyMessage(SHOW_SMALL_CTRL_BAR);
			}
		});
	}

	/**
	 * 
	 */
	protected void showSmallTimeLine() {
		// TODO Auto-generated method stub
		mSmallTimerBar = TshiftTimerView.getInstance(getActivity());
		mSmallTimerBar.setDuration(MAX_DURATION);
		mSmallTimerBar.setCallback(this);

		mSmallTimerBar.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss() {
				if (isRunning()) {
					mHandler.sendEmptyMessage(SHOW_BIG_CTRL_BAR);
				}
			}
		});

		mSmallTimerBar.setDuration(Long.MAX_VALUE);

		mSmallTimerBar.show(mSmallTimerBar);

		setRunning(true);
	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		// TODO Auto-generated method stub
		switch (keycode) {

		case KeyEvent.KEYCODE_MTKIR_FREEZE:
			if (isRunning()) {
				return true;
			} else {
			    isStarting = false;
			    isPlaying = false;
				startTimeShift();
			}
		case KeyEvent.KEYCODE_MEDIA_STOP:
//		case KeyEvent.KEYCODE_POWER:
			// getManager().getmTVLogicManager().releasePlayer();
		    isPlaying = false;
		    isStarting = false;
		    totalTime = 0;
			stopTimeShift();
			getManager().getController().stopPlayPvr();
//			getManager().restoreToDefault();
			 Util.Hlog("onKeyDown: StateTimeShift.java-->stop."+Util.secondToString(getmTiming()));   
			break;
		case KeyEvent.KEYCODE_GUIDE:
		case KeyEvent.KEYCODE_MTKIR_GUIDE:
		    Util.Hlog("onKeyDown: StateTimeShift.java-->return key GUIDE.");   
            return true;
		case KeyEvent.KEYCODE_MTKIR_PRECH:

		    return true;
		case KeyEvent.KEYCODE_CHANNEL_DOWN:
		case KeyEvent.KEYCODE_CHANNEL_UP:
		case KeyEvent.KEYCODE_MTKIR_CHUP:
		case KeyEvent.KEYCODE_MTKIR_CHDN:
		case KeyEvent.KEYCODE_MTKIR_SOURCE:
		case KeyEvent.KEYCODE_MTKIR_PIPPOP:
		case KeyEvent.KEYCODE_MTKIR_RED:
		case KeyEvent.KEYCODE_MTKIR_ANGLE:
		case KeyEvent.KEYCODE_MTKIR_RECORD:
		case KeyEvent.KEYCODE_MEDIA_EJECT:
		case KeyEvent.KEYCODE_MTKIR_ASPECT:
		case KeyEvent.KEYCODE_MTKIR_ZOOM:
			return true;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			if (isPlaying) {
            }else {
				isPlaying = true;
				isStarting = true;
				getManager().getController().startPlayPvr();
				totalTime = getmTiming();
				mHandler.sendEmptyMessageDelayed(REFRESH_TIMER_REDUCE,1000);

			}

			return true;
	     case KeyEvent.KEYCODE_0:
	        case KeyEvent.KEYCODE_1:
	        case KeyEvent.KEYCODE_2:
	        case KeyEvent.KEYCODE_3:
	        case KeyEvent.KEYCODE_4:
	        case KeyEvent.KEYCODE_5:
	        case KeyEvent.KEYCODE_6:
	        case KeyEvent.KEYCODE_7:
	        case KeyEvent.KEYCODE_8:
	        case KeyEvent.KEYCODE_9:
	        case KeyEvent.KEYCODE_MTKIR_GREEN:
	            Util.Hlog("onKeyDown: StateTimeShift.java-->return key Number.");   
	            
	            return true;
		case KeyEvent.KEYCODE_MEDIA_FAST_FORWARD:
		    Log.e("timeshift0527", "forwardplay");
		    if (totalTime<=10) {
		        Toast.makeText(mContext, "Please Record More Than 10 Second !", Toast.LENGTH_SHORT).show();
                return true;
            }
			forwardPlay();
			speedStep = getManager().getController().getSpeedStep();
			return true;
		
		case KeyEvent.KEYCODE_MEDIA_REWIND:
		   
		    Log.e("timeshift0527", "rewindplay");
		    if (totalTime<=10) {
		        Toast.makeText(mContext, "Please Record More Than 10 Second !", Toast.LENGTH_SHORT).show();
                return true;
            }
			rewindPlay();
			speedStep = getManager().getController().getSpeedStep()*(-1);
			return true;
		case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
		    return true;
		default:
			break;

		}
		return false;
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
	    clearWindow(false);
		if (isRunning()) {
			showBigCtrlBar();
		}

		

		mHandler.sendEmptyMessage(REFRESH_TIMER);
	}

	/* 
	 */
	@Override
	public void onPause() {
		clearWindow(true);
	}

	/* 
	 */
	@Override
	public void onStop() {
		stopTimeShift();
		clearAllWindow();
	}

	/* 
	 */
	@Override
	public void onRelease() {

		setRunning(false);
		mHandler.sendEmptyMessage(Clear_allWindow);
		
	}

	/*
	 * 
	 */
	public boolean startTimeShift() {
		getManager().showDiskAttention();
		mHandler.sendEmptyMessageDelayed(START_RECORD, 50);
		// getController().startRecordTshift();
		return false;
	}

	/*
	 * 
	 */
	public boolean stopTimeShift() {
		mHandler.sendEmptyMessageDelayed(STOP_RECORD, 50);
		return false;
	}

	public void refreshUI() {
	    Log.e("refreshUI", "-----------------------------"+Util.secondToString(mTiming));
		if (mBigCtrlBar != null && mBigCtrlBar.isShowing()
				&& mTshiftTimerView != null) {
			mTshiftTimerView.setText(Util.secondToString(mTiming));

			if (!isAnimationing) {
				startTimeLineAnim();
//				isAnimationing = true;
			}
		} else if (mSmallTimerBar != null && mSmallTimerBar.isShowing()) {
			mSmallTimerBar.setInfo(Util.secondToString(mTiming));
			if (isAnimationing) {
				stopTimeLineAnim();
				isAnimationing = false;
			}
		}

	}

	/**
	 * 
	 */
	private void stopTimeLineAnim() {
		try {
			mTimeLine[0].clearAnimation();
			mTimeLine[1].clearAnimation();
			mTimeLine[2].clearAnimation();
			mTimeLine[3].clearAnimation();
			mTimeLine[4].clearAnimation();
		} catch (Exception e) {
			Util.showELog("clear animation fail...");
		}
	}

	public void startTimeLineAnim() {
		int left1 = mTimeLine[0].getLeft();
		int left2 = mTimeLine[2].getLeft();

		int distance = left2 - left1;
		Long timeoffset = timelineOffset();

		int offset = (int) ((distance / (30f * 60)) * (timeoffset / 1000f));
		Util.showDLog("offset:" + offset);
		Util.showDLog("prepareTimeLine:" + timeoffset);

		Util.showDLog("left-1:" + left1);
		Util.showDLog("left-2:" + left2);

		Long duration=30L * 60 * 1000 / 500 * distance;
		Util.showDLog("Duration-3:"+duration);
		
		TranslateAnimation anim = Util.commonAimationIn(mContext, offset
				* (-1f),duration );
		
		Date[] timelineDate = prepareTimeLineDate();
		((TextView) mTimeLine[0]).setText(commonDateToStr(timelineDate[0]));
		((TextView) mTimeLine[1]).setText(commonDateToStr(timelineDate[1]));
		((TextView) mTimeLine[2]).setText(commonDateToStr(timelineDate[2]));

		mTimeLine[0].startAnimation(anim);
		mTimeLine[1].startAnimation(anim);
		mTimeLine[2].startAnimation(anim);

		float offsetOfDivid=(getmTiming() / (60f * 60)) * distance * (-1f);
		Long dividDuration=30L * 60 * 1000/ 500 * distance;
		
		
		Util.showDLog("offsetOfDivid-4:"+offsetOfDivid);
		Util.showDLog("dividDuration-5:"+dividDuration);

		setPlayerOffset(50);
		TranslateAnimation dividAnim = Util.commonAimationIn(mContext,
				(getPlayerOffset() / 100f)*0.5f * distance * (-1f));
		mTimeLine[3].startAnimation(dividAnim);
		
		TranslateAnimation progressdAnim = Util.commonAimationIn(mContext,
				((getmTiming() / (60f * 60)) * distance +13)* (-1f), 30L * 60 * 1000
						/ 500 * distance);	//offset+13
		((ProgressBar)mTimeLine[4]).setProgress(getmTiming());
		mTimeLine[4].startAnimation(progressdAnim);
	}

	/*
	 * 
	 */
	public void showSmallCtrlBar() {
		showSmallTimeLine();
	}

	/*
	 * 
	 */
	public void showBigCtrlBar() {
		if (mBigCtrlBar == null) {
			initBigCtrlBar();
		}else {
	      Log.e("showBigCtrlBar", "mBigCtrlBar.isShowing:"+mBigCtrlBar.isShowing());
	        if (mBigCtrlBar.isShowing()) {
	            return;
	        }
        }
		
		if (mSmallTimerBar != null )
		{
		    mSmallTimerBar.dismiss();
		    mSmallTimerBar = null;
		}
		mBigCtrlBar.show(mBigCtrlBar);
	}
	
	public void dissmissBigCtrlBar(){
	    mBigCtrlBar.dismiss();
	}
	
	public Handler getStateTimeShiftHandler(){
	    return mHandler;
	}

	/*
	 * 
	 */
	public void clearWindow(boolean bigger) {
		if (bigger) {
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing())
				mBigCtrlBar.dismiss();
		} else {
			if (mSmallTimerBar != null && mSmallTimerBar.isShowing()){
			    if (null!=vVideoRepeata) {
			        vVideoRepeata.setVisibility(View.GONE);
                }
			    mSmallTimerBar.dismiss();
			}
				
			mSmallTimerBar = null;
		}

	}

	/*
	 * 
	 */
	public void clearAllWindow() {
		clearWindow(true);
		clearWindow(false);
	}

	public int getmTiming() {
		return mTiming;
	}

	public void setmTiming(int mTiming) {
		this.mTiming = mTiming;
	}

	private Long timelineOffset() {

		Date date = new Date();

		int hour = date.getHours();
		int minutes = date.getMinutes();

		Date rDate = new Date();
		Date lDate = new Date();

		if (minutes >= 30) {
			rDate.setHours(hour + 1);
			rDate.setMinutes(0);
			rDate.setSeconds(0);

			lDate.setMinutes(30);
			lDate.setSeconds(0);

		} else {
			rDate.setMinutes(30);
			rDate.setSeconds(0);

			lDate.setMinutes(0);
			lDate.setSeconds(0);
		}
		return (date.getTime() - lDate.getTime());
	}

	private Date[] prepareTimeLineDate() {

		Date date = new Date();

		int hour = date.getHours();
		int minutes = date.getMinutes();

		Date rDate = new Date();
		Date lDate = new Date();

		if (minutes >= 30) {
			rDate.setHours(hour + 1);
			rDate.setMinutes(0);
			rDate.setSeconds(0);

			lDate.setMinutes(30);
			lDate.setSeconds(0);

		} else {
			rDate.setMinutes(30);
			rDate.setSeconds(0);

			lDate.setMinutes(0);
			lDate.setSeconds(0);
		}
		// return (lDate.getTime()-date.getTime());
		return new Date[] { new Date(lDate.getTime() - 30 * 60 * 1000), lDate,
				new Date(lDate.getTime() + 30 * 60 * 1000) };
	}

	String commonDateToStr(Date date) {
		SimpleDateFormat format = new SimpleDateFormat("HH:mm");
		String str = format.format(date);
		Util.showELog(str);
		return str;
	}
	
	   
    public void showFastPlayInfo(boolean FastForward){
        int status = getManager().getController().getPlayStatus();
//      if (status!=VideoConst.PLAY_STATUS_FF || status!=VideoConst.PLAY_STATUS_FR) {
//            return;
//        }
        int speedStep = getManager().getController().getSpeedStep();
        Util.Hlog("status:"+status+"_speedStep_:"+speedStep);
        
//      mHandler.sendEmptyMessage(123321);
        
        vVideoRepeata= mSmallTimerBar.getFastView();
        vPStatePlay = mSmallTimerBar.getIconView();
        
        if (FastForward) {
            onFast(speedStep, 0, Const.FILTER_VIDEO);
            
        }else {
            onFast(speedStep, 1, Const.FILTER_VIDEO);
        }
       
    }
    
    public void onFast(int speed, int status, int type) {
        vVideoRepeata.setVisibility(View.VISIBLE);

        if (speed == 1) {
            speedStep = 1;
            vVideoRepeata.setVisibility(View.GONE);
            return;
        }

        vVideoRepeata.setText(speed + "x");
        Drawable img_left = null;
        if (type == Const.FILTER_VIDEO) {
            vVideoRepeata.setTextColor(Color.WHITE);
        }

        if (status == 0) {
            if (type == Const.FILTER_VIDEO) {
                img_left = mContext.getResources().getDrawable(
                        R.drawable.toolbar_typeicon_ff_video);
            } else {
                img_left = mContext.getResources().getDrawable(
                        R.drawable.toolbar_typeicon_ff);
            }
        } else if (status == 1) {
            if (type == Const.FILTER_VIDEO) {
                img_left = mContext.getResources().getDrawable(
                        R.drawable.toolbar_typeicon_rew_video);
            } else {
                img_left = mContext.getResources().getDrawable(
                        R.drawable.toolbar_typeicon_rew);

            }
        } else if (status == 2) {
            img_left = mContext.getResources().getDrawable(
                    R.drawable.mmp_toolbar_typeicon_video_paly);

            vVideoRepeata.setText("1/" + speed + "x");

        }
        if(img_left != null){
            img_left.setBounds(0, 0, img_left.getMinimumWidth(), img_left
                .getMinimumHeight());
            }
        
        vVideoRepeata.setCompoundDrawables(img_left, null, null, null);
    }
	/**
	 * 
	 */
	private void forwardPlay() {
		getManager().getController().forwardPlay(1);
	}
	/**
	 * 
	 */
	private void rewindPlay() {
		getManager().getController().rewindPlay(1);
	}

	/**
	 * @return the playerOffset
	 */
	public int getPlayerOffset() {
		return playerOffset;
	}

	/**
	 * @param playerOffset the playerOffset to set
	 */
	public void setPlayerOffset(int playerOffset) {
		this.playerOffset = playerOffset;
	}
	public boolean isPlaying(){
	    return isPlaying;
	}
}
