#ifndef _NET_BUFFER_H
#define _NET_BUFFER_H

#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>
//#include <unistd.h>  // ssize_t

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer
{
public:

  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 4096;
//public:  

	explicit Buffer(size_t initialSize = kInitialSize)
	: m_buffer(kCheapPrepend + initialSize),
	  m_readerIndex(kCheapPrepend),
	  m_writerIndex(kCheapPrepend)
	  {
	    assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
	  }

  size_t readableBytes() const
  { return m_writerIndex - m_readerIndex; }

  size_t writableBytes() const
  { return m_buffer.size() - m_writerIndex; }

  size_t prependableBytes() const
  { return m_readerIndex; }

  const char* peek() const
  { return begin() + m_readerIndex; }

  char* beginWrite()
  { return begin() + m_writerIndex; }

  void hasWritten(size_t len)
  {
    assert(len <= writableBytes());
    m_writerIndex += len;
  }

  void unwrite(size_t len)
  {
    assert(len <= readableBytes());
    m_writerIndex -= len;
  }
  
  // retrieve returns void, to prevent
  // string str(retrieve(readableBytes()), readableBytes());
  // the evaluation of two functions are unspecified
  void retrieve(size_t len)
  {
    assert(len <= readableBytes());
    if (len < readableBytes())
    {
      m_readerIndex += len;
    }
    else
    {
      retrieveAll();
    }
  }

  void retrieveAll()
  {
  	m_readerIndex = kCheapPrepend;
  	m_writerIndex = kCheapPrepend;
  }

/*  void append(const char* /*restrict data, size_t len)
  {
    ensureWritableBytes(len);
    std::copy(data, data+len, beginWrite());
    hasWritten(len);
  }

  ssize_t Buffer::readFd(int fd, int* savedErrno);
*/

private:
  char* begin()
  {return &*m_buffer.begin(); }

  const char* begin() const
  {return &*m_buffer.begin(); }

private:
	std::vector<char> m_buffer;
	size_t m_readerIndex;
	size_t m_writerIndex;
};


#endif  // _NET_BUFFER_H
