#!/bin/bash
git submodule update --init --recursive
cd radix
git switch master
git pull
make
cd ../
rm -v ./src/*.o
make
