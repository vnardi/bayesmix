from google.protobuf.text_format import PrintMessage
from math import sqrt
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
  #GALAXY
  # Algorithm settings
  settings = algorithm_params_pb2.AlgorithmParams()
  settings.algo_id = "SplitMerge"
  settings.rng_seed = 4234231 
  settings.iterations = 5000
  settings.burnin = 1000
  settings.init_num_clusters = 3
  settings.neal8_n_aux = 3
  settings.splitmerge_n_restr_gs_updates = 5
  settings.splitmerge_n_mh_updates = 1
  settings.splitmerge_n_full_gs_updates = 1
  with open("resources/split_and_merge_tests/galaxy_tests/algo.asciipb", 'w') as f:
    PrintMessage(settings, f)

  # DP gamma hyperprior
  dp_prior = mixing_prior_pb2.DPPrior()
  dp_prior.gamma_prior.totalmass_prior.shape = 4.0
  dp_prior.gamma_prior.totalmass_prior.rate = 2.0
  with open("resources/split_and_merge_tests/galaxy_tests/mixing.asciipb", "w") as f:
    PrintMessage(dp_prior, f)

  # NNIG NGG hyperprior
  nnig_prior = hierarchy_prior_pb2.NNIGPrior()
  nnig_prior.ngg_prior.mean_prior.mean = 25
  nnig_prior.ngg_prior.mean_prior.var = 4
  nnig_prior.ngg_prior.var_scaling_prior.shape = 0.4
  nnig_prior.ngg_prior.var_scaling_prior.rate = 0.2
  nnig_prior.ngg_prior.shape = 4.0
  nnig_prior.ngg_prior.scale_prior.shape = 4.0
  nnig_prior.ngg_prior.scale_prior.rate = 2.0
  with open("resources/split_and_merge_tests/galaxy_tests/hierarchy.asciipb", "w") as f:
    PrintMessage(nnig_prior, f)


  #FAITHFUL
  # Algorithm settings
  settings = algorithm_params_pb2.AlgorithmParams()
  settings.algo_id = "SplitMerge"
  settings.rng_seed = 4234231 
  settings.iterations = 5000
  settings.burnin = 1000
  settings.init_num_clusters = 3
  settings.neal8_n_aux = 3
  settings.splitmerge_n_restr_gs_updates = 5
  settings.splitmerge_n_mh_updates = 1
  settings.splitmerge_n_full_gs_updates = 1
  with open("resources/split_and_merge_tests/faithful_tests/algo.asciipb", 'w') as f:
    PrintMessage(settings, f)

  # DP gamma hyperprior
  dp_prior = mixing_prior_pb2.DPPrior()
  dp_prior.gamma_prior.totalmass_prior.shape = 4.0
  dp_prior.gamma_prior.totalmass_prior.rate = 2.0
  with open("resources/split_and_merge_tests/faithful_tests/mixing.asciipb", "w") as f:
    PrintMessage(dp_prior, f)

  # NNW NGIW hyperprior
  nnw_prior = hierarchy_prior_pb2.NNWPrior()
  mu00 = [3, 3]
  mat = identity_list(2)
  nu0 = 4.0
  nnw_prior.ngiw_prior.mean_prior.mean.size = len(mu00)
  nnw_prior.ngiw_prior.mean_prior.mean.data[:] = mu00
  sig00 = [m/nu0 for m in mat]
  nnw_prior.ngiw_prior.mean_prior.var.rows = int(sqrt(len(sig00)))
  nnw_prior.ngiw_prior.mean_prior.var.cols = int(sqrt(len(sig00)))
  nnw_prior.ngiw_prior.mean_prior.var.data[:] = sig00
  nnw_prior.ngiw_prior.mean_prior.var.rowmajor = False
  nnw_prior.ngiw_prior.var_scaling_prior.shape = 0.4
  nnw_prior.ngiw_prior.var_scaling_prior.rate = 0.2
  nnw_prior.ngiw_prior.deg_free = nu0
  nnw_prior.ngiw_prior.scale_prior.deg_free = nu0
  tau00 = [nu0*m for m in mat]
  nnw_prior.ngiw_prior.scale_prior.scale.rows = int(sqrt(len(tau00)))
  nnw_prior.ngiw_prior.scale_prior.scale.cols = int(sqrt(len(tau00)))
  nnw_prior.ngiw_prior.scale_prior.scale.data[:] = tau00
  nnw_prior.ngiw_prior.scale_prior.scale.rowmajor = False
  with open("resources/split_and_merge_tests/faithful_tests/hierarchy.asciipb", "w") as f:
    PrintMessage(nnw_prior, f)