#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sp::util {

class JsonError : public std::runtime_error {
  public:
    explicit JsonError(const std::string& message) : std::runtime_error(message) {}
};

// Dependency-free JSON value used by config loading, model serialization and
// exports. nlohmann/json is wired into the CMake build as an optional
// dependency; this type keeps the project building on a bare toolchain, and it
// is the only place that would need swapping to adopt nlohmann everywhere.
// Object keys keep insertion order so dumps are stable and diffable.
class Json {
  public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    Json() = default;
    Json(bool value);
    Json(double value);
    Json(long long value);
    Json(int value);
    Json(const char* value);
    Json(std::string value);

    static Json array();
    static Json object();
    static Json parse(const std::string& text);
    static std::optional<Json> parseFile(const std::string& path);

    Type type() const { return type_; }
    bool isNull() const { return type_ == Type::Null; }
    bool isArray() const { return type_ == Type::Array; }
    bool isObject() const { return type_ == Type::Object; }

    bool asBool(bool fallback = false) const;
    double asNumber(double fallback = 0.0) const;
    long long asInt(long long fallback = 0) const;
    std::string asString(const std::string& fallback = "") const;

    bool contains(const std::string& key) const;
    // Missing keys read back as a null value, so callers can chain lookups and
    // rely on the as*() fallbacks instead of checking every level.
    const Json& at(const std::string& key) const;
    void set(const std::string& key, Json value);
    void push(Json value);

    const std::vector<Json>& elements() const { return array_; }
    const std::vector<std::pair<std::string, Json>>& entries() const { return object_; }
    std::size_t size() const;

    std::string dump(int indent = -1) const;

  private:
    explicit Json(Type type) : type_(type) {}
    void dumpTo(std::string& out, int indent, int depth) const;

    Type type_ = Type::Null;
    bool bool_ = false;
    double number_ = 0.0;
    std::string string_;
    std::vector<Json> array_;
    std::vector<std::pair<std::string, Json>> object_;
};

} // namespace sp::util
