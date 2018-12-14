#include "skip_list.hpp"

int main(void) {
  SkipList<size_t, int> skipList;
  skipList.insert(0, 1);
  skipList.insert(3, 2);
  skipList.insert(7, 3);
  skipList.insert(9, 4);
  skipList.insert(11, 5);
  skipList.insert(13, 6);

  std::cout << "13th element is " << skipList.find(13).value() << std::endl;

  skipList.erase(9);
  std::cout << "11th element is " << skipList.find(11).value() << std::endl;

  skipList.erase(3);
  skipList.print();

  return 0;
}

