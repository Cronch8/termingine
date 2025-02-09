#!/bin/bash

if [ -z $1 ]; then
    executableName="termingine"
else
    executableName="$1"
fi

gcc main.c -o $executableName -Wall -lncurses
