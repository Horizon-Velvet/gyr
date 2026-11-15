#!/usr/bin/env bash
set -euo pipefail

old_size=$(stat -c '%s' ./build/look 2>/dev/null || echo 0)

cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel

echo "Done building ./build/look"

new_size=$(stat -c '%s' ./build/look)

if (( old_size == 0 )); then
    echo "Initial build: $(numfmt --to=iec-i --suffix=B "$new_size")"
elif (( new_size > old_size )); then
    diff=$((new_size - old_size))
    echo "Binary size increased by $(numfmt --to=iec-i --suffix=B "$diff")"
elif (( new_size < old_size )); then
    diff=$((old_size - new_size))
    echo "Binary size decreased by $(numfmt --to=iec-i --suffix=B "$diff")"
else
    echo "Binary size unchanged"
fi

echo "Total of $(numfmt --to=iec-i --suffix=B "$new_size")"

trap 'printf "\n"' EXIT

duration=2.0
start=$EPOCHREALTIME

while :; do
    rem=$(awk -v s="$start" -v n="$EPOCHREALTIME" -v d="$duration" 'BEGIN {
        r = d - (n - s);
        printf "%.1f", (r > 0 ? r : 0)
    }')

    printf "\r\033[KTime left: %s" "$rem"
    
    [ "$rem" = "0.0" ] && break
    sleep 0.05
done

./build/look