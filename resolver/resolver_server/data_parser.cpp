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
				std::cerr << "unrecognized range for " << acc.id << std::endl;
			}
			std::size_t i = 0;
			for (auto& num : range_container)
			{
				acc.range[i % acc.range.size()] = num.asInt();
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
			acc.infrastructure = current_obj["infrastructure"].asInt();
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
				acc.attack.emplace_back(reference{ att.asString() });
			}
			for (auto& def : current_obj["defense"])
			{
				acc.defense.emplace_back(reference{ def.asString() });
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
			tiles.second = reference(ref_it->asString());
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
			order new_order;
			new_order.id = reference{ obj_names[i] };
			new_order.unit_source = reference(current_obj["unit"].asString());
			new_order.type = order::parse(current_obj["order"].asString());
			new_order.target = parse_coord_from_value(current_obj["target"]);

			_data.orders.emplace_back(new_order);
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
			unit new_unit;
			new_unit.id = reference{ obj_names[i] };
			new_unit.owner = reference(current_obj["owner"].asString());
			new_unit.type = reference(current_obj["type"].asString());
			new_unit.pos = parse_coord_from_value(current_obj["position"]);
			new_unit.endurance = current_obj["endurance"].asInt();

			_data.units.emplace_back(new_unit);
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

coordinate data_parser::parse_coord_from_value(const Json::Value& value)
{
	coordinate result;

	auto size = value.size();

	if (value.size() == 2)
	{
		auto x_it = value.begin();
		auto y_it = value.begin(); y_it++;
		result.x = x_it->asInt();
		result.y = y_it->asInt();
		result.z = -1 * (result.x + result.y);
	}

	if (value.size() == 3)
	{
		auto x_it = value.begin();
		auto y_it = value.begin(); y_it++;
		auto z_it = y_it; z_it++;
		result.x = x_it->asInt();
		result.y = y_it->asInt();
		result.z = z_it->asInt();
		if ((result.x + result.y + result.z) != 0)
		{
			std::cerr << "WARNING coordinate " << value << " not valid (sum not equal to zero)" << std::endl;
		}
	}

	if (value.size() != 2 && value.size() != 3)
	{
		std::cerr << "WARNING coordinate " << value << "not valid (size != 2 || 3)" << std::endl;
	}

	return result;
}
