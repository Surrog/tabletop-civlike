
#include "data_dumper.hpp"

data_dumper::data_dumper(const game_data& data, const astd::filesystem::path& target)
{
   if (!astd::filesystem::exists(target))
   {
      astd::filesystem::create_directories(target);
   }

   _status = dump_def_attack(target / "def_attack.json", data.attack_action);
   _status |= dump_def_defense(target / "def_defense.json", data.defense_action);
   _status |= dump_def_map(target / "def_map.json", data.terrains);
   _status |= dump_def_unit(target / "def_unit.json", data.unit_defs);
   _status |= dump_map(target / "map.json", data.current_map);
   _status |= dump_order(target / "order.json", data.units, false);
   _status |= dump_order(target / "order_rejected.json", data.units, true);
   _status |= dump_player(target / "player.json", data.players);
   _status |= dump_unit(target / "unit.json", data.units);
   _status |= dump_unit(target / "unit_dead.json", data.unit_dead);
}

data_dumper::error_code data_dumper::status() const
{
   return data_dumper::error_code(_status);
}

int data_dumper::dump_def_attack(const astd::filesystem::path& path, const std::vector<unit_action>& acc)
{
   if (!acc.size())
   {
      return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (const auto& action : acc)
      {
         Json::Value action_value;
         action_value["name"] = action.name;
         action_value["description"] = action.description;
         action_value["soft"] = action.soft;
         action_value["hard"] = action.hard;
         for (const auto& range : action.range)
         {
            action_value["range"].append(range);
         }
         action_value["cost"] = action.cost;
         root[action.id.serialize().data()] = action_value;
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_def_defense(const astd::filesystem::path& path, const std::vector<unit_action>& acc)
{
   if (!acc.size())
   {
      return NONE;
   }
   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (const auto& action : acc)
      {
         Json::Value action_value;
         action_value["name"] = action.name;
         action_value["description"] = action.description;
         action_value["soft"] = action.soft;
         action_value["hard"] = action.hard;

         root[action.id.serialize().data()] = action_value;
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_def_map(const astd::filesystem::path& path, const std::vector<terrain>& terrains)
{
   if (!terrains.size())
   {
      return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (const auto& terrain : terrains)
      {
         Json::Value action_value;
         action_value["name"] = terrain.name;
         action_value["description"] = terrain.description;
         action_value["infrastructure"] = terrain.infrastructure;
         action_value["cover"] = terrain.cover;
         action_value["texture"] = terrain.texture_path;

         root[terrain.id.serialize().data()] = action_value;
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_def_unit(const astd::filesystem::path& path, const std::vector<unit_definition>& unit_defs)
{
   if (!unit_defs.size())
   {
      return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (const auto& unit_def : unit_defs)
      {
         Json::Value action_value;
         action_value["name"] = unit_def.name;
         action_value["description"] = unit_def.description;
         for (const auto& attack : unit_def.attack)
         {
            action_value["attack"].append(attack.serialize().data());
         }

         for (const auto& defense : unit_def.defense)
         {
            action_value["defense"].append(defense.serialize().data());
         }

         for (const auto& action : unit_def.order_accessible)
         {
            action_value["actions"].append(order::serialize(action).data());
         }

         action_value["action_points"] = unit_def.action_point;
         action_value["cover_usage"] = unit_def.cover_usage;
         action_value["texture"] = unit_def.texture;

         root[unit_def.id.serialize().data()] = action_value;
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_map(const astd::filesystem::path& path, const map& current_map)
{
   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      root["name"] = current_map.name;
      root["description"] = current_map.description;
      root["diameter"] = current_map.diameter;

      for (auto& tile : current_map.grid)
      {
         Json::Value json_tile;
         Json::Value json_tile_position = coord_to_json_value(tile.first);
         Json::Value json_tile_reference = tile.second.serialize().data();

         json_tile.append(json_tile_position);
         json_tile.append(json_tile_reference);
         root["tiles"].append(json_tile);
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_order(const astd::filesystem::path& path, const std::vector<unit>& units, bool rejected)
{
   if (!units.size())
   {
      return NONE;
   }

   if (std::all_of(units.begin(), units.end(), [rejected](const unit& u)
   {
   }))
   {
	   return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (auto& un : units)
      {
		  Json::Value json_unit_orders;
		  for (auto& acc : un.actions)
		  {
			  Json::Value json_acc;
			  json_acc["action"] = order::serialize(acc.type).data();
			  json_acc["x"] = acc.target.x;
			  json_acc["y"] = acc.target.y;
			  json_unit_orders.append(json_acc);
		  }

         root[un.id.serialize().data()].append(json_unit_orders);
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_player(const astd::filesystem::path& path, const std::vector<player>& players)
{
   if (!players.size())
   {
      return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (auto& player : players)
      {
         Json::Value json_player;
         json_player["name"] = player.name;
         json_player["description"] = player.description;
         json_player["team"] = player.team;
         for (auto& rally : player.rally_point)
         {
            json_player["rally_points"].append(coord_to_json_value(rally));
         }

         root[player.id.serialize().data()].append(json_player);
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

int data_dumper::dump_unit(const astd::filesystem::path& path, const std::vector<unit>& units)
{
   if (!units.size())
   {
      return NONE;
   }

   std::ofstream stream(path.c_str(), std::ios::trunc);

   if (stream)
   {
      Json::Value root;

      for (auto& unit : units)
      {
         Json::Value json_unit;
         json_unit["owner"] = unit.owner.serialize().data();
         json_unit["type"] = unit.type.serialize().data();
         json_unit["position"] = coord_to_json_value(unit.pos);
         json_unit["endurance"] = unit.endurance;

         root[unit.id.serialize().data()].append(json_unit);
      }

      stream << root;
      return NONE;
   }
   return OPEN_FILE;
}

Json::Value data_dumper::coord_to_json_value(const coordinate& coord)
{
   Json::Value result;
   result.append(coord.x);
   result.append(coord.y);
   return result;
}