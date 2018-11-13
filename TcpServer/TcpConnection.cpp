#include <assert.h>

#include "TcpConnection.hh"
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

  LOG_DEBUG << "TcpConnection::ctor[" <<  m_name << "] at " << this
            << " fd=" << sockfd;

  p_socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnection::dtor[" << m_name << "] at "
  << this << " fd=" << p_channel->fd()
  << " state=" ;

  //assert(m_state == kDisconnected);

}

void TcpConnection::handleRead()
{
  LOG_TRACE << "TcpConnection::handleRead()";
  char buf[666] = {0};
  ssize_t n = sockets::read(p_channel->fd(), buf, sizeof buf);
  m_messageCallBack(shared_from_this(), buf, n);
}