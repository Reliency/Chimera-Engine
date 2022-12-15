#pragma once
#include "script_globals.hpp"
#include "menus.hpp"

#include "Natives.hpp"
#include "NativeHelper.hpp"
#include "ScriptManager.hpp"
#include "data.hpp"
#include "Vehicle_Mods.h"


using namespace Chimera;
static char search[64];

inline void set_mp_bitset(Vehicle veh)
{
	DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
	auto networkId = NETWORK::VEH_TO_NET(veh);
	if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
		NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
	VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
}
inline Vehicle spawn(Hash hash, NativeVector3 location, float heading, bool is_networked = true)
{

	while(!STREAMING::HAS_MODEL_LOADED(hash))
	{
		STREAMING::REQUEST_MODEL(hash);
		script::get_current()->yield();
	}

	auto veh = VEHICLE::CREATE_VEHICLE(hash, location.x, location.y, location.z, heading, is_networked, false, false);

	script::get_current()->yield();

	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

	if (*g_Hooking->m_is_session_started)
	{
		set_mp_bitset(veh);
	}

	return veh;
}


inline NativeVector3 get_spawn_location(bool spawn_inside)
{
	float y_offset = 0;

	if (g_local->veh != 0)
	{
		y_offset = 10.f;
	}
	else if (!spawn_inside)
	{
		y_offset = 5.f;
	}

	return ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(g_local->ped, 0.f, y_offset, 0.f);
}

