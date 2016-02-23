package com.mediatek.ui.mmp.util;

import java.util.ArrayList;

public class MenuFatherObject {
	public String content;
	public boolean enable;
	public boolean right;
	public boolean enter;
	public boolean hasnext;
	public ArrayList<MenuChildObject> menu;

	@Override
	public boolean equals(Object o) {
		return content.equals(o.toString());
	}

     @Override
	 public int hashCode(){
       return super.hashCode();
	 }

	
}
