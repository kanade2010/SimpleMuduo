#include <assert.h>
#include <sys/epoll.h>

#include "EventLoop.hh"
#include "Channel.hh"
#include "Logger.hh"
#include "Epoll.hh"

namespace sola{

int createEpollFd()
{
  int epfd = epoll_create1(EPOLL_CLOEXEC);
  if(epfd < 0)
  {
    LOG_SYSFATAL << "createEpollFd() Failed.";
  }
  
  return epfd;
}

void epollRegister(int epfd, int fd, struct epoll_event* ev)
{
  int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, ev);
  if(ret < 0)
  {
    LOG_SYSFATAL << "epollRegister() Failed.";
  }
}

void epollUpdate(int epfd, int fd, struct epoll_event* ev)
{
  int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, ev);
  if(ret < 0)
  {
    LOG_SYSFATAL << "epollUpdate() Failed.";
  }
}

void epollDelete(int epfd, int fd, struct epoll_event* ev)
{
  int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, ev);
  if(ret < 0)
  {
    LOG_SYSFATAL << "epollDelete() Failed.";
  }
}

}

const int Epoll::kMaxEpollConcurrencySize = 10000;
const int Epoll::kInitEpollEventsSize = 1024;

Epoll::Epoll(EventLoop* loop)
  : Poller(loop),
  m_epfd(sola::createEpollFd()),
  m_maxEventsSize(0),
  m_epollEvents(kInitEpollEventsSize)
{

}

Epoll::~Epoll()
{

}

TimeStamp Epoll::poll(int timeoutMs, ChannelList* activeChannels)
{
  LOG_TRACE << "Epoll::poll() maxConcurrencySize " << m_maxEventsSize;
  assert(m_epollEvents.size() > 0);
  int numEvents = ::epoll_wait(m_epfd, m_epollEvents.data(), m_maxEventsSize, timeoutMs);
  TimeStamp now(TimeStamp::now());
  if(numEvents > 0){
    LOG_TRACE << numEvents << " events happended";
    fillActiveChannels(numEvents, activeChannels);
  }
  else if(numEvents == 0){
    LOG_TRACE << " nothing happended";
  }
  else{
    LOG_SYSERR << "Epoll::epoll_wait()";
  }

  return now;
}

/*
 *fillActiveChannels() m_epollEvents 填充活动事件的fd， 把它对应
 *的Channel填入activeChannels。
 */

void Epoll::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
  for(EpollFdList::const_iterator it = m_epollEvents.begin();
      it != m_epollEvents.end() && numEvents > 0; ++it)
  {
      assert(it->events > 0);
      --numEvents;
      ChannelMap::const_iterator ch = m_channels.find(it->data.fd);
      assert(ch != m_channels.end());
      Channel* channel = ch->second;
      assert(channel->fd() == it->data.fd);
      channel->set_revents(it->events);
      activeChannels->push_back(channel);
  }
}


void Epoll::updateChannel(Channel* channel)
{
  assertInLoopThread();
  LOG_TRACE << "fd= " << channel->fd() << " events " << channel->events();
  if(channel->index() < 0){
    //a new one , add to epollEvents
    assert(m_channels.find(channel->fd()) == m_channels.end());
    struct epoll_event ev;
    ev.data.fd = channel->fd();
    ev.events = static_cast<uint32_t>(channel->events());
    sola::epollRegister(m_epfd, channel->fd(), &ev);
    m_maxEventsSize++;
    int idx = m_maxEventsSize;     //poll used, epoll not have significance, just a tag.
    channel->set_index(idx);
    m_channels[channel->fd()] = channel;
  }
  else{
    //update existing one
    assert(m_channels.find(channel->fd()) != m_channels.end());
    assert(m_channels[channel->fd()] == channel);
    struct epoll_event ev;
    ev.data.fd = channel->fd();
    ev.events = static_cast<uint32_t>(channel->events());
    sola::epollUpdate(m_epfd, channel->fd(), &ev);
  }

  if(m_maxEventsSize + 1 == m_epollEvents.capacity())
  {
    int size = m_epollEvents.capacity();
    size = std::min(2*size, kMaxEpollConcurrencySize);
    m_epollEvents.reserve(size);
  }

}


void Epoll::removeChannel(Channel* channel)
{
  assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd();
  assert(m_channels.find(channel->fd()) != m_channels.end());
  assert(m_channels[channel->fd()] == channel);
  assert(channel->isNoneEvent());
  struct epoll_event ev;
  ev.data.fd = channel->fd();
  LOG_TRACE << "Epoll::removeChannel() fd " << channel->fd() << " events " << channel->events();
  sola::epollDelete(m_epfd, channel->fd(), &ev);
  size_t n = m_channels.erase(channel->fd());
  assert(n == 1); (void)n;

  m_maxEventsSize--;
}


