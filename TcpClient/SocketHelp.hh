#ifndef _NET_SOCKETHELP_HH
#define _NET_SOCKETHELP_HH

#include <arpa/inet.h>

namespace sockets
{

///
/// Creates a non-blocking socket file descriptor,
/// abort if any error.
int createSocket(sa_family_t family);
int createNonblockingOrDie(sa_family_t);
int  connect(int sockfd, const struct sockaddr* addr);
void bindOrDie(int sockfd, const struct sockaddr* addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in6* addr);

ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);

void toIpPort(char* buf, size_t size,
                       const struct sockaddr* addr);
void toIp(char* buf, size_t size,
                   const struct sockaddr* addr);
int getSocketError(int sockfd);
struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);
void delaySecond(int sec);
//const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
//const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

}

#endif