#include <iostream>

#include "../thirdparty/cxxopts.hpp"
#include "argparser.hpp"

int main(int argc, char* argv[]){
	if(argc < 2){
		std::cout << "smam: No operation specified." << std::endl;
		return 1;
	}

	argparser::Opts opts(argc, argv);
	argparser::Operation op = opts.getOp();
	if(!op){
		std::cout << "smam: Invalid operation: " << argv[1] << std::endl;
		return 1;
	}

	std::cout << "Operation " << opts.getOp() << " on addons:" << std::endl;

	std::vector<std::string> addons = opts.getAddons();
	for(const auto& addon : addons)
		std::cout << "* " << addon << std::endl;

	return 0;
}
