#ifndef _SENDCORE_H_
#define _SENDCORE_H_

#include "DataType.h"

class SendCore {
private:
	std::mutex _mutex;
	std::thread* _thread;
	std::list<Task*> _tasks;

protected:
	// ���к���
	void doRun();

	// ����״̬
	bool isRun();

	// �رպ���
	void doClose();

public:
	std::atomic_int _send_num, _send_pkg;
	std::list<Task*> _tasks_buff;

	SendCore();

	// �������
	void addTask(Task* task);

	// ���������߳�
	void doThread();

	virtual ~SendCore();
};

#endif // !_SENDCORE_H_
