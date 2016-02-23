package com.mediatek.ui.mmp.commonview;

import java.util.ArrayList;
import java.util.Random;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;

import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;

public class ScoreView extends View {

	/**
	 * Parameters controlling the size of the tiles and their range within view.
	 * Width/Height are in pixels, and Drawables will be scaled to fit to these
	 * dimensions. X/Y Tile Counts are the number of tiles that will be drawn.
	 */

	private static final String TAG = "ScoreView";
	protected static int mTileSize;

	protected  int mXTileCount;
	protected  int mYTileCount;

	private static int mXOffset;
	private static int mYOffset;

	private static final int RED_STAR = 1;
	private static final int YELLOW_STAR = 2;

	private long mMoveDelay = 600;

//	int[] rawArray;

	/**
	 */
	private ArrayList<Coordinate> mCoordinate = new ArrayList<Coordinate>();

	/**
	 * Everyone needs a little randomness in their life
	 */
//	private static final Random RNG = new Random();

	/**
	 */
	private Bitmap[] mTileArray;

	/**
	 * A two-dimensional array of integers in which the number represents the
	 * index of the tile that should be drawn at that locations
	 */
	private int[][] mTileGrid;

	private final Paint mPaint = new Paint();

//	/**
//	 * Create a simple handler that we can use to cause animation to happen. We
//	 * set ourselves as a target and we can use the sleep() function to cause an
//	 * update/invalidate to occur at a later date.
//	 */
//	private RefreshHandler mRedrawHandler = new RefreshHandler();
//
//	class RefreshHandler extends Handler {
//
//		@Override
//		public void handleMessage(Message msg) {
//			ScoreView.this.invalidate();
//		}
//
//		public void sleep(long delayMillis) {
//			this.removeMessages(0);
//			sendMessageDelayed(obtainMessage(0), delayMillis);
//		}
//	};

	public ScoreView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		TypedArray a = context.obtainStyledAttributes(attrs,
				R.styleable.TileView);

		mTileSize = a.getInt(R.styleable.TileView_tileSize, 12);

		a.recycle();
		initView();
	}

	public ScoreView(Context context, AttributeSet attrs) {
		super(context, attrs);

		TypedArray a = context.obtainStyledAttributes(attrs,
				R.styleable.TileView);

		mTileSize = a.getInt(R.styleable.TileView_tileSize, 12);
		MtkLog.d(TAG, "mTileSize" + mTileSize);
		a.recycle();
		initView();
	}

	/**
	 * Rests the internal array of Bitmaps used for drawing tiles, and sets the
	 * maximum index of tiles to be inserted
	 * 
	 * @param tilecount
	 */

	private void resetTiles(int tilecount) {
		mTileArray = new Bitmap[tilecount];
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		mXTileCount = 30;//(int) Math.floor(w / mTileSize);
		mYTileCount = 20;//(int) Math.floor(h / mTileSize);
		MtkLog.e(TAG, "mXTileCount=="+mXTileCount+"   mYTileCount="+mYTileCount);
		mXOffset = ((w - (mTileSize * mXTileCount)) / 2);
		mYOffset = ((h - (mTileSize * mYTileCount)) / 2);

		mTileGrid = new int[mXTileCount][mYTileCount];
		clearTiles();
		initCoord();
	}

	private void initView() {
		
		mXTileCount =0;
		mYTileCount =0;
		setFocusable(true);

		Resources r = this.getContext().getResources();

		resetTiles(4);
		loadTile(RED_STAR, r.getDrawable(R.drawable.mmp_redstar));
		loadTile(YELLOW_STAR, r.getDrawable(R.drawable.mmp_yellowstar));
	}

	private void initCoord() {
		mCoordinate.clear();
		for (int i = 0; i < mXTileCount; i++) {
			for (int j = 0; j < mYTileCount; j++) {
				mCoordinate.add(new Coordinate(i, j));
			}
		}
		mMoveDelay = 100;
	}

	public void update(int[] arr) {
		clearTiles();
		for (Coordinate c : mCoordinate) {
			if (c.x < arr.length) {
				if (arr[c.x] <= mYTileCount / 2) {
					if (mYTileCount - c.y - 1 < arr[c.x]) {
						setTile(YELLOW_STAR, c.x, c.y);
					}
				} else {
					int position = mYTileCount - c.y - 1;
					if (position < mYTileCount / 2) {
						setTile(YELLOW_STAR, c.x, c.y);
					}
					if (mYTileCount / 2 <= position && position < arr[c.x]) {
						setTile(RED_STAR, c.x, c.y);
					}
				}
			}
		}
	}

	/**
	 * Function to set the specified Drawable as the tile for a particular
	 * integer key.
	 * 
	 * @param key
	 * @param tile
	 */
	 void loadTile(int key, Drawable tile) {
		Bitmap bitmap = Bitmap.createBitmap(mTileSize, mTileSize,
				Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);
		tile.setBounds(0, 0, mTileSize, mTileSize);
		tile.draw(canvas);
		mTileArray[key] = bitmap;
	}

	/**
	 * Resets all tiles to 0 (empty)
	 * 
	 */
	public void clearTiles() {
		
		for (int x = 0; x < mXTileCount; x++) {
			for (int y = 0; y < mYTileCount; y++) {
				setTile(0, x, y);
			}
		}
	}

	/**
	 * Used to indicate that a particular tile (set with loadTile and referenced
	 * by an integer) should be drawn at the given x/y coordinates during the
	 * next invalidate/draw cycle.
	 * 
	 * @param tileindex
	 * @param x
	 * @param y
	 */
	private void setTile(int tileindex, int x, int y) {
		if (mTileGrid !=null) {
			mTileGrid[x][y] = tileindex;
		}
	}

	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		for (int x = 0; x < mXTileCount; x += 1) {
			for (int y = 0; y < mYTileCount; y += 1) {
				
					try {
						if (mTileGrid[x][y] > 0) {
						canvas.drawBitmap(mTileArray[mTileGrid[x][y]], mXOffset + x
								* mTileSize, mYOffset + y * mTileSize, mPaint);}
					} catch (Exception e) {
						e.printStackTrace();
					}
					
				
			}
		}

	}

	/**
	 * Simple class containing two integer values and a comparison function.
	 * There's probably something I should use instead, but this was quick and
	 * easy to build.
	 * 
	 */
	private class Coordinate {
		public int x;
		public int y;

		public Coordinate(int newX, int newY) {
			x = newX;
			y = newY;
		}

		@Override
		public String toString() {
			return "Coordinate: [" + x + "," + y + "]";
		}
	}
}
