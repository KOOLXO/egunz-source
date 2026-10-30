#pragma once

#include <cstdlib>

namespace CameraSettings
{
constexpr int MinDistance = 150;
constexpr int MaxDistance = 1000;
constexpr int DefaultDistance = 290;

inline int ClampDistance(long value)
{
	return value < MinDistance ? MinDistance :
		(value > MaxDistance ? MaxDistance : static_cast<int>(value));
}

inline int ParseDistance(const char* text, int fallback = DefaultDistance)
{
	if (!text || !*text) return ClampDistance(fallback);
	char* end = nullptr;
	const long value = std::strtol(text, &end, 10);
	if (end == text || *end) return ClampDistance(fallback);
	return ClampDistance(value);
}
}
