// caliber --name "has_tostring for tostring<T>(arg)" -Iinclude --std=c++11
#include <string>

namespace io {
  template<typename T>
  std::string tostring(void *);
}

#include "has_tostring_helpers.hpp"

int main() {
  // Check traits for tostring<T>(void*)...

  // with a return type of std::string
  check_has_tostring<
    io::has_tostring<void*, std::string, false>,
    false, true, true, std::string, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, std::string, true>,
    true, false, false, std::string, void*, void, false, false
  >();

  // with a return type of bool
  check_has_tostring<
    io::has_tostring<void*, bool, false>,
    false, true, true, bool, void*, bool, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, bool, true>,
    true, false, false, bool, void*, void, false, false
  >();
}
