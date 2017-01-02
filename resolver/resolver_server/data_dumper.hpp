#ifndef DATA_DUMPER_HPP
#define DATA_DUMPER_HPP

#include "afilesystem.hpp"
#include "data.hpp"
#include "json/json.h"
#include <fstream>

class data_dumper
{
public:
   enum error_code : int {
      NONE,
      OPEN_FILE
   };

   data_dumper(const game_data& data, const astd::filesystem::path& target);

   error_code status() const;

private:
   int dump_def_attack(const astd::filesystem::path& path, const std::vector<unit_action>& acc);

   int dump_def_defense(const astd::filesystem::path& path, const std::vector<unit_action>& acc);

   int dump_def_map(const astd::filesystem::path& path, const std::vector<terrain>& terrains);

   int dump_def_unit(const astd::filesystem::path& path, const std::vector<unit_definition>& unit_defs);

   int dump_map(const astd::filesystem::path& path, const map& current_map);

   int dump_order(const astd::filesystem::path& path, const std::vector<order>& orders);

   int dump_player(const astd::filesystem::path& path, const std::vector<player>& players);

   int dump_unit(const astd::filesystem::path& path, const std::vector<unit>& units);

   Json::Value coord_to_json_value(const coordinate& coord);

   int _status;
};

#endif //!DATA_DUMPER_HPP