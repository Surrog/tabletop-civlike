#include "game_resolver.hpp"

#include <set>
#include <deque>
#include <vector>
#include <numeric>
#include <queue>
#include <algorithm>
#include <limits>
#include <functional>
#include <cassert>
#include "boost/container/flat_map.hpp"

const std::array<int (game_resolver::*)(const order& order), order::SIZE> game_resolver::order_state_machine
=
{
   &game_resolver::execute_none,
   &game_resolver::execute_move,
   &game_resolver::execute_fire,
   &game_resolver::execute_build,
};

const terrain game_resolver::bad_terrain_value;
const coordinate game_resolver::bad_coordinate_value
{ std::numeric_limits<int>::min()
, std::numeric_limits<int>::min()
, std::numeric_limits<int>::min() };

const unit_definition game_resolver::bad_unit_def_value;
const unit_action game_resolver::bad_unit_action;
unit game_resolver::bad_unit_value;
const player game_resolver::bad_player;


game_resolver::game_resolver(const game_data& game)
	: _data(game)
{
	resolve();
}

const game_data& game_resolver::data() const
{
	return _data;
}

game_data&& game_resolver::get()
{
	return std::move(_data);
}

int game_resolver::status() const
{
	return _status;
}

void game_resolver::resolve()
{
	std::sort(_data.orders.begin(), _data.orders.end(), [](const auto& lval, const auto& rval)
	{
		return lval.type > rval.type;
	});

	for (auto& unit : _data.units)
	{
		unit.action_point_remaining = get_unit_def(unit.type).action_point;
	}

	while (_data.orders.size() && _data.orders.back().type != order::BUILD)
	{
		if (execute_order(_data.orders.back()) != 0)
		{
			_order_rejected.emplace_back(std::move(_data.orders.back()));
		}
		_data.orders.pop_back();
	}

	bring_out_the_dead();
	close_combat_action();
	bring_out_the_dead();


	while (_data.orders.size())
	{
		if (execute_order(_data.orders.back()) != 0)
		{
			_order_rejected.emplace_back(std::move(_data.orders.back()));
		}
		_data.orders.pop_back();
	}

	bring_out_the_dead();

	_data.orders = _order_rejected;
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
	auto& unit = get_unit(order.unit_source);
	int result = NONE;
	if (unit.action_point_remaining > 0)
	{
		auto path = find_path_linear(unit, order.target);
		if (path.first < unit.action_point_remaining &&
			path.second.size())
		{
			unit.pos = order.target;
			unit.action_point_remaining -= static_cast<std::uint32_t>(std::ceil(path.first));
		}
		else
		{
			_order_rejected.push_back(order);
			result = ORDER_REFUSED;
		}
	}
	else
	{
		_order_rejected.push_back(order);
		result = ORDER_REFUSED;
	}

	return result;
}

unit_action game_resolver::calculate_unit_defense(const unit_definition& unit_def) const
{
	unit_action result;

	for (const auto& ref : unit_def.defense)
	{
		const auto& def = get_defense(ref);
		result.hard += def.hard;
		result.soft += def.soft;
	}
	return result;
}

int game_resolver::execute_fire(const order& order)
{
	return try_attack(get_unit(order.unit_source), order.target, true);
}

