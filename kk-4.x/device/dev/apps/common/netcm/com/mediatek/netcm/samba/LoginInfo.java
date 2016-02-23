package com.mediatek.netcm.samba;

/**
 * Auxiliary class for Samba manager.
 * Use to create the object of user's login information.
 * 
 */
public class LoginInfo {
    private String username;
    private String passwd;

    LoginInfo(String uname, String pwd) {
        username = uname;
        passwd = pwd;
    }

	/**
	 * Get the user name of this object.
	 * 
	 * @return the user name.
	 */
    public String getUserName() {
        return username;
    }

	/**
	 * Get the password of this object.
	 * 
	 * @return the password.
	 */
    public String getPasswd() {
        return passwd;
    }

	/**
	 * Record the user name.
	 * 
	 */
    public void setUserName(String uname) {
        username = uname;
    }

	/**
	 * Record the password.
	 * 
	 */
    public void setPasswd(String pwd) {
        passwd = pwd;
    }
}
