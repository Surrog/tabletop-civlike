#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <string>
#include <utility>

#include "reference.hpp"

struct coordinate
{
	std::int32_t x = 0;
	std::int32_t y = 0;
	std::int32_t z = 0;
};

// order.json
struct order
{
   enum T_type
   {
	  NONE,
      MOVE,
      FIRE,

	  SIZE
   };

   reference id;
   reference unit_source;
   T_type type;
   coordinate target;

   static const std::array< std::array<char, 5>, SIZE> converter_arr;
   static T_type parse(astd::string_view str);
   static astd::string_view serialize(order::T_type type);	   
};

//map.json
struct map
{
   std::string name;
   std::string description;
   std::int32_t diameter;
   std::vector<std::pair<coordinate, reference>> grid;
};

//def_attack.json & def_defense.json
struct unit_action
{
   reference id;
   std::string name;
   std::string description;
   std::int32_t soft;
   std::int32_t hard;
   std::array<std::int32_t, 2> range;
   std::int32_t cost;
};

//unit.json
struct unit
{
   reference id;
   reference owner;
   reference type;
   coordinate pos;
   std::int32_t endurance;
};

//def_unit.json
struct unit_definition
{
	reference id;
	std::string name;
	std::string description;
	std::vector<reference> attack;
	std::vector<reference> defense;
	std::vector<order::T_type> order_accessible;
	std::int32_t action_point;
	float cover_usage;
	std::string texture;
};

//player.json
struct player
{
   reference id;
   std::string name;
   std::string description;
   std::vector<coordinate> rally_point;
   char team;
};

//def_map.json
struct terrain
{
   reference id;
   std::string name;
   std::string description;
   std::int32_t infrastructure;
   std::int32_t cover;
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
