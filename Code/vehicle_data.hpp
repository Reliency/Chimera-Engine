#pragma once
#include "file.hpp"

class vehicle_data {
public:
	vehicle_data();
	vehicle_data(nlohmann::json& item_json);

	std::string name;
	std::string display_name;
	std::string display_manufacturer;
	std::string clazz;
	Hash hash;
};
