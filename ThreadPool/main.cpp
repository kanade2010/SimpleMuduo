#include "Thread.hh"
#include "MutexLock.hh"

#include <chrono>
#include<condition_variable>
#include "Condition.hh"

/*int g_sum = 0;

std::mutex g_mutex;
std::condition_variable g_cond;

void testfun()
{

printf("testfun() wait\n" );
  std::unique_lock<std::mutex> g_lock(g_mutex);
  g_cond.wait(g_lock);

  printf("testfun exit\n");
}

void testfun2()
{

printf("testfun2 wait\n" );
  std::unique_lock<std::mutex> g_lock(g_mutex);
  g_cond.wait_for(g_lock, std::chrono::milliseconds(3500));

  printf("testfun2 exit\n");
}


int main()
{
  Thread t(testfun);
  t.start();
  Thread t2(testfun2);
  t2.start();

  printf("%d\n", t.getThreadId());

  getchar();

  printf("notify testfun\n");
  g_cond.notify_one();

  getchar();
}*/

std::mutex g_mutex;

int main()
{
  Condition cond(g_mutex);

  cond.waitForSeconds(3.1658);

  printf("main exit\n");

  return 0;
}


