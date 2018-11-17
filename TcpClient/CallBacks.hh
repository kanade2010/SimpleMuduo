#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>
#include <memory>

#include "TimeStamp.hh"

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

namespace NetCallBacks
{

// All client visible callbacks go here.

typedef std::function<void()> TimerCallBack;
typedef std::function<void(const TcpConnectionPtr& )> ConnectionCallBack;
typedef std::function<void(const TcpConnectionPtr& , Buffer*, ssize_t )> MessageCallBack;
typedef std::function<void(const TcpConnectionPtr& )> CloseCallBack;

void defaultConnectionCallback();

}

#endif  // NET_CALLBACKS_H
