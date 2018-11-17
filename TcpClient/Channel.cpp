#include <poll.h>
#include <assert.h>
#include <sstream>

#include "Channel.hh"
#include "EventLoop.hh"
#include "Logger.hh"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

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
  if(m_revents & POLLNVAL)
  {
    LOG_WARN << "Channel::handleEvent() POLLNVAL";
  }

  if ((m_revents & POLLHUP) && !(m_revents & POLLIN))
  {
    if(true){
      LOG_WARN << "fd = " << m_fd << " Channel::handle_event() POLLHUP";
    }
    if (m_closeCallBack) m_closeCallBack();
  }

  if(m_revents & (POLLERR | POLLNVAL)){
    if(m_errorCallBack) m_errorCallBack();
  }

  if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)){
    if(m_readCallBack) m_readCallBack();
  }

  if(m_revents & POLLOUT){
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
  if (ev & POLLIN)
    oss << "IN ";
  if (ev & POLLPRI)
    oss << "PRI ";
  if (ev & POLLOUT)
    oss << "OUT ";
  if (ev & POLLHUP)
    oss << "HUP ";
  if (ev & POLLRDHUP)
    oss << "RDHUP ";
  if (ev & POLLERR)
    oss << "ERR ";
  if (ev & POLLNVAL)
    oss << "NVAL ";

  return oss.str().c_str();
}
