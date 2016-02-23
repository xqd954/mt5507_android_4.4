
package android.net.pppoe;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.net.NetworkStateTracker;
import android.net.NetworkInfo.DetailedState;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.os.Parcel;
import android.util.Slog;
import android.content.Context;
import android.content.IntentFilter;
import android.net.NetworkInfo;
import android.net.LinkCapabilities;
import android.net.LinkProperties;
import android.net.NetworkUtils;
import android.net.RouteInfo;
import android.net.ConnectivityManager;
import android.net.ethernet.EthernetManager;
import android.net.ethernet.EthernetStateTracker;
//import android.net.wifi.MWifiManager;
import android.text.TextUtils;
import android.net.LinkAddress;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.concurrent.atomic.AtomicBoolean;
import android.net.SamplingDataTracker;//add by hqs@tcl
import android.os.Messenger;//add by hqs@tcl
import android.net.LinkQualityInfo;//add by hqs@tcl

import java.io.File;
import java.io.Reader;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Track the state of pppoe connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 *
 * @hide
 */
public class PppoeStateTracker extends Handler implements NetworkStateTracker {
    private static final String TAG = "PppoeStateTracker";
    private NetworkInfo mNetworkInfo;
    private LinkProperties mLinkProperties;
    private LinkCapabilities mLinkCapabilities;
    private PppoeManager mPppoeManager;
    private int mPrefixLength;
    private BroadcastReceiver mPppoeStateReceiver;

    private AtomicBoolean mTeardownRequested = new AtomicBoolean(false);
    private AtomicBoolean mPrivateDnsRouteSet = new AtomicBoolean(false);
    private AtomicBoolean mDefaultRouteSet = new AtomicBoolean(false);

    /* For sending events to connectivity service handler */
    private Handler mCsHandler;
    private Context mContext;
    public static PppoeStateTracker sInstance;

    //add by hqs@tcl
    private SamplingDataTracker mSamplingDataTracker = new SamplingDataTracker();

    public PppoeStateTracker() {
        mNetworkInfo = new NetworkInfo(ConnectivityManager.TYPE_PPPOE, 0, "PPPOE", "");
        mLinkProperties = new LinkProperties();
        mNetworkInfo.setIsAvailable(false);
        setTeardownRequested(false);
        mPppoeManager = null;
    }

    public static synchronized PppoeStateTracker getInstance() {
        if (sInstance == null) sInstance = new PppoeStateTracker();
        return sInstance;
    }

    /**
    * -------------------------------------------------------------
    * Control Interface
    * -------------------------------------------------------------
    */
    /**
     * Begin monitoring data connectivity.
     *
     * This is the first method called when this interface is used.
     *
     * @param context is the current Android context
     * @param target is the Hander to which to return the events.
     */
    public void startMonitoring(Context context, Handler target) {
        mContext = context;
        mCsHandler = target;
        IntentFilter filter = new IntentFilter();
        filter.addAction(PppoeManager.PPPOE_STATE_ACTION);
		filter.addAction(EthernetManager.ETHERNET_STATE_CHANGED_ACTION);
        mPppoeStateReceiver = new PppoeStateReceiver();
        mContext.registerReceiver(mPppoeStateReceiver, filter);
    }

    /**
     * Fetch NetworkInfo for the network
     */
    public NetworkInfo getNetworkInfo() {
        return new NetworkInfo(mNetworkInfo);
    }

    /**
     * Return the LinkProperties for the connection.
     *
     * @return a copy of the LinkProperties, is never null.
     */
    public LinkProperties getLinkProperties() {
        return new LinkProperties(mLinkProperties);
    }

    /**
     * A capability is an Integer/String pair, the capabilities
     * are defined in the class LinkSocket#Key.
     *
     * @return a copy of this connections capabilities, may be empty but never null.
     */
    public LinkCapabilities getLinkCapabilities() {
        return new LinkCapabilities();
    }

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    public String getTcpBufferSizesPropName() {
        return "net.tcp.buffersize.default";
    }

    /**
     * Disable connectivity to a network
     * @return {@code true} if a teardown occurred, {@code false} if the
     * teardown did not occur.
     */
    public boolean teardown() {
        return true;
    }

