#include "Hooking.hpp"
#include "all.hpp"
#include "ScriptManager.hpp"
#include "Util.hpp"
#include "D3DRenderer.hpp"
#include "UI/UIManager.hpp"
#include "MinHook.h"
#include "memory.hpp"
#include "Natives.hpp"
#include "Loops.hpp"

#include <mmsystem.h>


namespace Chimera
{

	Hooking::Hooking() :
		m_swapchain(Signature("48 8B 0D ? ? ? ? 48 8D 55 B0 48 8B 01").Scan().Add(3).Rip().As<decltype(m_swapchain)&>()), // Slightly redundant. Need to conceptualize a better way...
		m_D3DHook(m_swapchain, 18)
	{
		memory::batch main_batch;

		main_batch.add("Screen Resolution", "66 0F 6E 0D ? ? ? ? 0F B7 3D", [this](memory::handle ptr)
			{
				m_resolution_x = ptr.sub(4).rip().as<int*>();
				m_resolution_y = ptr.add(4).rip().as<int*>();
			});
		main_batch.add("Game State", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", [this](memory::handle ptr)
			{
				m_game_state = ptr.add(2).rip().as<eGameState*>();
			});
		main_batch.add("Is Session Active", "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE", [this](memory::handle ptr)
			{
				m_is_session_started = ptr.add(3).rip().as<bool*>();
			});
		main_batch.add("Ped Factory", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", [this](memory::handle ptr)
			{
				m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
			});
		main_batch.add("Network Player Manager", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [this](memory::handle ptr)
			{
				m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
			});
		main_batch.add("Native Handler", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [this](memory::handle ptr)
			{
				m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
				m_get_native_handler = ptr.add(12).rip().as<Hooks::get_native_handler>();
			});
		main_batch.add("Fix Vectors", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [this](memory::handle ptr)
			{
				m_fix_vectors = ptr.as<Hooks::fix_vectors>();
			});
		main_batch.add("Swapchain", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", [this](memory::handle ptr)
			{
				m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
			});
		main_batch.add("Native Return Spoof", "FF E3", [this](memory::handle ptr)
			{
				m_native_return = ptr.add(0).as<PVOID>();
			});
		main_batch.add("Script Globals", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", [this](memory::handle ptr)
			{
				m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
			});
		main_batch.add("Native Table Initialization", "8B CB E8 ? ? ? ? 8B 43 70 ? 03 C4 A9 00 C0 FF FF", [this](memory::handle ptr)
			{
				m_native_tables = ptr.add(3).rip().as<PVOID>();
			});
		main_batch.add("Script Program VTable", "48 8B 1D ? ? ? ? 41 83 F8 FF", [this](memory::handle ptr)
			{
				m_scrTable = ptr.add(3).rip().as<decltype(m_scrTable)>();
			});
		main_batch.add("Script VM", "4D 8B F9 4D 8B C8 4C 8B C2 4D 63 57 0C 49 63 47 14 4D 8B 69 10 48 FF C8 49 8B D2", [this](memory::handle ptr)
			{
				m_script_vm = ptr.sub(0x26).as<Hooks::ScriptVM*>();
			});
		main_batch.add("Script Threads", "45 33 F6 8B E9 85 C9 B8", [this](memory::handle ptr)
			{
				m_script_threads = ptr.sub(4).rip().sub(8).as<decltype(m_script_threads)>();
			});
		main_batch.add("Frame Count", "8B 15 ? ? ? ? 41 FF CF", [this](memory::handle ptr)
			{
				m_FrameCount = ptr.add(2).rip().as<decltype(m_FrameCount)>();
			});
		main_batch.add("Register File", "40 88 7C 24 ? E8 ? ? ? ? 0F B7 44 24 ?", [this](memory::handle ptr)
			{
				m_register_file = ptr.add(5).as<Hooks::register_file_t>();
			});
		main_batch.add("Get Script Handle", "83 F9 FF 74 31 4C 8B 0D", [this](memory::handle ptr)
			{
				m_get_script_handle = ptr.as<Hooks::get_script_handle_t>();
			});

		main_batch.add("Ped Pool", "48 8B 05 ? ? ? ? 41 0F BF C8", [this](memory::handle ptr)
			{
				m_ped_pool = ptr.add(3).as<rage::GenericPool*>();
			});

		main_batch.add("Vehicle Pool", "48 8B 05 ? ? ? ? F3 0F 59 F6 48 8B 08", [this](memory::handle ptr)
			{
				m_vehicle_pool = *(rage::VehiclePool**)(*(uintptr_t*)ptr.add(3).rip().as<uintptr_t>());
			});

		main_batch.add("Prop Pool", "48 8B 05 ? ? ? ? 8B 78 10 85 FF", [this](memory::handle ptr)
			{
				m_prop_pool = ptr.add(3).rip().as<rage::GenericPool*>();
			});

		main_batch.add("Pickup Pool", "4C 8B 05 ? ? ? ? 40 8A F2 8B E9", [this](memory::handle ptr)
			{
				m_pickup_pool = ptr.add(3).rip().as<rage::GenericPool*>();
			});

		main_batch.add("Camera Pool", "48 8B C8 EB 02 33 C9 48 85 C9 74 26", [this](memory::handle ptr)
			{
				m_camera_pool = ptr.add(-9).rip().as<rage::GenericPool*>();
			});
		main_batch.add("Received Hook", "66 41 83 F9 ? 0F 83", [this](memory::handle ptr)
			{
				m_received_event = ptr.as<decltype(m_received_event)>();
			});
		// Read Bitbuffer WORD/DWORD
		main_batch.add("Read BitBuffer DWord", "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A", [this](memory::handle ptr)
			{
				m_read_bitbuf_dword = ptr.sub(5).as<decltype(m_read_bitbuf_dword)>();
			});

		// Read Bitbuffer Array
		main_batch.add("Read BitBuffer Array", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C", [this](memory::handle ptr)
			{
				m_read_bitbuf_array = ptr.as<decltype(m_read_bitbuf_array)>();
			});

		// Read Bitbuffer String
		main_batch.add("Read BitBuffer String", "E8 ? ? ? ? 48 8D 4F 3C", [this](memory::handle ptr)
			{
				m_read_bitbuf_string = ptr.add(1).rip().as<decltype(m_read_bitbuf_string)>();
			});

		// Read Bitbuffer Boolean
		main_batch.add("Read BitBuffer Bool", "E8 ? ? ? ? 84 C0 74 2D 48 8D 57 20", [this](memory::handle ptr)
			{
				m_read_bitbuf_bool = ptr.add(1).rip().as<decltype(m_read_bitbuf_bool)>();
			});

		// Write Bitbuffer WORD/DWORD
		main_batch.add("Write BitBuffer DWord", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF 01", [this](memory::handle ptr)
			{
				m_write_bitbuf_dword = ptr.as<decltype(m_write_bitbuf_dword)>();
			});

		// Write Bitbuffer QWORD
		main_batch.add("Write BitBuffer QWord", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41 83 F8 20", [this](memory::handle ptr)
			{
				m_write_bitbuf_qword = ptr.as<decltype(m_write_bitbuf_qword)>();
			});

		// Write Bitbuffer Int64
		main_batch.add("Write BitBuffer Int64", "E8 ? ? ? ? 8A 53 39 48 8B CF", [this](memory::handle ptr)
			{
				m_write_bitbuf_int64 = ptr.add(1).rip().as<decltype(m_write_bitbuf_int64)>();
			});

		// Write Bitbuffer Int32
		main_batch.add("Write BitBuffer Int32", "E8 ? ? ? ? 8A 53 74", [this](memory::handle ptr)
			{
				m_write_bitbuf_int32 = ptr.add(1).rip().as<decltype(m_write_bitbuf_int32)>();
			});

		// Write Bitbuffer Boolean
		main_batch.add("Write Bitbuffer Bool", "E8 ? ? ? ? 8A 57 39", [this](memory::handle ptr)
			{
				m_write_bitbuf_bool = ptr.add(1).rip().as<decltype(m_write_bitbuf_bool)>();
			});

		// Write Bitbuffer Array
		main_batch.add("Write Bitbuffer Array", "E8 ? ? ? ? 01 7E 08", [this](memory::handle ptr)
			{
				m_write_bitbuf_array = ptr.add(1).rip().as<decltype(m_write_bitbuf_array)>();
			});
		// Send Event Acknowledge
		main_batch.add("Send Event Acknowledgement", "48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 7A", [this](memory::handle ptr)
			{
				m_send_event_ack = ptr.sub(5).as<decltype(m_send_event_ack)>();
			});
		main_batch.add("Assign Player Index", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8A E8", [this](memory::handle ptr)
			{
				m_assign_physical_index = ptr.as<PVOID>();
			});
		main_batch.add("NPMI", "41 56 48 83 EC ? 48 8B F1 B9 ? ? ? ? 49 8B F9 41 8B E8 4C 8B F2 E8", [this](memory::handle ptr)
			{
				m_player_mgr_init = ptr.sub(0x13).as<decltype(m_player_mgr_init)>();
			});

		// Network Player Mgr Shutdown
		main_batch.add("NPMS", "48 8D 9F ? ? ? ? EB ? 48 8B 13 48 85 D2 74 ? 48 8B CB E8 ? ? ? ? 48 83 7B ? ? 75 ? 48 8D 9F", [this](memory::handle ptr)
			{
				m_player_mgr_destr = ptr.sub(0x1A).as<decltype(m_player_mgr_destr)>();
			});
		main_batch.add("Sync Tree Functions", "4C 8B FA 41 0F B7 D1", [this](memory::handle ptr)
			{
				m_received_clone_sync = ptr.sub(0x1D).as<decltype(m_received_clone_sync)>();
				m_get_sync_tree_for_type = ptr.add(0x14).rip().as<decltype(m_get_sync_tree_for_type)>(); // 0F B7 CA 83 F9 07 .as()
				m_get_net_object = ptr.add(0x76).rip().as<decltype(m_get_net_object)>(); // E8 ? ? ? ? 0F B7 53 7C .add(1).rip().as()
				m_get_sync_type_info = ptr.add(0x8C).rip().as<decltype(m_get_sync_type_info)>(); // 44 0F B7 C1 4C 8D 0D .as()
			});
		main_batch.add("CNetworkObjectMgr", "48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 33 FF 4C 8B F0", [this](memory::handle ptr)
			{
				m_network_object_mgr = ptr.add(3).rip().as<CNetworkObjectMgr**>();
			});
		main_batch.add("Transaction Helper", "48 8B 0D ? ? ? ? 41 8A D9", [this](memory::handle ptr) // 48 8B 0D ? ? ? ? 41 8A D9
			{
				transHelper = ptr.add(3).rip().as<int64_t*>();
			});
		main_batch.add("transactionPreparer", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 8D 64 24 ? 48 83 EC 30", [this](memory::handle ptr) // 48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 83 0A FF
			{
				m_transactionPreparer = ptr.as<Hooks::transactionPreparer>();
			});
		main_batch.add("transactionProcessor", "48 89 5C 24 ? 48 89 7C 24 ? 48 8D 64 24 ? 48 83 EC 20 48 8B 59 20", [this](memory::handle ptr) // 48 89 5C 24 ? 48 89 7C 24 ? E9 ? ? ? ? 48 89 54 24 ?
			{
				m_transactionProcessor = ptr.as<Hooks::transactionProcessor>();
			});
		main_batch.add("Cash Drop Bypass", "40 8A 2D ? ? ? ? 48 83 64 24 40 00", [this](memory::handle ptr)
			{
				m_cashdrop = ptr.add(3).rip().as<bool*>();
				//*m_cashdrop = 0x1; // Assume this will be changed back to default state after some time, so I also set this value before calling the ambient func
			});
		main_batch.add("Blame Patch", "0F 85 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 48 08 E8", [this](memory::handle ptr)
			{
				blame_patch = ptr.as<decltype(blame_patch)>();
			});
		main_batch.add("Create Ambient Pickup", "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D 68 C1 48 81 EC ? ? ? ? 45 8B F9 41 8B F0 4C 8B EA E8 ? ? ? ? 0F B7 0D ? ? ? ? 33 FF 44 8B C7 44 8B D7 44 8B E0 85 C9 7E 1C 48 8B 1D", [this](memory::handle ptr)
			{
				m_CREATE_AMBIENT_PICKUP = ptr.as<tCREATE_AMBIENT_PICKUP>();
			});
		main_batch.add("Trigger Script Event", "45 8B F0 41 8B F9 48 8B EA", [this](memory::handle ptr)
			{
				m_trigger_script_event = ptr.sub(0x1C).as<decltype(m_trigger_script_event)>();
			});
		main_batch.add("Add Owned Explosion", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 70 20 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 E8 41 8B D8", [this](memory::handle ptr)
			{
				m_add_owned_explosion = ptr.as<tAddOwnedExplosion>();
			});
		auto mem_region = memory::module(nullptr);
		main_batch.run(mem_region);

		Chimera::g_hwnd = FindWindowW(L"grcWindow", nullptr);

		if (!Chimera::g_hwnd)
			LOG(FATAL) << "Failed to find window.";

		if (MH_Initialize() != MH_OK) LOG(INFO) << MH_Initialize();
		m_D3DHook.Hook(&Hooks::Present, Hooks::PresentIndex);
		m_D3DHook.Hook(&Hooks::ResizeBuffers, Hooks::ResizeBuffersIndex);

		if (MH_CreateHook(m_script_vm, &Hooks::ScriptVM_Detour, &m_OriginalScriptVM) != MH_OK) LOG(FATAL) << "Script VM Failed";
		if (MH_CreateHook(m_native_tables, &Hooks::init_native_tables, &m_OriginalNativeTable) != MH_OK) LOG(FATAL) << "Native Tables Failed";
		if (MH_CreateHook(m_received_event, &Hooks::received_event, &m_OriginalReceivedEvent) != MH_OK) LOG(FATAL) << "Received Event Failed";
		if (MH_CreateHook(m_assign_physical_index, &Hooks::physical_player_index, &m_Originalm_physical_player_index) != MH_OK) LOG(FATAL) << "Physical Index Failed";
		if (MH_CreateHook(m_player_mgr_init, &Hooks::player_mgr_init, &m_Originalplayer_mgr_init) != MH_OK) LOG(FATAL) << "Network Player Manager Init Failed";
		if (MH_CreateHook(m_player_mgr_destr, &Hooks::player_mgr_destr, &m_Originalplayer_mgr_destr) != MH_OK) LOG(FATAL) << "Network Player Manager Destructor Failed";
		//if (MH_CreateHook(m_transactionPreparer, &Hooks::m_transactionPreparer, &m_Originalm_transactionPreparer) != MH_OK) LOG(FATAL) << "Received Event Failed";
		//if (MH_CreateHook(m_transactionProcessor, &Hooks::m_transactionProcessor, &m_Originalm_transactionProcessor) != MH_OK) LOG(FATAL) << "Received Event Failed";

	}

	Hooking::~Hooking() noexcept
	{
		m_D3DHook.Disable();
		SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
		if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) LOG(WARNING) << "Failed to disable Hooks";
		if(MH_Uninitialize() != MH_OK) LOG(WARNING) << "Failed to init minhook";

	}

	void Hooking::Hook()
	{
		g_Invoker.cache_handlers();
		m_D3DHook.Enable();
		MH_EnableHook(MH_ALL_HOOKS);
		m_og_wndproc = WNDPROC(SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, LONG_PTR(&Hooks::WndProc)));

	}

	void Hooking::Unhook()
	{
		m_D3DHook.Disable();
		MH_DisableHook(MH_ALL_HOOKS);
	}
}
