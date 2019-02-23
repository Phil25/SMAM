#include <map>

#include "databasemock.h"

using PlanMapper = std::map<std::string, Plan>;
extern PlanMapper dataLink;

Plan DatabaseMock::fetch(const std::string& id) const
{
	auto it = dataLink.find(id);
	if(it == dataLink.end()) return {};
	return it->second;
}

PlanMapper dataLink = {
	{
		"accelerator",
		{
			"https://builds.limetech.io/?p=accelerator",
			{
				File("a:.*"),
			}
		}
	},
	{
		"advancedinfiniteammo",
		{
			"https://forums.alliedmods.net/showpost.php?p=1754217",
			{
				File("p:AdvancedInfiniteAmmo.smx"),
			}
		}
	},
	{
		"afk_manager",
		{
			"https://forums.alliedmods.net/showpost.php?p=708265",
			{
				File("p:http://afkmanager.dawgclan.net/plugins/afk_manager4.smx"),
				File("t:http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt"),
			}
		}
	},
	{
		"connect",
		{
			"https://builds.limetech.io/?p=connect",
			{
				File("a:.*"),
			}
		}
	},
	{
		"dynamic_motd",
		{
			"https://forums.alliedmods.net/showpost.php?p=1387386",
			{
				File("p:dynamic_motd.smx"),
			}
		}
	},
	{
		"funcommandsx",
		{
			"https://forums.alliedmods.net/showpost.php?p=665771",
			{
				File("a:funcommandsX_.*.zip"),
			}
		}
	},
	{
		"steamtools",
		{
			"https://builds.limetech.io/?p=steamtools",
			{
				File("a:.*"),
			}
		}
	},
	{
		"tf2attributes",
		{
			"https://github.com/FlaminSarge/tf2attributes/",
			{
				File("p:tf2attributes.smx"),
				File("g:https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt"),
			}
		}
	},
	{
		"tf2items",
		{
			"https://builds.limetech.io/?p=tf2items",
			{
				File("a:.*"),
			}
		}
	},
	{
		"thriller",
		{
			"https://forums.alliedmods.net/showpost.php?p=1590169",
			{
				File("p:thriller.smx"),
				File("g:thriller.plugin.txt"),
			}
		}
	},
	{
		"updater",
		{
			"https://bitbucket.org/GoD_Tony/updater/downloads/",
			{
				File("p:updater.smx"),
			}
		}
	}
};
