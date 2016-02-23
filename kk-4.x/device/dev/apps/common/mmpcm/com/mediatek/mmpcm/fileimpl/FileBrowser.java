package com.mediatek.mmpcm.fileimpl;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.dm.MountPoint;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.device.DevListener;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.mmpcm.fileimpl.UsbFileOperater.OnUsbCopyProgressListener;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;
/**
 * This Class defines file browser 
 * @author MTK40642
 *
 */
public class FileBrowser {
    private static final String TAG = "File_Browser";

    static private FileBrowser fBrowser = null;
    static private MtkFile currentDir = null;
    static private int curSortType = FileConst.SORT_NAME;

    private UsbFileOperater usbManager;
    private DevManager devManager;
    private static FileBrowserListener fbListener;
    private static int ffilterType = FileConst.MMP_FF_ALL;
    private static DeviceList deviceList;
    private static FileList curFileList;
    private int tempRecursiveNum;

    /**
     * constructor of the class
     */
    private FileBrowser() {
        usbManager = UsbFileOperater.getInstance();
        devManager = DevManager.getInstance();
        deviceList = new DeviceList();
        curFileList = new FileList();
    }

    /**
     * get instance 
     * @return
     *     FileBrowser
     */
    public static FileBrowser getInstance() {
        if (fBrowser == null) {
            synchronized (FileBrowser.class) {
                if (fBrowser == null) {
                    fBrowser = new FileBrowser();
                }
            }
        }
        return fBrowser;
    }

    /**
     * Create a listener on file event
     * @param listener
     *            FileBrowserListener
     * @deprecated
     */
    public void setOnFileEventListener(FileBrowserListener listener) {
        fbListener = listener;
        devManager.addDevListener(deviceLis);
    }

    /**
     * Remove listener.
     * @deprecated
     */
    public void removeOnFileEventListener(){
    	fbListener = null;
    	devManager.removeDevListener(deviceLis);
    }
    /**
     * get mount count of the devices
     * @return
     *     the number of the mount devices
     */

    public int getMountCount() {
        return devManager.getMountCount();
    }

    /**
     * get mount point list
     */
    public void getMountPointList() {
        List<MountPoint> mList = new ArrayList<MountPoint>();
        deviceList.cleanList();

        /* get Local USB device */
        mList.addAll(0, devManager.getMountList());
        deviceList.addList(mList, DeviceList.TYPE_USB);
        sendMsg(FileConst.MSG_DEVICE_PARSED);
    }

    /**
     * get point information with path of the devices
     * @param path
     *        path of the devices
     * @return
     *        MountPoint
     */
    public MountPoint getPointInfo(String path) {
        return devManager.getPointInfo(path);
    }

    /**
     * set current directory 
     * @param dir
     *        the directory of the MtkFile
     * @return
     *        true if set success
     */
    public boolean setCurrentDir(MtkFile dir) {
        if (null == dir) {
            throw new NullPointerException("currentDir is null!");
        }
        currentDir = dir;
        return (currentDir != null);
    }

    /**
     * get current mtk file
     * 
     * @return
     */
    public MtkFile getCurrentDir() {
        return currentDir;
    }

    /**
     * get parent mtk file
     * 
     * @return
     */
    public MtkFile getParent() {
        return new MtkFile(currentDir.getParentFile());
    }