inline std::map<int, int32_t> get_owned_mods_from_vehicle_idx(script_global vehicle_idx)
{
	std::map<int, int32_t> owned_mods;

	for (int i = MOD_SECONDARY_CUSTOM; i <= MOD_MODEL_HASH; i++)
	{
		owned_mods[i] = 0;
	}

	int32_t val_32 = *vehicle_idx.at(32).as<int32_t*>();
	int32_t val_77 = *vehicle_idx.at(77).as<int32_t*>();
	int32_t val_102 = *vehicle_idx.at(102).as<int32_t*>();


	owned_mods[MOD_MODEL_HASH] = *vehicle_idx.at(66).as<int32_t*>();

	owned_mods[MOD_PLATE_STYLE] = *vehicle_idx.at(0).as<int32_t*>();
	owned_mods[MOD_WINDOW_TINT] = *vehicle_idx.at(65).as<int32_t*>();
	owned_mods[MOD_WHEEL_TYPE] = *vehicle_idx.at(69).as<int32_t*>();


	owned_mods[MOD_PRIMARY_COL] = *vehicle_idx.at(5).as<int32_t*>();
	owned_mods[MOD_SECONDARY_COL] = *vehicle_idx.at(6).as<int32_t*>();
	owned_mods[MOD_PEARLESCENT_COL] = *vehicle_idx.at(7).as<int32_t*>();
	owned_mods[MOD_WHEEL_COL] = *vehicle_idx.at(8).as<int32_t*>();
	owned_mods[MOD_INTERIOR_COL] = *vehicle_idx.at(97).as<int32_t*>();
	owned_mods[MOD_DASHBOARD_COL] = *vehicle_idx.at(99).as<int32_t*>();


	//CUSTOM PRIMARY
	owned_mods[MOD_PRIMARY_CUSTOM] = (val_77 & (1 << 13)) != 0;
	if (owned_mods[MOD_PRIMARY_CUSTOM])
	{
		owned_mods[MOD_PRIMARY_COL_R] = *vehicle_idx.at(71).as<int32_t*>();
		owned_mods[MOD_PRIMARY_COL_G] = *vehicle_idx.at(72).as<int32_t*>();
		owned_mods[MOD_PRIMARY_COL_B] = *vehicle_idx.at(73).as<int32_t*>();
	}


	//CUSTOM SECONDARY
	owned_mods[MOD_SECONDARY_CUSTOM] = (val_77 & (1 << 12)) != 0;
	if (owned_mods[MOD_SECONDARY_CUSTOM])
	{
		owned_mods[MOD_SECONDARY_COL_R] = *vehicle_idx.at(71).as<int32_t*>();
		owned_mods[MOD_SECONDARY_COL_G] = *vehicle_idx.at(72).as<int32_t*>();
		owned_mods[MOD_SECONDARY_COL_B] = *vehicle_idx.at(73).as<int32_t*>();
	}


	// TIRE SMOKE
	owned_mods[MOD_TIRESMOKE_COL_R] = *vehicle_idx.at(62).as<int32_t*>();
	owned_mods[MOD_TIRESMOKE_COL_G] = *vehicle_idx.at(63).as<int32_t*>();
	owned_mods[MOD_TIRESMOKE_COL_B] = *vehicle_idx.at(64).as<int32_t*>();
	owned_mods[MOD_TYRE_SMOKE] = !(
		owned_mods[MOD_TIRESMOKE_COL_R] == 255 &&
		owned_mods[MOD_TIRESMOKE_COL_G] == 255 &&
		owned_mods[MOD_TIRESMOKE_COL_B] == 255
		);


	// XENON
	if (val_32 > 0)
	{
		owned_mods[MOD_XENON_LIGHTS] = 1;
		owned_mods[MOD_XENON_COL] = val_32 - 2;
	}
	else
	{
		owned_mods[MOD_XENON_LIGHTS] = 0;
	}


	// NEON
	owned_mods[MOD_NEON_LEFT_ON] = (val_77 & (1 << 30)) != 0;
	owned_mods[MOD_NEON_RIGHT_ON] = (val_77 & (1 << 31)) != 0;
	owned_mods[MOD_NEON_FRONT_ON] = (val_77 & (1 << 28)) != 0;
	owned_mods[MOD_NEON_BACK_ON] = (val_77 & (1 << 29)) != 0;
	owned_mods[MOD_NEON_COL_R] = *vehicle_idx.at(74).as<int32_t*>();
	owned_mods[MOD_NEON_COL_G] = *vehicle_idx.at(75).as<int32_t*>();
	owned_mods[MOD_NEON_COL_B] = *vehicle_idx.at(76).as<int32_t*>();

	// TIRE OPTIONS
	owned_mods[MOD_TIRE_CAN_BURST] = (val_77 & (1 << 9)) == 0;
	if ((val_102 & 0b11) == 0b11)
	{
		owned_mods[MOD_DRIFT_TIRE] = 1;
	}

	owned_mods[MOD_TURBO] = *vehicle_idx.at(28).as<int32_t*>() != 0;

	owned_mods[MOD_FRONTWHEEL_VAR] = *vehicle_idx.at(60).as<int32_t*>() != 0;
	owned_mods[MOD_REARWHEEL_VAR] = *vehicle_idx.at(61).as<int32_t*>() != 0;


	// OTHER MODS
	for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
	{
		if (slot == MOD_TURBO || slot == MOD_TYRE_SMOKE || slot == MOD_XENON_LIGHTS)
		{
			continue;
		}

		int32_t val = *vehicle_idx.at(10 + slot).as<int32_t*>() - 1;
		if (val != -1)
		{
			owned_mods[slot] = val;
		}
	}

	// EXTRA
	for (int extra = MOD_EXTRA_9; extra <= MOD_EXTRA_1; extra++)
	{
		int gta_extra_id = (extra - MOD_EXTRA_0) * -1;
		owned_mods[extra] = val_77 >> (gta_extra_id - 1) & 1;
	}

	return owned_mods;
}

