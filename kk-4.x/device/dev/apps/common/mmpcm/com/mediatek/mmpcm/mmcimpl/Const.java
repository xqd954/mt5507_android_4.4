package com.mediatek.mmpcm.mmcimpl;

import com.mediatek.mmpcm.fileimpl.FileConst;
public class Const {

    public static final int FILTER_VIDEO = 1;
    public static final int FILTER_IMAGE = 2;
    public static final int FILTER_AUDIO = 3;
    public static final int FILTER_TEXT = 4;
    public static final int FILTER_THRDPHOTO = 5;

    public static final int FILE_TYPE_USB = FileConst.SRC_USB;
    public static final int FILE_TYPE_SMB = FileConst.SRC_SMB;
    public static final int FILE_TYPE_DLNA = FileConst.SRC_DLNA;

    public static final int REPEAT_NONE = 0;// repeat all file in current folder
    public static final int REPEAT_ONE = 1;// repeat one file
    public static final int REPEAT_ALL= 2;// play once
    public static final int REPEAT_MARKED = 3;// play marke file,only support
                                              // audio and photo file

    public static final boolean SHUFFLE_OFF = false;// shuffle mode off
    public static final boolean SHUFFLE_ON = true;// shuffle mode on

	public static final int CURRENTPLAY = 2; // auto next

    public static final int AUTOPLAY = 0; // auto next
    public static final int MANUALPRE = -1; // manual change to prev file
    public static final int MANUALNEXT = 1;// manual change to next file
    public static int SCREENMODE_FILELIST=1;//manual videoPlay back to filelist
    public static final int ORIENTATION_ARRAY[][]={ {0,0,0},   // invalid 0
    												{0,0,0},   //nomal    1
				    								{0,180,0}, //left - right  5
				    								{0,0,-180}, //180     3
				    								{0,180,180}, //l-r 180    7
				    								{0,180,90},//l-r 90   6
				    								{0,0,-90},  //90       2
				    								{0,180,270},//l-r 270  8
				    								{0,0,-270}}; //270    4
    
    
    public static final int ORIENTATION_NEXT_ARRAY[] = {0,1,5,3,7,6,2,8,4};

	public static final int ORIENTATION_ROTATE[] = {0,0,-90};

	public static final int ORIENTATION_NEXT_ROTATE_ARRAY[][] = {	{0,0,0},
																	{0,0,-90},
																	{0,0,-90},
																	{0,0,-90},
																	{0,180,-90},
																	{0,0,-90},
																	{0,0,-90},
																	{0,0,-90},
																	{0,180,-90}};
}