    /**
     * Reenable connectivity to a network after a {@link #teardown()}.
     * @return {@code true} if we're connected or expect to be connected
     */
    public boolean reconnect() {
        return false;
    }
	
    /**
     * Captive check is complete, switch to network
     */
    @Override
    public void captivePortalCheckComplete() {
        // not implemented
    }

    /**
     * Turn the wireless radio off for a network.
     * @param turnOn {@code true} to turn the radio on, {@code false}
     */
    public boolean setRadio(boolean turnOn) {
        return false;
    }

    /**
     * Returns an indication of whether this network is available for
     * connections. A value of {@code false} means that some quasi-permanent
     * condition prevents connectivity to this network.
     *
     * NOTE that this is broken on multi-connection devices.  Should be fixed in J release
     * TODO - fix on multi-pdp devices
     */
    public boolean isAvailable() {
        return mNetworkInfo.isAvailable();
    }

    /**
     * User control of data connection through this network, typically persisted
     * internally.
     */
    public void setUserDataEnable(boolean enabled) {
        // not support.
    }

    /**
     * Policy control of data connection through this network, typically not
     * persisted internally. Usually used when {@link NetworkPolicy#limitBytes}
     * is passed.
     */
    public void setPolicyDataEnable(boolean enabled) {
        // not support.
    }

    /**
     * -------------------------------------------------------------
     * Storage API used by ConnectivityService for saving
     * Network specific information.
     * -------------------------------------------------------------
     */

    /**
     * Check if private DNS route is set for the network
     */
    public boolean isPrivateDnsRouteSet() {
        return mPrivateDnsRouteSet.get();
    }

    /**
     * Set a flag indicating private DNS route is set
     */
    public void privateDnsRouteSet(boolean enabled) {
        mPrivateDnsRouteSet.set(enabled);
    }

    /**
     * Check if default route is set
     */
    public boolean isDefaultRouteSet() {
        return mDefaultRouteSet.get();
    }

    /**
     * Set a flag indicating default route is set for the network
     */
    public void defaultRouteSet(boolean enabled) {
        mDefaultRouteSet.set(enabled);
    }

    /**
     * Check if tear down was requested
     */
    public boolean isTeardownRequested() {
        return mTeardownRequested.get();
    }

    /**
     * Indicate tear down requested from connectivity
     */
    public void setTeardownRequested(boolean isRequested) {
        mTeardownRequested.set(isRequested);
    }

    /**
     * An external dependency has been met/unmet
     */
    public void setDependencyMet(boolean met) {
        // not support
    }

    private LinkAddress makeLinkAddress() {
        String ipaddr = mPppoeManager.getIpaddr();
        if (TextUtils.isEmpty(ipaddr)) {
            Slog.i(TAG, "pppoe ip is null");
            return null;
        }
        return new LinkAddress(NetworkUtils.numericToInetAddress(ipaddr), mPrefixLength);
    }

    private void updateLinkProperties() {
        mLinkProperties.clear();
        LinkAddress linkAddress = makeLinkAddress();
        mLinkProperties.addLinkAddress(linkAddress);

        try {
            //InetAddress ia = InetAddress.getByName(mPppoeManager.getRoute());
            InetAddress pppoeGateway = InetAddress.getByName(mPppoeManager.getIpaddr());
            RouteInfo routeInfo = new RouteInfo(linkAddress, pppoeGateway);
            mLinkProperties.addRoute(routeInfo);
        } catch (UnknownHostException e) {
            Slog.i(TAG, "failed to add route");
        }

        String dns1 = mPppoeManager.getDns1();
        if (TextUtils.isEmpty(dns1) == false) {
            mLinkProperties.addDns(NetworkUtils.numericToInetAddress(dns1));
        } else {
            Slog.i(TAG, "dns1 is empty");
        }

        String dns2 = mPppoeManager.getDns2();
        if (TextUtils.isEmpty(dns2) == false) {
            mLinkProperties.addDns(NetworkUtils.numericToInetAddress(dns2));
        } else {
            Slog.i(TAG, "dns2 is empty");
        }

        mLinkProperties.setInterfaceName(mPppoeManager.getInterfaceName());

        Slog.i(TAG, "print linkproperties of pppoe:");
        Slog.i(TAG, mLinkProperties.toString());
    }

