#pragma once

#include <map>

constexpr char cr = '\n';

enum class Ch { Std, Info, Warn, Error }; // channel

enum class Col {
	reset	= 0,
	red		= 31,
	green	= 32,
	yellow	= 33,
	blue	= 34,
};

extern std::ostream& operator<<(std::ostream& os, const Col& c);

class Printer
{
	struct ChannelData
	{
		std::ostream* out;
		std::string_view prefix;
		Col color;
	};

	static std::map<Ch, ChannelData> chData;

public:
	bool colors = true;

	void quiet(); // disable output entirely
	void noPrefix(); // disable prefixes such as [ERROR], etc

	// Change channel
	std::ostream& operator()(Ch c=Ch::Std);
};

extern Printer out; // global console output
