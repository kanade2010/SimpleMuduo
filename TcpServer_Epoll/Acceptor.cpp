#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <functional>

#include "SocketHelp.hh"
#include "InetAddress.hh"
#include "EventLoop.hh"
#include "Logger.hh"
#include "Acceptor.hh"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  :p_loop(loop),
  m_acceptSocket(sockets::createNonblockingOrDie(listenAddr.family())),
  m_acceptChannel(loop, m_acceptSocket.fd()),
  m_listenning(false),
  m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(m_idleFd >= 0);
  m_acceptSocket.setReuseAddr(true);
  m_acceptSocket.setReuseAddr(reuseport);
  m_acceptSocket.bindAddress(listenAddr);
  m_acceptChannel.setReadCallBack(
    std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
  m_acceptChannel.disableAll();
  m_acceptChannel.remove();
  ::close(m_idleFd);
}

void Acceptor::listen()
{
  p_loop->assertInLoopThread();
  m_listenning = true;
  m_acceptSocket.listen();
  m_acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
  LOG_TRACE << "Acceptor::handleRead()";
  p_loop->assertInLoopThread();
  InetAddress peerAddr;
  int connfd = m_acceptSocket.accept(&peerAddr);
  if(connfd >= 0)
  {
    if(m_newConnectionCallBack)
    {
      m_newConnectionCallBack(connfd, peerAddr);
    }
    else
    {
      sockets::close(connfd);
    }
  }
  else
  {
    LOG_SYSERR << "in Acceptor::handleRead";
    if(errno == EMFILE)
    {
      ::close(m_idleFd);
      m_idleFd = ::accept(m_acceptSocket.fd(), NULL, NULL);
      ::close(m_idleFd);
      m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}