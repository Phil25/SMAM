#!/bin/bash

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
declare -a data_updater=(
	"updater"
	"./mod/addons/sourcemod/plugins/updater.smx"
)

function test_addon()
{
	$SMAM install $1 --destination ./mod
	shift
	for file in $@; do
		if [ ! -f "$file" ] || [ `stat -c%s "$file"` -eq 0 ]; then
			echo -e "\e[31m[ ERROR ]\e[39m $file"
			exit 1
		else
			echo -e "\e[32m[    OK ]\e[39m $file"
		fi
	done
	echo ""
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

echo "Setting up environment in $BUILD_DEST/..."
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
test_addon ${data_tf2attributes[@]}
test_addon ${data_tf2items[@]}
test_addon ${data_thriller[@]}
test_addon ${data_updater[@]}

echo "Tearing down environment in $BUILD_DEST/..."
rm -vrf ./mod

cd - > /dev/null
