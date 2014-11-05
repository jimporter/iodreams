// caliber --name "has_tostring for tostring(arg)" -I../../include --std=c++11
#include <string>

namespace io {
  std::string tostring(void *);
}

#include "has_tostring_helpers.hpp"

int main() {
  // Check traits for tostring(void*)...

  // with the actual return type
  check_has_tostring<
    io::has_tostring<void*, std::string, false>,
    false, true, false, std::string, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, std::string, true>,
    true, false, false, std::string, void*, void, false, false
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<void*, string_proxy, false>,
    false, true, false, string_proxy, void*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<void*, string_proxy, true>,
    true, false, false, string_proxy, void*, void, false, false
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<void*, bool, false>,
    false, true, false, bool, void*, std::string, false, false
  >();
  check_has_tostring<
    io::has_tostring<void*, bool, true>,
    true, false, false, bool, void*, void, false, false
  >();
}
