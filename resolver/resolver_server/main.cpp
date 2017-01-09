
#include <iostream>
#include <array>
#include "afilesystem.hpp"
#include "resolver_config.hpp"
#include "data.hpp"
#include "data_parser.hpp"
#include "data_dumper.hpp"
#include "game_resolver.hpp"
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>


int main(int argc, char ** argv)
{
	std::cout << "This is resolver version " << RESOLVER_VERSION_MAJOR << "." << RESOLVER_VERSION_MINOR << std::endl;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()("help", "produce this help message")
		("o", boost::program_options::value<astd::filesystem::path>(), "<PATH> output directory")
		("i", boost::program_options::value<astd::filesystem::path>(), "<PATH> input directory");

	boost::program_options::positional_options_description p;
	p.add("i", -1);

	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
			.options(desc)
			.positional(p)
			.style(boost::program_options::command_line_style::unix_style |
				boost::program_options::command_line_style::allow_long_disguise)
			.allow_unregistered()
			.run()
			, vm);

		boost::program_options::notify(vm);
	}
	catch (boost::program_options::error e)
	{
		std::cerr << "ERROR : " << e.what() << std::endl;
		std::cout << "resolver_server [--help] [-o <output_path>] [-i] input_path" << std::endl;
		std::cout << desc << std::endl;
		return 1;
	}

	if (vm.find("help") != vm.end())
	{
		std::cout << "resolver_server [--help] [-o <output_path>] [-i] input_path" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	auto it_input = vm.find("i");
	if (it_input == vm.end())
	{
		std::cerr << "ERROR : No input directory !" << std::endl;
		std::cout << "resolver_server [--help] [-o <output_path>] [-i] input_path" << std::endl;
		std::cout << desc << std::endl;
		return 1;
	}

	astd::filesystem::path input_path = it_input->second.as<astd::filesystem::path>();
	if (!astd::filesystem::exists(input_path)
		|| !astd::filesystem::is_directory(input_path))
	{
		std::cerr << "ERROR : Input path doesn't exist or isn't a directory" << std::endl;
		return 1;
	}

	auto it_output = vm.find("o");
	astd::filesystem::path output_path;
	if (it_output == vm.end())
	{
		output_path = input_path / "output_dir";
	}
	else
	{
		output_path = it_output->second.as<astd::filesystem::path>();
		if (astd::filesystem::exists(output_path)
			&& !astd::filesystem::is_directory(output_path))
		{
			std::cerr << "ERROR : " << output_path << " is not a directory" << std::endl;
			return 1;
		}
	}

	data_parser parser(input_path);

	game_resolver resolver(parser.data());
	if (resolver.status() == 0)
	{
		data_dumper dump(resolver.data(), output_path);
	}
	return 0;
}