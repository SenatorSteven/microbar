#!/bin/bash
mkdir output
gcc -Wall -Wextra -pedantic microbar.c getParameters.c readConfig.c eventLoop.c $(pkg-config --cflags --libs x11) -o output/microbar
