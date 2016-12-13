#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include <array>
#include <cstddef>

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

   bool operator ==(const reference& rval)
   {
      return type == rval.type && num == rval.num;
   }

   bool operator!=(const reference& rval)
   {
      return !operator==(rval);
   }

private:
   enum
   {
      PREFIX_SIZE = 4
   };
   static constexpr std::array<std::array<const char, PREFIX_SIZE>, SIZE> converter_array =
   {
      "NUL", "ORD", "UNI", "DUN", "PLY", "DTI"
   };
};

#endif //!REFERENCE_HPP
