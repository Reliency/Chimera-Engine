#include "ped_data.hpp"

ped_data::ped_data()
{
	this->name = "";
	this->ped_type = "";
	this->hash = 0;
}
ped_data::ped_data(nlohmann::json& item_json)
{
	this->name = item_json["Name"];

	this->ped_type = item_json["Pedtype"];
	std::transform(this->ped_type.begin(), this->ped_type.end(), this->ped_type.begin(), ::toupper);

	this->hash = item_json["Hash"];
}
