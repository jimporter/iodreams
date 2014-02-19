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
typedef basic_formatter<char> formatter;

template<typename T, typename String = std::string>
class has_tostring {
  // It might be possible to simplify this, but it works.
  template<typename U>
  static constexpr auto has_tostring_(int, int, int, int, int, int) -> decltype(
      tostring<String>(std::declval<U>(), std::declval<const char*>()),
      tostring<String>(std::declval<U>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring<String>(std::declval<U>(), std::declval<const char*>())
    )>::value, "unexpected return type for tostring");
    static_assert(std::is_convertible<String, decltype(
      tostring<String>(std::declval<U>())
    )>::value, "unexpected return type for tostring");
    return 4 | 2 | 1;
  }
  template<typename U>
  static constexpr auto has_tostring_(int, int, int, int, int, ...) -> decltype(
      tostring(std::declval<U>(), std::declval<const char*>()),
      tostring(std::declval<U>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring(std::declval<U>(), std::declval<const char*>())
    )>::value, "unexpected return type for tostring");
    static_assert(std::is_convertible<String, decltype(
      tostring(std::declval<U>())
    )>::value, "unexpected return type for tostring");
    return 2 | 1;
  }

  template<typename U>
  static constexpr auto has_tostring_(int, int, int, int, ...) -> decltype(
      tostring<String>(std::declval<U>(), std::declval<const char*>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring<String>(std::declval<U>(), std::declval<const char*>())
    )>::value, "unexpected return type for tostring");
    return 4 | 2;
  }
  template<typename U>
  static constexpr auto has_tostring_(int, int, int, ...) -> decltype(
      tostring(std::declval<U>(), std::declval<const char*>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring(std::declval<U>(), std::declval<const char*>())
    )>::value, "unexpected return type for tostring");
    return 2;
  }

  template<typename U>
  static constexpr auto has_tostring_(int, int, ...) -> decltype(
      tostring<String>(std::declval<U>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring<String>(std::declval<U>())
    )>::value, "unexpected return type for tostring");
    return 4 | 1;
  }
  template<typename U>
  static constexpr auto has_tostring_(int, ...) -> decltype(
      tostring(std::declval<U>()),
      int()) {
    static_assert(std::is_convertible<String, decltype(
      tostring(std::declval<U>())
    )>::value, "unexpected return type for tostring");
    return 1;
  }

  template<typename U>
  static constexpr auto has_tostring_(...) -> int {
    return 0;
  }
public:
  // value is a bitfield describing what kind of tostring function we have:
  //   4 = tostring accepts a template for the return type
  //   2 = tostring accepts a format string
  //   1 = tostring accepts no format string
  enum { value = has_tostring_<T>(0, 0, 0, 0, 0, 0) };
};

namespace detail {
  // First, some helpers to call tostring whether or not it takes a template to
  // define the return type.
  template<typename String, typename T>
  inline auto tostring_(T &&t) -> typename std::enable_if<
    (has_tostring<T, String>::value & 4) == 0,
    decltype(tostring( std::declval<T>() ))
  >::type {
    return tostring(std::forward<T>(t));
  }

  template<typename String, typename T>
  inline auto tostring_(T &&t) -> typename std::enable_if<
    has_tostring<T, String>::value & 4,
    decltype(tostring<String>( std::declval<T>() ))
  >::type {
    return tostring<String>(std::forward<T>(t));
  }

  // Next, some helpers to delegate to the proper tostring based on whether
  // format args were provided and whether tostring accepts them.
  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    has_tostring<T, std::basic_string<Char, Traits>>::value == 0, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt = 0) {
    static_assert(has_tostring<T, std::basic_string<Char, Traits>>::value,
                  "no tostring function found");
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    has_tostring<T, std::basic_string<Char, Traits>>::value & 1, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t) {
    o << tostring_<std::basic_string<Char, Traits>>(t);
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    (has_tostring<T, std::basic_string<Char, Traits>>::value & 3) == 2, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t) {
    o << tostring_<std::basic_string<Char, Traits>>(t, "");
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    (has_tostring<T, std::basic_string<Char, Traits>>::value & 3) == 1, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt) {
    o << tostring_<std::basic_string<Char, Traits>>(t);
  }

  template<typename Char, typename Traits, typename T>
  inline typename std::enable_if<
    has_tostring<T, std::basic_string<Char, Traits>>::value & 2, void
  >::type
  try_format(std::basic_ostream<Char, Traits> &o, T &&t, const char *fmt) {
    o << tostring_<std::basic_string<Char, Traits>>(t, fmt);
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

// Not sure why I need this overload... you'd think the next one would be
// deduceable.
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