inline std::map<int, int32_t> get_owned_mods_from_vehicle(Vehicle vehicle)
{
	std::map<int, int32_t> owned_mods;

	for (int i = MOD_SECONDARY_CUSTOM; i <= MOD_MODEL_HASH; i++)
	{
		owned_mods[i] = 0;
	}

	owned_mods[MOD_MODEL_HASH] = ENTITY::GET_ENTITY_MODEL(vehicle);

	owned_mods[MOD_PLATE_STYLE] = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle);
	owned_mods[MOD_WINDOW_TINT] = VEHICLE::GET_VEHICLE_WINDOW_TINT(vehicle);
	owned_mods[MOD_WHEEL_TYPE] = VEHICLE::GET_VEHICLE_WHEEL_TYPE(vehicle);

	VEHICLE::GET_VEHICLE_COLOURS(vehicle, &owned_mods[MOD_PRIMARY_COL], &owned_mods[MOD_SECONDARY_COL]);
	VEHICLE::GET_VEHICLE_EXTRA_COLOURS(vehicle, &owned_mods[MOD_PEARLESCENT_COL], &owned_mods[MOD_WHEEL_COL]);
	VEHICLE::GET_VEHICLE_EXTRA_COLOUR_5(vehicle, &owned_mods[MOD_INTERIOR_COL]);
	VEHICLE::GET_VEHICLE_EXTRA_COLOUR_6(vehicle, &owned_mods[MOD_DASHBOARD_COL]);

	owned_mods[MOD_PRIMARY_CUSTOM] = VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(vehicle);
	if (owned_mods[MOD_PRIMARY_CUSTOM])
	{
		VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(
			vehicle, &owned_mods[MOD_PRIMARY_COL_R],
			&owned_mods[MOD_PRIMARY_COL_G], &owned_mods[MOD_PRIMARY_COL_B]
		);
	}

	owned_mods[MOD_SECONDARY_CUSTOM] = VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(vehicle);
	if (owned_mods[MOD_SECONDARY_CUSTOM])
	{
		VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(
			vehicle, &owned_mods[MOD_SECONDARY_COL_R],
			&owned_mods[MOD_SECONDARY_COL_G], &owned_mods[MOD_SECONDARY_COL_B]
		);
	}

	owned_mods[MOD_TYRE_SMOKE] = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_TYRE_SMOKE);
	if (owned_mods[MOD_TYRE_SMOKE])
	{
		VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(
			vehicle, &owned_mods[MOD_TIRESMOKE_COL_R],
			&owned_mods[MOD_TIRESMOKE_COL_G], &owned_mods[MOD_TIRESMOKE_COL_B]
		);
	}

	owned_mods[MOD_XENON_LIGHTS] = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_XENON_LIGHTS);
	if (owned_mods[MOD_XENON_LIGHTS])
	{
		owned_mods[MOD_XENON_COL] = (int8_t)VEHICLE::GET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehicle);
	}

	owned_mods[MOD_NEON_LEFT_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_LEFT);
	owned_mods[MOD_NEON_RIGHT_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_RIGHT);
	owned_mods[MOD_NEON_FRONT_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_FRONT);
	owned_mods[MOD_NEON_BACK_ON] = VEHICLE::GET_VEHICLE_NEON_ENABLED(vehicle, NEON_BACK);
	VEHICLE::GET_VEHICLE_NEON_COLOUR(
		vehicle, &owned_mods[MOD_NEON_COL_R],
		&owned_mods[MOD_NEON_COL_G], &owned_mods[MOD_NEON_COL_B]
	);

	owned_mods[MOD_TIRE_CAN_BURST] = VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(vehicle);
	owned_mods[MOD_DRIFT_TIRE] = VEHICLE::GET_DRIFT_TYRES_SET(vehicle);
	owned_mods[MOD_TURBO] = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, MOD_TURBO);

	owned_mods[MOD_FRONTWHEEL_VAR] = VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, MOD_FRONTWHEEL);
	owned_mods[MOD_REARWHEEL_VAR] = VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, MOD_REARWHEEL);

	for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
	{
		int count = VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, slot);
		if (count > 0)
		{
			int32_t val = VEHICLE::GET_VEHICLE_MOD(vehicle, slot);

			if (val != -1)
			{
				owned_mods[slot] = val;
			}
		}
	}

	for (int extra = MOD_EXTRA_14; extra <= MOD_EXTRA_1; extra++)
	{
		int gta_extra_id = (extra - MOD_EXTRA_0) * -1;
		if (VEHICLE::DOES_EXTRA_EXIST(vehicle, gta_extra_id))
		{
			owned_mods[extra] = VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(vehicle, gta_extra_id);
		}
	}

	return owned_mods;
}

