#ifndef _HIGHTIMER_H_
#define _HIGHTIMER_H_

#include <chrono>

class HighTimer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;

public:
	// �޲ι���
	HighTimer();

	// ����ʱ��
	void upData();

	// ��ȡʱ�䣬��
	float getSec();

	// ��ȡʱ�䣬����
	float getMSec();

	// ��ȡʱ�䣬΢��
	long long getUSec();
};

#endif // !_HIGHTIMER_H_
