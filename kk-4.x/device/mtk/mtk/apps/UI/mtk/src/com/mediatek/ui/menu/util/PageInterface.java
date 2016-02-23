package com.mediatek.ui.menu.util;

import java.util.List;

public interface PageInterface {
	/*
	 * records count
	 */
	public int getCount();
	/*
	 * pages count
	 */
	public int getPageNum();
	/*
	 * current page
	 */
	public int getCurrentPage();
	/*
	 * set items count of perpage
	 */
	public void setPerPageNum(int prePage);
	/*
	 * next page
	 */
	public void nextPage();
	/*
	 * up page
	 */
	public void prePage();
	/*
	 * head page
	 */
	public void headPage();
	/*
	 * last page
	 */
	public void lastPage();
	/*
	 * skip to n page
	 */
	public void gotoPage(int n);
	/*
	 * items count of perpage
	 */
	public int getPerPage();
	/*
	 *  whether or not have next page
	 */
	public boolean hasNextPage();
	/*
	 *  whether or not have up page
	 */
	public boolean hasPrePage();
	/*
	 * current data list
	 */
	public List<?> getCurrentList();


}
