#ifndef _NET_TCPSERVER_HH
#define _NET_TCPSERVER_HH

#include <memory>
#include <functional>

#include "Acceptor.hh"
#include "CallBacks.hh"

class EventLoop;

class TcpServer
{
public:
  TcpServer(EventLoop* loop, const InetAddress& listenAddr);
  ~TcpServer();

  void start();

private:
  const TcpServer operator=(const TcpServer&);
  TcpServer(const TcpServer&);

  void newConnetion();

  EventLoop* p_loop;
  std::unique_ptr<Acceptor> p_acceptor;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
};


#endif