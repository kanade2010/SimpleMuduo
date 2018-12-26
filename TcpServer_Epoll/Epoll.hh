#ifndef _NET_EPOLL_HH
#define _NET_EPOLL_HH

#include <vector>
#include <map>

#include "TimeStamp.hh"
#include "Poller.hh"

/*
 *#include <sys/poll.h>
 *struct pollfd
 *{
 *int fd; // 想查询的文件描述符.
 *short int events; // fd 上，我们感兴趣的事件
 *short int revents; /// 实际发生了的事件.
 *};
 */

struct epoll_event;
class Channel;
class EventLoop;

class Epoll : public Poller {
public:
  Epoll(EventLoop* loop);
  ~Epoll();

  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

private:
  const Epoll& operator=(const Epoll&);
  Epoll(const Epoll&);

  static const int kMaxEpollConcurrencySize;
  static const int kInitEpollEventsSize;

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  typedef std::vector<struct epoll_event> EpollFdList;

  int m_epfd;
  int m_maxEventsSize;
  EpollFdList m_epollEvents;
};


#endif
