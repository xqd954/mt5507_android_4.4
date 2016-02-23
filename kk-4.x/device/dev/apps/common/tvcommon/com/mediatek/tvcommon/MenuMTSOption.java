package com.mediatek.tvcommon;

import com.mediatek.tv.common.ConfigType;

public class MenuMTSOption extends TVOptionRange<Integer> {
	static final String name = "MENUMTSOPTION";
	
	TVOptionRange<Integer> optmts;

	@SuppressWarnings("unchecked")
	MenuMTSOption(TVOption<?> tvOption) {
		this.optmts = (TVOptionRange<Integer>) tvOption;
	}

	Integer[] mts_option = { ConfigType.AUD_MTS_MONO,
			ConfigType.AUD_MTS_NICAM_STEREO,
			ConfigType.AUD_MTS_NICAM_DUAL1, ConfigType.AUD_MTS_NICAM_DUAL2 };

	@Override
	public Integer getMax() {
		return mts_option.length - 1;
	}

	@Override
	public Integer getMin() {
		return 0;
	}

	@Override
	public Integer get() {
		for (int i = 0; i < mts_option.length; i++) {
			if (optmts.get().equals(mts_option[i])) {
				return i;
			}
		}
		return 1;
	}

	@Override
	public boolean canSet(Integer val) {
		if (val < 0 || val > mts_option.length - 1) {
			return false;
		}
		return true;
	}

	@Override
	public boolean set(Integer val) {
		optmts.set(mts_option[val]);
		return true;
	}
	
}
