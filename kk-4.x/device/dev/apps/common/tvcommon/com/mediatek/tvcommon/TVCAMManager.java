package com.mediatek.tvcommon;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.mediatek.tv.model.CIListener;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.HostControlTune;
import com.mediatek.tv.model.MMI;
import com.mediatek.tv.model.MMIEnq;
import com.mediatek.tv.model.MMIMenu;
import com.mediatek.tv.service.CIService;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;

public class TVCAMManager {
	private static TVCAMManager tvCAMManager = null;
	protected CIService ciService = null;
	private CAMMenu camMenu = null;
	private CAMEnquiry camEnquiry = null;
	private MMI mmi = null;
	private volatile boolean changed = true;
	private Context mContext = null;
	private Handler mHandler = null;
	private ITVCommon itv = null;
	public static synchronized TVCAMManager getInstance(Context context) {
		if (tvCAMManager == null) {
			tvCAMManager = new TVCAMManager(context);
		}

		return tvCAMManager;
	}

	protected TVCAMManager(Context context) {
		mContext = context.getApplicationContext();
		mHandler = new Handler(mContext.getMainLooper(),
				new Handler.Callback() {
					@Override
					public boolean handleMessage(Message msg) {
						// TODO Auto-generated method stub
						TVCAMManager.this.handleMessage(msg);
						return true;
					}
				});
		
		init();
	}

	protected void handleMessage(Message msg) {
		// TODO Auto-generated method stub
	}

	void init() {
		camMenu = new CAMMenu();
		camEnquiry = new CAMEnquiry();

		// Currently, our hardware only support one CI slot.
		ciService = CIService.getInstance(0);
		ciService.addCIListener(new CIListenerDelegater());
		itv = TVCommonNative.getDefault(mContext);
	}

	public static class CAMMenu {
		private MMIMenu menu = null;

		protected void setMMIMenu(MMIMenu menu) {
			this.menu = menu;
		}

		protected MMIMenu getMMIMenu() {
			return menu;
		}

		public String getTitle() {
			if (menu != null) {
				return menu.getTitle();
			}
			return null;
		}

		public String getSubtitle() {
			if (menu != null) {
				return menu.getSubtitle();
			}
			return null;
		}

		public String getBottom() {
			if (menu != null) {
				return menu.getBottom();
			}
			return null;
		}

		public String[] getItemList() {
			if (menu != null) {
				return menu.getItemList();
			}
			return null;
		}

		public void selectMenuItem(int num) {
			if (menu != null) {
				if (num >= 0 || num <= menu.getItemList().length - 1) {
					num += 1;
					menu.answer((char) num);
				}
			}
		}

		public void cancelCurrMenu() {
			if (menu != null) {
				menu.answer((char) 0);
			}
		}

		public String toString() {
			StringBuffer str = new StringBuffer("\n=== CAMMenu ===");
			str.append("\nTitle:" + getTitle());
			str.append("\nSubtitle:" + getSubtitle());
			str.append("\nBottom:" + getBottom());
			String [] sArray = getItemList();
			if(sArray != null)
			for (String item : sArray) {
				str.append("\nItem:" + item);
			}
			str.append("\n=== CAMMenu ===");
			return str.toString();
		}
	}

	public static class CAMEnquiry {
		private MMIEnq enq = null;

		protected void setMMIEnq(MMIEnq enq) {
			this.enq = enq;
		}

		protected MMIEnq getMMIEnq() {
			return enq;
		}

		public String getText() {
			return enq.getText();
		}

		public byte getAnsTextLen() {
			return enq.getAnsTextLen();
		}

		public boolean isBlindAns() {
			return enq.isBlindAns();
		}

		public void answerEnquiry(boolean isAns, String pwd) {
			enq.answer(isAns, pwd);
		}

		public String toString() {
			StringBuffer str = new StringBuffer("\n=== CAMEnquiry ===");
			str.append("\nTitle:" + getText());
			str.append("\nAnswer length:" + getAnsTextLen());
			str.append("\n=== CAMEnquiry ===");
			return str.toString();
		}
	}

	class CIListenerDelegater implements CIListener {

		public int camHostControlClearReplace() {
			notifyHostControlClearReplace();
			return 0;
		}

		public int camHostControlReplace() {
			notifyHostControlReplace();
			return 0;
		}

