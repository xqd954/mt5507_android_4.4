package com.mediatek.mmpcm.mmc;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public interface IPlayList {

    public boolean addFiles(int filterType,int fileType, List<String> files);

    /**
     * remove one file from playlist
     * 
     * @param fileType
     *            file type
     * @param index
     *            the file index in playlist which want to remove
     * @return return removed file return null if failed
     */
    String removeFile(int filterType, int index);

    /**
     * clear playlist
     * 
     * @param fileType
     *            file teype
     */
    void cleanList(int filterType);

    /**
     * get playlist
     * 
     * @param fileType
     *            file teype
     */
    ArrayList<String> getList(int filterType);

    /**
     * set current play file index
     * 
     * @param fileType
     *            file type
     * @param index
     *            file index in playlist
     * @return return true if success,return false if fail
     */
    boolean setCurrentIndex(int filterType, int index);

    /**
     * get the index of current played file
     * 
     * @param fileType
     *            file type
     * @return return index,eturn -1 if failed
     */
    int getCurrentIndex(int filterType);

    /**
     * get total file num in playlist
     * 
     * @param fileType
     *            file type
     * @return toutal num,return -1 if failed
     */
    int getFileNum(int filterType);

    /**
     * get file in playlist accroding to index
     * 
     * @param fileType
     *            file type
     * @param index
     *            index in playlist
     * @return file, eturn null if failed
     */
    String getFile(int filterType, int index);

    /**
     * get next file
     * 
     * @param fileType
     *            file type
     * @param mode
     *            play mode Const.AUTOPLAY means auto next Const.MANUALPRE means
     *            manual change to prev one Const.MANUALNEXT means manual
     *            change to next
     * @return next file return null if end of playlist
     */
    String getNext(int filterType, int mode);

    /**
     * set repeat mode
     * 
     * @param fileType
     *            file type
     * @param mode
     *            repeat mode
     * @return return true if success,return false if fail
     */
    boolean setRepeatMode(int filterType, int mode);

    /**
     * get repeat mode
     * 
     * @param fileType
     *            file type
     * @return play mode ,return -1 if failed
     */
    int getRepeatMode(int filterType);

    public void setShuffleMode(int filterType, boolean mode);

    public boolean getShuffleMode(int filterType);

    /**
     * mark file in REPEAT_MARKED mode only support audio and photo file
     * 
     * @param fileType
     *            file type
     * @param index
     *            index
     * @param mark
     *            is need to mark. true means mark false means unmark
     * @return return true if success,return false if fail
     */
    boolean setMark(int filterType, int index, boolean mark);

    /**
     * judge some file is marded or not only support audio and photo file
     * 
     * @param fileType
     *            file type
     * @param index
     *            file index
     * @return true means has marded false means unmarked
     */
    boolean getMark(int filterType, int index);
    
    public String getCurrentPath(int filterType);
    public File getCurrentFile(int filterType);
    public boolean setCurrentFile(int filterType, String path);
    public String getCurrentFileName(int filterType);
}
