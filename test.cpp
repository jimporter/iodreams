#include <iostream>
#include <string>

// For some reason, these need to be declared before we include io.hpp, which
// makes sense really (but is annoying).
template<typename T = std::string>
T tostring(const std::string &s, const char *params = 0);

std::string tostring(int);

std::wstring tostring(double);

#include "io.hpp"

template<>
std::string tostring(const std::string &s, const char *params) {
  if (params && params[0] == 'q')
    return "beep";
  return s;
}

template<>
std::wstring tostring(const std::string &, const char *params) {
  if (params && params[0] == 'q')
    return L"weep";
  return std::wstring(L"wide");
}

std::string tostring(int i) {
  return std::to_string(i);
}

std::wstring tostring(double i) {
  return std::to_wstring(i);
}

int main() {
  io::format(std::cout,
             "First we have {1}; wait, before that was {0|q}. Finally, {2}.\n",
             "foo", 1, "bar");

  io::format(std::wcout,
             L"First we have {1}; wait, before that was {0|q}. Finally, {2}.\n",
             "foo", 1.0, "bar");

  return 0;
}
