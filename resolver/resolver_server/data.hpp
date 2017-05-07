#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <string>
#include <utility>
#include <limits>
#include <ostream>
#include <map>

#include "reference.hpp"
#include "astring_view.hpp"
#include "static_vector.hpp"
#include "static_hashmap.hpp"

struct coordinate
{
   std::int32_t x = 0;
   std::int32_t y = 0;
   std::int32_t z = 0;
};

static std::ostream& operator<<(std::ostream& stream, const coordinate& coord)
{
	stream << coord.x << ' ' << coord.y << ' ' << coord.z;
	return stream;
}

static bool operator==(const coordinate& lval, const coordinate& rval)
{
	return lval.x == rval.x && lval.y == rval.y && lval.z == rval.z;
}

static bool operator<(const coordinate& lval, const coordinate& rval)
{
	bool result = lval.x < rval.x;
	if (lval.x == rval.x)
		result = lval.y < rval.y;
	if(lval.y == rval.y)
		result = lval.z < rval.z;
	return result;
}


// order.json
struct order
{
   enum T_type {
      NONE,
      MOVE,
      FIRE,
      BUILD,
      SIZE
   };

   T_type type;
   reference modifier;
   coordinate target;

   static const std::array< astd::string_view, SIZE> converter_arr;
   static T_type parse(astd::string_view str);
   static astd::string_view serialize(order::T_type type);
};

//map.json
struct map
{
   std::string name;
   std::string description;
   std::int32_t diameter = 0;
   xts::static_vector<std::pair<coordinate, reference>, 512> grid;
};

//def_attack.json & def_defense.json
struct unit_action
{
   reference id;
   std::string name;
   std::string description;
   std::int32_t soft = 0;
   std::int32_t hard = 0;
   std::array<std::uint32_t, 2> range = { 0 };
   std::int32_t cost = 0;
};

//unit.json
struct unit
{
   reference id;
   reference owner;
   reference type;
   coordinate pos;
   xts::static_vector<order, 32> actions;
   std::int32_t endurance = 0;
   float action_point_remaining = 0.f;
   bool action_invalid = false;

   enum 
   {
	   ENDURANCE_MAX = 100
   };
};

//def_unit.json
struct unit_definition
{
   reference id;
   std::string name;
   std::string description;
   xts::static_vector<reference, 8> attack;
   xts::static_vector<reference, 8> defense;
   xts::static_vector<order::T_type, order::SIZE> order_accessible;
   std::int32_t action_point = 0;
   float cover_usage = 0.f;
   std::string texture;
};

//player.json
struct player
{
   reference id;
   std::string name;
   std::string description;
   xts::static_vector<coordinate, 8> rally_point;
   char team = 0;
};

//def_map.json
struct terrain
{
   reference id;
   std::string name;
   std::string description;
   float infrastructure = 0.f; //let's avoid overflow
   std::int32_t cover = 0;
   std::string texture_path;
};


struct game_data
{
   map current_map;
   std::vector<unit_action> attack_action;
   std::vector<unit_action> defense_action;
   std::vector<unit> units;
   std::vector<unit> unit_dead;
   std::vector<unit_definition> unit_defs;
   std::vector<player> players;
   std::vector<terrain> terrains;
};

#endif //DATA_HPP
