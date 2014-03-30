#ifndef INC_IODREAMS_TOSTRING_HPP
#define INC_IODREAMS_TOSTRING_HPP

namespace io {

// We can't use ADL for pre-defined types (including ones in std, since it's
// impolite to put stuff there). Thus, these need to be declared *before* we
// get to has_tostring in tostring_utils.hpp.

template<typename T = std::string>
T tostring(const std::string &s, const char *params = 0);

template<>
std::string tostring(const std::string &s, const char *params) {
  if (params && params[0] == 'q')
    return "beep";
  return s;
}

template<>
std::wstring tostring(const std::string &, const char *params) {
  if (params && params[0] == 'q')
    return L"weep";
  return std::wstring(L"wide");
}

inline std::string tostring(int i) {
  return std::to_string(i);
}

inline std::wstring tostring(double i) {
  return std::to_wstring(i);
}

} // namespace io

#include "tostring_utils.hpp"

#endif
