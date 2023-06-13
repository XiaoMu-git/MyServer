#ifndef _SendCore_h_
#define _SendCore_h_

#include "DataType.h"

typedef std::shared_ptr<Task> TaskPtr;
class TaskCore {
private:
	std::mutex _mutex;
	std::thread _thread;
	std::list<TaskPtr> _tasks;

protected:
	void doWork();

public:
	void Thread();

	void addTask(TaskPtr& task);
};

#endif // !_SendCore_h_
