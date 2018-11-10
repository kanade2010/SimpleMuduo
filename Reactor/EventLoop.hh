#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <memory>
#include <vector>

#include "CurrentThread.hh"

class Poller;
class Channel;

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

	void quit();
	void updateChannel(Channel* channel);

	static EventLoop* getEventLoopOfCurrentThread();

private:
	EventLoop& operator=(const EventLoop&);
	EventLoop(const EventLoop&);

	void abortNotInLoopThread();

	typedef std::vector<Channel*> ChannelList;

	bool m_looping;
	bool m_quit;
	const pid_t m_threadId;
	std::unique_ptr<Poller> m_poller;
	ChannelList m_activeChannels;
};

#endif