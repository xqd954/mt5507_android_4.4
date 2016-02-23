package android.media;

import android.util.Log;

/**
 * SubtitleAttr Class can be used to get subtitle attribute info 
 * 
 * 
 */
public class SubtitleAttr {

	private final static String TAG = "SubtitleAttr";

    private static final int FLAG_TIME_OFFSET        = 1 << 0;
    private static final int FLAG_SHOW_HIDE          = 1 << 1;
    private static final int FLAG_FONT_INFO          = 1 << 2;
    private static final int FLAG_BG_COLOR           = 1 << 3;
    private static final int FLAG_TEXT_COLOR         = 1 << 4;
	private static final int FLAG_EDGE_COLOR         = 1 << 5;
    private static final int FLAG_DISPLAY_RECT       = 1 << 6;
    private static final int FLAG_DISPLAY_OFFSET     = 1 << 7;
    
    /**
     * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
     */
    public SubtitleAttr() {

    }
    
    /**
     * Subtitle_TimeOffset Class can be used to get subtitle time offset info 
     * 
     * 
     */
    public class Subtitle_TimeOffset {

        public static final int OFFSET_TYPE_AUTO        = 1;
        public static final int OFFSET_TYPE_USER_DEF    = 2;

        private int mTimeOffsetType;
        private int mOffsetValue;

        public Subtitle_TimeOffset(int offsetType, int offsetValue) {
            super();
            this.mTimeOffsetType = offsetType;
            this.mOffsetValue = offsetValue;
        }

    }
    
    /**
     * Subtitle_FontInfo Class can be used to get subtitle font info 
     * 
     * 
     */
    public class Subtitle_FontInfo {

        /* font size */
        public static final int SIZE_TYPE_SMALL    = 0;
        public static final int SIZE_TYPE_MEDIUM   = 1;
        public static final int SIZE_TYPE_LARGE    = 2;
        public static final int SIZE_TYPE_CUSTOM   = 3;

        private int mFontSizeType;
        private byte mCustomSize; // mCustomSize is valid only if mFontSizeType==SIZE_TYPE_CUSTOM
        
        /**
         * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
         * when the param "fontSizeType" == SIZE_TYPE_CUSTOM
         * the param "customSize" should be set by your interested value.
         * else customSize=0 is just ok.
         */
        public Subtitle_FontInfo(int fontSizeType, byte customSize) {
            super();
            this.mFontSizeType = fontSizeType;

			if (fontSizeType == SIZE_TYPE_CUSTOM) {
				this.mCustomSize = customSize;
			} else {
				this.mCustomSize = 0;
			}
        }
    }
    
    /**
     * Subtitle_Color Class can be used to get subtitle color info 
     * 
     * 
     */
    public class Subtitle_Color {
        private byte mA;
        private byte mR;
        private byte mG;
        private byte mB;
        
        /**
          * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
          */
        public Subtitle_Color(byte a, byte r, byte g, byte b) {
            super();
            this.mA = a;
            this.mR = r;
            this.mG = g;
            this.mB = b;
        }
    }
    
    /**
     * Subtitle_Color Class can be used to get subtitle display rect info 
     * 
     * 
     */
    public class Subtitle_DisplayRect {

        private int mX;
        private int mY;
        private int mW;
        private int mH;
        
        /**
          * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
          */
        public Subtitle_DisplayRect(int u4x, int u4y, int u4w, int u4h) {
            super();
            mX = u4x;
            mY = u4y;
            mW = u4w;
            mH = u4h;
        }

    }
    /**
     * Subtitle_DisplayOffset Class can be used to get subtitle display offset info 
     * 
     * 
     */
    public class Subtitle_DisplayOffset {

        private int mX;
        private int mY;
        
        /**
          * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
          */
        public Subtitle_DisplayOffset(int u4x, int u4y) {
            super();
            mX = u4x;
            mY = u4y;
        }
    }
    
