package com.mediatek.mmpcm.fileimpl;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class FileList {
    private int dirNum;
    private int totalNum;
    private List<MtkFile> fileList = new ArrayList<MtkFile>();

    public void addList(List<MtkFile> fl) {
        this.fileList.addAll(0,fl);
        this.totalNum = this.fileList.size();
    }

    public void addArray(MtkFile[] fl) {        
        //this.fileList.addAll(0,Arrays.asList(fl));
        int i;
        for(i = totalNum; i < fl.length; i ++){
            this.fileList.add(i,fl[i]);
        }
        
        this.totalNum = this.fileList.size();
    }

    /**
     * used for filebrowser
     */
    public void cleanList() {
        this.fileList.clear();
        this.dirNum = 0;
        this.totalNum = 0;
    }

    /**
     * used for UI layer
     * 
     * @return
     */
    public int getDirNum() {
        return this.dirNum;
    }

    /**
     * used for UI layer
     * 
     * @return
     */
    public List<MtkFile> getList() {
        return this.fileList;
    }
}
