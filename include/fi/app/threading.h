#ifndef _THREADING_H
#define _THREADING_H

#include <thread>
#include <deque>
#include <mutex>
#include <chrono>
#include <string>
#include <condition_variable>
#include <functional>

class TaskQueue
{
public:

	TaskQueue(std::string name="unnamed"):
	m_name(name),
	m_bExit(false),
	m_bStarted(false)
	{}

	void addTask(std::function<void()> fn);
	void addTask(std::function<void()> fn, uint32_t afterMilliseconds);
	void start();
	void stop();
	bool isRunning();

private:

	std::deque<std::function<void()>> m_taskQueue;
	std::deque<std::pair<std::chrono::time_point<std::chrono::steady_clock>,std::function<void()>>> m_delayedTaskQueue;

	std::thread
		m_thread,
		m_timerThread;

	std::string m_name;

	bool
		m_bExit,
		m_bStarted;

	std::mutex m_access, m_timerThreadAccess;
	std::condition_variable m_condition, m_timerThreadCondition;
	std::chrono::time_point<std::chrono::steady_clock> m_clock;
};


#endif /*_THREADING_H*/
