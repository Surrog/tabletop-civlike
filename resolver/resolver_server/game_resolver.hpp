#ifndef GAME_RESOLVER_HPP
#define GAME_RESOLVER_HPP

#include <array>
#include "data.hpp"
#include "boost/container/flat_map.hpp"
#include "boost/container/static_vector.hpp"


class game_resolver
{
public:
	enum error_code
	{
		NONE = 0,
		ORDER_REFUSED = 1,
		REF_MISSING = 2,
		FATAL_ERROR = 4
	};

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

	int close_combat_action();
	void bring_out_the_dead();
private:
	std::vector<order> _order_rejected;
	std::vector<unit> _dead_units;
	game_data _data;
	int _status = 0;

	struct pair_float_coord_compare
	{
		template <typename T>
		bool operator()(const T& lval, const T& rval)
		{
			return lval.first > rval.first;
		}
	};

	int try_attack(unit& attacker_unit, const coordinate& target, bool friendly_fire = true);

	static const std::array<int (game_resolver::*)(const order& order), order::SIZE> order_state_machine;
	static const terrain bad_terrain_value;
	static const coordinate bad_coordinate_value;
	static const unit_definition bad_unit_def_value;
	static const unit_action bad_unit_action;
	static const player bad_player;

	static unit bad_unit_value;

	std::pair<float, std::vector<coordinate>> find_path_linear(const unit& pos, const coordinate& destination) const;

	static std::vector<coordinate> construct_path(const boost::container::flat_map<coordinate, coordinate>& directions, const coordinate& target);
	template<typename T>
	coordinate flood_search_first(const coordinate& pos, const T& func, std::size_t distance_max = 0) const
	{
		std::set<coordinate> visited;
		std::deque<coordinate> opened;
		opened.push_back(pos);
		visited.insert(pos);
		std::size_t i = 1;

		while (opened.size() && i != distance_max)
		{
			for (auto& neighbor : neighbors(opened.front()))
			{
				if (func(neighbor))
					return neighbor;

				if (visited.find(neighbor) != visited.end())
				{
					opened.push_back(neighbor);
					visited.insert(neighbor);
				}
			}
			opened.pop_front();
		}
		return bad_coordinate_value;
	}

	template<typename T>
	std::vector<coordinate> flood_search_all(const coordinate& pos, const T& func, std::size_t distance_max = 0) const
	{
		std::set<coordinate> visited;
		std::deque<coordinate> opened;
		std::vector<coordinate> result;
		opened.push_back(pos);
		visited.insert(pos);
		std::size_t i = 1;

		while (opened.size() && i != distance_max)
		{
			for (auto& neighbor : neighbors(opened.front()))
			{
				if (visited.find(neighbor) != visited.end())
				{
					opened.push_back(neighbor);
					visited.insert(neighbor);
					if (func(neighbor))
					{
						result.push_back(neighbor);
					}
				}
			}
			opened.pop_front();
		}

		return result;
	}

	boost::container::static_vector<coordinate, 6> neighbors(const coordinate& coord) const;
	std::uint32_t distance(const coordinate& origin, const coordinate& target) const;
	std::vector<coordinate> line(const coordinate& origin, const coordinate& target) const;

	float get_movement_cost(const coordinate& coord, const player& pla) const;
	float get_movement_cost(const coordinate& coord) const;
	float get_movement_cost(const std::vector<coordinate>& coords) const;
	const terrain& get_terrain(const coordinate& coord) const;
	const terrain& get_terrain(const reference& ref) const;
	const unit_definition& get_unit_def(const reference& ref) const;
	const unit_action& get_attack(const reference& ref) const;
	const unit_action& get_defense(const reference& ref) const;
	const player& get_player(const reference& ref) const;
	unit& get_unit(const reference& ref);
	std::vector<std::reference_wrapper<unit>> get_units(const coordinate& ref);
	std::vector<std::reference_wrapper<const unit>> get_units(const coordinate& ref) const;
	bool has_unit(const coordinate& coord) const;
	unit_action calculate_unit_defense(const unit_definition& unit_def) const;
};

#endif //!GAME_RESOLVER_HPP
