#include <assert.h>

#include "SocketHelp.hh"
#include "Connector.hh"
#include "EventLoop.hh"
#include "Logger.hh"

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr, bool isUDPConn)
  :p_loop(loop),
  m_serverAddr(serverAddr),
  m_state(kDisconnected),
  m_retryDelayMs(kInitRetryDelayMs),
  m_isUDPConn(isUDPConn)
{
  LOG_DEBUG << "ctor[" << this << "]";
}

Connector::~Connector()
{
  LOG_DEBUG << "dtor[" << this << "]";
  assert(!p_channel);
}

void Connector::start()
{

  p_loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::restart()
{
  p_loop->assertInLoopThread();
  setState(kDisconnected);
  m_retryDelayMs = kInitRetryDelayMs;
  startInLoop();
}

void Connector::startInLoop()
{
  p_loop->assertInLoopThread();
  assert(m_state == kDisconnected);

  connect();
}

void Connector::stop()
{
  p_loop->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
  p_loop->assertInLoopThread();

  if(m_state == kConnecting)
  {
    int sockfd = removeAndResetChannel();
    sockets::close(sockfd);
    setState(kDisconnected);
  }
}

void Connector::connect()
{
  LOG_TRACE << "Connector::connect()";
  int sockfd = sockets::createNonblockingOrDie(m_serverAddr.family(), m_isUDPConn);
  int ret = sockets::connect(sockfd, m_serverAddr.getSockAddr());
  int savedErrno = (ret == 0) ? 0 : errno;

  if(ret != 0) LOG_TRACE << "connect error ("<< savedErrno << ") : " << strerror_tl(savedErrno);

  switch(savedErrno)
  {
    case 0:
    case EINPROGRESS:      //Operation now in progress
    case EINTR:            //Interrupted system call
    case EISCONN:          //Transport endpoint is already connected
      connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      retry(sockfd);
      LOG_SYSERR << "reSave Error. " << savedErrno;
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      break;

    default:
      LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      // connectErrorCallback_();
      break;
  }

}

void Connector::connecting(int sockfd)
{
  LOG_TRACE << "Connector::connecting] sockfd : " << sockfd;
  setState(kConnecting);
  assert(!p_channel);
  p_channel.reset(new Channel(p_loop, sockfd));
  p_channel->setWriteCallBack(std::bind(&Connector::handleWrite, this));
  //p_channel->setErrorCallback()

  //enableWriting if Channel Writeable ,Connect Success.
  p_channel->enableWriting();
}

void Connector::retry(int sockfd)
{
  sockets::close(sockfd);
  setState(kDisconnected);

  LOG_INFO << "Connector::retry - Retry connecting to " << m_serverAddr.toIpPort()
           << " in " << m_retryDelayMs << " milliseconds. ";

  p_loop->runAfter(m_retryDelayMs/1000.0, std::bind(&Connector::startInLoop, this));
  m_retryDelayMs = std::min(m_retryDelayMs * 2, kMaxRetryDelayMs);
}

int Connector::removeAndResetChannel()
{
  p_channel->disableAll();
  p_channel->remove();

  int sockfd = p_channel->fd();

  p_loop->queueInLoop(std::bind(&Connector::resetChannel, this));

  return sockfd;
}

void Connector::resetChannel()
{
  LOG_TRACE << "Connector::resetChannel()";
  p_channel.reset();
}

void Connector::handleWrite()
{
  LOG_TRACE << "Connector::handleWrite ";

  if(m_state == kConnecting)
  {
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);

    if(err)
    {
      LOG_WARN << "Connector::handleWrite - SO_ERROR = "
               << err << " " << strerror_tl(err);
      retry(sockfd);
    }
    /*else if (sockets::isSelfConnect(sockfd))
    {

    }*/
    else
    {
      setState(kConnected);
      m_newConnectionCallBack(sockfd);
    }

  }
  else
  {
    //怎么回事 , 小老弟？
    assert(m_state == kDisconnected);
  }

}

void Connector::handleError()
{
  LOG_ERROR << "Connector::handleError States " << m_state;

  if(m_state == kConnecting)
  {
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    LOG_TRACE << "SOCK_ERROR = " << err << " " << strerror_tl(err);
    retry(sockfd);
  }
}

