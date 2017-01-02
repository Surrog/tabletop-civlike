#include "data.hpp"

#include <iterator>

const std::array< astd::string_view, order::SIZE> order::converter_arr
= {
	"NONE"
	, "MOVE"
	, "FIRE"
   , "BUILD"
};

order::T_type order::parse(astd::string_view str)
{
	auto it = std::find(converter_arr.begin(), converter_arr.end(), str);
	
	if (it != converter_arr.end())
	{
		return static_cast<order::T_type>(std::distance(converter_arr.begin(), it));
	}
	std::cerr << "WARNING : " << str << " is not a reconized order" << std::endl;
	return order::NONE;
}

astd::string_view order::serialize(order::T_type type)
{
	if (std::size_t(type) < converter_arr.size())
	{
      return converter_arr[type];
	}
	else
	{
		std::cerr << "WARNING : order::" << type << " is not correctly serialized" << std::endl;
		return converter_arr[0];
	}
}
