#!/bin/bash

for f2 in GZ2P01/*.s; do
    echo "adding $f2 checksum"
    dest_sums+=($(md5sum $f2 | awk '{ print $1 }'))
done

for f1 in GZ2E01/*.s; do
    echo "Processing $f1 file.."
    current_sum=$(md5sum $f1 | awk '{ print $1 }')
    if [[ " ${dest_sums[@]} " =~ " ${current_sum} " ]]; then
        echo "$f1 matched"
    else
        echo "$f1" >> results2.txt
    fi

    # if [[ $match ]]; then
        
    # fi
done
