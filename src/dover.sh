#!/bin/bash
		
CURVER=`cat .ver`
NEWVER=`expr $CURVER + 1`
echo "$NEWVER" > .ver
GCCVER=`gcc --version | head -1`
MAKEDATE=`date`

printf "$NEWVER\n$MAKEDATE\n$GCCVER\n" > .verinfo
echo "Performing build $NEWVER"

