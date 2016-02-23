package com.mediatek.ui.menu.commonview;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;
import org.xmlpull.v1.XmlPullParserException;
import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.content.res.XmlResourceParser;
import android.graphics.drawable.Drawable;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.util.Xml;
/**
 * utilClass for MTKKeyboardView,provide keywords
 * @author hs_haosun
 *
 */
public class MTKKeyboard {
	static final String TAG = "Keyboard";

	private static final String TAG_KEYBOARD = "Keyboard";
	private static final String TAG_ROW = "Row";
	private static final String TAG_KEY = "Key";

	public static final int EDGE_LEFT = 0x01;
	public static final int EDGE_RIGHT = 0x02;
	public static final int EDGE_TOP = 0x04;
	public static final int EDGE_BOTTOM = 0x08;

	public static final int KEYCODE_SHIFT = -1;
	public static final int KEYCODE_DELETE = -5;

	private int mDefaultWidth;

	private int mDefaultHeight;

	private int mDefaultHorizontalGap;

	private int mDefaultVerticalGap;

	private boolean mShifted;

	private Key mShiftKey;

	private int mShiftKeyIndex = -1;

	private int mTotalHeight;

	private int mTotalWidth;

	private List<Key> mKeys;

	private List<Key> mModifierKeys;

	private int mDisplayWidth;

	private int mDisplayHeight;

	private int mKeyboardMode;

	private static final int GRID_WIDTH = 10;
	private static final int GRID_HEIGHT = 5;
	private static final int GRID_SIZE = GRID_WIDTH * GRID_HEIGHT;
	private int mCellWidth;
	private int mCellHeight;
	private int[][] mGridNeighbors;
	private int mProximityThreshold;

	private int mRowCount;
	private int[] mColumnCount;

	private static float SEARCH_DISTANCE = 1.8f;

	public static class Row {
		public int defaultWidth;

		public int defaultHeight;

		public int defaultHorizontalGap;

		public int verticalGap;

		public int rowEdgeFlags;

		public int mode;

		private MTKKeyboard parent;

		public Row(MTKKeyboard parent) {
			this.parent = parent;
		}

		public Row(Resources res, MTKKeyboard parent, XmlResourceParser parser) {
			this.parent = parent;
			TypedArray a = res.obtainAttributes(Xml.asAttributeSet(parser),
					R.styleable.Keyboard);
			defaultWidth = getDimensionOrFraction(a,
					R.styleable.Keyboard_keyWidth, parent.mDisplayWidth,
					parent.mDefaultWidth);
			defaultHeight = getDimensionOrFraction(a,
					R.styleable.Keyboard_keyHeight, parent.mDisplayHeight,
					parent.mDefaultHeight);
			defaultHorizontalGap = getDimensionOrFraction(a,
					R.styleable.Keyboard_horizontalGap, parent.mDisplayWidth,
					parent.mDefaultHorizontalGap);
			verticalGap = getDimensionOrFraction(a,
					R.styleable.Keyboard_verticalGap, parent.mDisplayHeight,
					parent.mDefaultVerticalGap);
			a.recycle();
			a = res.obtainAttributes(Xml.asAttributeSet(parser),
					R.styleable.Keyboard_Row);
			rowEdgeFlags = a.getInt(R.styleable.Keyboard_Row_rowEdgeFlags, 0);
			mode = a.getResourceId(R.styleable.Keyboard_Row_keyboardMode, 0);
		}
	}

	public static class Key {
		public int[] codes;

		public CharSequence label;

		public Drawable icon;

		public int width;

		public int height;

		public int gap;

		public boolean sticky;

		public int x;

		public int y;

		public boolean pressed;

		public boolean on;

		public CharSequence text;

		public int edgeFlags;

		public boolean modifier;

		private MTKKeyboard keyboard;

		public boolean repeatable;

		private final static int[] KEY_STATE_NORMAL_ON = {
				android.R.attr.state_checkable, android.R.attr.state_checked };

