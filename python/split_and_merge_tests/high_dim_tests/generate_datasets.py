import numpy as np
import scipy.stats as ss

# Run this algorithm from the root!

output_folder = "resources/datasets/"

n_data = 100

for d in [11,12,13,14,15]:
  data = np.zeros((n_data, d))
  n_data_first_clust = round(n_data/2)
  data[0:n_data_first_clust] = ss.multivariate_normal(
      np.repeat(3/np.sqrt(d), d), np.identity(d)).rvs(n_data_first_clust)
  n_data_second_clust = n_data-n_data_first_clust
  data[n_data_first_clust:n_data] = ss.multivariate_normal(
      np.repeat(-3/np.sqrt(d), d), np.identity(d)).rvs(n_data_second_clust)

  np.savetxt(f"{output_folder}synt_dataset_{d}d.csv", data, delimiter=",")
