/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.model.ScanParaDvbc;


public class NetworkIDOption extends TVOptionRange<Integer> {
	int validMask = 0;
	
	public static final int Network_ID_DEF = 0;
	public static final int Network_ID_END = 0;

	@Override
	public Integer getMax() {
		return Network_ID_END;
	}

	@Override
	public Integer getMin() {
		return Network_ID_DEF;
	}

	private int val = Network_ID_DEF;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		validMask = ScanParaDvbc.SB_DVBC_SCAN_INFO_NW_ID_VALID;
		return true;
	}
	
	public int getMask(){
		return validMask;
	}
}