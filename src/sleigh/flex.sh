#!/bin/bash

flex -oslghscan.cc slghscan.l
echo flex -oslghscan.cc slghscan.l
bison -d slghparse.y -o slghparse.cc
mv slghparse.hh slghparse.tab.hh

bison -p xml -o xml.cc xml.y
echo bison -p xml -o xml.cc xml.y
