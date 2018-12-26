#include <assert.h>
#include <sstream>
#include <poll.h>
#include <sys/epoll.h>

#include "Channel.hh"
#include "EventLoop.hh"
#include "Logger.hh"

#define _EPOLL_METHOD

#ifdef _EPOLL_METHOD
enum EventType
{
  EVENTIN = EPOLLIN,
  EVENTPRI = EPOLLPRI,
  EVENTOUT = EPOLLOUT,
  EVENTHUP = EPOLLHUP,
  EVENTRDHUP = EPOLLRDHUP,
  EVENTERR = EPOLLERR,
  EVENTET = EPOLLET,
};
#else
enum EventType
{
  EVENTIN = POLLIN,
  EVENTPRI = POLLPRI,
  EVENTOUT = POLLOUT,
  EVENTHUP = POLLHUP,
  EVENTRDHUP = POLLRDHUP,
  EVENTERR = POLLERR,
  EVENTNVAL = POLLNVAL,
};
#endif

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EVENTIN | EVENTPRI;
const int Channel::kWriteEvent = EVENTOUT;

Channel::Channel(EventLoop* loop, int fd)
  : p_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1),
    m_addedToLoop(false)
{

}

Channel::~Channel()
{

}

void Channel::update()
{
  m_addedToLoop = true;
  p_loop->updateChannel(this);
}

void Channel::remove()
{
  assert(isNoneEvent());
  m_addedToLoop = false;
  p_loop->removeChannel(this);
}

void Channel::handleEvent()
{
  LOG_TRACE << "Channel::handleEvent() ";
#ifndef _EPOLL_METHOD
  if(m_revents & EVENTNVAL)
  {
    LOG_WARN << "Channel::handleEvent() EVENTNVAL";
    if(m_errorCallBack) m_errorCallBack();
  }
#endif

  if ((m_revents & EVENTHUP) && !(m_revents & EVENTIN))
  {
    LOG_WARN << "fd = " << m_fd << " Channel::handle_event() EVENTHUP";
    if (m_closeCallBack) m_closeCallBack();
  }

  if(m_revents & (EVENTERR)){
    if(m_errorCallBack) m_errorCallBack();
  }

  if(m_revents & (EVENTIN | EVENTPRI | EVENTRDHUP)){
    if(m_readCallBack) m_readCallBack();
  }

  if(m_revents & EVENTOUT){
    if(m_writeCallBack) m_writeCallBack();
  }

}

std::string Channel::reventsToString() const
{
  return eventsToString(m_fd, m_revents);
}

std::string Channel::eventsToString() const
{
  return eventsToString(m_fd, m_events);
}

std::string Channel::eventsToString(int fd, int ev) const
{
  std::ostringstream oss;
  oss << fd << ": ";
  if (ev & EVENTIN)
    oss << "IN ";
  if (ev & EVENTPRI)
    oss << "PRI ";
  if (ev & EVENTOUT)
    oss << "OUT ";
  if (ev & EVENTHUP)
    oss << "HUP ";
  if (ev & EVENTRDHUP)
    oss << "RDHUP ";
  if (ev & EVENTERR)
    oss << "ERR ";
#ifndef _EPOLL_METHOD
  if (ev & EVENTNVAL)
    oss << "NVAL ";
#endif

  return oss.str().c_str();
}
