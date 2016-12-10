
#include <iostream>
#include "resolver_config.hpp"

int main(int argc, char ** argv)
{
   std::cout << "hello world !" << std::endl;
   std::cout << "this is resolver version " << RESOLVER_VERSION_MAJOR << "." << RESOLVER_VERSION_MINOR << std::endl;
   return 0;
}