		public int camHostControlTune() {
			HostControlTune tune = ciService.getHostControlTune();
			ChannelInfo info;
			
			try {
				info = tune.getTunedChannel(itv.getCurrentChannel().rawInfo);
				List<TVChannel> list = itv.getChannels();

				Iterator<TVChannel> it = list.iterator();
				while (it.hasNext()) {
					TVChannel ch = it.next();
					if (ch.rawInfo.equals(info)) {
						notifyHostControlTune(ch);
					}
				}
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			return 0;
		}

		public int camMMIClosed(byte arg0) {
			notifyMMIClosed(arg0);
			return 0;
		}

		public int camMMIEnqReceived() {
			notifyMMIEnqReceived(ciService.getMMIEnq());
			return 0;
		}

		public int camMMIMenuReceived() {
			notifyMMIMenuReceived(ciService.getMMIMenu());
			return 0;
		}

		public int camStatusUpdated(byte arg0) {
			switch (arg0) {
			case CIListener.CI_CAM_STATUS_INSERT:
				notifyCamInserted();
				break;

			case CIListener.CI_CAM_STATUS_NAME:
				notifyCamNamed(ciService.getCamName());
				break;

			case CIListener.CI_CAM_STATUS_REMOVE:
				notifyCamRemoved();
				break;

			default:
				break;
			}
			return 0;
		}

		public int camSystemIDStatusUpdated(byte arg0) {
			notifySystemIDStatusUpdated(arg0);
			return 0;
		}

	}

	public interface CamStatusUpdateListener {
		void camInsertUpdated();

		void camNameUpdated(String camName);

		void camRemoveUpdated();

		void camSystemIDStatusUpdated(byte sys_id_status);
	}

	public interface MenuEnqUpdateListener {
		void enqReceived(CAMEnquiry enquiry);

		void menuReceived(CAMMenu menu);

		void menuEnqClosed(byte mmiCloseDelay);
	}

	public interface CamHostControlListener {
		void camHostControlClearReplace();

		void camHostControlReplace();

		void camHostControlTune(TVChannel ch);
	}

	private List<CamStatusUpdateListener> camStatusListeners = new ArrayList<CamStatusUpdateListener>();
	private List<MenuEnqUpdateListener> menuEnqUpdateListeners = new ArrayList<MenuEnqUpdateListener>();
	private List<CamHostControlListener> camHostControlListeners = new ArrayList<CamHostControlListener>();

	public void registerCamStatusListener(CamStatusUpdateListener listener) {
		if (listener == null) {
			throw new IllegalArgumentException();
		}
		synchronized (this) {
			if (!camStatusListeners.contains(listener)) {
				camStatusListeners.add(listener);
			}
		}
	}

	public void removeCamStatusListener(CamStatusUpdateListener listener) {
		camStatusListeners.remove(listener);
	}

	public void clearCamStatusListeners() {
		camStatusListeners.clear();
	}

	public void registerMenuEnqUpdateListener(MenuEnqUpdateListener listener) {
		if (listener == null) {
			throw new IllegalArgumentException();
		}
		synchronized (this) {
			if (!menuEnqUpdateListeners.contains(listener)) {
				menuEnqUpdateListeners.add(listener);
			}
		}
	}

	public void removeMenuEnqUpdateListener(MenuEnqUpdateListener listener) {
		menuEnqUpdateListeners.remove(listener);
	}

	public void clearMenuEnqUpdateListener() {
		menuEnqUpdateListeners.clear();
	}

	public void registerCamHostControlListener(CamHostControlListener listener) {
		if (listener == null) {
			throw new IllegalArgumentException();
		}
		synchronized (this) {
			if (!camHostControlListeners.contains(listener)) {
				camHostControlListeners.add(listener);
			}
		}
	}

	public void removeCamHostControlListener(CamHostControlListener listener) {
		camHostControlListeners.remove(listener);
	}

	public void clearCamHostControlListener() {
		camHostControlListeners.clear();
	}

	class DelegaterMMIClosed implements Runnable {
		private byte mmiCloseDelay;

		public DelegaterMMIClosed(byte mmiCloseDelay) {
			this.mmiCloseDelay = mmiCloseDelay;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<MenuEnqUpdateListener> it = menuEnqUpdateListeners
					.iterator();
			while (it.hasNext()) {
				it.next().menuEnqClosed(mmiCloseDelay);
			}
		}

	}

	private void notifyMMIClosed(final byte mmiCloseDelay) {
		mHandler.post(new DelegaterMMIClosed(mmiCloseDelay));
	}

	class DelegaterMMIEnqReceived implements Runnable {
		private MMIEnq enq;

