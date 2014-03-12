#ifndef INC_IODREAMS_TOSTRING_UTILS_HPP
#define INC_IODREAMS_TOSTRING_UTILS_HPP

#include <type_traits>
#include <utility>

namespace io {

template<typename T, typename String, bool Formatted>
class has_tostring {
  template<class U> struct always_int { typedef int type; };
  template<class U> struct identity { typedef U type; };

  template<typename U>
  static constexpr typename always_int<
    decltype( tostring<String>(std::declval<U>()) )
  >::type check_args_(int, int) {
    return 2;
  }
  template<typename U>
  static constexpr typename always_int<
    decltype( tostring(std::declval<U>()) )
  >::type check_args_(int, ...) {
    return 1;
  }
  template<typename U>
  static constexpr int check_args_(...) {
    return 0;
  }

  template<typename U>
  static typename identity<
    decltype( tostring<String>(std::declval<U>()) )
  >::type return_type_(int, int);
  template<typename U>
  static typename identity<
    decltype( tostring(std::declval<U>()) )
  >::type return_type_(int, ...);
  template<typename U>
  static auto return_type_(...) -> void;
public:
  static const bool formatted = Formatted;
  static const bool exists = check_args_<T>(0, 0);
  static const bool templated = check_args_<T>(0, 0) == 2;

  using return_type = decltype(return_type_<T>(0, 0));
  using input_type = T;
  using string_type = String;
  static const bool compatible = std::is_convertible<
    return_type, string_type>::value;
  static const bool value = exists && compatible;
};

template<typename T, typename String>
class has_tostring<T, String, true> {
  template<class U> struct always_int { typedef int type; };
  template<class U> struct identity { typedef U type; };

  template<typename U>
  static constexpr typename always_int<
    decltype( tostring<String>(std::declval<U>(), std::declval<const char*>()) )
  >::type check_args_(int, int) {
    return 2;
  }
  template<typename U>
  static constexpr typename always_int<
    decltype( tostring(std::declval<U>(), std::declval<const char*>()) )
  >::type check_args_(int, ...) {
    return 1;
  }
  template<typename U>
  static constexpr int check_args_(...) {
    return 0;
  }

  template<typename U>
  static typename identity<
    decltype( tostring<String>(std::declval<U>(), std::declval<const char*>()) )
  >::type return_type_(int, int);
  template<typename U>
  static typename identity<
    decltype( tostring(std::declval<U>(), std::declval<const char*>()) )
  >::type return_type_(int, ...);
  template<typename U>
  static auto return_type_(...) -> void;
public:
  static const bool formatted = true;
  static const bool exists = check_args_<T>(0, 0);
  static const bool templated = check_args_<T>(0, 0) == 2;

  using string_type = String;
  using input_type = T;
  using return_type = decltype(return_type_<T>(0, 0));
  static const bool compatible = std::is_convertible<
    return_type, string_type>::value;
  static const bool value = exists && compatible;
};

// Some helpers to call tostring whether or not it takes a template to define
// the return type.

namespace detail {

  template<typename String, typename T>
  inline auto call_tostring_(T &&t) -> typename std::enable_if<
    !has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring(std::forward<T>(t));
  }

  template<typename String, typename T>
  inline auto call_tostring_(T &&t) -> typename std::enable_if<
    has_tostring<T, String, false>::templated,
    typename has_tostring<T, String, false>::return_type
  >::type {
    static_assert(has_tostring<T, String, false>::compatible,
                  "tostring function has incompatible return type");
    return tostring<String>(std::forward<T>(t));
  }

  template<typename String, typename T>
  inline auto call_tostring_(T &&t, const char *params) ->
    typename std::enable_if<
      !has_tostring<T, String, true>::templated,
      typename has_tostring<T, String, true>::return_type
  >::type {
    static_assert(has_tostring<T, String, true>::compatible,
                  "tostring function has incompatible return type");
    return tostring(std::forward<T>(t), params);
  }

  template<typename String, typename T>
  inline auto call_tostring_(T &&t, const char *params) ->
    typename std::enable_if<
      has_tostring<T, String, true>::templated,
      typename has_tostring<T, String, true>::return_type
  >::type {
    static_assert(has_tostring<T, String, true>::compatible,
                  "tostring function has incompatible return type");
    return tostring<String>(std::forward<T>(t), params);
  }

}

template<typename String, typename T>
inline auto call_tostring(T &&, const char * = 0) -> typename std::enable_if<
  !has_tostring<T, String, false>::value &&
  !has_tostring<T, String, true >::value,
  String
>::type {
  using unformatted = has_tostring<T, String, false>;
  using formatted = has_tostring<T, String, true>;
  static_assert(
    unformatted::exists || formatted::exists,
    "no tostring function found"
  );
  static_assert((!unformatted::exists && !formatted::exists) ||
                (unformatted::compatible || formatted::compatible),
                "tostring function has incompatible return type");
}

template<typename String, typename T>
inline auto call_tostring(T &&t, const char *params) -> typename std::enable_if<
  has_tostring<T, String, true>::value,
  typename has_tostring<T, String, true>::return_type
>::type {
  return detail::call_tostring_<String>(std::forward<T>(t), params);
}
template<typename String, typename T>
inline auto call_tostring(T &&t, const char *) -> typename std::enable_if<
  !has_tostring<T, String, true >::value &&
   has_tostring<T, String, false>::value,
  typename has_tostring<T, String, false>::return_type
>::type {
  return detail::call_tostring_<String>(std::forward<T>(t));
}

template<typename String, typename T>
inline auto call_tostring(T &&t) -> typename std::enable_if<
  has_tostring<T, String, false>::value,
  typename has_tostring<T, String, false>::return_type
>::type {
  return detail::call_tostring_<String>(std::forward<T>(t));
}
template<typename String, typename T>
inline auto call_tostring(T &&t) -> typename std::enable_if<
  !has_tostring<T, String, false>::value &&
   has_tostring<T, String, true >::value, void
>::type {
  return detail::call_tostring_<String>(std::forward<T>(t), nullptr);
}

} // namespace io

#endif
