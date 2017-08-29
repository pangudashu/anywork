#!bash/sh

#gcc -o minilua -lm src/host/minilua.c

minilua dynasm/dynasm.lua  -o myjit_x86_64.c -D X64 myjit_x86_64.dasc

gcc myjit.c -g -I./dynasm -o myjit
