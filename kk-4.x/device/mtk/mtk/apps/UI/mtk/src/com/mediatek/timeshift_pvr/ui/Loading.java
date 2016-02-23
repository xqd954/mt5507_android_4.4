package com.mediatek.timeshift_pvr.ui;

import java.util.Timer;
import java.util.TimerTask;
import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.widget.TextView;

/*
 * for three points...
 */

public class Loading extends TextView {
	private int count = 0;
	private Timer timer;
	private TimerTask task;
	private static final long delay = 1000;
	private static final long period = 500;

	public Loading(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public void drawLoading() {
		timer = new Timer("loading");
		task = new TimerTask() {
			public void run() {
				StringBuilder builder = new StringBuilder();
				for (int i = 0; i < count % 4; i++) {
					builder.append(" .");
				}
				final String temp = builder.toString();
				Loading.this.post(new Runnable() {
					public void run() {
						setText(temp);
					}
				});
				count++;
			}
		};
		timer.schedule(task, delay, period);
	}

	// stop drawing
	public void stopDraw() {
		task.cancel();
		timer.cancel();
		setText("");
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
	}

}