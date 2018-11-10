#ifndef _NET_CONNECTOR
#define _NET_CONNECTOR

#include <functional>
#include <memory>

#include "InetAddress.hh"
#include "Channel.hh"

class EventLoop;


class Connector
{
public:
  typedef std::function<void (int sockfd)> NewConnectionCallback;

  Connector(EventLoop* loop, const InetAddress& serverAddr);
  ~Connector();

  void setNewConnectionCallback(const NewConnectionCallback& cb)
  { m_newConnectionCallBack = cb; }

  void start();// can be called in any thread
  void restart();// must be called in loop thread
  void stop(); // can be called in any thread

  void handleWrite();
private:
  const Connector operator=(const Connector&);
  Connector(const Connector&);
  
  enum States { kDisconnected, kConnecting, kConnected };
  static const int kMaxRetryDelayMs = 30*1000;
  static const int kInitRetryDelayMs = 500;

  void connect();
  void connecting(int sockfd);
  void retry(int sockfd);

  EventLoop* p_loop;
  int m_retryDelayMs;
  InetAddress m_serverAddr;
  std::unique_ptr<Channel> p_channel;
  NewConnectionCallback m_newConnectionCallBack;

};


#endif