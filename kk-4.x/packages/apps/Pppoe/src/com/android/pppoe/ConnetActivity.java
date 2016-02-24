
package com.android.pppoe;

import android.app.Activity;
import android.content.Intent;
import android.net.pppoe.PppoeManager;
import android.net.pppoe.PppoeManager.OnRecvMsgListener;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ProgressBar;

import java.util.Timer;
import java.util.TimerTask;

public class ConnetActivity extends Activity
{
    private static final String TAG_PPOE = "PPPOE";
    public static final int MSG_TIME_PROGRESS     = 0;
    public static final int MSG_PPPOE_CONNECT     = 1;
    public static final int MSG_PPPOE_TIMEOUT     = 2;
    public static final int MSG_PPPOE_DISCONNECT  = 3;
    public static final int  MSG_PPPOE_AUTH_FAILED = 4;    
    public static final int  MSG_PPPOE_FAILED       = 5;
    public static final int  MSG_PPPOE_CONNECTING   = 6;
    
    private ProgressBar mProgress;
    private int mProgressStatus = 0;
    private String mAccount;
    private String mPassword;
    private int mDeviceType;
    private PppoeManager mpppoe;

    private Handler mHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Bundle bundle = new Bundle();
            if(msg.what != MSG_TIME_PROGRESS)
                Log.d(TAG_PPOE, "handleMessage "+ msg.what);
            switch (msg.what) {
                case MSG_PPPOE_CONNECT:               
                case MSG_PPPOE_TIMEOUT:               
                case MSG_PPPOE_AUTH_FAILED:               
                case MSG_PPPOE_FAILED:               
                //case MSG_PPPOE_CONNECTING:               
                case MSG_PPPOE_DISCONNECT:
                {           
                    mProgressStatus = 100; // stop the process 
                    bundle.putInt("ConnectStatus", msg.what);
                    doResult(bundle);
                }
                    break;
                case MSG_TIME_PROGRESS:
                    mProgressStatus++;
//                    Log.d(TAG_PPOE, "Time : "+ mProgressStatus);
                    if(mProgressStatus >= 100)
                    {
                        Log.d(TAG_PPOE, "TimeOut ");
                        sendEmptyMessage(MSG_PPPOE_TIMEOUT);
                        break;
                    }
                    sendEmptyMessageDelayed(MSG_TIME_PROGRESS, 1000);
                    mProgress.setProgress(mProgressStatus);
                    break;
                default:
                    break;
            }
         

        }
    };
    
    private void doResult(Bundle bundle)
    {
        mHandler.removeMessages(MSG_TIME_PROGRESS);
        Intent intent = new Intent();
        intent.putExtras(bundle);
        intent.setClass(ConnetActivity.this, ConnectResultActivity.class);
        startActivity(intent);
        finish();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connet);
        mProgress = (ProgressBar) findViewById(R.id.progressBar1);
        mpppoe = PppoeManager.getInstance();
    }

    protected void onStart()
    {
        super.onStart();
        startPPPOE();
        mProgress.setProgress(0);
        mpppoe.RegisterListener(new OnRecvMsgListener() {

            @Override
            public void onRecvMsg(int arg0) {
                // TODO Auto-generated method stub
                // if pppoe success noticefy mhandle
                switch (arg0) {
                    case PppoeManager.MSG_PPPOE_CONNECT:
                        mHandler.sendEmptyMessage(MSG_PPPOE_CONNECT);
                        break;
                    case PppoeManager.MSG_PPPOE_TIME_OUT:
                        mHandler.sendEmptyMessage(MSG_PPPOE_TIMEOUT);
                        break;
                    case PppoeManager.MSG_PPPOE_AUTH_FAILED:
                        mHandler.sendEmptyMessage(MSG_PPPOE_AUTH_FAILED);
                        break;
                    case PppoeManager.MSG_PPPOE_DISCONNECT:
                        mHandler.sendEmptyMessage(MSG_PPPOE_DISCONNECT);
                        break;
                    case PppoeManager.MSG_PPPOE_FAILED:
                        mHandler.sendEmptyMessage(MSG_PPPOE_FAILED);
                        break;
                    case PppoeManager.MSG_PPPOE_CONNECTING:
                        mHandler.sendEmptyMessage(MSG_PPPOE_CONNECTING);
                        break;
                    default:
                        break;
                }              
            }

        });
        mHandler.sendEmptyMessage(MSG_TIME_PROGRESS);
        // New Thread run timer
//        Thread time = new Thread(new Runnable()
//        {
//            @Override
//            public void run() {
//                // TODO Auto-generated method stub
//                try {
//                    while (mProgressStatus < 100)
//                    {                       
//                        Thread.sleep(1000);                       
//                        mHandler.sendEmptyMessage(MSG_TIME_PROGRESS);
//                        mProgressStatus++;
//                    }
//                } catch (Exception e)
//                {
//                    // TODO
//                    e.printStackTrace();
//                }
//            }
//
//        });
//        time.start();
    }

    private void GetData()
    {
        Bundle bundle;
        Intent intent = this.getIntent();
        bundle = intent.getExtras();
        mAccount = bundle.getString("username");
        mPassword = bundle.getString("password");
        if (bundle.get("device").equals("eth0"))
        {
            mDeviceType = 0;
        } else if (bundle.get("device").equals("wlan0"))
        {
            mDeviceType = 1;
        }
    }

    private int startPPPOE()
    {
        // TODO start connect here
        GetData();
        Log.d(TAG_PPOE, "account:"+mAccount + "   password:"+mPassword);
        mpppoe.connectPppoe(mAccount, mPassword, true);
        return 0;
    }
}
