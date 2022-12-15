#include "Common.hpp"
#include "file_manager.hpp"
#include "data.hpp"
#include "remote.hpp"
#include "httplib.hpp"
#define EXIST_IN_ARRAY(arr, val) (std::find(std::begin(arr), std::end(arr), val) != std::end(arr))

nlohmann::json ReadJsonFromFile(std::string file_name) {

	try {
		return nlohmann::json::parse(std::ifstream{ file_name, std::ios::in });
	}
	catch (nlohmann::json::parse_error& e) {
		return NULL;
	}

	return {};
}
data_manager::data_manager()
{

	httplib::Client cli("http://gtavdata.cloud");
	nlohmann::json weapons_json;
	nlohmann::json peds_json;
	nlohmann::json vehicles_json;

	if (!std::filesystem::exists(g_file_manager->get_project_file("lib\\version.json").get_path()) || std::filesystem::is_empty(g_file_manager->get_project_file("lib\\version.json").get_path()))
	{
		std::ofstream version_file(g_file_manager->get_project_file("lib\\version.json").get_path());
		LOG(INFO) << "GTA Data : Fresh Install...";
		if (auto res = cli.Get("/vehicles.json"))
		{
			std::ofstream vehicles_file(g_file_manager->get_project_file("lib\\vehicles.json").get_path());
			vehicles_file << res->body;
			vehicles_file.close();
			LOG(INFO) << "Vehicles.json : Success";
		}
		else LOG(WARNING) << "Vehicles.json : Failed";

		if (auto res = cli.Get("/peds.json"))
		{
			std::ofstream peds_file(g_file_manager->get_project_file("lib\\peds.json").get_path());
			peds_file << res->body;
			peds_file.close();
			LOG(INFO) << "Peds.json : Success";
		}
		else LOG(WARNING) << "Peds.json : Failed";

		if (auto res = cli.Get("/weapons.json"))
		{
			std::ofstream weapons_file(g_file_manager->get_project_file("lib\\weapons.json").get_path());
			weapons_file << res->body;
			weapons_file.close();
			LOG(INFO) << "Weapons.json : Success";
		}
		else LOG(WARNING) << "Weapons.json : Failed";
		if (auto res = cli.Get("/version.json"))
		{
			version_file << res->body;
			version_file.close();
			LOG(INFO) << "Version.json : Success";
		}

		load_vehicles(g_file_manager->get_project_file("lib\\vehicles.json").get_path());
		load_peds(g_file_manager->get_project_file("lib\\peds.json").get_path());
		load_weapons(g_file_manager->get_project_file("lib\\weapons.json").get_path());
		g_data_manager = this;
		return;
	}

	std::ofstream server_version_file(g_file_manager->get_project_file("lib\\server_version.json").get_path());

	if (std::filesystem::is_empty(g_file_manager->get_project_file("lib\\server_version.json").get_path()))
	{
		if (auto res = cli.Get("/version.json"))
		{
			server_version_file << res->body;
			server_version_file.close();
			LOG(INFO) << "Server Version.json : Success";
		}
		else
		{
			LOG(INFO) << "Failed to connect to server...";
			ImGui::InsertNotification({ ImGuiNotificationType_Error, 3000, "GTA Data Server Connection Failed!" });
			return;
		}
		nlohmann::json server_version_json = ReadJsonFromFile(g_file_manager->get_project_file("lib\\server_version.json").get_path().string());
		nlohmann::json local_version_json = ReadJsonFromFile(g_file_manager->get_project_file("lib\\version.json").get_path().string());

		if (server_version_json["Version"] == local_version_json["Version"])
		{
			LOG(INFO) << "GTA Data : Up to date";
			std::filesystem::remove(g_file_manager->get_project_file("lib\\server_version.json").get_path());
			load_vehicles(g_file_manager->get_project_file("lib\\vehicles.json").get_path());
			load_peds(g_file_manager->get_project_file("lib\\peds.json").get_path());
			load_weapons(g_file_manager->get_project_file("lib\\weapons.json").get_path());
			g_data_manager = this;
			return;
		}
		else
		{
			std::ofstream version_file(g_file_manager->get_project_file("lib\\version.json").get_path());

			LOG(INFO) << "GTA Data : Updating...";
			if (auto res = cli.Get("/vehicles.json"))
			{
				std::ofstream vehicles_file(g_file_manager->get_project_file("lib\\vehicles.json").get_path());
				vehicles_file << res->body;
				vehicles_file.close();
				LOG(INFO) << "Vehicles.json : Success";
			}
			else LOG(WARNING) << "Vehicles.json : Failed";

			if (auto res = cli.Get("/peds.json"))
			{
				std::ofstream peds_file(g_file_manager->get_project_file("lib\\peds.json").get_path());
				peds_file << res->body;
				peds_file.close();
				LOG(INFO) << "Peds.json : Success";
			}
			else LOG(WARNING) << "Peds.json : Failed";

			if (auto res = cli.Get("/weapons.json"))
			{
				std::ofstream weapons_file(g_file_manager->get_project_file("lib\\weapons.json").get_path());
				weapons_file << res->body;
				weapons_file.close();
				LOG(INFO) << "Weapons.json : Success";
			}
			else LOG(WARNING) << "Weapons.json : Failed";
			if (auto res = cli.Get("/version.json"))
			{
				version_file << res->body;
				version_file.close();
				LOG(INFO) << "Version.json : Success";
			}
			std::filesystem::remove(g_file_manager->get_project_file("lib\\server_version.json").get_path());
			load_vehicles(g_file_manager->get_project_file("lib\\vehicles.json").get_path());
			load_peds(g_file_manager->get_project_file("lib\\peds.json").get_path());
			load_weapons(g_file_manager->get_project_file("lib\\weapons.json").get_path());
		}
	}
	g_data_manager = this;
}

