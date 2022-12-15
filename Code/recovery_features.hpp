#include "Loops.hpp"
namespace recovery {
	static void stealthLoop(bool toggle)
	{
		if (toggle)
		{
			DWORD transId = -1;
			g_Hooking->m_transactionPreparer(*g_Hooking->transHelper, &transId, -1135378931, 1474183246, MISC::GET_HASH_KEY("SERVICE_EARN_YOHAN_SOURCE_GOODS"), 1445302971, 300000, 7);
			int64_t a3 = NULL;
			g_Hooking->m_transactionProcessor(*g_Hooking->transHelper, transId, a3);
		}
	}
}