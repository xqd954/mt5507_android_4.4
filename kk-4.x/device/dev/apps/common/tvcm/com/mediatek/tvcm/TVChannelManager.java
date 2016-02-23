package com.mediatek.tvcm;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.os.RemoteException;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.TVCommon;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.AnalogChannelInfo;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tvcommon.ITVCommon.ChannelFilter;
import com.mediatek.tvcommon.ITVCommon.ChannelsChangedListener;
import com.mediatek.tvcommon.TVChannel;
//import com.mediatek.ui.R.string;

public class TVChannelManager extends TVComponent {
	static private TVChannelManager instance;

	private List<ChannelInfo> palChInfo = new ArrayList<ChannelInfo>();
	public final static String LIST_DVB_NAME = "dtv";

	TVChannelManager(Context context) {
		super(context);
	}

	static public TVChannelManager getInstance(Context context) {
		if (instance == null) {
			instance = new TVChannelManager(context);
		}
		return instance;
	}

	// is this method to get all channels?
	public List<TVChannel> getChannels() {
		try {
			if(null != itv){
			List<TVChannel>  mChannelList =itv.getChannels();
			if(null==mChannelList)
			{
				mChannelList =new ArrayList<TVChannel>(1);
			}
			return mChannelList;
			}	
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return null;
	}

	public List<TVChannel> getChannels(int startIdx, ChannelFilter filter) {
		try {
			List<TVChannel> channels = itv.getChannels(filter);
			if(null != channels){
				int count = channels.size();
				return itv.getChannels(startIdx, count, filter);
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return null;
	}
	
	public List<TVChannel> getChannelList(String inputName){
		List<TVChannel>  mChannelList = null;
		if(null != itv){
			try {
				mChannelList = itv.getChannels(inputName);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return mChannelList;
	}

	// TODO Need to add the true channel filter, and this is the only temple
	// usage.
	// public interface ChannelFilter {
	// public boolean filter(TVChannel channel);
	// }

	public void deleteChannel(TVChannel ch) {
		try {
			itv.deleteChannel(ch);
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void swapChannel(TVChannel ch1, TVChannel ch2) {
		try {
			itv.swapChannel(ch1, ch2);
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void setPalPreSetChannel() {
		// new add for dumy
		if (null == tvMgr) {
			return;
		}
		if (palChInfo != null) {
			try {
				TVManager mTVMngr = tvMgr;
				if (mTVMngr != null) {
					ChannelService cs = (ChannelService) mTVMngr
							.getService(ChannelService.ChannelServiceName);
					if (cs != null) {
						cs.setChannelList(
								ChannelService.ChannelOperator.APPEND,
								ChannelCommon.DB_CABEL, palChInfo);
					}
				}
				// update(ChannelCommon.DB_CABEL);

				flush();
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public void addPalPreSetChannel(PalPreSetChannel ch) {
		if (ch != null) {
			AnalogChannelInfo chInfo = new AnalogChannelInfo(
					ChannelCommon.DB_CABEL);
			chInfo.setBrdcstMedium(TVCommon.BRDCST_MEDIUM_ANA_CABLE);
			chInfo.setChannelNumber(ch.getChannelNum());
			chInfo.setServiceName(ch.getChannelName());
			chInfo.setFrequency(ch.getFrequency());
			chInfo.setColorSys(ch.getColorSys());
			chInfo.setTvSys(ch.getTvSys());
			chInfo.setAudioSys(ch.getAudioSys());
			palChInfo.add(chInfo);
		}
	}

	public void clearPalPreSetChannel() {
		palChInfo.clear();
	}

	public void clear() {
		try {
			if(null != itv){
			 itv.clearAllChannels();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void insertChannel(TVChannel dst, TVChannel src) {
		try {
			itv.insertChannel(dst, src);
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public int getChannelLength() {
		try {
			List<TVChannel>  mChannelList =itv.getChannels();
			if(mChannelList !=null){
				return itv.getChannels().size();
			}
			return 0;
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return 0;
	}

	public TVChannel findChannelByNumber(int num) {
		try {
			return itv.findChannelByNumber(num);
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return null;
	}

	public void registerListListener(ChannelsChangedListener listener) {
		try {
			if(null != itv){
			  itv.addChannelsChangedListener(listener);
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void flush() {
		try {
			if(null !=itv ){
			  itv.flush();
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
