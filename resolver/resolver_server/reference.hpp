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
public:
   enum T_type
   {
      NUL, //null type, for invalid reference
      ORD, //order
      UNI, //unit in game
      DUN, //unit definition
      PLY, //player definition
      DTI, //terrain definition
      ATT, //attack definition
      DEF, //defense definition

      SIZE //keep this one at the end
   };

   T_type type = NUL;
   std::size_t num = 0;

   enum
   {
      PREFIX_SIZE = 4
   };

   static const std::array<astd::string_view, SIZE> converter_array;

   reference() = default;

   reference(astd::string_view str);

   bool operator ==(const reference& rval) const;

   bool operator!=(const reference& rval) const;

   bool operator<(const reference& rval) const;

   std::array<char, PREFIX_SIZE + 5> serialize() const;
};

std::ostream& operator<<(std::ostream& stream, const reference& ref);

#endif //!REFERENCE_HPP
