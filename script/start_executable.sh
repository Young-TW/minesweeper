#!/usr/bin/env bash

TEST_MACHINE_1=192.168.0.240
USER=young
TARGET_PATH=/home/$USER/minesweeper

ssh -Y $USER@$TEST_MACHINE_1 << EOF
    export DISPLAY=:0
    cd $TARGET_PATH
    ./minesweeper
EOF
