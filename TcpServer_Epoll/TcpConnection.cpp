#include <assert.h>

#include "TcpConnection.hh"
#include "EventLoop.hh"
#include "Logger.hh"
#include "SocketHelp.hh"

TcpConnection::TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr)
  :p_loop(loop),
  m_name(name),
  m_state(kConnecting),
  p_socket(new Socket(sockfd)),
  p_channel(new Channel(p_loop, sockfd)),
  m_localAddr(localAddr),
  m_peerAddr(peerAddr)
{

  p_channel->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
  p_channel->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));

  LOG_DEBUG << "TcpConnection::ctor[" <<  m_name << "] at " << this
            << " fd=" << sockfd;

  p_socket->setKeepAlive(true);
  //connectEstablished();  do not in Constructor call shared_from_this();
}

TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnection::dtor[" << m_name << "] at "
  << this << " fd=" << p_channel->fd()
  << " state=" << stateToString();

  assert(m_state == kDisConnected);

}

void TcpConnection::connectEstablished()
{
  LOG_TRACE << "TcpConnection::connectEstablished()";
  p_loop->assertInLoopThread();
  assert(m_state == kConnecting);
  setState(kConnected);

  p_channel->enableReading();

  LOG_TRACE << m_localAddr.toIpPort() << " -> "
            << m_peerAddr.toIpPort() << " is "
            << (isConnected() ? "UP" : "DOWN");

  if(m_connectionCallBack) m_connectionCallBack(shared_from_this());

}

void TcpConnection::send(const void* message, size_t len)
{
  if(m_state == kConnected)
  {
    if(p_loop->isInloopThread())
    {
      sendInLoop(message, len);
    }
    else
    {
      p_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message, len));
    }
  }
}

void TcpConnection::send(const std::string& message)
{
  if(m_state == kConnected)
  {
    if(p_loop->isInloopThread())
    {
      sendInLoop(message.data(), message.size());
    }
    else
    {
      p_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message.data(), message.size()));
    }
  }
}

void TcpConnection::send(Buffer* message)
{
  if(m_state == kConnected)
  {
    if(p_loop->isInloopThread())
    {
      sendInLoop(message->peek(), message->readableBytes());
      message->retrieveAll();
    }
    else
    {
      p_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message->peek(), message->readableBytes()));
      message->retrieveAll();
    }
  }
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
  p_loop->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;

  if(m_state == kDisConnected)
  {
    LOG_WARN << "disconnected, give up writing.";
    return ;
  }

  //if nothing in output Buffer, try writing directly.
  if(!p_channel->isWriting() && m_outputBuffer.readableBytes() == 0)
  {
    nwrote = sockets::write(p_channel->fd(), data, len);
    if(nwrote >= 0)
    {
      remaining = len - nwrote;
      if(remaining == 0 )//&& m_writeCompleteCallBack)
      {
        LOG_TRACE << "TcpConnection::sendInLoop() writeCompleteCallBack()";
        //fixd need
        //p_loop->queueInLoop(std::bind())
      }
    }
    else // nwrote < 0
    {
      nwrote = 0;
      if(errno != EWOULDBLOCK) // EAGIN
      {
        LOG_SYSERR << "TcpConnection::sendInLoop()";
        if(errno == EPIPE || errno == ECONNRESET)
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if(!faultError && remaining > 0)
  {
    /*size_t oldLen = m_outputBuffer.readableBytes();
    if(oldLen + remaining >= )
    {
        //fixd need
    }*/

    m_outputBuffer.append(static_cast<const char*>(data) + nwrote, remaining);
    if(!p_channel->isWriting())
    {
      p_channel->enableWriting();
    }
  }
}

void TcpConnection::shutdown()
{
  if(m_state == kConnected)
  {
    setState(kDisConnecting);
    p_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::shutdownInLoop()
{
  p_loop->assertInLoopThread();
  if(!p_channel->isWriting())
  {
    p_socket->shutdownWrite();
  }
}


void TcpConnection::handleRead()
{
  LOG_TRACE << "TcpConnection::handleRead()";

  int savedErrno = 0;

  ssize_t n = m_inputBuffer.readFd(p_channel->fd(), &savedErrno);
  if(n > 0){
    m_messageCallBack(shared_from_this(), &m_inputBuffer, n);
  }
  else if ( n==0 ){
    handleClose();
  }
  else
  {
    errno = savedErrno;
    LOG_SYSERR << "TcpConnection::handleRead()";
    handleError();
  }
}

void TcpConnection::handleWrite()
{
  LOG_TRACE << "TcpConnection::handleWrite()";
  p_loop->assertInLoopThread();
  if(p_channel->isWriting())
  {
    ssize_t n = sockets::write(p_channel->fd(), m_outputBuffer.peek(), m_outputBuffer.readableBytes());

    if(n > 0)
    {
      m_outputBuffer.retrieve(n);
      if(m_outputBuffer.readableBytes() == 0)
      {
        p_channel->disableWriting();
        if(0)//m_writeCompleteCallback)
        {

        }
        if(0)//m_state == kDisConnecting)
        {

        }
      }
      else
      {
        LOG_SYSERR << "TcpConnection::handleWrite";
      }
    }
  }
  else
  {
    LOG_ERROR << "Connection fd = " << p_channel->fd()
              << " is down, no more writing";
  }
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(p_channel->fd());
  LOG_ERROR << "TcpConnection::handleError [" << m_name
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::handleClose()
{
  LOG_TRACE << "TcpConnection::handleClose()";
  p_loop->assertInLoopThread();
  LOG_TRACE << "fd = " << p_channel->fd() << " state = " << stateToString();
  assert(m_state == kConnected || m_state == kDisConnecting);

  setState(kDisConnected);
  p_channel->disableAll();

  TcpConnectionPtr guardThis(shared_from_this());

  m_closeCallBack(guardThis); // to  Tcpserver ConnMap resource.
}

void TcpConnection::connectDestroyed()
{
  LOG_TRACE << "TcpConnection::connectDestroyed()";
  p_loop->assertInLoopThread();

  if (m_state == kConnected)
  {
    setState(kDisConnected);
    p_channel->disableAll();

    LOG_TRACE << m_localAddr.toIpPort() << " -> "
              << m_peerAddr.toIpPort() << " is "
              << (isConnected() ? "UP" : "DOWN");
  }

  p_channel->remove();
}

void TcpConnection::forceClose()
{
  LOG_TRACE << "TcpConnection::forceClose()";
  // FIXME: use compare and swap
  if (m_state == kConnected || m_state == kDisConnecting)
  {
    setState(kDisConnecting);
    p_loop->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
  }
}

void TcpConnection::forceCloseInLoop()
{
  p_loop->assertInLoopThread();
  if (m_state == kConnected || m_state == kDisConnecting)
  {
    // as if we received 0 byte in handleRead();
    handleClose();
  }
}

const char* TcpConnection::stateToString() const
{
  switch (m_state)
  {
    case kDisConnected:
      return "kDisConnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    case kDisConnecting:
      return "kDisConnecting";
    default:
      return "Unknown State";
  }
}