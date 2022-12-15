#include "Hooking.hpp"
BOOL Hooks::m_transactionPreparer(int64_t a1, DWORD* transactionID, int32_t a3, int64_t a4, int32_t amount, int32_t p4, int32_t mode, int32_t banked)
{
	/*
	LOG(INFO) << "a3: " << a3;
	LOG(INFO) << "a4: " << a4;
	LOG(INFO) << "amount: " << amount;
	LOG(INFO) << "p4: " << p4;
	LOG(INFO) << "mode: " << mode;
	LOG(INFO) << "banked: " << banked;
	//mode = 500000
	*/
	return static_cast<decltype(&Hooks::m_transactionPreparer)>(g_Hooking->m_Originalm_transactionPreparer)(a1, transactionID, a3, a4, amount,p4,mode,banked);

}

//		static BOOL m_transactionProcessor(int64_t a1, int64_t transactionID, int64_t unk);
BOOL Hooks::m_transactionProcessor(int64_t a1, int64_t transactionID, int64_t unk)
{
	LOG(INFO) << "A1: " << a1;
	LOG(INFO) << "transactionID: " << transactionID;
	LOG(INFO) << "unk: " << unk;
	return static_cast<decltype(&Hooks::m_transactionProcessor)>(g_Hooking->m_Originalm_transactionProcessor)(a1, transactionID, unk);
}