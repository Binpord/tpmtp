#!/bin/bash

exe="./array_sorter"
if [ ! -f $exe ]; then
  cmake . && make
fi

res_file="./cores"
if [ -f $res_file ]; then
  rm $res_file
fi

for i in $(seq 2 20); do
  exe_time=$( { time $exe 1000000 $i; } 2>&1 )
  echo $exe_time | gawk -v i="$i" '{ print i, $2 }' >> $res_file
done

python ./plot_result.py $res_file

res_file="./arr_size"
if [ -f $res_file ]; then
  rm $res_file
fi

for i in $(seq -f "%.0f" 1000000 1000000 20000000); do
  exe_time=$( { time $exe $i; } 2>&1 )
  echo $exe_time | gawk -v i="$i" '{ print i, $2 }' >> $res_file
done

python ./plot_result.py $res_file

