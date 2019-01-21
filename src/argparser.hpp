#pragma once

#include <iostream>
#include <vector>

namespace argparser{

enum Operation{
	Operation_Invalid = 0,
	Operation_Help,
	Operation_Version,
	Operation_Install,
	Operation_Remove,
	Operation_Search,
	Operation_Files
};

class Opts{
	const Operation op;

	std::string path = ".";
	std::vector<std::string> addons{};

public:
	Opts(int argc, char* argv[]) :
		op(parseOp(argv[1][0]))
	{
		if(!op) return;
		for(int i = 2; i < argc; ++i)
			addons.push_back(argv[i]);
	}

	Operation getOp() const{
		return op;
	}

	const std::vector<std::string>& getAddons() const{
		return addons;
	}

private:
	static Operation parseOp(char op){
		switch(op){
			case 'h': return Operation_Help;
			case 'v': return Operation_Version;
			case 'i': return Operation_Install;
			case 'r': return Operation_Remove;
			case 's': return Operation_Search;
			case 'f': return Operation_Files;
		}
		return Operation_Invalid;
	}
};

}
