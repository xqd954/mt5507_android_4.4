/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * @2010-2013 TCL CORPORATION All Rights Reserved.
 */

package com.tcl.os.storage;

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;
import android.os.UserHandle;
import java.io.File;

public class TStorageVolume implements Parcelable{

private int mStorageId;
private final File mPath;
private final int mDescriptionId;
private final boolean mPrimary;
private final boolean mRemovable;
private final boolean mEmulated;
private final int mMtpReserveSpace;
private final boolean mAllowMassStorage;
/** Maximum file size for the storage, or zero for no limit */
private final long mMaxFileSize;
/** When set, indicates exclusive ownership of this volume */
private final UserHandle mOwner;


public TStorageVolume(int storageId, File path, int descriptionId, boolean primary, boolean removable,
            boolean emulated, int mtpReserveSpace, boolean allowMassStorage, long maxFileSize,
            UserHandle owner) {
	mStorageId = storageId;
	mPath = path; 
	mDescriptionId = descriptionId;
	mPrimary = primary;
	mRemovable = removable; 
	mEmulated = emulated; 
	mMtpReserveSpace = mtpReserveSpace;
	mAllowMassStorage = allowMassStorage; 
    mMaxFileSize = maxFileSize; 
	mOwner = owner; 
}

private TStorageVolume(Parcel in) {
    mStorageId = in.readInt();
    mPath = new File(in.readString());
    mDescriptionId = in.readInt();
    mPrimary = in.readInt() != 0;
    mRemovable = in.readInt() != 0;
    mEmulated = in.readInt() != 0;
    mMtpReserveSpace = in.readInt();
    mAllowMassStorage = in.readInt() != 0;
    mMaxFileSize = in.readLong();
    mOwner = in.readParcelable(null);
}

/**
     * Returns the mount path for the volume.
     *
     * @return the mount path
     */
    public String getPath() {
		return mPath.toString(); 
    }

    public File getPathFile() {
		return mPath; 
    }

    /**
     * Returns a user visible description of the volume.
     *
     * @return the volume description
     */
    public String getDescription(Context context) {
		return context.getResources().getString(mDescriptionId);
    }

    public int getDescriptionId() {
		return	mDescriptionId; 
    }

    public boolean isPrimary() {
		return mPrimary;
    }

    /**
     * Returns true if the volume is removable.
     *
     * @return is removable
     */
    public boolean isRemovable() {
		return mRemovable;
    }

    /**
     * Returns true if the volume is emulated.
     *
     * @return is removable
     */
    public boolean isEmulated() {
		return mEmulated; 
    }

    /**
     * Returns the MTP storage ID for the volume.
     * this is also used for the storage_id column in the media provider.
     *
     * @return MTP storage ID
     */
    public int getStorageId() {
		return mStorageId; 
    }

    /**
     * Number of megabytes of space to leave unallocated by MTP.
     * MTP will subtract this value from the free space it reports back
     * to the host via GetStorageInfo, and will not allow new files to
     * be added via MTP if there is less than this amount left free in the storage.
     * If MTP has dedicated storage this value should be zero, but if MTP is
     * sharing storage with the rest of the system, set this to a positive value
     * to ensure that MTP activity does not result in the storage being
     * too close to full.
     *
     * @return MTP reserve space
     */
    public int getMtpReserveSpace() {
		return mMtpReserveSpace;
    }

    /**
     * Returns true if this volume can be shared via USB mass storage.
     *
     * @return whether mass storage is allowed
     */
    public boolean allowMassStorage() {
		return mAllowMassStorage; 
    }

    /**
     * Returns maximum file size for the volume, or zero if it is unbounded.
     *
     * @return maximum file size
     */
    public long getMaxFileSize() {
		return mMaxFileSize; 
    }

    public UserHandle getOwner() {
		return mOwner; 
    }

	public static final Creator<TStorageVolume> CREATOR = new Creator<TStorageVolume>() {
        @Override
        public TStorageVolume createFromParcel(Parcel in) {
            return new TStorageVolume(in);
        }

        @Override
        public TStorageVolume[] newArray(int size) {
            return new TStorageVolume[size];
        }
    };
	
	 public int describeContents() {
        return 0;
    }
    
	public void writeToParcel(Parcel parcel, int flags) {
        parcel.writeInt(mStorageId);
        parcel.writeString(mPath.toString());
        parcel.writeInt(mDescriptionId);
        parcel.writeInt(mPrimary ? 1 : 0);
        parcel.writeInt(mRemovable ? 1 : 0);
        parcel.writeInt(mEmulated ? 1 : 0);
        parcel.writeInt(mMtpReserveSpace);
        parcel.writeInt(mAllowMassStorage ? 1 : 0);
        parcel.writeLong(mMaxFileSize);
        parcel.writeParcelable(mOwner, flags);
    }
}

