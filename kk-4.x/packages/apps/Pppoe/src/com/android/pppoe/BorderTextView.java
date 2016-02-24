package com.android.pppoe;


import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.widget.TextView;


public class BorderTextView extends TextView
{
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        Paint p = new Paint();
        p.setColor(android.graphics.Color.BLACK);
        canvas.drawLine(0, 0, this.getWidth()-1, 0, p);
        canvas.drawLine(0, 0, 0, this.getHeight()-1, p);
        canvas.drawLine(this.getWidth()-1, 0, this.getWidth()-1, this.getHeight()-1, p);
        canvas.drawLine(0, this.getHeight()-1, this.getWidth()-1, this.getHeight()-1, p);
    }

    public BorderTextView(Context context) {
        super(context);
        // TODO Auto-generated constructor stub
    }
    
    public BorderTextView(Context context, AttributeSet arrts)
    {
        super(context, arrts);
        
    }
    
    public BorderTextView(Context context, AttributeSet arrts, int defStyle)
    {
        super(context, arrts, defStyle);
       // arrts.getAttributeIntValue("my", arg1, arg2)
    }
    
}