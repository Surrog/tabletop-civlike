#include "reference.hpp"

const std::array<std::array<const char, reference::PREFIX_SIZE>, reference::SIZE> reference::converter_array =
{
   "NUL", "ORD", "UNI", "DUN", "PLY", "DTI", "ATT", "DEF"
};

std::ostream& operator<<(std::ostream& stream, const reference& ref)
{
   stream << reference::converter_array[ref.type].data();
   auto old_width = stream.width(5);
   auto old_fill = stream.fill('0');
   stream << ref.num;
   stream.width(old_width);
   stream.fill(old_fill);
   return stream;
}

reference::reference(astd::string_view str)
{
   auto ref_type = str.substr(0, PREFIX_SIZE - 1);
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

bool reference::operator ==(const reference& rval)
{
   return type == rval.type && num == rval.num;
}

bool reference::operator!=(const reference& rval)
{
   return !operator==(rval);
}

bool reference::operator<(const reference& rval)
{
   return type < rval.type && num < rval.num;
}


//reference::reference()
//   : type(NUL), num(0)
//{}