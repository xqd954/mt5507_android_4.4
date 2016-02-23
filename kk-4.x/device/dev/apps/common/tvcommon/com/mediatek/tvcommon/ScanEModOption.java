/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.model.ScanParaDvbc;

public class ScanEModOption extends TVOptionRange<Integer> {
	int validMask = 0;
	
	public static final int MOD_UNKNOWN = 0;
	public static final int MOD_QAM_16 = 1;
	public static final int MOD_QAM_32 = 2;
	public static final int MOD_QAM_64 = 3;
	public static final int MOD_QAM_128 = 4;
	public static final int MOD_QAM_256 = 5;
	public static final int MOD_END = 5;

	static int scan_emod_tbl[] = { ScanParaDvbc.MOD_UNKNOWN,
			ScanParaDvbc.MOD_QAM_16, ScanParaDvbc.MOD_QAM_32,
			ScanParaDvbc.MOD_QAM_64, ScanParaDvbc.MOD_QAM_128,
			ScanParaDvbc.MOD_QAM_256 };

	@Override
	public Integer getMax() {
		return MOD_END;
	}

	@Override
	public Integer getMin() {
		return MOD_UNKNOWN;
	}

	private int val = MOD_UNKNOWN;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		validMask = ScanParaDvbc.SB_DVBC_SCAN_INFO_MOD_VALID;
		return true;
	}
	
	public int getMask(){
		return validMask;
	}
	
	int getRawScaneMod() {
		return scan_emod_tbl[val];
	}

	int getRawEnable() {
		// This is used for arg0, :)
		if (val == MOD_UNKNOWN)
			return 0;
		else
			return 1;
	}
}