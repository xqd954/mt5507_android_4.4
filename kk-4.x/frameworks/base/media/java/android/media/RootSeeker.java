package android.media;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

public class RootSeeker
{

public static int exec(String cmd) {
		// TODO Auto-generated method stub
		LocalSocketAddress lsa = new android.net.LocalSocketAddress("sserver", LocalSocketAddress.Namespace.RESERVED);
		LocalSocket ls = new LocalSocket();
		byte[] buffer = new byte[16]; // receive result "ok" or "fail"
		InputStream is = null;
		OutputStream os = null;
		
		
		try {
			Log.d("RootSeeker","start connect");
			ls.connect(lsa);
			is = ls.getInputStream();
			os = ls.getOutputStream();
			os.write(cmd.getBytes());
			int rlen = is.read(buffer);
			Log.d("MediaPlayer","rlen is "+rlen);


			if(rlen != -1){
				String result = new String(buffer, 0, rlen);
				if (result.equals("ok")) {
					Log.d("localSocketDemo", "OK");
				} else if (result.equals("fail")) {
					Log.d("localSocketDemo", "FAIL");
				}
			}else{
				Log.d("localSocketdemo"," read failed!!!!!!!!!!!");
			}
			
			is.close();
			os.close();
			ls.close();

			return 0;

		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return 0;
		
	}

}
