#include <assert.h>

#include "Acceptor.hh"
#include "TcpServer.hh"
#include "EventLoop.hh"
#include "Logger.hh"
#include "SocketHelp.hh"

void NetCallBacks::defaultConnectionCallback()
{
  LOG_TRACE << "defaultConnectionCallback ";
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name)
  :p_loop(loop),
  m_name(name),
  p_acceptor(new Acceptor(loop, listenAddr)),
  m_connectionCallBack(NetCallBacks::defaultConnectionCallback),
  m_nextConnId(1)
{
  p_acceptor->setNewConnectionCallBack(
    std::bind(&TcpServer::newConnetion, this, std::placeholders::_1, std::placeholders::_2));

}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
  assert(!p_acceptor->listenning());
  p_loop->runInLoop(
    std::bind(&Acceptor::listen, p_acceptor.get()));
}


void TcpServer::newConnetion(int sockfd, const InetAddress& peerAddr)
{
  LOG_TRACE << "TcpServer::newConnetion() ";
  p_loop->assertInLoopThread();

  char buf[64];
  snprintf(buf, sizeof buf, "#%d", m_nextConnId);
  ++m_nextConnId;
  std::string connName = m_name + buf;

  LOG_INFO << "TcpServer::newConnetion() [" << m_name
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  TcpConnectionPtr conn(new TcpConnection(p_loop, connName, sockfd, localAddr, peerAddr));
  conn->setConnectionCallBack(m_connectionCallBack);
  conn->setMessageCallBack(m_messageCallBack);

}