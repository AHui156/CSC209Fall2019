#!/usr/bin/env bash

compile(){
    gcc -Wall -std=gnu99 -g -o $1 $2

}

compile count count.c 
compile echo_arg echo_arg.c 
compile echo_stdin echo_stdin.c 
compile hello hello.c
 