    /**
     * This API used to setTimeOffset. 
     */
    public void setTimeOffset(Subtitle_TimeOffset timeOffset) {
		Log.i(TAG, "setTimeOffset(): Enter");

        this.mTimeOffset = timeOffset;
        mMask |= FLAG_TIME_OFFSET;

        Log.i(TAG, "setTimeOffset(): mMask=" + mMask);
		Log.i(TAG, "setTimeOffset(): Leave");
    }
    
    /**
     * This API used to setShowHide. 
     */
    public void setShowHide(boolean isShow) {
		Log.i(TAG, "setShowHide(): Enter");

		Log.i(TAG, "setShowHide(): isShow=" + isShow);

        this.mIsShow = isShow;
        mMask |= FLAG_SHOW_HIDE;

        Log.i(TAG, "setShowHide(): mMask=" + mMask);
		Log.i(TAG, "setShowHide(): Leave");
    }

    /**
     * This API used to setFontInfo. 
     */
    public void setFontInfo(Subtitle_FontInfo fontInfo) {
		Log.i(TAG, "setFontInfo(): Enter");

        this.mFontInfo = fontInfo;
        mMask |= FLAG_FONT_INFO;

		Log.i(TAG, "setFontInfo(): mMask=" + mMask);
		Log.i(TAG, "setFontInfo(): Leave");
    }

    /**
     * This API used to setBgColor. 
     */
    public void setBgColor(Subtitle_Color bgColor) {
		Log.i(TAG, "setBgColor(): Enter");

        this.mBgColor = bgColor;
        mMask |= FLAG_BG_COLOR;

		Log.i(TAG, "setBgColor(): mMask=" + mMask);
		Log.i(TAG, "setBgColor(): Leave");
    }
    
    /**
     * This API used to setTextColor. 
     */
    public void setTextColor(Subtitle_Color textColor) {
		Log.i(TAG, "setTextColor(): Enter");

        this.mTextColor = textColor;
        mMask |= FLAG_TEXT_COLOR;

		Log.i(TAG, "setTextColor(): mMask=" + mMask);
		Log.i(TAG, "setTextColor(): Leave");
    }
    
    /**
     * This API used to setEdgeColor. 
     */
	public void setEdgeColor(Subtitle_Color edgeColor) {
		Log.i(TAG, "setEdgeColor(): Enter");

        this.mEdgColor= edgeColor;
        mMask |= FLAG_EDGE_COLOR;

		Log.i(TAG, "setEdgeColor(): mMask=" + mMask);
		Log.i(TAG, "setEdgeColor(): Leave");
    }
    
    /**
     * This API used to setDisplayRect. 
     */
    public void setDisplayRect(Subtitle_DisplayRect displayRect) {
		Log.i(TAG, "setDisplayRect(): Enter");

        this.mDisplayRect = displayRect;
        mMask |= FLAG_DISPLAY_RECT;

		Log.i(TAG, "setDisplayRect(): mMask=" + mMask);
		Log.i(TAG, "setDisplayRect(): Leave");
    }
    
    /**
     * This API used to setDisplayOffset. 
     */
    public void setDisplayOffset(Subtitle_DisplayOffset displayOffset) {
		Log.i(TAG, "setDisplayOffset(): Enter");

        this.mDisplayOffset = displayOffset;
        mMask |= FLAG_DISPLAY_OFFSET;

		Log.i(TAG, "setDisplayOffset(): mMask=" + mMask);
		Log.i(TAG, "setDisplayOffset(): Leave");
    }
    
    /**
     * This API used to get isValid . 
     */
    public boolean isValid() {
    	Log.i(TAG, "isValid(): Occur");
		Log.i(TAG, "isValid(): mMask=" + mMask);

        return mMask != 0;
    }

	private int mMask = 0;
	private boolean mIsShow;
    private Subtitle_TimeOffset    mTimeOffset;
    private Subtitle_FontInfo      mFontInfo;
    private Subtitle_Color         mBgColor;
    private Subtitle_Color         mTextColor;
	private Subtitle_Color         mEdgColor;
    private Subtitle_DisplayRect   mDisplayRect;
    private Subtitle_DisplayOffset mDisplayOffset;

}
