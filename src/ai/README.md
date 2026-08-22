# AI Module — Phase 2 Placeholder

This directory is reserved for AI-augmented features planned for Phase 2:

- `IAiProvider` — abstract interface for AI backends (OpenAI, llama.cpp, etc.)
- `AiTaskService` — AI-powered task prioritization and scheduling
- `AiStudyPlanGenerator` — auto-generate study plans based on analytics
- `AiSuggestionEngine` — contextual study recommendations

## Integration Points

1. Service-layer interfaces allow drop-in AI replacements
2. DTO boundary prevents AI layer from depending on DB internals
3. `nlohmann/json` serialization on all models enables prompt construction
4. Config-driven AI settings (API key, model, temperature) ready in `default_config.json`
5. Migration system supports adding AI tables without breaking existing schema

## Example Interface

```cpp
class IAiProvider {
public:
  virtual ~IAiProvider() = default;
  virtual std::string suggest(const std::string& prompt) = 0;
  virtual std::string analyze(const nlohmann::json& context) = 0;
};
```
