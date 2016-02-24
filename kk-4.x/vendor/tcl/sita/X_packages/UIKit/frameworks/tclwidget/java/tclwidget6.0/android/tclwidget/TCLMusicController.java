/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.graphics.PixelFormat;
import android.media.AudioManager;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.android.internal.policy.PolicyManager;

import java.util.Formatter;
import java.util.Locale;

/**
 * A view containing controls for a MediaPlayer. Typically contains the
 * buttons like "Play/Pause", "Rewind", "Fast Forward" and a progress
 * slider. It takes care of synchronizing the controls with the state
 * of the MediaPlayer.
 * <p>
 * The way to use this class is to instantiate it programatically.
 * The MediaController will create a default set of controls
 * and put them in a window floating above your application. Specifically,
 * the controls will float above the view specified with setAnchorView().
 * The window will disappear if left idle for three seconds and reappear
 * when the user touches the anchor view.
 * <p>
 * Functions like show() and hide() have no effect when MediaController
 * is created in an xml layout.
 * 
 * MediaController will hide and
 * show the buttons according to these rules:
 * <ul>
 * <li> The "previous" and "next" buttons are hidden until setPrevNextListeners()
 *   has been called
 * <li> The "previous" and "next" buttons are visible but disabled if
 *   setPrevNextListeners() was called with null listeners
 * <li> The "rewind" and "fastforward" buttons are shown unless requested
 *   otherwise by using the MediaController(Context, boolean) constructor
 *   with the boolean set to false
 * </ul>
 */
public class TCLMusicController extends FrameLayout {

    private MediaPlayerControl  mPlayer;
    private Context             mContext;
    private View                mAnchor;
    private View                mRoot;
    private WindowManager       mWindowManager;
    private Window              mWindow;
    private View                mDecor;
    private boolean             mShowing;
    private boolean             mDragging;
    private static final int    sDefaultTimeout = 3000;
    private static final int    FADE_OUT = 1;
    private static final int    SHOW_PROGRESS = 2;
    private boolean             mUseFastForward;
    private boolean             mFromXml;
    private boolean             mListenersSet;
 
    private ImageButton         mPauseButton;
    private ImageButton         mFfwdButton;
    private ImageButton         mRewButton;
    private ImageButton         mNextButton;
    private ImageButton         mQuitButton;
    private ImageButton         mPrevButton;
    private ImageButton         mRecycleButton;
    private ImageButton         mVolumeButton;


	private View.OnClickListener mRecycleListener;
	private View.OnClickListener mPauseListener;
	private View.OnClickListener mFfwdListener;
	private View.OnClickListener mRewListener;
	private View.OnClickListener mStopListener;
	private View.OnClickListener mNextListener;
	private View.OnClickListener mPrevListener;
	private View.OnClickListener mVolumeListener;

    public TCLMusicController(Context context, AttributeSet attrs) {
        super(context, attrs);
        mRoot = this;
        mContext = context;
        mUseFastForward = true;
        mFromXml = true;
    }

    @Override
    public void onFinishInflate() {
        if (mRoot != null)
            initControllerView(mRoot);
    }

    public TCLMusicController(Context context, boolean useFastForward) {
        super(context);
        mContext = context;
        mUseFastForward = useFastForward;
        initFloatingWindow();
    }

    public TCLMusicController(Context context) {
        super(context);
        mContext = context;
        mUseFastForward = true;
        initFloatingWindow();
    }

    private void initFloatingWindow() {
        mWindowManager = (WindowManager)mContext.getSystemService("window");
        mWindow = PolicyManager.makeNewWindow(mContext);
        mWindow.setWindowManager(mWindowManager, null, null);
        mWindow.requestFeature(Window.FEATURE_NO_TITLE);
        mDecor = mWindow.getDecorView();
        mDecor.setOnTouchListener(mTouchListener);
        mWindow.setContentView(this);
        mWindow.setBackgroundDrawableResource(android.R.color.transparent);
        
        // While the media controller is up, the volume control keys should
        // affect the media stream type
        mWindow.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        setFocusable(true);
        setFocusableInTouchMode(true);
        setDescendantFocusability(ViewGroup.FOCUS_AFTER_DESCENDANTS);
        requestFocus();
    }

