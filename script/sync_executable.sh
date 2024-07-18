#!/usr/bin/env bash

TEST_MACHINE_1=192.168.0.240
USER=young
TARGET_PATH=/home/$USER/minesweeper

scp build/minesweeper $USER@$TEST_MACHINE_1:$TARGET_PATH
