
#include <iostream>
#include <array>
#include "afilesystem.hpp"
#include "resolver_config.hpp"
#include "data.hpp"
#include "data_parser.hpp"
#include "data_dumper.hpp"
#include "game_resolver.hpp"

int main(int argc, char ** argv)
{
   std::cout << "hello world !" << std::endl;
   std::cout << "this is resolver version " << RESOLVER_VERSION_MAJOR << "." << RESOLVER_VERSION_MINOR << std::endl;
      
   if (argc > 1)
   {
	   std::cout << "input directory " << argv[1] << std::endl;
      astd::filesystem::path output_path = argv[1];
      output_path = output_path / "output_dir";
	   data_parser parser(argv[1]);

      game_resolver resolver(parser.data());
      if (resolver.status() == 0)
      {
         data_dumper dump(resolver.data(), output_path);
      }
   }
   else
   {
	   std::cout << "no input directory !" << std::endl;
   }
   return 0;
}