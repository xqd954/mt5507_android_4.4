/*******************************************************************************
 *
 *  Filename:      btif_sock.h
 *
 *  Description:   Bluetooth socket Interface
 *
 *******************************************************************************/

#ifndef BTIF_SOCK_H
#define BTIF_SOCK_H

#include <hardware/bt_sock.h>

bt_status_t btif_sock_init();
btsock_interface_t *btif_sock_get_interface();
void btif_sock_cleanup();

#endif
