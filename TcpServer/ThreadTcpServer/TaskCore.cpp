#include "TaskCore.h"

void TaskCore::doWork() {
	while (true) {
		if (!_tasks.empty()) {
			std::lock_guard<std::mutex> lock(_mutex);
			auto task = _tasks.front();
			_tasks.pop_front();
			task->doTask();
		}
		else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void TaskCore::Thread() {
	_thread = std::thread(std::mem_fn(&TaskCore::doWork), this);
	_thread.detach();
}

void TaskCore::addTask(TaskPtr& task) {
	std::lock_guard<std::mutex> lock(_mutex);
	_tasks.push_back(task);
}