    private OnTouchListener mTouchListener = new OnTouchListener() {
        public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                if (mShowing) {
                    hide();
                }
            }
            return false;
        }
    };
    
	/**
     * Sets the MediaPlayer of MediaPlayerControl.
	 * @author TCL TVOS Team
	 * @param player The player to which to control.
     */
    public void setMediaPlayer(MediaPlayerControl player) {
        mPlayer = player;
        updatePausePlay();
    }

    /**
     * Set the view that acts as the anchor for the control view.
	 * @author TCL TVOS Team
     * This can for example be a VideoView, or your Activity's main view.
     * @param view The view to which to anchor the controller when it is visible.
     */
    public void setAnchorView(View view) {
        mAnchor = view;

        FrameLayout.LayoutParams frameParams = new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        );

        removeAllViews();
        View v = makeControllerView();
        addView(v, frameParams);
    }

    /**
     * Create the view that holds the widgets that control playback.
     * Derived classes can override this to create their own.
     * @return The controller view.
     * @hide This doesn't work as advertised
     */
    protected View makeControllerView() {
        LayoutInflater inflate = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mRoot = inflate.inflate(tvos.R.layout.tcl_music_controller, null);

        initControllerView(mRoot);

        return mRoot;
    }

    private void initControllerView(View v) {
    	mRecycleButton = (ImageButton) v.findViewById(tvos.R.id.recycle);
    	if (mRecycleButton != null) {
    		mRecycleButton.setImageResource(tvos.R.drawable.tcl_btn_normal);
    		mRecycleButton.setOnClickListener(mRecycleListener);
    	}
    	
    	mQuitButton = (ImageButton) v.findViewById(tvos.R.id.close);
        if (mQuitButton != null) {
        	mQuitButton.setOnClickListener(mStopListener);
        }
        
        mVolumeButton = (ImageButton) v.findViewById(tvos.R.id.volume);
        if (mVolumeButton != null) {
        	mVolumeButton.setOnClickListener(mVolumeListener);
        }
        
        
        mPauseButton = (ImageButton) v.findViewById(tvos.R.id.media_controller_pause);
        if (mPauseButton != null) {
            mPauseButton.requestFocus();
            mPauseButton.setOnClickListener(mPauseListener);
        }

        mFfwdButton = (ImageButton) v.findViewById(tvos.R.id.ff);
        if (mFfwdButton != null) {
            mFfwdButton.setOnClickListener(mFfwdListener);
            if (!mFromXml) {
                mFfwdButton.setVisibility(mUseFastForward ? View.VISIBLE : View.GONE);
            }
        }

        mRewButton = (ImageButton) v.findViewById(tvos.R.id.frw);
        if (mRewButton != null) {
            mRewButton.setOnClickListener(mRewListener);
            if (!mFromXml) {
                mRewButton.setVisibility(mUseFastForward ? View.VISIBLE : View.GONE);
            }
        }

        // By default these are hidden. They will be enabled when setPrevNextListeners() is called 
        mNextButton = (ImageButton) v.findViewById(tvos.R.id.next);
        if (mNextButton != null) {
        	mNextButton.setOnClickListener(mNextListener);
        }
        mPrevButton = (ImageButton) v.findViewById(tvos.R.id.prev);
        if (mPrevButton != null) {
        	 mPrevButton.setOnClickListener(mPrevListener);
        }

    }

    /**
     * Show the controller on screen. It will go away
     * automatically after 3 seconds of inactivity.
	 * @author TCL TVOS Team
     */
    public void show() {
        show(sDefaultTimeout);
    }

    /**
     * Disable pause or seek buttons if the stream cannot be paused or seeked.
     * This requires the control interface to be a MediaPlayerControlExt
     */
    private void disableUnsupportedButtons() {
        try {
            if (mPauseButton != null && !mPlayer.canPause()) {
                mPauseButton.setEnabled(false);
            }
            if (mRewButton != null && !mPlayer.canSeekBackward()) {
                mRewButton.setEnabled(false);
            }
            if (mFfwdButton != null && !mPlayer.canSeekForward()) {
                mFfwdButton.setEnabled(false);
            }
        } catch (IncompatibleClassChangeError ex) {
            // We were given an old version of the interface, that doesn't have
            // the canPause/canSeekXYZ methods. This is OK, it just means we
            // assume the media can be paused and seeked, and so we don't disable
            // the buttons.
        }
    }
    
    /**
     * Show the controller on screen. It will go away
     * automatically after 'timeout' milliseconds of inactivity.
	 * @author TCL TVOS Team
     * @param timeout The timeout in milliseconds. Use 0 to show
     * the controller until hide() is called.
     */
    public void show(int timeout) {

        if (!mShowing && mAnchor != null) {
            if (mPauseButton != null) {
                mPauseButton.requestFocus();
            }
            disableUnsupportedButtons();

            int [] anchorpos = new int[2];
            mAnchor.getLocationOnScreen(anchorpos);

            WindowManager.LayoutParams p = new WindowManager.LayoutParams();
            p.gravity = Gravity.TOP;
            p.width = mAnchor.getWidth();
            p.height = LayoutParams.WRAP_CONTENT;
            p.x = 0;
            p.y = anchorpos[1] + mAnchor.getHeight() - p.height;
            p.format = PixelFormat.TRANSLUCENT;
            p.type = WindowManager.LayoutParams.TYPE_APPLICATION_PANEL;
            p.flags |= WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM;
            p.token = null;
            p.windowAnimations = 0; // android.R.style.DropDownAnimationDown;
            mWindowManager.addView(mDecor, p);
            mShowing = true;
        }
        updatePausePlay();
        
        // cause the progress bar to be updated even if mShowing
        // was already true.  This happens, for example, if we're
        // paused with the progress bar showing the user hits play.
        //mHandler.sendEmptyMessage(SHOW_PROGRESS);

        Message msg = mHandler.obtainMessage(FADE_OUT);
        if (timeout != 0) {
            mHandler.removeMessages(FADE_OUT);
            mHandler.sendMessageDelayed(msg, timeout);
        }
    }
    
    /**
     * <p>Indicate whether this controller window is showing on screen.</p>
	 * @author TCL TVOS Team
     * @return true if the controller is showing, false otherwise.
     */
    public boolean isShowing() {
        return mShowing;
    }

    /**
     * Remove the controller from the screen.
	 * @author TCL TVOS Team
     */
    public void hide() {
        if (mAnchor == null)
            return;

        if (mShowing) {
            try {
                //mHandler.removeMessages(SHOW_PROGRESS);
                mWindowManager.removeView(mDecor);
            } catch (IllegalArgumentException ex) {
                Log.w("MediaController", "already removed");
            }
            mShowing = false;
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            int pos;
            switch (msg.what) {
                case FADE_OUT:
                    hide();
                    break;           
            }
        }
    };

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        show(sDefaultTimeout);
        return true;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent ev) {
        show(sDefaultTimeout);
        return false;
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        if (event.getRepeatCount() == 0 && event.isDown() && (
                keyCode ==  KeyEvent.KEYCODE_HEADSETHOOK ||
                keyCode ==  KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE ||
                keyCode ==  KeyEvent.KEYCODE_SPACE)) {
            doPauseResume();
            show(sDefaultTimeout);
            if (mPauseButton != null) {
                mPauseButton.requestFocus();
            }
            return true;
        } else if (keyCode ==  KeyEvent.KEYCODE_MEDIA_STOP) {
            if (mPlayer.isPlaying()) {
                mPlayer.pause();
                updatePausePlay();
            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
                keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
            // don't show the controls for volume adjustment
            return super.dispatchKeyEvent(event);
        } else if (keyCode == KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_MENU) {
            hide();

            return true;
        } else {
            show(sDefaultTimeout);
        }
        return super.dispatchKeyEvent(event);
    }


    private void updatePausePlay() {
        if (mRoot == null || mPauseButton == null)
            return;

        if (mPlayer.isPlaying()) {
            mPauseButton.setImageResource(tvos.R.drawable.tcl_btn_pause);
        } else {
            mPauseButton.setImageResource(tvos.R.drawable.tcl_btn_play);
        }
    }

    private void doPauseResume() {
        if (mPlayer.isPlaying()) {
            mPlayer.pause();
        } else {
            mPlayer.start();
        }
        updatePausePlay();
    }

    
    @Override
    public void setEnabled(boolean enabled) {
        if (mPauseButton != null) {
            mPauseButton.setEnabled(enabled);
        }
        if (mFfwdButton != null) {
            mFfwdButton.setEnabled(enabled);
        }
        if (mRewButton != null) {
            mRewButton.setEnabled(enabled);
        }
        if (mNextButton != null) {
            mNextButton.setEnabled(enabled);
        }
        if (mPrevButton != null) {
            mPrevButton.setEnabled(enabled);
        }
        
        disableUnsupportedButtons();
        super.setEnabled(enabled);
    }

    public interface MediaPlayerControl {
        void    start();
        void    pause();
        int     getDuration();
        int     getCurrentPosition();
        void    seekTo(int pos);
        boolean isPlaying();
        int     getBufferPercentage();
        boolean canPause();
        boolean canSeekBackward();
        boolean canSeekForward();
    }

    /**
     * Sets the RecycleListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param recycleListener The callback that will run
     */
	public void setRecycleListener(View.OnClickListener recycleListener){
		this.mRecycleListener = recycleListener;
	}

    /**
     * Gets the RecycleListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the RecycleListener of MediaPlayerControl. 
     */
	public View.OnClickListener getRecycleListener(){
		return mRecycleListener;
	}

    /**
     * Sets the VolumeListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param volumeListener The callback that will run
     */
	public void setVolumeListener(View.OnClickListener volumeListener){
		this.mVolumeListener = volumeListener;
	}

    /**
     * Gets the VolumeListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the VolumeListener of MediaPlayerControl. 
     */
	public View.OnClickListener getVolumeListener(){
		return mVolumeListener;
	}

    /**
     * Sets the PrevListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param prevListener The callback that will run
     */
	public void setPrevListener(View.OnClickListener prevListener){
		this.mPrevListener = prevListener;
	}

    /**
     * Gets the PrevListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the PrevListener of MediaPlayerControl. 
     */
	public View.OnClickListener getPrevListener(){
		return mPrevListener;
	}

    /**
     * Sets the NextListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param nextListener The callback that will run
     */
	public void setNextListener(View.OnClickListener nextListener){
		this.mNextListener = nextListener;
	}

    /**
     * Gets the NextListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the NextListener of MediaPlayerControl. 
     */
	public View.OnClickListener getNextListener(){
		return mNextListener;
	}

    /**
     * Sets the StopListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param stopListener The callback that will run
     */
	public void setStopListener(View.OnClickListener stopListener){
		this.mStopListener = stopListener;
	}

    /**
     * Gets the StopListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the StopListener of MediaPlayerControl. 
     */
	public View.OnClickListener getStopListener(){
		return mStopListener;
	}

    /**
     * Sets the RewListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param rewListener The callback that will run
     */
	public void setRewListener(View.OnClickListener rewListener){
		this.mRewListener = rewListener;
	}

    /**
     * Gets the RewListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the RewListener of MediaPlayerControl. 
     */
	public View.OnClickListener getRewListener(){
		return mRewListener;
	}

    /**
     * Sets the FfwdListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param ffwdListener The callback that will run
     */
	public void setFfwdListener(View.OnClickListener ffwdListener){
		this.mFfwdListener = ffwdListener;
	}

    /**
     * Gets the FfwdListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the FfwdListener of MediaPlayerControl. 
     */
	public View.OnClickListener getFfwdListener(){
		return mFfwdListener;
	}

    /**
     * Sets the PauseListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param pauseListener The callback that will run
     */
	public void setPauseListener(View.OnClickListener pauseListener){
		this.mPauseListener = pauseListener;
	}

   /**
     * Gets the PauseListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the PauseListener of MediaPlayerControl. 
     */
	public View.OnClickListener getPauseListener(){
		return mPauseListener;
	}
	
	/**
     * Sets the RecycleButtonIcons of MediaPlayerControl. 
	 * @author TCL TVOS Team
	 * @param mode
	 *  0  - com.android.internal.R.drawable.tcl_btn_normal
	 *  1  - com.android.internal.R.drawable.tcl_btn_re_one
	 *  2  - com.android.internal.R.drawable.tcl_btn_re_all
	 */
	public void setRecycleButtonIcons(int mode){
		if (mRecycleButton != null){
			switch(mode){

				case 0:
					mRecycleButton.setImageResource(tvos.R.drawable.tcl_btn_normal);
				case 1:
					mRecycleButton.setImageResource(tvos.R.drawable.tcl_btn_re_one);
				case 2:
					mRecycleButton.setImageResource(tvos.R.drawable.tcl_btn_re_all);
			}			
		}
	}
}

