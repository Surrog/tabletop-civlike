
#include <iostream>
#include <array>
#include "afilesystem.hpp"
#include "resolver_config.hpp"
#include "data.hpp"
#include "data_parser.hpp"

int main(int argc, char ** argv)
{
   std::cout << "hello world !" << std::endl;
   std::cout << "this is resolver version " << RESOLVER_VERSION_MAJOR << "." << RESOLVER_VERSION_MINOR << std::endl;
   if (argc > 1)
   {
	   std::cout << "input directory " << argv[1] << std::endl;
	   data_parser parser(argv[1]);
   }
   else
   {
	   std::cout << "no input directory !" << std::endl;
   }
   return 0;
}