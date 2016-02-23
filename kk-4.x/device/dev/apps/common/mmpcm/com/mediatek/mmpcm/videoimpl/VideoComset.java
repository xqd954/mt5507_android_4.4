package com.mediatek.mmpcm.videoimpl;

import android.graphics.Rect;

import com.mediatek.mmpcm.MmpTool;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.service.InputService;
import com.mediatek.tv.service.InputService.OutputRegionCapability;

public class VideoComset{

	private InputService inputManager;
	private int MaxZoomType;
	private int curZoomType;

	public VideoComset() {
		TVManager tvManager = TVManager.getInstance(null);
		inputManager = (InputService) tvManager
				.getService(InputService.InputServiceName);
		MaxZoomType = VideoConst.VOUT_ZOOM_TYPE_1X;
		curZoomType = VideoConst.VOUT_ZOOM_TYPE_1X;
		calMaxZoom();
	}

	/**
	 * get current zoom type
	 */
	public int getCurZoomType() {
		return curZoomType;

	}

	/**
	 * get zoom types
	 */
	public int[] getZoomTypes() {
		return null;

	}

	/**
	 * set zoom type
	 */
	public void setZoomType(int type) {
		if (type < VideoConst.VOUT_ZOOM_TYPE_1X
				|| type > VideoConst.VOUT_ZOOM_TYPE_512X) {
			MmpTool.LOG_DBG("Comset.setZoomType"
					+ "This zoom type is not supported !!!");
			curZoomType = VideoConst.VOUT_ZOOM_TYPE_1X;
		}
		curZoomType = type;
	}

	/**
	 * get speed
	 * 
	 * @return
	 */
	public int getSpeed() {
		return 0;
	}

	/**
	 * set picture detail message
	 */
	public void setPictureDetail() {

	}

	/**
	 * change video zoom
	 * 
	 * @param videoZoomType
	 *            the type of zoom
	 */
	public int videoZoom(int videoZoomType) {

		Rect orgVidRect = new Rect();
		Rect orgDspRect = new Rect();
		Rect adjVidRect = new Rect();
		Rect adjDspRect = new Rect();
		float fdivisor = (float) 0.0;
		int maxZoom;
		int ret = -1;

		if (inputManager == null) {
			MmpTool.LOG_DBG("input manager null!!!");
			return -1;
		}

		if (calMaxZoom() == -1)
			return -1;

		maxZoom = MaxZoomType;
		MmpTool.LOG_INFO("video" + "getMaxZoom():" + maxZoom);
		if (maxZoom < 0) {
			maxZoom = VideoConst.VOUT_ZOOM_TYPE_1X;
		}

		if (maxZoom < videoZoomType) {
			MmpTool.LOG_INFO("video" + "maxZoom < videoZoomType");
			return -1;
		}

		if (videoZoomType < VideoConst.VOUT_ZOOM_TYPE_1X
				|| videoZoomType > VideoConst.VOUT_ZOOM_TYPE_512X) {
			MmpTool.LOG_DBG("video zoom type is not supported!");
			return -1;
		}
		orgVidRect = inputManager
				.getScreenOutputVideoRect(InputService.INPUT_OUTPUT_MAIN);
		orgDspRect = inputManager
				.getScreenOutputRect(InputService.INPUT_OUTPUT_MAIN);
		if (null != orgVidRect) {
			MmpTool.LOG_DBG("video" + "orgVidRect.x = " + orgVidRect.left);
			MmpTool.LOG_DBG("video" + "orgVidRect.y = " + orgVidRect.top);
			MmpTool.LOG_DBG("video" + "orgVidRect.w = " + orgVidRect.right);
			MmpTool.LOG_DBG("video" + "orgVidRect.h = " + orgVidRect.bottom);
		}
		if (null != orgDspRect) {
			MmpTool.LOG_DBG("video" + "orgDspRect.x = " + orgDspRect.left);
			MmpTool.LOG_DBG("video" + "orgDspRect.y = " + orgDspRect.top);
			MmpTool.LOG_DBG("video" + "orgDspRect.w = " + orgDspRect.right);
			MmpTool.LOG_DBG("video" + "orgDspRect.h = " + orgDspRect.bottom);
		}

		adjDspRect.left = adjDspRect.top = 0;
		adjDspRect.right = VideoConst.VOUT_REGION_MAX_WIDTH;
		adjDspRect.bottom = VideoConst.VOUT_REGION_MAX_HEIGTH;

		if (0 == videoZoomType % 2) {
			fdivisor = (float) ((1 << (videoZoomType / 2)) * 1.000);
		} else {
			fdivisor = (float) ((1 << (videoZoomType / 2)) * 1.414);
		}

		adjVidRect.left = (int) (VideoConst.VOUT_REGION_MAX_WIDTH / 2 - VideoConst.VOUT_REGION_MAX_WIDTH
				/ fdivisor / 2);
		adjVidRect.right = (int) (VideoConst.VOUT_REGION_MAX_WIDTH / 2 + VideoConst.VOUT_REGION_MAX_WIDTH
				/ fdivisor / 2);
		adjVidRect.top = (int) (VideoConst.VOUT_REGION_MAX_HEIGTH / 2 - VideoConst.VOUT_REGION_MAX_HEIGTH
				/ fdivisor / 2);
		adjVidRect.bottom = (int) (VideoConst.VOUT_REGION_MAX_HEIGTH / 2 + VideoConst.VOUT_REGION_MAX_HEIGTH
				/ fdivisor / 2);

		ret = inputManager.setScreenOutputVideoRect(
				InputService.INPUT_OUTPUT_MAIN, adjVidRect);
		MmpTool.LOG_DBG("setScreenOutputVideoRect ret = " + ret);
		if (ret == InputService.INPUT_RET_FAIL)
			return -1;

		ret = inputManager.setScreenOutputRect(InputService.INPUT_OUTPUT_MAIN,
				adjDspRect);
		MmpTool.LOG_DBG("setScreenOutputRect ret = " + ret);
		if (ret == InputService.INPUT_RET_FAIL)
			return -1;

		curZoomType = videoZoomType;
		return ret;
	}

