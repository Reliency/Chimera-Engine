#include "Loops.hpp"
#include "script_globals.hpp"
namespace self {
	static void godmode(bool toggle)
	{
		static uint32_t last_bits = 0;
		if (get_local_ped() == nullptr)
		{
			return;
		}

		uint32_t bits = 0;
		if (toggle)
		{
			bits = static_cast<int>(eEntityProofs::GOD);
		}

		uint32_t changed_bits = bits ^ last_bits;
		uint32_t changed_or_enabled_bits = bits | changed_bits;

		if (changed_or_enabled_bits)
		{
			uint32_t unchanged_bits = get_local_ped()->m_damage_bits & ~changed_or_enabled_bits;
			get_local_ped()->m_damage_bits = unchanged_bits | bits;
			last_bits = bits;
		}
	}

	static void neverWanted(bool toggle)
	{
		if (toggle)
		{
			if(get_local_ped()->m_player_info->m_wanted_level > 0)
				get_local_ped()->m_player_info->m_wanted_level = 0;

		}
	}
	static void superman(bool toggle)
	{
		if (toggle) {
			int left_right = PAD::GET_CONTROL_VALUE(2, 188);
			int up_down = PAD::GET_CONTROL_VALUE(2, 189);

			if (ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(PLAYER::PLAYER_PED_ID()) < 3.f) {
				WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, -1, true, true);

				NativeVector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
				coords.z += 100.f;
				ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, false, false, false, false);
			}
			else {
				if (up_down == 254 || ImGui::GetIO().KeysDown[ImGuiKey_S]) {
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0.f, 3.f, 10.f, 0.f, 0.f, 0.f, false, true, true, true, false, true);
				}

				if (up_down == 0 || ImGui::GetIO().KeysDown[ImGuiKey_W]) {
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, false, true, true, true, false, true);
				}

				if (ImGui::GetIO().KeyShift) {
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0.f, 15.f, .3f, 0.f, 0.f, 0.f, false, true, true, true, false, true);
				}
				
				if (left_right == 254 || ImGui::GetIO().KeysDown[ImGuiKey_D]) {
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 1.20f, 0.f, 0.f, 0.f, 0.1f, 0.f, false, true, true, true, false, true);
				}

				if (left_right == 0 || ImGui::GetIO().KeysDown[ImGuiKey_A]) {
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, -1.20f, 0.f, 0.f, 0.f, 0.1f, 0.f, false, true, true, true, false, true);
				}
			}
		}
	}
	static void setRagdoll(bool toggle) {

		if (toggle) {
			PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), !toggle);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), !toggle);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), !toggle);
		}
	}
	static void invisible(bool toggle) {
		if (!toggle)
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, 0);
		else
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), false, 0);
	}
	static void off_radar(bool toggle)
	{
		if (toggle)
		{
			*script_global(2689235).at(PLAYER::PLAYER_ID()).at(453).at(208).as<int*>() = toggle;
			*script_global(2703735).at(56).as<int*>() = NETWORK::GET_NETWORK_TIME() + 1;
		}
	}
}