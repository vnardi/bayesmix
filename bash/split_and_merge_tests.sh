#!/usr/bin/env bash

data=$1

if [ "$data" == 'galaxy' ]; then
	hier='NNIG'
elif [ "$data" == 'faithful' ]; then
	hier='NNW'
else
	echo 'Use arg galaxy or faithful'
	exit 
fi

folder="${data}_tests"

build/run_mcmc \
  --algo-params-file resources/split_and_merge_tests/${folder}/algo.asciipb \
  --hier-type ${hier} --hier-args resources/split_and_merge_tests/${folder}/hierarchy.asciipb \
  --mix-type DP --mix-args resources/split_and_merge_tests/${folder}/mixing.asciipb \
  --coll-name resources/split_and_merge_tests/${folder}/out/chains.recordio \
  --data-file resources/datasets/${data}.csv \
  --grid-file resources/datasets/${data}_grid.csv \
  --dens-file resources/split_and_merge_tests/${folder}/out/density_file.csv \
  --n-cl-file resources/split_and_merge_tests/${folder}/out/numclust.csv \
  --clus-file resources/split_and_merge_tests/${folder}/out/clustering.csv \
  --best-clus-file resources/split_and_merge_tests/${folder}/out/best_clustering.csv
echo
