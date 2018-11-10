#include <errno.h>
#include <thread>
#include <strings.h>
#include <poll.h>
#include "EventLoop.hh"
#include "Channel.hh"
#include "Poller.hh"
#include "Logger.hh"


/*
void test()
{

	printf(" hello world  %d \n",CurrentThread::tid());

	CurrentThread::t_threadName = "test";

	printf("tid %s name : %s len : %d\n", CurrentThread::tidString(), CurrentThread::name(), CurrentThread::tidStringLength());
	sleep(2);
}


int main()
{
	std::thread thread1(test);
	std::thread thread2(test);
	std::thread thread3(test);
	CurrentThread::t_threadName = "main";

	printf(" main  name : %s %d \n",CurrentThread::name(), CurrentThread::tid());

	thread1.join();


}

*/

/*
EventLoop* g_loop;

void test()
{

  g_loop->loop();

}


int main()
{
  EventLoop testloop;

  //testloop.loop();

  g_loop = &testloop;

  std::thread test_thread(test);

  test_thread.join();

  return 0;
}

*/


//Reactor Test
//单次触发定时器
#include <sys/timerfd.h>

EventLoop* g_loop;

void timeout()
{
  printf("timeout!\n");
  g_loop->quit();
}

int main()
{

  EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK |TFD_CLOEXEC);

  Channel channel(&loop, timerfd);
  channel.setReadCallBack(timeout);
  channel.eableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 3;
  timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.loop();

  close(timerfd);

}



/*
EventLoop* g_loop;

void readCallBack()
{

  LOG_DEBUG << "readCallBack happend.";
  char buf[1024] = {0};
  ssize_t n = read(0, buf, 1024);
  if(n < 0) LOG_FATAL << "read error";

  LOG_DEBUG << buf;

}

int main()
{
  struct pollfd fds;
  fds.fd = 0;

  int fd = 0;

  EventLoop stdinLoop;
  g_loop = &stdinLoop;

  Channel channel(&stdinLoop, fd);
  channel.setReadCallBack(readCallBack);
  channel.eableReading();

  g_loop->loop();

}


*/
