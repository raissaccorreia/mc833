pids=""
for i in {1..10}
do
    echo $i
    ./cliente 127.0.0.1 8000 < input.txt &
    pids="$pids $!"
done

for pid in $pids; do
    wait $pid
done
