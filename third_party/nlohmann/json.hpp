#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <initializer_list>

namespace nlohmann {

class json {
public:
  enum class Type { Null, Boolean, Integer, Double, String, Array, Object };

  using array_t = std::vector<json>;
  using object_t = std::map<std::string, json>;

  json() : data_(nullptr_t{}) {}
  json(std::nullptr_t) : data_(nullptr_t{}) {}
  json(bool v) : data_(v) {}
  json(int v) : data_(static_cast<int64_t>(v)) {}
  json(int64_t v) : data_(v) {}
  json(double v) : data_(v) {}
  json(const char* v) : data_(std::string(v)) {}
  json(const std::string& v) : data_(v) {}
  json(std::string&& v) : data_(std::move(v)) {}
  json(array_t v) : data_(std::move(v)) {}
  json(object_t v) : data_(std::move(v)) {}

  // Support brace-initializer lists like {{"key", val}, {"key2", val2}}
  json(std::initializer_list<std::pair<const char*, json>> init) {
    object_t obj;
    for (auto& p : init) {
      obj[std::string(p.first)] = p.second;
    }
    data_ = std::move(obj);
  }

  Type type() const { return static_cast<Type>(data_.index()); }
  bool is_null() const { return std::holds_alternative<nullptr_t>(data_); }
  bool is_boolean() const { return std::holds_alternative<bool>(data_); }
  bool is_number_integer() const { return std::holds_alternative<int64_t>(data_); }
  bool is_number_float() const { return std::holds_alternative<double>(data_); }
  bool is_string() const { return std::holds_alternative<std::string>(data_); }
  bool is_array() const { return std::holds_alternative<array_t>(data_); }
  bool is_object() const { return std::holds_alternative<object_t>(data_); }

  bool contains(const std::string& key) const {
    if (!is_object()) return false;
    return std::get<object_t>(data_).count(key) > 0;
  }

  // get<T>() method
  template<typename T> T get() const;

  // Value accessors
  template<typename T> T value(const std::string& key, T default_val) const;

  // Array access
  json& operator[](size_t idx) { return std::get<array_t>(data_)[idx]; }
  const json& operator[](size_t idx) const { return std::get<array_t>(data_)[idx]; }

  // Object access
  json& operator[](const std::string& key) {
    return std::get<object_t>(data_)[key];
  }
  const json& operator[](const std::string& key) const {
    static const json null_val;
    auto& obj = std::get<object_t>(data_);
    auto it = obj.find(key);
    return it != obj.end() ? it->second : null_val;
  }

  std::string dump(int indent = -1) const;
  static json parse(const std::string& s);
  static json parse(std::istream& is);

private:
  using nullptr_t = std::nullptr_t;
  using variant_t = std::variant<nullptr_t, bool, int64_t, double, std::string, array_t, object_t>;
  variant_t data_;

  const array_t& get_ref_array() const { return std::get<array_t>(data_); }
  const object_t& get_ref_object() const { return std::get<object_t>(data_); }
  const std::string& get_ref_string() const { return std::get<std::string>(data_); }
  int64_t get_ref_int() const { return std::get<int64_t>(data_); }
  double get_ref_double() const { return std::get<double>(data_); }
  bool get_ref_bool() const { return std::get<bool>(data_); }

  std::string dump_internal(int indent, int level) const;

