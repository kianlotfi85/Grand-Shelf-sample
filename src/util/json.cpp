#include "util/json.h"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace sp::util {
namespace {

const Json& nullValue() {
    static const Json value;
    return value;
}

void appendShortEscape(std::string& out, char code) {
    out.push_back('\\');
    out.push_back(code);
}

void appendQuoted(std::string& out, const std::string& text) {
    out.push_back('"');
    for (const char raw : text) {
        const unsigned char ch = static_cast<unsigned char>(raw);
        switch (ch) {
        case '"':
            appendShortEscape(out, '"');
            break;
        case '\\':
            appendShortEscape(out, '\\');
            break;
        case '\n':
            appendShortEscape(out, 'n');
            break;
        case '\r':
            appendShortEscape(out, 'r');
            break;
        case '\t':
            appendShortEscape(out, 't');
            break;
        case '\b':
            appendShortEscape(out, 'b');
            break;
        case '\f':
            appendShortEscape(out, 'f');
            break;
        default:
            if (ch < 0x20) {
                char buffer[8] = {};
                std::snprintf(buffer, sizeof(buffer), "u%04x", ch);
                out.push_back('\\');
                out += buffer;
            } else {
                out.push_back(raw);
            }
            break;
        }
    }
    out.push_back('"');
}

class Parser {
  public:
    explicit Parser(const std::string& text) : text_(text) {}

    Json run() {
        skipSpace();
        Json value = parseValue();
        skipSpace();
        if (pos_ != text_.size()) {
            fail("trailing characters");
        }
        return value;
    }

  private:
    [[noreturn]] void fail(const std::string& message) const {
        throw JsonError("json: " + message + " at offset " + std::to_string(pos_));
    }

    static bool isSpace(char ch) {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }

    void skipSpace() {
        while (pos_ < text_.size() && isSpace(text_[pos_])) {
            ++pos_;
        }
    }

    char peek() const {
        if (pos_ >= text_.size()) {
            fail("unexpected end of input");
        }
        return text_[pos_];
    }

    void expect(const char* literal) {
        const std::string text(literal);
        if (text_.compare(pos_, text.size(), text) != 0) {
            fail("invalid literal");
        }
        pos_ += text.size();
    }

    Json parseValue() {
        switch (peek()) {
        case '{':
            return parseObject();
        case '[':
            return parseArray();
        case '"':
            return Json(parseString());
        case 't':
            expect("true");
            return Json(true);
        case 'f':
            expect("false");
            return Json(false);
        case 'n':
            expect("null");
            return Json();
        default:
            return parseNumber();
        }
    }

    Json parseObject() {
        Json result = Json::object();
        ++pos_;
        skipSpace();
        if (peek() == '}') {
            ++pos_;
            return result;
        }
        while (true) {
            skipSpace();
            if (peek() != '"') {
                fail("expected object key");
            }
            const std::string key = parseString();
            skipSpace();
            if (peek() != ':') {
                fail("expected colon after object key");
            }
            ++pos_;
            skipSpace();
            result.set(key, parseValue());
            skipSpace();
            const char ch = peek();
            if (ch == ',') {
                ++pos_;
                continue;
            }
            if (ch == '}') {
                ++pos_;
                return result;
            }
            fail("expected comma or closing brace");
        }
    }

    Json parseArray() {
        Json result = Json::array();
        ++pos_;
        skipSpace();
        if (peek() == ']') {
            ++pos_;
            return result;
        }
        while (true) {
            skipSpace();
            result.push(parseValue());
            skipSpace();
            const char ch = peek();
            if (ch == ',') {
                ++pos_;
                continue;
            }
            if (ch == ']') {
                ++pos_;
                return result;
            }
            fail("expected comma or closing bracket");
        }
    }

    std::string parseString() {
        ++pos_;
        std::string out;
        while (true) {
            if (pos_ >= text_.size()) {
                fail("unterminated string");
            }
            const char ch = text_[pos_++];
            if (ch == '"') {
                return out;
            }
            if (ch != '\\') {
                out.push_back(ch);
                continue;
            }
            if (pos_ >= text_.size()) {
                fail("unterminated escape sequence");
            }
            const char escape = text_[pos_++];
            switch (escape) {
            case '"':
                out.push_back('"');
                break;
            case '\\':
                out.push_back('\\');
                break;
            case '/':
                out.push_back('/');
                break;
            case 'b':
                out.push_back('\b');
                break;
            case 'f':
                out.push_back('\f');
                break;
            case 'n':
                out.push_back('\n');
                break;
            case 'r':
                out.push_back('\r');
                break;
            case 't':
                out.push_back('\t');
                break;
            case 'u':
                appendCodepoint(out);
                break;
            default:
                fail("unknown escape sequence");
            }
        }
    }

    // Basic multilingual plane only; surrogate pairs are passed through as two
    // separate code points, which is enough for config and export payloads.
    void appendCodepoint(std::string& out) {
        if (pos_ + 4 > text_.size()) {
            fail("truncated unicode escape");
        }
        unsigned int code = 0;
        for (int i = 0; i < 4; ++i) {
            const char ch = text_[pos_++];
            code *= 16;
            if (ch >= '0' && ch <= '9') {
                code += static_cast<unsigned int>(ch - '0');
            } else if (ch >= 'a' && ch <= 'f') {
                code += static_cast<unsigned int>(ch - 'a' + 10);
            } else if (ch >= 'A' && ch <= 'F') {
                code += static_cast<unsigned int>(ch - 'A' + 10);
            } else {
                fail("invalid hex digit in unicode escape");
            }
        }
        if (code < 0x80) {
            out.push_back(static_cast<char>(code));
        } else if (code < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (code >> 6)));
            out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xE0 | (code >> 12)));
            out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        }
    }

    Json parseNumber() {
        const std::size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        while (pos_ < text_.size()) {
            const char ch = text_[pos_];
            const bool numeric = std::isdigit(static_cast<unsigned char>(ch)) != 0 ||
                                 ch == '.' || ch == 'e' || ch == 'E' || ch == '+' || ch == '-';
            if (!numeric) {
                break;
            }
            ++pos_;
        }
        if (pos_ == start) {
            fail("invalid number");
        }
        try {
            return Json(std::stod(text_.substr(start, pos_ - start)));
        } catch (const std::exception&) {
            fail("invalid number");
        }
    }

    const std::string& text_;
    std::size_t pos_ = 0;
};

} // namespace

