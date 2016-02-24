/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * @2010-2013 TCL CORPORATION All Rights Reserved.
 */

package  com.tcl.os.storage;

import android.os.RemoteException;
import com.android.internal.util.Preconditions;
import android.os.ServiceManager;
import android.os.Parcelable;
import android.util.Log;
import android.os.storage.StorageManager;
import android.os.storage.StorageEventListener;
import android.os.storage.StorageVolume;
import android.os.Environment;
import android.os.storage.IMountService;
import android.content.Context;
import android.os.Handler;
import android.os.RemoteException;
import android.os.Looper;
import android.os.Message;
import android.content.Context;
import android.os.storage.IMountServiceListener;

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;
import java.io.IOException;
import java.io.FileNotFoundException;

import com.google.android.collect.Maps;
import com.tcl.os.storage.TStorageVolume;
import com.tcl.os.storage.TStorageEventListener;

public class TStorageManager
{
	private static final String TAG = "TStorageManager";

	final private IMountService mMountService;
	private static TStorageManager mInstance = null;
	private StorageManager mStorageManager;
	private final Object mStorageEventListenerLock = new Object();
	private final HashMap<TStorageEventListener, StorageEventListener> mStorageEventListenerMap = Maps.newHashMap();

	private TStorageManager(Context context) {
		mMountService = IMountService.Stub.asInterface(ServiceManager.getService("mount"));
   		if (mMountService == null) {
   			Log.e(TAG, "Unable to connect to mount service! - is it running yet?");
        	return;
    	}
		mStorageManager = (StorageManager)context.getSystemService(Context.STORAGE_SERVICE); 
	}

	public static synchronized TStorageManager getInstance(Context context){
		if (mInstance == null) {
			mInstance = new TStorageManager(context);
		}
		return mInstance;
	}
/*
 * Get Volume list.
 * @author TCL TVOS Team
 * @return StorageVolume[] type list
 */
	public StorageVolume[] getVolumeList() {
		if (mMountService == null) return new StorageVolume[0];
		try {
			Parcelable[] list = mMountService.getVolumeList();
        	if (list == null) return new StorageVolume[0];
        	int length = list.length;
        	StorageVolume[] result = new StorageVolume[length];
        	for (int i = 0; i < length; i++) {
            	result[i] = (StorageVolume)list[i];
        	}
        	return result;
    	} catch (RemoteException e) {
        	Log.e(TAG, "Failed to get volume list", e);
        	return null;
    	}
	} 

/*
 * Get Volume list and different from getVolumeList().
 * It uses different constructor.
 * @author TCL TVOS Team
 * @return StorageVolume[] type list
 */
	public TStorageVolume[] getVolumeListTcl() {
		if (mMountService == null) return new TStorageVolume[0];
		try {
			Parcelable[] list = mMountService.getVolumeList();
        	if (list == null) return new TStorageVolume[0];
        	int length = list.length;
        	TStorageVolume[] result = new TStorageVolume[length];
        	for (int i = 0; i < length; i++) {
				StorageVolume tsv = (StorageVolume)list[i];
            	result[i] = new TStorageVolume(tsv.getStorageId(), tsv.getPathFile(), tsv.getDescriptionId(),
												tsv.isPrimary(), tsv.isRemovable(), tsv.isEmulated(), tsv.getMtpReserveSpace(),
												tsv.allowMassStorage(), tsv.getMaxFileSize(), tsv.getOwner());
        	}
        	return result;
    	} catch (RemoteException e) {
        	Log.e(TAG, "Failed to get volume list", e);
        	return null;
    	}
	} 

/*
 * Get Volume Path.
 * @author TCL TVOS Team
 * @return String type path.
 */
	public String[] getVolumePaths() {
		StorageVolume[] volumes = getVolumeList();
    	if (volumes == null) return null;
    	int count = volumes.length;
    	String[] paths = new String[count];
    	for (int i = 0; i < count; i++) {
    		paths[i] = volumes[i].getPath();
    	}
    	return paths;
	}

/*
 * Get Volume state.
 * @author TCL TVOS Team
 * @param mountPoint the place of device mount.
 * @return state
 */
	public String getVolumeState(String mountPoint) {
		if (mMountService == null) return Environment.MEDIA_REMOVED;
		try {
			return mMountService.getVolumeState(mountPoint);
		} catch (RemoteException e) {
			Log.e(TAG, "Failed to get volume state", e);
			return null;
		}
	}

/*
 * Get Volume file system type.
 * @author TCL TVOS Team
 * @param mountPoint the place of device mount
 * @return file system type
 */
    public String getVolumeFstype(String mountPoint) {
        Preconditions.checkNotNull(mountPoint, "Path cannot be null");
        if (mMountService == null) return new String();
        try {
            return mMountService.getVolumeFstype(mountPoint);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get volume state", e); 
            return null;
        }   
    } 

/*
 * Get Volume label
 * @author TCL TVOS Team
 * @param mountPoint the place of device mount
 * @return label name
 */
    public String getVolumeLabel(String mountPoint) {
        Preconditions.checkNotNull(mountPoint, "Path cannot be null");
        if (mMountService == null) return new String();
        try {
            return mMountService.getVolumeLabel_TCL(mountPoint); // lvh@tcl, mstar duplicate , 2014-05-19
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get volume state", e); 
            return null;
        }   
    }   
	
/*
 * Get Volume uuid.
 * @author TCL TVOS Team
 * @param mountPoint the place of device mount
 * @return uuid
 */
    public String getVolumeUuid(String mountPoint) {
        Preconditions.checkNotNull(mountPoint, "mountPoint cannot be null");
        if (mMountService == null) return new String();
        try {
            return mMountService.getVolumeUuid(mountPoint);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get volume state", e);
            return null;
        }
    }

