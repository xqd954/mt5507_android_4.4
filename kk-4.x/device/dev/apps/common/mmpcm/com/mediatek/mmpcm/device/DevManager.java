package com.mediatek.mmpcm.device;

import java.util.ArrayList;
import java.util.Iterator;

import com.mediatek.dm.Device;
import com.mediatek.dm.DeviceManager;
import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.dm.DeviceManagerListener;
import com.mediatek.dm.MountPoint;

/**
 * 
 *This class represents manager local device.
 *
 */
public class DevManager {
    private static DevManager dman = null;
    private DeviceManager dm;
    private ArrayList<DevListener> onDevListener;
    static private final String TAG = "DevManager";
    
    private DevManager(){
        onDevListener = new ArrayList<DevListener>();
        dm = DeviceManager.getInstance();
        dm.addListener(dmListener);
    }
    /**
     * Get device manager instance.
     * @return
     */
    public static DevManager getInstance(){
        if (dman == null) {
            synchronized (DevManager.class) {
               // if (dman == null) {
                    dman = new DevManager();
                //}
            }
        }
        return dman;
    }
    /**
     * Get mount point count.
     * @return
     */
    public int getMountCount(){
        return dm.getMountPointCount();
    }
    /**
     * Get mount point list.
     * @return
     */
    public ArrayList<MountPoint> getMountList(){
        return dm.getMountPointList();
    }
    /**
     * Get mount point info by specified path.
     * @param path
     * @return
     */
    public MountPoint getPointInfo(String path){
        return dm.getMountPoint(path);
    }
    /**
     * Add a device notify listenr.
     * @param devListener
     */
    public void addDevListener(DevListener devListener){
        onDevListener.add(devListener);
    }
    /**
     * Remove a device listener by specified device listener.
     * @param devListener
     */
    public void removeDevListener(DevListener devListener){  	
        onDevListener.remove(devListener);
    }
    
    public void removeDevListeners(){
    	onDevListener.clear();
    }
	/**
	 * Mount a iso file by specified iso file path.
	 * @param isoFilePath
	 */
    public void mountISOFile( String isoFilePath)
    {
    	dm.mountISO(isoFilePath);
    }
    /**
     * Unmount a iso file by specified iso file path.
     * @param isoMountPath
     */
    public void umoutISOFile(String isoMountPath)
    {
    	dm.umountISO(isoMountPath);
    }
    /**
     * Check the device whether is virtual device by specified path.
     * @param isMountPath
     * @return
     */
    public boolean isVirtualDev(String isMountPath)
    {
    	return dm.isVirtualDevice(isMountPath);
    }
    //end ISO
    /**
     * Unmount device by specified mount point
     * @param mountPoint
     */ 
    public void unMountDevice(MountPoint mountPoint){
    	unMountDevice(getDeviceName(mountPoint));
    }
    

    private void unMountDevice(String devName){
    	if (devName != null){
    		dm.umountDevice(devName);
    	}
    	
    }
    
    private String getDeviceName(MountPoint mountPoint){
    	Device dv = dm.getParentDevice(mountPoint);
    	return dv != null ? dv.mDeviceName : null;
    }
    /**
     * destroy device manager and remove devices listenr.
     */
    public void destroy(){
    	dm.removeListener(dmListener);
    	onDevListener.clear();
    	dman = null;
    }
    
   private DeviceManagerListener dmListener = new DeviceManagerListener(){

        public void onEvent(DeviceManagerEvent arg0) {
           
            if(onDevListener.size() > 0){
                Iterator<DevListener> it = onDevListener.iterator(); 
                
                while(it.hasNext()){
                    DevListener lis = it.next();
                    lis.onEvent(arg0);
                }               
            }
        }        
    };
}
