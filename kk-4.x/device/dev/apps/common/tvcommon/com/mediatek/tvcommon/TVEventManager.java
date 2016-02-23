package com.mediatek.tvcommon;

import java.util.ArrayList;
import java.util.Vector;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.EventInfo;
import com.mediatek.tv.model.EventUpdateReason;
import com.mediatek.tv.service.EventService;
import com.mediatek.tv.service.IEventNotify;

/**
 * This Class mainly manage Electronic program guides (EPG) which provide users
 * of television, radio, and other media applications with continuously updated
 * menus displaying broadcast programming or scheduling information for current
 * and upcoming programming.
 */
public class TVEventManager {
	static private TVEventManager instance = null;
	protected EventService rawSrv;
	private Handler mHandler;

	class IEventNotifyDelegate implements IEventNotify {
		public void notifyUpdate(EventUpdateReason reason, final int svlid,
				final int channelID) {
			switch (reason) {
			case EVENT_REASON_PF_UPDATE:
				// TODO: Check id with current channel.
				notifyPFChange();
				break;
			case EVENT_REASON_SCHEDULE_UPDATE:
				notifySCHEDULEChange();
				break;
			default:
				break;
			}
		}
	}

	TVEventManager(Context context) {
		rawSrv = (EventService) TVManager.getInstance().getService(
				EventService.EventServiceName);
		if(rawSrv != null)
			rawSrv.addListener(new IEventNotifyDelegate());
		mHandler = new Handler(Looper.getMainLooper());
	}

	public static TVEventManager getInstance(Context context) {
		if (instance == null) {
			instance = new TVEventManager(context);
		}

		return instance;
	}

	private Vector<PFEventListener> pfListeners = new Vector<PFEventListener>(3);

	/**
	 * register PFEventListener
	 */
	public synchronized void registerPFEventListener(PFEventListener listener) {
		pfListeners.add(listener);
	}

	/**
	 * unregister PFEventListener
	 */
	public synchronized void unregisterPFEventListener(PFEventListener listener) {
		pfListeners.remove(listener);
	}

	protected void notifyPFChange() {
		@SuppressWarnings("unchecked")
		final Vector<PFEventListener> listeners = (Vector<PFEventListener>) pfListeners
				.clone();

		mHandler.post(new Runnable() {
			public void run() {
				for (PFEventListener listener : listeners) {
					listener.onChange();
				}
			}
		});
	}

	/**
	 * @return the current and the next TV event of current TV channel.
	 */
	public TVEvent[] getPFEvent() {
		TVEvent[] events = new TVEvent[] { null, null };
		ITVCommon itv = TVCommonNative.getDefault(null);
		try {
			if(itv == null)
				return null;
			TVChannel curCh = itv.getCurrentChannel();
			if (curCh != null) {
				rawSrv.setCurrentChannel(curCh.rawInfo);
				ArrayList<EventInfo> rawEvents = rawSrv
						.getPFEvents(curCh.rawInfo);

				if (rawEvents.size() > 0) {
					if (rawEvents.get(0) != null) {
						events[0] = new TVEvent(rawEvents.get(0), curCh);
					}
				}
				if (rawEvents.size() > 1) {
					if (rawEvents.get(1) != null) {
						events[1] = new TVEvent(rawEvents.get(1), curCh);
					}
				}
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return events;
	}

	/**
	 * Interface definition for a callback to invoke when either current or the
	 * next TV event changed.
	 */
	public static interface PFEventListener {
		/**
		 * Called when either the current or the next TV event changed.
		 */
		public void onChange();
	}
	private Vector<SCHEDULEEventListener> mSCHEDULEEventListeners = new Vector<SCHEDULEEventListener>(3);
	
	public synchronized void registerSCHEDULEEventListener(SCHEDULEEventListener listener) {
		mSCHEDULEEventListeners.add(listener);
	}
	
	public synchronized void unregisterSCHEDULEEventListener(SCHEDULEEventListener listener) {
		mSCHEDULEEventListeners.remove(listener);
	}
	
	protected void notifySCHEDULEChange() {
		@SuppressWarnings("unchecked")
		final Vector<SCHEDULEEventListener> listeners = (Vector<SCHEDULEEventListener>) mSCHEDULEEventListeners
				.clone();

		mHandler.post(new Runnable() {
			public void run() {
				for (SCHEDULEEventListener listener : listeners) {
					listener.onChange();
				}
			}
		});
	}
	public static interface SCHEDULEEventListener{
		public void onChange();
	}

	/**
	 * Interface definition for a callback to invoke when read TV events from
	 * monitor.
	 */
	public static interface EventListener {
		public void onChange(TVChannel ch);
	}

	TVSchedEventMonitor schedMonitor = new TVSchedEventMonitor(this);

	public TVSchedEventMonitor getSchedMonitor() {
		return schedMonitor;
	}

	public void setCommand(TVEventCommand eventCommand) {
		try {
			if(null != rawSrv){
			rawSrv.eventSetCommand(eventCommand);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}
}
