#include <iostream>
#include <cstdlib>

#include "thread_pool.hpp"

const int DEFAULT_M = 4;

void printUsage(const char *name) {
  std::cout << "Usage:" << std::endl
            << name << " N [M]" << std::endl
            << "N - length of array to sort" << std::endl
            << "M - amount of threads, default = " << DEFAULT_M << std::endl;
}

std::vector<unsigned> generateRandomArray(size_t N) {
  std::vector<unsigned> arr(N);

  srand(time(nullptr));
  for (auto &num : arr)
    num = (unsigned)rand() % 1000000;

  return arr;
}

template <class Container> void reportContainer(Container container) {
  std::cout << "[";

  for (auto value : container)
    std::cout << value << ", ";

  std::cout << "]";
}

template <class Iter> void mergeSort(Iter first, Iter last) {
  if (last - first > 1) {
    Iter middle = first + (last - first) / 2;
    mergeSort(first, middle); // [first, middle)
    mergeSort(middle, last);  // [middle, last)
    std::inplace_merge(first, middle, last);
  }
}

void sortUsingThreadPool(std::vector<unsigned> arr, ThreadPool &threadPool) {
  using Iter = decltype(arr)::iterator;
  using Futures = std::list<std::future<decltype(mergeSort(Iter(), Iter()))>>;

  //std::cout << "Array before sorting: ";
  //reportContainer(arr);
  //std::cout << std::endl;

  const size_t &N = threadPool.size();
  const size_t size = arr.size() / N;
  std::list<Iter> borders;
  borders.push_back(arr.begin());
  Futures futures;
  for (size_t i = 0; i < N; i++) {
    Iter &first = borders.back(), last = (i == N - 1) ? arr.end() : first + size;
    borders.push_back(last);

    futures.push_back(threadPool.addTask(mergeSort<Iter>, first, last));
  }

  while (borders.size() >= 3) {
    for (auto i = borders.begin(); i != borders.end(); ++i) {
      auto first = i;
      if (++i == borders.end())
        break;

      auto middle = i;
      if (++i == borders.end())
        break;

      auto &last = i;
      futures.push_back(threadPool.addTask(std::inplace_merge<Iter>, *first, *middle, *last));
      borders.erase(middle);
    }
  }

  for (auto &future : futures)
    future.wait();

  //std::cout << "Array after sorting: ";
  //reportContainer(arr);
  //std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  int N = std::atoi(argv[1]);
  if (N < 0) {
    std::cerr << "Error: expected N to be a positive number" << std::endl;
    printUsage(argv[0]);
  }

  int M = argc > 2 ? std::atoi(argv[2]) : DEFAULT_M;

  std::vector<unsigned> arr = generateRandomArray(N);
  ThreadPool blockingThreadPool(M);
  sortUsingThreadPool(arr, blockingThreadPool);

  return 0;
}

