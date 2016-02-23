package com.mediatek.tvcommon;





/**

 * Options with value in a range.

 * Currently there is no checking the set function in the range in the

 * base abstract class.  Implementation should do the checking not 

 * only for the range. 

 * 

 * @author mtk40063

 *

 * @param <T>

 */

public abstract class TVOptionRange<T  extends Comparable<T>> extends TVOption<T> {

	

	/**

	 * Get min value

	 * @return

	 */

	abstract public T getMin();

	

	/**

	 * Get max value.

	 * @return

	 */

	abstract public T getMax();	

	



	public boolean canSet(T val) {		

		return (val.compareTo(getMax()) <= 0 && val.compareTo(getMin()) >= 0);

	}

	

	public String toString() {

		return super.toString() + " in Range:[" + getMin() + "-" + getMax() + "]";

	}

	

}

