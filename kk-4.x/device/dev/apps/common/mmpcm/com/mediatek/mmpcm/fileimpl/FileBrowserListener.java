package com.mediatek.mmpcm.fileimpl;

public interface FileBrowserListener {
    public void OnFileParseing(FileList fl);
    
    public void OnFileFound(FileList fl);

    public void OnFileRemove(FileList fl);

    public void OnFileFailed(int errMsg);
    
    /**
     * for get mount list
     * @param dl
     */
    public void OnDeviceFound(DeviceList dl);
    
    /**
     * for device mounted when has in MMP
     */
    public void OnDeviceMount(); 
    
    /**
     * for device unmounted when has in MMP
     */
    public void OnDeviceUnmount();
    
    public void OnDeviceUnsupport();
}
