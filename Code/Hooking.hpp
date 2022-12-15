#pragma once
#include "VMTHook.hpp"
#include "player/CPlayerInfo.hpp"
#include "ped/CPedFactory.hpp"
#include "network/CNetworkPlayerMgr.hpp"
#include "network/netObject.hpp"
#include "Invoker.hpp"
#include "script/scrThreadContext.hpp"
#include "script/scrProgram.hpp"
#include "script/GtaThread.hpp"
#include "Pools.hpp"
#include "net_event.hpp"
#include "fwddec.h"
namespace Chimera
{
	struct Hooks
	{

		static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static constexpr auto PresentIndex = 8;
		static HRESULT Present(IDXGISwapChain* dis, UINT syncInterval, UINT flags);

		static constexpr auto ResizeBuffersIndex = 13;
		static HRESULT ResizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	
		using get_native_handler = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable* registration_table, rage::scrNativeHash hash);
		using fix_vectors = void(*)(rage::scrNativeCallContext* call_ctx);
		
		using get_script_handle_t = int64_t(*)(int64_t);
		using register_file_t = uint32_t * (*)(int*, const char*, bool, const char*, bool);
		
		using send_event_ack = void(*)(void* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, int event_index, int event_handled_bitset);

		static rage::eThreadState ScriptVM_Detour(uint64_t* stack, uint64_t** globals, rage::scrProgram* program, rage::scrThreadContext* threadContext);
		using ScriptVM = rage::eThreadState(uint64_t* stack, uint64_t** globals, rage::scrProgram* program, rage::scrThreadContext* threadContext);
	
		static void received_event(void* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int unk, datBitBuffer* bit_buffer);

