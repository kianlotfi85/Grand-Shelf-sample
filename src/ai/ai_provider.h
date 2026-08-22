#pragma once

#include <optional>
#include <string>
#include <vector>

namespace sp::ai {

struct AiRequest {
    std::string prompt;
    std::string context_json;
    std::optional<double> temperature;
};

struct AiResponse {
    std::string text;
    std::string model;
};

// Phase 2 seam. Application holds an IAiProvider* that stays null in Phase 1,
// so every call site must already handle "no provider configured".
class IAiProvider {
  public:
    virtual ~IAiProvider() = default;

    virtual std::string name() const = 0;
    virtual AiResponse suggest(const AiRequest &request) = 0;
};

} // namespace sp::ai
