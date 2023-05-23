#include "SendCore.h"

SendCore::SendCore() {
	_thread = NULL;
	_send_num = _send_pkg = 0;
}

// 添加任务
void SendCore::addTask(Task* task) {
	std::lock_guard<std::mutex> lock(_mutex);
	_tasks_buff.push_back(task);
}

// 运行函数
void SendCore::doRun() {
	while (true) {
		if (!_tasks_buff.empty()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto task : _tasks_buff) _tasks.push_back(task);
			_tasks_buff.clear();
		}

		if (_tasks.empty()) {
			std::chrono::microseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		for (auto task : _tasks) {
			task->doTask();
			delete task;
		}
		_tasks.clear();
	}
}

// 开启核心线程
void SendCore::doThread() {
	_thread = new std::thread(std::mem_fn(&SendCore::doRun), this);
	_thread->detach();
}

// 运行状态
bool SendCore::isRun() {
	return _thread != NULL;
}

// 关闭核心
void SendCore::doClose() {
	if (isRun()) {
		for (auto task : _tasks) delete task; _tasks.clear();
		for (auto task : _tasks_buff) delete task; _tasks_buff.clear();
		delete _thread;
	}
}

SendCore::~SendCore() {
	doClose();
}
