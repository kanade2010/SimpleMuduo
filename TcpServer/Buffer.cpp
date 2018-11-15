#include "Buffer.hh"

const ssize_t kExtraBufferSize 65535

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
  // saved an ioctl()/FIONREAD call to tell how much to read
  char extrabuf[kExtraBufferSize];
  struct iovec vec[2];
  const size_t writable = writableBytes();
  vec[0].iov_base = begin() + m_writerIndex;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  // when there is enough space in this buffer, don't read into extrabuf.
  // when extrabuf is used, we read 128k-1 bytes at most.
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = sockets::readv(fd, vec, iovcnt);
  if (n < 0)
  {
    *savedErrno = errno;
  }
  else if (static_cast<size_t>(n) <= writable)
  {
    m_writerIndex += n;
  }
  else
  {
    m_writerIndex = buffer_.size();
    append(extrabuf, n - writable);
  }
  // if (n == writable + sizeof extrabuf)
  // {
  //   goto line_30;
  // }
  return n;
}

/*
int main()
{
  Buffer buffer;

  std::cout << buffer.writableBytes() << std::endl;
  strcpy(buffer.beginWrite(), "123456789");
  std::cout << buffer.writableBytes() << std::endl;
  buffer.hasWritten(9);
  std::cout << buffer.writableBytes() << std::endl;
  buffer.unwrite(4);
  std::cout << buffer.writableBytes() << std::endl;
  strcpy(buffer.beginWrite(), "123456789");
  std::cout << buffer.writableBytes() << std::endl;
  std::cout << buffer.peek() << std::endl;

}*/