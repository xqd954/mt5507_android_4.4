package com.mediatek.tvcm;

import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.DtType;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tvcommon.TVOptionRange;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.os.SystemProperties;
import android.os.SystemClock;
import android.os.PowerManager;
import android.util.Log;


public class TVTimerManager extends TVComponent {
	static TVTimerManager instance;
	private static final String TAG = "TVTimerManager";

	public TVTimerManager(Context context) {
		super(context);
	}

	static public TVTimerManager getInstance(Context context) {
		if (instance == null) {
			instance = new TVTimerManager(context);
			instance.init();
		}

		return instance;
	}

	class TimerNfy implements Runnable {
		@Override
		public void run() {
			while (true) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

				final Vector<TVTimer> timersSpot;
				final Vector<TVTimerListener> listenerSpot;
				synchronized (TVTimerManager.this) {
					timersSpot = (Vector<TVTimer>) startedTimers.clone();
					listenerSpot = (Vector<TVTimerListener>) listeners.clone();
				}

				Enumeration<TVTimer> e = timersSpot.elements();
				while (e.hasMoreElements()) {
					TVTimer t = e.nextElement();

					if (t.getTimeLeft() <= 0) {
						Log.d(TAG,"come in TVTimerManager t.work()");
						t.work();
					}
				}

				getHandler().post(new Runnable() {
					@Override
					public void run() {
						Enumeration<TVTimerListener> e = listenerSpot
								.elements();
						while (e.hasMoreElements()) {
							TVTimerListener l = e.nextElement();
							if (l != null) {
								l.onTimeTick((TVTimer[]) timersSpot
										.toArray(new TVTimer[timersSpot.size()]));
							}
						}
					}
				});
			}
		}
	}

	void init() {
		String data = "tuner_cab_dig_0";
		BroadcastService rawSrv = (BroadcastService) tvMgr
				.getService(BroadcastService.BrdcstServiceName);
		try {
			if(rawSrv !=null){
				int ret = rawSrv.dtSetSyncSrc(DtType.DT_SYNC_WITH_DVB_TDT,
						DtType.DT_SRC_TYPE_MPEG_2_BRDCST, data);
				TvLog("sync broadcast time " + ret);
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		new Thread(new TimerNfy()).start();
	}

	Vector<TVTimer> startedTimers = new Vector<TVTimer>(4);
	Vector<TVTimerListener> listeners = new Vector<TVTimerListener>();

	public static interface TVTimerListener {
		public void onTimeTick(TVTimer[] timers);
	}

	protected Hashtable<String, TVTimer> timers = new Hashtable<String, TVTimer>();

	public synchronized PowerOffTimer getPowerOffTimer(String name) {
		PowerOffTimer timer = (PowerOffTimer) timers.get(name);
		if (timer == null) {
			timer = new PowerOffTimer(name, this);
			timers.put(name, timer);
		}

		return timer;
	}

	synchronized void addStartedTimers(TVTimer timer) {
		startedTimers.add(timer);
	}

	synchronized void removeStartedTimers(TVTimer timer) {
		startedTimers.remove(timer);
	}

	/**
	 * add TVTimerListener listener
	 */
	public synchronized void addTimeListener(TVTimerListener lis) {
		listeners.add(lis);
	}

	/**
	 * remove TVTimerListener listener
	 */
	public synchronized void removeTimeListener(TVTimerListener lis) {
		listeners.remove(lis);
	}

	public static class PowerOffTimer extends TVTimer {
		PowerOffTimer(String name, TVTimerManager tm) {
			super(name, tm);
		}

		void work() {	
		    if (tm.context != null) {
		        PreferenceManager.getDefaultSharedPreferences(tm.context).edit().putInt("power_off_timer", 1).commit();
	            tm.getContent().flushMedia();
            }
		   
			if(("1".equals(SystemProperties.get("ro.pm.warmboot")))&&
				("0".equals(SystemProperties.get("mtk.auto_test"))) && tm.context != null){				
				Log.d(TAG,"come in TVTimerManager PowerOffTimer pm.goToSleep");
				PowerManager pm = (PowerManager)tm.context.getSystemService(
							Context.POWER_SERVICE);
	            pm.goToSleep(SystemClock.uptimeMillis());
	            cancel();
			}else{
				Log.d(TAG,"come in TVTimerManager PowerOffTimer tm.getContent().sendPowerOff()");
				tm.getContent().sendPowerOff();
				cancel();
			}
		}
	}


	protected final static String POWERON_CHANNEL = "poweron_channel";

	public short getPowerOnChannel() {
		TVStorage st = getContent().getStorage();
		String chNum = st.get(POWERON_CHANNEL);
		if(chNum != null) {
			return new Short(chNum).shortValue();
		}else {
			return 0;
		}
	}

	/**
	 * Set the power on channel, when next boot up, TV will select this channel.
	 */
	public void setPowerOnChannel(Short chNum) {
		 TVStorage st = getContent().getStorage();
		 st.set(POWERON_CHANNEL, new Short(chNum).toString());
	}

	public void setPowerOnTimer(Date time) {
		TVOptionRange<Integer> pt = (TVOptionRange<Integer>) getContent()
				.getConfigurer().getOption(ConfigType.CFG_POWER_ON_TIMER);
		if (pt != null) {
			pt.set((int) (time.getTime() / 1000));
		}
	}

	public long getBroadcastUTC() {
		BroadcastService rawSrv = (BroadcastService) TVManager.getInstance(
				context).getService(BroadcastService.BrdcstServiceName);
		// Sorry for this arugments,
		// TVManager told me that these are 'output'
		// arguments. I don't need to init them.
		int[] args = new int[2];
		try {
			// That is in second...
			if(rawSrv !=null){
				return rawSrv.dtGetBrdcstUtc(args) * 1000;
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return System.currentTimeMillis();
	}
	
	public long getUTC(){
		BroadcastService rawSrv = (BroadcastService) TVManager.getInstance(
				context).getService(BroadcastService.BrdcstServiceName);
		// Sorry for this arugments,
		// TVManager told me that these are 'output'
		// arguments. I don't need to init them.
		int[] args = new int[2];
		try {
			// That is in second...
			if(rawSrv !=null){
				Log.d(TAG,"dtGetUtc:"+rawSrv.dtGetUtc(args) * 1000);
				return rawSrv.dtGetUtc(args) * 1000;
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
		Log.d(TAG,"currentTimeMillis:"+System.currentTimeMillis());
		return System.currentTimeMillis();
	}

	public long getBroadcastTZOffset() {
		BroadcastService rawSrv = (BroadcastService) TVManager.getInstance(
				context).getService(BroadcastService.BrdcstServiceName);
		try {
			// That is in second...
			if(rawSrv !=null){
				return rawSrv.dtGetTz() * 1000;
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return 0;
	}
}
