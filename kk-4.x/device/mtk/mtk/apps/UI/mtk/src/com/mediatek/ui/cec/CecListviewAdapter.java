package com.mediatek.ui.cec;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem.DataType;
import com.mediatek.ui.menu.commonview.OnValueChangedListener;
import com.mediatek.ui.menu.util.MenuConfigManager;

import java.util.List;

public class CecListviewAdapter extends BaseAdapter{
    
    private List<CecDataItem> mDataGroup;
    private Context mContext;
    private MenuConfigManager configManager;
    
    public CecListviewAdapter(Context mContext){
        super();
        this.mContext = mContext;
    }
    public CecListviewAdapter(Context mContext,List<CecDataItem> mDataGroup){
        super();
        this.mContext = mContext;
        this.mDataGroup = mDataGroup;
        configManager = MenuConfigManager.getInstance(mContext);
    }
    public void setDataGroup(List<CecDataItem> mDataGroup){
        this.mDataGroup = mDataGroup;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return mDataGroup == null?0:mDataGroup.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return mDataGroup==null?null:mDataGroup.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return mDataGroup==null?0:position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder = new ViewHolder();
        if (mDataGroup!=null && mDataGroup.size()>0) {
            CecDataItem dataItem = mDataGroup.get(position);
            if (dataItem.getDataType()==CecDataItem.DataType.OPTIONVIEW) {
                if (holder.optionView==null) {
                    holder.optionView = new CecOptionView(mContext);
                }
                holder.optionView.setAdapter(dataItem);
                holder.optionView.setmId(dataItem.mItemID);
                holder.optionView.setValueChangedListener(new OnValueChangedListener() {
                    public void onValueChanged(View v, int value) {
                        CecOptionView view = (CecOptionView)v;
                        String mId = view.getmId();
                        configManager.setValue(mId, value+ configManager.getMin(mId));
                    }
                });
                return holder.optionView;
            }
            if (dataItem.getDataType()==CecDataItem.DataType.CecMainItemView) {
                if (holder.mainItemView==null) {
                    holder.mainItemView = new CecMainItemView(mContext);
                }
                holder.mainItemView.setAdapter(dataItem);
                holder.mainItemView.setmId(dataItem.mItemID);
                return holder.mainItemView;
            }
        }
        return convertView;
    }
    
    static class ViewHolder {
        CecOptionView optionView;
        CecMainItemView mainItemView;
    }
    
    public static class CecDataItem{
        public int userDefined;
        public String mItemID;
        public String mName;
        public String[] mOptionValue;
        public int mInitValue;
        private volatile DataType mDataType = DataType.OPTIONVIEW;
        private int deviceType = 0;
        
        public enum DataType {
            OPTIONVIEW,CecMainItemView, CHANNELLISTVIEW, TEXTCOMMVIEW
        }
        public CecDataItem(String mItemID, String mName, int mInitValue,
                String[] mOptionVaule, DataType mDataType){
            this.mItemID = mItemID;
            this.mName = mName;
            this.mDataType = mDataType;
            this.mInitValue = mInitValue;
            if (mDataType==DataType.OPTIONVIEW) {
                this.mOptionValue = mOptionVaule;
            }
            
        }
        
        public String getName (){
            return mName;
        }
        public DataType getDataType(){
            return mDataType;
        }
        
        public int getInitValue(){
            return mInitValue;
        }
        public void setInitValue(int value){
            this.mInitValue = value;
        }
        
        public void setDeviceType(int value){
            deviceType = value;
        }
        public int getDeviceType(){
            return deviceType;
        }
        public String[] getmOptionValue() {
            return mOptionValue;
        }

        public void setmOptionValue(String[] mOptionValue) {
            this.mOptionValue = mOptionValue;
        }
    } 

}
