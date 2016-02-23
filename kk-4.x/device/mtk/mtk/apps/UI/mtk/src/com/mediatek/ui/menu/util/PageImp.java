package com.mediatek.ui.menu.util;

import java.util.List;

import com.mediatek.ui.util.MtkLog;

public class PageImp implements PageInterface {
    
	private static final String TAG="PageImp";
	/*
	 * data source
	 */
	private List<?> list;
	/*
	 * show current data list
	 */
	private List<?> currentList;
	/*
	 * default item count of every page 
	 */
	private int perPage = 9;

	/*
	 * current page
	 */
    private int currentPage = 1;
	/*
	 * pages count
	 */
	private int pageNum = 1;
	/*
	 * records count
	 */
	private int allNum;

	/*
	 * Construction method
	 */
	public PageImp() {

	}

	/*
	 * Construction method,
	 * list source
	 * perPage items count of every page
	 */
	public PageImp(List<?> list, int perPage) {
		init(list, perPage);
	}
	/*
	 * initial
	 */
	private void init(List<?> list, int perPage) {
		this.list = list;
		if (perPage >= 1) {
			this.perPage = perPage;
		}
		if (list != null && list.size() > this.perPage) {
			this.currentList = list.subList(0, this.perPage - 1);
		}
		if (list != null) {
			this.allNum = list.size();
//			this.pageNum = (this.allNum + this.perPage - 1) / this.perPage;
		    this.pageNum =this.allNum%perPage==0?(this.allNum/perPage):(this.allNum/perPage+1);
		    if(pageNum ==0){
		    	pageNum =1;
		    }
		}
		MtkLog.d(TAG, "init()"+"perPage:"+this.perPage+"allnum:"+allNum);
	}
	/*
	 * records count
	 */
	public int getCount() {
		return this.allNum;
	}
	/*
	 * current data list
	 */
	public List<?> getCurrentList() {
		if (this.currentPage == this.pageNum) {
			if(list ==null){
				MtkLog.d(TAG, "list----is NUll");
			}else{
				this.currentList = this.list.subList((this.currentPage-1)*this.perPage, this.allNum);
			}
		}else {
			if(this.list.size()>=this.perPage*this.currentPage){
				this.currentList = this.list.subList((this.currentPage-1)*this.perPage,this.perPage*this.currentPage );
			}else
			{
				this.currentList = list;
			}
			
			
		}
//		Log.d(TAG, "currentList size"+currentList.size());
		return currentList;
	}
	/*
	 * current page
	 */
	public int getCurrentPage() {
		return this.currentPage;
	}
	/*
	 * pages count
	 */
	public int getPageNum() {
		return this.pageNum;
	}
	/*
	 * items count of page
	 */
	public int getPerPage() {
		return this.perPage;
	}
	/*
	 * skip to n page
	 */
	public void gotoPage(int n) {
		if (n > this.pageNum) {
			n = this.pageNum;
			this.currentPage = n;
		} else {
			this.currentPage = n;
		}
	}
	/*
	 * whether or not have next page
	 */
	public boolean hasNextPage() {
		this.currentPage++;
		if (this.currentPage <= this.pageNum) {
			return true;
		} else {
			this.currentPage = this.pageNum;
		}
		return false;
	}
	/*
	 * whether or not have up page
	 */
	public boolean hasPrePage() {
		this.currentPage--;
		if (this.currentPage > 0) {
			return true;
		} else {
			this.currentPage = 1;
		}
		return false;
	}
	/*
	 * head page
	 */
	public void headPage() {
		this.currentPage = 1;
	}
	/*
	 * last page
	 */
	public void lastPage() {
		this.currentPage = this.pageNum;
	}
	/*
	 * next page
	 */
	public void nextPage() {
		MtkLog.d(TAG, "nextPage");
		this.hasNextPage();
	}
	/*
	 * up page
	 */
	public void prePage() {
		MtkLog.d(TAG, "prePage");
		this.hasPrePage();
	}
	/*
	 * set items count of perpage
	 */
	public void setPerPageNum(int perPage) {
		this.perPage = perPage;
	}

}
