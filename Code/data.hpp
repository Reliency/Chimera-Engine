#pragma once
#pragma once
#include "common.hpp"
#include "file.hpp"
#include "vehicle_data.hpp"
#include "ped_data.hpp"
#include "weapon_data.hpp"
#include "rage/joaat.hpp"


class data_manager
{
	std::vector<std::string> m_vehicle_class_arr;
	std::map<Hash, int> m_vehicle_hash_idx_map;
	std::vector<vehicle_data> m_vehicle_data_arr;
	const vehicle_data empty_vehicle_data = vehicle_data();

	std::vector<std::string> m_ped_type_arr;
	std::map<Hash, int> m_ped_hash_idx_map;
	std::vector<ped_data> m_ped_data_arr;
	const ped_data empty_ped_data = ped_data();

	std::vector<std::string> m_weapon_type_arr;
	std::map<Hash, int> m_weapon_hash_idx_map;
	std::vector<weapon_data> m_weapon_data_arr;
	const weapon_data empty_weapon_data = weapon_data();

public:
	data_manager();
	~data_manager();

	const vehicle_data& find_vehicle_by_hash(Hash hash);
	const std::vector<std::string>& get_vehicle_class_arr();
	const std::vector<vehicle_data>& get_vehicle_arr();

	const ped_data& find_ped_by_hash(Hash hash);
	const std::vector<std::string>& get_ped_type_arr();
	const std::vector<ped_data>& get_ped_arr();

	const weapon_data& find_weapon_by_hash(Hash hash);
	const std::vector<std::string>& get_weapon_type_arr();
	const std::vector<weapon_data>& get_weapon_arr();

private:
	void load_from_file(
		std::string file_path,
		bool(data_manager::* load_func)(std::filesystem::path), std::string data_name
	);

	bool load_vehicles(std::filesystem::path path);
	bool load_peds(std::filesystem::path path);
	bool load_weapons(std::filesystem::path path);
};

inline data_manager* g_data_manager{};