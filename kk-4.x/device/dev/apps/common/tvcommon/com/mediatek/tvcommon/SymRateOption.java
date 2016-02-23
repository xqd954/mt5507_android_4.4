/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.model.ScanParaDvbc;


public class SymRateOption extends TVOptionRange<Integer> {
	int validMask = 0;
	
	public static final int Rate_DEF = 0;
	public static final int Rate_END = 9999;
	
	@Override
	public Integer getMax() {
		return Rate_END;
	}

	@Override
	public Integer getMin() {
		return Rate_DEF;
	}

	private int val = Rate_DEF;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		validMask = ScanParaDvbc.SB_DVBC_SCAN_INFO_SYM_VALID;
		return true;
	}
	
	public int getMask(){
		return validMask;
	}

}