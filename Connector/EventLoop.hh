#ifndef _NET_EVENTLOOP_H
#define _NET_EVENTLOOP_H

#include <memory>
#include <vector>
#include <functional>

#include "TimerId.hh"
#include "TimeStamp.hh"
#include "TimerQueue.hh"
#include "CallBacks.hh"
#include "CurrentThread.hh"
#include "MutexLock.hh"
#include "Channel.hh"

class Poller;
//class Channel;

class EventLoop
{
public:
	typedef std::function<void()> Functor;

	EventLoop();
	~EventLoop();
	void loop();

	void assertInLoopThread()
	{
		if(!isInloopThread())
		{
			abortNotInLoopThread();
		}
	}

	bool isInloopThread() const {return m_threadId == CurrentThread::tid(); }
	void runInLoop(const Functor& cb);
  void queueInLoop(const Functor& cb);

	void quit();

	void wakeup();
	void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

	TimerId runAt(const TimeStamp& time, const NetCallBacks::TimerCallBack& cb);
	TimerId runAfter(double delay, const NetCallBacks::TimerCallBack& cb);
	TimerId runEvery(double interval, const NetCallBacks::TimerCallBack& cb);

	static EventLoop* getEventLoopOfCurrentThread();

private:
	EventLoop& operator=(const EventLoop&);
	EventLoop(const EventLoop&);

	void abortNotInLoopThread();
  void handleRead();  // waked up
	void doPendingFunctors();

	typedef std::vector<Channel*> ChannelList;

	bool m_looping;
	bool m_quit;
	const pid_t m_threadId;
	std::unique_ptr<Poller> m_poller;
	std::unique_ptr<TimerQueue> m_timerQueue;
	ChannelList m_activeChannels;

	std::unique_ptr<Channel> p_wakeupChannel;
	int m_wakeupFd;
	mutable MutexLock m_mutex;
  bool m_callingPendingFunctors; /* atomic */
  std::vector<Functor> m_pendingFunctors; // @GuardedBy mutex_

};

#endif