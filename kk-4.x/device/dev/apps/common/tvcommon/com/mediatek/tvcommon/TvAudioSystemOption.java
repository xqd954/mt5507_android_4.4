/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.common.ChannelCommon;

public class TvAudioSystemOption extends TVOptionRange<Integer> {
	public final static int TV_AUTO = 0;
	public final static int TV_BG = 1;
	public final static int TV_A2_BG = 2;
	public final static int TV_I = 3;
	public final static int TV_DK = 4;
	public final static int TV_A2_DK = 5;
	public final static int TV_A2_DK1 = 6;
	public final static int TV_A2_DK2 = 7;
	public final static int TV_M = 8;
	public final static int TV_END = 8;

	static int tv_sys_tbl[] = {
			ChannelCommon.TV_SYS_UNKNOWN,
			ChannelCommon.TV_SYS_B | ChannelCommon.TV_SYS_G,
			ChannelCommon.TV_SYS_B | ChannelCommon.TV_SYS_G,
			ChannelCommon.TV_SYS_I,
			ChannelCommon.TV_SYS_D | ChannelCommon.TV_SYS_K,
			ChannelCommon.TV_SYS_D | ChannelCommon.TV_SYS_K,
			ChannelCommon.TV_SYS_D | ChannelCommon.TV_SYS_K,
			ChannelCommon.TV_SYS_D | ChannelCommon.TV_SYS_K,
			ChannelCommon.TV_SYS_M,
			ChannelCommon.TV_SYS_M | ChannelCommon.TV_SYS_N};

	static int aud_sys_tbl[] = {
			ChannelCommon.AUDIO_SYS_UNKNOWN,
			ChannelCommon.AUDIO_SYS_FM_MONO | ChannelCommon.AUDIO_SYS_NICAM,
			ChannelCommon.AUDIO_SYS_FM_A2,
			ChannelCommon.AUDIO_SYS_FM_MONO | ChannelCommon.AUDIO_SYS_NICAM,
			ChannelCommon.AUDIO_SYS_FM_MONO | ChannelCommon.AUDIO_SYS_NICAM,
			ChannelCommon.AUDIO_SYS_FM_A2,
			ChannelCommon.AUDIO_SYS_FM_A2_DK1,
			ChannelCommon.AUDIO_SYS_FM_A2_DK2,
			ChannelCommon.AUDIO_SYS_BTSC,
			ChannelCommon.AUDIO_SYS_BTSC};

	@Override
	public Integer getMax() {
		return TV_END;
	}

	@Override
	public Integer getMin() {
		return TV_AUTO;
	}

	private int val = TV_AUTO;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		return true;
	}

	public int getRawTvSystem() {
		return tv_sys_tbl[val];
	}

	public int getRawAudSystem() {
		return aud_sys_tbl[val];
	}

	int getRawEnable() {
		// This is used for arg0, :)
		if (val == TV_AUTO)
			return 0;
		else
			return 1;
	}
}