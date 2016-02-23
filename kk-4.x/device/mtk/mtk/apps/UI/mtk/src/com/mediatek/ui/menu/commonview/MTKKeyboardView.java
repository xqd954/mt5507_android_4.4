package com.mediatek.ui.menu.commonview;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.Region.Op;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.PopupWindow;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.MTKKeyboard.Key;

/**
 * channel edit_nameEdit keyboard
 * 
 * @author hs_haosun
 * 
 */
public class MTKKeyboardView extends View implements View.OnClickListener {

	private static final int NOT_A_KEY = -1;
	private static final int[] KEY_DELETE = { MTKKeyboard.KEYCODE_DELETE };

	private MTKKeyboard mKeyboard;
	private int mCurrentKeyIndex = 0;
	private int mLabelTextSize;
	private int mKeyTextSize;
	private int mKeyTextColor;
	private float mShadowRadius;
	private int mShadowColor;
	private float mBackgroundDimAmount;
	private int[] mOffsetInWindow;

	private PopupWindow mPopupKeyboard;
	private View mMiniKeyboardContainer;
	private MTKKeyboardView mMiniKeyboard;
	private boolean mMiniKeyboardOnScreen;
	private int mMiniKeyboardOffsetX;
	private int mMiniKeyboardOffsetY;
	private Map<Key, View> mMiniKeyboardCache;
	private int[] mWindowOffset;
	private Key[] mKeys;

	private static final int MSG_SHOW_PREVIEW = 1;
	private static final int MSG_REMOVE_PREVIEW = 2;
	private static final int MSG_REPEAT = 3;
	private static final int MSG_LONGPRESS = 4;
	private int mVerticalCorrection;
	private int mProximityThreshold;
	private boolean mShowPreview = true;

	private int mLastX;
	private int mLastY;
	private int mStartX;
	private int mStartY;
	private boolean mProximityCorrectOn;
	private Paint mPaint;
	private Rect mPadding;

	private long mDownTime;
	private long mLastMoveTime;
	private int mLastKey;
	private int mLastCodeX;
	private int mLastCodeY;
	private int mCurrentKey = 0;
	private int mDownKey = NOT_A_KEY;
	private long mLastKeyTime;
	private long mCurrentKeyTime;
	private int[] mKeyIndices = new int[12];
	private GestureDetector mGestureDetector;
	private int mPopupX;
	private int mPopupY;
	private int mRepeatKeyIndex = NOT_A_KEY;
	private int mPopupLayout;
	private boolean mAbortKey;
	private Key mInvalidatedKey;
	private Rect mClipRegion = new Rect(0, 0, 0, 0);
	private boolean mPossiblePoly;
	private SwipeTracker mSwipeTracker = new SwipeTracker();
	private int mSwipeThreshold;
	private boolean mDisambiguateSwipe;
	private int mOldPointerCount = 1;
	private float mOldPointerX;
	private float mOldPointerY;
	private Drawable mKeyBackground;

	private static final int REPEAT_INTERVAL = 50;
	private static final int REPEAT_START_DELAY = 400;
	private static final int LONGPRESS_TIMEOUT = ViewConfiguration
			.getLongPressTimeout();
	private static int MAX_NEARBY_KEYS = 12;
	private int[] mDistances = new int[MAX_NEARBY_KEYS];

	private int mLastSentIndex;
	private int mTapCount;
	private long mLastTapTime;
	private boolean mInMultiTap;
	private static final int MULTITAP_INTERVAL = 800;
	private boolean mDrawPending;
	private Rect mDirtyRect = new Rect();
	private Bitmap mBuffer;
	private boolean mKeyboardChanged;
	private Canvas mCanvas;

	private OnKeyboardActionListener mKeyboardActionListener;

	public interface OnKeyboardActionListener {
		void onPress(int primaryCode);

		void onRelease(int primaryCode);

		void onKey(int primaryCode, int[] keyCodes);

