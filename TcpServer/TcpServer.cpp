

#include "TcpServer.hh"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
  :p_loop(loop),
  p_acceptor(new Acceptor(loop, listenAddr))
{
  p_acceptor->setNewConnectionCallBack(
    std::bind(&TcpServer::newConnetion, this));

}


void TcpServer::start()
{
  //p_acceptor
}


void TcpServer::newConnetion()
{
  
}