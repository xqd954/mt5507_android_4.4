package android.view;


import android.graphics.Rect;
import android.util.Log;

import java.lang.ref.WeakReference;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * User: mtk40058
 * Date: 11/7/11
 * Time: 1:01 PM
 */

/** @hide */
public class HolderSourceManager {
    private HashMap<WeakReference<Object> , WeakReference<Object> > mMaps = new HashMap<WeakReference<Object>, WeakReference<Object>>();
    //private static final String TAG = "HolderSourceManager";

    private static HolderSourceManager manager;

    public synchronized static HolderSourceManager getInstance() {
        if(manager == null) {
            manager = new HolderSourceManager();
        }
        return manager;
    }

    public void add(Object key, Object value) {
        //Log.v(TAG, "Add Object " + value + " by key " + key);
        updateMap();
        mMaps.put(new WeakReference<Object>(key), new WeakReference<Object>(value));
    }

    private void updateMap() {
        ArrayList<WeakReference<Object>> list = new ArrayList<WeakReference<Object>>();
        for (WeakReference<Object> objectWeakReference : mMaps.keySet()) {
            if(objectWeakReference.get() == null) {
                list.add(objectWeakReference);
            }
        }
        for (Object o : list) {
            mMaps.remove(o) ;
        }
        list.clear();


        for (Map.Entry<WeakReference<Object>, WeakReference<Object>> entry : mMaps.entrySet()) {
            if(entry.getValue().get() == null) {
                list.add(entry.getKey()) ;
            }
        }
        for (Object o : list) {
            mMaps.remove(o) ;
        }

        list.clear();
    }

    public Rect get(Object key) {
        //Log.v(TAG, "Get by key " + key);
        updateMap();
        Rect rect = new Rect(0,0,0,0);

        WeakReference<Object> reference = null;
        for (WeakReference<Object> objectWeakReference : mMaps.keySet()) {
            Object wr = objectWeakReference.get();
            if((wr != null) && wr.equals(key)) {
                reference = mMaps.get(objectWeakReference);
                break;
            }
        }
        if(reference == null) {
            //throw new RuntimeException("Not Found");
            return rect;
        }
        Object obj = reference.get();
        if(obj == null) {
            //throw new RuntimeException("Object has been GC");
            return rect;
        }
        //Log.v(TAG, "Get Object is " + obj);
        if((obj instanceof SurfaceView) || (obj instanceof ViewRootImpl)) {
            View v = (View) obj;
            int[] pos = new int[2];
            v.getLocationOnScreen(pos);

            rect.left = pos[0];
            rect.top  = pos[1];
            rect.right = rect.left + v.getMeasuredWidth();
            rect.bottom = rect.top + v.getMeasuredHeight();

            //Log.v(TAG, "Get display rect " + rect);

        } else {
            //throw new RuntimeException("Not Support");
        }
        return rect;
    }
}
