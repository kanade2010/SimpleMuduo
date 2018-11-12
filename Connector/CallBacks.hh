#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>
#include <memory>

#include "TimeStamp.hh"

namespace NetCallBacks
{

// All client visible callbacks go here.

typedef std::function<void()> TimerCallBack;

}

#endif  // NET_CALLBACKS_H
