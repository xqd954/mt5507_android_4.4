package com.mediatek.tvcommon;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.Set;

import android.content.Context;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.ConfigValue;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.ConfigService;

public class TVConfigurer {

	private Hashtable<String, TVOption<?>> optionTable = new Hashtable<String, TVOption<?>>();

	public static final int CFG_GRP_DTV = ConfigType.CONFIG_VALUE_GROUP_DTV;
	public static final int CFG_GRP_ATV = ConfigType.CONFIG_VALUE_GROUP_ATV;
	public static final int CFG_GRP_AV = ConfigType.CONFIG_VALUE_GROUP_AV;
	public static final int CFG_GRP_COMPONENT = ConfigType.CONFIG_VALUE_GROUP_COMPONENT;
	public static final int CFG_GRP_HDMI = ConfigType.CONFIG_VALUE_GROUP_HDMI;
	public static final int CFG_GRP_DVI = ConfigType.CONFIG_VALUE_GROUP_DVI;
	public static final int CFG_GRP_VGA = ConfigType.CONFIG_VALUE_GROUP_VGA;
	// This is a special group for : When we leave TV... ...WTF....
	public static final int CFG_GRP_LEAVE = ConfigType.CONFIG_VALUE_GROUP_MMP;

    protected static final String[] CFG_GRP_NAMES = {
            "atv", "dtv", "av", "component", "hdmi", "dvi", "vga", "mmp"
    };
	private ConfigService rawSrv;

	private TVConfigurer(Context context) {
		rawSrv = (ConfigService) TVManager.getInstance(
				context.getApplicationContext()).getService(
				ConfigService.ConfigServiceName);
		init();
	}