	private int calMaxZoom() {
		int maxZoom = VideoConst.VOUT_ZOOM_TYPE_1X;
		float fdivisor = (float) 0.0;
		int videoWidth = 0;
		int videoHeight = 0;

		if (inputManager == null) {
			MmpTool.LOG_DBG("input manager null!!!");
			return -1;
		}

		OutputRegionCapability capability = inputManager
				.getScreenOutputVideoCapability(InputService.INPUT_OUTPUT_MAIN);
		if (null != capability) {
			MmpTool.LOG_INFO("video" + "capability.width_min:"
					+ capability.width_min);
			MmpTool.LOG_INFO("video" + "capability.height_min:"
					+ capability.height_min);
		}

		for (maxZoom = VideoConst.VOUT_ZOOM_TYPE_1X; maxZoom <= VideoConst.VOUT_ZOOM_TYPE_512X; maxZoom++) {
			if (0 == maxZoom % 2) {
				fdivisor = (float) ((1 << (maxZoom / 2)) * 1.000);
			} else {
				fdivisor = (float) ((1 << (maxZoom / 2)) * 1.414);
			}

			videoWidth = (int) (VideoConst.VOUT_REGION_MAX_WIDTH / fdivisor);
			videoHeight = (int) (VideoConst.VOUT_REGION_MAX_HEIGTH / fdivisor);

			if (null != capability) {
				int mWidthmin = capability.width_min;
				int mHeightmin = capability.height_min;
				if ((mWidthmin > videoWidth) || (mHeightmin > videoHeight)) {
					MaxZoomType = maxZoom - 1;
					return 1;
				}
			}

		}

		MaxZoomType = VideoConst.VOUT_ZOOM_TYPE_512X;
		return 1;
	}

	/**
	 * get max zoom
	 */
	public int getMaxZoom() {
		return MaxZoomType;
	}

	/**
	 * reset video zoom
	 * 
	 * @return
	 */
	public int videoZoomReset() {
		Rect outRect = new Rect(0, 0, VideoConst.VOUT_REGION_MAX_WIDTH,
				VideoConst.VOUT_REGION_MAX_HEIGTH);

		if (inputManager == null) {
			MmpTool.LOG_DBG("input manager null!!!");
			return -1;
		}

		MmpTool.LOG_DBG("zoom reset!!!");
		curZoomType = VideoConst.VOUT_ZOOM_TYPE_1X;
		return inputManager.setScreenOutputRect(InputService.INPUT_OUTPUT_MAIN,
				outRect);
	}
}