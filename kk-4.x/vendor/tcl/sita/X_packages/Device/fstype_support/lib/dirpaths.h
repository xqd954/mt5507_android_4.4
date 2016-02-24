/*
 * This file contains the path names for various directories as
 * controlled by the configure script.
 */
/*
 * Modify by Sarah Zhang (zxia@tcl.com)
 * We just use blkid/uuid folders of e2fsprogs/lib.
 * So add some extra files to help compile from build folders.
 */

/* Where to put the messages file for internationalization support */
#define LOCALEDIR		"/usr/share/locale"

/* Where to find the mke2fs.conf and e2fsck.conf files */
#define ROOT_SYSCONFDIR		"/etc"
