#!/usr/bin/env bash

cd ..
make clean
cd dir || return

../gitm init
../gitm commit

