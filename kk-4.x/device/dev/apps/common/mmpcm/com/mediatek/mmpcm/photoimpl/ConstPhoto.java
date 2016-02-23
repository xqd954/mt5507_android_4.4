package com.mediatek.mmpcm.photoimpl;
import com.mediatek.mmpcm.fileimpl.FileConst;

public class ConstPhoto {


	public static final int LEFT_TO_RIGHT = 1;	
	public static final int RIGHT_TO_LEFT = 2;
	public static final int TOP_TO_BOTTOM = 3;
	public static final int BOTTOM_TO_TOP = 4;
	public static final int LEFTTOP_TO_RIGHTDOWN = 5;
	public static final int LEFTDOWN_TO_RIGHTTOP = 6;
	
	public static final int LEFT_TO_RIGHT_OUT = 31;	
	public static final int RIGHT_TO_LEFT_OUT = 32;
	public static final int TOP_TO_BOTTOM_OUT = 33;
	public static final int BOTTOM_TO_TOP_OUT = 34;
	public static final int LEFTTOP_TO_RIGHTDOWN_OUT = 35;
	public static final int LEFTDOWN_TO_RIGHTTOP_OUT = 36;
	public static final int side_to_mid_out = 37;
	public static final int mid_to_side_out = 38;
	
	public static final int EFFECT_START = 9;
	public static final int EFFECT_END = 15;		
	
	public static final int DEFAULT = 0;
	
	public static final int side_to_mid = 7;
	public static final int mid_to_side = 8;
	
	public static final int dissolve = 9;
	public static final int wipe_right = 10;
	public static final int wipe_left = 11;
	public static final int wipe_top = 12;
	public static final int wipe_bottom = 13;
	public static final int box_in = 14;
	public static final int box_out = 15;
	public static final int RADNOM = 16;
	public static final int fragment = 40;
	public static final int GIF_ANIM =100;

	/*"ZOOMOUT" and "ROTATE_R" are used in EffectView.java*/
	public static final int ZOOMOUT = 21;
	public static final int ZOOMIN = 22;
	public static final int ROTATE_R = 23;
	//add by shuming for Fix CR DTV00401969
	public static final int ROTATE_PHOTO = 27;
	public static final int DRAWBLACK = 24;
	
	public static final int ZOOM_OUT = 0;
	public static final int ZOOM_IN = 1;
	public static final int ZOOM_MAXMULT = 8;
	public static final int MOVE_DIST = 5;
	
	public static final int LOCAL = FileConst.SRC_USB;
	public static final int SAMBA = FileConst.SRC_SMB;
	public static final int DLNA = FileConst.SRC_DLNA;
	public static final int URL = FileConst.SRC_HTTP;
	
	public static final String TempFolderPath = "/tmp";
	
	public static final int DECODEFAILURE = 1;
	
	public static final int ZOOM_1X = 1;
	public static final int ZOOM_2X = 2;
	public static final int ZOOM_4X = 4;
	
	public static final int UNCONSTRAINED = -1;
	public static final int MAX_NUMOF_PIXS = 3840*2160;
	public static final int MAX_4K2K_WIDTH = 3840;
	public static final int MAX_4K2K_HEIGHT = 2160;
	
}
