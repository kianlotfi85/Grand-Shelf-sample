#pragma once
#include <string>
#include <format>
#include <string_view>
#include <iostream>
#include <sstream>
#include <cstdio>

namespace fmt {

template<typename... Args>
std::string format(std::format_string<Args...> fmt_str, Args&&... args) {
  return std::format(fmt_str, std::forward<Args>(args)...);
}

template<typename... Args>
void print(std::format_string<Args...> fmt_str, Args&&... args) {
  std::cout << std::format(fmt_str, std::forward<Args>(args)...);
}

template<typename... Args>
void print(std::ostream& os, std::format_string<Args...> fmt_str, Args&&... args) {
  os << std::format(fmt_str, std::forward<Args>(args)...);
}

template<typename... Args>
void print(FILE* f, std::format_string<Args...> fmt_str, Args&&... args) {
  std::string s = std::format(fmt_str, std::forward<Args>(args)...);
  fwrite(s.c_str(), 1, s.size(), f);
}

namespace detail {
  inline std::string ansi(int code) { return "\033[" + std::to_string(code) + "m"; }
  inline std::string reset_ansi() { return "\033[0m"; }
}

struct text_style {
  int fg_code = 0;
  int em_code = 0;
  std::string wrap(const std::string& s) const {
    std::string r;
    if (em_code) r += detail::ansi(em_code);
    if (fg_code) r += detail::ansi(fg_code);
    r += s;
    if (fg_code || em_code) r += detail::reset_ansi();
    return r;
  }
};

inline text_style emphasis(int e) { text_style s; s.em_code = e; return s; }
inline text_style operator|(text_style a, text_style b) { text_style s; s.fg_code = a.fg_code ? a.fg_code : b.fg_code; s.em_code = a.em_code ? a.em_code : b.em_code; return s; }
inline text_style fg(int c) { text_style s; s.fg_code = c; return s; }

// Style-aware print overloads
template<typename... Args>
void print(text_style style, std::format_string<Args...> fmt_str, Args&&... args) {
  std::string s = std::format(fmt_str, std::forward<Args>(args)...);
  std::cout << style.wrap(s);
}

template<typename... Args>
void print(std::ostream& os, text_style style, std::format_string<Args...> fmt_str, Args&&... args) {
  std::string s = std::format(fmt_str, std::forward<Args>(args)...);
  os << style.wrap(s);
}

namespace color {
  constexpr int red = 31, green = 32, yellow = 33, blue = 34, cyan = 36, white = 37;
}

namespace emphasis_val { inline text_style bold = emphasis(1);
}

} // namespace fmt
