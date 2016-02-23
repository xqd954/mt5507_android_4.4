/**   
* @Description: TODO()
*/
package com.mediatek.timeshift_pvr.controller;

/**
 * @param <T>
 *
 */
public interface StateScheduleListCallback<T> {

	boolean switchToScheduleItemInfo(T t);
	boolean switchToScheduleList(T t);
	boolean onItemClick(T t);
}
