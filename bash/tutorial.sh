#!/usr/bin/env bash

# build/run \
#   algorithm_settings_file \
#   hierarchy_type  hierarchy_prior_file \
#   mixing_type  mixing_prior_file \
#   collector_name \
#   data_file \
#   grid_file \
#   density_output_file \
#   numclust_output_file \
#   clustering_output_file \
#   [hierarchy_covariates_file] \
#   [hierarchy_grid_covariates_file] \
#   [mixing_covariates_file] \
#   [mixing_grid_covariates_file]

build/run \
  resources/tutorial/algo.asciipb \
  NNIG resources/tutorial/nnig_ngg.asciipb \
  DP   resources/tutorial/dp_gamma.asciipb \
  "" \
  resources/tutorial/data.csv \
  resources/tutorial/grid.csv \
  resources/tutorial/out/density.csv \
  resources/tutorial/out/numclust.csv \
  resources/tutorial/out/clustering.csv