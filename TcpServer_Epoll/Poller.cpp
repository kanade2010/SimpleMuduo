#include <assert.h>

#include "Poller.hh"
#include "Channel.hh"
#include "EventLoop.hh"


Poller::Poller(EventLoop* loop)
  : p_Loop(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(Channel* channel) const
{
  assertInLoopThread();
  ChannelMap::const_iterator it = m_channels.find(channel->fd());
  return it != m_channels.end() && it->second == channel;
}

void Poller::assertInLoopThread() const
{
  p_Loop->assertInLoopThread();
}