#!/bin/bash

for (( i = 1; i<= $2; i++)); do

	grep "$1" run_"$i"/* 

done
