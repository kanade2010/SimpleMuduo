#ifndef _NET_CONNECTOR
#define _NET_CONNECTOR

#include <functional>
#include <memory>

#include "InetAddress.hh"
#include "Channel.hh"

class EventLoop;


class Connector// : public std::enable_shared_from_this<Connector>
{
public:
  typedef std::function<void (int sockfd)> NewConnectionCallBack;

  Connector(EventLoop* loop, const InetAddress& serverAddr, bool isUDPConn = false);
  ~Connector();

  void setNewConnectionCallBack(const NewConnectionCallBack& cb)
  { m_newConnectionCallBack = cb; }

  void start();// can be called in any thread
  void restart();// must be called in loop thread
  void stop(); // can be called in any thread

private:
  const Connector operator=(const Connector&);
  Connector(const Connector&);

  enum States { kDisconnected, kConnecting, kConnected };
  static const int kMaxRetryDelayMs = 30*1000;
  static const int kInitRetryDelayMs = 500;

  void connect();
  void connecting(int sockfd);

  void handleWrite();
  void handleError();

  void retry(int sockfd);
  int removeAndResetChannel();
  void resetChannel();

  void setState(States s) { m_state = s; }
  void startInLoop();
  void stopInLoop();

  EventLoop* p_loop;
  int m_retryDelayMs;
  InetAddress m_serverAddr;

  States m_state;

  std::unique_ptr<Channel> p_channel;
  NewConnectionCallBack m_newConnectionCallBack;

  bool m_isUDPConn;
};


#endif