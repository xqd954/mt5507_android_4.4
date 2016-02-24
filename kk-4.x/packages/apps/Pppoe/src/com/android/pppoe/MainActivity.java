
package com.android.pppoe;

import android.os.Bundle;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

@SuppressLint("NewApi")
public class MainActivity extends Activity {

    private Button mDial;
    private Spinner mspinner;
    private EditText mUsername;
    private EditText mPassword;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mDial = (Button)findViewById(R.id.dial);
        mspinner = (Spinner)findViewById(R.id.spinner);
        mUsername = (EditText)findViewById(R.id.enteraccount);
        mPassword = (EditText)findViewById(R.id.Editpassword);
        
        //getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
        
        //set spinner
        try {
            setDevice();
        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
        mDial.setOnClickListener(new OnClickListener(){
            
            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                //get the account
                String straccount = (String)mUsername.getText().toString();
                //get password
                String strpassword = (String)mPassword.getText().toString();
                
                String strdevice = (String)mspinner.getSelectedItem().toString();
                
                if(straccount.isEmpty() || strpassword.isEmpty() || strdevice.isEmpty())
                {
                    Toast.makeText(getApplicationContext(), "Enter is empty", Toast.LENGTH_LONG).show();
                    return;
                }
                
                Bundle bundle = new Bundle();
                bundle.putString("username", straccount);
                bundle.putString("password", strpassword);
                bundle.putString("device", strdevice);
                               
                
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, ConnetActivity.class);
                intent.putExtras(bundle);
                startActivity(intent);
            }
            
        });
        
        mUsername.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            
            @Override
            public boolean onEditorAction(TextView arg0, int arg1, KeyEvent arg2) {
                // TODO Auto-generated method stub
                if(arg0.equals(mUsername))
                {
                    if(arg1 == EditorInfo.IME_ACTION_DONE)
                    {
                        
                    }
                }
                return false;
            }
        });
        
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    private void setDevice() throws SocketException
    {
        //ArrayAdapter<CharSequence> adapt = ArrayAdapter.createFromResource(this,
          //                  R.array.devicesitem, android.R.layout.simple_spinner_item);
        //adapt.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);
        ArrayAdapter<String> adapt = new ArrayAdapter<String>(this, android.R.layout.simple_dropdown_item_1line);
        Enumeration<NetworkInterface> ei = NetworkInterface.getNetworkInterfaces();
        
        while(ei.hasMoreElements())
        {            
            String str = ei.nextElement().toString();
            int x = str.indexOf("[");
            int y = str.indexOf("]");
            String s1 = str.substring(x+1, y);
            adapt.add(s1);
            Log.d("PPPOE", str);
            Log.d("PPPOE", s1);
        }
        
        //adapt.add("eth0");
        //adapt.add("other");
        mspinner.setAdapter(adapt);
    }
    

}
