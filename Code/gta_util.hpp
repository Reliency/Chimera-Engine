#pragma once
#include "Hooking.hpp"
#include <script/tlsContext.hpp>
#include <script/GtaThread.hpp>
#include "ped/CPedFactory.hpp"
namespace Chimera
{
	inline CPed* get_local_ped()
	{
		if (auto ped_factory = *g_Hooking->m_ped_factory)
		{
			return ped_factory->m_local_ped;
		}

		return nullptr;
	}

	inline CPlayerInfo* get_local_playerinfo()
	{
		if (auto ped_factory = *g_Hooking->m_ped_factory)
		{
			if (auto ped = ped_factory->m_local_ped)
			{
				return ped->m_player_info;
			}
		}

		return nullptr;
	}

	inline CNetworkPlayerMgr* get_network_player_mgr()
	{
		return *g_Hooking->m_network_player_mgr;
	}
	inline float* get_water_collision_ptr(CNavigation* nav)
	{
		auto nav_addr = (uint64_t)nav;
		return (float*)(*(uint64_t*)(nav_addr + 0x10) + 0x54);
	}

	inline void reset_ped_oxygen_time(CPed* ped)
	{
		auto ped_addr = (uint64_t)ped;
		*(float*)(*(uint64_t*)(ped_addr + 0x10C0) + 0x278) = 0;
	}
	template <typename F, typename ...Args>
	void execute_networked_script(F&& callback, Args &&...args)
	{
		if (!*g_Hooking->m_is_session_started) {
			LOG(INFO) << "SESSION START";
			std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);
			return;
		}
		auto tls_ctx = rage::tlsContext::get();
		auto og_thread = tls_ctx->m_script_thread;

		tls_ctx->m_script_thread = find_networked_script_thread();

		tls_ctx->m_is_script_thread_active = true;

		std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

		tls_ctx->m_script_thread = og_thread;
		tls_ctx->m_is_script_thread_active = og_thread != nullptr;

		return;
	}
	template <typename F, typename ...Args>
	void execute_as_script(rage::joaat_t script_hash, F&& callback, Args &&...args)
	{
		auto tls_ctx = rage::tlsContext::get();
		for (auto thread : *g_Hooking->m_script_threads)
		{
			if (!thread || !thread->m_context.m_thread_id || thread->m_context.m_script_hash != script_hash || !thread->m_handler->is_networked())
				continue;

			auto og_thread = tls_ctx->m_script_thread;

			tls_ctx->m_script_thread = thread;
			tls_ctx->m_is_script_thread_active = true;

			std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

			tls_ctx->m_script_thread = og_thread;
			tls_ctx->m_is_script_thread_active = og_thread != nullptr;

			return;
		}
	}

	inline GtaThread* find_networked_script_thread()
	{
		for (auto thread : *g_Hooking->m_script_threads)
		{
			if (!thread || !thread->m_context.m_thread_id) continue;
			if (thread->m_handler->is_networked())
			{
				return thread;
			}
		}

		return nullptr;
	}
	inline GtaThread* find_script_thread(rage::joaat_t hash)
	{
		for (auto thread : *g_Hooking->m_script_threads)
		{
			if (thread
				&& thread->m_context.m_thread_id
				&& thread->m_handler
				&& thread->m_script_hash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}
}