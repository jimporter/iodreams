#ifndef INC_IO_HPP
#define INC_IO_HPP

#include <iostream>
#include <string>
#include <vector>

namespace io {

class formatter {
public:
  formatter(const std::string &fmt) {
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

  std::vector<std::string> strings_;
  std::vector<size_t> placeholders_;
};

template<typename T>
class has_tostring {
  template<typename U>
  static constexpr auto has_tostring_(int, int, int) -> decltype(
      tostring(std::declval<U>(), std::declval<const char*>()),
      tostring(std::declval<U>()),
      int()) {
    return 2 | 1;
  }
  template<typename U>
  static constexpr auto has_tostring_(int, int, ...) -> decltype(
      tostring(std::declval<U>(), std::declval<const char*>()),
      int()) {
    return 2;
  }
  template<typename U>
  static constexpr auto has_tostring_(int, ...) -> decltype(
      tostring(std::declval<U>()),
      int()) {
    return 1;
  }
  template<typename U>
  static constexpr auto has_tostring_(...) -> int {
    return 0;
  }
public:
  enum { value = has_tostring_<T>(0, 0, 0) };
};

namespace detail {
  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value == 0, void>::type
  try_format(std::ostream &o, T &&t) {
    o << "error!";
  }

  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value & 1, void>::type
  try_format(std::ostream &o, T &&t) {
    o << tostring(t);
  }

  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value == 2, void>::type
  try_format(std::ostream &o, T &&t) {
    o << tostring(t, "");
  }

  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value == 0, void>::type
  try_format(std::ostream &o, T &&t, const char *fmt) {
    o << "error!";
  }

  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value == 1, void>::type
  try_format(std::ostream &o, T &&t, const char *fmt) {
    o << tostring(t);
  }

  template<typename T>
  inline typename std::enable_if<has_tostring<T>::value & 2, void>::type
  try_format(std::ostream &o, T &&t, const char *fmt) {
    o << tostring(t, fmt);
  }

  template<typename ...Rest>
  inline void format_one(std::ostream &o, int i, Rest &&...args) {
    // This should never actually be called.
  }

  template<typename T, typename ...Rest>
  inline void format_one(std::ostream &o, int i, T &&x, Rest &&...args) {
    if(i == 0)
      try_format(o, x);
    else
      format_one(o, i-1, args...);
  }

  template<typename ...Rest>
  inline void format_one(std::ostream &o, const char *fmt, int i,
                         Rest &&...args) {
    // This should never actually be called.
  }

  template<typename T, typename ...Rest>
  inline void format_one(std::ostream &o, const char *fmt, int i, T &&x,
                         Rest &&...args) {
    if(i == 0)
      try_format(o, x, fmt);
    else
      format_one(o, i-1, args...);
  }
}

template<typename ...T>
void format(std::ostream &o, const std::string &fmt, T &&...args) {
  format(o, formatter(fmt), args...);
}

template<typename ...T>
void format(std::ostream &o, const formatter &fmt,
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
