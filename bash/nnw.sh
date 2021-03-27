#!/usr/bin/env bash

build/run \
  resources/asciipb/thesis/Neal3.asciipb \
  NNW resources/asciipb/thesis/hier_faithful.asciipb \
  DP  resources/asciipb/thesis/DP.asciipb \
  "" \
  resources/nnw/data.csv \
  resources/nnw/grid.csv \
  resources/nnw/dens.csv \
  resources/nnw/nclu.csv \
  resources/nnw/clus.csv