int game_resolver::try_attack(unit& attacker_unit, const coordinate& target, bool friendly_fire)
{
	int result = 0;

	if (attacker_unit.action_point_remaining > 0)
	{
		if (attacker_unit.endurance < 20)
			return NONE;

		auto& attacker_def = get_unit_def(attacker_unit.type);
		auto dis = distance(attacker_unit.pos, target);
		auto att_it = find_if(attacker_def.attack.begin(), attacker_def.attack.end(), [&dis, acc = attacker_unit.action_point_remaining, this](const auto& ref) {
			auto& att = get_attack(ref);
			bool point_ok = att.cost > 0 ? att.cost < acc : acc > 0;
			return point_ok &&
				att.range[0] <= dis && att.range[1] >= dis;
		});

		if (att_it != attacker_def.attack.end())
		{
			const auto& att = get_attack(*att_it);
			const auto& terrain = get_terrain(target);
			auto attacking_team = get_player(attacker_unit.owner).team;

			for (auto& targeted_unit : get_units(target))
			{
				if (friendly_fire || (get_player(targeted_unit.get().owner).team != attacking_team))
				{
					const auto& target_def = get_unit_def(targeted_unit.get().type);
					auto unit_targeted_def = calculate_unit_defense(target_def);
					float floating_damage = std::max((att.soft - (unit_targeted_def.soft * terrain.cover * target_def.cover_usage))
						+ (att.hard - (unit_targeted_def.hard * terrain.cover * target_def.cover_usage)), 0.f);
					if (attacker_unit.endurance < 80)
						floating_damage = floating_damage * attacker_unit.endurance / 100;
					targeted_unit.get().endurance -= static_cast<std::uint32_t>(std::floor(floating_damage));
				}
			}

			if (att.cost >= 0)
				attacker_unit.action_point_remaining -= att.cost;
			else
				attacker_unit.action_point_remaining = 0;

			return NONE;
		}
	}
	result = ORDER_REFUSED;
	return result;
}

int game_resolver::execute_build(const order& order)
{
	std::cerr << "WARNING : BUILD action not yet implemented - ref " << order.id << std::endl;
	return 0;
}

int game_resolver::close_combat_action()
{
	boost::container::flat_map<coordinate, std::vector<std::reference_wrapper<unit>>> unit_per_case;
	unit_per_case.reserve(_data.units.size() / 2);

	for (auto& unit : _data.units)
	{
		unit_per_case[unit.pos].push_back(unit);
	}

	for (auto& pair : unit_per_case)
	{
		if (pair.second.size() > 1)
		{
			std::set<char> teams;

			for (auto& unit : pair.second)
			{
				teams.insert(get_player(unit.get().owner).team);
			}
			if (teams.size() > 1)
			{
				for (auto& unit : pair.second)
				{
					try_attack(unit.get(), pair.first, false);
				}
			}

			std::sort(pair.second.begin(), pair.second.end(), [](const auto& lval, const auto& rval)
			{
				bool result = lval.get().endurance > rval.get().endurance;
				if (lval.get().endurance == rval.get().endurance)
					result = lval.get().action_point_remaining > rval.get().action_point_remaining;
				return result;
			});

			auto neigh = neighbors(pair.first);
			while (pair.second.size() > 1)
			{
				auto& pla = get_player(pair.second.back().get().owner);
				std::sort(neigh.begin(), neigh.end(), [&pla, this](const auto& lval, const auto& rval)
				{
					return distance(lval, pla.rally_point[0]) < distance(rval, pla.rally_point[0]);
				});

				auto it = std::find_if_not(neigh.begin(), neigh.end(), [this](const auto& val) { return has_unit(val); });
				if (it != neigh.end())
					pair.second.back().get().pos = *it;
				else
					pair.second.back().get().endurance = 0;
				pair.second.pop_back();
			}
		}
	}

	return NONE;
}

void game_resolver::bring_out_the_dead()
{
	std::sort(_data.units.begin(), _data.units.end(), [](const auto& lval, const auto& rval) {return lval.endurance > rval.endurance; });
	auto unit_to_delete_it = std::find_if(_data.units.begin(), _data.units.end(), [](const auto& unit) {return unit.endurance <= 0; });
	auto order_to_delete = std::remove_if(_data.orders.begin(), _data.orders.end(), [unit_to_delete_it, unit_end = _data.units.end()](const auto& order)
	{
		return std::find_if(unit_to_delete_it, unit_end, [order](const auto& dead_unit)
		{ return order.unit_source == dead_unit.id; }) != unit_end;
	});
	_data.orders.erase(order_to_delete, _data.orders.end());
	std::for_each(unit_to_delete_it, _data.units.end(), [this](auto&& unit_dead){ _data.unit_dead.emplace_back(unit_dead); });
	_data.units.erase(unit_to_delete_it, _data.units.end());
}

