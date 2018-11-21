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
#include "EventLoopThread.hh"

EventLoop* g_loop;

void newConnetion(const TcpConnectionPtr& conn)
{
  LOG_DEBUG << "newConnetion() : Connected a new connection.";
  char data[] = {0x61, 0x07, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77, 0x05, 0x62, 0x61, 0x69, 0x64, 0x75, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01}; 
  conn->send(data, sizeof data);
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, ssize_t len)
{
  LOG_DEBUG << "onMessage() : received " << buf->readableBytes() << " Bytes from connection [" << conn->name();
//  LOG_DEBUG << "onMessage : " << buf->retrieveAsString(len);

  for(int i =0 ; i < len; i++ ){
    printf("%4d ", *(buf->peek()));
    buf->retrieve(1);
  }

}

int main()
{
  EventLoopThread loopThread;

  InetAddress serverAddr("119.29.29.29", 53);

  TcpClient client(loopThread.startLoop(), serverAddr);

  client.setConnectionCallBack(newConnetion);
  client.setMessageCallBack(onMessage);
  client.start();

}
