#!/bin/bash -e 

in_file=$1
out_file=$2

out_data0=`xxd -i -c 16 $in_file`
#echo "$out_data0"

# delete all strings before '{'
out_data1=${out_data0##*\{}
#echo "$out_data1"

# delete all strings after '}'
out_data2=${out_data1%%\}*}
#echo "$out_data2"

echo "$out_data2" > $out_file