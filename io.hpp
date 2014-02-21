#ifndef INC_IO_HPP
#define INC_IO_HPP

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace io {

template<typename T>
class basic_formatter {
public:
  basic_formatter(const T &fmt) {
    size_t start = 0;
    for(size_t i = 0; i < fmt.size(); i++) {
      // Jesus Christ how horrifying
      if(fmt[i] == '{') {
        strings_.push_back(fmt.substr(start, i-start));
        placeholders_.push_back(fmt[i+1] - '0');
        i += 2; // Skip the number and the '}'
        start = i + 1;
      }
    }
    strings_.push_back(fmt.substr(start));
  }

  std::vector<T> strings_;
  std::vector<size_t> placeholders_;
};
typedef basic_formatter<std::string> formatter;
typedef basic_formatter<std::wstring> wformatter;

template<typename T, typename String, bool Formatted>
class has_tostring {
  template<typename U>
  static constexpr auto check_args_(int, int) -> decltype(
      tostring<String>(std::declval<U>()),
      int()) {
    return 2;
  }
  template<typename U>
  static constexpr auto check_args_(int, ...) -> decltype(
      tostring(std::declval<U>()),
      int()) {
    return 1;
  }
  template<typename U>
  static constexpr auto check_args_(...) -> int {
    return 0;
  }

  template<typename U>
  static auto return_type_(int, int) -> decltype(
    tostring<String>(std::declval<U>())
  );
  template<typename U>
  static auto return_type_(int, ...) -> decltype(
    tostring(std::declval<U>())
  );
  template<typename U>
  static auto return_type_(...) -> void;
public:
  static const bool formatted = Formatted;
  static const bool value = check_args_<T>(0, 0);
  static const bool templated = check_args_<T>(0, 0) == 2;

  using string_type = String;
  using return_type = decltype(return_type_<T>(0, 0));
  static const bool compatible = std::is_convertible<
    string_type, return_type>::value;
};

template<typename T, typename String>
class has_tostring<T, String, true> {
  template<typename U>
  static constexpr auto check_args_(int, int) -> decltype(
      tostring<String>(std::declval<U>(), std::declval<const char*>()),
      int()) {
    return 2;
  }
  template<typename U>
  static constexpr auto check_args_(int, ...) -> decltype(
      tostring(std::declval<U>(), std::declval<const char*>()),
      int()) {
    return 1;
  }
  template<typename U>
  static constexpr auto check_args_(...) -> int {
    return 0;
  }

  template<typename U>
  static auto return_type_(int, int) -> decltype(
    tostring<String>(std::declval<U>(), std::declval<const char*>())
  );
  template<typename U>
  static auto return_type_(int, ...) -> decltype(
    tostring(std::declval<U>(), std::declval<const char*>())
  );
  template<typename U>
  static auto return_type_(...) -> void;
public:
  static const bool formatted = true;
  static const bool value = check_args_<T>(0, 0);
  static const bool templated = check_args_<T>(0, 0) == 2;

  using input_type = String;
  using return_type = decltype(return_type_<T>(0, 0));
  static const bool compatible = std::is_convertible<
    input_type, return_type>::value;
};

namespace detail {
  // First, some helpers to call tostring whether or not it takes a template to
  // define the return type.
  template<typename String, typename T>
  inline auto call_tostring(T &&t) -> typename std::enable_if<
    !has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring(std::forward<T>(t));
  }

  template<typename String, typename T>
  inline auto call_tostring(T &&t) -> typename std::enable_if<
    has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring<String>(std::forward<T>(t));
  }

  template<typename String, typename T>
  inline auto call_tostring(T &&t, const char *fmt) -> typename std::enable_if<
    !has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring(std::forward<T>(t), fmt);
  }

  template<typename String, typename T>
  inline auto call_tostring(T &&t, const char *fmt) -> typename std::enable_if<
    has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring<String>(std::forward<T>(t), fmt);
  }

  // Next, some helpers to delegate to the proper tostring based on whether
  // format args were provided and whether tostring accepts them.
  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    !has_tostring<T, std::basic_string<Char, Traits>, false>::value &&
    !has_tostring<T, std::basic_string<Char, Traits>, true >::value, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt = 0) {
    static_assert(
      !has_tostring<T, std::basic_string<Char, Traits>, false>::value &&
      !has_tostring<T, std::basic_string<Char, Traits>, true >::value,
      "no tostring function found"
    );
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    has_tostring<T, std::basic_string<Char, Traits>, false>::value, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t) {
    o << call_tostring<std::basic_string<Char, Traits>>(t);
  }
  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    !has_tostring<T, std::basic_string<Char, Traits>, false>::value &&
     has_tostring<T, std::basic_string<Char, Traits>, true >::value, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t) {
    o << call_tostring<std::basic_string<Char, Traits>>(t, "");
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    has_tostring<T, std::basic_string<Char, Traits>, true>::value, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt) {
    o << call_tostring<std::basic_string<Char, Traits>>(t, fmt);
  }
  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    !has_tostring<T, std::basic_string<Char, Traits>, true >::value &&
     has_tostring<T, std::basic_string<Char, Traits>, false>::value, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt) {
    o << call_tostring<std::basic_string<Char, Traits>>(t);
  }

  // Finally, some helpers to return a formatted version of the i'th argument
  // in a template parameter pack.
  template<typename Char, typename Traits, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &o, int i,
                         Rest &&...args) {
    // This should never actually be called.
  }

  template<typename Char, typename Traits, typename T, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &o, int i, T &&x,
                         Rest &&...args) {
    if(i == 0)
      try_format(o, x);
    else
      format_one(o, i-1, args...);
  }

  template<typename Char, typename Traits, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &o, const char *fmt,
                         int i, Rest &&...args) {
    // This should never actually be called.
  }

  template<typename Char, typename Traits, typename T, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &o, const char *fmt,
                         int i, T &&x, Rest &&...args) {
    if(i == 0)
      try_format(o, x, fmt);
    else
      format_one(o, i-1, args...);
  }
}

template<typename Char, typename Traits, typename ...T>
inline void format(std::basic_ostream<Char, Traits> &o, const Char *fmt,
                   T &&...args) {
  format(o, basic_formatter<std::basic_string<Char, Traits>>(fmt), args...);
}

template<typename Char, typename Traits, typename ...T>
inline void format(std::basic_ostream<Char, Traits> &o,
                   const std::basic_string<Char, Traits> &fmt,
                   T &&...args) {
  format(o, basic_formatter<std::basic_string<Char, Traits>>(fmt), args...);
}

template<typename Char, typename Traits, typename ...T>
void format(std::basic_ostream<Char, Traits> &o,
            const basic_formatter<std::basic_string<Char, Traits>> &fmt,
            T &&...args) {
  auto str = fmt.strings_.begin();
  for(auto i : fmt.placeholders_) {
    o << *str;
    ++str;
    detail::format_one(o, i, args...);
  }
  o << *str;
}

} // namespace io

#endif
