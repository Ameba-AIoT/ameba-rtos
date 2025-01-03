/**
 ******************************************************************************
 * @file    syscall.h
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
*/

#ifndef _SYSCALL_SOCKET_H_
#define _SYSCALL_SOCKET_H_

/* C standard library includes. */
// #include <stddef.h>
// #include <stdio.h>

// #include "ameba.h"

// typedef uint32_t sys_socklen_t;

int syscall_lwip_socket(int domain, int type, int protocol);
int syscall_lwip_bind(int s, void *name, int namelen);
int syscall_lwip_listen(int s, int backlog);
int syscall_lwip_connect(int s, void *name, int namelen);
int syscall_lwip_accept(int s, void *addr, void *addrlen);
int syscall_lwip_recvfrom(int s, void *mem, int len, int flags, void *from, int *fromlen);
int syscall_lwip_read(int s, void *mem, int len);
int syscall_lwip_recv(int s, void *mem, int len, int flags);
int syscall_lwip_sendto(int s, void *data, int size, int flags, void *to, int tolen);
int syscall_lwip_write(int s, void *data, int size);
int syscall_lwip_send(int s, void *data, int size, int flags);
int syscall_lwip_close(int s);
int syscall_lwip_shutdown(int s, int how);
int syscall_lwip_getpeername(int s, void *name, int *namelen);
int syscall_lwip_getsockname(int s, void *name, int *namelen);
int syscall_lwip_setsockopt(int s, int level, int optname, void *optval, int optlen);
int syscall_lwip_getsockopt(int s, int level, int optname, void *optval, int *optlen);
int syscall_lwip_recvmsg(int s, void *message, int flags);
int syscall_lwip_sendmsg(int s, void *msg, int flags);
int syscall_lwip_select(int maxfdp1, void *readset, void *writeset, void *exceptset, void *timeout);
int syscall_lwip_poll(void *fds, int nfds, int timeout);
int syscall_lwip_ioctl(int s, long cmd, void *argp);
const char *syscall_lwip_inet_ntop(int af, void *src, char *dst, int size);
int syscall_lwip_inet_pton(int af, void *src, void *dst);
int syscall_lwip_readv(int s, void *iov, int iovcnt);
int syscall_lwip_writev(int s, void *iov, int iovcnt);
int syscall_lwip_fcntl(int s, int cmd, int val);
int syscall_lwip_errno(void);
char *syscall_lwip_if_indextoname(unsigned int ifindex, void *ifname);
unsigned int syscall_lwip_if_nametoindex(void *ifname);
unsigned short syscall_lwip_htons(unsigned short n);
unsigned int syscall_lwip_htonl(unsigned int n);
int syscall_lwip_getaddrinfo(void *nodename, void *servname, void *hints, void **res);
void syscall_lwip_freeaddrinfo(void *ai);
unsigned int syscall_ipaddr_addr(const char *cp);
void *syscall_lwiperf_start_tcp_server(void *local_addr, unsigned short local_port, void *report_fn, void *report_arg);
void *syscall_lwiperf_start_tcp_client(void *remote_addr, unsigned short remote_port, int type, void *report_fn, void *report_arg);
void syscall_lwiperf_server_start(void);
char *syscall_ip4addr_ntoa(void *addr);
int syscall_ip4addr_aton(const char *cp, void *addr);

#endif