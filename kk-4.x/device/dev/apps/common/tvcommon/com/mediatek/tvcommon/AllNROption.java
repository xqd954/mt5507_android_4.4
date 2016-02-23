package com.mediatek.tvcommon;

public class AllNROption extends TVOptionRange<Integer> {

	static final String name = "ALLNROPTION";
	
	TVOptionRange<Integer> opt2d;
	TVOptionRange<Integer> opt3d;
		
	@SuppressWarnings("unchecked")
	AllNROption(TVOption<?> tvOption, TVOption<?> tvOption2) {
		this.opt2d = (TVOptionRange<Integer>) tvOption;
		this.opt3d = (TVOptionRange<Integer>) tvOption2;
	}

	@Override
	public Integer getMax() {
		return opt2d.getMax();
	}

	@Override
	public Integer getMin() {
		return opt2d.getMin();
	}

	@Override
	public Integer get() {
		return opt2d.get();
	}

	@Override
	public boolean set(Integer val) {
		opt2d.set(val);
		opt3d.set(val);
		return true;
	}

}
