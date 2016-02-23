package com.mediatek.ui.mmp.commonview;

import java.util.List;
import java.util.Vector;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import com.mediatek.mmpcm.audioimpl.LyricTimeContentInfo;
import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;

public class LrcView extends View {

	private static final String TAG = "LrcView";
	private static Vector<LyricTimeContentInfo> lrc_map;
	private Paint mWhitePaint;

	private Paint mRedPaint;
	private int[] lrcWidth;
	private int lrcHeight = 0;
	private int viewWidth = 0;
	private List<LyricTimeContentInfo> lrcarr;
	private int mCurrentLine = 0;
	private int mLrcLine;
	private int Lines = 8;
	private String noLrc;

	public LrcView(Context context, AttributeSet attrs) {
		super(context, attrs);
		noLrc = context.getString(R.string.mmp_info_nolrc);
		mWhitePaint = new Paint();
		mRedPaint = new Paint();
	}

	
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		if (lrcarr == null) {
			// canvas.drawText(noLrc, viewWidth / 3, lrcHeight / 2,
			// mWhitePaint);
			return;
		}
		for (int i = 0; i < lrcarr.size(); i++) {
			canvas.drawText(lrcarr.get(i).getLyricContent(), lrcWidth[i], 40
					+ lrcHeight * i, mWhitePaint);
		}
		// show current music word

		mRedPaint.setColor(Color.GREEN);
		if (mCurrentLine < lrcarr.size()) {
			canvas.drawText(lrcarr.get(mCurrentLine).getLyricContent(),
					lrcWidth[mCurrentLine], 40 + lrcHeight * mCurrentLine,
					mRedPaint);
		}
	}

	public void noLrc(String lrc) {
		lrcarr = null;
		noLrc = lrc;
		viewWidth = this.getWidth();
		lrcHeight = this.getHeight();
		mWhitePaint.setTextSize(24);

		mWhitePaint.setColor(Color.WHITE);
		this.invalidate();
	}

	public void init(Vector<LyricTimeContentInfo> lrc, int line) {
		mCurrentLine = 0;
		lrc_map = lrc;
		// Modified by Dan for fix hide lrc bug
		if (line == 0) {
			Lines = 8;
			setVisibility(View.INVISIBLE);
		} else {
			Lines = line;
			setVisibility(View.VISIBLE);
		}
		
		lrcarr = new Vector<LyricTimeContentInfo>(Lines);
		lrcWidth = new int[Lines];
		if (mLrcLine >= lrc_map.size() - 1) {
			mLrcLine = 0;
		}
		if (mLrcLine + Lines < lrc_map.size()) {
			lrcarr = lrc_map.subList(mLrcLine, mLrcLine + Lines);
		} else {
			lrcarr = lrc_map.subList(mLrcLine, lrc_map.size());
		}

		mWhitePaint.setTextSize(24);
		mRedPaint.setTextSize(24);

		mWhitePaint.setColor(Color.WHITE);

		viewWidth = this.getWidth();
		for (int i = 0; i < lrcarr.size(); i++) {
			lrcWidth[i] = (viewWidth - (int) mWhitePaint.measureText(lrcarr
					.get(i).getLyricContent())) / 2;
		}
		lrcHeight = this.getHeight() / Lines;
	}
	
	public void setLines(int line)
	{
		Lines =line;
	}

	public void setlrc(int currentline,boolean isRefreshImmediately) {

		if (lrc_map == null) {
			return;
		}
		if (currentline > lrc_map.size() - 1) {
			return;
		}
		if (currentline != mLrcLine||isRefreshImmediately) {
			MtkLog.i(TAG, "--------  mLrcLine  :" + mLrcLine
					+ "    currentline:" + currentline);
			mLrcLine = currentline;
			int endline;
			if (currentline % Lines == 0) {
				mCurrentLine = 0;
			} else {
				mCurrentLine = currentline % Lines;
				currentline = (currentline / Lines) * Lines;
			}

			if ((currentline + Lines) >= lrc_map.size()) {
				endline = lrc_map.size();
			} else {
				endline = currentline + Lines;
			}
			lrcarr = lrc_map.subList(currentline, endline);
			for (int i = 0; i < lrcarr.size(); i++) {
				lrcWidth[i] = (viewWidth - (int) mWhitePaint.measureText(lrcarr
						.get(i).getLyricContent())) / 2;
			}
			this.invalidate();
		}
	}
}
