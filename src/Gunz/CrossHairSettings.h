#pragma once

#include <cmath>
#include <cstdlib>
#include <cstdint>

namespace CrossHairSettings
{
constexpr float MinSize = 0.25f;
constexpr float MaxSize = 3.0f;

inline float ClampSize(float value)
{
	if (!std::isfinite(value)) return 1.0f;
	return value < MinSize ? MinSize : (value > MaxSize ? MaxSize : value);
}

inline float ParseSize(const char* text, float fallback = 1.0f)
{
	if (!text || !*text) return ClampSize(fallback);
	char* end = nullptr;
	const float value = std::strtof(text, &end);
	if (end == text || *end || !std::isfinite(value)) return ClampSize(fallback);
	return ClampSize(value);
}

// Six RGB hex digits, optionally prefixed with '#'. Keep the old color on invalid input.
inline uint32_t ParseColor(const char* text, uint32_t fallback = 0xFFFFFF)
{
	if (!text) return fallback;
	if (*text == '#') ++text;
	uint32_t color = 0;
	for (int i = 0; i < 6; ++i)
	{
		const char c = *text++;
		unsigned digit;
		if (c >= '0' && c <= '9') digit = c - '0';
		else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
		else return fallback;
		color = (color << 4) | digit;
	}
	return *text ? fallback : color;
}
}
