#pragma once
#include "file.hpp"

class weapon_data {
public:
	weapon_data();
	weapon_data(nlohmann::json& item_json);

	std::string name;
	bool throwable;
	std::string weapon_type;

	Hash hash;
	Hash reward_hash;
	Hash reward_ammo_hash;
};
