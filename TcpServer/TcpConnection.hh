#ifndef _NET_TCPCONNECTION_HH
#define _NET_TCPCONNECTION_HH

#include <memory>
#include <string>

#include "InetAddress.hh"
#include "Socket.hh"
#include "CallBacks.hh"
#include "Channel.hh"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> 
{
public:
  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
  ~TcpConnection();

  void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }
  void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }

private:
  enum StateE { kConnecting, kConnected, };

  void setState(StateE s) { m_state = s; }
  void handleRead();

  EventLoop* p_loop;
  std::string m_name;
  StateE m_state;
  std::unique_ptr<Socket> p_socket;
  std::unique_ptr<Channel> p_channel;
  InetAddress m_localAddr;
  InetAddress m_peerAddr;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

#endif