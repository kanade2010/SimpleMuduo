#include "SocketHelp.hh"
#include "Logger.hh"
#include "Endian.hh"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/uio.h>  // readv

int sockets::createSocket(sa_family_t family){
  // Call "socket()" to create a (family) socket of the specified type.
  // But also set it to have the 'close on exec' property (if we can)

	int sock;

	//CLOEXEC，即当调用exec（）函数成功后，文件描述符会自动关闭。
	//在以往的内核版本（2.6.23以前）中，需要调用 fcntl(fd, F_SETFD, FD_CLOEXEC) 来设置这个属性。
	//而新版本（2.6.23开始）中，可以在调用open函数的时候，通过 flags 参数设置 CLOEXEC 功能，
#ifdef SOCK_CLOEXEC
  sock = socket(family, SOCK_STREAM|SOCK_CLOEXEC, 0);
  if (sock != -1 || errno != EINVAL) return sock;
  // An "errno" of EINVAL likely means that the system wasn't happy with the SOCK_CLOEXEC; fall through and try again without it:
#endif

  sock = socket(family, SOCK_STREAM, 0);

#ifdef FD_CLOEXEC
  if (sock != -1) fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif
  return sock;
}

int sockets::connect(int sockfd, const struct sockaddr* addr)
{
  return ::connect(sockfd, addr, sizeof(struct sockaddr));
}

void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in* addr)
{
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
  {
    LOG_SYSERR << "sockets::fromIpPort";
  }
}

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
  return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
  return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd)
{
  if (::close(sockfd) < 0)
  {
    LOG_SYSERR << "sockets::close";
  }
}

void sockets::shutdownWrite(int sockfd)
{
  if(::shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_SYSERR << "sockets::shutdownWrite";
  }
}

void sockets::delaySecond(int sec)
{
  struct  timeval tv;
  tv.tv_sec = sec;
  tv.tv_usec = 0;
  select(0, NULL, NULL, NULL, &tv);
}

int sockets::createNonblockingOrDie(sa_family_t family, bool isUDP)
{
  int sockfd;
  if(!isUDP)
  {
    sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  }
  else
  {
    sockfd = ::socket(family, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  }

  if (sockfd < 0)
  {
    LOG_SYSFATAL << "sockets::createNonblockingOrDie";
  }

  return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr* addr)
{
  int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr)));
  if(ret < 0)
  {
    LOG_SYSFATAL << "sockets::bindOrDie";
  }
}

void sockets::listenOrDie(int sockfd)
{
  int ret = ::listen(sockfd, SOMAXCONN);
  if(ret < 0)
  {
    LOG_SYSFATAL << "sockets::listenOrDie";
  }
}

int sockets::accept(int sockfd, struct sockaddr_in6* addr)
{
  socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#if defined (NO_ACCEPT4)
  int connfd = ::accept(sockfd, (struct sockaddr *)(addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
  int connfd = ::accept4(sockfd, (struct sockaddr *)(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
  if (connfd < 0)
  {
    int savedErrno = errno;
    LOG_SYSERR << "Socket::accept";
    switch (savedErrno)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: // ???
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG_FATAL << "unexpected error of ::accept " << savedErrno;
        break;
      default:
        LOG_FATAL << "unknown error of ::accept " << savedErrno;
        break;
    }
  }
  return connfd;
}

void sockets::toIpPort(char* buf, size_t size,
                       const struct sockaddr* addr)
{
  toIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in* addr4 = (const struct sockaddr_in* )(addr);
  uint16_t port = sockets::networkToHost16(addr4->sin_port);
  assert(size > end);
  snprintf(buf+end, size-end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size,
                   const struct sockaddr* addr)
{
  if (addr->sa_family == AF_INET)
  {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in* addr4 = (const struct sockaddr_in* )(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
  }
  else if (addr->sa_family == AF_INET6)
  {
    assert(size >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6* addr6 = (const struct sockaddr_in6* )(addr);
    ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
  }
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd)
{
  struct sockaddr_in6 localaddr;
  ::bzero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if(::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localaddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::getLocalAddr";
  }

  return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd)
{
  struct sockaddr_in6 peeraddr;
  bzero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&peeraddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

int sockets::getSocketError(int sockfd)
{
  int optval;

  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
  {
    return errno;
  }
  else
  {
    return optval;
  }
}

int sockets::getSocketType(int sockfd)
{
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);
  if(::getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &optval, &optlen) < 0)
  {
    LOG_SYSERR << "sockets::getSocketType";
  }

  return optval;
}

/*
const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}
*/
