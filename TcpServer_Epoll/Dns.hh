#ifndef _DNS_HH
#define _DNS_HH

#include <string.h>
#include <string>

const int kSmallBuffer = 1024;

template<int SIZE>
class DnsBuffer
{
public:
  DnsBuffer(): m_cur(m_data){
  }

  ~DnsBuffer(){
    //printf("%s", m_data);
  }

  void append(const char* /*restrict*/ buf, size_t len){
  // append partially
    if (/*implicit_cast<size_t>*/(avail()) > len)
    {
      memcpy(m_cur, buf, len);
      m_cur += len;
    }
  }

  // write in m_data directly
  char* current() {  return m_cur; };
  int avail() const { return static_cast<int> (end() - m_cur); }
  void add(size_t len) { m_cur += len; }
  int length() const {return m_cur - m_data;}
  void bzero() { ::bzero(m_data, sizeof(m_data)); }
  void reset() {m_cur = m_data;}

  const char* data() const { return m_data; }

private:
  const char* end() const { return m_data + sizeof(m_data); }

  char m_data[SIZE];
  char* m_cur;
};

class DnsReq{
public:
  DnsReq(const std::string& domain);
  ~DnsReq();

  const char* data(){ return m_buffer.data(); }

private:

  DnsBuffer<kSmallBuffer> m_buffer;

};

DnsReq::DnsReq(const std::string& domain)
{
  char tmp[] = {1,0,0,1,0,0,0,0,0,0}; //查询请求，期望递归
  m_buffer.append(tmp, sizeof tmp);
}

DnsReq::~DnsReq()
{
}

#endif