		private final static int[] KEY_STATE_PRESSED_ON = {
				android.R.attr.state_pressed, android.R.attr.state_checkable,
				android.R.attr.state_checked };

		private final static int[] KEY_STATE_NORMAL_OFF = { android.R.attr.state_checkable };

		private final static int[] KEY_STATE_PRESSED_OFF = {
				android.R.attr.state_pressed, android.R.attr.state_checkable };

		private final static int[] KEY_STATE_NORMAL = {};

		private final static int[] KEY_STATE_PRESSED = { android.R.attr.state_pressed };

		public Key(Row parent) {
			keyboard = parent.parent;
		}

		public Key(Resources res, Row parent, int x, int y,
				XmlResourceParser parser) {
			this(parent);

			this.x = x;
			this.y = y;

			TypedArray a = res.obtainAttributes(Xml.asAttributeSet(parser),
					R.styleable.Keyboard);

			width = getDimensionOrFraction(a, R.styleable.Keyboard_keyWidth,
					keyboard.mDisplayWidth, parent.defaultWidth);
			height = getDimensionOrFraction(a, R.styleable.Keyboard_keyHeight,
					keyboard.mDisplayHeight, parent.defaultHeight);
			gap = getDimensionOrFraction(a, R.styleable.Keyboard_horizontalGap,
					keyboard.mDisplayWidth, parent.defaultHorizontalGap);
			a.recycle();
			a = res.obtainAttributes(Xml.asAttributeSet(parser),
					R.styleable.Keyboard_Key);
			this.x += gap;
			TypedValue codesValue = new TypedValue();
			a.getValue(R.styleable.Keyboard_Key_codes, codesValue);
			if (codesValue.type == TypedValue.TYPE_INT_DEC
					|| codesValue.type == TypedValue.TYPE_INT_HEX) {
				codes = new int[] { codesValue.data };
			} else if (codesValue.type == TypedValue.TYPE_STRING) {
				codes = parseCSV(codesValue.string.toString());
			}

			repeatable = a.getBoolean(R.styleable.Keyboard_Key_isRepeatable,
					false);
			modifier = a.getBoolean(R.styleable.Keyboard_Key_isModifier, false);
			sticky = a.getBoolean(R.styleable.Keyboard_Key_isSticky, false);
			edgeFlags = a.getInt(R.styleable.Keyboard_Key_keyEdgeFlags, 0);
			edgeFlags |= parent.rowEdgeFlags;

			icon = a.getDrawable(R.styleable.Keyboard_Key_keyIcon);
			if (icon != null) {
				icon.setBounds(0, 0, icon.getIntrinsicWidth(), icon
						.getIntrinsicHeight());
			}
			label = a.getText(R.styleable.Keyboard_Key_keyLabel);
			text = a.getText(R.styleable.Keyboard_Key_keyOutputText);

			if (codes == null && !TextUtils.isEmpty(label)) {
				codes = new int[] { label.charAt(0) };
			}
			a.recycle();
		}

		public void onPressed() {
			pressed = !pressed;
		}

		public void onReleased(boolean inside) {
			pressed = !pressed;
			if (sticky) {
				on = !on;
			}
		}

		int[] parseCSV(String value) {
			int count = 0;
			int lastIndex = 0;
			if (value.length() > 0) {
				count++;
				while ((lastIndex = value.indexOf(",", lastIndex + 1)) > 0) {
					count++;
				}
			}
			int[] values = new int[count];
			count = 0;
			StringTokenizer st = new StringTokenizer(value, ",");
			while (st.hasMoreTokens()) {
				try {
					values[count++] = Integer.parseInt(st.nextToken());
				} catch (NumberFormatException nfe) {
					MtkLog.e(TAG, "Error parsing keycodes " + value);
				}
			}
			return values;
		}

