#!/usr/bin/env bash

bash script/build.sh

bash script/sync_executable.sh
bash script/sync_img.sh

bash script/start_executable.sh
