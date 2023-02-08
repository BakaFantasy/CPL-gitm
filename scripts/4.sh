#!/usr/bin/bash

root="$(../gitm commit)"
echo hello >blob1
blob1="$(../gitm commit)"
../gitm checkout "$root"
echo "this is hell" >blob2
blob2="$(../gitm commit)"
merge1="$(../gitm merge $blob1)"
mkdir blob_dst
mkdir blob_dst/ccc
echo world >blob_dst/ccc/py
dst="$(../gitm commit)"
../gitm checkout "$blob2"
mkdir blob_head
mkdir blob_head/aaa
echo new >blob_head/aaa/bbb
HEAD="$(../gitm commit)"
gdb --args ../gitm merge "$dst"
#echo "root = $root"
#echo "blob1 = $blob1"
#echo "blob2 = $blob2"
#echo "merge1 = $merge1"
#echo "HEAD = $HEAD"
#echo "dst = $dst"