		static void* physical_player_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index);
		static void player_mgr_init(CNetworkPlayerMgr* self, std::uint64_t a2, std::uint32_t a3, std::uint32_t a4[4]);
		static void player_mgr_destr(CNetworkPlayerMgr* self);
		static char testDtour();
		using read_bitbuf_dword = bool(*)(datBitBuffer* buffer, PVOID read, int bits);
		using read_bitbuf_string = bool(*)(datBitBuffer* buffer, char* read, int bits);
		using read_bitbuf_bool = bool(*)(datBitBuffer* buffer, bool* read, int bits);
		using read_bitbuf_array = bool(*)(datBitBuffer* buffer, PVOID read, int bits, int unk);
		using write_bitbuf_qword = bool(*)(datBitBuffer* buffer, uint64_t val, int bits);
		using write_bitbuf_dword = bool(*)(datBitBuffer* buffer, uint32_t val, int bits);
		using write_bitbuf_int64 = bool(*)(datBitBuffer* buffer, int64_t val, int bits);
		using write_bitbuf_int32 = bool(*)(datBitBuffer* buffer, int32_t val, int bits);
		using write_bitbuf_bool = bool(*)(datBitBuffer* buffer, bool val, int bits);
		using write_bitbuf_array = bool(*)(datBitBuffer* buffer, void* val, int bits, int unk);

		using get_sync_type_info = const char* (*)(uint16_t sync_type, char a2);

		using get_sync_tree_for_type = int64_t(*)(CNetworkObjectMgr* mgr, uint16_t sync_type);

		using get_net_object = rage::netObject* (*)(CNetworkObjectMgr* mgr, int16_t id, bool can_delete_be_pending);
		using trigger_script_event = void(*)(int event_group, int64_t* args, int arg_count, int player_bits);
		using transactionPreparer = BOOL(__fastcall*)(int64_t a1, DWORD* transactionID, int32_t a3, int64_t a4, int32_t amount, int32_t p4, int32_t mode, int32_t banked);
		using transactionProcessor = BOOL(__fastcall*)(int64_t a1, int64_t transactionID, int64_t unk);
		static BOOL m_transactionPreparer(int64_t a1, DWORD* transactionID, int32_t a3, int64_t a4, int32_t amount, int32_t p4, int32_t mode, int32_t banked);
		static BOOL m_transactionProcessor(int64_t a1, int64_t transactionID, int64_t unk);
		using ScriptVM = rage::eThreadState(uint64_t* stack, uint64_t** globals, rage::scrProgram* program, rage::scrThreadContext* threadContext);
		static bool increment_stat_event(CNetworkIncrementStatEvent* net_event_struct, CNetGamePlayer* sender);
		static bool scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player);
		static bool init_native_tables(rage::scrProgram* program);
	};

	class Hooking
	{
	public:
		explicit Hooking();
		~Hooking() noexcept;
		Hooking(Hooking const&) = delete;
		Hooking(Hooking&&) = delete;
		Hooking& operator=(Hooking const&) = delete;
		Hooking& operator=(Hooking&&) = delete;

		void Hook();
		void Unhook();
		friend struct Hooks;

		IDXGISwapChain** m_swapchain{};

		int* m_resolution_x;
		int* m_resolution_y;
		std::uint32_t* m_FrameCount;
		rage::atArray<GtaThread*>* m_script_threads{};
		eGameState* m_game_state{};
		bool* m_is_session_started{};
		CPedFactory** m_ped_factory{};
		CNetworkPlayerMgr** m_network_player_mgr{};
		rage::scrNativeRegistrationTable* m_native_registration_table{};
		Hooks::get_native_handler m_get_native_handler{};
		Hooks::fix_vectors m_fix_vectors{};

		PVOID m_assign_physical_index{};
		PVOID m_player_mgr_init;
		PVOID m_player_mgr_destr;
		PVOID m_native_tables{};
		//Transaction Stuff - Secrets -
		int64_t* transHelper{};
		Hooks::transactionPreparer m_transactionPreparer{};
		Hooks::transactionProcessor m_transactionProcessor{};
		PVOID m_native_return;
		WNDPROC m_og_wndproc = nullptr;
		PVOID blame_patch;


		Hooks::ScriptVM* m_script_vm{};

		//Native Patterns:
		bool* m_cashdrop;

		//ScriptHookV Start
		Hooks::get_script_handle_t m_get_script_handle{};
		Hooks::register_file_t m_register_file{};
		rage::GenericPool* m_ped_pool{};
		rage::VehiclePool* m_vehicle_pool{};
		rage::GenericPool* m_prop_pool{};
		rage::GenericPool* m_pickup_pool{};
		rage::GenericPool* m_camera_pool{};
		//ScriptHookV End

		//Random Natives
		Hooks::trigger_script_event m_trigger_script_event;
		using tAddOwnedExplosion = BOOL(*)(Ped ped, NativeVector3 coords, int explosionType, float damageScale, BOOL isAudible, BOOL isInvisible, float cameraShake);
		tAddOwnedExplosion m_add_owned_explosion{};
		using tCREATE_AMBIENT_PICKUP = Pickup(*)(DWORD pickupHash, NativeVector3 pos, int unk0, int value, DWORD modelHash, bool unk1, bool unk2);
		tCREATE_AMBIENT_PICKUP m_CREATE_AMBIENT_PICKUP{};
		//End Random Natives

		Hooks::read_bitbuf_dword m_read_bitbuf_dword{};
		Hooks::read_bitbuf_string m_read_bitbuf_string{};
		Hooks::read_bitbuf_bool m_read_bitbuf_bool{};
		Hooks::read_bitbuf_array m_read_bitbuf_array{};
		Hooks::write_bitbuf_qword m_write_bitbuf_qword{};
		Hooks::write_bitbuf_dword m_write_bitbuf_dword{};
		Hooks::write_bitbuf_int64 m_write_bitbuf_int64{};
		Hooks::write_bitbuf_int32 m_write_bitbuf_int32{};
		Hooks::write_bitbuf_bool m_write_bitbuf_bool{};
		Hooks::write_bitbuf_array m_write_bitbuf_array{};

		PVOID m_received_clone_sync;
		Hooks::get_sync_tree_for_type m_get_sync_tree_for_type{};
		Hooks::get_sync_type_info m_get_sync_type_info{};
		Hooks::get_net_object m_get_net_object{};
		CNetworkObjectMgr** m_network_object_mgr{};
		std::int64_t** m_script_globals{};
		rage::scrProgramTable* m_scrTable{};
		// Received Event
		PVOID m_received_event{};
		Hooks::send_event_ack m_send_event_ack{};


		void* m_OriginalScriptVM{};
		void* m_OriginalReceivedEvent{};
		void* m_OriginalChatFilter{};
		void* m_OriginalWndProc{};
		void* m_Originalm_transactionPreparer{};
		void* m_Originalm_physical_player_index{};
		void* m_Originalplayer_mgr_init{};
		void* m_Originalplayer_mgr_destr{};
		void* m_OriginalNativeTable{};
		void* m_Originaltestdt{};
		void* m_Originalm_transactionProcessor{};
	private:
		VMTHook m_D3DHook;

	};

	inline std::unique_ptr<Hooking> g_Hooking;
}
