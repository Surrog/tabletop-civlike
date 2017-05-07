#include "data_parser.hpp"

data_parser::data_parser(const astd::filesystem::path& directory)
{
	parse_configuration_directory(directory);
}

const game_data& data_parser::data() const
{
	return _data;
}

game_data&& data_parser::get()
{
	return std::move(_data);
}

int data_parser::parse_def_attack(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			unit_action acc;
			acc.id = reference{ obj_names[i] };
			acc.name = current_obj["name"].asString();
			acc.description = current_obj["description"].asString();
			acc.soft = current_obj["soft"].asInt();
			acc.hard = current_obj["hard"].asInt();
			auto& range_container = current_obj["range"];
			if (!range_container.size() || range_container.size() > 2)
			{
            std::cerr << path << std::endl;
				std::cerr << "unrecognized range for " << acc.id << std::endl;
			}
			std::size_t range_index = 0;
			for (auto& num : range_container)
			{
				acc.range[range_index % acc.range.size()] = num.asUInt();
            range_index++;
			}

			acc.cost = current_obj["cost"].asInt();

			_data.attack_action.emplace_back(acc);
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_def_defense(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			unit_action acc;
			acc.id = reference{ obj_names[i] };
			acc.name = current_obj["name"].asString();
			acc.description = current_obj["description"].asString();
			acc.soft = current_obj["soft"].asInt();
			acc.hard = current_obj["hard"].asInt();

			_data.defense_action.emplace_back(acc);
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

//def_map
int data_parser::parse_def_map(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			terrain acc;
			acc.id = reference{ obj_names[i] };
			acc.name = current_obj["name"].asString();
			acc.description = current_obj["description"].asString();
			acc.infrastructure = current_obj["infrastructure"].asFloat();
			acc.cover = current_obj["cover"].asInt();
			acc.texture_path = current_obj["texture"].asString();

			_data.terrains.emplace_back(acc);
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

//def_unit
int data_parser::parse_def_unit(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			unit_definition acc;
			acc.id = reference{ obj_names[i] };
			acc.name = current_obj["name"].asString();
			acc.description = current_obj["description"].asString();
			for (auto& att : current_obj["attack"])
			{
				acc.attack.emplace_back(reference{ att.asCString() });
			}
			for (auto& def : current_obj["defense"])
			{
				acc.defense.emplace_back(reference{ def.asCString() });
			}

			for (auto& ord : current_obj["actions"])
			{
				acc.order_accessible.emplace_back(order::parse(ord.asString()));
			}

			acc.action_point = current_obj["action_points"].asInt();
			acc.cover_usage = current_obj["cover_usage"].asFloat();
			acc.texture = current_obj["texture"].asString();

			_data.unit_defs.emplace_back(acc);
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_map(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		map acc;
		acc.name = root["name"].asString();
		acc.description = root["description"].asString();
		acc.diameter = root["diameter"].asUInt();

		for (auto& hexa : root["tiles"])
		{
			std::pair<coordinate, reference> tiles;

			auto pos_it = hexa.begin();
			auto ref_it = hexa.begin(); ref_it++;

			tiles.first = parse_coord_from_value(*pos_it);
			tiles.second = reference(ref_it->asCString());
			acc.grid.emplace_back(std::move(tiles));
		}

		_data.current_map = acc;
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_order(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			auto unit_ref = reference(obj_names[i]);
			auto unit_it = std::find_if(_data.units.begin(), _data.units.end(),
				[&unit_ref](const unit& u) {return u.id == unit_ref; });
			if (unit_it == _data.units.end())
			{
				unit u;
				u.id = unit_ref;
				unit_it = _data.units.insert(_data.units.end(), std::move(u));
			}

			for (auto& acc : current_obj)
			{
				order ord;
				ord.type = order::parse(acc["action"].asCString());
				ord.target = parse_coord_from_value(acc["x"].asInt(), acc["y"].asInt());
				
				if (acc["modifier"] != Json::Value())
				{
					ord.modifier = reference(acc["modifier"].asCString());
				}
				unit_it->actions.emplace_back(ord);
			}
	
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_player(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			player new_player;
			new_player.id = reference{ obj_names[i] };
			new_player.description = current_obj["description"].asString();
			new_player.team = current_obj["team"].size() ? current_obj["team"].asCString()[0] : '1';
			for (auto& new_rally : current_obj["rally_points"])
			{
				new_player.rally_point.emplace_back(parse_coord_from_value(new_rally));
			}

			_data.players.emplace_back(new_player);
			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_unit(const astd::filesystem::path& path)
{
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::ifstream stream(path.c_str());
	if (!stream)
	{
		return FILE_OPEN_FAILED;
	}
	bool parsing_ok = Json::parseFromStream(rbuilder, stream, &root, &errs);
	if (parsing_ok)
	{
		auto obj_names = root.getMemberNames();
		std::size_t i = 0;
		for (auto& current_obj : root)
		{
			auto unit_ref = reference(obj_names[i]);
			auto unit_it = std::find_if(_data.units.begin(), _data.units.end(),
				[&unit_ref](const unit& u) {return u.id == unit_ref; });
			if (unit_it == _data.units.end())
			{
				unit u;
				u.id = unit_ref;
				unit_it = _data.units.insert(_data.units.end(), std::move(u));
			}

			unit_it->id = reference{ obj_names[i] };
			unit_it->owner = reference(current_obj["owner"].asCString());
			unit_it->type = reference(current_obj["type"].asCString());
			unit_it->pos = parse_coord_from_value(current_obj["position"]);
			unit_it->endurance = current_obj["endurance"].asInt();

			++i;
		}
	}
	else
	{
		std::cerr << "parsing " << path << " failed : " << errs << std::endl;
		return JSON_PARSING_FAILED;
	}

	return NONE;
}

int data_parser::parse_configuration_directory(const astd::filesystem::path& directory)
{
	constexpr std::size_t FILE_TYPE_SIZE = 8;
	typedef std::pair<std::string, int(data_parser::*)(const astd::filesystem::path&)> state_machine_pair;
	std::array<state_machine_pair, FILE_TYPE_SIZE> parsing_state_machine =
	{
		state_machine_pair{ "def_attack", &data_parser::parse_def_attack }
		,{ "def_defense", &data_parser::parse_def_defense }
		,{ "def_map", &data_parser::parse_def_map }
		,{ "def_unit", &data_parser::parse_def_unit }
		,{ "map", &data_parser::parse_map }
		,{ "order", &data_parser::parse_order }
		,{ "player", &data_parser::parse_player }
		,{ "unit", &data_parser::parse_unit }
	};

	astd::filesystem::directory_iterator it(directory);
	astd::filesystem::directory_iterator ite;

	int result = 0;

	while (it != ite)
	{
		if (it->status().type() == astd::filesystem::file_type::regular)
		{
			auto parsing_function_it = std::find_if(parsing_state_machine.begin(), parsing_state_machine.end(), [&it](const state_machine_pair& pair) {return !it->path().filename().generic_string().compare(0, pair.first.size(), pair.first); });
			if (parsing_function_it != parsing_state_machine.end())
			{
				result = result | (this->*parsing_function_it->second) (it->path());
			}
			else
			{
				std::cerr << "WARNING : file " << it->path() << " not recognized by the resolver" << std::endl;
			}
		}
		++it;
	}

	return result;
}

coordinate data_parser::parse_coord_from_value(std::int32_t x, std::int32_t y)
{
	return { x, y, -1 * (x + y) };
}

coordinate data_parser::parse_coord_from_value(std::int32_t x, std::int32_t y, std::int32_t z)
{
	coordinate result{ x, y, z };
	if ((result.x + result.y + result.z) != 0)
	{
		std::cerr << "WARNING : coordinate " << result << " not valid (sum not equal to zero)" << std::endl;
	}

	return result;
}

coordinate data_parser::parse_coord_from_value(const Json::Value& value)
{
	coordinate result;

	auto size = value.size();

	if (value.size() == 2)
	{
		auto x_it = value.begin();
		auto y_it = value.begin(); y_it++;
		result = parse_coord_from_value(x_it->asInt(), y_it->asInt());
	}

	if (value.size() == 3)
	{
		auto x_it = value.begin();
		auto y_it = value.begin(); y_it++;
		auto z_it = y_it; z_it++;
		result = parse_coord_from_value(x_it->asInt(), y_it->asInt(), z_it->asInt());
	}

	if (value.size() != 2 && value.size() != 3)
	{
		std::cerr << "WARNING : coordinate " << value << "not valid (size != 2 || 3)" << std::endl;
	}

	return result;
}
