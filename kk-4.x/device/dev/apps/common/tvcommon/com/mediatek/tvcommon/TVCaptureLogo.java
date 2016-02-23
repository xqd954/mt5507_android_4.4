package com.mediatek.tvcommon;

import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.view.WindowManager;

import com.mediatek.common.capture.MtkCaptureCapability;
import com.mediatek.common.capture.MtkCaptureInfo;
import com.mediatek.common.capture.MtkCaptureLogo;
import com.mediatek.common.capture.MtkCaptureLogoSaveInfo;
import com.mediatek.common.capture.NotSupportException;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.BroadcastService;

public class TVCaptureLogo {
	private Handler mHandler;
	static Context mContext;
	static private TVCaptureLogo instance;
	private byte[] ps_path = null;

	public static final int capture_save_index_0 = 0;
	public static final int capture_save_index_1 = 1;
	public static final int capture_save_index_none = 0xff;
	public static final int capture_save_index_default = 0x100;

	public interface LogoCaptureListener {
		public static final int CAP_COMPLETE = 0;
		public static final int CAP_FAIL = 1;
		public static final int CAP_CANCLE = 2;

		public void onEvent(int event);
	}

	public static int CAP_LOGO_TV = MtkCaptureLogo.CAPTURE_SRC_TYPE_TV_VIDEO;
	public static int CAP_LOGO_MM_VIDEO = MtkCaptureLogo.CAPTURE_SRC_TYPE_MM_VIDEO;
	public static int CAP_LOGO_MM_IMAGE = MtkCaptureLogo.CAPTURE_SRC_TYPE_MM_IMAGE_ANDROID;

	public static int CAP_LOGO_MAX = CAP_LOGO_MM_IMAGE + 1;

	MtkCaptureLogo[] captureLogos = new MtkCaptureLogo[CAP_LOGO_MAX];

	public TVCaptureLogo(Context context) {
		mContext = context;
		mHandler = new Handler(Looper.getMainLooper());
	}

	static TVCaptureLogo getInstance(Context context) {
		if (instance == null) {
			instance = new TVCaptureLogo(context);
		}

		return instance;
	}

	private void captureCmLogo(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener, byte videoPath, int skBitmap,
			int bufferWidth, int bufferHeight, int bufferPitch, int colorMode) {
		if (source < 0 || source >= captureLogos.length) {
			throw new IllegalArgumentException();
		}
		WindowManager wm = (WindowManager) mContext
				.getSystemService(Context.WINDOW_SERVICE);
		int moveable_width = wm.getDefaultDisplay().getWidth();
		int moveable_height = wm.getDefaultDisplay().getHeight();
		if (rect == null) {
			rect = new Rect(0, 0, moveable_width, moveable_height);
		}
		MtkCaptureInfo mtkCaptureInfo = new MtkCaptureInfo(
				MtkCaptureLogo.CAP_FMT_TYPE_MPEG, 80, 262144,
				MtkCaptureLogo.CAP_OUT_RES_TYPE_HD, rect, moveable_width,
				moveable_height, videoPath, skBitmap, bufferWidth,
				bufferHeight, bufferPitch, colorMode);

		if (captureLogos[source] == null) {
			captureLogos[source] = new MtkCaptureLogo(source);
		}
		final MtkCaptureLogo logo = captureLogos[source];

		logo.setOnEventListener(new MtkCaptureLogo.OnEventListener() {
			public boolean onEvent(MtkCaptureLogo cap, final int event) {
				mHandler.post(new Runnable() {
					public void run() {
						if (listener != null) {

							if (event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_DONE) {
								listener.onEvent(LogoCaptureListener.CAP_COMPLETE);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_ERR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_OPEN_ERROR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_ERROR) {
								listener.onEvent(LogoCaptureListener.CAP_FAIL);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_DONE) {
								MtkCaptureLogoSaveInfo mtkCaptureSaveinfo = new MtkCaptureLogoSaveInfo(
										MtkCaptureLogo.CAP_DEVICE_TYPE_INTERNAL,
										logoId, ps_path);
								logo.SaveAsBootLogo(mtkCaptureSaveinfo);

							}
						}
					}
				});
				return false;
			}
		});
		try {
			logo.Capture(mtkCaptureInfo);
		} catch (NotSupportException e) {
			e.printStackTrace();
		}
	}

	/**
	 * 
	 * @param source
	 * @param rect
	 * @param logoId
	 * @param listener
	 */
	public void captureLogo(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener) {

		captureCmLogo(source, rect, logoId, listener, (byte) 0, 0, 0, 0, 0, 0);
	}