inline void teleport_into_vehicle(Vehicle veh)
{
	PED::SET_PED_INTO_VEHICLE(g_local->ped, veh, -1);
}

static const std::map<Hash, std::map<int, std::vector<int32_t>>> mod_blacklists = {
	{ VEHICLE_BANSHEE, {
		{ MOD_SPOILERS, { 3, 4 } },
		{ MOD_COLUMNSHIFTERLEVERS, { 0, 1, 2, 3 } },
		{ MOD_SPEAKERS, { 0 } },
		{ MOD_LIVERY, { 15, 16 } }
	} },
	{ VEHICLE_SENTINEL, {
		{ MOD_SPOILERS, { 4, 5 } },
		{ MOD_COLUMNSHIFTERLEVERS, { 0, 1, 2, 3 } },
		{ MOD_SPEAKERS, { 0 } },
		{ MOD_LIVERY, { 0, 1 } }
	} }
};

bool check_mod_blacklist(Hash model, int mod_slot, int mod)
{
	if (mod_blacklists.find(model) == mod_blacklists.end())
	{
		return false;
	}

	auto veh_slot_blacklist = mod_blacklists.find(model)->second;
	if (veh_slot_blacklist.find(mod_slot) == veh_slot_blacklist.end())
	{
		return false;
	}

	auto veh_mod_blacklist = veh_slot_blacklist.find(mod_slot)->second;
	if (std::find(veh_mod_blacklist.begin(), veh_mod_blacklist.end(), mod) != veh_mod_blacklist.end())
	{
		return true;
	}

	return false;
}

