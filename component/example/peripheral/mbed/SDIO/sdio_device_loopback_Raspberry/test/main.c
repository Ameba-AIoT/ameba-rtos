#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>

#define ATCMD_BUFLEN 	64


#define	RTK_SDIO_IOCTL_MAGIC 	0x88
#define RTK_SDIO_DBG			_IOR(RTK_SDIO_IOCTL_MAGIC, 0, unsigned int)
#define RTK_SDIO_LOOPSTART		_IOR(RTK_SDIO_IOCTL_MAGIC, 1, unsigned int)


/*
* For SDIO IO debug
* Command Format: sdio fn rw addr cnt val
* Example 1: sdio 1 1 0x60 2 0xffff
* Meaning 1: sdio function 1, write 2 bytes to register 0x60, value is 0xffff
* Example 2: sdio 1 0 0x18 4
* Meaning 2: sdio function 1, read 4 bytes from register 0x18
* sdio function 0 is for CCCR operations
*/
static void sdio_ctl(int fd, char *pbuf, int len)
{
	int ret;
	if (len <= 1) {
		return;
	}

	ret = ioctl(fd, RTK_SDIO_DBG, pbuf);
	if (ret == -1) {
		perror("Error in ioctl");
		exit(EXIT_FAILURE);
	}

	printf("value: 0x%08x\n", *((unsigned int *)pbuf));
}


/*
* For SDIO loopback control
* Command Format: loopback start/stop
*/
static void loopback_ctl(int fd, char *pbuf, int len)
{
	int ret;
	if (len <= 1) {
		return;
	}

	ret = ioctl(fd, RTK_SDIO_LOOPSTART, pbuf);
	if (ret == -1) {
		perror("Error in ioctl");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	char buf[ATCMD_BUFLEN];
	int fd, value, ret;

	fd = open("/dev/sdio_chardev", O_RDWR);
	if (fd == -1) {
		perror("Error opening device");
		return -1;
	}

	do {
		memset(buf, 0, sizeof(buf));
		printf("\n\rAT: ");
		fgets((char *)buf, ATCMD_BUFLEN, stdin);
		buf[strlen(buf) - 1] = '\0'; //or here is the carrier return '\r'
		if (strcmp((const char *)buf, (const char *)("exit")) == 0) {
			break;
		} else if (memcmp((const char *)buf, (const char *)("sdio"), 4) == 0) {
			sdio_ctl(fd, (char *)buf, strlen(buf));
		} else if (memcmp((const char *)buf, (const char *)("loopback"), 8) == 0) {
			loopback_ctl(fd, (char *)buf, strlen(buf));
		}
	} while (1);

	close(fd);
	return 0;
}
