#include <iostream>
#include <string>

// For some reason, these need to be declared before we include io.hpp, which
// makes sense really (but is annoying).
//std::string tostring(const std::string &);
template<typename T = std::string>
T tostring(const std::string &s) {
  return s;
}

template<>
std::wstring tostring(const std::string &s) {
  return std::wstring(L"wide");
}

std::string tostring(int);
std::wstring tostring(double);

#include "io.hpp"

std::string tostring(int i) {
  return std::to_string(i);
}
std::wstring tostring(double i) {
  return std::to_wstring(i);
}

int main() {
  io::format(std::cout,
             "First we have {1}; wait, before that was {0}. Finally, {2}.\n",
             "foo", 1, "bar");

  io::format(std::wcout,
             L"First we have {1}; wait, before that was {0}. Finally, {2}.\n",
             "foo", 1.0, "bar");

  return 0;
}
