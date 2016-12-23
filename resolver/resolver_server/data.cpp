#include "data.hpp"

#include <iterator>

const std::array< std::array<char, 5>, order::SIZE> order::converter_arr
= {
	"NONE"
	, "MOVE"
	, "FIRE"
};

order::T_type order::parse(astd::string_view str)
{
	auto it = std::find_if(converter_arr.begin(), converter_arr.end(), [&str](auto& arr) {
		return std::equal(arr.begin(), arr.end(), str.begin());
	});
	if (it != converter_arr.end())
	{
		return static_cast<order::T_type>(std::distance(converter_arr.begin(), it));
	}
	std::cerr << "WARNING " << str << " is not a reconized order" << std::endl;
	return order::NONE;
}

astd::string_view order::serialize(order::T_type type)
{
	if (std::size_t(type) < converter_arr.size())
	{
		return astd::string_view(converter_arr[type].data());
	}
	else
	{
		std::cerr << "WARNING order::" << type << " is not correctly serialized" << std::endl;
		return astd::string_view(converter_arr[0].data());
	}
}
