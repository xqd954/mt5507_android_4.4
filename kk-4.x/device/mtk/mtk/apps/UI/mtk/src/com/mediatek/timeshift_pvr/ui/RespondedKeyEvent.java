package com.mediatek.timeshift_pvr.ui;



public interface RespondedKeyEvent {

	public void setValue(int value);

	public int getValue();

	public void onKeyLeft();

	public void onKeyRight();

	public void onKeyEnter();

	public void showValue(int value);
}
