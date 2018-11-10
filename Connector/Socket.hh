#ifndef _NET_SOCKET_HH
#define _NET_SOCKET_HH

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

class InetAddress;

class Socket{
public:
  explicit Socket(int sockfd) : m_sockfd(sockfd) { }
  ~Socket();

  int fd() const { return m_sockfd; }
  //return true if success.
  bool getTcpInfo(struct tcp_info* ) const;
  bool getTcpInfoString(char* buf, int len) const;

  void bindAddress(const InetAddress& localaddr);
  void listen();
  int accept(int sockfd, struct sockaddr_in6* addr);

  int accept(InetAddress* peeraddr);
  void shutdownWrite();

  void setTcpNoDelay(bool on);

  void setReuseAddr(bool on);

  void setReusePort(bool on);

  void setKeepAlive(bool on);

private:
  const int m_sockfd;

};

#endif