data_manager::~data_manager()
{
	g_data_manager = nullptr;
}

const vehicle_data& data_manager::find_vehicle_by_hash(Hash hash)
{
	int idx = -1;

	if (m_vehicle_hash_idx_map.count(hash))
	{
		idx = m_vehicle_hash_idx_map[hash];
	}

	if (idx == -1)
	{
		return empty_vehicle_data;
	}

	return m_vehicle_data_arr[idx];
}

const std::vector<std::string>& data_manager::get_vehicle_class_arr()
{
	return m_vehicle_class_arr;
}

const std::vector<vehicle_data>& data_manager::get_vehicle_arr()
{
	return m_vehicle_data_arr;
}


const ped_data& data_manager::find_ped_by_hash(Hash hash)
{
	int idx = -1;

	if (m_ped_hash_idx_map.count(hash))
	{
		idx = m_ped_hash_idx_map[hash];
	}

	if (idx == -1)
	{
		return empty_ped_data;
	}

	return m_ped_data_arr[idx];
}

const std::vector<std::string>& data_manager::get_ped_type_arr()
{
	return m_ped_type_arr;
}

const std::vector<ped_data>& data_manager::get_ped_arr()
{
	return m_ped_data_arr;
}


const weapon_data& data_manager::find_weapon_by_hash(Hash hash)
{
	int idx = -1;

	if (m_weapon_hash_idx_map.count(hash))
	{
		idx = m_weapon_hash_idx_map[hash];
	}

	if (idx == -1)
	{
		return empty_weapon_data;
	}

	return m_weapon_data_arr[idx];
}

const std::vector<std::string>& data_manager::get_weapon_type_arr()
{
	return m_weapon_type_arr;
}

const std::vector<weapon_data>& data_manager::get_weapon_arr()
{
	return m_weapon_data_arr;
}


void data_manager::load_from_file(
	std::string file_path,
	bool(data_manager::* load_func)(std::filesystem::path), std::string data_name
) {
	LOG(INFO) << file_path;
	file file_to_load(g_file_manager->get_project_file(file_path));
	LOG(INFO) << file_to_load.get_path();
	auto file_to_load_path = file_to_load.get_path();
}

bool data_manager::load_vehicles(std::filesystem::path path)
{
	
	std::ifstream file(path);
	nlohmann::json all_vehicles;

	try
	{
		file >> all_vehicles;

		if (!all_vehicles.is_array())
		{
			throw std::exception("Invalid json format.");
		}

		m_vehicle_class_arr.clear();
		m_vehicle_hash_idx_map.clear();
		m_vehicle_data_arr.clear();

		for (auto& item_json : all_vehicles)
		{
			if (
				item_json["Hash"].is_null() ||
				item_json["Name"].is_null() ||
				item_json["MonetaryValue"] == 0 ||
				!item_json["Bones"].is_array() ||
				item_json["Bones"][0] == "stub"
				) {
				continue;
			}

			auto item = vehicle_data(item_json);

			m_vehicle_hash_idx_map[item_json["Hash"]] = (int)m_vehicle_data_arr.size();

			m_vehicle_data_arr.push_back(item);

			if (std::find(m_vehicle_class_arr.begin(), m_vehicle_class_arr.end(), item.clazz) == m_vehicle_class_arr.end())
			{
				m_vehicle_class_arr.push_back(item.clazz);
			}

			std::sort(m_vehicle_class_arr.begin(), m_vehicle_class_arr.end());
		}
	}
	catch (const std::exception& ex)
	{
		LOG(WARNING) << "Failed to load vehicles.json:\n" << ex.what();
		return false;
	}

	return true;
}


