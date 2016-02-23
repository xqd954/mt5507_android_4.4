// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.media;

import android.content.Context;
/// M: for keep background light on
import android.content.pm.PackageManager;
import android.Manifest.permission;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.AsyncTask;
/// M: for keep background light on
import android.os.PowerManager;
import android.text.TextUtils;
import android.util.Base64;
import android.util.Base64InputStream;
import android.util.Log;
import android.view.Surface;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

/// M: Add mediatek xlog import
//import com.mediatek.xlog.Xlog;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;

/**
* A wrapper around android.media.MediaPlayer that allows the native code to use it.
* See media/base/android/media_player_bridge.cc for the corresponding native code.
*/
@JNINamespace("media")
public class MediaPlayerBridge {

    private static final String TAG = "MediaPlayerBridge";

    // Local player to forward this to. We don't initialize it here since the subclass might not
    // want it.
    private LoadDataUriTask mLoadDataUriTask;
    private MediaPlayer mPlayer;
    private long mNativeMediaPlayerBridge;

    @CalledByNative
    private static MediaPlayerBridge create(long  nativeMediaPlayerBridge) {
        Log.i(TAG, "create()");
        return new MediaPlayerBridge(nativeMediaPlayerBridge);
    }

    protected MediaPlayerBridge(long nativeMediaPlayerBridge) {
        mNativeMediaPlayerBridge = nativeMediaPlayerBridge;
    }

    protected MediaPlayerBridge() {
    }

    @CalledByNative
    protected void destroy() {
        if (mLoadDataUriTask != null) {
            mLoadDataUriTask.cancel(true);
            mLoadDataUriTask = null;
        }
        mNativeMediaPlayerBridge = 0;
    }

    protected MediaPlayer getLocalPlayer() {
        if (mPlayer == null) {
            mPlayer = new MediaPlayer();
        }
        Log.i(TAG, "getLocalPlayer() = " + mPlayer);
        return mPlayer;
    }

    @CalledByNative
    protected void setSurface(Surface surface) {
        Log.i(TAG, "Enter setSurface()");
        getLocalPlayer().setSurface(surface);
    }

    @CalledByNative
    protected boolean prepareAsync() {
        Log.i(TAG, "Enter prepareAsync()");
        try {
            getLocalPlayer().prepareAsync();
        } catch (IllegalStateException e) {
            Log.e(TAG, "Unable to prepare MediaPlayer.", e);
            return false;
        }
        return true;
    }

    @CalledByNative
    protected boolean isPlaying() {
        boolean ret = getLocalPlayer().isPlaying();
        Log.i(TAG, "isPlaying() = " + ret);
        return ret;
    }

    @CalledByNative
    protected int getVideoWidth() {
        int ret = getLocalPlayer().getVideoWidth();
        Log.i(TAG, "getVideoWidth() = " + ret);
        return ret;
    }

    @CalledByNative
    protected int getVideoHeight() {
        int ret = getLocalPlayer().getVideoHeight();
        Log.i(TAG, "Enter getVideoHeight() = "+ ret);
        return ret;
    }

    @CalledByNative
    protected int getCurrentPosition() {
        int ret = getLocalPlayer().getCurrentPosition();
        Log.i(TAG, "Enter getCurrentPosition() = "+ ret );
        return ret;
    }

    @CalledByNative
    protected int getDuration() {
        int ret = getLocalPlayer().getDuration();
        Log.i(TAG, "Enter getDuration() = "+ ret );
        return ret;
    }

    @CalledByNative
    protected void release() {
        Log.i(TAG, "Enter release()");
        getLocalPlayer().release();
    }

    @CalledByNative
    protected void setVolume(double volume) {
        Log.i(TAG, "Enter setVolume() , volume="+volume);
        getLocalPlayer().setVolume((float) volume, (float) volume);
    }

    @CalledByNative
    protected void start() {
        Log.i(TAG, "Enter start()");
        getLocalPlayer().start();
        /// M: for Media button intent handler
        MediaButtonIntentHandler handler = MediaButtonIntentHandler.getInstance();
        if (handler != null) {
            handler.registeHandler(null);
        }
    }

    @CalledByNative
    protected void pause() {
        Log.i(TAG, "Enter pause()");
        getLocalPlayer().pause();
        /// M: for Media button intent handler
        MediaButtonIntentHandler handler = MediaButtonIntentHandler.getInstance();
        if (handler != null) {
            handler.unRegisteHandler(null);
        }
    }