    private void notifyStateChange(String status) {
        if (status.equals(PppoeManager.PPPOE_STATE_CONNECT)
            || status.equals(PppoeManager.PPPOE_STATE_DISCONNECT)) {
            if (status.equals(PppoeManager.PPPOE_STATE_CONNECT)) {
                Slog.i(TAG, "PppoeManager.PPPOE_STATE_CONNECT status -->" + status);
                mNetworkInfo.setDetailedState(DetailedState.CONNECTED,null,null);
                mNetworkInfo.setIsAvailable(true);
                updateLinkProperties();
            } else if (status.equals(PppoeManager.PPPOE_STATE_DISCONNECT)) {
                Slog.i(TAG, "PppoeManager.PPPOE_STATE_DISCONNECT status -->" + status);
                mNetworkInfo.setDetailedState(DetailedState.DISCONNECTED,null,null);
                mNetworkInfo.setIsAvailable(false);
            }

            Message message = mCsHandler.obtainMessage(EVENT_STATE_CHANGED, new NetworkInfo(mNetworkInfo));
            message.sendToTarget();
        }
    }

    private void checkPppoeManager() {
        if (mPppoeManager == null) {
            mPppoeManager = (PppoeManager)mContext.getSystemService(Context.PPPOE_SERVICE);
        }
    }

    private class PppoeStateReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            checkPppoeManager();
            String action = intent.getAction();
            if (action.equals(PppoeManager.PPPOE_STATE_ACTION)) {
                Slog.i(TAG, "receive PppoeManager.PPPOE_STATE_ACTION");
                String pppState = intent.getStringExtra(PppoeManager.PPPOE_STATE_STATUE);
                notifyStateChange(pppState);
            } else if (action.equals(EthernetManager.ETHERNET_STATE_CHANGED_ACTION)) {
                if (intent.getIntExtra(EthernetManager.EXTRA_ETHERNET_STATE, -1) == EthernetStateTracker.EVENT_HW_DISCONNECTED) {
                    Slog.i(TAG, "receive EthernetStateTracker.EVENT_HW_DISCONNECTED");
                    if (mPppoeManager.getPppoeStatus().equals(PppoeManager.PPPOE_STATE_CONNECT)) {
                        String netif = mPppoeManager.PppoeGetInterface();
                        if (netif != null && netif.startsWith("eth") /* && isCableAvailable(netif) == false */) {
                            mPppoeManager.disconnectPppoe();
                        }
                    }
                }
            } 
		}
    }
		
     //add by hqs@tcl---below
    @Override
    public void startSampling(SamplingDataTracker.SamplingSnapshot s) {
        mSamplingDataTracker.startSampling(s);
    }

    @Override
    public void stopSampling(SamplingDataTracker.SamplingSnapshot s) {
        mSamplingDataTracker.stopSampling(s);
    }

    @Override
    public String getNetworkInterfaceName() {
        if (mLinkProperties != null) {
            return mLinkProperties.getInterfaceName();
        } else {
            return null;
        }
    }

    @Override
    public void supplyMessenger(Messenger messenger) {
        // not supported on this network
    }
    @Override
    public void removeStackedLink(LinkProperties link) {
        mLinkProperties.removeStackedLink(link);
    }
    @Override
    public void addStackedLink(LinkProperties link) {
        mLinkProperties.addStackedLink(link);
    }
    /**
     * Captive check is complete, switch to network
     */
    @Override
    public void captivePortalCheckCompleted(boolean isCaptivePortal) {
        // not implemented
    }

    /**
     * Return link info
     * @return an object of type WifiLinkQualityInfo
     */
    //this func used just for compile temp---hqs@tcl
    @Override
    public LinkQualityInfo getLinkQualityInfo() {
        //if (mNetworkInfo == null) {
            // no data available yet; just return
            return null;
        //}
    }

}



