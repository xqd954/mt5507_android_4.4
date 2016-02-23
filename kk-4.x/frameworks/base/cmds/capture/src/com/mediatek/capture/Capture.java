package com.mediatek.capture;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.graphics.Rect;
import android.os.HandlerThread;
import android.os.Looper;

public class Capture {
	private HandlerThread thread;

	public static void main(final String[] args) {
		(new Capture()).run(args);
	}

	private void run(String[] args) {
		if (args.length < 1) {
			showUsage();
			return;
		}

		String command = args[0];

		if (command.equals("0") || command.equals("1")) {
			String str;
			if (args.length >= 2) {
				str = args[1];
			} else {
				// Create a media file name
				String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss")
						.format(new Date());
				str = "/mnt/usbdisk" + File.separator + "IMG_" + timeStamp
						+ ".jpg";
			}

			File file = new File(str);
			if (file.getParent() != null) {
				File dir = new File(file.getParent());
				if (!dir.exists()) {
					dir.mkdirs();
				}
			} else {
				System.err.println("Illegal jpg path name:" + str);
				return;
			}

			thread = new HandlerThread("capture");
			thread.start();
			CaptureScreen.getInstance(thread.getLooper())
					.captureJPGPictureOnce(Integer.valueOf(command),
							new Rect(0, 0, 1280, 720), file);
			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			thread.quit();
		} else {
			System.err.println("Error: Unknown command: " + command);
			showUsage();
			return;
		}
	}

	private void showUsage() {
		System.err.println("usage: capture [0|1] [jpg path]");
		System.err.println("e.g.: # capture 0 /mnt/usbdisk/test.jpg");
	}
}