bool data_manager::load_peds(std::filesystem::path path)
{
	std::ifstream file(path);
	nlohmann::json all_peds;

	try
	{
		file >> all_peds;

		if (!all_peds.is_array())
		{
			throw std::exception("Invalid json format.");
		}

		m_ped_type_arr.clear();
		m_ped_hash_idx_map.clear();
		m_ped_data_arr.clear();

		for (auto& item_json : all_peds)
		{
			if (
				item_json["Hash"].is_null() ||
				item_json["Name"].is_null()
				) {
				continue;
			}

			auto item = ped_data(item_json);

			m_ped_hash_idx_map[item_json["Hash"]] = (int)m_ped_data_arr.size();

			m_ped_data_arr.push_back(item);

			if (std::find(m_ped_type_arr.begin(), m_ped_type_arr.end(), item.ped_type) == m_ped_type_arr.end())
			{
				m_ped_type_arr.push_back(item.ped_type);
			}

			std::sort(m_ped_type_arr.begin(), m_ped_type_arr.end());
		}
	}
	catch (const std::exception& ex)
	{
		LOG(WARNING) << "Failed to load peds.json:\n" << ex.what();
		return false;
	}

	return true;
}


bool data_manager::load_weapons(std::filesystem::path path)
{
	std::ifstream file(path);
	nlohmann::json all_weapons;

	try
	{
		file >> all_weapons;

		if (!all_weapons.is_array())
		{
			throw std::exception("Invalid json format.");
		}

		m_weapon_type_arr.clear();
		m_weapon_hash_idx_map.clear();
		m_weapon_data_arr.clear();

		constexpr Hash hash_blacklist_arr[] = {
			CONSTEXPR_JOAAT("WEAPON_BIRD_CRAP"),
			CONSTEXPR_JOAAT("WEAPON_DIGISCANNER"),
			CONSTEXPR_JOAAT("WEAPON_GARBAGEBAG"),
			CONSTEXPR_JOAAT("WEAPON_GRENADELAUNCHER_SMOKE"),
			CONSTEXPR_JOAAT("WEAPON_HANDCUFFS"),
			CONSTEXPR_JOAAT("WEAPON_METALDETECTOR"),
			CONSTEXPR_JOAAT("GADGET_NIGHTVISION"),
			CONSTEXPR_JOAAT("GADGET_PARACHUTE"),
			CONSTEXPR_JOAAT("WEAPON_TRANQUILIZER"),
			CONSTEXPR_JOAAT("WEAPON_STINGER")
		};

		for (auto& item_json : all_weapons)
		{
			if (
				item_json["Hash"].is_null() ||
				item_json["Name"].is_null() ||
				item_json["IsVehicleWeapon"]
				) {
				continue;
			}

			if (EXIST_IN_ARRAY(hash_blacklist_arr, item_json["Hash"]))
			{
				continue;
			}

			auto item = weapon_data(item_json);

			if (item.name == "Invalid" || item.name == "Unarmed" || item.weapon_type == "NULL")
			{
				continue;
			}

			m_weapon_hash_idx_map[item_json["Hash"]] = (int)m_weapon_data_arr.size();

			m_weapon_data_arr.push_back(item);

			if (std::find(m_weapon_type_arr.begin(), m_weapon_type_arr.end(), item.weapon_type) == m_weapon_type_arr.end())
			{
				m_weapon_type_arr.push_back(item.weapon_type);
			}

			std::sort(m_weapon_type_arr.begin(), m_weapon_type_arr.end());
		}

	}
	catch (const std::exception& ex)
	{
		LOG(WARNING) << "Failed to load weapons.json:\n" << ex.what();
		return false;
	}

	return true;
}
