package com.mediatek.tvcm;

import java.util.Date;
import java.util.Timer;

public abstract class TVTimer {
	TVTimerManager tm = null;
	private long delay;
	Date date;
	boolean started = false;
	final String name;
	int type;
	
	public static final int TYPE_FIXED=0;
	public static final int TYPE_DELAY=1;
	
	TVTimer(String name, TVTimerManager tm) {
		this.name = name;
		this.tm = tm;
	}
	
	Timer sleepTimer = null;
	
	/**
	 * @return the name of TV timer
	 */
	public String getName() {
		return name;
	}

	/**
	 * @return the type of TV timer, either TYPE_FIXED or TYPE_DELAY
	 */
	public synchronized int getType() {
		return type;
	}
	
	public synchronized void setTimer(Date date){
		type = TYPE_FIXED;
		this.date = (Date) date.clone();
	}
	
	/**
	 * Set the delay time
	 * 
	 * @param delay
	 *            delay time in milliseconds since January 1, 1970 00:00:00 UTC.
	 */
	public synchronized void setTimer(long delay) {
		type = TYPE_DELAY;		
		this.delay = delay;
		if(started) {
			date = new Date(System.currentTimeMillis() + delay);
		}
	}
	
	public synchronized long getTimeLeft() {
		if (started && (null !=date)) {
			return date.getTime() - System.currentTimeMillis();
		}
		
		return 0;
	}
	
	public synchronized boolean isStarted() {
		return started;
	}
	
	
	abstract void work();
	
	/**
	 * Cancel the timer.
	 */
	public synchronized void cancel() {
		started = false;
		tm.removeStartedTimers(this);
	}
	
	public synchronized void start() {
		if(type == TYPE_DELAY) {
			date = new Date(System.currentTimeMillis() + delay);
		}
		
		if(started) {
			return ;
		}
		
		started = true;
		tm.addStartedTimers(this);
	}
}
