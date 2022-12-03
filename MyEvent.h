#ifndef MY_EVENT_H_
#define MY_EVENT_H_


#if _WIN32
#include "../common/MyCoreDef.h"

class MyEvent {

public:
  MyEvent();
  virtual ~MyEvent();

  int Init(BOOL manualReset, BOOL initialState);
  BOOL Set();
  BOOL Reset();
  BOOL Pulse();
  int  Wait(int milliseconds);

private:
  HANDLE m_Event;
};

#endif // _WIN32

#endif // MY_EVENT_H_