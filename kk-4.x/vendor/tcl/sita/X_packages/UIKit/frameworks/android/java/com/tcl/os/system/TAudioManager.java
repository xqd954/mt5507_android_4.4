package com.tcl.os.system;

import android.content.Context;
import android.media.IAudioService;
import android.os.IBinder;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.util.Log;

/**
 * AudioManager provides access to volume and ringer mode control.
 * <p>
 * Use <code>Context.getSystemService(Context.AUDIO_SERVICE)</code> to get
 * an instance of this class.
 */
public class TAudioManager {

	private static String TAG = "TAudioManager";
    /**
     * hide volume panel.
     *
     */
    public void hideVolumePanel() {
		IBinder b = ServiceManager.getService(Context.AUDIO_SERVICE);
        IAudioService service = IAudioService.Stub.asInterface(b);
        try {
            service.hideVolumePanel();
        } catch (Exception e) {
            Log.e(TAG, "hideVolumePanel:", e);
        }
    }

}
