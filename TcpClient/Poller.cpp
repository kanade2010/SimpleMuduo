#include "Poller.hh"
#include "Logger.hh"
#include <assert.h>
#include <poll.h>
#include <signal.h>

Poller::Poller(EventLoop* loop)
  : m_pOwerLoop(loop)
{

}

Poller::~Poller()
{

}

TimeStamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
  LOG_TRACE << "Poller::poll()";
  int numEvents = ::poll(/*&*m_pollfds.begin()*/m_pollfds.data(), m_pollfds.size(), timeoutMs);
  TimeStamp now(TimeStamp::now());
  if(numEvents > 0){
    LOG_TRACE << numEvents << " events happended";
    fillActiveChannels(numEvents, activeChannels);
  }
  else if(numEvents == 0){
    LOG_TRACE << " nothing happended";
  }
  else{
    LOG_SYSERR << "Poller::poll()";
  }

  return now;
}

/*
 *fillActiveChannels()遍历m_pollfds， 找出有活动事件的fd， 把它对应
 *的Channel填入activeChannels。
 */

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
  for(PollFdList::const_iterator pfd = m_pollfds.begin();
      pfd != m_pollfds.end() && numEvents > 0; ++pfd)
  {
    if(pfd->revents > 0)
    {
      --numEvents;
      ChannelMap::const_iterator ch = m_channels.find(pfd->fd);
      assert(ch != m_channels.end());
      Channel* channel = ch->second;
      assert(channel->fd() == pfd->fd);
      channel->set_revents(pfd->revents);
      activeChannels->push_back(channel);
    }
  }
}


void Poller::updateChannel(Channel* channel)
{
  assertInLoopThread();
  LOG_TRACE << "fd= " << channel->fd() << " events " << channel->events();
  if(channel->index() < 0){
    //a new one , add to pollfds
    assert(m_channels.find(channel->fd()) == m_channels.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    m_pollfds.push_back(pfd);
    int idx = static_cast<int>(m_pollfds.size()) - 1;
    channel->set_index(idx);
    m_channels[pfd.fd] = channel;

  }
  else{
    //update existing one
    assert(m_channels.find(channel->fd()) != m_channels.end());
    assert(m_channels[channel->fd()] == channel);
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(m_pollfds.size()));
    struct pollfd& pfd = m_pollfds[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if(channel->isNoneEvent()){
      LOG_TRACE << "Poller::updateChannel() Ignore This Pollfd";
      pfd.fd = -1;
    }
  }

}


void Poller::removeChannel(Channel* channel)
{
  assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd();
  assert(m_channels.find(channel->fd()) != m_channels.end());
  assert(m_channels[channel->fd()] == channel);
  assert(channel->isNoneEvent());
  int idx = channel->index();
  assert(0 <= idx && idx < static_cast<int>(m_pollfds.size()));
  const struct pollfd& pfd = m_pollfds[idx];
  (void)pfd;
  LOG_TRACE << "Poller::removeChannel() idx " << idx << " pfd.fd " << pfd.fd << " pfd.events " << pfd.events;
  assert(pfd.fd == - 1 && pfd.events == channel->events());
  size_t n = m_channels.erase(channel->fd());
  assert(n == 1); (void)n;
  if(m_pollfds.size() - 1 == idx)
  {
    m_pollfds.pop_back();
  }
  else
  {
    int channelAtEnd = m_pollfds.back().fd;
    iter_swap(m_pollfds.begin() + idx, m_pollfds.end() - 1);
    if(channelAtEnd < 0)
    {
      channelAtEnd = -channelAtEnd - 1;
    }
    m_channels[channelAtEnd]->set_index(idx);
    m_pollfds.pop_back();
  }
}


