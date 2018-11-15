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

void TcpConnection::handleRead()
{
  LOG_TRACE << "TcpConnection::handleRead()";
  char buf[666] = {0};
  ssize_t n = sockets::read(p_channel->fd(), buf, sizeof buf);
  if(n > 0){
    m_messageCallBack(shared_from_this(), buf, n);
  }
  else if ( n==0 ){
    handleClose();
  }
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG_ERROR << "TcpConnection::handleError [" << m_name
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::handleClose()
{
  LOG_TRACE << "TcpConnection::handleClose()";
  p_loop->assertInLoopThread();
  LOG_TRACE << "fd = " << p_channel->fd() << " state = " << stateToString();
  assert(m_state == kConnected );//|| m_state == kDisConnecting);

  setState(kDisConnected);
  p_channel->disableAll();

  TcpConnectionPtr guardThis(shared_from_this());

  m_closeCallBack(guardThis); // to release Tcpserver ConnMap resource.
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


const char* TcpConnection::stateToString() const
{
  switch (m_state)
  {
    case kDisConnecting:
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