		void onText(CharSequence text);
	}

	Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MSG_SHOW_PREVIEW:
				showKey(msg.arg1);
				break;
			case MSG_REPEAT:
				if (repeatKey()) {
					Message repeat = Message.obtain(this, MSG_REPEAT);
					sendMessageDelayed(repeat, REPEAT_INTERVAL);
				}
				break;
			case MSG_LONGPRESS:
				openPopupIfRequired((MotionEvent) msg.obj);
				break;
			}
		}
	};

	private int mCurrentRow = 0;
	private int mCurrentColumn = 0;

	public MTKKeyboardView(Context context, AttributeSet attrs) {
		this(context, attrs, R.attr.keyboardViewStyle);
	}

	public MTKKeyboardView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		TypedArray a = context.obtainStyledAttributes(attrs,
				R.styleable.KeyboardView, defStyle, 0);

		int previewLayout = 0;
		int keyTextSize = 0;

		int n = a.getIndexCount();

		for (int i = 0; i < n; i++) {
			int attr = a.getIndex(i);

			switch (attr) {
			case R.styleable.KeyboardView_keyBackground:
				mKeyBackground = a.getDrawable(attr);
				break;
			case R.styleable.KeyboardView_verticalCorrection:
				mVerticalCorrection = a.getDimensionPixelOffset(attr, 0);
				break;
			case R.styleable.KeyboardView_keyPreviewLayout:
				previewLayout = a.getResourceId(attr, 0);
				break;
			case R.styleable.KeyboardView_keyTextSize:
				mKeyTextSize = a.getDimensionPixelSize(attr, 18);
				break;
			case R.styleable.KeyboardView_keyTextColor:
				mKeyTextColor = a.getColor(attr, 0xFF000000);
				break;
			case R.styleable.KeyboardView_labelTextSize:
				mLabelTextSize = a.getDimensionPixelSize(attr, 14);
				break;
			case R.styleable.KeyboardView_popupLayout:
				mPopupLayout = a.getResourceId(attr, 0);
				break;
			case R.styleable.KeyboardView_shadowColor:
				mShadowColor = a.getColor(attr, 0);
				break;
			case R.styleable.KeyboardView_shadowRadius:
				mShadowRadius = a.getFloat(attr, 0f);
				break;
			}
		}

		// a = context.obtainStyledAttributes(
		// com.android.internal.R.styleable.Theme);
		// mBackgroundDimAmount =
		// a.getFloat(android.R.styleable.Theme_backgroundDimAmount, 0.5f);
		mBackgroundDimAmount = 0.5f;

		mPopupKeyboard = new PopupWindow(context);
		mPopupKeyboard.setBackgroundDrawable(null);
		mPaint = new Paint();
		mPaint.setAntiAlias(true);
		mPaint.setTextSize(keyTextSize);
		mPaint.setTextAlign(Align.CENTER);
		mPaint.setAlpha(255);

		mPadding = new Rect(0, 0, 0, 0);
		mMiniKeyboardCache = new HashMap<Key, View>();
		if (mKeyBackground != null) {
			mKeyBackground.getPadding(mPadding);
		}

		mSwipeThreshold = (int) (500 * getResources().getDisplayMetrics().density);
		mDisambiguateSwipe = false;
		resetMultiTap();
		initGestureDetector();
	}

	private void initGestureDetector() {
		mGestureDetector = new GestureDetector(getContext(),
				new GestureDetector.SimpleOnGestureListener() {
					@Override
					public boolean onFling(MotionEvent me1, MotionEvent me2,
							float velocityX, float velocityY) {
						if (mPossiblePoly)
							return false;
						final float absX = Math.abs(velocityX);
						final float absY = Math.abs(velocityY);
						float deltaX = me2.getX() - me1.getX();
						float deltaY = me2.getY() - me1.getY();
						int travelX = getWidth() / 2;
						int travelY = getHeight() / 2;
						// height
						mSwipeTracker.computeCurrentVelocity(1000);
						final float endingVelocityX = mSwipeTracker
								.getXVelocity();
						final float endingVelocityY = mSwipeTracker
								.getYVelocity();
						boolean sendDownKey = false;
						if (velocityX > mSwipeThreshold && absY < absX
								&& deltaX > travelX) {
							if (mDisambiguateSwipe
									&& endingVelocityX < velocityX / 4) {
								sendDownKey = true;
							} else {

								return true;
							}
						} else if (velocityX < -mSwipeThreshold && absY < absX
								&& deltaX < -travelX) {
							if (mDisambiguateSwipe
									&& endingVelocityX > velocityX / 4) {
								sendDownKey = true;
							} else {

								return true;
							}
						} else if (velocityY < -mSwipeThreshold && absX < absY
								&& deltaY < -travelY) {
							if (mDisambiguateSwipe
									&& endingVelocityY > velocityY / 4) {
								sendDownKey = true;
							} else {

								return true;
							}
						} else if (velocityY > mSwipeThreshold
								&& absX < absY / 2 && deltaY > travelY) {
							if (mDisambiguateSwipe
									&& endingVelocityY < velocityY / 4) {
								sendDownKey = true;
							} else {

								return true;
							}
						}

						if (sendDownKey) {
							detectAndSendKey(mDownKey, mStartX, mStartY,
									me1.getEventTime());
						}
						return false;
					}
				});

		mGestureDetector.setIsLongpressEnabled(false);
	}

	public void setOnKeyboardActionListener(OnKeyboardActionListener listener) {
		mKeyboardActionListener = listener;
	}

	protected OnKeyboardActionListener getOnKeyboardActionListener() {
		return mKeyboardActionListener;
	}

	public void setKeyboard(MTKKeyboard keyboard) {
		if (mKeyboard != null) {
			showPreview(NOT_A_KEY);
		}

		removeMessages();
		mKeyboard = keyboard;
		List<Key> keys = mKeyboard.getKeys();
		mKeys = keys.toArray(new Key[keys.size()]);
		requestLayout();

		mKeyboardChanged = true;
		invalidateAllKeys();
		computeProximityThreshold(keyboard);
		mMiniKeyboardCache.clear();
		mAbortKey = true;
	}

	public MTKKeyboard getKeyboard() {
		return mKeyboard;
	}

	public boolean setShifted(boolean shifted) {
		if (mKeyboard != null) {
			if (mKeyboard.setShifted(shifted)) {
				invalidateAllKeys();
				return true;
			}
		}
		return false;
	}

	public boolean isShifted() {
		if (mKeyboard != null) {
			return mKeyboard.isShifted();
		}
		return false;
	}

	public void setPreviewEnabled(boolean previewEnabled) {
		mShowPreview = previewEnabled;
	}

	public boolean isPreviewEnabled() {
		return mShowPreview;
	}

	public void setVerticalCorrection(int verticalOffset) {

	}

	public void setPopupOffset(int x, int y) {
		mMiniKeyboardOffsetX = x;
		mMiniKeyboardOffsetY = y;
	}

	public void setProximityCorrectionEnabled(boolean enabled) {
		mProximityCorrectOn = enabled;
	}

	public boolean isProximityCorrectionEnabled() {
		return mProximityCorrectOn;
	}

	public void onClick(View v) {
		dismissPopupKeyboard();
	}

	private CharSequence adjustCase(CharSequence label) {
		if (mKeyboard.isShifted() && label != null && label.length() < 3
				&& Character.isLowerCase(label.charAt(0))) {
			label = label.toString().toUpperCase();
		}
		return label;
	}

	@Override
	public void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		if (mKeyboard == null) {
			setMeasuredDimension(getPaddingLeft() + getPaddingRight(),
					getPaddingTop() + getPaddingBottom());
		} else {
			int width = mKeyboard.getMinWidth() + getPaddingLeft()
					+ getPaddingRight();
			if (MeasureSpec.getSize(widthMeasureSpec) < width + 10) {
				width = MeasureSpec.getSize(widthMeasureSpec);
			}
			setMeasuredDimension(width, mKeyboard.getHeight() + getPaddingTop()
					+ getPaddingBottom());
		}
	}

	private void computeProximityThreshold(MTKKeyboard keyboard) {
		if (keyboard == null)
			return;
		final Key[] keys = mKeys;
		if (keys == null)
			return;
		int length = keys.length;
		int dimensionSum = 0;
		for (int i = 0; i < length; i++) {
			Key key = keys[i];
			dimensionSum += Math.min(key.width, key.height) + key.gap;
		}
		if (dimensionSum < 0 || length == 0)
			return;
		mProximityThreshold = (int) (dimensionSum * 1.4f / length);
		mProximityThreshold *= mProximityThreshold;
	}

	public void onSizeChanged(int w, int h, int oldw, int oldh) {
		super.onSizeChanged(w, h, oldw, oldh);
		mBuffer = null;
	}

	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		if (mDrawPending || mBuffer == null || mKeyboardChanged) {
			onBufferDraw();
		}
		canvas.drawBitmap(mBuffer, 0, 0, null);
	}

	private void onBufferDraw() {
		if (mBuffer == null || mKeyboardChanged) {
			if (mBuffer == null
					|| mKeyboardChanged
					&& (mBuffer.getWidth() != getWidth() || mBuffer.getHeight() != getHeight())) {
				mBuffer = Bitmap.createBitmap(getWidth(), getHeight(),
						Bitmap.Config.ARGB_8888);
				mCanvas = new Canvas(mBuffer);
			}
			invalidateAllKeys();
			mKeyboardChanged = false;
		}
		final Canvas canvas = mCanvas;
		canvas.clipRect(mDirtyRect, Op.REPLACE);

		if (mKeyboard == null)
			return;

		final Paint paint = mPaint;
		final Rect clipRegion = mClipRegion;
		final Rect padding = mPadding;
		final int kbdPaddingLeft = getPaddingLeft();
		final int kbdPaddingTop = getPaddingTop();
		final Key[] keys = mKeys;
		final Key invalidKey = mInvalidatedKey;

		paint.setColor(mKeyTextColor);
		boolean drawSingleKey = false;
		if (invalidKey != null && canvas.getClipBounds(clipRegion)) {
			if (invalidKey.x + kbdPaddingLeft - 1 <= clipRegion.left
					&& invalidKey.y + kbdPaddingTop - 1 <= clipRegion.top
					&& invalidKey.x + invalidKey.width + kbdPaddingLeft + 1 >= clipRegion.right
					&& invalidKey.y + invalidKey.height + kbdPaddingTop + 1 >= clipRegion.bottom) {
				drawSingleKey = true;
			}
		}
		canvas.drawColor(0x00000000, PorterDuff.Mode.CLEAR);
		final int keyCount = keys.length;
		for (int i = 0; i < keyCount; i++) {
			final Key key = keys[i];
			if (drawSingleKey && !invalidKey.equals(key)) {
				continue;
			}
			int[] drawableState = key.getCurrentDrawableState();
			if (drawableState.length > 0
					&& android.R.attr.state_pressed == drawableState[0]) {
				paint.setColor(Color.RED);
			} else {
				paint.setColor(Color.WHITE);
			}
			String label = key.label == null ? null : adjustCase(key.label)
					.toString();
			canvas.translate(key.x + kbdPaddingLeft, key.y + kbdPaddingTop);
			if (label != null) {
				if (label.length() > 1 && key.codes.length < 2&&mLabelTextSize>0) {
					paint.setTextSize(mLabelTextSize);
					paint.setTypeface(Typeface.DEFAULT_BOLD);
				} else {
					paint.setTextSize(mKeyTextSize);
					paint.setTypeface(Typeface.DEFAULT);
				}
				paint.setShadowLayer(mShadowRadius, 0, 0, mShadowColor);
				canvas.drawText(label,
						(key.width - padding.left - padding.right) / 2
								+ padding.left,
						(key.height - padding.top - padding.bottom) / 2
								+ (paint.getTextSize() - paint.descent()) / 2
								+ padding.top, paint);
				paint.setShadowLayer(0, 0, 0, 0);
			} else if (key.icon != null) {
				final int drawableX = (key.width - padding.left - padding.right - key.icon
						.getIntrinsicWidth()) / 2 + padding.left;
				final int drawableY = (key.height - padding.top
						- padding.bottom - key.icon.getIntrinsicHeight())
						/ 2 + padding.top;
				canvas.translate(drawableX, drawableY);
				key.icon.setBounds(0, 0, key.icon.getIntrinsicWidth(),
						key.icon.getIntrinsicHeight());
				key.icon.draw(canvas);
				canvas.translate(-drawableX, -drawableY);
			}
			canvas.translate(-key.x - kbdPaddingLeft, -key.y - kbdPaddingTop);
		}
		mInvalidatedKey = null;
		if (mMiniKeyboardOnScreen) {
			paint.setColor((int) (mBackgroundDimAmount * 0xFF) << 24);
			canvas.drawRect(0, 0, getWidth(), getHeight(), paint);
		}

		mDrawPending = false;
		mDirtyRect.setEmpty();
	}

	private int getKeyIndices(int x, int y, int[] allKeys) {
		final Key[] keys = mKeys;
		int primaryIndex = NOT_A_KEY;
		int closestKey = NOT_A_KEY;
		int closestKeyDist = mProximityThreshold + 1;
		java.util.Arrays.fill(mDistances, Integer.MAX_VALUE);
		int[] nearestKeyIndices = mKeyboard.getNearestKeys(x, y);
		final int keyCount = nearestKeyIndices.length;
		for (int i = 0; i < keyCount; i++) {
			final Key key = keys[nearestKeyIndices[i]];
			int dist = 0;
			boolean isInside = key.isInside(x, y);
			if (((mProximityCorrectOn && (dist = key.squaredDistanceFrom(x, y)) < mProximityThreshold) || isInside)
					&& key.codes[0] > 32) {
				final int nCodes = key.codes.length;
				if (dist < closestKeyDist) {
					closestKeyDist = dist;
					closestKey = nearestKeyIndices[i];
				}

				if (allKeys == null)
					continue;

				for (int j = 0; j < mDistances.length; j++) {
					if (mDistances[j] > dist) {
						System.arraycopy(mDistances, j, mDistances, j + nCodes,
								mDistances.length - j - nCodes);
						System.arraycopy(allKeys, j, allKeys, j + nCodes,
								allKeys.length - j - nCodes);
						for (int c = 0; c < nCodes; c++) {
							allKeys[j + c] = key.codes[c];
							mDistances[j + c] = dist;
						}
						break;
					}
				}
			}

			if (isInside) {
				primaryIndex = nearestKeyIndices[i];
			}
		}
		if (primaryIndex == NOT_A_KEY) {
			primaryIndex = closestKey;
		}
		return primaryIndex;
	}

	private void detectAndSendKey(int index, int x, int y, long eventTime) {
		if (index != NOT_A_KEY && index < mKeys.length) {
			final Key key = mKeys[index];
			if (key.text != null) {
				mKeyboardActionListener.onText(key.text);
				mKeyboardActionListener.onRelease(NOT_A_KEY);
			} else {
				int code = key.codes[0];
				int[] codes = new int[MAX_NEARBY_KEYS];
				Arrays.fill(codes, NOT_A_KEY);
				getKeyIndices(x, y, codes);

				if (mInMultiTap) {
					if (mTapCount != -1) {
						mKeyboardActionListener.onKey(
								MTKKeyboard.KEYCODE_DELETE, KEY_DELETE);
					} else {
						mTapCount = 0;
					}
					code = key.codes[mTapCount];
				}
				mKeyboardActionListener.onKey(code, codes);
				mKeyboardActionListener.onRelease(code);
			}
			mLastSentIndex = index;
			mLastTapTime = eventTime;
		}
	}

	private void showPreview(int keyIndex) {
		int oldKeyIndex = mCurrentKeyIndex;

		mCurrentKeyIndex = keyIndex;
		final Key[] keys = mKeys;
		if (oldKeyIndex != mCurrentKeyIndex) {
			if (oldKeyIndex != NOT_A_KEY && keys.length > oldKeyIndex) {
				keys[oldKeyIndex].onReleased(mCurrentKeyIndex == NOT_A_KEY);
				invalidateKey(oldKeyIndex);
			}
			if (mCurrentKeyIndex != NOT_A_KEY && keys.length > mCurrentKeyIndex) {
				keys[mCurrentKeyIndex].onPressed();
				invalidateKey(mCurrentKeyIndex);
			}
		}
		if (oldKeyIndex != mCurrentKeyIndex && mShowPreview) {
			mHandler.removeMessages(MSG_SHOW_PREVIEW);
		}
	}

	private void showKey(final int keyIndex) {
		if (keyIndex < 0 || keyIndex >= mKeys.length)
			return;
		mHandler.removeMessages(MSG_REMOVE_PREVIEW);
		if (mOffsetInWindow == null) {
			mOffsetInWindow = new int[2];
			getLocationInWindow(mOffsetInWindow);
			mOffsetInWindow[0] += mMiniKeyboardOffsetX;
			mOffsetInWindow[1] += mMiniKeyboardOffsetY;
		}

	}

	public void invalidateAllKeys() {
		mDirtyRect.union(0, 0, getWidth(), getHeight());
		mDrawPending = true;
		invalidate();
	}

	public void invalidateKey(int keyIndex) {
		if (mKeys == null)
			return;
		if (keyIndex < 0 || keyIndex >= mKeys.length) {
			return;
		}
		final Key key = mKeys[keyIndex];
		mInvalidatedKey = key;
		mDirtyRect.union(key.x + getPaddingLeft(), key.y + getPaddingTop(),
				key.x + key.width + getPaddingLeft(), key.y + key.height
						+ getPaddingTop());
		onBufferDraw();
		invalidate(key.x + getPaddingLeft(), key.y + getPaddingTop(), key.x
				+ key.width + getPaddingLeft(), key.y + key.height
				+ getPaddingTop());
	}

	private boolean openPopupIfRequired(MotionEvent me) {
		if (mPopupLayout == 0) {
			return false;
		}
		if (mCurrentKey < 0 || mCurrentKey >= mKeys.length) {
			return false;
		}

		Key popupKey = mKeys[mCurrentKey];
		boolean result = onLongPress(popupKey);
		if (result) {
			mAbortKey = true;
			showPreview(NOT_A_KEY);
		}
		return result;
	}

	protected boolean onLongPress(Key popupKey) {
		if (mWindowOffset == null) {
			mWindowOffset = new int[2];
			getLocationInWindow(mWindowOffset);
		}
		mPopupX = popupKey.x + getPaddingLeft();
		mPopupY = popupKey.y + getPaddingTop();
		mPopupX = mPopupX + popupKey.width
				- mMiniKeyboardContainer.getMeasuredWidth();
		mPopupY = mPopupY - mMiniKeyboardContainer.getMeasuredHeight();
		final int x = mPopupX + mMiniKeyboardContainer.getPaddingRight()
				+ mWindowOffset[0];
		final int y = mPopupY + mMiniKeyboardContainer.getPaddingBottom()
				+ mWindowOffset[1];
		mMiniKeyboard.setPopupOffset(x < 0 ? 0 : x, y);
		mMiniKeyboard.setShifted(isShifted());
		mPopupKeyboard.setContentView(mMiniKeyboardContainer);
		mPopupKeyboard.setWidth(mMiniKeyboardContainer.getMeasuredWidth());
		mPopupKeyboard.setHeight(mMiniKeyboardContainer.getMeasuredHeight());
		mPopupKeyboard.showAtLocation(this, Gravity.NO_GRAVITY, x, y);
		mMiniKeyboardOnScreen = true;
		// mMiniKeyboard.onTouchEvent(getTranslatedEvent(me));
		invalidateAllKeys();
		return true;
	}

	protected void onFocusChanged(boolean gainFocus, int direction,
			Rect previouslyFocusedRect) {
		super.onFocusChanged(gainFocus, direction, previouslyFocusedRect);
		if (gainFocus) {
			showPreview(mCurrentKey);
		} else {
			showPreview(NOT_A_KEY);
		}
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		boolean consumed = true;
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_LEFT:
			if (mCurrentColumn == 0) {
				mCurrentColumn = mKeyboard.getColumnCount()[mCurrentRow] - 1;
				mCurrentKey = mCurrentKey + mCurrentColumn;

			} else {
				mCurrentKey--;
				mCurrentColumn--;
			}
			break;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			if (mCurrentColumn == mKeyboard.getColumnCount()[mCurrentRow] - 1) {
				mCurrentColumn = 0;
				mCurrentKey = mCurrentKey
						- mKeyboard.getColumnCount()[mCurrentRow] + 1;
			} else {
				mCurrentKey++;
				mCurrentColumn++;
			}
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			if (mCurrentRow == 0) {
				consumed = false;
			} else {
				mCurrentRow--;
				if (mCurrentColumn >= mKeyboard.getColumnCount()[mCurrentRow]) {
					mCurrentKey -= mCurrentColumn + 1;
					mCurrentColumn = mKeyboard.getColumnCount()[mCurrentRow] - 1;
				} else {
					mCurrentKey -= mKeyboard.getColumnCount()[mCurrentRow];
				}
			}
			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			if (mCurrentRow == mKeyboard.getRowCount() - 1) {
				consumed = false;
			} else {
				mCurrentRow++;
				if (mCurrentColumn >= mKeyboard.getColumnCount()[mCurrentRow]) {
					mCurrentKey += mKeyboard.getColumnCount()[mCurrentRow - 1]
							- (mCurrentColumn
									- mKeyboard.getColumnCount()[mCurrentRow] + 1);
					mCurrentColumn = mKeyboard.getColumnCount()[mCurrentRow] - 1;
				} else {
					mCurrentKey += mKeyboard.getColumnCount()[mCurrentRow - 1];
				}
			}
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
			final Key key = mKeys[mCurrentKey];
			mKeyboardActionListener.onKey(key.codes[0], key.codes);
			break;
		default:
			consumed = false;
			break;
		}

		if (consumed) {
			showPreview(mCurrentKey);
			return true;
		} else {
			return super.onKeyDown(keyCode, event);
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent me) {
		final int pointerCount = me.getPointerCount();
		final int action = me.getAction();
		boolean result = false;
		final long now = me.getEventTime();
		if (pointerCount != mOldPointerCount) {
			if (pointerCount == 1) {
				MotionEvent down = MotionEvent.obtain(now, now,
						MotionEvent.ACTION_DOWN, me.getX(), me.getY(),
						me.getMetaState());
				result = onModifiedTouchEvent(down, false);
				down.recycle();
				if (action == MotionEvent.ACTION_UP) {
					result = onModifiedTouchEvent(me, true);
				}
			} else {
				MotionEvent up = MotionEvent.obtain(now, now,
						MotionEvent.ACTION_UP, mOldPointerX, mOldPointerY,
						me.getMetaState());
				result = onModifiedTouchEvent(up, true);
				up.recycle();
			}
		} else {
			if (pointerCount == 1) {
				result = onModifiedTouchEvent(me, false);
				mOldPointerX = me.getX();
				mOldPointerY = me.getY();
			} else {
				result = true;
			}
		}
		mOldPointerCount = pointerCount;

		return result;
	}

	private boolean onModifiedTouchEvent(MotionEvent me, boolean possiblePoly) {
		int touchX = (int) me.getX() - getPaddingLeft();
		int touchY = (int) me.getY() + mVerticalCorrection - getPaddingTop();
		final int action = me.getAction();
		final long eventTime = me.getEventTime();
		int keyIndex = getKeyIndices(touchX, touchY, null);
		mPossiblePoly = possiblePoly;

		if (action == MotionEvent.ACTION_DOWN)
			mSwipeTracker.clear();
		mSwipeTracker.addMovement(me);

		if (mGestureDetector.onTouchEvent(me)) {
			showPreview(NOT_A_KEY);
			mHandler.removeMessages(MSG_REPEAT);
			mHandler.removeMessages(MSG_LONGPRESS);
			return true;
		}

		if (mMiniKeyboardOnScreen) {
			return true;
		}

		switch (action) {
		case MotionEvent.ACTION_DOWN:
			mAbortKey = false;
			mStartX = touchX;
			mStartY = touchY;
			mLastCodeX = touchX;
			mLastCodeY = touchY;
			mLastKeyTime = 0;
			mCurrentKeyTime = 0;
			mLastKey = NOT_A_KEY;
			mCurrentKey = keyIndex;
			mCurrentRow = computeCurrentRow(keyIndex);
			mCurrentColumn = computeCurrentColumn(keyIndex);
			mDownKey = keyIndex;
			mDownTime = me.getEventTime();
			mLastMoveTime = mDownTime;
			checkMultiTap(eventTime, keyIndex);
			mKeyboardActionListener
					.onPress(keyIndex != NOT_A_KEY ? mKeys[keyIndex].codes[0]
							: 0);
			if (mCurrentKey >= 0 && mKeys[mCurrentKey].repeatable) {
				mRepeatKeyIndex = mCurrentKey;
				repeatKey();
				Message msg = mHandler.obtainMessage(MSG_REPEAT);
				mHandler.sendMessageDelayed(msg, REPEAT_START_DELAY);
			}
			if (mCurrentKey != NOT_A_KEY) {
				Message msg = mHandler.obtainMessage(MSG_LONGPRESS, me);
				mHandler.sendMessageDelayed(msg, LONGPRESS_TIMEOUT);
			}
			showPreview(keyIndex);
			break;

		case MotionEvent.ACTION_MOVE:
			boolean continueLongPress = false;
			if (keyIndex != NOT_A_KEY) {
				if (mCurrentKey == NOT_A_KEY) {
					mCurrentKey = keyIndex;
					mCurrentKeyTime = eventTime - mDownTime;
				} else {
					if (keyIndex == mCurrentKey) {
						mCurrentKeyTime += eventTime - mLastMoveTime;
						continueLongPress = true;
					} else if (mRepeatKeyIndex == NOT_A_KEY) {
						resetMultiTap();
						mLastKey = mCurrentKey;
						mLastCodeX = mLastX;
						mLastCodeY = mLastY;
						mLastKeyTime = mCurrentKeyTime + eventTime
								- mLastMoveTime;
						mCurrentKey = keyIndex;
						mCurrentKeyTime = 0;
					}
				}
			}
			if (!continueLongPress) {
				mHandler.removeMessages(MSG_LONGPRESS);
				if (keyIndex != NOT_A_KEY) {
					Message msg = mHandler.obtainMessage(MSG_LONGPRESS, me);
					mHandler.sendMessageDelayed(msg, LONGPRESS_TIMEOUT);
				}
			}
			showPreview(mCurrentKey);
			break;

		case MotionEvent.ACTION_UP:
			removeMessages();
			if (keyIndex == mCurrentKey) {
				mCurrentKeyTime += eventTime - mLastMoveTime;
			} else {
				resetMultiTap();
				mLastKey = mCurrentKey;
				mLastKeyTime = mCurrentKeyTime + eventTime - mLastMoveTime;
				mCurrentKey = keyIndex;
				mCurrentKeyTime = 0;
			}
			if (mCurrentKeyTime < mLastKeyTime && mLastKey != NOT_A_KEY) {
				mCurrentKey = mLastKey;
				touchX = mLastCodeX;
				touchY = mLastCodeY;
			}
			showPreview(NOT_A_KEY);
			Arrays.fill(mKeyIndices, NOT_A_KEY);
			if (mRepeatKeyIndex == NOT_A_KEY && !mMiniKeyboardOnScreen
					&& !mAbortKey) {
				detectAndSendKey(mCurrentKey, touchX, touchY, eventTime);
			}
			invalidateKey(keyIndex);
			mRepeatKeyIndex = NOT_A_KEY;
			break;
		case MotionEvent.ACTION_CANCEL:
			removeMessages();
			mAbortKey = true;
			showPreview(NOT_A_KEY);
			invalidateKey(mCurrentKey);
			break;
		}
		mLastX = touchX;
		mLastY = touchY;
		return true;
	}

	private boolean repeatKey() {
		Key key = mKeys[mRepeatKeyIndex];
		detectAndSendKey(mCurrentKey, key.x, key.y, mLastTapTime);
		return true;
	}

	private void removeMessages() {
		mHandler.removeMessages(MSG_REPEAT);
		mHandler.removeMessages(MSG_LONGPRESS);
		mHandler.removeMessages(MSG_SHOW_PREVIEW);
	}

	private void dismissPopupKeyboard() {
		if (mPopupKeyboard.isShowing()) {
			mPopupKeyboard.dismiss();
			mMiniKeyboardOnScreen = false;
			invalidateAllKeys();
		}
	}

	public boolean handleBack() {
		if (mPopupKeyboard.isShowing()) {
			dismissPopupKeyboard();
			return true;
		}
		return false;
	}

	private void resetMultiTap() {
		mLastSentIndex = NOT_A_KEY;
		mTapCount = 0;
		mLastTapTime = -1;
		mInMultiTap = false;
	}

	private void checkMultiTap(long eventTime, int keyIndex) {
		if (keyIndex == NOT_A_KEY)
			return;
		Key key = mKeys[keyIndex];
		if (key.codes.length > 1) {
			mInMultiTap = true;
			if (eventTime < mLastTapTime + MULTITAP_INTERVAL
					&& keyIndex == mLastSentIndex) {
				mTapCount = (mTapCount + 1) % key.codes.length;
				return;
			} else {
				mTapCount = -1;
				return;
			}
		}
		if (eventTime > mLastTapTime + MULTITAP_INTERVAL
				|| keyIndex != mLastSentIndex) {
			resetMultiTap();
		}
	}

	// a stupid and dull method to get current row index
	private int computeCurrentRow(int curIndex) {
		if (curIndex <= 40 && curIndex >= 0) {
			return curIndex / 9;
		} else {
			return -1;
		}
	}

	// a stupid and dull method to get current column index
	private int computeCurrentColumn(int curIndex) {
		if (curIndex <= 40 && curIndex >= 0) {
			return curIndex % 9;
		} else {
			return -1;
		}
	}

	private static class SwipeTracker {

		static final int NUM_PAST = 4;
		static final int LONGEST_PAST_TIME = 200;

		final float mPastX[] = new float[NUM_PAST];
		final float mPastY[] = new float[NUM_PAST];
		final long mPastTime[] = new long[NUM_PAST];

		float mYVelocity;
		float mXVelocity;

		public void clear() {
			mPastTime[0] = 0;
		}

		public void addMovement(MotionEvent ev) {
			long time = ev.getEventTime();
			final int N = ev.getHistorySize();
			for (int i = 0; i < N; i++) {
				addPoint(ev.getHistoricalX(i), ev.getHistoricalY(i),
						ev.getHistoricalEventTime(i));
			}
			addPoint(ev.getX(), ev.getY(), time);
		}

		private void addPoint(float x, float y, long time) {
			int drop = -1;
			int i;
			final long[] pastTime = mPastTime;
			for (i = 0; i < NUM_PAST; i++) {
				if (pastTime[i] == 0) {
					break;
				} else if (pastTime[i] < time - LONGEST_PAST_TIME) {
					drop = i;
				}
			}
			if (i == NUM_PAST && drop < 0) {
				drop = 0;
			}
			if (drop == i)
				drop--;
			final float[] pastX = mPastX;
			final float[] pastY = mPastY;
			if (drop >= 0) {
				final int start = drop + 1;
				final int count = NUM_PAST - drop - 1;
				System.arraycopy(pastX, start, pastX, 0, count);
				System.arraycopy(pastY, start, pastY, 0, count);
				System.arraycopy(pastTime, start, pastTime, 0, count);
				i -= (drop + 1);
			}
			pastX[i] = x;
			pastY[i] = y;
			pastTime[i] = time;
			i++;
			if (i < NUM_PAST) {
				pastTime[i] = 0;
			}
		}

		public void computeCurrentVelocity(int units) {
			computeCurrentVelocity(units, Float.MAX_VALUE);
		}

		public void computeCurrentVelocity(int units, float maxVelocity) {
			final float[] pastX = mPastX;
			final float[] pastY = mPastY;
			final long[] pastTime = mPastTime;

			final float oldestX = pastX[0];
			final float oldestY = pastY[0];
			final long oldestTime = pastTime[0];
			float accumX = 0;
			float accumY = 0;
			int N = 0;
			while (N < NUM_PAST) {
				if (pastTime[N] == 0) {
					break;
				}
				N++;
			}

			for (int i = 1; i < N; i++) {
				final int dur = (int) (pastTime[i] - oldestTime);
				if (dur == 0)
					continue;
				float dist = pastX[i] - oldestX;
				float vel = (dist / dur) * units;
				if ((int)accumX == 0)
					accumX = vel;
				else
					accumX = (accumX + vel) * .5f;

				dist = pastY[i] - oldestY;
				vel = (dist / dur) * units;
				if ((int)accumY == 0)
					accumY = vel;
				else
					accumY = (accumY + vel) * .5f;
			}
			mXVelocity = accumX < 0.0f ? Math.max(accumX, -maxVelocity) : Math
					.min(accumX, maxVelocity);
			mYVelocity = accumY < 0.0f ? Math.max(accumY, -maxVelocity) : Math
					.min(accumY, maxVelocity);
		}

		public float getXVelocity() {
			return mXVelocity;
		}

		public float getYVelocity() {
			return mYVelocity;
		}
	}
}
