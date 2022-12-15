#pragma once
#include "Natives.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"
#include "enums.hpp"
#include "toggles.hpp"
#include "Common.hpp"
#include "player_getter.hpp"
#include "local_features.hpp"
#include "recovery_features.hpp"
#include "player_features.hpp"
namespace Chimera {
	class local {
	public:
		Chimera::Ped ped;
		Chimera::Player id;
		Chimera::NativeVector3 pos;
		Chimera::Vehicle veh;
	};
	inline std::unique_ptr<local> g_local{};

	static void onTickGTA()
	{
		g_local->id = PLAYER::PLAYER_ID();
		g_local->ped = PLAYER::PLAYER_PED_ID();

		if (PED::IS_PED_IN_ANY_VEHICLE(g_local->ped, 0))
		{
			g_local->veh = PED::GET_VEHICLE_PED_IS_IN(g_local->ped, false);
		}
		else
		{
			g_local->veh = 0;
		}

		g_FiberQueue->fiber_tick();
		recovery::stealthLoop(toggles::recovery::stealth_loop);
		self::godmode(toggles::self::godmode);
		self::neverWanted(toggles::self::never_wanted);
		self::superman(toggles::self::superman);
		self::setRagdoll(toggles::self::ragdoll);
		self::invisible(toggles::self::invisible);
		self::off_radar(toggles::self::off_radar);
		remote_player::spectate(toggles::player::spectate);
		remote_player::cash_drop(toggles::player::remote_cash_drop);
		remote_player::off_radar(toggles::player::off_radar);
		remote_player::never_wanted(toggles::player::never_wanted);
	}


}