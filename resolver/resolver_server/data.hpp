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

struct order
{
   enum T_type
   {
      MOVE,
      FIRE
   };

   reference ref;
   T_type type;
   coord target;
};

//map.json
struct map
{
   std::string name;
   std::string description;
   int width;
   int length;
   std::vector<std::pair<coord, std::string>> grid;
};

//def_unit.json
struct unit_action
{
   int soft;
   int hard;
   int range;
   int cost;
};

//def_unit.json
struct unit
{
   reference ref;
   std::string name;
   std::string description;
   std::vector<unit_action> attacks;
   std::vector<unit_action> defenses;
   int health;
   int action_point;
   int mobility;
   std::string texture;
};

//def_player.json
struct player
{
   reference ref;
   std::string name;
};

//def_map.json
struct terrain
{
   reference ref;
   std::string name;
   std::string description;
   uint32_t infrastructure;
   uint32_t cover;
   std::string texture_path;
};

#endif //DATA_HPP