	/* Begin to modify by Sarah Zhang(zxia@tcl.com) */
    public String getVolumeDevpath(String mountPoint) {
        Preconditions.checkNotNull(mountPoint, "mountPoint cannot be null");
        if (mMountService == null) return new String();
        try {
            return mMountService.getVolumeDevpath(mountPoint);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get volume state", e);
            return null;
        }
    }
	/* End to modify by Sarah Zhang(zxia@tcl.com) */

	public int getVolumeProperty(String mountPoint) {
		int result = 0;		
		File mFile = new File("/proc/mounts");
		FileReader mFileReader = null;

		try {
			mFileReader = new FileReader(mFile);
		} catch (FileNotFoundException e) {
			Log.e(TAG, "not find file", e);	
			return 0;
		}

		try {
        	BufferedReader mBufferedReader  = new BufferedReader(mFileReader);
			String tmpString = null;
			while((tmpString = mBufferedReader.readLine()) != null) {
				if (tmpString.contains(mountPoint))	{
					result |= 0x01;
					String[] str = tmpString.split(" ");	
					if (str[3].regionMatches(0, "ro", 0, 2)) {
						result |= 0x02;	
					} else if (str[3].regionMatches(0, "rw", 0, 2)){
						result |= 0x04;	
					}
						break;
				}
			}
		} catch (IOException e) {
			Log.e(TAG,"read /proc/mounts failed",e);
			return 0;
		}

 		return result;
	}
	
/*
 * Unmount Volume.
 * @author TCL TVOS Team
 * @param mountPoint the place of device mount
 */
	public void unmountVolume(String mountPoint) {
		Preconditions.checkNotNull(mountPoint, "Path cannot be null");
        if(mMountService == null) {
			return;
		}
        try{
            Log.d(TAG, "unmount path via storagemanager Volume by cuixk");
            mMountService.unmountVolume(mountPoint, true,false);
        }catch(RemoteException e){
            Log.e(TAG, "unmount Volume failed",e);
        }
	}

	public void unmountVolume(String mountPoint, boolean force, boolean removeEncryption) { /* lvh@tcl */
		Preconditions.checkNotNull(mountPoint, "Path cannot be null");
        if(mMountService == null) {
			return;
		}
        try{
            Log.d(TAG, "unmount path via storagemanager Volume by cuixk");
            mMountService.unmountVolume(mountPoint, force,removeEncryption);
        }catch(RemoteException e){
            Log.e(TAG, "unmount Volume failed",e);
        }
	}

/*
 * Register Listener.
 * @author TCL TVOS Team
 * @param listener Event listener
 */
	public void registerListener(TStorageEventListener listener) {
		final TStorageEventListener mListener = listener;
		StorageEventListener mStorageEventListener = new StorageEventListener() {
			@Override
			public void onUsbMassStorageConnectionChanged(boolean available){
				mListener.onUsbMassStorageConnectionChanged(available);
			}
			
			@Override
			public void onStorageStateChanged(String path, String oldState, String newState){
				mListener.onStorageStateChanged(path, oldState, newState);
			}
		};

		synchronized(mStorageEventListenerLock) {
			mStorageEventListenerMap.put(listener, mStorageEventListener);
		}
		mStorageManager.registerListener(mStorageEventListener);
	}
	
	public void unregisterListener(TStorageEventListener listener) {
		final TStorageEventListener mListener = listener;
		StorageEventListener mStorageEventListener;
		synchronized(mStorageEventListenerLock) {
			mStorageEventListener = mStorageEventListenerMap.get(listener);
		}
		mStorageManager.unregisterListener(mStorageEventListener);
 	}

}

