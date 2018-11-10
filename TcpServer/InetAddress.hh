#ifndef _NET_ADDRESS_HH
#define _NET_ADDRESS_HH

#include <netinet/in.h>
#include <string>

class InetAddress
{
 public:
  /// Constructs an endpoint with given port number.
  /// Mostly used in TcpServer listening.
  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

  /// Constructs an endpoint with given ip and port.
  /// @c ip should be "1.2.3.4"
  InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

  /// Constructs an endpoint with given struct @c sockaddr_in
  /// Mostly used when accepting new connections
  explicit InetAddress(const struct sockaddr_in& addr)
    : m_addr(addr)
  { }

  explicit InetAddress(const struct sockaddr_in6& addr)
    : m_addr6(addr)
  { }

  sa_family_t family() const { return m_addr.sin_family; }
  //std::string toIp() const;
  //std::string toIpPort() const;

  const struct sockaddr* getSockAddr() const { return (struct sockaddr*)(&m_addr6); }
  void setSockAddrInet6(const struct sockaddr_in6& addr6) { m_addr6 = addr6; }

  std::string toIpPort() const;

  uint32_t ipNetEndian() const;

  // resolve hostname to IP address, not changing port or sin_family
  // return true on success.
  // thread safe
  // static bool resolve(StringArg hostname, StringArg* ip);
  // static std::vector<InetAddress> resolveAll(const char* hostname, uint16_t port = 0);

 private:
  union{
    struct sockaddr_in m_addr;
    struct sockaddr_in6 m_addr6;
  };
};

#endif