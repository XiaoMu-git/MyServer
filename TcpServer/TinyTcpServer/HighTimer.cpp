#include "HighTimer.h"

HighTimer::HighTimer() {
	upData();
}

void HighTimer::upData() {
	_begin = std::chrono::high_resolution_clock::now();
}

float HighTimer::getSec() {
	return getUSec() * 0.000001f;
}

float HighTimer::getMSec() {
	return getUSec() * 0.001f;
}

long long HighTimer::getUSec() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
}