Json::Json(bool value) : type_(Type::Bool), bool_(value) {}

Json::Json(double value) : type_(Type::Number), number_(value) {}

Json::Json(long long value) : type_(Type::Number), number_(static_cast<double>(value)) {}

Json::Json(int value) : Json(static_cast<long long>(value)) {}

Json::Json(const char* value) : type_(Type::String), string_(value == nullptr ? "" : value) {}

Json::Json(std::string value) : type_(Type::String), string_(std::move(value)) {}

Json Json::array() { return Json(Type::Array); }

Json Json::object() { return Json(Type::Object); }

Json Json::parse(const std::string& text) {
    Parser parser(text);
    return parser.run();
}

std::optional<Json> Json::parseFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return std::nullopt;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return parse(buffer.str());
}

bool Json::asBool(bool fallback) const {
    switch (type_) {
    case Type::Bool:
        return bool_;
    case Type::Number:
        return number_ != 0.0;
    case Type::String:
        return string_ == "true" || string_ == "1" || string_ == "yes";
    default:
        return fallback;
    }
}

double Json::asNumber(double fallback) const {
    if (type_ == Type::Number) {
        return number_;
    }
    if (type_ == Type::Bool) {
        return bool_ ? 1.0 : 0.0;
    }
    if (type_ == Type::String) {
        try {
            return std::stod(string_);
        } catch (const std::exception&) {
            return fallback;
        }
    }
    return fallback;
}

long long Json::asInt(long long fallback) const {
    if (type_ == Type::Null || type_ == Type::Array || type_ == Type::Object) {
        return fallback;
    }
    return static_cast<long long>(std::llround(asNumber(static_cast<double>(fallback))));
}

std::string Json::asString(const std::string& fallback) const {
    if (type_ == Type::String) {
        return string_;
    }
    if (type_ == Type::Null) {
        return fallback;
    }
    return dump();
}

bool Json::contains(const std::string& key) const {
    for (const auto& entry : object_) {
        if (entry.first == key) {
            return true;
        }
    }
    return false;
}

const Json& Json::at(const std::string& key) const {
    for (const auto& entry : object_) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    return nullValue();
}

void Json::set(const std::string& key, Json value) {
    if (type_ != Type::Object) {
        type_ = Type::Object;
    }
    for (auto& entry : object_) {
        if (entry.first == key) {
            entry.second = std::move(value);
            return;
        }
    }
    object_.emplace_back(key, std::move(value));
}

void Json::push(Json value) {
    if (type_ != Type::Array) {
        type_ = Type::Array;
    }
    array_.push_back(std::move(value));
}

std::size_t Json::size() const {
    if (type_ == Type::Array) {
        return array_.size();
    }
    if (type_ == Type::Object) {
        return object_.size();
    }
    return 0;
}

std::string Json::dump(int indent) const {
    std::string out;
    dumpTo(out, indent, 0);
    return out;
}

void Json::dumpTo(std::string& out, int indent, int depth) const {
    const bool pretty = indent >= 0;
    const std::string pad =
        pretty ? std::string(static_cast<std::size_t>(indent * (depth + 1)), ' ') : std::string();
    const std::string closePad =
        pretty ? std::string(static_cast<std::size_t>(indent * depth), ' ') : std::string();

    switch (type_) {
    case Type::Null:
        out += "null";
        break;
    case Type::Bool:
        out += bool_ ? "true" : "false";
        break;
    case Type::Number:
        if (std::isfinite(number_) && number_ == std::floor(number_) &&
            std::fabs(number_) < 1e15) {
            out += std::to_string(static_cast<long long>(number_));
        } else {
            char buffer[32] = {};
            std::snprintf(buffer, sizeof(buffer), "%.10g", number_);
            out += buffer;
        }
        break;
    case Type::String:
        appendQuoted(out, string_);
        break;
    case Type::Array:
        if (array_.empty()) {
            out += "[]";
            break;
        }
        out += pretty ? "[\n" : "[";
        for (std::size_t i = 0; i < array_.size(); ++i) {
            if (i != 0) {
                out += pretty ? ",\n" : ",";
            }
            out += pad;
            array_[i].dumpTo(out, indent, depth + 1);
        }
        if (pretty) {
            out += "\n";
        }
        out += closePad;
        out += "]";
        break;
    case Type::Object:
        if (object_.empty()) {
            out += "{}";
            break;
        }
        out += pretty ? "{\n" : "{";
        for (std::size_t i = 0; i < object_.size(); ++i) {
            if (i != 0) {
                out += pretty ? ",\n" : ",";
            }
            out += pad;
            appendQuoted(out, object_[i].first);
            out += pretty ? ": " : ":";
            object_[i].second.dumpTo(out, indent, depth + 1);
        }
        if (pretty) {
            out += "\n";
        }
        out += closePad;
        out += "}";
        break;
    }
}

} // namespace sp::util
