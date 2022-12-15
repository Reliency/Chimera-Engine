#pragma once
#include "file.hpp"

class ped_data {
public:
	ped_data();
	ped_data(nlohmann::json& item_json);

	std::string name;
	std::string ped_type;
	Hash hash;
};
