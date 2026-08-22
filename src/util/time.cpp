#include "util/time.h"

#include <cmath>
#include <cstdio>

namespace sp::util {
namespace {

std::tm toLocalTm(TimePoint tp) {
    const std::time_t seconds = Clock::to_time_t(tp);
    std::tm result{};
    localtime_r(&seconds, &result);
    return result;
}

TimePoint fromLocalTm(std::tm tm) {
    tm.tm_isdst = -1;
    return Clock::from_time_t(std::mktime(&tm));
}

} // namespace

std::string formatTimestamp(TimePoint tp) {
    std::tm tm = toLocalTm(tp);
    char buffer[32] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

std::string formatDate(TimePoint tp) {
    std::tm tm = toLocalTm(tp);
    char buffer[16] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
    return std::string(buffer);
}

std::optional<TimePoint> parseTimestamp(const std::string& text) {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    const int fields = std::sscanf(text.c_str(), "%d-%d-%d%*[ T]%d:%d:%d", &year,
                                   &month, &day, &hour, &minute, &second);
    if (fields < 3) {
        return std::nullopt;
    }
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return std::nullopt;
    }
    std::tm tm{};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    return fromLocalTm(tm);
}

std::string nowTimestamp() { return formatTimestamp(Clock::now()); }

std::string today() { return formatDate(Clock::now()); }

std::string formatDuration(long long seconds) {
    if (seconds < 0) {
        seconds = 0;
    }
    const long long hours = seconds / 3600;
    const long long minutes = (seconds % 3600) / 60;
    char buffer[32] = {};
    if (hours > 0) {
        std::snprintf(buffer, sizeof(buffer), "%lldh %02lldm", hours, minutes);
    } else if (minutes > 0) {
        std::snprintf(buffer, sizeof(buffer), "%lldm", minutes);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%llds", seconds);
    }
    return std::string(buffer);
}

std::string formatClock(long long seconds) {
    if (seconds < 0) {
        seconds = 0;
    }
    const long long hours = seconds / 3600;
    const long long minutes = (seconds % 3600) / 60;
    const long long rest = seconds % 60;
    char buffer[32] = {};
    if (hours > 0) {
        std::snprintf(buffer, sizeof(buffer), "%lld:%02lld:%02lld", hours, minutes, rest);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%02lld:%02lld", minutes, rest);
    }
    return std::string(buffer);
}

TimePoint startOfDay(TimePoint tp) {
    std::tm tm = toLocalTm(tp);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return fromLocalTm(tm);
}

TimePoint startOfWeek(TimePoint tp, bool mondayFirst) {
    const int wd = weekday(tp);
    const int back = mondayFirst ? (wd == 0 ? 6 : wd - 1) : wd;
    return startOfDay(addDays(tp, -back));
}

TimePoint startOfMonth(TimePoint tp) {
    std::tm tm = toLocalTm(tp);
    tm.tm_mday = 1;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return fromLocalTm(tm);
}

TimePoint addDays(TimePoint tp, int days) {
    std::tm tm = toLocalTm(tp);
    tm.tm_mday += days;
    return fromLocalTm(tm);
}

TimePoint addMonths(TimePoint tp, int months) {
    std::tm tm = toLocalTm(tp);
    tm.tm_mon += months;
    return fromLocalTm(tm);
}

int weekday(TimePoint tp) { return toLocalTm(tp).tm_wday; }

std::string weekdayName(int wd) {
    static const char* const names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    if (wd < 0 || wd > 6) {
        return "any";
    }
    return names[wd];
}

long long daysBetween(TimePoint a, TimePoint b) {
    using Days = std::chrono::duration<double, std::ratio<86400>>;
    const auto diff = std::chrono::duration_cast<Days>(startOfDay(a) - startOfDay(b));
    return static_cast<long long>(std::llround(diff.count()));
}

std::optional<int> parseClockMinutes(const std::string& hhmm) {
    int hours = 0;
    int minutes = 0;
    if (std::sscanf(hhmm.c_str(), "%d:%d", &hours, &minutes) != 2) {
        return std::nullopt;
    }
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return std::nullopt;
    }
    return hours * 60 + minutes;
}

} // namespace sp::util
