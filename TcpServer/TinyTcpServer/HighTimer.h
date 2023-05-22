#ifndef _HIGHTIMER_H_
#define _HIGHTIMER_H_

#include <chrono>

class HighTimer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;

public:
	// 无参构造
	HighTimer();

	// 更新时间
	void upData();

	// 获取时间，秒
	float getSec();

	// 获取时间，毫秒
	float getMSec();

	// 获取时间，微秒
	long long getUSec();
};

#endif // !_HIGHTIMER_H_
