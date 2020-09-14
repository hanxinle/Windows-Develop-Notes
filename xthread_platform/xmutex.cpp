#include "xmutex.h"
#include <windows.h>

xmutex::xmutex() {
    section = new CRITICAL_SECTION();
    InitializeCriticalSection((CRITICAL_SECTION*)this->section);
}

xmutex::~xmutex() {
    DeleteCriticalSection((CRITICAL_SECTION*)this->section);
}

void xmutex::lock() {
    EnterCriticalSection((CRITICAL_SECTION*)this->section);
}

void xmutex::unlock() {
    LeaveCriticalSection((CRITICAL_SECTION*)this->section);
}