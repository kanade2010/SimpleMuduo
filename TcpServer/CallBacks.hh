#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>
#include <memory>

#include "TimeStamp.hh"

namespace NetCallBacks
{

// All client visible callbacks go here.


typedef std::function<void()> TimerCallback;
typedef std::function<void ()> ConnectionCallBack;


}

#endif  // NET_CALLBACKS_H
