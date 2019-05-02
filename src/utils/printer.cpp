#include "printer.h"

std::ostream& operator<<(std::ostream& os, const Col& c)
{
	if(!out.colors) return os;
	return os << "\033[1;" << static_cast<int>(c) << 'm';
}

// Helper for iterating channels
static constexpr std::array<Ch, 4>
chs = { Ch::Std, Ch::Info, Ch::Warn, Ch::Error };

void Printer::quiet()
{
	for(auto& c : chs) chData[c].out->rdbuf(nullptr);
}

void Printer::noPrefix()
{
	for(auto& c : chs) chData[c].prefix = "";
}

std::ostream& Printer::operator()(Ch c)
{
	*chData[c].out << chData[c].color;
	*chData[c].out << chData[c].prefix;
	*chData[c].out << Col::reset;

	return *chData[c].out;
}

std::map<Ch, Printer::ChannelData> Printer::chData{
	{Ch::Std,	{&std::cout, "    ", Col::reset}},
	{Ch::Info,	{&std::cout, "[I] ", Col::blue}},
	{Ch::Warn,	{&std::clog, "[W] ", Col::yellow}},
	{Ch::Error,	{&std::cerr, "[E] ", Col::red}}
};

Printer out; // instantiate Printer global
