#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <string>
#include <utility>

#include "reference.hpp"

struct coord
{
   int x;
   int y;
   int z;
};

// order.json
struct order
{
   enum T_type
   {
      MOVE,
      FIRE
   };

   reference id;
   reference unit_source;
   T_type type;
   coord target;
};

//map.json
struct map
{
   std::string name;
   std::string description;
   int diameter;
   std::vector<std::pair<coord, reference>> grid;
};

//def_attack.json & def_defense.json
struct unit_action
{
   reference id;
   std::string name;
   std::string description;
   int soft;
   int hard;
   std::pair<int, int> range;
   int cost;
};

//unit.json
struct unit
{
   reference id;
   reference owner;
   reference type;
   coord pos;
   int endurance;
};

//def_unit.json
struct unit_definition
{
	reference id;
	std::string name;
	std::string definition;
	std::vector<reference> attack;
	std::vector<reference> defense;
	std::vector<order::T_type> order_accessible;
	int action_point;
	int cover_usage;
	std::string texture;
};

//player.json
struct player
{
   reference id;
   std::string name;
   std::string description;
   coord rally_point;
   char team;
};

//def_map.json
struct terrain
{
   reference id;
   std::string name;
   std::string description;
   uint32_t infrastructure;
   uint32_t cover;
   std::string texture_path;
};


struct game_data
{
	std::vector<order> orders;
	map current_map;
   std::vector<unit_action> attack_action;
   std::vector<unit_action> defense_action;
	std::vector<unit> units;
	std::vector<unit_definition> unit_defs;
	std::vector<player> players;
	std::vector<terrain> terrains;
};

#endif //DATA_HPP
