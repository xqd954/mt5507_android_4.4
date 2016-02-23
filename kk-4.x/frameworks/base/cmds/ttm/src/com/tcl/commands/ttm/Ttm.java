/*
**
** Copyright 2015, TCL 
**
*/


package com.tcl.commands.ttm;

import android.app.ActivityThread;
import android.content.Context;
import android.util.AndroidException;

import android.os.Looper;

import com.tcl.tvmanager.TTvSoundManager;

public class Ttm {

    private boolean ismute;

    /**
     * Command-line entry point.
     *
     * @param args The command-line arguments
     */
    public static void main(String[] args) {
        try {
            (new Ttm()).run(args);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void showUsage() {
        System.out.println(
                "usage: ttm [subcommand] [options]\n" +
                "       ttm mutetv\n" +
                "       ttm unmutetv"
                );
    }

    public void run(String[] args) throws Exception {
        Looper.prepareMainLooper();

        ActivityThread at = ActivityThread.systemMain();
        Context context = at.getSystemContext();

        if (context == null) {
            System.err.println("context is null");
            throw new AndroidException("context is null");
        }

        if (args.length == 1 && args[0].equals("mutetv")) {
            mutetv(context);
        } else if (args.length == 1 && args[0].equals("unmutetv")) {
            unmutetv(context);
        } else {
            showUsage();
        }

        //Looper.loop();
    }

    private void mutetv(Context context) {
        TTvSoundManager tsm = TTvSoundManager.getInstance(context);
        if (tsm == null) {
            System.err.println("TSM is null");
            return ;
        }
        tsm.setAudioMuteEnabled(true);

        System.out.println("AudioMuteEnabled: " + tsm.getAudioMuteEnabled());
    } 

    private void unmutetv(Context context) {
        TTvSoundManager tsm = TTvSoundManager.getInstance(context);
        if (tsm == null) {
            System.err.println("TSM is null");
            return ;
        }
        tsm.setAudioMuteEnabled(false);

        System.out.println("AudioMuteEnabled: " + tsm.getAudioMuteEnabled());
 
    }

}
