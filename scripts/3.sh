#!/usr/bin/bash

root="$(../gitm commit)"
echo hello >apple
blob1="$(../gitm commit)"
../gitm checkout "$root"
echo hello >banana
blob2="$(../gitm commit)"
echo "root = $root"
echo "blob1 = $blob1"
echo "blob2 = $blob2"
#blob3="$(../gitm merge $blob1)"
