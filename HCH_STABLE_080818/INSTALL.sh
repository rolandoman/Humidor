#!/bin/bash

mkdir source
cat HCH_STABLE_080818.ino aa_definitions.h *.ino > source/source.ino
cp Makefile source/
cd source
make
