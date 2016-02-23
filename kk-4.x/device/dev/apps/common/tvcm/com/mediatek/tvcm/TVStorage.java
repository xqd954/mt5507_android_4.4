package com.mediatek.tvcm;

import com.mediatek.common.capture.MtkCaptureCapability;
import com.mediatek.common.capture.MtkCaptureInfo;
import com.mediatek.common.capture.MtkCaptureLogo;
import com.mediatek.common.capture.MtkCaptureLogoSaveInfo;
import com.mediatek.common.capture.NotSupportException;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.BroadcastService;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Point;
import android.graphics.Rect;
import android.view.WindowManager;

public class TVStorage extends TVComponent {
	private static final String TVAP_TABLE = "tvap_storage_table";
	final private SharedPreferences pref;
	final private Editor editor;
	private static TVStorage storage;

	private TVStorage(Context context) {
		super(context);

		pref = context.getSharedPreferences(TVAP_TABLE, Context.MODE_PRIVATE);
		editor = pref.edit();
	}

	static TVStorage getInstance(Context context) {
		if (storage == null) {
			storage = new TVStorage(context);
		}
		return storage;
	}

	public SharedPreferences getShareFerences() {
		return pref;
	}

	public Editor getEditor() {
		return editor;
	}

	public void set(String k, String v) {
		getEditor().putString(k, v);
		getEditor().commit();
		TVContent.getInstance(context).flushMedia();
	}

	/**
	 * @return the value of Key
	 * @param key
	 *            the key to identify value
	 */
	public String get(String key) {
		return pref.getString(key, null);
	}

	/**
	 * @return the value of key
	 * @param key
	 *            the key to identify value
	 * @param defVal
	 *            if the Key key do not exit in share preference, return the
	 *            default value.
	 */
	public String get(String key, String defValue) {
		return pref.getString(key, defValue);
	}

	public void clean() {
		getEditor().clear();
		getEditor().commit();
		TVContent.getInstance(context).flushMedia();
	}

	public static final int capture_save_index_0 = 0;
	public static final int capture_save_index_1 = 1;
	public static final int capture_save_index_none = 0xff;
	public static final int capture_save_index_default = 0x100;

	private static final String TAG = "TVStorage";

	private byte[] ps_path = null;

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

	/**
	 * 
	 * @param source
	 *            Source : CAP_LOGO_TV, CAP_LOGO_MM or CAP_LOGO_MM_IMAGE
	 * @param rect
	 *            Can be null, for full screen
	 * @param logoId
	 *            Id
	 * @param listener
	 *            Can be null.
	 */

	private void captureCmLogo(int source, Rect rect, final int logoId,
			final LogoCaptureListener listener, byte videoPath, int skBitmap,
			int bufferWidth, int bufferHeight, int bufferPitch, int colorMode) {
		if (source < 0 || source >= captureLogos.length) {
			throw new IllegalArgumentException();
		}
		WindowManager wm = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);
		Point mPoint = new Point();
		wm.getDefaultDisplay().getRealSize(mPoint);
		int moveable_width = mPoint.x;
		int moveable_height = mPoint.y;
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
				getHandler().post(new Runnable() {
					public void run() {
						if (listener != null) {

							if (event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_DONE) {
								listener.onEvent(TVStorage.LogoCaptureListener.CAP_COMPLETE);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_ERR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_OPEN_ERROR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_ERROR) {
								listener.onEvent(TVStorage.LogoCaptureListener.CAP_FAIL);
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

		if (source != TVStorage.CAP_LOGO_TV)
			throw new IllegalArgumentException();

		WindowManager wm = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);
		Point myPoint = new Point();
		wm.getDefaultDisplay().getRealSize(myPoint);
		int moveable_width = myPoint.x;
		int moveable_height = myPoint.y;
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
				getHandler().post(new Runnable() {
					public void run() {
						if (listener != null) {

							if (event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_DONE) {
								listener.onEvent(TVStorage.LogoCaptureListener.CAP_COMPLETE);
							} else if (event == MtkCaptureLogo.CAP_EVENT_TYPE_CAP_ERR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_OPEN_ERROR
									|| event == MtkCaptureLogo.CAP_EVENT_TYPE_SAVE_ERROR) {
								listener.onEvent(TVStorage.LogoCaptureListener.CAP_FAIL);
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
