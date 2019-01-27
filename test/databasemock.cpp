#include <map>

#include "databasemock.h"

extern std::map<std::string, std::vector<std::string>> dataLink;

std::vector<std::string> DatabaseMock::fetch(const std::string& id) const{
	auto it = dataLink.find(id);
	if(it == dataLink.end()) return {};
	return it->second;
}

std::map<std::string, std::vector<std::string>> dataLink = {
	{"advancedinfiniteammo", {
		"https://forums.alliedmods.net/showpost.php?p=1754217",
		"p:AdvancedInfiniteAmmo.sp",
	}},
	{"afk_manager", {
		"https://forums.alliedmods.net/showpost.php?p=708265",
		"p:http://afkmanager.dawgclan.net/plugins/afk_manager4.smx",
		"t:http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt",
	}},
	{"dynamic_motd", {
		"https://forums.alliedmods.net/showpost.php?p=1387386",
		"p:dynamic_motd.smx",
	}},
	{"funcommandsx", {
		"https://forums.alliedmods.net/showpost.php?p=665771",
		"a:funcommandsX_*.zip",
	}},
	{"tf2attributes", {
"https://github.com/FlaminSarge/tf2attributes/",
"p:tf2attributes.smx",
"g:https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt",
	}},
	{"tf2items", {
		"https://builds.limetech.io/?p=tf2items",
		"a:*",
	}},
	{"thriller", {
		"https://forums.alliedmods.net/showpost.php?p=1590169",
		"p:thriller.sp",
		"g:thriller.plugin.txt",
	}},
	{"updater", {
		"https://bitbucket.org/GoD_Tony/updater/downloads/",
		"p:updater.smx",
	}}
};