    /**
     * list the directory and filtered files in current path
     * 
     * @param file_filter
     * @return
     */
    public void listFilterFiles() {
        List<MtkFile> fileList = new ArrayList<MtkFile>();

        curFileList.cleanList();

        fileList = usbManager.listFilterFiles(ffilterType, currentDir,
                curSortType);

        curFileList.addList(fileList);

        if (fileList != null && fileList.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * used for playlist,UI layer don't call it
     * 
     * @param file_filter
     * @return the file list in current path
     */
    public MtkFile[] listFiles(int filterType) {
        return usbManager.getFile(currentDir, filterType, curSortType);
    }

    /**
     * list the directory and all files in current path
     * 
     * @return
     */
    public void listAllFiles() {
        List<MtkFile> fileList = new ArrayList<MtkFile>();
        curFileList.cleanList();

        ffilterType = FileConst.MMP_FF_ALL;

        fileList = usbManager.listAllFiles(currentDir, curSortType);

        curFileList.addList(fileList);

        if (fileList != null && fileList.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * get audio files
     * @return
     */
    private void getAudioFiles(Collection<MtkFile> af, File d) {

        Collections.addAll(af, usbManager.listAudio(d));

        tempRecursiveNum = tempRecursiveNum + af.size();
        MmpTool.LOG_DBG("size = " + af.size());
        if (tempRecursiveNum > FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME) {

            tempRecursiveNum -= FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME;

            curFileList.addArray(af.toArray(new MtkFile[af.size()]));

            sendMsg(FileConst.MSG_FILE_PARSEING);
        }

        List<File> mDirs = usbManager.getDirectory(d);

        if (mDirs != null && mDirs.size() > 0) {
            for (File f : mDirs) {
                getAudioFiles(af, f);
            }
        }
    }

    /**
     * list  audio for recursive play
     * @param curMount
     *            MtkFile
     */
    public void listRecursiveAudio(MtkFile curMount) {

        curFileList.cleanList();

        Collection<MtkFile> allFile = new ArrayList<MtkFile>();

        getAudioFiles(allFile, curMount);

        List<MtkFile> mfiles = new ArrayList<MtkFile>();
        for (MtkFile f : allFile) {
            mfiles.add(f);
        }

        curFileList.addList(mfiles);

        if (mfiles.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * get image files
     * @return
     */
    private void getImageFiles(Collection<MtkFile> af, File d) {
        Collections.addAll(af, usbManager.listAudio(d));

        tempRecursiveNum = tempRecursiveNum + af.size();

        if (tempRecursiveNum > FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME) {

            tempRecursiveNum -= FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME;

            curFileList.addArray(af.toArray(new MtkFile[af.size()]));

            sendMsg(FileConst.MSG_FILE_PARSEING);
        }

        List<File> mDirs = usbManager.getDirectory(d);

        if (mDirs != null && mDirs.size() > 0) {
            for (File f : mDirs) {
                getImageFiles(af, f);
            }
        }
    }
    
    /**
     * list image of recursive play
     * @param curMount
     *         MtkFile
     */
    public void listRecursiveImage(MtkFile curMount) {

        curFileList.cleanList();

        Collection<MtkFile> allFile = new ArrayList<MtkFile>();

        getImageFiles(allFile, curMount);

        List<MtkFile> mfiles = new ArrayList<MtkFile>();

        for (MtkFile f : allFile) {
            mfiles.add(f);
        }

        curFileList.addList(mfiles);

        if (mfiles.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * get Video Files
     * @return
     */
    private void getVideoFiles(Collection<MtkFile> af, File d) {
        Collections.addAll(af, usbManager.listAudio(d));

        tempRecursiveNum = tempRecursiveNum + af.size();

        if (tempRecursiveNum > FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME) {

            tempRecursiveNum -= FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME;

            curFileList.addArray(af.toArray(new MtkFile[af.size()]));

            sendMsg(FileConst.MSG_FILE_PARSEING);
        }

        List<File> mDirs = usbManager.getDirectory(d);

        if (mDirs != null && mDirs.size() > 0) {
            for (File f : mDirs) {
                getVideoFiles(af, f);
            }
        }
    }

    /**
     * list video for recursive play
     * @param curMount
     *          MtkFile
     */
    public void listRecursiveVideo(MtkFile curMount) {

        curFileList.cleanList();

        Collection<MtkFile> allFile = new ArrayList<MtkFile>();

        getVideoFiles(allFile, curMount);

        List<MtkFile> mfiles = new ArrayList<MtkFile>();
        for (MtkFile f : allFile) {
            mfiles.add(f);
        }

        curFileList.addList(mfiles);

        if (mfiles.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * get Text Files
     * @return
     */
    private void getTextFiles(Collection<MtkFile> af, File d) {
        Collections.addAll(af, usbManager.listAudio(d));

        tempRecursiveNum = tempRecursiveNum + af.size();

        if (tempRecursiveNum > FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME) {

            tempRecursiveNum -= FileConst.RECURSIVE_PARSE_COUNT_PER_TIIME;

            curFileList.addArray(af.toArray(new MtkFile[af.size()]));

            sendMsg(FileConst.MSG_FILE_PARSEING);
        }

        List<File> mDirs = usbManager.getDirectory(d);

        if (mDirs != null && mDirs.size() > 0) {
            for (File f : mDirs) {
                getTextFiles(af, f);
            }
        }
    }

    /**
     * list Text for recursive play
     * @param curMount
     *         MtkFile
     */
    public void listRecursiveText(MtkFile curMount) {

        curFileList.cleanList();

        Collection<MtkFile> allFile = new ArrayList<MtkFile>();

        getTextFiles(allFile, curMount);

        List<MtkFile> mfiles = new ArrayList<MtkFile>();
        for (MtkFile f : allFile) {
            mfiles.add(f);
        }

        curFileList.addList(mfiles);

        if (mfiles.size() > 0) {
            sendMsg(FileConst.MSG_FILE_PARSED);
        } else {
            sendMsg(FileConst.MSG_FILE_EMPTY);
        }
    }

    /**
     * shuffle files
     * @param
     * @return
     */
    static public MtkFile[] shuffleFiles(MtkFile f[]) {
        List<MtkFile> list = new ArrayList<MtkFile>(Arrays.asList(f));
        Collections.shuffle(list);
        return list.toArray(new MtkFile[list.size()]);
    }

    /**
     * set current type
     * @param sort_type
     *          current type
     */
    public void setSortType(int sortType) {
        curSortType = sortType;
    }

    /**
     * get current type
     * @return
     */
    public int getSortType() {
        return curSortType;
    }

    /**
     * set File Filter Type
     * @param sort_type
     */
    public void setFileFilterType(int filterType) {
        ffilterType = filterType;
    }

    /**
     * get File Filter Type
     * @return
     */
    public int getFileFilterType() {
        return ffilterType;
    }

    /**
     * get File Type
     * @param file
     * @return
     */
    public int getFileType(MtkFile file) {
        if (file.isTextFile()) {
            return FileConst.FILE_TYPE_TEXT;
        } else if (file.isPhotoFile()) {
            return FileConst.FILE_TYPE_IMAGE;
        } else if (file.isVideoFile()) {
            return FileConst.FILE_TYPE_VIDEO;
        } else if (file.isAudioFile()) {
            return FileConst.FILE_TYPE_AUDIO;
        }

        return FileConst.FILE_TYPE_IMAGE;
    }

    /**
     * get extension information of the file
     * @param file
     *        MtkFile
     * @return
     */
    public String getFileExtension(MtkFile file) {
        if (file.isDirectory() == false) {
            String name = file.getName();
            int lastIndexOfDot = name.lastIndexOf('.');
            int fileLength = name.length();

            return name.substring(lastIndexOfDot + 1, fileLength);
        }
        return null;
    }

    /**
     * set copy abort
     * @param abort
     */
    public void setCopyAbort(boolean abort) {
        usbManager.setCopyAbort(abort);
    }

    /**
     * create Usb Copy Progress Listener 
     * @param listener
     */
    public void setOnCopyProgressListener(OnUsbCopyProgressListener listener) {
        usbManager.setOnUsbCopyProgressListener(listener);
    }

    /**
     * copy file to the destination
     * @param src
     *        source
     * @param dst
     *        destination
     * @return
     * @throws IOException
     */
    public boolean copyFile2Dir(MtkFile src, MtkFile dst) throws IOException {
        return usbManager.copyFile2Dir(src, dst);
    }

    /**
     * Add File To DeleteList
     * @param f
     *       the file to be deleted
     * @return
     * @throws IOException
     */
    public boolean addFileToDeleteList(MtkFile f) throws IOException {
        return usbManager.addFileToDeleteList(f);
    }

    /**
     * delete files of the USB
     */
    public void deleteFiles() {
        usbManager.deleteFiles();
    }

    private void sendMsg(int msg) {
        Message m = new Message();
        m.what = msg;
        fbMsgHandler.sendMessage(m);
    }

    /**
     * handler message of the file browser
     */
    Handler fbMsgHandler = new Handler() {
        public void handleMessage(Message msg) {

            switch (msg.what) {
            case FileConst.MSG_DEVICE_PARSED:
                if (fbListener != null) {
                    fbListener.OnDeviceFound(deviceList);
                }
                break;

            case FileConst.MSG_FILE_PARSEING:
                if (fbListener != null) {
                    fbListener.OnFileParseing(curFileList);
                }
                break;

            case FileConst.MSG_FILE_PARSED:
                if (fbListener != null) {
                    fbListener.OnFileFound(curFileList);
                }
                break;

            case FileConst.MSG_FILE_EMPTY:
                if (fbListener != null) {
                    fbListener.OnFileFailed(msg.what);
                }
                break;

            default:
                break;
            }

        }
    };

    /**
     * create device listener
     */
    DevListener deviceLis = new DevListener() {

        public void onEvent(DeviceManagerEvent event) {
            // TODO Auto-generated method stub
            switch (event.getType()) {
            case DeviceManagerEvent.mounted:
                if (fbListener != null) {
                    fbListener.OnDeviceMount();
                }
                break;

            case DeviceManagerEvent.umounted:
                if (fbListener != null) {
                    fbListener.OnDeviceUnmount();
                }
                break;

            case DeviceManagerEvent.unsupported:
                break;

            case DeviceManagerEvent.connected:
                break;

            case DeviceManagerEvent.disconnected:
                break;

            default:
                break;

            }
        }
    };
}
