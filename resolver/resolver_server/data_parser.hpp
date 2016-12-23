#ifndef DATA_PARSER_HPP
#define DATA_PARSER_HPP

#include "afilesystem.hpp"
#include "json/json.h"
#include "data.hpp"
#include <utility>
#include <string>
#include <array>
#include <cstddef>
#include <fstream>

struct data_parser {
public:
   enum error_code : int
   {
      NONE = 0
      , FILE_OPEN_FAILED
      , JSON_PARSING_FAILED

   };

	data_parser(const astd::filesystem::path& directory)
	{
		parse_configuration_directory(directory);
	}

	const game_data& data() const
	{
		return _data;
	}

   game_data&& get()
	{
		return std::move(_data);
	}

private:
   game_data _data;

	int parse_def_attack(const astd::filesystem::path& path)
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
            auto range_str = current_obj["range"].asString();
            if (range_str.size() == 1)
               acc.range.first = acc.range.second = xts::fast_str_to_uint(range_str);
            else if (range_str.size() == 3)
            {
               acc.range.first = xts::fast_str_to_uint(range_str.substr(0, 1));
               acc.range.second = xts::fast_str_to_uint(range_str.substr(2, 1));
            }
            else
            {
               std::cerr << "unrecognized range for " << acc.id << std::endl;
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

	int parse_def_defense(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_def_map(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_def_unit(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_map(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_order(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_player(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_unit(const astd::filesystem::path& path)
	{
		return 0;
	}

	int parse_configuration_directory(const astd::filesystem::path& directory)
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
};


#endif //DATA_PARSER_HPP