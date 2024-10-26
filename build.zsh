#!/bin/zsh

if [[ -d bin ]]; then
    rm -rf bin
fi

mkdir bin

clang \
    -I src/includes \
    src/includes/commands.c \
    src/includes/terminal.c \
    src/includes/signal_handler.c \
    src/includes/executor.c \
    src/includes/flow_control.c \
    src/main.c \
    -o ./bin/main \
    -lncurses \
    --std=c23 \
    -Wall \
    -Wextra \
    -Werror \
    -pedantic \
    -Wno-unused-parameter \
    -Wno-unused-variable \
    -Wno-unused-function \
    -Wno-unused-value \
    -Wno-unused-but-set-variable \
    -Wno-unused-but-set-parameter \
    -Wno-unused-local-typedefs

# ./bin/main -v
# ./bin/main --interval 1 top
# ./bin/main --interval 10 ls -la /Users/franciscodelahoz
./bin/main --interval 1 date
# ./bin/main -e $PWD/test.sh
# ./bin/main $PWD/test.sh
