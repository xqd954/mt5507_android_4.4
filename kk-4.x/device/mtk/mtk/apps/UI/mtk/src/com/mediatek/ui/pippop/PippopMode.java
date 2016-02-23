/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����4:51:22  
 */
package com.mediatek.ui.pippop;

/**
 *
 */
public interface PippopMode {

	public boolean doAction_PIP_POP();

	public boolean doAction_ENTER();

	public boolean doAction_LEFT();

	public boolean doAction_RIGHT();

	public boolean doAction_SWAP();

	public boolean doAction_PIP_POS();

	public boolean doAction_PIP_SIZE();

	public void doAction_TIMEOUT();

	public void setFocusASDefault();

	public void changeState();

}
