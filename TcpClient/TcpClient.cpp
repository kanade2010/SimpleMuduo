#include <assert.h>
#include <functional>

#include "Logger.hh"
#include "EventLoop.hh"
#include "SocketHelp.hh"
#include "TcpClient.hh"
#include "TcpConnection.hh"

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr)
  :p_loop(loop),
  m_isConnectd(false),
  m_enRetry(false),
  p_connector(new Connector(loop, serverAddr))
{
  LOG_TRACE << "ctor[" << this << "]";
  p_connector->setNewConnectionCallBack(std::bind(&TcpClient::newConnetion, this, std::placeholders::_1));
}


TcpClient::~TcpClient()
{
  LOG_TRACE << "dtor[" << this << "]";
}

void TcpClient::start()
{
  assert(!m_isConnectd);
  connect();
}

void TcpClient::connect()
{
  p_connector->start();
}

void TcpClient::disconnect()
{
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if(p_connection)
    {
      p_connection->shutdown();
    }
  }
}

void TcpClient::newConnetion(int sockfd)
{
  LOG_TRACE << "TcpClient::newConnetion()";
  p_loop->assertInLoopThread();

  InetAddress localAddr(sockets::getLocalAddr(sockfd));

  InetAddress peerAddr(sockets::getPeerAddr(sockfd));
  char buf[64];
  snprintf(buf, sizeof buf, ":%s", peerAddr.toIpPort().c_str());
  std::string connName = buf;

  TcpConnectionPtr conn(new TcpConnection(p_loop, connName, sockfd, localAddr, peerAddr));
  conn->setConnectionCallBack(m_connectionCallBack);
  conn->setMessageCallBack(m_messageCallBack);
  //conn->setWriteCompleteCallBack();
  conn->setCloseCallBack(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    p_connection = conn;
    m_isConnectd = true;
  }

  conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
  LOG_TRACE << "TcpClient::removeConnection()";
  p_loop->assertInLoopThread();
  assert(p_loop == conn->getLoop());

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    assert(p_connection  == conn);
    p_connection.reset();
    m_isConnectd = false;
  }

  p_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));

  if (m_enRetry)
  {
    LOG_INFO << "TcpClient::connect[" /*<< m_name*/ << "] - Reconnecting to "
             ;//<< p_connector->serverAddress().toIpPort();
    p_connector->restart();
  }

}