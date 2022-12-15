#include "Common.hpp"
#include <mutex>
#include <thread>
#include "fiber_pool.hpp"
void fiber_pool::queue_job(std::function<void()> func)
{
	if (func)
	{
		std::lock_guard lock(m_mutex);
		m_jobs.push(std::move(func));
	}
}

void fiber_pool::fiber_tick()
{
	std::unique_lock lock(m_mutex);
	if (!m_jobs.empty())
	{
		auto job = std::move(m_jobs.top());
		m_jobs.pop();
		lock.unlock();

		std::invoke(std::move(job));
	}
}