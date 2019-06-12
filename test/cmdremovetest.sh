#!/bin/bash

echo ""
echo -e "\t\e[35m#############################################"
echo -e "\t\e[35m############## TESTING REMOVAL ##############"
echo -e "\t\e[35m#############################################"
echo -e "\t\e[39m"

declare -a data_advancedinfiniteammo=(
	"advancedinfiniteammo"
	"./mod/addons/sourcemod/plugins/AdvancedInfiniteAmmo.smx"
)
declare -a data_afk_manager=(
	"afk_manager"
	"./mod/addons/sourcemod/plugins/afk_manager4.smx"
	"./mod/addons/sourcemod/translations/afk_manager.phrases.txt"
)
declare -a data_dynamic_motd=(
	"dynamic_motd"
	"./mod/addons/sourcemod/plugins/dynamic_motd.smx"
)
declare -a data_funcommandsx=(
	"funcommandsx"
	"./mod/addons/sourcemod/plugins/funcommandsX.smx"
	"./mod/addons/sourcemod/configs/happy_phrases.ini"
	"./mod/addons/sourcemod/translations/funcommandsX.phrases.txt"
	"./mod/addons/sourcemod/translations/ru/funcommandsX.phrases.txt"
	"./mod/addons/sourcemod/translations/tr/funcommandsX.phrases.txt"
)
declare -a data_tf2attributes=(
	"tf2attributes"
	"./mod/addons/sourcemod/plugins/tf2attributes.smx"
	"./mod/addons/sourcemod/gamedata/tf2.attributes.txt"
)
declare -a data_tf2items=(
	"tf2items"
	"./mod/addons/sourcemod/scripting/tf2items_manager.sp"
	"./mod/addons/sourcemod/scripting/include/tf2items.inc"
	"./mod/addons/sourcemod/extensions/tf2items.ext.2.ep2v.so"
)
declare -a data_thriller=(
	"thriller"
	"./mod/addons/sourcemod/plugins/thriller.smx"
	"./mod/addons/sourcemod/gamedata/thriller.plugin.txt"
)
declare -a data_rtd=(
	"rtd"
	"./mod/addons/sourcemod/configs/rtd2_perks.default.cfg"
	"./mod/addons/sourcemod/plugins/rtd.smx"
	"./mod/addons/sourcemod/scripting/rtd.sp"
	"./mod/addons/sourcemod/translations/rtd2.phrases.txt"
	"./mod/addons/sourcemod/translations/rtd2_perks.phrases.txt"
)
declare -a data_updater=(
	"updater"
	"./mod/addons/sourcemod/plugins/updater.smx"
)

function test_addon()
{
	ADDON="$1"
	shift

	$SMAM install $ADDON -d ./mod
	for file in $@; do
		if [ ! -f "$file" ] || [ `stat -c%s "$file"` -eq 0 ]; then
			echo -e "\e[31m[ ERROR ]\e[39m $file"
			exit 1
		else
			echo -e "\e[32m[    OK ]\e[39m $file"
		fi
	done

	$SMAM remove $ADDON -d ./mod
	for file in $@; do
		if [ ! -f "$file" ]; then
			echo -e "\e[32m[    OK ]\e[39m Not found: $file"
		else
			echo -e "\e[31m[ ERROR ]\e[39m Still exists: $file"
			exit 1
		fi
	done
	echo ""
}

function test_shared()
{
	echo -e "\t\e[35m##############################################"
	echo -e "\t\e[35m### TESTING SHARED FILE REMOVAL PREVENTION ###"
	echo -e "\t\e[35m##############################################"
	echo -e "\t\e[39m"

	shift
	UNSHARED_FILE="$1" # bin
	SHARED_FILE="$2" # gamedata

	$SMAM install thriller sharedthriller -d ./mod
	for file in $@; do
		if [ ! -f "$file" ] || [ `stat -c%s "$file"` -eq 0 ]; then
			echo -e "\e[31m[ ERROR ]\e[39m $file"
			exit 1
		else
			echo -e "\e[32m[    OK ]\e[39m $file"
		fi
	done

	$SMAM remove sharedthriller -d ./mod
	for file in $@; do
		if [ ! -f "$file" ] || [ `stat -c%s "$file"` -eq 0 ]; then
			echo -e "\e[31m[ ERROR ]\e[39m $file"
			exit 1
		else
			echo -e "\e[32m[    OK ]\e[39m $file"
		fi
	done

	$SMAM install sharedthriller -d ./mod
	$SMAM remove thriller -d ./mod
	if [ ! -f "$SHARED_FILE" ] || [ `stat -c%s "$SHARED_FILE"` -eq 0 ]; then
		echo -e "\e[31m[ ERROR ]\e[39m $SHARED_FILE"
		exit 1
	else
		echo -e "\e[32m[    OK ]\e[39m $SHARED_FILE"
	fi

	if [ ! -f "$UNSHARED_FILE" ]; then
		echo -e "\e[32m[    OK ]\e[39m $UNSHARED_FILE"
	else
		echo -e "\e[31m[ ERROR ]\e[39m $UNSHARED_FILE"
		exit 1
	fi
}

if [ "$#" -lt 2 ]; then
	echo "Usage: $0 <build destination> <smam bin (relative)>"
	exit 1
fi

SMAM="$2"
BUILD_DEST="$1"

if [ ! -d "$BUILD_DEST" ]; then
	echo "Directory $BUILD_DEST does not exist"
	exit 1
fi

cd "$BUILD_DEST"

if [ ! -f $SMAM ]; then
	echo "$SMAM not found! Make sure its path is relative to $BUILD_DEST/."
	exit 1
fi

echo "Setting up environment in $BUILD_DEST/"
rm -rf ./mod
mkdir -vp ./mod/cfg \
	./mod/addons/sourcemod/configs \
	./mod/addons/sourcemod/data \
	./mod/addons/sourcemod/gamedata \
	./mod/addons/sourcemod/plugins \
	./mod/addons/sourcemod/scripting \
	./mod/addons/sourcemod/translations
echo ""

test_addon ${data_tf2attributes[@]}
test_addon ${data_advancedinfiniteammo[@]}
test_addon ${data_afk_manager[@]}
test_addon ${data_dynamic_motd[@]}
test_addon ${data_funcommandsx[@]}

# unused directory `source` created by funcommandsx
if [ -d "./mod/addons/sourcemod/source" ]; then
	echo -e "\e[31m[ ERROR ]\e[39m Unused directories not removed"
	exit 1
else
	echo -e "\e[32m[    OK ]\e[39m Unused directories removed"
fi

test_addon ${data_tf2attributes[@]}
test_addon ${data_tf2items[@]}
test_addon ${data_thriller[@]}
test_addon ${data_rtd[@]}
test_addon ${data_updater[@]}

test_shared ${data_thriller[@]}

echo "Tearing down environment in $BUILD_DEST/"
rm -vrf ./mod

cd - > /dev/null
