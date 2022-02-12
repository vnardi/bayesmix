from google.protobuf.text_format import PrintMessage
from math import sqrt
import numpy as np
from proto.py import algorithm_params_pb2
from proto.py import distribution_pb2
from proto.py import mixing_prior_pb2
from proto.py import hierarchy_prior_pb2

# Run this from root with python -m python.generate_asciipb

def identity_list(dim):
  """Returns the list of entries of a dim-dimensional identity matrix."""
  ide = dim*dim*[0.0]
  for i in range(0, dim*dim, dim+1):
    ide[i] = 1.0
  return ide

if __name__ == "__main__":
  dim_array = np.genfromtxt("resources/split_and_merge_tests/high_dim_tests/dim_to_test.csv",
    delimiter=",", dtype=int)

  for d in dim_array:
    # Algorithm settings
    settings = algorithm_params_pb2.AlgorithmParams()
    settings.algo_id = "SplitMerge"
    settings.rng_seed = 4234231
    settings.iterations = 5000
    settings.burnin = 1000
    settings.init_num_clusters = 1
    settings.neal8_n_aux = 3
    settings.splitmerge_n_restr_gs_updates = 5
    settings.splitmerge_n_mh_updates = 1
    settings.splitmerge_n_full_gs_updates = 1
    with open(f"resources/split_and_merge_tests/high_dim_tests/algo_{d}d.asciipb", 'w') as f:
      PrintMessage(settings, f)

    # DP gamma hyperprior
    dp_prior = mixing_prior_pb2.DPPrior()
    dp_prior.gamma_prior.totalmass_prior.shape = 1.0
    dp_prior.gamma_prior.totalmass_prior.rate = 1.0
    with open(f"resources/split_and_merge_tests/high_dim_tests/mixing_{d}d.asciipb", "w") as f:
      PrintMessage(dp_prior, f)

    # NNW
    nnw_prior = hierarchy_prior_pb2.NNWPrior()
    mu0 = [0]*d
    mat = identity_list(d)
    nnw_prior.fixed_values.mean.size = len(mu0)
    nnw_prior.fixed_values.mean.data[:] = mu0
    nnw_prior.fixed_values.var_scaling = 1
    nnw_prior.fixed_values.deg_free = d
    nnw_prior.fixed_values.scale.rows = int(sqrt(len(mat)))
    nnw_prior.fixed_values.scale.cols = int(sqrt(len(mat)))
    nnw_prior.fixed_values.scale.data[:] = mat
    nnw_prior.fixed_values.scale.rowmajor = False
    with open(f"resources/split_and_merge_tests/high_dim_tests/hierarchy_{d}d.asciipb", "w") as f:
      PrintMessage(nnw_prior, f)