    @CalledByNative
    protected void seekTo(int msec) throws IllegalStateException {
        Log.i(TAG, "Enter seekTo() , msec="+msec);
        getLocalPlayer().seekTo(msec);
    }

    @CalledByNative
    protected boolean setDataSource(
            Context context, String url, String cookies, boolean hideUrlLog) {
        Uri uri = Uri.parse(url);
        Log.i(TAG, "setDataSource( " + url + " )");
        Log.i(TAG, "setDataSource( " + cookies + " )");
        Log.i(TAG, "setDataSource( " + hideUrlLog + " )");
        HashMap<String, String> headersMap = new HashMap<String, String>();
        if (hideUrlLog) headersMap.put("x-hide-urls-from-log", "true");
        if (!TextUtils.isEmpty(cookies)) headersMap.put("Cookie", cookies);

        if (url.toLowerCase().contains("m3u8") &&
            url.toLowerCase().contains("iqiyi.com")) {
            Log.i(TAG, "Enter setDataSource() , put User-Agent=iPad to headers");
            headersMap.put("User-Agent", "Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B367 Safari/531.21.10");
        }


        try {
            getLocalPlayer().setDataSource(context, uri, headersMap);
            /// M: to keep the background light on while playing and getting data
            if (context.checkCallingOrSelfPermission(permission.WAKE_LOCK)
                    == PackageManager.PERMISSION_GRANTED) {
                Log.i(TAG, "setWakeMode()");
                getLocalPlayer().setWakeMode(context, PowerManager.SCREEN_BRIGHT_WAKE_LOCK);
            }
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    @CalledByNative
    protected boolean setDataUriDataSource(final Context context, final String url) {
        if (mLoadDataUriTask != null) {
            mLoadDataUriTask.cancel(true);
            mLoadDataUriTask = null;
        }

        if (!url.startsWith("data:")) return false;
        int headerStop = url.indexOf(',');
        if (headerStop == -1) return false;
        String header = url.substring(0, headerStop);
        final String data = url.substring(headerStop + 1);

        String headerContent = header.substring(5);
        String headerInfo[] = headerContent.split(";");
        if (headerInfo.length != 2) return false;
        if (!"base64".equals(headerInfo[1])) return false;

        mLoadDataUriTask = new LoadDataUriTask(context, data);
        mLoadDataUriTask.execute();
        return true;
    }

    private class LoadDataUriTask extends AsyncTask <Void, Void, Boolean> {
        private final String mData;
        private final Context mContext;
        private File mTempFile;

        public LoadDataUriTask(Context context, String data) {
            mData = data;
            mContext = context;
        }

        @Override
        protected Boolean doInBackground(Void... params) {
            FileOutputStream fos = null;
            try {
                mTempFile = File.createTempFile("decoded", "mediadata");
                fos = new FileOutputStream(mTempFile);
                InputStream stream = new ByteArrayInputStream(mData.getBytes());
                Base64InputStream decoder = new Base64InputStream(stream, Base64.DEFAULT);
                byte[] buffer = new byte[1024];
                int len;
                while ((len = decoder.read(buffer)) != -1) {
                    fos.write(buffer, 0, len);
                }
                decoder.close();
                return true;
            } catch (IOException e) {
                return false;
            } finally {
                try {
                    if (fos != null) fos.close();
                } catch (IOException e) {
                    // Can't do anything.
                }
            }
        }

        @Override
        protected void onPostExecute(Boolean result) {
            if (isCancelled()) {
                deleteFile();
                return;
            }

            try {
                getLocalPlayer().setDataSource(mContext, Uri.fromFile(mTempFile));
            } catch (IOException e) {
                result = false;
            }

            deleteFile();
            assert (mNativeMediaPlayerBridge != 0);
            nativeOnDidSetDataUriDataSource(mNativeMediaPlayerBridge, result);
        }

        private void deleteFile() {
            if (mTempFile == null) return;
            if (!mTempFile.delete()) {
                // File will be deleted when MediaPlayer releases its handler.
                Log.e(TAG, "Failed to delete temporary file: " + mTempFile);
                assert (false);
            }
        }
    }

    protected void setOnBufferingUpdateListener(MediaPlayer.OnBufferingUpdateListener listener) {
        Log.i(TAG, "setOnBufferingUpdateListener( " + listener + " )");
        getLocalPlayer().setOnBufferingUpdateListener(listener);
    }

    protected void setOnCompletionListener(MediaPlayer.OnCompletionListener listener) {
        Log.i(TAG, "setOnCompletionListener( " + listener + " )");
        getLocalPlayer().setOnCompletionListener(listener);
    }

    protected void setOnErrorListener(MediaPlayer.OnErrorListener listener) {
        Log.i(TAG, "setOnErrorListener( " + listener + " )");
        getLocalPlayer().setOnErrorListener(listener);
    }

    protected void setOnPreparedListener(MediaPlayer.OnPreparedListener listener) {
        Log.i(TAG, "setOnPreparedListener( " + listener + " )");
        getLocalPlayer().setOnPreparedListener(listener);
    }

    protected void setOnSeekCompleteListener(MediaPlayer.OnSeekCompleteListener listener) {
        Log.i(TAG, "setOnSeekCompleteListener( " + listener + " )");
        getLocalPlayer().setOnSeekCompleteListener(listener);
    }

    protected void setOnVideoSizeChangedListener(MediaPlayer.OnVideoSizeChangedListener listener) {
        Log.i(TAG, "setOnVideoSizeChangedListener( " + listener + " )");
        getLocalPlayer().setOnVideoSizeChangedListener(listener);
    }

    /// M: for toast when audio/video is not supported.
    protected void setOnInfoListener(MediaPlayer.OnInfoListener listener) {
        Log.i(TAG, "setOnInfoListener( " + listener + " )");
        getLocalPlayer().setOnInfoListener(listener);
    }

    protected static class AllowedOperations {
        private final boolean mCanPause;
        private final boolean mCanSeekForward;
        private final boolean mCanSeekBackward;

        public AllowedOperations(boolean canPause, boolean canSeekForward,
                boolean canSeekBackward) {
            Log.i(TAG, "AllowedOperations( " + canPause + ", " + canSeekForward + ", " + canSeekBackward + " )");
            mCanPause = canPause;
            mCanSeekForward = canSeekForward;
            mCanSeekBackward = canSeekBackward;
        }

        @CalledByNative("AllowedOperations")
        private boolean canPause() { return mCanPause; }

        @CalledByNative("AllowedOperations")
        private boolean canSeekForward() { return mCanSeekForward; }

        @CalledByNative("AllowedOperations")
        private boolean canSeekBackward() { return mCanSeekBackward; }
    }

    /**
     * Returns an AllowedOperations object to show all the operations that are
     * allowed on the media player.
     */
    @CalledByNative
    protected AllowedOperations getAllowedOperations() {
        MediaPlayer player = getLocalPlayer();
        boolean canPause = true;
        boolean canSeekForward = true;
        boolean canSeekBackward = true;
        Log.i(TAG, "getAllowedOperations( " + player + " )");
        try {
            Method getMetadata = player.getClass().getDeclaredMethod(
                    "getMetadata", boolean.class, boolean.class);
            getMetadata.setAccessible(true);
            Object data = getMetadata.invoke(player, false, false);
            if (data != null) {
                Class<?> metadataClass = data.getClass();
                Method hasMethod = metadataClass.getDeclaredMethod("has", int.class);
                Method getBooleanMethod = metadataClass.getDeclaredMethod("getBoolean", int.class);

                int pause = (Integer) metadataClass.getField("PAUSE_AVAILABLE").get(null);
                int seekForward =
                    (Integer) metadataClass.getField("SEEK_FORWARD_AVAILABLE").get(null);
                int seekBackward =
                        (Integer) metadataClass.getField("SEEK_BACKWARD_AVAILABLE").get(null);
                hasMethod.setAccessible(true);
                getBooleanMethod.setAccessible(true);
                canPause = !((Boolean) hasMethod.invoke(data, pause))
                        || ((Boolean) getBooleanMethod.invoke(data, pause));
                canSeekForward = !((Boolean) hasMethod.invoke(data, seekForward))
                        || ((Boolean) getBooleanMethod.invoke(data, seekForward));
                canSeekBackward = !((Boolean) hasMethod.invoke(data, seekBackward))
                        || ((Boolean) getBooleanMethod.invoke(data, seekBackward));
            }
        } catch (NoSuchMethodException e) {
            Log.e(TAG, "Cannot find getMetadata() method: " + e);
        } catch (InvocationTargetException e) {
            Log.e(TAG, "Cannot invoke MediaPlayer.getMetadata() method: " + e);
        } catch (IllegalAccessException e) {
            Log.e(TAG, "Cannot access metadata: " + e);
        } catch (NoSuchFieldException e) {
            Log.e(TAG, "Cannot find matching fields in Metadata class: " + e);
        }
        return new AllowedOperations(canPause, canSeekForward, canSeekBackward);
    }

    private native void nativeOnDidSetDataUriDataSource(long nativeMediaPlayerBridge,
                                                        boolean success);
}
