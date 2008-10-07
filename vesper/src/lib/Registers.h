#pragma once

#include "Types.h"

extern "C"  Address readInstructionPointer();
extern "C"  Address readStackPointer();
extern "C"  Address readBasePointer();

extern "C"  void writeStackPointer(Address ptr);
extern "C"  void writeBasePointer(Address ptr);

extern "C"  Address readPageDirectory();
extern "C"  void writePageDirectory(Address pageDirPhysical);
extern "C"  void flushPageDirectory(void);

extern "C"  void enablePaging(void);
extern "C"  void enableInterrupts(void);
extern "C"  void disableInterrupts(void);

// defined in schedule/CriticalSection.cpp
extern "C" void criticalSection();
extern "C" void endCriticalSection();
