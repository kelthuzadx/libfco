#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include "fco.h"

void bar(fco::Scheduler* s, void* param) {
  for (int i = 0; i < 5; i++) {
    std::cout << "world\n";
    fco::yield(s);
  }
}

int main() {
  fco::Scheduler* s = fco::initialize();
  int barFunc = fco::newco(s, bar, nullptr);
  for (int i = 0; i < 5; i++) {
    std::cout << "hello\n";
    fco::resume(s, barFunc);
  }
  fco::destroy(s);
  return 0;
}
