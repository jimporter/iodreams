/* caliber --name "has_tostring for tostring(arg, fmt)" -I../../include
   --std=c++11 */
#include <string>

namespace io {
  std::string tostring(void *, const char *);
}

#include "has_tostring_helpers.hpp"

int main() {
  // Check traits for tostring(void*, const char*)...

  // with the actual return type
  check_has_tostring<
    io::has_tostring<void*, std::string, false>,
    false, false, false, std::string, void*, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<void*, std::string, true>,
    true, true, false, std::string, void*, std::string, true, true
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<void*, string_proxy, false>,
    false, false, false, string_proxy, void*, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<void*, string_proxy, true>,
    true, true, false, string_proxy, void*, std::string, true, true
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<void*, bool, false>,
    false, false, false, bool, void*, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<void*, bool, true>,
    true, true, false, bool, void*, std::string, false, false
  >();
}
