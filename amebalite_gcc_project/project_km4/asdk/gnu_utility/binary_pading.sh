#!/bin/bash 

################
# Library
################
Usage() {
    echo "Usage: $0 [Image Name] [size]"
}

################
# Main
################
if [ "$#" -lt 2 ]; then
    Usage
    exit 1
fi

# Get Parameters
form_file=$1
to_file=$2
size=$3

if [ $size -eq 0 ]; then
	echo "File size is zero, check."
else
	echo "size , $size."

	dd if=$form_file ibs=1k count=$size >  temp.bin
	dd if=/dev/zero ibs=1k count=$size >> temp.bin
	dd if=temp.bin ibs=1k count=$size > $to_file

	rm temp.bin
fi
