#ifndef _NET_CHANNEL_H
#define _NET_CHANNEL_H

#include <functional>


class EventLoop;

/*
 *每个Channel对象自始至终只属于一个EventLoop，
 *因此每个Channel对象都只属于某一个IO线程。 每个Channel对象自始至
 *终只负责一个文件描述符（fd） 的IO事件分发， 但它并不拥有这个fd，
 *也不会在析构的时候关闭这个fd。 Channel会把不同的IO事件分发为不
 *同的回调， 例如ReadCallback、 WriteCallback等
 */

class Channel {
public:
  typedef std::function<void()> EventCallBack;
  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent();
  void setReadCallBack(const EventCallBack& cb) { m_readCallBack = cb; }
  void setWriteCallBack(const EventCallBack& cb) { m_writeCallBack = cb; }
  void setErrorCallBack(const EventCallBack& cb) { m_errorCallBack = cb; }
  void setCloseCallBack(const EventCallBack& cb) { m_closeCallBack = cb; }

  int fd() const { return m_fd; }
  int events() const { return m_events; }
  void set_revents(int revt) { m_revents = revt; }
  bool isNoneEvent() const { return m_events == kNoneEvent; }

  void enableReading() { m_events |= kReadEvent; update(); }
  void disableReading() { m_events &= ~kReadEvent; update(); }
  void enableWriting() { m_events |= kWriteEvent; update(); }
  bool isWriting() { m_events &= kWriteEvent; }
  bool isReading() { m_events &= kReadEvent; }
  void disableWriting() { m_events &= ~kWriteEvent; update(); }
  void disableAll() { m_events = kNoneEvent; update(); }

  int index() { return m_index; }
  void set_index(int idx) { m_index =idx; }

  // for debug
  std::string reventsToString() const;
  std::string eventsToString() const;

  EventLoop* ownerLoop() { return p_loop; }
  void remove();

private:
  Channel& operator=(const Channel&);
  Channel(const Channel&);

  void update();

  //used for r/eventsToString()
  std::string eventsToString(int fd, int ev) const;

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop* p_loop;
  const int m_fd;
  int m_events;    // 等待的事件
  int m_revents;   // 实际发生了的事件
  int m_index;
  bool m_addedToLoop;

  EventCallBack m_readCallBack;
  EventCallBack m_writeCallBack;
  EventCallBack m_errorCallBack;
  EventCallBack m_closeCallBack;
};

#endif
