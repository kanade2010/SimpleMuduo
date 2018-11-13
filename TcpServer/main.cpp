#include <errno.h>
#include <thread>
#include <strings.h>
#include <poll.h>
#include "EventLoop.hh"
#include "Channel.hh"
#include "Poller.hh"
#include "Logger.hh"
#include "AsyncLogging.hh"
#include "TcpServer.hh"

const off_t kRollSize = 2048*1000;

AsyncLogging* g_asynclog = NULL;

void asyncOutput(const char* logline, int len){
  g_asynclog->append(logline, len);
}

void AsyncFlush()
{
  g_asynclog->stop();
}

#include "Acceptor.hh"
#include "SocketHelp.hh"
#include "InetAddress.hh"

void onConnection(const TcpConnectionPtr& conn)
{
  printf("onConnection\n");
}

void onMessage(const TcpConnectionPtr& conn, const char* data, ssize_t len)
{
  printf("onMessage\n");
}


void newConnetion(int sockfd, const InetAddress& peeraddr)
{
  LOG_DEBUG << "newConnetion() : accepted a new connection from";
  ::write(sockfd, "How are you?\n", 13);
  //::sockets::close(sockfd);
}

int main()
{
  AsyncLogging log("/dev/stdout", kRollSize, 0.1);
  g_asynclog = &log;
  Logger::setOutput(asyncOutput);
  Logger::setFlush(AsyncFlush);
  g_asynclog->start();

  InetAddress listenAddr(8888);
  EventLoop loop;
  TcpServer Tserver(&loop, listenAddr, "TcpServer");
  Tserver.setConnectionCallBack(onConnection);
  Tserver.setMessageCallBack(onMessage);
  Tserver.start();

  loop.loop();

}