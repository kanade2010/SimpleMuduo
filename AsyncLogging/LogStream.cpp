#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "LogStream.hh"


LogStream::LogStream(){
}

LogStream::~LogStream(){
}

LogStream& LogStream::operator<<(bool v){
  m_buffer.append(v ? "1" : "0", 1);
  return *this;
}


LogStream& LogStream::operator<<(short v){
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
  formatInteger(v);
  return *this;
}

const char digitsHex[] = "0123456789ABCDEF";

size_t convertHex(char buf[], uintptr_t value)
{
  uintptr_t i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

LogStream& LogStream::operator<<(const void* p){
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (m_buffer.avail() >= kMaxNumericSize)
  {
    char* buf = m_buffer.current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf+2, v);
    m_buffer.add(len+2);
  }
  return *this;
}

LogStream& LogStream::operator<<(float v)
  {
    *this << static_cast<double>(v);
    return *this;
  }

LogStream& LogStream::operator<<(double v){
  if(m_buffer.avail() >= kMaxNumericSize){
    int len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", v);
    m_buffer.add(len);
  }
  return *this;
}

LogStream& LogStream::operator<<(char v){
  m_buffer.append(&v, 1);
  return *this;
}

LogStream& LogStream::operator<<(const char *str){
  if(str){
    m_buffer.append(str, strlen(str));
  }else{
    m_buffer.append("(NULL)", 6);
  }

  return *this;
}

LogStream& LogStream::operator<<(const std::string& s){
  if(!s.empty()){
    m_buffer.append(s.c_str(), s.size());
  }else{
    m_buffer.append("(NULL)", 6);
  }

  return *this;
}


const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

//convert to str
template<typename T>
size_t convert(char buf[], T value){
  T i = value;
  char *p = buf;

  do{
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while(i != 0);

  if(value < 0){
    *p++ = '-';
  }

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

template<typename T>
void LogStream::formatInteger(T v)
{
  if(m_buffer.avail() >= kMaxNumericSize){
    size_t len = convert(m_buffer.current(), v);
    m_buffer.add(len);
  }
}

template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
  m_length = snprintf(m_buf, sizeof(m_buf), fmt, val);
  assert(static_cast<size_t>(m_length) < sizeof(m_buf));
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);