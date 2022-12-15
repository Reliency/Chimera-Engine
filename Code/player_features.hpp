#pragma once
#include <timeapi.h>
#include "Loops.hpp"
#include "Script.hpp"
#include "script_globals.hpp"
namespace remote_player {
	static bool flipped = true;
	static void spectate(bool toggle)
	{
		if (!g_player_getter->get_selected()->is_valid() || !toggle)
		{
			if (toggle) toggles::player::spectate = false;

			if (!flipped)
			{
				flipped = true;

				NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(false, PLAYER::PLAYER_PED_ID());
				HUD::SET_MINIMAP_IN_SPECTATOR_MODE(false, PLAYER::PLAYER_PED_ID());
			}

			return;
		}
		const Ped target = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_getter->get_selected()->id());
		NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(true, target);
		HUD::SET_MINIMAP_IN_SPECTATOR_MODE(true, target);
		NativeVector3 SelectCoords = ENTITY::GET_ENTITY_COORDS(target, true);
		GRAPHICS::DRAW_MARKER(2, SelectCoords.x, SelectCoords.y, SelectCoords.z + 1.25, 0, 0, 0, 0, 180, 0, 0.25, 0.25, 0.25, 200, 94, 100, 255, 1, 1, 1, 0, 0, 0, 0);
		flipped = false;
	}
	static int DelayOne = GetCurrentTime();
	static void cash_drop(bool toggle) {
		/*
		if (toggle) {
				Ped iPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_getter->get_selected()->id());
				NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(iPed, 0);//0xde78f17e -> 0x1E9A99F8
				*script_global(2787534).at(3).as<float*>() = pCoords.x;
				*script_global(2787534).at(4).as<float*>() = pCoords.y;
				*script_global(2787534).at(5).as<float*>() = pCoords.z + 1.5f;
				*script_global(2787534).at(1).as<int*>() = 2500;
				int part2 = *script_global(2787534).as<int*>();
				*script_global(4534105).at(part2, 85).at(66).at(2).as<int*>() = 2;
				*script_global(2787540).as<int*>() = 1 << 0;
				LOG(INFO) << "Cash: " << *script_global(1853348).at(g_player_getter->get_selected()->id(), 834).at(205).at(3).as<int*>();
		}
		*/
		if (toggle) {
			if ((timeGetTime() - DelayOne) >= 250)
			{
				Ped iPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_getter->get_selected()->id());
				NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(iPed, 0);//0xde78f17e -> 0x1E9A99F8
				while (!STREAMING::HAS_MODEL_LOADED(-1666779307))
				{
					STREAMING::REQUEST_MODEL(-1666779307);
					script::get_current()->yield();
				}
				int flag = 0;
				MISC::SET_BIT(&flag, 3);
				MISC::SET_BIT(&flag, 4);
				pCoords.z + 1.5;
				g_Hooking->m_CREATE_AMBIENT_PICKUP(0xE175C698, pCoords, flag, 2500, -1666779307, 0, 1);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(-1666779307);
				DelayOne = timeGetTime();
			}
		}
	}
	static void off_radar(bool toggle) {
		if (toggle)
		{
			int unk1 = NETWORK::GET_NETWORK_TIME();
			int unk2 = NETWORK::GET_NETWORK_TIME() + (60000 * 59);
			int id = g_player_getter->get_selected()->id();
			int unk6 = *script_global(1892703).at(id, 599).at(510).as<int*>();
			int64_t args[7] = { -1973627888, id, NETWORK::GET_NETWORK_TIME(), NETWORK::GET_NETWORK_TIME() + 86400, 1, 1, unk6 };
			g_Hooking->m_trigger_script_event(1, args, 7, 1 << id);
		}
	}
	static void never_wanted(bool toggle)
	{
		if (toggle)
		{
			int id = g_player_getter->get_selected()->id();
			int64_t args[3] = {
			1449852136,
			g_player_getter->get_selected()->id(),
			*script_global(1853348).at(id, 599).at(510).as<int*>()
			};
			g_Hooking->m_trigger_script_event(1, args, 3, 1 << id);
		}
	}

	static void explode(bool blame = false) {
		if (blame)
		{
			g_player_getter->iterate([](const player_entry& entry)
				{
					NativeVector3 targetCords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(entry.second->id()), false);
					*(unsigned short*)g_Hooking->blame_patch = 0xE990;
					g_Hooking->m_add_owned_explosion(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_getter->get_selected()->id()), targetCords, 29, 9999.0f, false, true, 0.0f);
					*(unsigned short*)g_Hooking->blame_patch = 0x850F;
				});
		}
		else {
			NativeVector3 targetCords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_getter->get_selected()->id()), false);
			FIRE::ADD_EXPLOSION(targetCords.x, targetCords.y, targetCords.z, 29, 9999.0f, true, false, 0.0f, false);
		}
	}
}