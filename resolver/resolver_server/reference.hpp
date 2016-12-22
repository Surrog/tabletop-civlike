#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include <array>
#include <cstddef>
#include <iterator>
#include <iostream>
#include "boost/lexical_cast.hpp"
#include "astring_view.hpp"
#include "fast_convert.hpp"

struct reference
{
   enum T_type
   {
      NUL, //null type, for invalid reference
      ORD, //order
      UNI, //unit in game
      DUN, //unit definition
      PLY, //player definition
      DTI, //terrain definition

      SIZE //keep this one at the end
   };

   T_type type;
   std::size_t num;

   reference()
      : type(NUL), num(0)
   {}

   reference(const reference& rval) = default;
   reference& operator=(const reference& rval) = default;
   reference(reference&& rval) noexcept = default;
   reference& operator=(reference&& rval) noexcept = default;

   reference(astd::string_view str)
   {
	   auto ref_type = str.substr(0, PREFIX_SIZE);
	   auto num_found = str.substr(PREFIX_SIZE - 1);
	   auto it = std::find_if(converter_array.begin(), converter_array.end(), [&ref_type](auto& arr) {
		   return std::equal(ref_type.begin(), ref_type.end(), arr.begin());
	   });
	   if (it != converter_array.end())
	   {
		   type = static_cast<T_type>(std::distance(converter_array.begin(), it));
	   }
	   else
	   {
		   std::cerr << "ERROR: reference type " << ref_type << " not known" << std::endl;
		   type = NUL;
	   }

	   num = xts::fast_str_to_uint(num_found);
   }

   bool operator ==(const reference& rval)
   {
      return type == rval.type && num == rval.num;
   }

   bool operator!=(const reference& rval)
   {
      return !operator==(rval);
   }

   bool operator<(const reference& rval)
   {
	   return type < rval.type && num < rval.num;
   }

private:
   enum
   {
      PREFIX_SIZE = 4
   };
   const std::array<std::array<const char, PREFIX_SIZE>, SIZE> converter_array =
   {
      "NUL", "ORD", "UNI", "DUN", "PLY", "DTI"
   };
};

#endif //!REFERENCE_HPP
