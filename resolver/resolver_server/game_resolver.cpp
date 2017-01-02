#include "game_resolver.hpp"

const std::array<int (game_resolver::*)(const order& order), order::SIZE> game_resolver::order_state_machine
= 
{
   &game_resolver::execute_none,
   &game_resolver::execute_move,
   &game_resolver::execute_fire,
   &game_resolver::execute_build,
};

game_resolver::game_resolver(const game_data& game)
   : _result(game)
{
   resolve();
}

const game_data& game_resolver::data() const
{
   return _result;
}

game_data&& game_resolver::get()
{
   return std::move(_result);
}

int game_resolver::status() const
{
   return _status;
}

void game_resolver::resolve()
{
   std::sort(_result.orders.begin(), _result.orders.end(), [](const auto& lval, const auto& rval)
   {
      return lval.type > rval.type;
   });

   while (_result.orders.size() && _result.orders.back().type != order::BUILD)
   {
      if (execute_order(_result.orders.back()) != 0)
      {
         _order_rejected.emplace_back(std::move(_result.orders.back()));
      }
      _result.orders.pop_back();
   }

   bring_out_the_dead();
   resolving_cac();
   bring_out_the_dead();

   while (_result.orders.size())
   {
      if (execute_order(_result.orders.back()) != 0)
      {
         _order_rejected.emplace_back(std::move(_result.orders.back()));
      }
      _result.orders.pop_back();
   }

   _result.orders = _order_rejected;
}

int game_resolver::execute_order(const order& order)
{
   return (this->*order_state_machine[order.type])(order);
}

int game_resolver::execute_none(const order& order)
{
   std::cerr << "WARNING : trying to execute none order - ref:" << order.id << std::endl;
   return 0;
}

int game_resolver::execute_move(const order& order)
{
   std::cerr << "WARNING : MOVE action not yet implemented - ref " << order.id << std::endl;
   return 0;
}

int game_resolver::execute_fire(const order& order)
{
   std::cerr << "WARNING : FIRE action not yet implemented - ref " << order.id << std::endl;
   return 0;
}

int game_resolver::execute_build(const order& order)
{
   std::cerr << "WARNING : BUILD action not yet implemented - ref " << order.id << std::endl;
   return 0;
}

void game_resolver::resolving_cac()
{

}

void game_resolver::bring_out_the_dead()
{
   for (const auto& unit : _result.units)
   {
      if (unit.endurance == 0)
      {
         auto it_new_end = std::remove_if(_result.orders.begin(), _result.orders.end(), [&unit](const auto& order)
         {
            return order.unit_source == unit.id;
         });
         _result.orders.resize(std::distance(_result.orders.begin(), it_new_end));
         _result.unit_dead.push_back(unit);
      }
   }

   auto it_new_end = std::remove_if(_result.units.begin(), _result.units.end(), [](const auto& unit) {return unit.endurance == 0; });
   _result.units.resize(std::distance(_result.units.begin(), it_new_end));
}