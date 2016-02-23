package com.mediatek.timeshift_pvr.manager;

import android.content.Context;

import com.mediatek.mmp.MtkMediaPlayer;
import com.mediatek.mmp.MtkMediaPlayer.OnCompletionListener;
import com.mediatek.mmp.MtkMediaPlayer.OnPreparedListener;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;

public class TPService implements OnPreparedListener,OnCompletionListener{
    
    private ITVCommon tvCommon;
    private TVContent mTvContent;
    private TVInputManager mTvInputManager;

    @Override
    public void onCompletion(MtkMediaPlayer arg0) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onPrepared(MtkMediaPlayer arg0) {
        // TODO Auto-generated method stub
        
    }
    
    /**
     * Resume from MMP to TV
     */
    private void resumeToTV(Context context) {
        mTvContent = TVContent.getInstance(context);
        tvCommon = TVCommonNative.getDefault(context);
        mTvInputManager = mTvContent.getInputManager();
        try {
            mTvContent.enterTV();
            if (tvCommon != null) {
                tvCommon.enterOutputMode(ITVCommon.OUTPUT_MODE_NORMAL);
            }
            TVOutput output = mTvInputManager.getOutput("main");
            if (null != output) {
                output.connect(output.getInput());
            }
        } catch (Exception e) {
            
        }
    }

}
