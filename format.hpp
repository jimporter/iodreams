#ifndef INC_IODREAMS_FORMAT_HPP
#define INC_IODREAMS_FORMAT_HPP

#include <cassert>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "tostring.hpp"

namespace io {

namespace detail {
  // Convert an arbitrary string type to an std::string, assuming all code
  // points are already ASCII.
  template<typename T>
  inline std::string charify(const T &s) {
    return std::string(s.begin(), s.end());
  }

  inline std::string charify(const std::string &s) {
    return s;
  }
}

class format_string_error : public std::invalid_argument {
  using std::invalid_argument::invalid_argument;
};

template<typename T>
class basic_format_string {
public:
  typedef T string_type;

  struct placeholder {
    placeholder(size_t index) : index(index) {}
    placeholder(size_t index, const std::string &params)
      : index(index), params(params) {}
    placeholder(size_t index, std::string &&params)
      : index(index), params(std::move(params)) {}

    size_t index;
    std::string params;
  };

  basic_format_string(const T &fmt) {
    enum parse_state {
      START,
      OPEN_BRACE,
      CLOSE_BRACE,
      PH_INDEX,
      PH_PARAMS
    };

    parse_state state = START;
    size_t start = 0, ph_index;
    string_type working_string;
    for(size_t i = 0; i < fmt.size(); i++) {
      switch(state) {
      case START:
        if(fmt[i] == '{') {
          state = OPEN_BRACE;
          working_string += fmt.substr(start, i - start);
        }
        else if(fmt[i] == '}') {
          state = CLOSE_BRACE;
          working_string += fmt.substr(start, i - start);
        }
        break;
      case OPEN_BRACE:
        if(fmt[i] == '{') {
          state = START;
          start = i;
        }
        else if(std::isdigit(fmt[i])) {
          state = PH_INDEX;
          ph_index = fmt[i] - '0';
        }
        else if(fmt[i] == '|') {
          ph_index = 0; // TODO: infer the index in a smarter way
          state = PH_PARAMS;
          start = i + 1;
        }
        else if(fmt[i] == '}') {
          ph_index = 0; // TODO: infer the index in a smarter way
          strings_.push_back(working_string);
          working_string = string_type();
          placeholders_.push_back({ ph_index });
          state = START;
          start = i + 1;
        }
        else {
          throw format_string_error("unexpected character in placeholder");
        }
        break;
      case CLOSE_BRACE:
        if(fmt[i] == '}') {
          state = START;
          start = i;
        }
        else {
          throw format_string_error("unexpected '}' in string");
        }
        break;
      case PH_INDEX:
        if(std::isdigit(fmt[i])) {
          ph_index = ph_index * 10 + fmt[i] - '0';
        }
        else if(fmt[i] == '|') {
          state = PH_PARAMS;
          start = i + 1;
        }
        else if(fmt[i] == '}') {
          strings_.push_back(working_string);
          working_string = string_type();
          placeholders_.push_back({ ph_index });
          state = START;
          start = i + 1;
        }
        else {
          throw format_string_error(
            "unexpected character in placeholder index"
          );
        }
        break;
      case PH_PARAMS:
        if(fmt[i] == '}') {
          strings_.push_back(working_string);
          working_string = string_type();
          placeholders_.push_back({
            ph_index, detail::charify(fmt.substr(start, i - start))
          });
          state = START;
          start = i + 1;
        }
        else if(fmt[i] > 127) {
          throw format_string_error(
            "unexpected character in placeholder params"
          );
        }
        break;
      default:
        assert(false && "invalid parser state");
      }
    }

    if(state != START)
      throw format_string_error("unexpected end of string");
    working_string += fmt.substr(start);
    strings_.push_back(working_string);
  }

  std::vector<string_type> strings_;
  std::vector<placeholder> placeholders_;
};
typedef basic_format_string<std::string> format_string;
typedef basic_format_string<std::wstring> wformat_string;

namespace detail {
  // Some helpers to return a formatted version of the i'th argument in a
  // template parameter pack.
  template<typename Char, typename Traits, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &, size_t,
                         const std::string &, Rest &&...) {
    assert(false && "format_one() failed somehow!");
  }

  template<typename Char, typename Traits, typename T, typename ...Rest>
  inline void format_one(std::basic_ostream<Char, Traits> &o, size_t i,
                         const std::string &params, T &&x, Rest &&...args) {
    if(i == 0) {
      using String = std::basic_string<Char, Traits>;
      // XXX: Maybe use boost::optional here?
      o << (params.size() ? call_tostring<String>(x, params) :
                            call_tostring<String>(x));
    }
    else {
      format_one(o, i-1, params, args...);
    }
  }
}

template<typename Char, typename Traits, typename ...T>
inline void format(std::basic_ostream<Char, Traits> &o, const Char *fmt,
                   T &&...args) {
  format(o, basic_format_string<std::basic_string<Char, Traits>>(fmt), args...);
}

template<typename Char, typename Traits, typename ...T>
inline void format(std::basic_ostream<Char, Traits> &o,
                   const std::basic_string<Char, Traits> &fmt,
                   T &&...args) {
  format(o, basic_format_string<std::basic_string<Char, Traits>>(fmt), args...);
}

template<typename Char, typename Traits, typename ...T>
void format(std::basic_ostream<Char, Traits> &o,
            const basic_format_string<std::basic_string<Char, Traits>> &fmt,
            T &&...args) {
  auto str = fmt.strings_.begin();
  for(auto ph : fmt.placeholders_) {
    o << *str;
    ++str;
    detail::format_one(o, ph.index, ph.params, args...);
  }
  o << *str;
}

} // namespace io

#endif
