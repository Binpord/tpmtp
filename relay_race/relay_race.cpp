#include "race_runner.hpp"
#include <QCoreApplication>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void print_usage(char *progname) {
  std::cout << "Usage:" << std::endl
            << progname << " N" << std::endl
            << "N - amount of threads in race" << std::endl;
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  int N = atoi(argv[1]); // amount of threads
  if (N < 0) {
    print_usage(argv[0]);
    return 2;
  }

  RaceRunner runner(N);
  runner.tuneRunner();
  RaceRunnerThread th_runner;
  runner.moveToThread(&th_runner);
  th_runner.start();
  th_runner.wait();

  return 0;
}
