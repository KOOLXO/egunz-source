// Standalone validation: g++ -std=c++14 tests/CrossHairSettingsTests.cpp -o /tmp/crosshair-tests && /tmp/crosshair-tests
#include "../src/Gunz/CrossHairSettings.h"
#include <cassert>
#include <limits>
#include <initializer_list>

int main()
{
	using namespace CrossHairSettings;
	// A preview must retain the gameplay pixel scale, including fractional sizes.
	for (int width : {800, 1280, 1920, 2560, 3840})
		for (float size : {0.30f, 1.00f, 3.00f})
		{
			const float expected = width / 800.f * size;
			assert(std::fabs(PixelScale(width, size) - expected) < 0.00001f);
			assert(PixelScale(width, size) == PixelScale(width, ParseSize(size == 0.30f ? "0.30" : size == 1.f ? "1.00" : "3.00")));
		}
	assert(ParseSize("0.75") == 0.75f);
	assert(ParseSize("3") == MaxSize);
	assert(ParseSize("-1") == MinSize);
	assert(ParseSize("100") == MaxSize);
	assert(ParseSize("nan", 0.75f) == 0.75f);
	assert(ParseSize("inf", 0.75f) == 0.75f);
	assert(ParseSize("1e1000", 0.75f) == 0.75f);
	assert(ParseSize("1.5junk", 0.75f) == 0.75f);
	assert(ParseSize("", 0.75f) == 0.75f);
	assert(ParseSize(nullptr, 0.75f) == 0.75f);
	assert(ClampSize(std::numeric_limits<float>::quiet_NaN()) == 1.0f);
	assert(ParseColor("9CE4EF") == 0x9CE4EF);
	assert(ParseColor("#9ce4ef") == 0x9CE4EF);
	assert(ParseColor("000000") == 0);
	assert(ParseColor("FFFFFF") == 0xFFFFFF);
	assert(ParseColor("", 0x123456) == 0x123456);
	assert(ParseColor(nullptr, 0x123456) == 0x123456);
	assert(ParseColor("12345", 0x123456) == 0x123456);
	assert(ParseColor("1234567", 0x123456) == 0x123456);
	assert(ParseColor("GG0000", 0x123456) == 0x123456);
	// Exercise null-terminated short inputs at every possible length.
	for (int length = 0; length < 6; ++length)
	{
		char text[7] = {};
		for (int i = 0; i < length; ++i) text[i] = 'F';
		assert(ParseColor(text, 0x123456) == 0x123456);
	}
}
