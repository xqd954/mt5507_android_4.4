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
public class TCLAlbumController extends FrameLayout {

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
    private ImageButton         mTurnRButton;
    private ImageButton         mTurnLButton;
    private ImageButton         mNextButton;
    private ImageButton         mPrevButton;
    private ImageButton         mZoomUButton;
    private ImageButton         mZoomDButton;
    private ImageButton         mInfoButton;
    private ImageButton         mMusicButton;
    private ImageButton         mSetButton;

	private View.OnClickListener mSetListener;
	private View.OnClickListener mPauseListener;
	private View.OnClickListener mTurnRListener;
	private View.OnClickListener mTurnLListener;
	private View.OnClickListener mStopListener;
	private View.OnClickListener mNextListener;
	private View.OnClickListener mPrevListener;
	private View.OnClickListener mMusicListener;
	private View.OnClickListener mInfoListener;
	private View.OnClickListener mZoomDListener;
	private View.OnClickListener mZoomUListener;

    public TCLAlbumController(Context context, AttributeSet attrs) {
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

    public TCLAlbumController(Context context, boolean useFastForward) {
        super(context);
        mContext = context;
        mUseFastForward = useFastForward;
        initFloatingWindow();
    }

    public TCLAlbumController(Context context) {
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
        mRoot = inflate.inflate(com.android.internal.R.layout.tcl_album_controller, null);

        initControllerView(mRoot);

        return mRoot;
    }

    private void initControllerView(View v) {
    	mSetButton = (ImageButton) v.findViewById(com.android.internal.R.id.set);
    	if (mSetButton != null) {
    		mSetButton.setOnClickListener(mSetListener);
    	}
    	
    	mMusicButton = (ImageButton) v.findViewById(com.android.internal.R.id.music);
        if (mMusicButton != null) {
        	mMusicButton.setOnClickListener(mMusicListener);
        }
        
        mInfoButton = (ImageButton) v.findViewById(com.android.internal.R.id.info);
        if (mInfoButton != null) {
        	mInfoButton.setOnClickListener(mInfoListener);
        }
        
        
        mPauseButton = (ImageButton) v.findViewById(com.android.internal.R.id.media_controller_pause);
        if (mPauseButton != null) {
            mPauseButton.requestFocus();
            mPauseButton.setOnClickListener(mPauseListener);
        }

        mZoomDButton = (ImageButton) v.findViewById(com.android.internal.R.id.zoom_down);
        if (mZoomDButton != null) {
            mZoomDButton.setOnClickListener(mZoomDListener);

        }

        mZoomUButton = (ImageButton) v.findViewById(com.android.internal.R.id.zoom_up);
        if (mZoomUButton != null) {
            mZoomUButton.setOnClickListener(mZoomUListener);
   
        }

        // By default these are hidden. They will be enabled when setPrevNextListeners() is called 
        mNextButton = (ImageButton) v.findViewById(com.android.internal.R.id.next);
        if (mNextButton != null) {
        	mNextButton.setOnClickListener(mNextListener);
        }
        mPrevButton = (ImageButton) v.findViewById(com.android.internal.R.id.prev);
        if (mPrevButton != null) {
        	 mPrevButton.setOnClickListener(mPrevListener);
        }
        mTurnRButton = (ImageButton) v.findViewById(com.android.internal.R.id.turn_right);
        if (mTurnRButton != null) {
        	mTurnRButton.setOnClickListener(mTurnRListener);
        }
        mTurnLButton = (ImageButton) v.findViewById(com.android.internal.R.id.turn_left);
        if (mTurnLButton != null) {
        	 mTurnLButton.setOnClickListener(mTurnLListener);
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
            mPauseButton.setImageResource(com.android.internal.R.drawable.tcl_btn_pause);
        } else {
            mPauseButton.setImageResource(com.android.internal.R.drawable.tcl_btn_play);
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
     * Sets the SetListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param setListener The callback that will run
     */
	public void setSetListener(View.OnClickListener setListener){
		this.mSetListener = setListener;
	}

    /**
     * Gets the SetListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the SetListener of MediaPlayerControl. 
     */
	public View.OnClickListener getSetListener(){
		return mSetListener;
	}

    /**
     * Sets the InfoListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param infoListener The callback that will run
     */
	public void setInfoListener(View.OnClickListener infoListener){
		this.mInfoListener = infoListener;
	}

    /**
     * Gets the InfoListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the InfoListener of InfoListener. 
     */
	public View.OnClickListener getInfoListener(){
		return mInfoListener;
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
     * Sets the ZoomUListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param zoomUListener The callback that will run
     */
	public void setZoomUListener(View.OnClickListener zoomUListener){
		this.mZoomUListener = zoomUListener;
	}

    /**
     * Gets the ZoomUListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the ZoomUListener of MediaPlayerControl. 
     */
	public View.OnClickListener getZoomUListener(){
		return mZoomUListener;
	}

    /**
     * Sets the ZoomDListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param zoomDListener The callback that will run
     */
	public void setZoomDListener(View.OnClickListener zoomDListener){
		this.mZoomDListener = zoomDListener;
	}

    /**
     * Gets the ZoomDListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the ZoomDListener of MediaPlayerControl. 
     */
	public View.OnClickListener getZoomDListener(){
		return mZoomDListener;
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
     * Sets the MusicListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param musicListener The callback that will run
     */
	public void setMusicListener(View.OnClickListener musicListener){
		this.mMusicListener = musicListener;
	}

   /**
     * Gets the MusicListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the MusicListener of MediaPlayerControl. 
     */
	public View.OnClickListener getMusicListener(){
		return mMusicListener;
	}
	
	/**
     * Sets the TurnRListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param turnRListener The callback that will run
     */
	public void setTurnRListener(View.OnClickListener turnRListener){
		this.mTurnRListener = turnRListener;
	}

   /**
     * Gets the TurnRListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the TurnRListener of MediaPlayerControl. 
     */
	public View.OnClickListener getTurnRListener(){
		return mTurnRListener;
	}
	
	/**
     * Sets the TurnLListener of MediaPlayerControl. 
	 * @author TCL TVOS Team
     * @param turnLListener The callback that will run
     */
	public void setTurnLListener(View.OnClickListener turnLListener){
		this.mTurnLListener = turnLListener;
	}

   /**
     * Gets the TurnLListener of MediaPlayerControl. 
     * @author TCL TVOS Team
     * @return View.OnClickListener the TurnLListener of MediaPlayerControl. 
     */
	public View.OnClickListener getTurnLListener(){
		return mTurnLListener;
	}

}

