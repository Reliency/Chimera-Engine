#include "Common.hpp"
#include "crossmap.hpp"
#include "invoker.hpp"
#include "logger.hpp"
#include "Hooking.hpp"
using namespace Chimera;
extern "C" void	_call_asm(void* context, void* function, void* ret);

native_call_context::native_call_context()
{
	m_return_value = &m_return_stack[0];
	m_args = &m_arg_stack[0];
}

void native_invoker::cache_handlers()
{
	for (const rage::scrNativeMapping& mapping : g_crossmap)
	{
		rage::scrNativeHandler handler = g_Hooking->m_get_native_handler(
			g_Hooking->m_native_registration_table, mapping.second);
		m_handler_cache.emplace(mapping.first, handler);
	}
}

void native_invoker::begin_call()
{
	m_call_context.reset();
}

void native_invoker::end_call(rage::scrNativeHash hash)
{
	if (auto it = m_handler_cache.find(hash); it != m_handler_cache.end())
	{
		rage::scrNativeHandler handler = it->second;

		__try
		{
			_call_asm(&m_call_context, handler, g_Hooking->m_native_return);
			g_Hooking->m_fix_vectors(&m_call_context);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			[hash]() { LOG(WARNING) << "EXCEPTION: " << hash << " NATIVE."; }();
		}
	}
	else
	{
		[hash]() { LOG(WARNING) << "FAILED: " << HEX_TO_UPPER(hash) << " HANDLER."; }();
	}
}