// caliber --name "has_tostring for tostring(arg, fmt=0)" -Iinclude --std=c++11
#include <string>

namespace io {
  std::string tostring(void *, const char * = 0);
}

#include "has_tostring_helpers.hpp"

int main() {
  // Check traits for tostring(void*, const char* = 0)...

  // with the actual return type
  check_has_tostring<
    io::has_tostring<void*, std::string, false>,
    false, true, false, std::string, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, std::string, true>,
    true, true, false, std::string, void*, std::string, true, true
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<void*, string_proxy, false>,
    false, true, false, string_proxy, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, string_proxy, true>,
    true, true, false, string_proxy, void*, std::string, true, true
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<void*, bool, false>,
    false, true, false, bool, void*, std::string, false, false
  >();
  check_has_tostring<
    io::has_tostring<void*, bool, true>,
    true, true, false, bool, void*, std::string, false, false
  >();
}
