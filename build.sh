#!/bin/bash

gcc -pthread -o server src/tiny.c 
emcc -pthread src/tiny.c