		public boolean isInside(int x, int y) {
			boolean leftEdge = (edgeFlags & EDGE_LEFT) > 0;
			boolean rightEdge = (edgeFlags & EDGE_RIGHT) > 0;
			boolean topEdge = (edgeFlags & EDGE_TOP) > 0;
			boolean bottomEdge = (edgeFlags & EDGE_BOTTOM) > 0;
			if ((x >= this.x || (leftEdge && x <= this.x + this.width))
					&& (x < this.x + this.width || (rightEdge && x >= this.x))
					&& (y >= this.y || (topEdge && y <= this.y + this.height))
					&& (y < this.y + this.height || (bottomEdge && y >= this.y))) {
				return true;
			} else {
				return false;
			}
		}

		public int squaredDistanceFrom(int x, int y) {
			int xDist = this.x + width / 2 - x;
			int yDist = this.y + height / 2 - y;
			return xDist * xDist + yDist * yDist;
		}

		public int[] getCurrentDrawableState() {
			int[] states = KEY_STATE_NORMAL;

			if (on) {
				if (pressed) {
					states = KEY_STATE_PRESSED_ON;
				} else {
					states = KEY_STATE_NORMAL_ON;
				}
			} else {
				if (sticky) {
					if (pressed) {
						states = KEY_STATE_PRESSED_OFF;
					} else {
						states = KEY_STATE_NORMAL_OFF;
					}
				} else {
					if (pressed) {
						states = KEY_STATE_PRESSED;
					}
				}
			}
			return states;
		}
	}

	public MTKKeyboard(Context context, int xmlLayoutResId) {
		this(context, xmlLayoutResId, 0);
	}

	public MTKKeyboard(Context context, int xmlLayoutResId, int parentW,
			int parentH) {
		mDisplayWidth = parentW;
		mDisplayHeight = parentH;
		mDefaultWidth = mDisplayWidth / 10;
		mDefaultHeight = mDefaultWidth;
		mDefaultHorizontalGap = 0;
		mDefaultVerticalGap = 0;
		mKeys = new ArrayList<Key>();
		mModifierKeys = new ArrayList<Key>();
		mKeyboardMode = 0;
		loadKeyboard(context, context.getResources().getXml(xmlLayoutResId));
	}

	public MTKKeyboard(Context context, int xmlLayoutResId, int modeId) {
		DisplayMetrics dm = context.getResources().getDisplayMetrics();
		mDisplayWidth = dm.widthPixels;
		mDisplayHeight = dm.heightPixels;
		MtkLog.v(TAG, "keyboard's display metrics:" + dm);

		mDefaultHorizontalGap = 0;
		mDefaultWidth = mDisplayWidth / 10;
		mDefaultVerticalGap = 0;
		mDefaultHeight = mDefaultWidth;
		mKeys = new ArrayList<Key>();
		mModifierKeys = new ArrayList<Key>();
		mKeyboardMode = modeId;
		loadKeyboard(context, context.getResources().getXml(xmlLayoutResId));
	}

	public MTKKeyboard(Context context, int layoutTemplateResId,
			CharSequence characters, int columns, int horizontalPadding) {
		this(context, layoutTemplateResId);
		int x = 0;
		int y = 0;
		int column = 0;
		mTotalWidth = 0;

		Row row = new Row(this);
		row.defaultHeight = mDefaultHeight;
		row.defaultWidth = mDefaultWidth;
		row.defaultHorizontalGap = mDefaultHorizontalGap;
		row.verticalGap = mDefaultVerticalGap;
		row.rowEdgeFlags = EDGE_TOP | EDGE_BOTTOM;
		final int maxColumns = columns == -1 ? Integer.MAX_VALUE : columns;
		for (int i = 0; i < characters.length(); i++) {
			char c = characters.charAt(i);
			if (column >= maxColumns
					|| x + mDefaultWidth + horizontalPadding > mDisplayWidth) {
				x = 0;
				y += mDefaultVerticalGap + mDefaultHeight;
				column = 0;
			}
			final Key key = new Key(row);
			key.x = x;
			key.y = y;
			key.width = mDefaultWidth;
			key.height = mDefaultHeight;
			key.gap = mDefaultHorizontalGap;
			key.label = String.valueOf(c);
			key.codes = new int[] { c };
			column++;
			x += key.width + key.gap;
			mKeys.add(key);
			if (x > mTotalWidth) {
				mTotalWidth = x;
			}
		}
		mTotalHeight = y + mDefaultHeight;
	}