inline void max_vehicle(Vehicle veh)
{
	Hash model = ENTITY::GET_ENTITY_MODEL(veh);

	VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TURBO, TRUE);
	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TYRE_SMOKE, TRUE);
	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_XENON_LIGHTS, TRUE);
	VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 1);
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);

	for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
	{
		if (slot == MOD_LIVERY) {
			continue;
		}

		int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh, slot);
		if (count > 0)
		{
			int selected_mod = -1;

			for (int mod = count - 1; mod >= -1; mod--)
			{

				if (check_mod_blacklist(model, slot, mod))
				{
					continue;
				}

				selected_mod = mod;
				break;
			}

			if (selected_mod != -1)
			{
				VEHICLE::SET_VEHICLE_MOD(veh, slot, selected_mod, true);
			}
		}
	}
}
inline Vehicle clone_from_owned_mods(std::map<int, int32_t> owned_mods, NativeVector3 location, float heading, bool is_networked = true)
{
	auto vehicle = spawn(owned_mods[MOD_MODEL_HASH], location, heading, is_networked);
	if (vehicle == 0)
	{
		return 0;
	}

	for (int i = MOD_NEON_COL_B; i <= MOD_MODEL_HASH; i++)
	{
		if (owned_mods.count(i) == 0)
		{
			owned_mods[i] = 0;
		}
	}

	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	script::get_current()->yield(10ms);

	VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle, owned_mods[MOD_PLATE_STYLE]);
	VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, owned_mods[MOD_WINDOW_TINT]);
	VEHICLE::SET_VEHICLE_WHEEL_TYPE(vehicle, owned_mods[MOD_WHEEL_TYPE]);
	script::get_current()->yield(10ms);

	VEHICLE::SET_VEHICLE_COLOURS(vehicle, owned_mods[MOD_PRIMARY_COL], owned_mods[MOD_SECONDARY_COL]);
	VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, owned_mods[MOD_PEARLESCENT_COL], owned_mods[MOD_WHEEL_COL]);
	VEHICLE::SET_VEHICLE_EXTRA_COLOUR_5(vehicle, owned_mods[MOD_INTERIOR_COL]);
	VEHICLE::SET_VEHICLE_EXTRA_COLOUR_6(vehicle, owned_mods[MOD_DASHBOARD_COL]);

	if (owned_mods[MOD_PRIMARY_CUSTOM])
	{
		VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(
			vehicle, owned_mods[MOD_PRIMARY_COL_R],
			owned_mods[MOD_PRIMARY_COL_G], owned_mods[MOD_PRIMARY_COL_B]
		);
	}

	if (owned_mods[MOD_SECONDARY_CUSTOM])
	{
		VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(
			vehicle, owned_mods[MOD_SECONDARY_COL_R],
			owned_mods[MOD_SECONDARY_COL_G], owned_mods[MOD_SECONDARY_COL_B]
		);
	}

	if (owned_mods[MOD_TYRE_SMOKE])
	{
		VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(
			vehicle, owned_mods[MOD_TIRESMOKE_COL_R],
			owned_mods[MOD_TIRESMOKE_COL_G], owned_mods[MOD_TIRESMOKE_COL_B]
		);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_TYRE_SMOKE, owned_mods[MOD_TYRE_SMOKE]);
	}

	if (owned_mods[MOD_XENON_LIGHTS])
	{
		VEHICLE::SET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehicle, owned_mods[MOD_XENON_COL]);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_XENON_LIGHTS, owned_mods[MOD_XENON_LIGHTS]);
	}

	VEHICLE::SET_VEHICLE_NEON_COLOUR(
		vehicle, owned_mods[MOD_NEON_COL_R],
		owned_mods[MOD_NEON_COL_G], owned_mods[MOD_NEON_COL_B]
	);
	VEHICLE::SET_VEHICLE_NEON_ENABLED(vehicle, NEON_LEFT, owned_mods[MOD_NEON_LEFT_ON]);
	VEHICLE::SET_VEHICLE_NEON_ENABLED(vehicle, NEON_RIGHT, owned_mods[MOD_NEON_RIGHT_ON]);
	VEHICLE::SET_VEHICLE_NEON_ENABLED(vehicle, NEON_FRONT, owned_mods[MOD_NEON_FRONT_ON]);
	VEHICLE::SET_VEHICLE_NEON_ENABLED(vehicle, NEON_BACK, owned_mods[MOD_NEON_BACK_ON]);


	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, owned_mods[MOD_TIRE_CAN_BURST]);
	VEHICLE::SET_DRIFT_TYRES(vehicle, owned_mods[MOD_DRIFT_TIRE]);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_TURBO, owned_mods[MOD_TURBO]);

	for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
	{
		if (owned_mods.count(slot) && owned_mods[slot] != -1)
		{
			bool custom_tire = false;

			if (slot == MOD_FRONTWHEEL)
			{
				custom_tire = owned_mods[MOD_FRONTWHEEL_VAR];
			}
			else if (slot == MOD_REARWHEEL)
			{
				custom_tire = owned_mods[MOD_REARWHEEL_VAR];
			}

			VEHICLE::SET_VEHICLE_MOD(vehicle, slot, owned_mods[slot], custom_tire);
		}
	}

	for (int extra = MOD_EXTRA_14; extra <= MOD_EXTRA_1; extra++)
	{
		int gta_extra_id = (extra - MOD_EXTRA_0) * -1;
		if (owned_mods.count(extra) && VEHICLE::DOES_EXTRA_EXIST(vehicle, gta_extra_id))
		{
			VEHICLE::SET_VEHICLE_EXTRA(vehicle, gta_extra_id, owned_mods[extra] == 0);
		}
	}

	return vehicle;
}