	/**
	 * for picture.
	 * 
	 * @param source
	 * @param rect
	 * @param logoId
	 * @param listener
	 * @param skBitmap
	 * @param bufferWidth
	 * @param bufferHeight
	 * @param bufferPitch
	 * @param colorMode
	 */
	public void captureLogo(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener, int skBitmap, int bufferWidth,
			int bufferHeight, int bufferPitch, int colorMode) {

		captureCmLogo(source, rect, logoId, listener, (byte) 0, skBitmap,
				bufferWidth, bufferHeight, bufferPitch, colorMode);
	}

	public void cancleCaptureLogo(int source) {
		if (source < 0 || source >= captureLogos.length) {
			throw new IllegalArgumentException();
		}
		if (captureLogos[source] != null) {
			captureLogos[source].Stop();

		}
	}

	/**
	 * release resouce when finished
	 * 
	 * @param source
	 */
	public void finishCaptureLogo(int source) {
		if (source < 0 || source >= captureLogos.length) {
			throw new IllegalArgumentException();
		}
		// if (captureLogos[source] != null) {
		// }
	}

	public void setBootLogo(int source, int id) {
		if (source < 0 || source >= captureLogos.length) {
			throw new IllegalArgumentException();
		}
		if (captureLogos[source] == null) {
			captureLogos[source] = new MtkCaptureLogo(source);
		}
		MtkCaptureLogo logo = captureLogos[source];
		MtkCaptureCapability capCapability = logo.QueryCapability();
		if (id == capture_save_index_default) {
			if (capCapability.b_default_exist) {
				logo.SelectAsBootLogo(MtkCaptureLogo.CAP_DEVICE_TYPE_DEFAULT,
						id);
			} else {
				logo.SelectAsBootLogo(MtkCaptureLogo.CAP_DEVICE_TYPE_INTERNAL,
						capture_save_index_none);
			}
		} else {
			// fix the problem capture logo not correct when clear eeprom
			logo.SelectAsBootLogo(MtkCaptureLogo.CAP_DEVICE_TYPE_INTERNAL, id);
		}

	}

	/* Capture TV Screen and save to "path" */
	public void captureTVPic(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener, String path) {
		captureCmTVPic(source, rect, logoId, listener, (byte) 0, 0, 0, 0, 0, 0,
				path);
	}

	private void captureCmTVPic(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener, byte videoPath, int skBitmap,
			int bufferWidth, int bufferHeight, int bufferPitch, int colorMode,
			final String path) {

		if (source != CAP_LOGO_TV)
			throw new IllegalArgumentException();

		WindowManager wm = (WindowManager) mContext
				.getSystemService(Context.WINDOW_SERVICE);
		int moveable_width = wm.getDefaultDisplay().getWidth();
		int moveable_height = wm.getDefaultDisplay().getHeight();
		if (rect == null) {
			rect = new Rect(0, 0, moveable_width, moveable_height);
		}
		MtkCaptureInfo mtkCaptureInfo = new MtkCaptureInfo(
				MtkCaptureLogo.CAP_FMT_TYPE_JPEG, 80, 262144,
				MtkCaptureLogo.CAP_OUT_RES_TYPE_HD, rect, moveable_width,
				moveable_height, videoPath, skBitmap, bufferWidth,
				bufferHeight, bufferPitch, colorMode);

		if (captureLogos[source] == null) {
			captureLogos[source] = new MtkCaptureLogo(source);
		}
		final MtkCaptureLogo logo = captureLogos[source];

		logo.setOnEventListener(new MtkCaptureLogo.OnEventListener() {
			public boolean onEvent(MtkCaptureLogo cap, final int event) {
				mHandler.post(new Runnable() {
					public void run() {
						if (listener != null) {

							if (event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_DONE) {
								listener.onEvent(LogoCaptureListener.CAP_COMPLETE);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_ERR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_OPEN_ERROR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_ERROR) {
								listener.onEvent(LogoCaptureListener.CAP_FAIL);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_DONE) {

								logo.Save(path);

							}
						}
					}
				});
				return false;
			}
		});

		try {
			logo.Capture(mtkCaptureInfo);
		} catch (NotSupportException e) {
			e.printStackTrace();
		}

	}

	/**
	 * is capture logo can do or not.
	 * 
	 * @return boolean
	 */
	public boolean isCaptureLogo() {
		TVManager tvMgr = TVManager.getInstance(mContext);
		if (tvMgr != null) {
			BroadcastService brdSrv = (BroadcastService) tvMgr
					.getService(BroadcastService.BrdcstServiceName);
			if (brdSrv != null) {
				try {
					return brdSrv.isCaptureLogo();
				} catch (TVMException e) {
					e.printStackTrace();
				}
			}
		}
		return false;
	}

}
