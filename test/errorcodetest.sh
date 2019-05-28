#!/bin/bash

echo ""
echo -e "\t\e[35m##############################################"
echo -e "\t\e[35m############ TESTING ERROR CODES #############"
echo -e "\t\e[35m##############################################"
echo -e "\t\e[39m"

function test_return()
{
	EXPECTED=$1
	ACTUAL=$2
	if [[ $EXPECTED -eq $ACTUAL ]]; then
		echo -e "\e[32m[    OK ]\e[39m $ACTUAL == $EXPECTED"
	else
		echo -e "\e[31m[ ERROR ]\e[39m got $ACTUAL instead of $EXPECTED"
		exit 1
	fi
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

chmod -w -R ./mod
$SMAM install thriller -d ./mod # 1 - permission error
test_return 1 $?
chmod +w -R ./mod

# Error code 2 should be checked with above as well

$SMAM -d ./mod # 3 - no command
test_return 3 $?

$SMAM badcommand -d ./mod # 4 - unknown command
test_return 4 $?

$SMAM install -d ./mod # 5 - no addons
test_return 5 $?

$SMAM install thriller # 6 - SM root not found
test_return 6 $?

echo "Tearing down environment in $BUILD_DEST/"
rm -vrf ./mod

cd - > /dev/null
