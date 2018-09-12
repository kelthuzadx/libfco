#ifndef _MSC_VER
#error "this fast coroutine library only supports MSVC building chain"
#endif

#include <Windows.h>
#include <cstdint>
#include <map>

namespace fco {
static constexpr int ERR_NOT_EXIST_CO = -1;

enum Status {
  READY,  // Set up to READY when fco::newco() called
  AWAIT,  // Set up to AWAIT when fco::yield() called
};

struct Scheduler;
struct Coroutine;

struct Coroutine {
  void (*task)(Scheduler*, void*);
  void* userData;
  char status;
  LPVOID winFiber;
};

struct Scheduler {
  std::map<int, Coroutine*> coroutines;
  int currentIdx;
  LPVOID main;
};

void __stdcall __entry(LPVOID lpParameter);

Scheduler* initialize();

void destroy(Scheduler* s);

int newco(Scheduler* scheduler, void (*task)(Scheduler*, void*),
          void* userData);

void resume(Scheduler* s, int coid);

void yield(Scheduler* scheduler);

int current(Scheduler* scheduler);

}  // namespace fco
