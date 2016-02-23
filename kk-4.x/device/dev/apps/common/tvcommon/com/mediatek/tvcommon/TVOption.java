package com.mediatek.tvcommon;


/**
 * Option base class.
 * 
 * @author mtk40063
 *
 * @param <T> Value type
 */
public abstract class TVOption<T> {
	
	public TVOption() {
	}
	
		
	/**
	 * Set value 
	 * @param val
	 * @return
	 */
	abstract public boolean set(T val);

	/**
	 * Get value
	 * @return value
	 */
	abstract public T get();
	
	/**
	 * Val can be set
	 * @param val
	 * @return
	 */
	abstract boolean canSet(T val);
	
	public String toString() {
		return "Val = " + get();
	}
}

