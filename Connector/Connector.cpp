#include <assert.h>

#include "SocketHelp.hh"
#include "Connector.hh"
#include "Logger.hh"

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
  :p_loop(loop),
  m_serverAddr(serverAddr),
  m_retryDelayMs(kInitRetryDelayMs)
{

}

Connector::~Connector()
{

}

void Connector::start()
{

  connect();
}

void Connector::connect()
{
  int sockfd = sockets::createNonblockingOrDie(m_serverAddr.family());
  int ret = sockets::connect(sockfd, m_serverAddr.getSockAddr());
  int savedErrno = (ret == 0) ? 0 : errno;

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
      //retry(sockfd);
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

  LOG_INFO << "Connector::retry - Retry connecting to " << m_serverAddr.toIpPort()
           << " in " << m_retryDelayMs << " milliseconds. ";

}

void Connector::handleWrite()
{
  LOG_TRACE << "Connector::handleWrite ";
  m_newConnectionCallBack(p_channel->fd());
}



