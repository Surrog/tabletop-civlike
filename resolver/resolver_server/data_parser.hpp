#ifndef DATA_PARSER_HPP
#define DATA_PARSER_HPP

#include "afilesystem.hpp"
#include "data.hpp"
#include <utility>
#include <string>
#include <array>
#include <cstddef>

struct data_parser {
public:
	data_parser(const astd::filesystem::path& directory)
	{
		parse_configuration_directory(directory);
	}

	const data& get() const
	{
		return _data;
	}

	data&& fetch()
	{
		return std::move(_data);
	}

private:
	data _data;

	int parse_def_attack(const astd::filesystem::path& path)
	{
		return 0;
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
			state_machine_pair{ "def_attack.json", &data_parser::parse_def_attack }
			,{ "def_defense.json", &data_parser::parse_def_defense }
			,{ "def_map.json", &data_parser::parse_def_map }
			,{ "def_unit.json", &data_parser::parse_def_unit }
			,{ "map.json", &data_parser::parse_map }
			,{ "order.json", &data_parser::parse_order }
			,{ "player.json", &data_parser::parse_player }
			,{ "unit.json", &data_parser::parse_unit }
		};

		astd::filesystem::directory_iterator it(directory);
		astd::filesystem::directory_iterator ite;

		int result = 0;

		while (it != ite)
		{
			if (it->status().type() == astd::filesystem::file_type::regular)
			{
				auto parsing_function_it = std::find_if(parsing_state_machine.begin(), parsing_state_machine.end(), [&it](const state_machine_pair& pair) {return pair.first == it->path().filename().generic_string(); });
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