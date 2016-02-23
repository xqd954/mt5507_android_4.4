/*
 * NETBIOS protocol formats
 *
 * @(#) $Header: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/tcpdump/netbios.h#1 $
 */

struct p8022Hdr {
    u_char	dsap;
    u_char	ssap;
    u_char	flags;
};

#define	p8022Size	3		/* min 802.2 header size */

#define UI		0x03		/* 802.2 flags */

