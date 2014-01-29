#include <iostream>
#include <string>

// For some reason, these need to be declared before we include io.hpp, which
// makes sense really (but is annoying).
std::string tostring(const std::string &);
std::string tostring(int);

#include "io.hpp"

std::string tostring(const std::string &s) {
  return s;
}
std::string tostring(int i) {
  return std::to_string(i);
}

int main() {
  io::format(std::cout,
             "First we have {1}; wait, before that was {0}. Finally, {2}.\n",
             "foo", 1, "bar");
  return 0;
}
