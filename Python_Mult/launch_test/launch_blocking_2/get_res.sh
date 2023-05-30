#!/bin/bash

for (( i = 1; i<= 20; i++)); do

	grep "$1" run_"$i"/* 

done
