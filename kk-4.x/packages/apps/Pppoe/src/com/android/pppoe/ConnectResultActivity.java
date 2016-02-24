package com.android.pppoe;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.RemoteException;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import android.net.pppoe.PppoeInfo;
import android.net.pppoe.PppoeManager;
import android.net.pppoe.PppoeManager.OnRecvMsgListener;
import android.util.Log;

public class ConnectResultActivity extends Activity
{
    private Button mbutton;
    private BorderTextView  mIP;
    private BorderTextView  mMask;
    private BorderTextView  mGateway;
    private BorderTextView  mDns1;
    private BorderTextView  mDns2;
    
    private TextView  mState;

    private PppoeManager mpppoe;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_result);
        mbutton = (Button)findViewById(R.id.hungup);
        mIP     = (BorderTextView )findViewById(R.id.ip);
        mMask   = (BorderTextView )findViewById(R.id.mask);
        mGateway= (BorderTextView )findViewById(R.id.gateway);
        mDns1   = (BorderTextView )findViewById(R.id.dns1);
        mDns2   = (BorderTextView )findViewById(R.id.dns2);
        mState  = (TextView )findViewById(R.id.states);
        mpppoe  =  PppoeManager.getInstance();
      
        mbutton.setOnClickListener(new OnClickListener(){

            @Override
            public void onClick(View arg0) {
                mpppoe.disconnectPppoe();
            }
            
        });
        
    }
    
    protected void onStart()
    {
        super.onStart();
        Intent intent = this.getIntent();
        Bundle bundle = intent.getExtras();
        int    state = bundle.getInt("ConnectStatus");
        switch(state)
        {
            case ConnetActivity.MSG_PPPOE_CONNECT:
                mState.setText("Connect Success");
                break;
            case ConnetActivity.MSG_PPPOE_TIMEOUT:
                mState.setText("Connect TimeOut");
                break;
            case ConnetActivity.MSG_PPPOE_DISCONNECT:
                mState.setText("Disconnect");
                break;
            case ConnetActivity.MSG_PPPOE_AUTH_FAILED:
                mState.setText("AUTH_FAILED");
                break;
            case ConnetActivity.MSG_PPPOE_FAILED:
                mState.setText("Connect FAILED"); 
                break;
            default:
                break;
              
        }
        if(state == ConnetActivity.MSG_PPPOE_CONNECT)
        {
            PppoeInfo info = mpppoe.getPppoeInfo();
			Log.d("PPPOERESULT", "ip "+ info.ipaddr);
			Log.d("PPPOERESULT", "netmask "+ info.netmask);
			Log.d("PPPOERESULT", "route "+ info.route);
			Log.d("PPPOERESULT", "dns1 "+ info.dns1);
			Log.d("PPPOERESULT", "dns2 "+ info.dns2);
            mIP.setText(info.ipaddr);
            mMask.setText(info.netmask);
            mGateway.setText(info.route);
            mDns1.setText(info.dns1);
            mDns2.setText(info.dns2);
        }
        
    }
}


