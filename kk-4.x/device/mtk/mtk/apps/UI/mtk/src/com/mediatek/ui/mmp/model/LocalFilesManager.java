package com.mediatek.ui.mmp.model;

import java.io.File;
import java.util.List;

import android.content.Context;

import com.mediatek.mmpcm.fileimpl.AudioFile;
import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.mmpcm.fileimpl.PhotoFile;
import com.mediatek.mmpcm.fileimpl.TextFile;
import com.mediatek.mmpcm.fileimpl.ThrDPhotoFile;
import com.mediatek.mmpcm.fileimpl.UsbFileOperater;
import com.mediatek.mmpcm.fileimpl.VideoFile;

public class LocalFilesManager extends FilesManager<FileAdapter> {
	private static final String TAG = "LocalFilesManager";

	private static LocalFilesManager sInstance;
	private UsbFileOperater mOperator;

	private LocalFilesManager(Context context) {
		super(context);

		mOperator = UsbFileOperater.getInstance();
		setSortType(SORT_BY_NAME);
	}

	public static LocalFilesManager getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new LocalFilesManager(context);
		}

		return sInstance;
	}

	@Override
	public void setSortType(int sortType) {
		switch (sortType) {
		case SORT_BY_NAME:
			mSortType = FileConst.SORT_NAME;
			break;
		case SORT_BY_DATE:
			mSortType = FileConst.SORT_DATE;
			break;
		case SORT_BY_ALBUM:
			mSortType = FileConst.SORT_ALBUM;
			break;
		case SORT_BY_ARTIST:
			mSortType = FileConst.SORT_ARTIST;
			break;
		case SORT_BY_GENRE:
			mSortType = FileConst.SORT_GENRE;
			break;
		case SORT_BY_TYPE:
			mSortType = FileConst.SORT_TYPE;
			break;
		default:
			break;
		}

	}

	@Override
	public List<FileAdapter> listAllFiles(String path) {
		File dir = new File(path);
		int filter = 0;
		switch (mContentType) {
		case CONTENT_PHOTO:
			filter = FileConst.MMP_FF_PHOTO;
			PhotoFile.openFileInfo(mContext);
			break;
		case CONTENT_AUDIO:
			filter = FileConst.MMP_FF_AUDIO;
			AudioFile.openFileInfo(mContext);
			break;
		case CONTENT_VIDEO:
			filter = FileConst.MMP_FF_VIDEO;
			VideoFile.openFileInfo(mContext);
			break;
		case CONTENT_TEXT:
			filter = FileConst.MMP_FF_TEXT;
			break;
		case CONTENT_THRDPHOTO:
			filter = FileConst.MMP_FF_THRDPHOTO;
			ThrDPhotoFile.openFileInfo(mContext);
			break;
		default:
			break;
		}

		List<MtkFile> originalFiles = mOperator.listFilterFiles(filter, dir,
				mSortType);

		mFiles = wrapFiles(originalFiles);
		logFiles(TAG);

		return mFiles;
	}

	@Override
	public List<FileAdapter> listRecursiveFiles(int contentType) {
		List<MtkFile> originalFiles = null;
		if (null == mRootPath) {
			return null;
		}
		File dir = new File(mRootPath);

		switch (contentType) {
		case CONTENT_PHOTO:
			originalFiles = mOperator.listRecursivePhoto(dir, mSortType);
			PhotoFile.openFileInfo(mContext);
			break;
		case CONTENT_AUDIO:
			originalFiles = mOperator.listRecursiveAudio(dir, mSortType);
			AudioFile.openFileInfo(mContext);
			break;
		case CONTENT_VIDEO:
			originalFiles = mOperator.listRecursiveVideo(dir, mSortType);
			VideoFile.openFileInfo(mContext);
			break;
		case CONTENT_TEXT:
			originalFiles = mOperator.listRecursiveText(dir, mSortType);
			TextFile.openFileInfo(mContext);
			break;
		case CONTENT_THRDPHOTO:
			originalFiles = mOperator.listRecursiveThrdPhoto(dir, mSortType);
			ThrDPhotoFile.openFileInfo(mContext);
			break;
		default:
			break;
		}

		mFiles = wrapFiles(originalFiles);

		return mFiles;
	}

	@Override
	protected FileAdapter newWrapFile(Object originalFile) {
		return new LocalFileAdapter((MtkFile) originalFile);
	}

	@Override
	public void destroy() {
		destroyManager();
	}

	@Override
	public void destroyManager() {
		// TODO Auto-generated method stub
		sInstance = null;
	}
}
