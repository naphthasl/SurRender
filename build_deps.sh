#!/bin/bash
git submodule update --init --recursive
cd radix
git switch master
git pull
make
cd ../
make
