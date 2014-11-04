#ifndef INC_IODREAMS_TEST_COMPILATION_HAS_TOSTRING_HELPERS_HPP
#define INC_IODREAMS_TEST_COMPILATION_HAS_TOSTRING_HELPERS_HPP

#include <string>
#include <type_traits>

namespace io {
  // At least one templated tostring function must exist for the traits to work.
  // Just make one that'll never actually be selected.
  template<typename = void>
  void tostring();
}

#include "iodreams/tostring_utils.hpp"

template<typename Traits, bool Formatted, bool Exists, bool Templated,
         typename StringType, typename InputType, typename ReturnType,
         bool Compatible, bool Value>
void check_has_tostring() {
  static_assert(Traits::formatted == Formatted,
                "unexpected has_tostring::formatted value");
  static_assert(Traits::exists == Exists,
                "unexpected has_tostring::exists value");
  static_assert(Traits::templated == Templated,
                "unexpected has_tostring::templated value");

  static_assert(std::is_same<typename Traits::string_type, StringType>::value,
                "unexpected has_tostring::string_type value");
  static_assert(std::is_same<typename Traits::input_type, InputType>::value,
                "unexpected has_tostring::input_type value");
  static_assert(std::is_same<typename Traits::return_type, ReturnType>::value,
                "unexpected has_tostring::return_type value");
  static_assert(Traits::compatible == Compatible,
                "unexpected has_tostring::compatible value");
  static_assert(Traits::value == Value,
                "unexpected has_tostring::value value");
}

struct string_proxy {
  string_proxy(const std::string &);
};

#endif
