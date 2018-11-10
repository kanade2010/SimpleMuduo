#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <functional>

#include "EventLoop.hh"

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

  int fd() const { return m_fd; }
  int events() const { return m_events; }
  void set_revents(int revt) { m_revents = revt; }
  bool isNoneEvent() const { return m_events == kNoneEvent; }

  void eableReading() { m_events |=  kReadEvent; update(); }

  int index() { return m_index; }
  void set_index(int idx) { m_index =idx; }

  EventLoop* ownerLoop() { return m_pLoop; }

private:
  Channel& operator=(const Channel&);
  Channel(const Channel&);

  void update();

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop* m_pLoop;
  const int m_fd;
  int m_events;    // 等待的事件
  int m_revents;   // 实际发生了的事件
  int m_index;

  EventCallBack m_readCallBack;
  EventCallBack m_writeCallBack;
  EventCallBack m_errorCallBack;
};

#endif
