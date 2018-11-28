#ifndef _NET_TCPCLIENT_HH
#define _NET_TCPCLIENT_HH

#include <memory>
#include <mutex>

#include "Connector.hh"
#include "CallBacks.hh"

class EventLoop;

class TcpClient
{
public:
  TcpClient(EventLoop* loop, const InetAddress& serverAddr);//, const std::string& name);
  ~TcpClient();

  void start();
  void disconnect();
  void stop();

  void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }
  void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }

  TcpConnectionPtr connection() const { std::lock_guard<std::mutex> lock(m_mutex); return p_connection; }

  bool isConnected() const { return m_isConnectd; }

private:
  TcpClient(const TcpClient&);
  TcpClient& operator=(const TcpClient&);

  void connect();
  void newConnetion(int sockfd);
  void removeConnection(const TcpConnectionPtr& conn);

  EventLoop* p_loop;

  bool m_isConnectd;
  bool m_enRetry;
  std::unique_ptr<Connector> p_connector;
  TcpConnectionPtr p_connection;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
  mutable std::mutex m_mutex;  // Guard p_connection;
};

#endif