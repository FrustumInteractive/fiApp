#include "fi/app/threading.h"

void TaskQueue::addTask(std::function<void()> fn)
{
	std::unique_lock<std::mutex> lock(m_access);
	m_taskQueue.push_back(fn);
	m_condition.notify_one();
}

void TaskQueue::addTask(std::function<void()> fn, uint32_t afterMilliseconds)
{
	std::unique_lock<std::mutex> lock(m_timerThreadAccess);
	std::chrono::time_point<std::chrono::steady_clock> tp = std::chrono::steady_clock::now() +
	std::chrono::milliseconds(afterMilliseconds);

	m_delayedTaskQueue.push_back(std::make_pair(tp, fn));
	m_timerThreadCondition.notify_one();
}

void TaskQueue::start()
{
	if(!m_bStarted)
	{
		m_thread = std::thread([=] {
			while(!m_bExit)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(m_access);
					if(m_taskQueue.empty())
					{
						m_condition.wait_for(lock, std::chrono::milliseconds(5));
						continue;
					}
					task = std::move(m_taskQueue.front());
					m_taskQueue.pop_front();
				}
				task();
			}
		});

		m_timerThread = std::thread([=] {
			while(!m_bExit)
			{
				std::unique_lock<std::mutex> lock(m_timerThreadAccess);
				for(size_t i=0; i < m_delayedTaskQueue.size(); i++)
				{
					auto pair = m_delayedTaskQueue[i];
					if(pair.first < std::chrono::steady_clock::now())
					{
						addTask(std::move(pair.second));
						m_delayedTaskQueue.pop_front();
					}
				}
				m_timerThreadCondition.wait_for(lock, std::chrono::milliseconds(1));
			}
		});

		m_bStarted = true;
	}
}

void TaskQueue::stop()
{
	if(m_bStarted)
	{
		m_bExit = true;
		m_thread.join();
		m_timerThread.join();
	}
}

bool TaskQueue::isRunning()
{
	return m_bStarted;
}
