#!/usr/bin/env bash

data=$1

build/run_mcmc \
  --algo-params-file resources/split_and_merge_tests/high_dim_tests/algo_${data}d.asciipb \
  --hier-type NNW --hier-args resources/split_and_merge_tests/high_dim_tests/hierarchy_${data}d.asciipb \
  --mix-type DP --mix-args resources/split_and_merge_tests/high_dim_tests/mixing_${data}d.asciipb \
  --coll-name resources/split_and_merge_tests/high_dim_tests/out/chains_${data}d.recordio \
  --data-file resources/datasets/synt_dataset_${data}d.csv \
  --n-cl-file resources/split_and_merge_tests/high_dim_tests/out/numclust_${data}d.csv \
  --clus-file resources/split_and_merge_tests/high_dim_tests/out/clustering_${data}d.csv \
  --best-clus-file resources/split_and_merge_tests/high_dim_tests/out/best_clustering_${data}d.csv
echo
