#!/bin/bash

filename=$1

if [ ! -f ${filename} ]; then
    echo "file is not existed"
    exit -1
fi

filesize=`ls -l | grep "${filename}" | awk '{ print $5}'`
count=`awk 'BEGIN {printf "%.2f\n", ('${filesize}'/512)}'`  
count=`echo $count | awk '{print int($0)==$0 ? int($0) : int(int($0*10/10+1))}'` #向上取整

if [ $# -eq 1 ]; then
    dd if=$filename of=/home/amdin/os/bochs-2.6.2/hd60M.img bs=512 count=$count conv=notrunc
   echo "dd if=$filename of=/home/amdin/os/bochs-2.6.2/hd60M.img bs=512 count=$count conv=notrunc"
elif [ $# -eq 2 ]; then
   dd if=$filename of=/home/amdin/os/bochs-2.6.2/hd60M.img bs=512 count=$count seek=$2 conv=notrunc
   echo "dd if=$filename of=/home/amdin/os/bochs-2.6.2/hd60M.img bs=512 count=$count seek=$2 conv=notrunc"
fi


