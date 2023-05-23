#ifndef _SENDCORE_H_
#define _SENDCORE_H_

#include "DataType.h"

class SendCore {
private:
	std::mutex _mutex;
	std::thread* _thread;
	std::list<Task*> _tasks;

protected:
	// 运行函数
	void doRun();

	// 运行状态
	bool isRun();

	// 关闭核心
	void doClose();

public:
	std::atomic_int _send_num, _send_pkg;
	std::list<Task*> _tasks_buff;

	SendCore();

	// 添加任务
	void addTask(Task* task);

	// 开启核心线程
	void doThread();

	virtual ~SendCore();
};

#endif // !_SENDCORE_H_
