#!/bin/bash

flex -oslghscan.yy.c slghscan.l
echo flex -oslghscan.yy.c slghscan.l
bison --debug -d slghparse.y
echo bison --debug -d slghparse.y