	public List<Key> getKeys() {
		return mKeys;
	}

	public List<Key> getModifierKeys() {
		return mModifierKeys;
	}

	protected int getHorizontalGap() {
		return mDefaultHorizontalGap;
	}

	protected void setHorizontalGap(int gap) {
		mDefaultHorizontalGap = gap;
	}

	protected int getVerticalGap() {
		return mDefaultVerticalGap;
	}

	protected void setVerticalGap(int gap) {
		mDefaultVerticalGap = gap;
	}

	protected int getKeyHeight() {
		return mDefaultHeight;
	}

	protected void setKeyHeight(int height) {
		mDefaultHeight = height;
	}

	protected int getKeyWidth() {
		return mDefaultWidth;
	}

	protected void setKeyWidth(int width) {
		mDefaultWidth = width;
	}

	public int getHeight() {
		return mTotalHeight;
	}

	public int getMinWidth() {
		return mTotalWidth;
	}

	public boolean setShifted(boolean shiftState) {
		if (mShiftKey != null) {
			mShiftKey.on = shiftState;
		}
		if (mShifted != shiftState) {
			mShifted = shiftState;
			return true;
		}
		return false;
	}

	public boolean isShifted() {
		return mShifted;
	}

	public int getShiftKeyIndex() {
		return mShiftKeyIndex;
	}

	private void computeNearestNeighbors() {
		mCellWidth = (getMinWidth() + GRID_WIDTH - 1) / GRID_WIDTH;
		mCellHeight = (getHeight() + GRID_HEIGHT - 1) / GRID_HEIGHT;
		mGridNeighbors = new int[GRID_SIZE][];
		int[] indices = new int[mKeys.size()];
		final int gridWidth = GRID_WIDTH * mCellWidth;
		final int gridHeight = GRID_HEIGHT * mCellHeight;
		for (int x = 0; x < gridWidth; x += mCellWidth) {
			for (int y = 0; y < gridHeight; y += mCellHeight) {
				int count = 0;
				for (int i = 0; i < mKeys.size(); i++) {
					final Key key = mKeys.get(i);
					if (key.squaredDistanceFrom(x, y) < mProximityThreshold
							|| key.squaredDistanceFrom(x + mCellWidth - 1, y) < mProximityThreshold
							|| key.squaredDistanceFrom(x + mCellWidth - 1, y
									+ mCellHeight - 1) < mProximityThreshold
							|| key.squaredDistanceFrom(x, y + mCellHeight - 1) < mProximityThreshold) {
						indices[count++] = i;
					}
				}
				int[] cell = new int[count];
				System.arraycopy(indices, 0, cell, 0, count);
				mGridNeighbors[(y / mCellHeight) * GRID_WIDTH
						+ (x / mCellWidth)] = cell;
			}
		}
	}

	public int[] getNearestKeys(int x, int y) {
		if (mGridNeighbors == null)
			computeNearestNeighbors();
		if (x >= 0 && x < getMinWidth() && y >= 0 && y < getHeight()) {
			int index = (y / mCellHeight) * GRID_WIDTH + (x / mCellWidth);
			if (index < GRID_SIZE) {
				return mGridNeighbors[index];
			}
		}
		return new int[0];
	}

	protected Row createRowFromXml(Resources res, XmlResourceParser parser) {
		return new Row(res, this, parser);
	}

	protected Key createKeyFromXml(Resources res, Row parent, int x, int y,
			XmlResourceParser parser) {
		return new Key(res, parent, x, y, parser);
	}

