#!/usr/bin/bash

root="$(../gitm commit)"
echo hello >aaa
blob1="$(../gitm commit)"
../gitm checkout "$root"
echo hell >bbb
blob2="$(../gitm commit)"
../gitm checkout "$root"
mkdir -p ccc/cc/c
echo "it's me" >ccc/cc/c/blob3
blob3="$(../gitm commit)"
mkdir -p ddd/dd
echo "dont hello" >ddd/dd/blob4
blob4="$(../gitm commit)"

../gitm checkout "$blob1"
merge1="$(../gitm merge $blob2)"
mkdir true_b
mv bbb true_b
dst="$(../gitm commit)"
../gitm checkout "$blob4"
HEAD="$(../gitm merge $blob2)"
echo -n "merge: "
../gitm merge "$dst"
../gitm checkout "$dst"
echo -n "merge: "
../gitm merge "$HEAD"
#echo "root = $root"
#echo "blob1 = $blob1"
#echo "blob2 = $blob2"
#echo "blob3 = $blob3"
#echo "blob4 = $blob4"
#echo "merge1 = $merge1"
#echo "HEAD = $HEAD"
#echo "dst = $dst"
