#pragma once

#include <chrono>
#include <ctime>
#include <optional>
#include <string>

namespace sp::util {

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

// Every timestamp the app writes is a local-time "YYYY-MM-DD HH:MM:SS" string.
// One sortable textual format lets SQL group by day, week or month with plain
// string prefixes and keeps ORDER BY chronological.
std::string formatTimestamp(TimePoint tp);
std::string formatDate(TimePoint tp);
std::optional<TimePoint> parseTimestamp(const std::string& text);
std::string nowTimestamp();
std::string today();

std::string formatDuration(long long seconds);
std::string formatClock(long long seconds);

TimePoint startOfDay(TimePoint tp);
TimePoint startOfWeek(TimePoint tp, bool mondayFirst = true);
TimePoint startOfMonth(TimePoint tp);
TimePoint addDays(TimePoint tp, int days);
TimePoint addMonths(TimePoint tp, int months);
int weekday(TimePoint tp);
std::string weekdayName(int wd);
long long daysBetween(TimePoint a, TimePoint b);
std::optional<int> parseClockMinutes(const std::string& hhmm);

} // namespace sp::util
