#include <errno.h>
#include <thread>
#include <strings.h>
#include <poll.h>
#include "EventLoop.hh"
#include "Channel.hh"
#include "Poller.hh"
#include "Logger.hh"
#include "Connector.hh"
#include "SocketHelp.hh"
#include "InetAddress.hh"
#include "TcpClient.hh"
#include "TcpConnection.hh"
#include "Buffer.hh"

EventLoop* g_loop;

void newConnetion(const TcpConnectionPtr& conn)
{
  LOG_DEBUG << "newConnetion() : Connected a new connection.";
  conn->send("hello world");
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, ssize_t len)
{
  LOG_DEBUG << "onMessage() : received " << buf->readableBytes() << " Bytes from connection [" << conn->name();
  LOG_DEBUG << "onMessage : " << buf->retrieveAsString(len);

}

int main()
{
  EventLoop loop;
  g_loop = &loop;

  InetAddress serverAddr("127.0.0.1", 8888);
  TcpClient client(&loop, serverAddr);
  client.setConnectionCallBack(newConnetion);
  client.setMessageCallBack(onMessage);
  client.start();

  loop.loop();

}
