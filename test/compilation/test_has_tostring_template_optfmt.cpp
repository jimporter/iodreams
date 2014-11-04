/* caliber --name "has_tostring for tostring<T>(arg, fmt=0)" -Iinclude
   --std=c++11 */
#include <string>

namespace io {
  template<typename T>
  std::string tostring(void *, const char * = 0);
}

#include "has_tostring_helpers.hpp"

int main() {
  // Check traits for tostring<T>(void*, const char* = 0)...

  // with a return type of std::string
  check_has_tostring<
    io::has_tostring<void*, std::string, false>,
    false, true, true, std::string, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, std::string, true>,
    true, true, true, std::string, void*, std::string, true, true
  >();

  // with a return type of bool
  check_has_tostring<
    io::has_tostring<void*, bool, false>,
    false, true, true, bool, void*, bool, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, bool, true>,
    true, true, true, bool, void*, bool, true, true
  >();
}