  struct Parser {
    const std::string& s;
    size_t pos = 0;
    Parser(const std::string& str) : s(str) {}
    void skip_ws() { while (pos < s.size() && (s[pos]==' '||s[pos]=='\n'||s[pos]=='\r'||s[pos]=='\t')) pos++; }
    char peek() { return pos < s.size() ? s[pos] : '\0'; }
    char next() { return pos < s.size() ? s[pos++] : '\0'; }
    json parse_value();
    json parse_string();
    json parse_number();
    json parse_array();
    json parse_object();
    json parse_literal(const std::string& lit, json val);
  };
};

// get<T> specializations
template<> inline int64_t json::get<int64_t>() const { return get_ref_int(); }
template<> inline std::string json::get<std::string>() const { return get_ref_string(); }
template<> inline double json::get<double>() const { return get_ref_double(); }
template<> inline bool json::get<bool>() const { return get_ref_bool(); }

// value specializations
template<> inline int64_t json::value(const std::string& key, int64_t d) const {
  if (!is_object()) return d;
  auto& obj = get_ref_object();
  auto it = obj.find(key);
  return it != obj.end() && it->second.is_number_integer() ? it->second.get_ref_int() : d;
}
template<> inline std::string json::value(const std::string& key, std::string d) const {
  if (!is_object()) return d;
  auto& obj = get_ref_object();
  auto it = obj.find(key);
  return it != obj.end() && it->second.is_string() ? it->second.get_ref_string() : d;
}
template<> inline bool json::value(const std::string& key, bool d) const {
  if (!is_object()) return d;
  auto& obj = get_ref_object();
  auto it = obj.find(key);
  return it != obj.end() && it->second.is_boolean() ? it->second.get_ref_bool() : d;
}
template<> inline int json::value(const std::string& key, int d) const {
  return static_cast<int>(value<int64_t>(key, d));
}
template<> inline double json::value(const std::string& key, double d) const {
  if (!is_object()) return d;
  auto& obj = get_ref_object();
  auto it = obj.find(key);
  if (it == obj.end()) return d;
  if (it->second.is_number_float()) return it->second.get_ref_double();
  if (it->second.is_number_integer()) return static_cast<double>(it->second.get_ref_int());
  return d;
}

// dump
inline std::string json::dump(int indent) const { return dump_internal(indent, 0); }
inline std::string json::dump_internal(int indent, int level) const {
  std::ostringstream os;
  std::string pad = indent < 0 ? "" : std::string(level * indent, ' ');
  std::string pin = indent < 0 ? "" : std::string((level + 1) * indent, ' ');
  std::string nl = indent < 0 ? "" : "\n";
  switch (type()) {
    case Type::Null: os << "null"; break;
    case Type::Boolean: os << (get_ref_bool() ? "true" : "false"); break;
    case Type::Integer: os << get_ref_int(); break;
    case Type::Double: os << get_ref_double(); break;
    case Type::String: os << '"' << get_ref_string() << '"'; break;
    case Type::Array: {
      os << "[" << nl;
      auto& a = get_ref_array();
      for (size_t i = 0; i < a.size(); i++) {
        if (i) os << "," << nl;
        os << pin << a[i].dump_internal(indent, level + 1);
      }
      os << nl << pad << "]"; break;
    }
    case Type::Object: {
      os << "{" << nl;
      bool first = true;
      for (auto& [k, v] : get_ref_object()) {
        if (!first) os << "," << nl;
        first = false;
        os << pin << '"' << k << "\": " << v.dump_internal(indent, level + 1);
      }
      os << nl << pad << "}"; break;
    }
  }
  return os.str();
}

inline json json::parse(std::istream& is) {
  return parse(std::string(std::istreambuf_iterator<char>(is), {}));
}
inline json json::parse(const std::string& s) { Parser p(s); p.skip_ws(); return p.parse_value(); }
inline json json::Parser::parse_value() {
  skip_ws();
  char c = peek();
  if (c == '"') return parse_string();
  if (c == '[') return parse_array();
  if (c == '{') return parse_object();
  if (c == 't') return parse_literal("true", true);
  if (c == 'f') return parse_literal("false", false);
  if (c == 'n') return parse_literal("null", nullptr);
  return parse_number();
}
inline json json::Parser::parse_string() {
  next(); std::string r;
  while (pos < s.size() && peek() != '"') r += next();
  next(); return r;
}
inline json json::Parser::parse_number() {
  size_t st = pos;
  if (peek() == '-') pos++;
  while (pos < s.size() && (isdigit(peek()) || peek() == '.' || peek() == 'e' || peek() == 'E' || peek() == '+' || peek() == '-')) pos++;
  std::string n = s.substr(st, pos - st);
  if (n.find('.') != std::string::npos || n.find('e') != std::string::npos || n.find('E') != std::string::npos)
    return std::stod(n);
  return static_cast<int64_t>(std::stoll(n));
}
inline json json::Parser::parse_array() {
  next(); array_t a; skip_ws();
  if (peek() == ']') { next(); return a; }
  while (true) { a.push_back(parse_value()); skip_ws(); if (peek() == ']') { next(); break; } next(); }
  return a;
}
inline json json::Parser::parse_object() {
  next(); object_t o; skip_ws();
  if (peek() == '}') { next(); return o; }
  while (true) { std::string k = parse_string().get_ref_string(); skip_ws(); next(); o[k] = parse_value(); skip_ws(); if (peek() == '}') { next(); break; } next(); }
  return o;
}
inline json json::Parser::parse_literal(const std::string& lit, json val) {
  for (size_t i = 0; i < lit.size(); i++) { if (peek() != lit[i]) throw std::runtime_error("JSON parse error"); next(); }
  return val;
}

} // namespace nlohmann
