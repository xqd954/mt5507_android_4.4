package org.chromium.android_webview;

import android.content.Context;
import android.net.WebAddress;
import android.os.AsyncTask;
import android.view.ViewGroup;
import android.util.Log;

//import com.mediatek.xlog.Xlog;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

@JNINamespace("android_webview")
public class AwPasswordHandler {

    private static final String LOGTAG = "AwPasswordHandler";

    private static AwPasswordHandler mInstance = null;
    private static final String SCHEME_HOST_DELIMITER = "://";
    private static boolean mEnableSave = false;
    private long mNativeAwPasswordHandlerDelegateImpl;

    private AwContents mAwContents;
    private AwBrowserContext mBrowserContext = null;
    private PasswordDatabase mDb = null;

    public static AwPasswordHandler create(AwContents awContents, ViewGroup view, AwBrowserContext browserContext) {
        if (mInstance == null) {
            mInstance = new AwPasswordHandler(awContents, view, browserContext);
        }
        return mInstance;
    }

    @CalledByNative
    public static AwPasswordHandler getInstance() {
        Log.d(LOGTAG, "getInstance, mInstance = " + mInstance);
        return mInstance;
    }

    private AwPasswordHandler(AwContents awContents, ViewGroup view, AwBrowserContext browserContext) {
        mAwContents = awContents;
        mBrowserContext = browserContext;
        mDb = mBrowserContext.getPasswordDatabase(view.getContext());
    }
    
    public void setViewInfo(AwContents awContents) {//, ViewGroup view) {
        Log.d(LOGTAG, "setViewInfo, awcontents = " + awContents);
        mAwContents = awContents;
    }

    @CalledByNative
    private void reqeustPassword(
                    final long nativeAwPasswordHandlerDelegateImpl, final String[] data, final String[] data16) {
        WebAddress uri = new WebAddress(data[PasswordDatabase.FIELD_SIGNON_REALM]);
        final String newScheme = uri.getScheme() + SCHEME_HOST_DELIMITER + uri.getHost();
        mNativeAwPasswordHandlerDelegateImpl = nativeAwPasswordHandlerDelegateImpl;
        Log.d(LOGTAG, "reqeustPassword, newScheme = " + newScheme);
        Log.d(LOGTAG, "reqeustPassword, data16 = " + data16[0] + ", " + data16[1] + ", " + data16[2] + ", " + data16[3]);

		new AsyncTask<String, Void, PasswordDatabase.RequestRetData>() {
            @Override
            protected PasswordDatabase.RequestRetData doInBackground(String... params) {
                Log.d(LOGTAG, "reqeustPassword doInBackground " + newScheme);
                return mDb.getUsernamePassword(newScheme);
            }

            @Override
            protected void onPostExecute(PasswordDatabase.RequestRetData result) {
                Log.d(LOGTAG, "reqeustPassword onPostExecute " + result);
                if (result != null) {
                    String[] ret_data16 = new String[4];
                    String[] ret_data = new String[4];

                    if (!result.signonRealm.isEmpty()) {
                        ret_data[PasswordDatabase.FIELD_SIGNON_REALM] = result.signonRealm;
                    } else {
                        ret_data[PasswordDatabase.FIELD_SIGNON_REALM] = data[PasswordDatabase.FIELD_SIGNON_REALM];
                    }
                    if (!result.originSignonRealm.isEmpty()) {
                        ret_data[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM] = result.originSignonRealm;
                    } else {
                        ret_data[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM] = data[PasswordDatabase.FIELD_ORIGIN_SIGNON_REALM];
                    }
                    if (!result.originSpec.isEmpty()) {
                        ret_data[PasswordDatabase.FIELD_ORIGIN_SPEC] = result.originSpec;
                    } else {
                        ret_data[PasswordDatabase.FIELD_ORIGIN_SPEC] = data[PasswordDatabase.FIELD_ORIGIN_SPEC];
                    }
                    if (!result.actionSpec.isEmpty()) {
                        ret_data[PasswordDatabase.FIELD_ACTION_SPEC] = result.actionSpec;
                    } else {
                        ret_data[PasswordDatabase.FIELD_ACTION_SPEC] = data[PasswordDatabase.FIELD_ACTION_SPEC];
                    }
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, ret_data = " + ret_data[0] + ", " + ret_data[1]);
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, ret_data = " + ret_data[2] + ", " + ret_data[3]);
                    
                    if (!result.usernameName.isEmpty()) {
                        ret_data16[PasswordDatabase.FIELD_USERNAME_NAME] = result.usernameName;
                    } else {
                        ret_data16[PasswordDatabase.FIELD_USERNAME_NAME] = data16[PasswordDatabase.FIELD_USERNAME_NAME];
                    }
                    if (!result.usernameValue.isEmpty()) {
                        ret_data16[PasswordDatabase.FIELD_USERNAME_VALUE] = result.usernameValue;
                    } else {
                        ret_data16[PasswordDatabase.FIELD_USERNAME_VALUE] = data16[PasswordDatabase.FIELD_USERNAME_VALUE];
                    }
                    if (!result.passwordName.isEmpty()) {
                        ret_data16[PasswordDatabase.FIELD_PASSWORD_NAME] = result.passwordName;
                    } else {
                        ret_data16[PasswordDatabase.FIELD_PASSWORD_NAME] = data16[PasswordDatabase.FIELD_PASSWORD_NAME];
                    }
                    ret_data16[PasswordDatabase.FIELD_PASSWORD_VALUE] = result.passwordValue;
                    Log.d(LOGTAG, "reqeustPassword onPostExecute, ret_data16 = " + ret_data16[0] + ", " + ret_data16[1] + ", " + ret_data16[2] + ", " + ret_data16[3]);
                    nativeOnRequestDone(mNativeAwPasswordHandlerDelegateImpl, ret_data, ret_data16);
                }
            }
        }.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    public void clickHandler(final String[] data, final String[] data16, final boolean remember) {
        WebAddress uri = new WebAddress(data[PasswordDatabase.FIELD_SIGNON_REALM]);
        final String newScheme = uri.getScheme() + SCHEME_HOST_DELIMITER + uri.getHost();
        new AsyncTask<Object, Void, Void>() {
            @Override
            protected Void doInBackground(Object... params) {
                Log.d(LOGTAG, "clickHandler doInBackground " + newScheme + ", " + remember);
                mDb.setUsernamePassword(newScheme, data, data16, remember);
                return null;
            }
        }.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    @CalledByNative
    private void showPasswordDialog(final String[] data, final String[] data16, boolean permitted) {
        Log.d(LOGTAG, "showPasswordDialog permitted " + permitted);
        if (!permitted) {
            clickHandler(data, data16, true);
            return;
        }
        if (mAwContents != null) {
            mAwContents.showPasswordDialog(data, data16, permitted);
        }
    }

    public static void setSavePassword(boolean save) {
        Log.d(LOGTAG, "setSavePassword " + save);
        mEnableSave = save;
    }

    @CalledByNative
    private boolean getSavePassword() {
        Log.d(LOGTAG, "getSavePassword " + mEnableSave);
        return mEnableSave;
    }

    private native void nativeOnRequestDone(
        long nativeAwPasswordHandlerDelegateImpl, String[] data, String[] data16);
}
