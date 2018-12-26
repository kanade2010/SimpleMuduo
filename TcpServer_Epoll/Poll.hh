#ifndef _NET_POLL_HH
#define _NET_POLL_HH

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

struct pollfd;
class Channel;
class EventLoop;

class Poll : public Poller {
public:
  Poll(EventLoop* loop);
  ~Poll();

  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

private:
  const Poll& operator=(const Poll&);
  Poll(const Poll&);

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;

  PollFdList m_pollfds;
};


#endif
