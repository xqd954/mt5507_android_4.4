package com.mediatek.mmpcm.fileimpl;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.dm.MountPoint;

public class DeviceList {
    public static final int TYPE_USB = 1;
    public static final int TYPE_SMB = 2;
    public static final int TYPE_DLNA = 3;

    private static int usbCount;
    private static int smbCount;
    private static int dlnaCount;
    private static List<MountPoint> usbList = new ArrayList<MountPoint>(64);
    private static List<MountPoint> smbList = new ArrayList<MountPoint>(128);
    private static List<MountPoint> dlnaList = new ArrayList<MountPoint>(128);
    
    public void addList(List<MountPoint> dl, int deviceType) {
        if (deviceType == TYPE_USB) {
            usbList.addAll(dl);
            usbCount = usbList.size();
        } else if (deviceType == TYPE_SMB) {
            smbList.addAll(dl);
            smbCount = smbList.size();
        } else if (deviceType == TYPE_DLNA) {
            dlnaList.addAll(dl);
            dlnaCount = dlnaList.size();
        }
    }

    public List<MountPoint> getList() {
        List<MountPoint> List = new ArrayList<MountPoint>();

        List.addAll(0, usbList);
        List.addAll(usbCount, smbList);
        List.addAll((usbCount + smbCount), dlnaList);
        return List;
    }
    
    public int getCount() {
        return (usbList.size() + smbList.size() + dlnaList.size());
    }
    
    public void cleanList(){
        usbList.clear();
        smbList.clear();
        dlnaList.clear();
        usbCount = 0;
        smbCount = 0;
        dlnaCount = 0;
    }
    
    public void cleanList(int deviceType){
        if (deviceType == TYPE_USB) {
            usbList.clear();
            usbCount = 0;
        } else if (deviceType == TYPE_SMB) {
            smbList.clear();
            smbCount = 0;
        } else if (deviceType == TYPE_DLNA) {
            dlnaList.clear();
            dlnaCount = 0;
        }
    }
}
