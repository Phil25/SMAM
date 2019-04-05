#include <iostream>

#include "opts.hpp"

void install(const Opts&)
{
}

void remove(const Opts&)
{
}

void info(const Opts&)
{
}

void search(const Opts&)
{
}

int main(int argc, const char* argv[])
{
	const Opts opts(argc, argv);

	if(opts.help())
	{
		std::cout << opts.getDescription() << '\n';
	}
	else
	{
		const auto& command = opts.getCommand();

		try
		{
			std::map<std::string_view, void (*)(const Opts&)>
			{
				{"install", install},
				{"remove", remove},
				{"uninstall", remove},
				{"info", info},
				{"search", search},
			}
			.at(command)(opts);
		}
		catch(const std::out_of_range& e)
		{
			std::cerr << "error: unknown command \"" << command << "\"\n";
			return 1;
		}
	}

	return 0;
}
