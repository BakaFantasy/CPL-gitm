#!/usr/bin/bash

root="$(../gitm commit)"
echo hello >aaa
blob1="$(../gitm commit)"
../gitm checkout "$root"
echo hell >bbb
blob2="$(../gitm commit)"
../gitm checkout "$root"
echo "it's me" >ccc
blob3="$(../gitm commit)"
echo "dont hello" >ddd
blob4="$(../gitm commit)"

../gitm checkout "$blob2"
merge1="$(../gitm merge $blob1)"
echo "i wanna" >>bbb
dst="$(../gitm commit)"
../gitm checkout "$blob2"
HEAD="$(../gitm merge $blob4)"
echo "root = $root"
echo "blob1 = $blob1"
echo "blob2 = $blob2"
echo "blob3 = $blob3"
echo "blob4 = $blob4"
echo "merge1 = $merge1"
echo "HEAD = $HEAD"
echo "dst = $dst"
