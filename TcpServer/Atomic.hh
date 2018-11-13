#ifndef _ATOMIC_HH
#define _ATOMIC_HH

#include <stdint.h>

template <typename T>
class AtomicIntegerT
{
public:
  AtomicIntegerT()
  :m_value(0)
  {

  }

  T get()
  {
    return __sync_val_compare_and_swap(&m_value, 0, 0);
  }

  T incrementAndGet()
  {
    return addAndGet(1);
  }

  T decrementAndGet()
  {
    return addAndGet(-1);
  }

private:
  AtomicIntegerT& operator=(const AtomicIntegerT&);
  AtomicIntegerT(const AtomicIntegerT&);

  T getAndAdd(T x)
  {
    return __sync_fetch_and_add(&m_value, x);
  }

  T addAndGet(T x)
  {
    return getAndAdd(x) + x;
  }

  volatile T m_value; 

};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

#endif