	private void loadKeyboard(Context context, XmlResourceParser parser) {
		boolean inKey = false;
		boolean inRow = false;
		int x = 0;
		int y = 0;
		Key key = null;
		Row currentRow = null;
		Resources res = context.getResources();
		boolean skipRow = false;

		int[] columnCount = new int[10];

		try {
			int event;
			while ((event = parser.next()) != XmlResourceParser.END_DOCUMENT) {
				if (event == XmlResourceParser.START_TAG) {
					String tag = parser.getName();
					if (TAG_ROW.equals(tag)) {
						inRow = true;
						x = 0;
						currentRow = createRowFromXml(res, parser);
						skipRow = currentRow.mode != 0
								&& currentRow.mode != mKeyboardMode;
						if (skipRow) {
							skipToEndOfRow(parser);
							inRow = false;
						}
					} else if (TAG_KEY.equals(tag)) {
						inKey = true;
						if(currentRow != null){
							key = createKeyFromXml(res, currentRow, x, y, parser);
						}	
						if (mKeys.size() == 0) {
							if(key != null){
								key.onPressed();
							}
						}
						mKeys.add(key);
						if (null != key && key.codes != null && key.codes[0] == KEYCODE_SHIFT) {
							mShiftKey = key;
							mShiftKeyIndex = mKeys.size() - 1;
							mModifierKeys.add(key);
						}
					} else if (TAG_KEYBOARD.equals(tag)) {
						parseKeyboardAttributes(res, parser);
					}
				} else if (event == XmlResourceParser.END_TAG) {
					if (inKey) {
						inKey = false;
						if(key != null){
						x += key.gap + key.width;
						}
						if (x > mTotalWidth) {
							mTotalWidth = x;
						}
						columnCount[mRowCount]++;
					} else if (inRow) {
						inRow = false;
						y += currentRow.verticalGap;
						y += currentRow.defaultHeight;
						mRowCount++;
					} else {

					}
				}
			}
			mColumnCount = columnCount;
		} catch (Exception e) {
			MtkLog.e(TAG, "Parse error:" + e);
			e.printStackTrace();
		}
		mTotalHeight = y - mDefaultVerticalGap;
	}

	private void skipToEndOfRow(XmlResourceParser parser)
			throws XmlPullParserException, IOException {
		int event;
		while ((event = parser.next()) != XmlResourceParser.END_DOCUMENT) {
			if (event == XmlResourceParser.END_TAG
					&& parser.getName().equals(TAG_ROW)) {
				break;
			}
		}
	}

	private void parseKeyboardAttributes(Resources res, XmlResourceParser parser) {
		TypedArray a = res.obtainAttributes(Xml.asAttributeSet(parser),
				R.styleable.Keyboard);

		mDefaultWidth = getDimensionOrFraction(a,
				R.styleable.Keyboard_keyWidth, mDisplayWidth,
				mDisplayWidth / 10);
		mDefaultHeight = getDimensionOrFraction(a,
				R.styleable.Keyboard_keyHeight, mDisplayHeight, 50);
		mDefaultHorizontalGap = getDimensionOrFraction(a,
				R.styleable.Keyboard_horizontalGap, mDisplayWidth, 0);
		mDefaultVerticalGap = getDimensionOrFraction(a,
				R.styleable.Keyboard_verticalGap, mDisplayHeight, 0);
		mProximityThreshold = (int) (mDefaultWidth * SEARCH_DISTANCE);
		mProximityThreshold = mProximityThreshold * mProximityThreshold;
	}

	static int getDimensionOrFraction(TypedArray a, int index, int base,
			int defValue) {
		TypedValue value = a.peekValue(index);
		if (value == null)
			return defValue;
		if (value.type == TypedValue.TYPE_DIMENSION) {
			return a.getDimensionPixelOffset(index, defValue);
		} else if (value.type == TypedValue.TYPE_FRACTION) {
			return Math.round(a.getFraction(index, base, base, defValue));
		}
		return defValue;
	}

	public void setRowCount(int mRowCount) {
		this.mRowCount = mRowCount;
	}

	public int getRowCount() {
		return mRowCount;
	}

	public void setColumnCount(int[] mColumnCount) {
		this.mColumnCount = mColumnCount;
	}

	public int[] getColumnCount() {
		return mColumnCount;
	}
}
