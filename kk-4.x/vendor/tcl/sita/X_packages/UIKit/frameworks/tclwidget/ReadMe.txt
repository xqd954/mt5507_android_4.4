
1. frameworks/base/core/java/android/widget/ProgressBar.java
	增加函数
	_SetTclStyle(Context context , TypedArray a);

2. 应用程序修改说明
	当没有设置style，默认为TCLProgressBar.TclStyle_Circle
	只有用到	style为如下的需要修改 
		?android:attr/tclProgressBarStyleHorizontal
		?android:attr/tclProgressBarStyleCircle
		?android:attr/tclProgressBarStyleHorizontalWhite
	a. xml布局修改
		<ProgressBar => 修改为android.tclwidget.TCLProgressBar
			android:id="@+id/progressbar2"
			android:layout_width="wrap_content" 
			android:layout_height="wrap_content" 
			style="?android:attr/tclProgressBarStyleHorizontal" => 删除 
		/>
	b. 代码修改
		import android.tclwidget.TCLProgressBar;
		... ...
		dl1 = (TCLProgressBar) findViewById(R.id.tclprogressbar); 
        dl1.SetTclStyle(this,TCLProgressBar.TclStyle_Circle); => 增加
		对应的类型为：
			?android:attr/tclProgressBarStyleCircle => TCLProgressBar.TclStyle_Circle
			?android:attr/tclProgressBarStyleHorizontal => TCLProgressBar.TclStyle_Horizontal
			?android:attr/tclProgressBarStyleHorizontalWhite => TCLProgressBar.TclStyle_HorizontalWhite
