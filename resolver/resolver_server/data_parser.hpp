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

class data_parser {
public:
   enum error_code : int
   {
      NONE = 0
      , FILE_OPEN_FAILED
      , JSON_PARSING_FAILED

   };

   data_parser(const astd::filesystem::path& directory);

   const game_data& data() const;

   game_data&& get();

private:
   game_data _data;

   int parse_def_attack(const astd::filesystem::path& path);

   int parse_def_defense(const astd::filesystem::path& path);

   int parse_def_map(const astd::filesystem::path& path);

   int parse_def_unit(const astd::filesystem::path& path);

   int parse_map(const astd::filesystem::path& path);

   int parse_order(const astd::filesystem::path& path);

   int parse_player(const astd::filesystem::path& path);

   int parse_unit(const astd::filesystem::path& path);

   int parse_configuration_directory(const astd::filesystem::path& directory);

   static coordinate parse_coord_from_value(std::int32_t x, std::int32_t y);

   static coordinate parse_coord_from_value(std::int32_t x, std::int32_t y, std::int32_t z);

   static coordinate parse_coord_from_value(const Json::Value&);
};


#endif //DATA_PARSER_HPP