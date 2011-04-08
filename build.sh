#!/bin/bash
# This file is part of Flick. See ./COPYING for legal details.
# Purpose: Provide a rudimentary build system.

PROJECT_NAME="flick"
DEBUG_FLAGS="-g"
COMPILE="g++ $DEBUG_FLAGS -Wall"
LIBRARIES=(X11)
AUX_FILES=(AUTHORS EMAIL README build.sh)
MODULES=(client server)

for l in ${LIBRARIES[@]}; do COMPILE="$COMPILE -l$l"; done
for f in ${AUX_FILES[@]}; do INCLUDED="$INCLUDED $f"; done
for m in ${MODULES[@]}; do $COMPILE -o $PROJECT_NAME-$m $m.cpp; done
tar cf $PROJECT_NAME.tar $INCLUDED *.h *.cpp && gzip -9 $PROJECT_NAME.tar
