package com.mediatek.ui.util;

import java.util.List;

public interface PageInterface {

	public int getCount();

	public int getPageNum();

	public int getCurrentPage();

	public void setPerPageNum(int prePage);

	public void nextPage();

	public void prePage();

	public void headPage();

	public void lastPage();

	public void gotoPage(int n);

	public int getPerPage();

	public boolean hasNextPage();

	public boolean hasPrePage();

	public List<?> getCurrentList();

}
