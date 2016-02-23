package com.mediatek.timeshift_pvr.manager;

public interface IManagerInterface<T> {

	boolean setState(T state);

	void onResume();

	void onPause();

	void onStop();

	void init();

	void clearErrorState();
	
	boolean requestPermission(int keyCode);
	
	boolean pvrIsRunning();

	boolean tshiftIsRunning();

	boolean pvrIsPlaying();

	boolean diskIsOperating();
}
