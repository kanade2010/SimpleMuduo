#include <errno.h>
#include "EventLoop.hh"
#include <thread>

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

