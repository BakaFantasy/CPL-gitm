#!/usr/bin/env bash

for ((i = 0; i < $1; i++)); do
    echo "hello, $(date +%N)" >>"$(date +%N)"
    ../gitm commit
    sleep 0.01
done
