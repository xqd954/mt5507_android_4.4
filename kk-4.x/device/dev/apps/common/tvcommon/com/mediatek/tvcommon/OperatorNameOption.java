/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.model.ScanParaDvbc;

public class OperatorNameOption extends TVOptionRange<Integer> {
	public static final int OPERATOR_NAME_OTHERS = 0;
	public static final int OPERATOR_NAME_UPC = 1;
	public static final int OPERATOR_NAME_COMHEM = 2;
	public static final int OPERATOR_NAME_CANAL_DIGITAL = 3;
	public static final int OPERATOR_NAME_TELE2 = 4;
	public static final int OPERATOR_NAME_STOFA = 5;
	public static final int OPERATOR_NAME_YOUSEE = 6;
	public static final int OPERATOR_NAME_ZIGGO = 7;
	public static final int OPERATOR_NAME_UNITYMEDIA = 8;
	public static final int OPERATOR_NAME_NUMERICABLE = 9;

	public static final int OPERATOR_NAME_CHINA_BASE = 10;
	public static final int OPERATOR_NAME_CHINA_GUANGZHOU_SHENGWANG = 11;
	public static final int OPERATOR_NAME_CHINA_GUANGZHOU_SHIWANG = 12;
	public static final int OPERATOR_NAME_CHINA_HUNAN_GUANGDIAN = 13;
	public static final int OPERATOR_NAME_CHINA_CHANGSHA_GUOAN = 14;
	public static final int OPERATOR_NAME_CHINA_WUHAN_SHENGWANG = 15;
	public static final int OPERATOR_NAME_CHINA_WUHAN_SHIWANG = 16;
	public static final int OPERATOR_NAME_CHINA_XIAN_GUANGDIAN = 17;
	public static final int OPERATOR_NAME_CHINA_BEIJING_GEHUAYOUXIAN = 18;
	public static final int OPERATOR_NAME_CHINA_QINGDAO_GUANGDIAN = 19;
	public static final int OPERATOR_NAME_CHINA_HUBEI_HUANGSHI = 20;
	public static final int OPERATOR_NAME_CHINA_DALIAN_GUANGDIAN = 21;
	public static final int OPERATOR_NAME_CHINA_SHAOXING_GUANGDIAN = 22;
	public static final int OPERATOR_NAME_CHINA_NEIMENGGU_YOUXIAN = 23;
	public static final int OPERATOR_NAME_CHINA_EZHOU_YOUXIAN = 24;
	public static final int	OPERATOR_NAME_CHINA_SHANGHAI = 25;
	public static final int OPERATOR_NAME_CHINA_SANMING_YOUXIAN = 26;
	public static final int OPERATOR_NAME_END = 26;
	
	static int operator_name_tbl[] = {
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_OTHERS,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_UPC,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_COMHEM,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CANAL_DIGITAL,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_TELE2,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_STOFA,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_YOUSEE,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_ZIGGO,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_UNITYMEDIA,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_NUMERICABLE,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_BASE,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_GUANGZHOU_SHENGWANG,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_GUANGZHOU_SHIWANG,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_HUNAN_GUANGDIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_CHANGSHA_GUOAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_WUHAN_SHENGWANG,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_WUHAN_SHIWANG,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_XIAN_GUANGDIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_BEIJING_GEHUAYOUXIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_QINGDAO_GUANGDIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_HUBEI_HUANGSHI,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_DALIAN_GUANGDIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_SHAOXING_GUANGDIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_NEIMENGGU_YOUXIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_EZHOU_YOUXIAN,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_SHANGHAI,
			ScanParaDvbc.SB_DVBC_OPERATOR_NAME_CHINA_SANMING_YOUXIAN};

	@Override
	public Integer getMax() {
		return OPERATOR_NAME_END;
	}

	@Override
	public Integer getMin() {
		return OPERATOR_NAME_OTHERS;
	}

	private int val = OPERATOR_NAME_OTHERS;

	@Override
	public Integer get() {
		return val;
	}

	@Override
	public boolean set(Integer val) {
		this.val = val.intValue();
		return true;
	}

	int getRawOperatorName() {
		return operator_name_tbl[val];
	}

	int getRawEnable() {
		// This is used for arg0, :)
		if (val == OPERATOR_NAME_OTHERS)
			return 0;
		else
			return 1;
	}
}