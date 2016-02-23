package com.mediatek.ui.mmp.model;

public class TextPage {
	//start line
	private int startLine;
	//content
	private String content;
	
	public TextPage(int startPage, String content) {
		super();
		this.startLine = startPage;
		this.content = content;
	}
	public int getStartPage() {
		return startLine;
	}
	public void setStartPage(int startPage) {
		this.startLine = startPage;
	}
	public String getContent() {
		return content;
	}
	public void setContent(String content) {
		this.content = content;
	}
	@Override
	public String toString() {
		return "Page [startLine=" + startLine + ", content=" + content + "]";
	}
	
	
	
	
}
