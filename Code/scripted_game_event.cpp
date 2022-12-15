#include "protections.hpp"
#include "UI/UIManager.hpp"
bool Hooks::scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player)
{
	const auto args = scripted_game_event->m_args;

	const auto hash = static_cast<eRemoteEvent>(args[0]);
	const auto player_name = player->get_name();

	switch (hash)
	{
	case eRemoteEvent::Bounty:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Bounty" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Bounty \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::CeoBan:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: CEO Ban" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: CEO Ban \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::CeoKick:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: CEO Kick" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: CEO Kick \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::CeoMoney:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: CEO Cash" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: CEO Cash \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::ClearWantedLevel:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Clear Wanted" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Clear Wanted \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::Crash:
	case eRemoteEvent::Crash2:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Script Crash" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Script Crash \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::Notification:
		switch (static_cast<eRemoteEvent>(args[2]))
		{
		case eRemoteEvent::NotificationMoneyBanked:
		case eRemoteEvent::NotificationMoneyRemoved:
		case eRemoteEvent::NotificationMoneyStolen:
			if (g_Protections.script_event)
			{
				LOG(INFO) << "Script Event: Notification" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Notification \n From: %s", player_name });
				return true;
			}
			break;
		}
		break;
	case eRemoteEvent::ForceMission:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Force Mission" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Force Mission \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::GiveCollectible:
		if (g_Protections.script_event)
		{
			if (args[2] == 8)
			{
				LOG(INFO) << "Script Event: Model Switch" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Model Swap \n From: %s", player_name });
				return true;
			}
		}
		break;
	case eRemoteEvent::GtaBanner:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Banner" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Banner \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::MCTeleport:
		if (g_Protections.script_event && args[3] <= 32)
		{
			LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
			return true;
		}
		else if (g_Protections.script_event && args[3] > 32)
		{
			LOG(INFO) << "Script Event: Script Crash" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Script Crash \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::NetworkBail:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Network Bail" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Network Bail \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::PersonalVehicleDestroyed:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Vehicle Destroyed" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Veh Destroyed \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::RemoteOffradar:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Network Bail" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Network Bail \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::TSECommand:
		if (g_Protections.script_event && static_cast<eRemoteEvent>(args[2]) == eRemoteEvent::TSECommandRotateCam)
		{
			LOG(INFO) << "Script Event: Camera Rotation" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Rotate Camera \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::SendToCayoPerico:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Cayo Teleport" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Island Teleport \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::SendToCutscene:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Cutscene" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Cutscene \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::SendToLocation:
	{
		bool known_location = false;

		if (args[2] == 0 && args[3] == 0)
		{
			if (args[4] == 4 && args[5] == 0)
			{
				known_location = true;

				if (g_Protections.script_event)
				{
					LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
					g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
					return true;
				}
			}
			else if ((args[4] == 3 || args[4] == 4) && args[5] == 1)
			{
				known_location = true;

				if (g_Protections.script_event)
				{
					LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
					g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
					return true;
				}
			}
		}

		if (!known_location)
		{
			LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
			return true;
		}
		break;
	}
	case eRemoteEvent::SoundSpam:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Sound Spam" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Sound Spam \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::Spectate:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Spectate" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Spectate \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::Teleport:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::TransactionError:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Transaction Spam" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Transaction \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::VehicleKick:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Vehicle Kick" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Vehicle Kick \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::ForceMission2:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Mission" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Force Mission \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::TeleportToWarehouse:
		if (g_Protections.script_event)
		{
			LOG(INFO) << "Script Event: Teleport" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Teleport \n From: %s", player_name });
			return true;
		}
		break;
	case eRemoteEvent::StartActivity:
		eActivityType activity = static_cast<eActivityType>(args[2]);
		if (g_Protections.script_event)
		{
			if (activity == eActivityType::Survival || activity == eActivityType::Mission || activity == eActivityType::Deathmatch || activity == eActivityType::BaseJump || activity == eActivityType::Race)
			{
				LOG(INFO) << "Script Event: Activity" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Mission Activity \n From: %s", player_name });
				return true;
			}
			else if (activity == eActivityType::Darts)
			{
				LOG(INFO) << "Script Event: Activity" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Darts Activity \n From: %s", player_name });
				return true;
			}
			else if (activity == eActivityType::PilotSchool)
			{
				LOG(INFO) << "Script Event: Activity" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Pilot Activity \n From: %s", player_name });
				return true;
			}
			else if (activity == eActivityType::ImpromptuDeathmatch)
			{
				LOG(INFO) << "Script Event: Deathmatch" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Deathmatch \n From: %s", player_name });
				return true;
			}
			else if (activity == eActivityType::DefendSpecialCargo || activity == eActivityType::GunrunningDefend || activity == eActivityType::BikerDefend)
			{
				LOG(INFO) << "Script Event: Raid" << "\n" << "From: " << player_name;
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Raid \n From: %s", player_name });
				return true;
			}
		}
		else if (g_Protections.script_event && activity == eActivityType::Tennis)
		{
			LOG(INFO) << "Script Event: Activity" << "\n" << "From: " << player_name;
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "Script Event: Tennis \n From: %s", player_name });
			return true;
		}
		break;
	}

	return false;
}