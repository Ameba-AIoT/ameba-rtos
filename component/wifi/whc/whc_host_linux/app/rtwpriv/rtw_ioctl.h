/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 *****************************************************************************/

#ifndef __RTW_IOCTL_H__
#define __RTW_IOCTL_H__

int iw_sockets_open(void);
void iw_sockets_close(int skfd);
int wlan_ioctl_mp(int skfd, char *ifname, void *pBuffer, unsigned int BufferSize);
int rtw_io_ctrl(char *ifname, char *inbuf, bool isudp);
void rtw_io_ctrl_nochk(char *ifname, char *inbuf);
int do_io_ctrl(char *ifname, char *pBufdata);

#endif /* __RTW_IOCTL_H__ */
