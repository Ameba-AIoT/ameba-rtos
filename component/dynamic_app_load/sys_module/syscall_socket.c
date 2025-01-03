/**
 ******************************************************************************
 * @file    syscall.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
*/

#include "syscall_socket.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#ifdef KERNEL
int syscall_lwip_socket(int domain, int type, int protocol)
{
	return lwip_socket(domain, type, protocol);
}

int syscall_lwip_bind(int s, void *name, int namelen)
{
	return lwip_bind(s, (const struct sockaddr *)name, namelen);
}

int syscall_lwip_listen(int s, int backlog)
{
	return lwip_listen(s, backlog);
}

int syscall_lwip_connect(int s, void *name, int namelen)
{
	return lwip_connect(s, (const struct sockaddr *)name, namelen);
}

int syscall_lwip_accept(int s, void *addr, void *addrlen)
{
	return lwip_accept(s, (struct sockaddr *)addr, addrlen);
}

int syscall_lwip_recvfrom(int s, void *mem, int len, int flags, void *from, int *fromlen)
{
	return lwip_recvfrom(s, mem, len, flags, (struct sockaddr *)from, (socklen_t *)fromlen);
}

int syscall_lwip_read(int s, void *mem, int len)
{
	return lwip_read(s, mem, len);
}

int syscall_lwip_recv(int s, void *mem, int len, int flags)
{
	return lwip_recv(s, mem, len, flags);
}

int syscall_lwip_sendto(int s, void *data, int size, int flags, void *to, int tolen)
{
	return lwip_sendto(s, data, size, flags, (const struct sockaddr *)to, tolen);
}

int syscall_lwip_write(int s, void *data, int size)
{
	return lwip_write(s, data, size);
}

int syscall_lwip_send(int s, void *data, int size, int flags)
{
	return lwip_send(s, data, size, flags);
}

int syscall_lwip_close(int s)
{
	return lwip_close(s);
}

int syscall_lwip_shutdown(int s, int how)
{
	return lwip_shutdown(s, how);
}

int syscall_lwip_getpeername(int s, void *name, int *namelen)
{
	return lwip_getpeername(s, (struct sockaddr *)name, (socklen_t *)namelen);
}

int syscall_lwip_getsockname(int s, void *name, int *namelen)
{
	return lwip_getsockname(s, (struct sockaddr *)name, (socklen_t *)namelen);
}

int syscall_lwip_setsockopt(int s, int level, int optname, void *optval, int optlen)
{
	return lwip_setsockopt(s, level, optname, optval, optlen);
}

int syscall_lwip_getsockopt(int s, int level, int optname, void *optval, int *optlen)
{
	return lwip_getsockopt(s, level, optname, optval, (socklen_t *)optlen);
}

int syscall_lwip_recvmsg(int s, void *message, int flags)
{
	return lwip_recvmsg(s, (struct msghdr *)message, flags);
}

int syscall_lwip_sendmsg(int s, void *message, int flags)
{
	return lwip_sendmsg(s, (struct msghdr *)message, flags);
}

int syscall_lwip_select(int maxfdp1, void *readset, void *writeset, void *exceptset, void *timeout)
{
	return lwip_select(maxfdp1, (fd_set *)readset, (fd_set *)writeset, (fd_set *)exceptset, (struct timeval *)timeout);
}
int syscall_lwip_poll(void *fds, int nfds, int timeout)
{
	return lwip_poll((struct pollfd *)fds, nfds, timeout);
}

int syscall_lwip_ioctl(int s, long cmd, void *argp)
{
	return lwip_ioctl(s, cmd, argp);
}

const char *syscall_lwip_inet_ntop(int af, void *src, char *dst, int size)
{
	return lwip_inet_ntop(af, src, dst, size);
}

int syscall_lwip_inet_pton(int af, void *src, void *dst)
{
	return lwip_inet_pton(af, src, dst);
}
int syscall_lwip_readv(int s, void *iov, int iovcnt)
{
	return lwip_readv(s, (const struct iovec *)iov, iovcnt);
}

int syscall_lwip_writev(int s, void *iov, int iovcnt)
{
	return lwip_writev(s, (const struct iovec *)iov, iovcnt);
}

int syscall_lwip_fcntl(int s, int cmd, int val)
{
	return lwip_fcntl(s, cmd, val);
}

int syscall_lwip_errno(void)
{
	return errno;
}

char *syscall_lwip_if_indextoname(unsigned int ifindex, void *ifname)
{
	(void)ifindex;
	(void)ifname;
	return NULL;
}

unsigned int syscall_lwip_if_nametoindex(void *ifname)
{
	(void)ifname;
	return 0;
}

unsigned short syscall_lwip_htons(unsigned short n)
{
	return lwip_htons(n);
}
unsigned int syscall_lwip_htonl(unsigned int n)
{
	return lwip_htonl(n);
}

int syscall_lwip_getaddrinfo(void *nodename, void *servname, void *hints, void **res)
{
	(void)nodename;
	(void)servname;
	(void)hints;
	(void)res;
	return 0;
}
void syscall_lwip_freeaddrinfo(void *ai)
{
	(void)ai;
}
unsigned int syscall_ipaddr_addr(const char *cp)
{
	return ipaddr_addr(cp);
}

void *syscall_lwiperf_start_tcp_server(void *local_addr, unsigned short local_port, void *report_fn, void *report_arg)
{
	(void)local_addr;
	(void)local_port;
	(void)report_fn;
	(void)report_arg;
	return NULL;
}

void *syscall_lwiperf_start_tcp_client(void *remote_addr, unsigned short remote_port, int type, void *report_fn, void *report_arg)
{
	(void)remote_addr;
	(void)remote_port;
	(void)type;
	(void)report_fn;
	(void)report_arg;
	return NULL;
}

void syscall_lwiperf_server_start(void)
{

}
char *syscall_ip4addr_ntoa(void *addr)
{
	return ip4addr_ntoa((const ip4_addr_t *)addr);
}
int syscall_ip4addr_aton(const char *cp, void *addr)
{
	return ip4addr_aton(cp, (ip4_addr_t *)addr);
}

#endif

