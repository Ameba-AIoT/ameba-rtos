/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 *****************************************************************************/

#include "rtwpriv.h"
#include "rtw_api.h"
int iw_sockets_open(void)
{
	static const int families[] = {
		AF_INET,
		AF_IPX,
#ifdef PLATFORM_LINUX
		AF_AX25,
#endif
		AF_APPLETALK
	};
	unsigned int	i;
	int		sock;
	/*
	 * Now pick any (exisiting) useful socket family for generic queries
	 * Note : don't open all the socket, only returns when one matches,
	 * all protocols might not be valid.
	 * Workaround by Jim Kaba <jkaba@sarnoff.com>
	 * Note : in 99% of the case, we will just open the inet_sock.
	 * The remaining 1% case are not fully correct...
	 */
	/* Try all families we support */
	for (i = 0; i < sizeof(families) / sizeof(int); ++i) {
		/* Try to open the socket, if success returns it */
		sock = socket(families[i], SOCK_DGRAM, 0);
		if (sock >= 0) {
			return sock;
		}
	}
	return -1;
}



/*------------------------------------------------------------------*/
/*
 * Close the socket used for ioctl.
 */
void iw_sockets_close(int skfd)
{
	close(skfd);
}


int wlan_ioctl_mp(
	int skfd,
	char *inifname,
	void *pBuffer,
	unsigned int BufferSize)
{
	int err;
	struct ifreq ifr;
	union iwreq_data u;
	err = 0;
	memset(&u, 0, sizeof(union iwreq_data));
	u.data.pointer = pBuffer;
	u.data.length = (unsigned short)BufferSize;
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, inifname, strlen(inifname));
	ifr.ifr_ifru.ifru_data = &u;
	err = ioctl(skfd, RTW_IOCTL_MP, &ifr);
	if (u.data.length == 0) {
		*(char *)pBuffer = 0;
	}
	return err;
}


int rtw_io_ctrl(char *ifname, char *inbuf, bool isudp)
{
	int skfd, err = 0;
	char cp_inbuf[BUF_SIZE];
	char in_cmd[BUF_SIZE];
	sscanf(inbuf, "%s ", in_cmd);
	sprintf(cp_inbuf, "%s", inbuf);
	skfd = iw_sockets_open();
	err = wlan_ioctl_mp(skfd, ifname, inbuf, strlen(inbuf) + 1);
	iw_sockets_close(skfd);
	if (err < 0) {
		fprintf(stderr, "Interface doesn't accept private ioctl...\n");
		fprintf(stderr, "%s: %s\n", cp_inbuf, strerror(errno));
	} else {
		if (strlen(inbuf) != 0) {
			printf("%-8.16s %s:%s\n", ifname, in_cmd, inbuf);
		}
		err = 0;
	}
	if (err < 0) {
		printf("CMD ioctrl Error = %s\n", in_cmd);
	}
	return err;
}

void rtw_io_ctrl_nochk(char *ifname, char *inbuf)
{
	int skfd = 0;
	char cp_inbuf[BUF_SIZE];
	char in_cmd[32];
	sscanf(inbuf, "%s ", in_cmd);
	sprintf(cp_inbuf, "%s", inbuf);
	skfd = iw_sockets_open();
	wlan_ioctl_mp(skfd, ifname, inbuf, strlen(inbuf) + 1);
	iw_sockets_close(skfd);
	return;
}
int do_io_ctrl(char *ifname, char *pBufdata)
{
	int skfd, err = 0;
	skfd = iw_sockets_open();
	err = wlan_ioctl_mp(skfd, ifname, pBufdata, strlen(pBufdata) + 1);
	iw_sockets_close(skfd);
	if (err < 0) {
		fprintf(stderr, "Interface doesn't accept private ioctl...\n");
		fprintf(stderr, "%s\n", strerror(errno));
		return -1;
	}
	return err;
}
