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

// Both gameplay and the settings preview draw in screen pixels.
inline float PixelScale(int screenWidth, float size)
{
	return screenWidth / 800.f * ClampSize(size);
}

inline float ParseSize(const char* text, float fallback = 1.0f)
{
	if (!text || !*text) return ClampSize(fallback);
	char* end = nullptr;
	const float value = std::strtof(text, &end);
	if (end == text || *end || !std::isfinite(value)) return ClampSize(fallback);
	return ClampSize(value);
}

// Presets use the same RGB values for swatches, the picker, and gameplay.
inline uint32_t PresetColor(int index)
{
	constexpr uint32_t colors[] = {0xFFFFFF, 0x00FF00, 0x00FFFF, 0xFFFF00, 0xFF0000, 0xFF69B4};
	return index >= 0 && index < 6 ? colors[index] : colors[0];
}

inline int ParseChannel(const char* text, int fallback)
{
	if (!text || !*text) return fallback;
	char* end = nullptr;
	const long value = std::strtol(text, &end, 10);
	if (end == text || *end) return fallback;
	return value < 0 ? 0 : (value > 255 ? 255 : int(value));
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
