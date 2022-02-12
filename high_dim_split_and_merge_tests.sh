#!/usr/bin/env bash

bash/compile_protos.sh

python -m python.split_and_merge_tests.high_dim_tests.generate_asciipb

RED=$'\e[0;31m'
NC=$'\e[0m'
echo -e "\n\n${RED}Settings updated! Starting the tests...${NC}\n\n"

python python/split_and_merge_tests/high_dim_tests/tests.py
