#!/bin/bash

flex -oslghscan.yy.c slghscan.l
echo flex -oslghscan.yy.c slghscan.l
bison -d slghparse.y
echo bison -d slghparse.y
