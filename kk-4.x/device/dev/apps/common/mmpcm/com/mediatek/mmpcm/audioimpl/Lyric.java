package com.mediatek.mmpcm.audioimpl;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.Collections;
import java.util.Vector;

import com.mediatek.mmpcm.audio.ILyric;

public class Lyric implements ILyric {

	private String lyricAlbum;
	private String lyricArtist;
	private String lyricEditor;
	private String lyricTitle;
	private long lyricTimeOffset;
	private Vector<LyricTimeContentInfo> lyricTimeContentInfo;
   /**
    * Construction method
    * @param path
    */
	public Lyric(String path) {
		boolean result = false;

		lyricAlbum = null;
		lyricArtist = null;
		lyricEditor = null;
		lyricTitle = null;
		lyricTimeOffset = 0;
		lyricTimeContentInfo = null;

		result = parseLyricFile(path);
		if (result == false) {
			lyricAlbum = null;
			lyricArtist = null;
			lyricEditor = null;
			lyricTitle = null;
			lyricTimeOffset = 0;
			lyricTimeContentInfo = null;
		}
	}

	private int parser(String str, Vector<String> lyricContent,
			Vector<Long> lyricTime, int result) {
		String subStr = null;
		int bPos = 0;
		int mPos = 0;
		int ePos = 0;
		int index = 0;
		int count = 0;
		int step = 0;

		str = str.trim();
		while (str.length() != 0) {
			bPos = str.indexOf("[", index);
			mPos = str.indexOf(":", index);
			ePos = str.indexOf("]", index);

			if (bPos != -1 && bPos < mPos && mPos < ePos) {
				step += ePos - bPos + 1;
				subStr = str.substring(bPos + 1, mPos);
				if (subStr.equals("ti")) {
					lyricTitle = str.substring(mPos + 1, ePos);
				} else if (subStr.equals("ar")) {
					lyricArtist = str.substring(mPos + 1, ePos);
				} else if (subStr.equals("al")) {
					lyricAlbum = str.substring(mPos + 1, ePos);
				} else if (subStr.equals("by")) {
					lyricEditor = str.substring(mPos + 1, ePos);
				} else if (subStr.equals("offset")) {
					lyricTimeOffset = Long.parseLong(str.substring(
							mPos + 1, ePos));
				} else {
					long time = 0;
					long min = 0;
					long sec = 0;
					long msec = 0;
					int pos = 0;

					try {
						min = Long.parseLong(subStr);
						subStr = str.substring(mPos + 1, ePos);

						pos = subStr.indexOf(".");
						if (pos > 0) {
							sec = Long.parseLong(subStr.substring(0, 2));
							msec = Long.parseLong(subStr.substring(pos + 1,
									pos + 3));
						} else {
							sec = Long.parseLong(subStr.substring(0, 2));
						}

						time = (min * 60 + sec) * 1000 + msec + lyricTimeOffset;
						lyricTime.addElement(time);
						count++;

					} catch (NumberFormatException ex) {
						return -1;
					}
				}

				index = ePos + 1;
			} else {
				if (step > 0) {
					break;
				} 
				else {
					if (result > 0) {
						int size = lyricContent.size();
						String temp = lyricContent.lastElement() + str;
						index = size - result;
						
						for (int i = 0; i < result; i++) {
							lyricContent.set(index + i, temp);
						}
						
						return result;
					}
					else {
						return -1;
					}
										
				}

			}
		}

		if (count > 0) {
			if (str.length() > step) {
				subStr = str.substring(step, str.length());
			} else {
				subStr = "";
			}

			for (int i = 0; i < count; i++) {
				lyricContent.addElement(subStr);
			}
		}

		return count;
	}
	
