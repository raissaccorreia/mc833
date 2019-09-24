#!/bin/bash
pids=""
for i in {1..10}
do
    echo "cliente $i"
    ./cliente 127.0.0.1  < input.txt &
    pids="$pids $!"
done

for i in {1..10}; do
    wait $pid
done 