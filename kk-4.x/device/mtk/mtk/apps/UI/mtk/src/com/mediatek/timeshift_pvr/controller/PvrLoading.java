package com.mediatek.timeshift_pvr.controller;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.widget.TextView;

/*
 * for three points...
 */

public class PvrLoading extends TextView {
	private int count = 0;
	private Timer timer;
	private TimerTask task;
	private static final long delay = 500;
	private static final long period = 500;
	
	private String mTextValue="";

	public PvrLoading(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public void drawLoading() {
		timer = new Timer("loading");
		task = new TimerTask() {
			public void run() {
				StringBuilder builder = new StringBuilder();
				builder.append(mTextValue);
				for (int i = 0; i < count % 4; i++) {
					builder.append(" .");
				}
				final String temp = builder.toString();
				PvrLoading.this.post(new Runnable() {
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
	
	protected void setTextValue(String text){
		mTextValue=text;
		setText(text);
	}

}