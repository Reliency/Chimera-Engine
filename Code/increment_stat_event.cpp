#include "Hooking.hpp"
#include "net_event.hpp"
#include "protections.hpp"
#include "UI/UIManager.hpp"
bool Hooks::increment_stat_event(CNetworkIncrementStatEvent* net_event, CNetGamePlayer* sender)
{
	switch (net_event->m_stat)
	{
	case CONSTEXPR_JOAAT("MPPLY_BAD_CREW_STATUS"):
	case CONSTEXPR_JOAAT("MPPLY_BAD_CREW_MOTTO"):
	case CONSTEXPR_JOAAT("MPPLY_BAD_CREW_NAME"):
	case CONSTEXPR_JOAAT("MPPLY_BAD_CREW_EMBLEM"):
	case CONSTEXPR_JOAAT("MPPLY_EXPLOITS"):
	case CONSTEXPR_JOAAT("MPPLY_GAME_EXPLOITS"):
	case CONSTEXPR_JOAAT("MPPLY_TC_ANNOYINGME"):
	case CONSTEXPR_JOAAT("MPPLY_TC_HATE"):
	case CONSTEXPR_JOAAT("MPPLY_VC_ANNOYINGME"):
	case CONSTEXPR_JOAAT("MPPLY_VC_HATE"):
		if (g_Protections.reports_event)
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Block report from %s", sender->get_name() });
		return true;
	}

	return false;
}