#!bash/sh

#gcc -o minilua -lm src/host/minilua.c

minilua ../dynasm/dynasm.lua  -o func_test.c -D X64 dummy.dasc

gcc main.c -I../dynasm
