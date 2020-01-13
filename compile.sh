#!/bin/bash
if [ ! -d "output" ]; then
	mkdir output
fi
gcc -Wall -Wextra -pedantic globals.c microbar.c getParameters.c readConfig.c eventLoop.c -lX11 -lXrandr -o output/microbar
exit 0
