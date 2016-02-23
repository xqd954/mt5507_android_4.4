package com.mediatek.ui.nav.util;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.content.Context;

import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcommon.ITVCommon.ChannelFilter;
import com.mediatek.tvcommon.ITVCommon.ChannelsChangedListener;
import com.mediatek.tvcommon.TVChannel;

public class NavChannel {

	private static NavChannel instance;
	private TVChannelManager mTvChannelManager;
	private TVChannelSelector mTvChannelSelector;
	private TVInputManager mTVInputManager;
	private static TVContent mTv;

	private List<String> mSourceList = new ArrayList<String>();

	private long startShiftChannelTime = 0, endShiftChannelTime = 0;
	public static final int CHANNEL_UP = 0;
	public static final int CHANNEL_DOWN = 1;
	public static final int CHANNEL_PRE = 2;

	private Context mContext;

	private static IChannelListChangedListener mClcListener;

	private NavChannel(Context context) {
		mTv = TVContent.getInstance(context);
		mTVInputManager = mTv.getInputManager();
		mTvChannelManager = mTv.getChannelManager();
		mTvChannelSelector = mTv.getChannelSelector();
		mContext = context;

		mSourceList = Arrays.asList(mTVInputManager.getInputSourceArray());
		// mChannelList = mTvChannelManager.getChannels();

		mTvChannelManager.registerListListener(mChannelListChangedListener);
		// mTvChannelSelector.addSelectorListener(mTvSeclectorListener);
		// mTVInputManager.registerSourceListener(mInputSourceListener);

	}

	public static synchronized NavChannel getInstance(Context context) {
		if (instance == null) {
			instance = new NavChannel(context);
		}
		return instance;
	}

	public List<TVChannel> iGetChannelList() {
		return mTvChannelManager.getChannels();
	}

	/**
	 * 
	 * @param channelType
	 *            ANALOGUE_TYPE,DIGITAL_TYPE,RADIO_TYPE
	 * @return channel {@link List} List<TVChannel>
	 */
	private final static int RADIO_TYPE = 1;

	public List<TVChannel> iGetChannelList(int channelType) {
		List<TVChannel> channelList = null;
		if (channelType == RADIO_TYPE) {
			channelList = mTvChannelManager.getChannels(0,
					new ChannelFilter() {

						public boolean filter(TVChannel channel) {
							if (channel.isRadio()) {
								return true;
							}
							return false;
						}
					});

		} else {
			channelList = mTvChannelManager.getChannels();// get all channels.
		}
		return channelList;
	}

	public TVChannel iGetCurrentChannel() {
		return mTvChannelSelector.getCurrentChannel();
	}

	public boolean iSelectChannel(TVChannel ch) {
		if (ch == null) {
			return false;
		}
		// if (!ch.isValid()) {
		// return false;
		// }
		if (ch.equals(iGetCurrentChannel()) && isCurrentSourceTv()) {
			return false;
		}

		iSetSourcetoTv();
		startShiftChannelTime = System.currentTimeMillis();
		mTvChannelSelector.select(ch);
		return true;
	}

	/* set input source on TV */
	public void iSetSourcetoTv() {
		if (null != iGetCurrentSouce()
				&& null != mTVInputManager
						.getTypeFromInputSource(iGetCurrentSouce())
				&& !TVInputManager.INPUT_TYPE_TV.equals(mTVInputManager
						.getTypeFromInputSource(iGetCurrentSouce()))) {
			mTVInputManager.changeInputSource(iFindTvSource());
		}
	}

	public boolean isCurrentSourceTv() {
		String mSource = iGetCurrentSouce();
		String mTypeSource = null;
		if(null != mSource){
		   mTypeSource = mTVInputManager.getTypeFromInputSource(mSource);
		}
		return (null != mTypeSource && mTypeSource
				.equals(TVInputManager.INPUT_TYPE_TV));
	}

	public String iGetCurrentSouce() {
		return mTVInputManager.getCurrInputSource();
	}

	private String iFindTvSource() {
		int tmp = 0;
    
		for (tmp = 0; tmp < mSourceList.size(); tmp++) {
			String mTypeString = mTVInputManager
					.getTypeFromInputSource(mSourceList.get(tmp));
			if (null != mTypeString
					&& mTypeString.equals(TVInputManager.INPUT_TYPE_TV)) {
				return mSourceList.get(tmp);
			}
		}
		return null;
	}

	/**
	 * Record the main output has signal
	 * 
	 * @return
	 */
	public boolean iCurrentInputSourceHasSignal() {
		return mTVInputManager.getDefaultOutput().hasSignal();
	}

	public boolean iCurrentChannelHasSignal() {
		return mTvChannelSelector.hasSignal();
	}

	/**
	 * Select channel.
	 * 
	 * @param Type
	 *            0,1,2
	 */
	public void iSetChannel(int Type) {
		iSetSourcetoTv();
		startShiftChannelTime = System.currentTimeMillis();

		switch (Type) {
		case CHANNEL_UP:
			mTvChannelSelector.channelUp();
			break;
		case CHANNEL_DOWN:
			mTvChannelSelector.channelDown();
			break;
		case CHANNEL_PRE:
			mTvChannelSelector.selectPrev();
			break;
		default:
			break;
		}

	};

	// favorite
	public boolean isSetChannelFavorite(TVChannel mTvChannel) {
		if (mTvChannel == null) {
			return false;
		} else {
			if (mTvChannel.isFavorite()) {
				mTvChannel.setFavorite(false);
				return false;
			} else {
				mTvChannel.setFavorite(true);
				return true;
			}
		}
	}

	public List<TVChannel> iGetFavoriteList() {
		return mTvChannelManager.getChannels(0,
				ChannelFilter.TVFavoriteFilter);
	}

	/**
	 * favorite channel up and down filter
	 */
	public void iSelectFavChanUp() {
		mTvChannelSelector.favoriteChannelUp();
	}

	public void iSelectFavChanDown() {
		mTvChannelSelector.favoriteChannelDown();
	}

	// call back

	public void iRegisteClcListener(IChannelListChangedListener mListener) {
		mClcListener = mListener;
	}

	private ChannelsChangedListener mChannelListChangedListener = new ChannelsChangedListener() {
		public void onChanged() {
			if (mClcListener != null) {
				mClcListener.updateChannelList();
			}
		}
	};

	public interface IChannelListChangedListener {
		public void updateChannelList();
	}

	public void isSetChannelFavorite(FavoriteListListener mListener,
			TVChannel mTvChannel) {
		if (mTvChannel != null) {
			if (mTvChannel.isFavorite()) {
				mTvChannel.setFavorite(false);
			} else {
				mTvChannel.setFavorite(true);
			}
			mTvChannelManager.flush();
			// mTvChannel.flush();

			mListener.updateFavoriteList();
		}
	}

	public interface FavoriteListListener {
		void updateFavoriteList();
	}

}
