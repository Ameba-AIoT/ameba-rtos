/**
 ******************************************************************************
 * @file    app_syscall_socket.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
// #include "basic_types.h"
#include "app_syscall.h"
extern uint8_t app_sgot[];

/* lwip */
int lwip_socket(int domain, int type, int protocol)
{
	return sys->lwip_socket(domain, type, protocol);
}

int lwip_bind(int s, void *name, int namelen)
{
	return sys->lwip_bind(s, name, namelen);
}

int lwip_listen(int s, int backlog)
{
	return sys->lwip_listen(s, backlog);
}

int lwip_connect(int s, void *name, int namelen)
{
	return sys->lwip_connect(s, name, namelen);
}

int lwip_accept(int s, void *addr, int *addrlen)
{
	return sys->lwip_accept(s, addr, addrlen);
}

ssize_t lwip_recvfrom(int s, void *mem, size_t len, int flags, void *from, int *fromlen)
{
	return sys->lwip_recvfrom(s, mem, len, flags, from, fromlen);
}

ssize_t lwip_read(int s, void *mem, size_t len)
{
	return sys->lwip_read(s, mem, len);
}


ssize_t lwip_recv(int s, void *mem, size_t len, int flags)
{
	return sys->lwip_recv(s, mem, len, flags);
}

ssize_t lwip_send(int s, void *dataptr, size_t size, int flags)
{
	return sys->lwip_send(s, dataptr, size, flags);
}

ssize_t lwip_sendto(int s, void *dataptr, size_t size, int flags, void *to, int tolen)
{
	return sys->lwip_sendto(s, dataptr, size, flags, to, tolen);
}

ssize_t lwip_write(int s, void *dataptr, size_t size)
{
	return sys->lwip_write(s, dataptr, size);
}

int lwip_close(int s)
{
	return sys->lwip_close(s);
}

int lwip_shutdown(int s, int how)
{
	return sys->lwip_shutdown(s, how);
}

int lwip_getpeername(int s, void *name, int *namelen)
{
	return sys->lwip_getpeername(s, name, namelen);
}


int lwip_getsockname(int s, void *name, int *namelen)
{
	return sys->lwip_getsockname(s, name, namelen);
}

int lwip_getsockopt(int s, int level, int optname, void *optval, int *optlen)
{
	return sys->lwip_getsockopt(s, level, optname, optval, optlen);
}

int lwip_setsockopt(int s, int level, int optname, const void *optval, int optlen)
{
	return sys->lwip_setsockopt(s, level, optname, optval, optlen);
}

int lwip_recvmsg(int s, void *message, int flags)
{
	return sys->lwip_recvmsg(s, message, flags);
}

int lwip_sendmsg(int s, void *message, int flags)
{
	return sys->lwip_sendmsg(s, message, flags);
}

int lwip_select(int maxfdp1, void *readset, void *writeset, void *exceptset, void *timeout)
{
	return sys->lwip_select(maxfdp1, readset, writeset, exceptset, timeout);
}

int lwip_poll(void *fds, int nfds, int timeout)
{
	return sys->lwip_poll(fds, nfds, timeout);
}

int lwip_ioctl(int s, long cmd, void *argp)
{
	return sys->lwip_ioctl(s, cmd, argp);
}

int lwip_inet_ntop(int af, void *src, void *dst, int size)
{
	return sys->lwip_inet_ntop(af, src, dst, size);
}

int lwip_inet_pton(int af, void *src, void *dst)
{
	return sys->lwip_inet_pton(af, src, dst);
}

int lwip_readv(int s, void *iov, int iovcnt)
{
	return sys->lwip_readv(s, iov, iovcnt);
}

int lwip_writev(int s, void *iov, int iovcnt)
{
	return sys->lwip_writev(s, iov, iovcnt);
}

int lwip_fcntl(int s, int cmd, int val)
{
	return sys->lwip_fcntl(s, cmd, val);
}

int lwip_errno(void)
{
	return sys->lwip_errno();
}

char *lwip_if_indextoname(unsigned int ifindex, void *ifname)
{
	return sys->lwip_if_indextoname(ifindex, ifname);
}

unsigned int lwip_if_nametoindex(void *ifname)
{
	return sys->lwip_if_nametoindex(ifname);
}

unsigned short lwip_htons(unsigned short n)
{
	return sys->lwip_htons(n);
}

unsigned int lwip_htonl(unsigned int n)
{
	return sys->lwip_htonl(n);
}

int lwip_getaddrinfo(void *nodename, void *servname, void *hints, void **res)
{
	return sys->lwip_getaddrinfo(nodename, servname, hints, res);
}

void lwip_freeaddrinfo(void *ai)
{
	return sys->lwip_freeaddrinfo(ai);
}

unsigned int ipaddr_addr(const char *cp)
{
	return sys->ipaddr_addr(cp);
}

void *lwiperf_start_tcp_server(void *local_addr, unsigned short local_port, void *report_fn, void *report_arg)
{
	return sys->lwiperf_start_tcp_server(local_addr, local_port, report_fn, report_arg);
}

void *lwiperf_start_tcp_client(void *remote_addr, unsigned short remote_port, int type, void *report_fn, void *report_arg)
{
	return sys->lwiperf_start_tcp_client(remote_addr, remote_port, type, report_fn, report_arg);
}

void lwiperf_server_start(void)
{
	return sys->lwiperf_server_start();
}

char *ip4addr_ntoa(void *addr)
{
	return sys->ip4addr_ntoa(addr);
}

int ip4addr_aton(const char *cp, void *addr)
{
	return sys->ip4addr_aton(cp, addr);
}