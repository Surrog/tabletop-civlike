#ifndef GAME_RESOLVER_HPP
#define GAME_RESOLVER_HPP

#include <array>
#include "data.hpp"

class game_resolver
{
public:
   game_resolver(const game_data& game);

   const game_data& data() const;
   game_data&& get();

   int status() const;

   void resolve();
   int execute_order(const order& order);
   int execute_none(const order& order);
   int execute_move(const order& order);
   int execute_fire(const order& order);
   int execute_build(const order& order);

   void resolving_cac();
   void bring_out_the_dead();

private:
   std::vector<order> _order_rejected;
   std::vector<unit> _dead_units;
   game_data _result;
   int _status = 0;

   static const std::array<int (game_resolver::*)(const order& order), order::SIZE> order_state_machine;
};

#endif //!GAME_RESOLVER_HPP
