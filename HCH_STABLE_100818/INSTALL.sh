#!/bin/bash

echo "installing..."

if [ ! -d "source" ]; then
        mkdir source
fi

rm -rf source/*

pname=`basename "$PWD"`

cd raw
cat $pname.ino aa_definitions.h *.ino > ../source/source.ino

cp ../Makefile source/
cd source
make