	private String getCharset(File file) {
		String charset = "GBK";
		byte[] first3Bytes = new byte[3];
		FileInputStream fis = null;
		try {
			boolean checked = false;
			fis = new FileInputStream(file);
			BufferedInputStream bis = new BufferedInputStream(fis);
			bis.mark(1);
			int read = bis.read(first3Bytes, 0, 3);
			if (read == -1)
				return charset;
			if (first3Bytes[0] == (byte) 0xFF && first3Bytes[1] == (byte) 0xFE) {
				charset = "UTF-16LE";
				checked = true;
			} else if (first3Bytes[0] == (byte) 0xFE
					&& first3Bytes[1] == (byte) 0xFF) {
				charset = "UTF-16BE";
				checked = true;
			} else if (first3Bytes[0] == (byte) 0xEF
					&& first3Bytes[1] == (byte) 0xBB
					&& first3Bytes[2] == (byte) 0xBF) {
				charset = "UTF-8";
				checked = true;
			}
			bis.reset();
			if (!checked) {
				int loc = 0;
				while ((read = bis.read()) != -1) {
					loc++;
					if (read >= 0xF0)
						break;
					if (0x80 <= read && read <= 0xBF) 
						break;
					if (0xC0 <= read && read <= 0xDF) {
						read = bis.read();
						if (0x80 <= read && read <= 0xBF)
							continue;
						else
							break;
					} else if (0xE0 <= read && read <= 0xEF) {
						read = bis.read();
						if (0x80 <= read && read <= 0xBF) {
							read = bis.read();
							if (0x80 <= read && read <= 0xBF) {
								charset = "UTF-8";
								break;
							} else
								break;
						} else
							break;
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if(fis != null)
				try {
					fis.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		}
		return charset;
	}

	private boolean parseLyricFile(String path) {
		int result = 0;
		FileInputStream fileByte = null;
		InputStreamReader fileChar = null;
		int readChar = 0;
		String str = "";
		Vector<String> lyricContent = new Vector<String>();
		Vector<Long> lyricTime = new Vector<Long>();
		try {
			fileByte = new FileInputStream(path);
			fileChar = new InputStreamReader(fileByte, getCharset(new File(path)));
			
			
			while ((readChar = fileChar.read()) != -1
					|| str.length() != 0) {
				if (readChar != -1 && readChar != 13 && readChar != 10) {
					str = str.concat("" + (char) readChar);
				} else {
					if (str.length() != 0) {
						result = parser(str, lyricContent, lyricTime, result);
						if (result == -1) {
							return false;
						}

						str = "";
					}
				}
			}
			
		} catch (FileNotFoundException ex) {
			ex.printStackTrace();
			return false;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}finally
		{
			try {
				if(null!=fileChar)
				{
				fileChar.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}finally
			{
				try {
					if(null!=fileByte)
					{
					fileByte.close();
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		int lyricTimeSize = lyricTime.size();
		int lyricContentSize = lyricContent.size();
		if (lyricTimeSize > 0 && lyricTimeSize == lyricContentSize) {
			Vector<Long> lyricTimeClone = (Vector<Long>) lyricTime.clone();
			Collections.sort(lyricTimeClone);

			lyricTimeContentInfo = new Vector<LyricTimeContentInfo>();

			long currentLineStartTime = 0;
			String currentLineLyricContent = null;
			long nextLineStartTime = 0;

			for (int i = 0; i < lyricContentSize; i++) {
				currentLineStartTime = (lyricTime.get(i)).longValue();
				currentLineLyricContent = lyricContent.get(i);

				LyricTimeContentInfo timeContentInfo = new LyricTimeContentInfo(
						currentLineStartTime, currentLineLyricContent);
				lyricTimeContentInfo.add(timeContentInfo);
			}

			Collections.sort(lyricTimeContentInfo);

			int i = 0;
			for (; i < lyricContentSize - 1; i++) {
				nextLineStartTime = (lyricTimeClone.get(i + 1)).longValue();
				lyricTimeContentInfo.get(i).setLyricTimeContentInfo(
						nextLineStartTime);
			}

			lyricTimeContentInfo.get(i).setLyricTimeContentInfo(0xffffffffL);

		}

		return true;

	}
	/**
	 * Get lyrice album.
	 * @return
	 */
	public String getLyricAlbum() {
		// TODO Auto-generated method stub
		return lyricAlbum;
	}
	/**
	 * Get lyrice artist.
	 * @return
	 */
	public String getLyricArtist() {
		// TODO Auto-generated method stub
		return lyricArtist;
	}
	/**
	 * Get lyrice editor.
	 * @return
	 */
	public String getLyricEditor() {
		// TODO Auto-generated method stub
		return lyricEditor;
	}
	/**
	 * Get lyrice time offset.
	 * @return
	 */
	public long getLyricTimeOffset() {
		// TODO Auto-generated method stub
		return lyricTimeOffset;
	}
	/**
	 * Get lyrice title.
	 * @return
	 */
	public String getLyricTitle() {
		// TODO Auto-generated method stub
		return lyricTitle;
	}
	/**
	 * Get lyrice time content inforation.
	 * @return
	 */
	public Vector<LyricTimeContentInfo> getLyricTimeContentInfo() {
		return lyricTimeContentInfo;
	}
	/**
	 * According to the parameters set lyric timeOffset
	 * @param timeOffset
	 */
	public void setLyricTimeOffset(long timeOffset) {
		if (timeOffset != lyricTimeOffset) {
			lyricTimeOffset = timeOffset;

			if (lyricTimeContentInfo != null) {
				int index = 0;
				while (index < lyricTimeContentInfo.size()) {
					LyricTimeContentInfo tmp = lyricTimeContentInfo.get(index);
					tmp.setStartTime(tmp.getStartTime() + lyricTimeOffset);
					tmp.setEndTime(tmp.getEndTime() + lyricTimeOffset);

					index++;
				}
			}

		}
	}
	/**
	 * Get line by specified time.
	 * @param time
	 * @return int time line
	 */
	public int getLine(long time) {
		if (lyricTimeContentInfo != null) {
			int line = 0;
			long min = lyricTimeContentInfo.firstElement().getStartTime();
			if (time < min) {
				return -1;
			} else {
				while (line < lyricTimeContentInfo.size()) {
					if (time >= lyricTimeContentInfo.get(line).getEndTime()) {
						line++;
					} else {
						break;
					}
				}

				return line;
			}
		} else {
			return -2;
		}
	}

}
