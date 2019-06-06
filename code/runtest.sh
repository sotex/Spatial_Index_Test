#!/bin/sh
if [[ $# -ne 1 ]]; then
	echo "Usage $0 testprogram"
	exit
fi

for ((i=1;i<=5;i++))
do
	logfile="../resultlog/log_$1_${i}.txt"
	echo "./$1 > $logfile"
	./$1 > $logfile
done
