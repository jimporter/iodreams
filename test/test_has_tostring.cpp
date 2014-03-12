#include <string>
#include <type_traits>

struct string_proxy {
  string_proxy(const std::string &);
};

namespace io {
  template<typename = void>
  void tostring();

  std::string tostring(int*);
  std::string tostring(int**, const char*);
  std::string tostring(int***, const char* = 0);

  template<typename T>
  T tostring(int****);
  template<typename T>
  T tostring(int*****, const char *);
  template<typename T>
  T tostring(int******, const char * = 0);
}

#include "../tostring_utils.hpp"

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

int main() {
  // ***** Check traits for tostring(int*) *****

  // with the actual return type
  check_has_tostring<
    io::has_tostring<int*, std::string, false>,
    false, true, false, std::string, int*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int*, std::string, true>,
    true, false, false, std::string, int*, void, false, false
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<int*, string_proxy, false>,
    false, true, false, string_proxy, int*, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int*, string_proxy, true>,
    true, false, false, string_proxy, int*, void, false, false
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<int*, bool, false>,
    false, true, false, bool, int*, std::string, false, false
  >();
  check_has_tostring<
    io::has_tostring<int*, bool, true>,
    true, false, false, bool, int*, void, false, false
  >();

  // ***** Check traits for tostring(int**, const char*) *****

  // with the actual return type
  check_has_tostring<
    io::has_tostring<int**, std::string, false>,
    false, false, false, std::string, int**, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<int**, std::string, true>,
    true, true, false, std::string, int**, std::string, true, true
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<int**, string_proxy, false>,
    false, false, false, string_proxy, int**, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<int**, string_proxy, true>,
    true, true, false, string_proxy, int**, std::string, true, true
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<int**, bool, false>,
    false, false, false, bool, int**, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<int**, bool, true>,
    true, true, false, bool, int**, std::string, false, false
    >();


  // ***** Check traits for tostring(int***, const char* = 0) *****

  // with the actual return type
  check_has_tostring<
    io::has_tostring<int***, std::string, false>,
    false, true, false, std::string, int***, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int***, std::string, true>,
    true, true, false, std::string, int***, std::string, true, true
  >();

  // with a compatible return type
  check_has_tostring<
    io::has_tostring<int***, string_proxy, false>,
    false, true, false, string_proxy, int***, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int***, string_proxy, true>,
    true, true, false, string_proxy, int***, std::string, true, true
  >();

  // with an incompatible return type
  check_has_tostring<
    io::has_tostring<int***, bool, false>,
    false, true, false, bool, int***, std::string, false, false
  >();
  check_has_tostring<
    io::has_tostring<int***, bool, true>,
    true, true, false, bool, int***, std::string, false, false
    >();

  // ***** Check traits for tostring<T>(int****) *****

  // with a return type of std::string
  check_has_tostring<
    io::has_tostring<int****, std::string, false>,
    false, true, true, std::string, int****, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int****, std::string, true>,
    true, false, false, std::string, int****, void, false, false
  >();

  // with a return type of bool
  check_has_tostring<
    io::has_tostring<int****, bool, false>,
    false, true, true, bool, int****, bool, true, true
  >();
  check_has_tostring<
    io::has_tostring<int****, bool, true>,
    true, false, false, bool, int****, void, false, false
  >();

  // ***** Check traits for tostring<T>(int*****, const char *) *****

  // with a return type of std::string
  check_has_tostring<
    io::has_tostring<int*****, std::string, false>,
    false, false, false, std::string, int*****, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<int*****, std::string, true>,
    true, true, true, std::string, int*****, std::string, true, true
  >();

  // with a return type of bool
  check_has_tostring<
    io::has_tostring<int*****, bool, false>,
    false, false, false, bool, int*****, void, false, false
  >();
  check_has_tostring<
    io::has_tostring<int*****, bool, true>,
    true, true, true, bool, int*****, bool, true, true
  >();

  // ***** Check traits for tostring<T>(int******, const char * = 0) *****

  // with a return type of std::string
  check_has_tostring<
    io::has_tostring<int******, std::string, false>,
    false, true, true, std::string, int******, std::string, true, true
  >();
  check_has_tostring<
    io::has_tostring<int******, std::string, true>,
    true, true, true, std::string, int******, std::string, true, true
  >();

  // with a return type of bool
  check_has_tostring<
    io::has_tostring<int******, bool, false>,
    false, true, true, bool, int******, bool, true, true
  >();
  check_has_tostring<
    io::has_tostring<int******, bool, true>,
    true, true, true, bool, int******, bool, true, true
  >();
}
