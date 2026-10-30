// g++ -std=c++14 tests/CameraSettingsTests.cpp -o /tmp/camera-tests && /tmp/camera-tests
#include "../src/Gunz/CameraSettings.h"
#include <cassert>
#include <climits>

int main()
{
	using namespace CameraSettings;
	assert(DefaultDistance == 290);
	assert(ParseDistance("290") == DefaultDistance);
	assert(ParseDistance("500") == 500);
	assert(ParseDistance("150") == MinDistance);
	assert(ParseDistance("1000") == MaxDistance);
	assert(ParseDistance("0") == MinDistance);
	assert(ParseDistance("-500") == MinDistance);
	assert(ParseDistance("1001") == MaxDistance);
	assert(ParseDistance("9999999999999999999999999") == MaxDistance);
	assert(ParseDistance("-9999999999999999999999999") == MinDistance);
	assert(ParseDistance(nullptr) == DefaultDistance);
	assert(ParseDistance("") == DefaultDistance);
	assert(ParseDistance("nan", 500) == 500);
	assert(ParseDistance("inf", 500) == 500);
	assert(ParseDistance("500junk", 600) == 600);
	assert(ParseDistance("500.5", 600) == 600);
	assert(ClampDistance(LONG_MAX) == MaxDistance);
	assert(ClampDistance(LONG_MIN) == MinDistance);
	for (int distance = MinDistance; distance <= MaxDistance; ++distance)
		assert(ClampDistance(distance) == distance);
}