	private void init() {
		optionTable.put(AllNROption.name, new AllNROption(
				getOption(IntegerOption.CFG_NR),
				getOption(IntegerOption.CFG_3DNR)));
		optionTable.put(MJCModeOption.name, new MJCModeOption(
				getOption(IntegerOption.CFG_MJC_MODE),
				getOption(IntegerOption.CFG_MJC_FCT)));
		optionTable.put(MenuMTSOption.name, new MenuMTSOption(
				getOption(IntegerOption.CFG_AUD_MTS)));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX,
				new IntegerArrayRangeProxyOption(this, FACTORY_TV_DBOX, 0, 0,
						1, 0));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_CVBS_DELAY,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_CVBS_DELAY, 0, 0, 99999, 1));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_CVBS_SWING,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_CVBS_SWING, 0, 0, 99999, 3));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_SNOW_GEN,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_SNOW_GEN, 0, 0, 1, 4));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_TVD_CHECK,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_TVD_CHECK, 0, 0, 99999, 2));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_TVD_GAIN,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_TVD_GAIN, 0, 0, 99999, 5));
		optionTable.put(TVConfigurer.FACTORY_TV_DBOX_VLOCK_RATIO,
				new IntegerArrayRangeProxyOption(this,
						FACTORY_TV_DBOX_VLOCK_RATIO, 0, 0, 99999, 6));
	}

	private static TVConfigurer configurer = null;

	static public TVConfigurer getInstance(Context context) {
		if (configurer == null) {
			configurer = new TVConfigurer(context);
		}
		return configurer;
	}

	ConfigService getRawSrv() {
		return rawSrv;
	}

	void setGroup(int idx) {
		ConfigValue configValue = new ConfigValue();
		configValue.setIntValue(idx);
		try {
			if(rawSrv != null)
				rawSrv.setCfg(ConfigType.CFG_VALUE_GROUP, configValue);
			// rawSrv.updateCfg(ConfigType.CFG_VALUE_GROUP);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	int getGroup() {
		ConfigValue configValue = new ConfigValue();
		try {
			configValue = rawSrv.getCfg(ConfigType.CFG_VALUE_GROUP);
			return configValue.getIntValue();
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return CFG_GRP_ATV;
	}

	/**
	 * Reset to User settings
	 */
	public void resetUser() {
		try {
			if(null != rawSrv){
			rawSrv.resetCfgGroup(ConfigType.RESET_USER);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Reset to Factory settings
	 */
	public void resetFactory() {
		try {
			if(null != rawSrv){
			rawSrv.resetCfgGroup(ConfigType.RESET_FACTORY);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public final synchronized TVOption<?> getOption(IntegerOption option) {
		String name = option.getName();
		if (optionTable.contains(name) == false) {
			IntegerRangeProxyOption intOption = new IntegerRangeProxyOption(
					this, name, option.getDefaultVal(), option.getMin(),
					option.getMax());
			optionTable.put(name, intOption);
			return intOption;
		}
		return optionTable.get(name);
	}

	/**
	 * Get TVOption for specified name
	 * 
	 * @param name
	 * @return
	 */
	public final synchronized TVOption<?> getOption(String name) {
		if (optionTable.containsKey(name) == false) {
			IntegerOption option = IntegerOption.contain(name);
			if (option != null) {
				IntegerRangeProxyOption intOption = new IntegerRangeProxyOption(
						this, name, option.getDefaultVal(), option.getMin(),
						option.getMax());
				optionTable.put(name, intOption);
			}
		}
		return optionTable.get(name);
	}

	/**
	 * Get all options iterator.
	 * 
	 * @return
	 */
	public synchronized Iterator<String> getAllOptions() {
		Set<String> names = optionTable.keySet();
		Iterator<String> itr = names.iterator();
		return itr;
	}

	static class IntegerRangeProxyOption extends TVOptionRange<Integer> {
		private ConfigService rawSrv = null;
		private int min;
		private int max;
		private int defaultValue;
		private String name;
		private int dummyVal;

		boolean hotMinMax; // If min and max is not constant

		public Integer getMax() {
			if (hotMinMax) {
				ConfigValue rawMinMax = new ConfigValue();
				try {
					rawSrv.getCfgMinMax(name, rawMinMax);
					// TODO: FIX HERE
					return 100;
				} catch (TVMException e) {
					e.printStackTrace();
				}

			} else {
				return new Integer(max);
			}
			// ....
			return 100;
		}

		public Integer getMin() {
			if (hotMinMax) {
				ConfigValue rawMinMax = new ConfigValue();
				try {
					rawSrv.getCfgMinMax(name, rawMinMax);
					// TODO: FIX HERE
					return 0;
				} catch (TVMException e) {
					e.printStackTrace();
				}

			} else {
				return new Integer(min);
			}
			// ...
			return 0;
		}

		public Integer getDefault() {
			return new Integer(defaultValue);
		}

		public Integer get() {
			ConfigValue rawVal;
			// For dummy
			if (rawSrv == null) {
				return new Integer(dummyVal);
			}
			try {
				rawVal = rawSrv.getCfg(name);

				return new Integer(rawVal.getIntValue());
			} catch (TVMException e) {
				e.printStackTrace();
			}
			// TODO: Throw exception...
			return null;
		}

		public boolean set(Integer val) {
			// For dummy
			if (rawSrv == null) {
				System.out.println(" CONFIG:" + name + " val:" + val);
				dummyVal = val.intValue();
				return true;
			}
			ConfigValue rawVal = new ConfigValue();
			rawVal.setIntValue(val.intValue());
			try {
				rawSrv.setCfg(name, rawVal);
				rawSrv.updateCfg(name);
				return true;
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// TODO: Throw exception?...
			return false;
		}

		public IntegerRangeProxyOption(TVConfigurer cfger, String cfgName,
				int defaultValue, int min, int max) {
			dummyVal = defaultValue;
			this.min = min;
			this.max = max;
			this.hotMinMax = false;
			this.name = cfgName;
			this.rawSrv = cfger.getRawSrv();
			this.defaultValue = defaultValue;

		}

		public IntegerRangeProxyOption(TVConfigurer cfger, String cfgName,
				int defaultValue) {
			dummyVal = defaultValue;
			this.hotMinMax = true;
			this.name = cfgName;
			this.rawSrv = cfger.getRawSrv();
			this.defaultValue = defaultValue;

		}

	}

	public String getModelname() {
		String modeName = null;
		try {
			ConfigValue val = rawSrv.getCfg(ConfigType.VINFO_MODEL_NAME);
			if (val != null) {
				modeName = val.getStringValue();
			} else {
				modeName = "";
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return modeName;
	}

	public String getSerialNum() {
		String serialNum = null;
		try {
			ConfigValue val = rawSrv.getCfg(ConfigType.VINFO_SERIAL_NUM);
			if (val != null) {
				serialNum = val.getStringValue();
			} else {
				serialNum = "";
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return serialNum;
	}

	public String getVersion() {
		String version = null;
		try {
			ConfigValue val = rawSrv.getCfg(ConfigType.VINFO_VERSION);
			if (val != null) {
				version = val.getStringValue();
			} else {
				version = "";
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return version;
	}

	public int[] getSpectrum() {
		ConfigValue configvalueIn = new ConfigValue();
		ConfigValue configvalueOut = new ConfigValue();
		int[] InValue = new int[1];

		InValue[0] = ConfigType.D_INTERFACE_OP_GET_NORMAL;
		configvalueIn.setIntArrayValue(InValue);
		try {
			configvalueOut = rawSrv.getDrvCfg(ConfigType.D_CFG_AUD_SPECTRUM,
					configvalueIn);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return configvalueOut.getIntArrayValue();
	}

	public static final int CFG_3D_MODE = 0;
	public static final int CFG_LR_SWITCH = 1;
	public static final int CFG_DEPTH_FIELD = 2;
	public static final int CFG_PROTRUDE = 3;
	public static final int CFG_DISTANCE_TV = 4;
	public static final int CFG_3D_2D = 5;
	public static final int CFG_OSD_DEPTH = 6;
	public static final int CFG_FPR = 7;
	public static final int CFG_3D_NAV = 8;
	public static final int CFG_IMG_SAFETY = 9;

	private static final int NUM_3DMODE = 11;

	public boolean checkType(int optionName) throws TVMException {
		if (optionName < CFG_3D_MODE || optionName > CFG_IMG_SAFETY)
			return false;

		ConfigValue cfgValueHigh = rawSrv
				.getCfg(ConfigType.CFG_VIDEO_3D_CTRL_CAP_HIGH16);
		ConfigValue cfgValueLow = rawSrv
				.getCfg(ConfigType.CFG_VIDEO_3D_CTRL_CAP_LOW16);
		int cfgValue = ((cfgValueHigh.getIntValue()) << 16)
				+ (cfgValueLow.getIntValue() & 0xFFFF);

		if (((cfgValue >> (optionName * 2)) & 0x3) == 3)
			return true;
		else
			return false;
	}

	protected boolean[] get3DModeState() throws TVMException {
		boolean[] optionFeature = new boolean[NUM_3DMODE];
		ConfigValue featureValueHigh = rawSrv
				.getCfg(ConfigType.CFG_VIDEO_3D_MODE_CAP_HIGH16);// get from dtv
		ConfigValue featureValueLow = rawSrv
				.getCfg(ConfigType.CFG_VIDEO_3D_MODE_CAP_LOW16);
		int featureValue = ((featureValueHigh.getIntValue()) << 16)
				+ (featureValueLow.getIntValue() & 0xFFFF);

		for (int i = 0; i < NUM_3DMODE; i++) {
			if (((featureValue >> (i * 2)) & 0x3) == 3)
				optionFeature[i] = true;
			else
				optionFeature[i] = false;
		}

		return optionFeature;
	}

	@SuppressWarnings("unchecked")
	public int check3DModeSubTpye(boolean left) throws TVMException {
		TVOptionRange<Integer> opt = (TVOptionRange<Integer>) getOption(ConfigType.CFG_3D_MODE);
		int curOpt = 0;
		if (opt != null) {
			curOpt = opt.get();
		}
		boolean state[] = get3DModeState();

		if (left) {
			int preOpt = curOpt - 1;
			while (preOpt >= 0) {
				if (state[preOpt] == true)
					return preOpt;
				else
					preOpt--;
			}
			preOpt = NUM_3DMODE - 1;
			while (preOpt >= curOpt) {
				if (state[preOpt] == true)
					return preOpt;
				else
					preOpt--;
			}
		} else {
			int nextOpt = curOpt + 1;
			while (nextOpt < NUM_3DMODE) {
				if (state[nextOpt] == true)
					return nextOpt;
				else
					nextOpt++;
			}
			nextOpt = 0;
			while (nextOpt <= curOpt) {
				if (state[nextOpt] == true)
					return nextOpt;
				else
					nextOpt++;
			}
		}
		return curOpt;
	}

	public static final String ALL_NR_OPTION = AllNROption.name;
	public static final String MENU_MTS_OPTION = MenuMTSOption.name;
	public static final String MJC_MODE_OPTION = MJCModeOption.name;

	public static final String FACTORY_TV_DBOX = "factory_tv_dbox";
	public static final String FACTORY_TV_DBOX_CVBS_DELAY = "factory_tv_dbox_cvbs_delay";
	public static final String FACTORY_TV_DBOX_TVD_CHECK = "factory_tv_dbox_tvd_check";
	public static final String FACTORY_TV_DBOX_CVBS_SWING = "factory_tv_dbox_cvbs_swing";
	public static final String FACTORY_TV_DBOX_SNOW_GEN = "factory_tv_dbox_snow_gen";
	public static final String FACTORY_TV_DBOX_TVD_GAIN = "factory_tv_dbox_tvd_gain";
	public static final String FACTORY_TV_DBOX_VLOCK_RATIO = "factory_tv_dbox_vlock_ratio";

	// add for Descrambler box ; please don't use for other thing!!!!
	private class IntegerArrayRangeProxyOption extends TVOptionRange<Integer> {

		private ConfigService rawSrv = null;
		private int min;
		private int max;
		private String name;
		private int dummyVal;
		private int indexC;

		public Integer getMax() {
			return new Integer(max);
		}

		public Integer getMin() {
			return new Integer(min);
		}

		public Integer get() {
			ConfigValue rawVal;
			// For dummy
			if (rawSrv == null) {
				return dummyVal;
			}
			try {
				rawVal = rawSrv.getCfg(ConfigType.CFG_DESCRAMBLER);
				if(rawVal != null){
					if(rawVal.getIntArrayValue() != null && rawVal.getIntArrayValue().length > indexC)
						return new Integer(rawVal.getIntArrayValue()[indexC]);
				}
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// TODO: Throw exception...
			return null;
		}

		public boolean set(Integer val) {
			if (rawSrv == null) {
				System.out.println(" CONFIG: ConfigType.CFG_DESCRAMBLER"
						+ " val:" + val);
				dummyVal = val.intValue();
				return true;
			}

			try {
				ConfigValue rawVal = rawSrv.getCfg(ConfigType.CFG_DESCRAMBLER);
				int[] arr = rawVal.getIntArrayValue();
				if (arr != null) {
					arr[indexC] = val.intValue();
					rawVal.setIntArrayValue(arr);
				}
				rawSrv.setCfg(ConfigType.CFG_DESCRAMBLER, rawVal);
				rawSrv.updateCfg(ConfigType.CFG_DESCRAMBLER);

				return true;
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// TODO: Throw exception?...
			return false;
		}

		public IntegerArrayRangeProxyOption(TVConfigurer cfger, String cfgName,
				int defaultValue, int min, int max, int index) {
			dummyVal = defaultValue;
			this.min = min;
			this.max = max;
			this.name = cfgName;
			this.rawSrv = cfger.getRawSrv();
			this.indexC = index;
		}
	}
}
