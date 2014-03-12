#include <iostream>
#include <string>
#include <vector>

#include "format.hpp"

struct foo {};

std::string tostring(foo) {
  return "i am a foo";
}

int main() {
  io::format(std::cout,
             "First we have {1}; wait, before that was {0|q}. Finally, {2}.\n",
             "foo", 1, foo());

  io::format(std::wcout,
             L"First we have {1}; wait, before that was {0|q}. Finally, {2}.\n",
             "foo", 1.0, "bar");

  return 0;
}
