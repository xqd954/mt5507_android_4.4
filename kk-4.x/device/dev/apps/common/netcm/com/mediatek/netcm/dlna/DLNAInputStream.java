package com.mediatek.netcm.dlna;

import java.io.IOException;

import com.mediatek.dlna.object.Content;
import com.mediatek.dlna.object.ContentInputStream;

/**
 * This Class is the Content object streaming, use to download the content resource.
 * 
 */
public class DLNAInputStream extends ContentInputStream {

	public DLNAInputStream(Content content) {
		super(content);
	}

	public int read() throws IOException {
		return super.read();
	}

	public int read(byte[] b) throws IOException {
		return super.read(b);
	}

	public int read(byte[] b, int off, int len) throws IOException {
		return super.read(b, off, len);
	}

	public long skip(long n) throws IOException {
		return super.skip(n);
	}

	public int available() throws IOException {
		return super.available();
	}

	public void close() throws IOException {
		super.close();
	}

	public void reset() throws IOException {
		super.reset();
	}

	public void mark(int readlimit) {
		super.mark(readlimit);
	}

	public boolean markSupported() {
		return super.markSupported();
	}

	public long size() {
		return super.size();
	}
}