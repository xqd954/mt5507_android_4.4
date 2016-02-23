package com.mediatek.tvcommon;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Map;
import java.util.TreeSet;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;
import com.mediatek.tvcommon.TVEventManager.EventListener;


import android.os.Handler;

import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.EventActiveWindow;
import com.mediatek.tv.model.EventInfo;

/**
 * TVSchedEventMonitor is TV events monitor. When you need to get TV events, you
 * need to tell detail informations which TV events you are interest to monitor.
 */
public class TVSchedEventMonitor {
	long startTime;
	long duration;
	TVChannel[] channels = null;
	private EventListener listener;
	Map<TVChannel, TreeSet<TVEvent>> map;
	private Handler mHandler = new Handler();

	TVEventManager em;

	TVSchedEventMonitor(TVEventManager em) {
		this.em = em;
		// have the effect of a copy on write map
		map = new ConcurrentHashMap<TVChannel, TreeSet<TVEvent>>();
	}

	void reset() {
		map.clear();
	}

	/**
	 * Set the start time and duration to monitor.
	 */
	public void setTime(long startTime, long duration) {
		this.startTime = startTime;
		this.duration = duration;
	}

	/**
	 * Set the TV channels to monitor
	 */
	public void setChannels(TVChannel[] channels) {
		this.channels = channels;
	}

	/**
	 * Set listener to TV monitor.
	 */
	public void setListener(EventListener listener) {
		this.listener = listener;
	}

	TVChannel lookforChannelFromRaw(ChannelInfo rawCh) {
		if (channels != null) {
			for (TVChannel ch : channels) {
				if (ch.rawInfo.equals(rawCh)) {
					return ch;
				}
			}
		}
		return null;
	}

	TVChannel lookforChannelFromRaw(int svlId, int channelId) {
		if (channels != null) {
			for (TVChannel ch : channels) {
				if (ch.rawInfo.getChannelId() == channelId
						&& ch.rawInfo.getSvlId() == svlId) {
					return ch;
				}
			}
		}
		return null;
	}

	void addInfo(TVChannel ch, TVEvent event) {
		TreeSet<TVEvent> ts = map.get(ch);
		if (ts == null) {
			ts = new TreeSet<TVEvent>(new Comparator<TVEvent>() {
				public int compare(TVEvent e1, TVEvent e2) {
					if (e1.getStartTime() < e2.getStartTime()) {
						return -1;
					} else if (e1.getStartTime() > e2.getStartTime()) {
						return 1;
					} else {
						return 0;
					}
				}
			});
			map.put(ch, ts);
		}
		ts.add(event);
	}

	/**
	 * Get TV events which you need. Before you invoke this method, you should
	 * set time, Channel to monitor and then read from monitor.
	 * <p>
	 * e.g.
	 * <p>
	 * setTime (1321594984000L, 2*3600*1000);
	 * <p>
	 * setChannels (channels);
	 * <p>
	 * syncRead();
	 * <p>
	 * for (TVChannel ch: channels){
	 * <p>
	 * getEvents(ch);}
	 * <p>
	 */
	public TVEvent[] getEvents(TVChannel ch) {
		TreeSet<TVEvent> ts = map.get(ch);
		if (ts == null) {
			return null;
		} else {
			return ts.toArray(new TVEvent[ts.size()]);
		}
	}

	void notifyCh(final TVChannel ch) {
		mHandler.post(new Runnable() {
			public void run() {
				listener.onChange(ch);
			}
		});
	}

	void onSchedulChange(int svlId, int channelId) {
		final TVChannel ch = this.lookforChannelFromRaw(svlId, channelId);
		if (ch == null) {
			return;
		}

		Map<ChannelInfo, ArrayList<EventInfo>> rawEventMap;
		try {
			rawEventMap = em.rawSrv
					.getScheduleEvents(new ChannelInfo[] { ch.rawInfo });
			if (rawEventMap == null) {
				return;
			}
			map.remove(ch);
			for (ChannelInfo rawCh : rawEventMap.keySet()) {
				ArrayList<EventInfo> rawEvents = rawEventMap.get(rawCh);
				for (EventInfo rawEvent : rawEvents) {
					addInfo(ch, new TVEvent(rawEvent, ch));
				}
			}

			listener.onChange(ch);
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	/**
	 * If you set a long time duration and many channels, it may need to spend
	 * many time to read TV events from monitor. This read operation should do
	 * in a new thread in order to avoid block UI thread.
	 */
	public void asyncRead() {
		new Thread(new Runnable() {

			public void run() {
				read(true);
			}
		}).start();

	}

	/**
	 * Read TV events from monitor. This is a synchronized way.
	 */
	public void syncRead() {
		read(false);
	}

	void read(boolean notifyEach) {
		reset();

		EventActiveWindow ew = new EventActiveWindow();
		Vector<ChannelInfo> rawChs = new Vector<ChannelInfo>();

		for (TVChannel ch : channels) {
			rawChs.add(ch.rawInfo);
		}
		ChannelInfo[] rawChArray = (ChannelInfo[]) rawChs
				.toArray(new ChannelInfo[rawChs.size()]);
		ew.setChannels(rawChArray);
		ew.setStartTime(startTime / 1000);
		ew.setDuration(duration / 1000);

		// TODO: New Async task or Thread.
		try {
			em.rawSrv.setAciveWindow(ew);
			Map<ChannelInfo, ArrayList<EventInfo>> rawEventMap = em.rawSrv
					.getScheduleEvents(rawChArray);
			if(rawEventMap != null)
			for (ChannelInfo rawCh : rawEventMap.keySet()) {
				ArrayList<EventInfo> rawEvents = rawEventMap.get(rawCh);
				TVChannel ch = lookforChannelFromRaw(rawCh);
				for (EventInfo rawEvent : rawEvents) {
					addInfo(ch, new TVEvent(rawEvent, ch));
				}
				if (notifyEach) {
					notifyCh(ch);
				}
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