		public DelegaterMMIEnqReceived(MMIEnq enq) {
			this.enq = enq;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<MenuEnqUpdateListener> it = menuEnqUpdateListeners
					.iterator();
			mmi = enq;
			camEnquiry.setMMIEnq(enq);
			while (it.hasNext()) {
				it.next().enqReceived(camEnquiry);
			}
		}
	}

	private void notifyMMIEnqReceived(final MMIEnq enq) {
		mHandler.post(new DelegaterMMIEnqReceived(enq));
	}

	class DelegaterMMIMenuReceived implements Runnable {
		private MMIMenu menu;

		public DelegaterMMIMenuReceived(MMIMenu menu) {
			this.menu = menu;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<MenuEnqUpdateListener> it = menuEnqUpdateListeners
					.iterator();
			mmi = menu;
			camMenu.setMMIMenu(menu);
			while (it.hasNext()) {
				it.next().menuReceived(camMenu);
			}
		}
	}

	private void notifyMMIMenuReceived(final MMIMenu menu) {
		mHandler.post(new DelegaterMMIMenuReceived(menu));
	}

	class DelegaterHostControlTune implements Runnable {
		private TVChannel ch;

		public DelegaterHostControlTune(TVChannel ch) {
			this.ch = ch;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamHostControlListener> it = camHostControlListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camHostControlTune(ch);
			}
		}

	}

	private void notifyHostControlTune(final TVChannel ch) {
		mHandler.post(new DelegaterHostControlTune(ch));
	}

	class DelegaterHostControlReplace implements Runnable {

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamHostControlListener> it = camHostControlListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camHostControlReplace();
			}
		}

	}

	private void notifyHostControlReplace() {
		mHandler.post(new DelegaterHostControlReplace());
	}

	class DelegaterHostControlClearReplace implements Runnable {

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamHostControlListener> it = camHostControlListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camHostControlClearReplace();
			}
		}

	}

	private void notifyHostControlClearReplace() {
		mHandler.post(new DelegaterHostControlClearReplace());
	}

	class DelegaterCamNamed implements Runnable {
		private String name;

		public DelegaterCamNamed(String name) {
			this.name = name;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamStatusUpdateListener> it = camStatusListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camNameUpdated(name);
			}
		}
	}

	private void notifyCamNamed(final String name) {
		mHandler.post(new DelegaterCamNamed(name));
	}

	class DelegaterCamInserted implements Runnable {

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamStatusUpdateListener> it = camStatusListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camInsertUpdated();
			}
		}

	}

	private void notifyCamInserted() {
		mHandler.post(new DelegaterCamInserted());
	}

	class DelegaterCamRemoved implements Runnable {

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamStatusUpdateListener> it = camStatusListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camRemoveUpdated();
			}
		}

	}

	private void notifyCamRemoved() {
		mHandler.post(new DelegaterCamRemoved());
	}

	class DelegaterSystemIDStatusUpdated implements Runnable {
		private byte sysIdStatus;

		public DelegaterSystemIDStatusUpdated(byte sysIdStatus) {
			this.sysIdStatus = sysIdStatus;
		}

		public void run() {
			if (!changed) {
				return;
			}
			Iterator<CamStatusUpdateListener> it = camStatusListeners
					.iterator();
			while (it.hasNext()) {
				it.next().camSystemIDStatusUpdated(sysIdStatus);
			}
		}

	}

	private void notifySystemIDStatusUpdated(final byte sysIdStatus) {
		mHandler.post(new DelegaterSystemIDStatusUpdated(sysIdStatus));
	}

	protected void switchInputPath(boolean on_off) {
		ciService.getCIInputDTVPath().switchPath(on_off);
	}

	protected void switchTSPath(boolean on_off) {
		ciService.getCITSPath().switchPath(on_off);
	}

	public int getCISlotNum() {
		return CIService.getSlotNum();
	}

	public String getCamName() {
		return ciService.getCamName();
	}

	public void closeCurrentMMI() {
		if (mmi != null) {
			mmi.close();
		}
	}

	public CAMMenu getCamMenu() {
		return camMenu;
	}

	public CAMEnquiry getCamEnquiry() {
		return camEnquiry;
	}

	public boolean isSlotActive() {
		return ciService.isSlotActive();
	}

	public void enterCAMMainMenu() {
		ciService.enterMMI();
	}

	public void selectMenuItem(int num) {
		camMenu.selectMenuItem(num);
	}

	public void cancelCurrMenu() {
		camMenu.cancelCurrMenu();
	}

	public void answerEnquiry(boolean isAns, String pwd) {
		camEnquiry.answerEnquiry(isAns, pwd);
	}

	public void enableNotification(boolean changed) {
		this.changed = changed;
	}
}