boost::container::static_vector<coordinate, 6> game_resolver::neighbors(const coordinate& coord) const
{
	boost::container::static_vector<coordinate, 6> result{
		coordinate{ coord.x + 1	, coord.y - 1, coord.z },
			coordinate{ coord.x + 1	, coord.y	, coord.z - 1 },
			coordinate{ coord.x		, coord.y + 1, coord.z - 1 },
			coordinate{ coord.x - 1	, coord.y + 1, coord.z },
			coordinate{ coord.x - 1	, coord.y	, coord.z + 1 },
			coordinate{ coord.x		, coord.y + 1, coord.z - 1 }
	};

	auto to_erase = std::remove_if(result.begin(), result.end(), [this](const auto& coord)
	{
		return std::max({ std::abs(coord.x), std::abs(coord.y), std::abs(coord.z) }) > _data.current_map.diameter
			|| get_terrain(coord).infrastructure == 0.f;
	});
	result.erase(to_erase, result.end());

	return result;
}

std::pair<float, std::vector<coordinate>> game_resolver::find_path_linear(const unit& uni, const coordinate& target) const
{
	boost::container::flat_map<coordinate, pair_float_coordinate> visited;
	std::priority_queue<std::pair<float, coordinate>, std::vector<std::pair<float, coordinate>>, pair_float_coord_compare> opened;

	opened.push(std::make_pair(0.f, uni.pos));
	visited[uni.pos] = { 0.f, {} };
	bool found = false;

	while (opened.size() && !found)
	{
		for (auto& neighbor : neighbors(opened.top().second))
		{
			float neighbor_total_cost = opened.top().first
				+ get_movement_cost(neighbor, get_player(uni.owner))
				+ static_cast<float>(distance(neighbor, target)) / 2
				;

			auto& visited_neighbor_value = visited[neighbor];
			if (neighbor_total_cost < visited_neighbor_value.cost)
			{
				visited_neighbor_value.cost = neighbor_total_cost;
				visited_neighbor_value.coordinate_from = opened.top().second;

				opened.push({ neighbor_total_cost, neighbor });
				if (neighbor == target)
					found = true;
			}
		}
		opened.pop();
	}

	std::pair<float, std::vector<coordinate>> result;
	if (found)
	{
		result.second = construct_path(visited, target);
		result.first = get_movement_cost(result.second);
	}
	return result;
}

std::uint32_t game_resolver::distance(const coordinate& origin, const coordinate& target) const
{
	return std::max({ std::abs(origin.x - target.x), std::abs(origin.y - target.y), std::abs(origin.z - target.z) });
}

float game_resolver::get_movement_cost(const coordinate& coord, const player& pla) const
{
	auto base_cost = get_movement_cost(coord);
	auto units = get_units(coord);
	if (std::any_of(units.begin(), units.end(), [&pla, this](const auto& unit)
	{
		return get_player(unit.get().owner).team != pla.team;
	}))
	{
		base_cost += std::numeric_limits<float>::max() / 3.f;
	}
	return base_cost;
}

float game_resolver::get_movement_cost(const coordinate& coord) const
{
	auto infra = get_terrain(coord).infrastructure;
	if (infra == 0.f)
		return std::numeric_limits<float>::max() / 3.f;
	return 1.f / infra;
}

float game_resolver::get_movement_cost(const std::vector<coordinate>& coords) const
{
	float result = 0.f;
	for (const auto& coord : coords)
	{
		result += get_movement_cost(coord);
	}
	return result;
}


const terrain& game_resolver::get_terrain(const coordinate& coord) const
{
	auto find_it = std::find_if(_data.current_map.grid.cbegin(), _data.current_map.grid.cend(), [&coord](const auto& pair)
	{
		return pair.first == coord;
	});
	if (find_it != _data.current_map.grid.cend())
	{
		return get_terrain(find_it->second);
	}
	return bad_terrain_value;
}

const terrain& game_resolver::get_terrain(const reference& ref) const
{
	assert(ref.type == reference::DTI);
	auto find_it = std::find_if(_data.terrains.cbegin(), _data.terrains.cend(),
		[&ref](const auto& terrain)
	{
		return terrain.id == ref;
	});

	if (find_it != _data.terrains.cend())
	{
		return *find_it;
	}
	std::cerr << "WARNING : failing to find terrain " << ref << std::endl;
	return bad_terrain_value;
}

