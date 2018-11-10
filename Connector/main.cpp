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

EventLoop* g_loop;

void newConnetion(int sockfd)
{
  LOG_DEBUG << "newConnetion() : Connected a new connection.";
  g_loop->quit();
}

int main()
{
  EventLoop loop;
  g_loop = &loop;

  InetAddress serverAddr("127.0.0.1", 8888);
  Connector client(&loop, serverAddr);
  client.setNewConnectionCallback(newConnetion);
  client.start();

  loop.loop();

}