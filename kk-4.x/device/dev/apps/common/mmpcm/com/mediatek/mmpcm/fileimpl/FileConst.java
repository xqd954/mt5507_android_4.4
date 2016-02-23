package com.mediatek.mmpcm.fileimpl;

public class FileConst {

    public static final int SMALL = 1;
    public static final int MEDIUM = 2;
    public static final int LARGE = 3;

    public static final int SORT_NAME = 1;
    public static final int SORT_DATE = 2;
    public static final int SORT_GENRE = 3;
    public static final int SORT_ARTIST = 4;
    public static final int SORT_ALBUM = 5;
    public static final int SORT_TYPE = 6;

    public static final int FILE_TYPE_UNKNOW = 0;
    public static final int FILE_TYPE_DIRECTORY = 1;
    public static final int FILE_TYPE_VIDEO = 2;
    public static final int FILE_TYPE_AUDIO = 3;
    public static final int FILE_TYPE_IMAGE = 4;
    public static final int FILE_TYPE_TEXT = 5;
    public static final int FILE_TYPE_DEVICE = 6;

	public static final int SRC_ALL = 0;
    public static final int SRC_USB = 1;
	public static final int SRC_DLNA = 2;
    public static final int SRC_SMB = 3;
    public static final int SRC_HTTP = 4;

    public static final int MMP_FF_ALL = 0;
    public static final int MMP_FF_VIDEO = 1;
    public static final int MMP_FF_PHOTO = 2;
    public static final int MMP_FF_AUDIO = 3;
    public static final int MMP_FF_TEXT = 4;
	/*
	* add
	*/
	public static final int MMP_FF_ISO = 5;
	public static final int MMP_FF_ISOVIDEO = 6;
	public static final int MMP_FF_THRDPHOTO = 7; //by lei add
	   

    public static final int RECURSIVE_PARSE_COUNT_PER_TIIME = 100;

    public static final int MSG_DEVICE_PARSED = 1;
    public static final int MSG_FILE_PARSEING = 2;
    public static final int MSG_FILE_PARSED = 3;
    public static final int MSG_FILE_EMPTY = 4;
    public static final int MSG_FILE_REMOVE = 5;
    public static final int MSG_SMB_NEED_AUTH = 6;
    public static final int MSG_SMB_UNKNOW_HOST = 7;

	public static final long MAX_PHOTO_SIZE = 100*1024*1024;
    public static final String photoSuffix[] = { ".png", ".bmp", ".jpg",
            ".PNG", ".jpeg", ".gif",".webp"};
    public static final String thrdPhotoSuffix[] = {".mpo", ".jps",".pns" };
    public static final String textSuffix[] = { ".txt" };
    public static final String audioSuffix[] = { ".mp3", ".wma", ".m3u",
            ".m3u8", ".wav", ".aif", ".m4a", ".aac", ".ac3", ".ec3", ".pls",
            ".wpl", ".ogg", ".mp2", ".ra" ,".flac",".ape"};
    public static final String videoSuffix[] = { ".rmvb", ".avi", ".mkv",
            ".mp4", ".3gp", ".flv", ".mpg", ".ts", ".wmv", ".vob", ".rm",
            ".mov", ".avs", ".divx", ".asf", ".mpe", ".mpeg", ".dat", ".asx",
            ".m4v", ".tp", ".trp", ".tts", ".m2ts", ".mts", ".m1v", ".m2v",
            ".m4v", ".264", ".vc1", ".flv", ".hlv", ".pvr", ".ogm", ".webm",
            ".ram",".iso",".ssif",".264",".265"};
    public static final String isovideoSuffix[] = {".ssif"};
}