std::vector<coordinate> game_resolver::line(const coordinate& origin, const coordinate& target) const
{
	auto size = distance(origin, target);
	std::vector<coordinate> result;
	result.reserve(size);

	for (std::uint32_t i = 0; i < size; i++)
	{
		double offset = 1.0 / (size * i);
		result.emplace_back(
			coordinate{
				static_cast<std::int32_t>(std::round(static_cast<double>(origin.x + (target.x - origin.x)) * offset)),
				static_cast<std::int32_t>(std::round(static_cast<double>(origin.y + (target.y - origin.y)) * offset)),
				static_cast<std::int32_t>(std::round(static_cast<double>(origin.z + (target.z - origin.z)) * offset))
		}
		);
	}
	return result;
}


const unit_definition& game_resolver::get_unit_def(const reference& ref) const
{
	assert(ref.type == reference::DUN);
	auto find_it = std::find_if(_data.unit_defs.begin(), _data.unit_defs.end(),
		[&ref](const auto& def)
	{
		return def.id == ref;
	});

	if (find_it != _data.unit_defs.end())
	{
		return *find_it;
	}

	std::cerr << "WARNING : bad unit def - ref " << ref << std::endl;
	return bad_unit_def_value;
}

std::vector<coordinate> game_resolver::construct_path(const boost::container::flat_map<coordinate, pair_float_coordinate>& directions, const coordinate& target)
{
	std::vector<coordinate> result;
	result.reserve(6);

	auto end = directions.end();
	auto start_pos = directions.find(target);
	while (start_pos != end)
	{
		result.push_back(start_pos->first);
		start_pos = directions.find(start_pos->second.coordinate_from);
	}

	return result;
}

unit& game_resolver::get_unit(const reference& ref)
{
	assert(ref.type == reference::UNI);

	auto it = std::find_if(_data.units.begin(), _data.units.end(),
		[&ref](const auto& unit) {
		return unit.id == ref;
	});
	if (it != _data.units.end())
		return *it;

	std::cerr << "WARNING : bad unit ref - " << ref << std::endl;
	return bad_unit_value;
}

std::vector<std::reference_wrapper<unit>> game_resolver::get_units(const coordinate& ref)
{
	std::vector<std::reference_wrapper<unit>> result;

	for (auto& unit : _data.units)
	{
		if (unit.pos == ref)
		{
			result.push_back(unit);
		}
	}

	return result;
}

std::vector<std::reference_wrapper<const unit>> game_resolver::get_units(const coordinate& ref) const
{
	std::vector<std::reference_wrapper<const unit>> result;

	for (auto& unit : _data.units)
	{
		if (unit.pos == ref)
		{
			result.push_back(unit);
		}
	}

	return result;
}

bool game_resolver::has_unit(const coordinate & coord) const
{
	return std::any_of(_data.units.begin(), _data.units.end(), [&coord](const auto& unit)
	{
		return unit.pos == coord;
	});
}


const unit_action& game_resolver::get_attack(const reference& ref) const
{
	assert(ref.type == reference::ATT);

	auto it = std::find_if(_data.attack_action.begin(), _data.attack_action.end(), [&ref](const auto& val) {return ref == val.id; });
	if (it != _data.attack_action.end())
		return *it;

	std::cerr << "WARNING : No attack with ref " << ref << std::endl;
	return bad_unit_action;
}

const unit_action& game_resolver::get_defense(const reference& ref) const
{
	assert(ref.type == reference::DEF);

	auto it = std::find_if(_data.defense_action.begin(), _data.defense_action.end(), [&ref](const auto& val) {return ref == val.id; });
	if (it != _data.defense_action.end())
		return *it;
	std::cerr << "WARNING : No defense with ref " << ref << std::endl;
	return bad_unit_action;
}

const player & game_resolver::get_player(const reference & ref) const
{
	assert(ref.type == reference::PLY);

	auto it = std::find_if(_data.players.begin(), _data.players.end(), [&ref](const auto& pla)
	{
		return pla.id == ref;
	});
	if (it != _data.players.end())
	{
		return *it;
	}
	std::cerr << "WARNING : bad player ref " << ref << std::endl;
	return bad_player;
}
