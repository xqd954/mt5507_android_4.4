/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.common.ChannelCommon;

public class ColorSystemOption extends TVOptionRange<Integer> {
	public static final int COLOR_SYS_AUTO = 0;
	public static final int COLOR_SYS_PAL = 1;
	public static final int COLOR_SYS_SECAM = 2;
	public static final int COLOR_SYS_NTSC = 3;
	public static final int COLOR_SYS_END = COLOR_SYS_NTSC;

	static int color_sys_tbl[] = {ChannelCommon.COLOR_SYS_UNKNOWN,
			ChannelCommon.COLOR_SYS_PAL, ChannelCommon.COLOR_SYS_SECAM,
			ChannelCommon.COLOR_SYS_NTSC};

	@Override
	public Integer getMax() {
		return COLOR_SYS_END;
	}

	@Override
	public Integer getMin() {
		return COLOR_SYS_AUTO;
	}

	private int val = COLOR_SYS_AUTO;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		return true;
	}

	public int getRawColorSystem() {
		return color_sys_tbl[val];
	}

	int getRawEnable() {
		// This is used for arg0, :)
		if (val == COLOR_SYS_AUTO)
			return 0;
		else
			return 1;
	}
}