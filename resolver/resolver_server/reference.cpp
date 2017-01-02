#include "reference.hpp"
#include "boost/lexical_cast.hpp"

const std::array<astd::string_view, reference::SIZE> reference::converter_array =
{
   "NUL", "ORD", "UNI", "DUN", "PLY", "DTI", "ATT", "DEF"
};

std::ostream& operator<<(std::ostream& stream, const reference& ref)
{
   stream << ref.serialize().data();
   return stream;
}

reference::reference(astd::string_view str)
{
   auto ref_type = str.substr(0, PREFIX_SIZE - 1);
   auto num_found = str.substr(PREFIX_SIZE - 1);
   auto it = std::find(converter_array.begin(), converter_array.end(), ref_type);

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

bool reference::operator ==(const reference& rval) const
{
   return type == rval.type && num == rval.num;
}

bool reference::operator!=(const reference& rval) const
{
   return !operator==(rval);
}

bool reference::operator<(const reference& rval) const
{
   return type < rval.type && num < rval.num;
}

std::array<char, reference::PREFIX_SIZE + 5> reference::serialize() const
{
   std::array<char, PREFIX_SIZE + 5> result = { 0 };
   std::array<char, 6> filled_number;
   std::fill(filled_number.begin(), filled_number.end(), '0');
   filled_number[5] = '\0';

   auto number_start = std::copy(converter_array[type].begin(), converter_array[type].begin() + PREFIX_SIZE - 1, result.begin());

   auto number = boost::lexical_cast<std::array<char, 6>>(num);
   std::copy(number.rbegin() + (number.size() - std::strlen(number.data())), number.rend(), filled_number.rbegin() + 1); //let's place the number at the end of the string but not erasing the /0
   std::copy(filled_number.begin(), filled_number.end(), number_start);
   return result;
}