package com.mediatek.tvcommon;

import com.mediatek.tv.common.ConfigType;

public class MJCModeOption extends TVOptionRange<Integer> {
	static final String name = "MJCMODEOPTION";
	
	TVOptionRange<Integer> optmjcmd;
	TVOptionRange<Integer> optmjcfc;

	@SuppressWarnings("unchecked")
	MJCModeOption(TVOption<?> tvOption, TVOption<?> tvOption2) {
		this.optmjcmd = (TVOptionRange<Integer>) tvOption;
		this.optmjcfc = (TVOptionRange<Integer>) tvOption2;
	}

	Integer[] mjc_mode_option = { ConfigType.CFG_MJC_MODE_0,
			ConfigType.CFG_MJC_MODE_1, ConfigType.CFG_MJC_MODE_NONE };

	@Override
	public Integer getMax() {
		return mjc_mode_option.length - 1;
	}

	@Override
	public Integer getMin() {
		return 0;
	}

	@Override
	public Integer get() {
		for (int i = 0; i < mjc_mode_option.length; i++) {
			if (optmjcmd.get().equals(mjc_mode_option[i])) {
				return i;
			}
		}
		return 0;
	}

	@Override
	public boolean set(Integer val) {
		optmjcmd.set(mjc_mode_option[val]);
		optmjcfc.set(ConfigType.CFG_MJC_ON);
		return true;
	}

}
