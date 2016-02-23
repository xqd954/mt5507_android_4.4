package com.mediatek.tvcm;

import java.util.Enumeration;
import java.util.Vector;

import android.content.Context;
import android.os.RemoteException;

import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tvcommon.ITVCommon.TVSelectorListener;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVChannelState;
import com.mediatek.tvcommon.TVChannelState.ChannelListener;

public class TVChannelSelector extends TVComponent {
	static private TVChannelSelector instance;
	private TVChannelState mtvcState;
	
	private TVTeleTextManager mTvTeleTextManager;

	public int finetuneStep = 10000;

	private TVSelectorListener itvcListener = new TVSelectorListener() {

		@Override
		public void onChannelSelected(TVChannel ch) {
			Enumeration<SelectorListener> e = listeners.elements();
			SelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onChannelSelect(ch);
			}

		}

		@Override
		public void onChannelBlocked(TVChannel ch) {
			Enumeration<SelectorListener> e = listeners.elements();
			SelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onBlock(ch);
			}

		}

		@Override
		public void onInputSelected(String outputName, String inputName) {
			// TODO Auto-generated method stub

		}

		@Override
		public void onInputBlocked(String inputName) {
			// TODO Auto-generated method stub

		}

	};

	private ChannelListener mcListener = new ChannelListener() {

		@Override
		public void onChannelSignalChange(boolean hasSignal) {
			Enumeration<SelectorListener> e = listeners.elements();
			SelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onSignalChange(hasSignal);
			}
		}

		@Override
		public void onChannelScramble(boolean hasScramble, String state) {
			Enumeration<SelectorListener> e = listeners.elements();
			SelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onScramble(hasScramble, state);
			}
		}

	};

	public interface SelectorListener {
		public void onChannelSelect(TVChannel ch);

		public void onSignalChange(boolean hasSignal);

		public void onBlock(TVChannel ch);

		public void onScramble(boolean hasScramble, String state);
	}

	private Vector<SelectorListener> listeners = new Vector<SelectorListener>();

	public synchronized void addSelectorListener(SelectorListener listener) {
		if (listeners.size() == 0) {
			mtvcState.addChannelListener(mcListener);
			try {
				if(null != itv){
					itv.addSelectorListener(itvcListener);
				}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}

		listeners.add(listener);
	}

	public synchronized void removeSelectorListener(SelectorListener listener) {
		listeners.remove(listener);

		if (listeners.size() == 0) {
			mtvcState.removeChannelListener(mcListener);
			try {
				if(null != itv){
					itv.removeSelectorListener(itvcListener);
				}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	TVChannelSelector(Context context) {
		super(context);
		mtvcState = TVChannelState.getInstance(context);
		mTvTeleTextManager = TVTeleTextManager.getInstance(context);
	}

	static public TVChannelSelector getInstance(Context context) {
		if (instance == null) {
			instance = new TVChannelSelector(context);
		}
		return instance;
	}

	public TVChannel getCurrentChannel() {
		try {
			if(null != itv){
			return itv.getCurrentChannel();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	public void channelUp() {
		if(null != itv){
		try {
			mTvTeleTextManager.setTTXIconShow(false);
			itv.channelUp();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		}
	}

	public void channelDown() {
		if(null != itv){
		try {
			mTvTeleTextManager.setTTXIconShow(false);
			itv.channelDown();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	 }
	}

	public void selectPrev() {
		if(null != itv){
		try {
			mTvTeleTextManager.setTTXIconShow(false);
			itv.selectPrev();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	 }
	}

	public void select(TVChannel ch) {
		try {
			if(null != itv){
				mTvTeleTextManager.setTTXIconShow(false);
			itv.select(ch);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void select(short num) {
		try {
			if(null != itv){
				mTvTeleTextManager.setTTXIconShow(false);
			itv.select(num);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public boolean hasSignal() {
		return mtvcState.hasSignal();
	}

	public void favoriteChannelUp() {
		try {
			if(null != itv){
			itv.favoriteChannelUp();
			}	
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void favoriteChannelDown() {
		try {
			if(null != itv){
			itv.favoriteChannelDown();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void stop() {
		synchronized (this) {
			BroadcastService brdcstService = (BroadcastService) tvMgr
					.getService(BroadcastService.BrdcstServiceName);
			try {
				if(brdcstService !=null)
				brdcstService.syncStopService();
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
	}

	public String getScrambleState() {
		return mtvcState.getScrambleState();
	}

	public boolean hasScramble() {
		return mtvcState.hasScramble();
	}

	public void finetune(int step) {
		TVChannel currentChannel = null;
		try {
			currentChannel = itv.getCurrentChannel();
			if (currentChannel != null) {
				currentChannel.finetune(step);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}

	}

	public void exitFinetune() {
		TVChannel currentChannel = null;
		try {
			currentChannel = itv.getCurrentChannel();
			if (currentChannel != null)
				currentChannel.exitFinetune();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void setDefaultFinetuneStep(int finetuneStep) {
		this.finetuneStep = finetuneStep;
	}

	public void finetuneLower() {
		finetune(-finetuneStep);
	}

	public void finetuneHigher() {
		finetune(finetuneStep);
	}
}
