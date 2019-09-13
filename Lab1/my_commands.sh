#!/usr/bin/env bash 

./echo_arg csc209 >> echo_out.txt 
./echo_stdin 
./count 209 | wc -m
ls -S1 | ./echo_stdin
