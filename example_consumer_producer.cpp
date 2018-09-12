#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include "fco.h"

std::vector<int> vec;

void producer(fco::Scheduler* s, void* param) {
  while (vec.size() < 10) {
    int resource = rand();
    std::cout << "Producing " << resource << "\n";
    vec.push_back(resource);
  }
  fco::resume(s, (int)param);
  fco::yield(s);
}

void consumer(fco::Scheduler* s, void* param) {
  int producerCo = fco::newco(s, producer, (void*)fco::current(s));

  while (true) {
    while (!vec.empty()) {
      int resource = vec.back();
      vec.pop_back();
      std::cout << "Consuming " << resource << "\n";
    }
    fco::resume(s, producerCo);
  }
}

void factory() {
  fco::Scheduler* s = fco::initialize();
  int consumerCo = fco::newco(s, consumer, nullptr);
  fco::resume(s, consumerCo);

  fco::destroy(s);
}

int main() {
  srand((int)time(0));
  factory();
  system("pause");
  return 0;
}