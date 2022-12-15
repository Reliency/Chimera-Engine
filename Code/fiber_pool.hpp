#include "Common.hpp"
#include <mutex>
#include <stack>
#pragma once
class fiber_pool
{
public:
	void queue_job(std::function<void()> func);

	void fiber_tick();
private:
	std::recursive_mutex m_mutex;
	std::stack<std::function<void()>> m_jobs;
};

inline std::unique_ptr<fiber_pool> g_FiberQueue{};