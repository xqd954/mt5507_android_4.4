/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * @2010-2013 TCL CORPORATION All Rights Reserved.
 */

package com.tcl.os.storage;

public abstract class TStorageEventListener {

    /**
     * Called when the detection state of a USB Mass Storage host has changed.
     * @param connected true if the USB mass storage is connected.
     */
    public void onUsbMassStorageConnectionChanged(boolean connected) {
    }

    /**
     * Called when storage has changed state
     * @param path the filesystem path for the storage
     * @param oldState the old state as returned by {@link com.tcl.os.storage.TStorageManager.getVolumeState(String path)}.
     * @param newState the old state as returned by {@link com.tcl.os.storage.TStorageManager.getVolumeState(String path)}.
     */
    public void onStorageStateChanged(String path, String oldState, String newState) {
    }
}
