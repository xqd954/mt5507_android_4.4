package android.tclwidget;


import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

/**ScrollBar类。
 * 竖型的进度条，使用setMax()方法来设置总进度。setProgress()方法来设置当前进度。*/
public class TCLSliderBar extends View {
	private Context mContext;
	
	Drawable mdrawbale, mbgdrawbale, mbthumbrawbale;
	int width, height;
	private int mMax = 100,mlinewidth=18;
	private int mProgress = 0;
	private String TAG = "ScrollBar";
	
	public TCLSliderBar(Context context) {
		super(context);
		mContext = context;
		init();
	}

	public TCLSliderBar(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		init();
	}

	public TCLSliderBar(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mContext = context;
		init();
	}
	
	private void init() {

		mbgdrawbale = mContext.getResources().getDrawable(com.android.internal.R.drawable.tcl_slider_bg);
		mbthumbrawbale = mContext.getResources().getDrawable(com.android.internal.R.drawable.tcl_slider_thumb);
	}
	
	/**设置进度条范围：0～max*/
	public void setMax(int max) {
		if (max > 0)
			mMax = max;
		
	}
	
	/**设置滑动条宽度：0～max,建议18～22*/
	public void setSliderWidth(int width) {
		if ((width>=18)/*&& (width<=22)*/)
			mlinewidth = width;
		
	}

	public void setProgress(int progress) {
		if (progress > mMax) 
			mProgress = mMax;
		else
			mProgress = progress;
		invalidate();
	}
	
	public int getMax() {
		return mMax;
	}

	public int getSliderWidth() {
		return mlinewidth;
	}
	
	
	public int getProgress() {
		return mProgress;
	}
	
	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		// TODO Auto-generated method stub
		int heightSpec = MeasureSpec.getSize(heightMeasureSpec);
		int heightMode = MeasureSpec.getMode(heightMeasureSpec);
		
		width = 40;
		if (heightMode == MeasureSpec.EXACTLY)
			height = heightSpec;
		else
			height = 100;
		setMeasuredDimension(width, height);
		
	}

	@Override
	protected void onDraw(Canvas canvas) {
		int mid = width/2;
		int top = 0;
		int scrollbar = 100;
		int progressHeight = ((height-top-scrollbar)*mProgress)/mMax;
		
		Log.d(TAG, String.format("--> %d, %d", mMax, mProgress));
		
		mbgdrawbale.setBounds(mid - mlinewidth/2, top,mid + mlinewidth/2, height);
		mbgdrawbale.draw(canvas);		
	//	mdrawbale.setBounds(mid - linewidth/2, top, mid + linewidth/2, progressHeight + top);
	//	mdrawbale.draw(canvas);
		mbthumbrawbale.setBounds(mid - mlinewidth/2,  progressHeight+top, mid + mlinewidth/2,  progressHeight+top+scrollbar);
		mbthumbrawbale.draw(canvas);
	}
}

