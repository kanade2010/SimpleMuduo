#ifndef _NET_POLLER_HH
#define _NET_POLLER_HH

#include <vector>
#include <map>

#include "TimeStamp.hh"
#include "EventLoop.hh"
#include "Channel.hh"

/*
 *#include <sys/poll.h>
 *struct pollfd
 *{
 *int fd; // 想查询的文件描述符.
 *short int events; // fd 上，我们感兴趣的事件
 *short int revents; /// 实际发生了的事件.
 *};
 */

struct pollfd;
//class Channel;

/**
  *Poller class是IO multiplexing的封装。 它现在是个具体类， 而在
  *muduo中是个抽象基类， 因为muduo同时支持poll(2)和epoll(4)两种IO
  *multiplexing机制。 Poller是EventLoop的间接成员， 只供其owner
  *EventLoop在IO线程调用， 因此无须加锁。 其生命期与EventLoop相等。
  *Poller并不拥有Channel， Channel在析构之前必须自己
  *unregister（EventLoop::removeChannel()） ， 避免空悬指针。
**/

class Poller{
public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);
  ~Poller();

  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

  void assertInLoopThread() { m_pOwerLoop->assertInLoopThread(); }

private:
  Poller& operator=(const Poller&);
  Poller(const Poller&);

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;
  typedef std::map<int, Channel*> ChannelMap;

  EventLoop* m_pOwerLoop;
  PollFdList m_pollfds;
  ChannelMap m_channels;

};


#endif
