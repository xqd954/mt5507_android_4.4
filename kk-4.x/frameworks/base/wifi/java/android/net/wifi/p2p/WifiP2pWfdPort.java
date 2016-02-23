package android.net.wifi.p2p;

import android.os.Parcelable;
import android.os.Parcel;
import android.util.Log;

public class WifiP2pWfdPort implements Parcelable {

	private static final String TAG = "WifiP2pWfdPort";

	public String mCtrlPort;

	public WifiP2pWfdPort() {
	}

	public WifiP2pWfdPort(String string) throws IllegalArgumentException {
		Log.e(TAG, "WifiP2pWfdPort string:" + string);
		String[] tokens = string.split("[ \n]");

		if (tokens.length < 1) {
			throw new IllegalArgumentException("Malformed supplicant event");
		}
		if (tokens[0].startsWith("P2P-WFD-DEV-INFO")) {
			String[] nameValue = tokens[2].split("=");
			if (nameValue[0].equals("wfd_dev_info")) {
				mCtrlPort = String.valueOf(parseHex(nameValue[1].substring(12,
						16)));
				Log.d("WifiP2pWfdPort", "rtspPort:" + mCtrlPort);
			}
		}
	}
	   /** Implement the Parcelable interface */
    public int describeContents() {
        return 0;
    }
	public String toString() {
		StringBuffer sbuf = new StringBuffer();
		sbuf.append("\n WFD CtrlPort: ").append(mCtrlPort);
		return sbuf.toString();
	}

	/** copy constructor */
	public WifiP2pWfdPort(WifiP2pWfdPort source) {
		if (source != null) {
			mCtrlPort = source.mCtrlPort;
		}
	}
	 //supported formats: 0x1abc, 0X1abc, 1abc
    private int parseHex(String hexString) {
        int num = 0;
        if (hexString.startsWith("0x") || hexString.startsWith("0X")) {
            hexString = hexString.substring(2);
        }

        try {
            num = Integer.parseInt(hexString, 16);
        } catch(NumberFormatException e) {
            Log.e("WifiP2pConfig", "Failed to parse hex string " + hexString);
        }
        return num;
    }
	/** Implement the Parcelable interface */
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeString(mCtrlPort);
	}

	public void readFromParcel(Parcel in) {
		mCtrlPort = in.readString();
	}

	/** Implement the Parcelable interface */
	public static final Creator<WifiP2pWfdPort> CREATOR = new Creator<WifiP2pWfdPort>() {
		public WifiP2pWfdPort createFromParcel(Parcel in) {
			WifiP2pWfdPort device = new WifiP2pWfdPort();
			device.readFromParcel(in);
			return device;
		}

		public WifiP2pWfdPort[] newArray(int size) {
			return new WifiP2pWfdPort[size];
		}
	};

}
