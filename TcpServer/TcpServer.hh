#ifndef _NET_TCPSERVER_HH
#define _NET_TCPSERVER_HH

#include <map>
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
  TcpServer& operator=(const TcpServer&);
  TcpServer(const TcpServer&);

  void newConnetion(int sockfd, const InetAddress& peerAddr);
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  EventLoop* p_loop;
  std::string m_name;
  std::unique_ptr<Acceptor> p_acceptor;
  ConnectionMap m_connectionsMap;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
  int m_nextConnId;  
};


#endif