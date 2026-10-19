#!/usr/bin/env bash

# Compile the server from its source files
gcc -I./src src/server.c src/models.c src/common.c -o run_server -lpthread

# Run only if compilation succeeded
if [ $? -eq 0 ]; then
    "./run_server"
fi