float m_heading = 0.5;
void menu::vehicles()
{

	ImGui::SetScrollX(0);
	ImGui::GetStyle().WindowPadding = ImVec2(11, 11);
	g_UiManager->Toggle("Spawn Inside", false, &toggles::vehicle::spawn_inside);
	g_UiManager->Toggle("Spawn Maxed", false, &toggles::vehicle::spawn_maxed);

	static int selected_class = -1;
	auto class_arr = g_data_manager->get_vehicle_class_arr();
	ImGui::SetNextItemWidth(321);
	if (ImGui::BeginCombo("##class", selected_class == -1 ? "ALL" : class_arr[selected_class].c_str()))
	{
		if (ImGui::Selectable("ALL", selected_class == -1))
		{
			selected_class = -1;
		}

		for (int i = 0; i < class_arr.size(); i++)
		{
			if (ImGui::Selectable(class_arr[i].c_str(), selected_class == i))
			{
				selected_class = i;
			}

			if (selected_class == i)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::ListBoxHeader("###vehicles", { 325, 300 }))
	{
		if (g_local->veh)
		{
			static auto veh_hash = 0;

			g_FiberQueue->queue_job([] {
				veh_hash = ENTITY::GET_ENTITY_MODEL(g_local->veh);
				});

			if (veh_hash)
			{
				auto item = g_data_manager->find_vehicle_by_hash(veh_hash);
				const std::string_view text = "Current Vehicle [" + item.display_name + "]";
				if (ImGui::Selectable(text.data(), false))
				{
					g_FiberQueue->queue_job([item] {
						if (g_local->veh)
						{
							NativeVector3 spawn_location = get_spawn_location(toggles::vehicle::spawn_inside);
							float spawn_heading = ENTITY::GET_ENTITY_HEADING(g_local->ped);

							auto owned_mods = get_owned_mods_from_vehicle(g_local->veh);

							auto veh = clone_from_owned_mods(owned_mods, spawn_location, spawn_heading);

							if (veh == 0)
							{
								g_UiManager->Notification({ ImGuiNotificationType_Error, 3000, "Failed to spawn: %s", item.display_name.data() });
							}
							else
							{
								if (toggles::vehicle::spawn_maxed)
								{
									max_vehicle(veh);
								}


								if (toggles::vehicle::spawn_inside)
								{
									teleport_into_vehicle(veh);
								}
							}
						}
						});
				}

			}
		}

		auto item_arr = g_data_manager->get_vehicle_arr();

		if (item_arr.size() > 0)
		{
			std::string lower_search = search;
			std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), tolower);

			for (auto& item : item_arr) {
				std::string display_name = item.display_name;
				std::string display_manufacturer = item.display_manufacturer;
				std::string clazz = item.clazz;

				std::transform(display_name.begin(), display_name.end(), display_name.begin(), ::tolower);
				std::transform(display_manufacturer.begin(), display_manufacturer.end(), display_manufacturer.begin(), ::tolower);

				if ((
					selected_class == -1 || class_arr[selected_class] == clazz
					) && (
						display_name.find(lower_search) != std::string::npos ||
						display_manufacturer.find(lower_search) != std::string::npos
						)) {
					ImGui::PushID(item.hash);
					if (ImGui::Selectable(item.display_name.data(), false))
						g_FiberQueue->queue_job([item]
							{

								NativeVector3 spawn_location = get_spawn_location(toggles::vehicle::spawn_inside);
								float spawn_heading = ENTITY::GET_ENTITY_HEADING(g_local->ped);
								const Vehicle veh = spawn(item.hash, spawn_location, spawn_heading);

								if (veh == 0)
								{
									g_UiManager->Notification({ ImGuiNotificationType_Error, 3000, "Failed to spawn: %s", item.display_name.data( )});
								}
								else
								{
									if (toggles::vehicle::spawn_maxed)
									{
										max_vehicle(veh);
									}


									if (toggles::vehicle::spawn_inside)
									{
										teleport_into_vehicle(veh);
									}
								}
							});
					ImGui::PopID();

				}
			}
		}
		else
		{
			ImGui::Text("Vehicle registry failed to load!");
		}
		ImGui::ListBoxFooter();
	}

}