#include "fco.h"

// Initialize fco library, return a global scheduler
fco::Scheduler* fco::initialize() {
  Scheduler* sched = new Scheduler;
  sched->currentIdx = ERR_NOT_EXIST_CO;
  sched->main = ConvertThreadToFiber(NULL);
  return sched;
}

// Release all resources
void fco::destroy(Scheduler* s) {
  for (auto& c : s->coroutines) {
    DeleteFiber(c.second->winFiber);
  }
  delete s;
}

// This is should NEVER BE called on user land
void __stdcall fco::__entry(LPVOID lpParameter) {
  // Execute the task of current coroutine
  Scheduler* s = (Scheduler*)lpParameter;
  Coroutine* currentCo = s->coroutines[s->currentIdx];
  (currentCo->task)(s, currentCo->userData);

  // Clean up executed task
  s->coroutines.erase(s->coroutines.find(s->currentIdx));
  s->currentIdx = ERR_NOT_EXIST_CO;
  currentCo->status = Status::READY;
  DeleteFiber(currentCo->winFiber);
  delete currentCo;

  // Switch to entry function
  SwitchToFiber(s->main);
}

// Create new coroutine and return an unique identity
int fco::newco(Scheduler* scheduler, void (*task)(fco::Scheduler*, void*),
               void* userData) {
  Coroutine* co = new Coroutine;
  co->task = task;
  co->userData = userData;
  co->winFiber = CreateFiber(0, __entry, scheduler);
  if (co->winFiber == NULL) {
    return ERR_NOT_EXIST_CO;
  }
  co->status = Status::READY;
  int newCoId =
      scheduler->coroutines.size() != 0
          ? scheduler->coroutines.end().operator--().operator*().first + 1
          : 0;
  scheduler->coroutines.insert(std::make_pair(newCoId, co));
  return newCoId;
}

// Resume suspended coroutine by given coid
void fco::resume(fco::Scheduler* scheduler, int coid) {
  if (coid < 0) {
    return;
  }
  Coroutine* co = scheduler->coroutines[coid];
  if (co->status == Status::READY || co->status == Status::AWAIT) {
    scheduler->currentIdx = coid;
    scheduler->coroutines[scheduler->currentIdx]->status = Status::AWAIT;
    co->status = Status::READY;
    SwitchToFiber(co->winFiber);
  }
}

// Yield CPU time to main coroutine
void fco::yield(fco::Scheduler* scheduler) {
  Coroutine* co = scheduler->coroutines[scheduler->currentIdx];
  co->status = Status::AWAIT;

  scheduler->currentIdx = ERR_NOT_EXIST_CO;
  SwitchToFiber(scheduler->main);
}

// Get current running coroutine identity
int fco::current(Scheduler* scheduler) { return scheduler->currentIdx; }
