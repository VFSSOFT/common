#include "MyEvent.h"

#include <Windows.h>

MyEvent::MyEvent() : m_Event(NULL) {
}

MyEvent::~MyEvent() {
  if (m_Event != NULL) {
    CloseHandle(m_Event);
  }
}

int MyEvent::Init(BOOL manualReset, BOOL initialState) {
  m_Event = CreateEvent(
    NULL,
    manualReset,
    initialState,
    NULL
  );

  if (m_Event == NULL) {
    return (int)GetLastError();
  }
  return 0;
}

BOOL MyEvent::Reset() {
  return ::ResetEvent(m_Event);
}
BOOL MyEvent::Set() {
  return ::SetEvent(m_Event);
}
BOOL MyEvent::Pulse() {
  return ::PulseEvent(m_Event);
}
int MyEvent::Wait(int milliseconds) {
  if (milliseconds < 0) milliseconds = INFINITE;
  return ::WaitForSingleObject(m_Event, milliseconds);
}

