#ifndef _NET_TCPSERVER_HH
#define _NET_TCPSERVER_HH

#include <memory>
#include <functional>

#include "CallBacks.hh"
#include "Acceptor.hh"
#include "TcpConnection.hh"

class EventLoop;

class TcpServer
{
public:
  TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name = "Serv ");
  ~TcpServer();

  void start();

  void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }
  void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }

private:
  const TcpServer operator=(const TcpServer&);
  TcpServer(const TcpServer&);

  void newConnetion(int sockfd, const InetAddress& peerAddr);

  EventLoop* p_loop;
  std::string m_name;
  std::unique_ptr<Acceptor> p_acceptor;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
  int m_nextConnId;  
};


#endif