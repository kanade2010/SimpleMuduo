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

  EventLoop* getLoop() const { return p_loop; }
  const std::string& name() const { return m_name; }
  void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }
  void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }
  void setCloseCallBack(const NetCallBacks::CloseCallBack& cb) { m_closeCallBack = cb; }

  // called when TcpServer accepts a new connection
  void connectEstablished();   // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed();  // should be called only once

  bool isConnected() const { return m_state == kConnected; }
  bool isDisConnected() const { return m_state == kDisConnected; }
private:
  enum StateE { kDisConnected, kConnecting, kConnected, };

  void setState(StateE s) { m_state = s; }
  void handleRead();
  void handleClose();

  EventLoop* p_loop;
  std::string m_name;
  StateE m_state;
  std::unique_ptr<Socket> p_socket;
  std::unique_ptr<Channel> p_channel;
  InetAddress m_localAddr;
  InetAddress m_peerAddr;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
  NetCallBacks::CloseCallBack m_closeCallBack;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

#endif