#!/bin/bash

cd ./src/wasm_c
emcc -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s EXPORTED_FUNCTIONS=_malloc --no-entry webserver.c -o server.wasm
echo "Fix static w2c_malloc to public w2c_malloc and andd tiny_ to WASM_RT_MODULE_PREFIX"
gcc wasm_tiny.c wasm_server.c wasm-rt-impl.c wasm_import.h -o wasm_server -pthread

