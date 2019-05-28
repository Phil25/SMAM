#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo "Usage: $0 <build destination> <smam bin (relative)>"
	exit 1
fi

SMAM="$2"
BUILD_DEST="$1"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
$DIR/cmdinstalltest.sh "$BUILD_DEST" "$SMAM" \
&& $DIR/cmdremovetest.sh "$BUILD_DEST" "$SMAM" \
&& $DIR/errorcodetest.sh "$BUILD_DEST" "$SMAM"
