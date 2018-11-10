#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include "CurrentThread.hh"

class EventLoop
{
public:
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


	static EventLoop* getEventLoopOfCurrentThread();

private:
	EventLoop& operator=(const EventLoop&);
	EventLoop(const EventLoop&);

	void abortNotInLoopThread();
	bool m_looping;
	const pid_t m_threadId;
};

#endif