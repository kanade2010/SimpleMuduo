#include <poll.h>
#include "Channel.hh"
#include "Logger.hh"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
  : m_pLoop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{

}

Channel::~Channel()
{

}

void Channel::update()
{
  m_pLoop->updateChannel(this);
}


void Channel::handleEvent()
{
  if(m_revents & POLLNVAL)
  {
    LOG_WARN << "Channel::handleEvent() POLLNVAL";
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

