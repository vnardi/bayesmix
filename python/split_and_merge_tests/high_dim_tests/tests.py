import subprocess
import numpy as np
import matplotlib.pyplot as plt
import arviz as az

resources_path = "./resources/split_and_merge_tests/high_dim_tests/"

dim_array=np.genfromtxt("resources/split_and_merge_tests/high_dim_tests/dim_to_test.csv",
	delimiter=",", dtype=int)

for d in dim_array:
	cmd = ["./bash/high_dim_split_and_merge_tests.sh", str(d)]
	result = subprocess.run(cmd, capture_output=True)
	print(result.stdout.decode("utf-8"))
	print(result.stderr.decode("utf-8"))

	output_path = resources_path+"out/"

	best_clust = np.genfromtxt(f"{output_path}best_clustering_{d}d.csv", delimiter=",")

	n_clust = np.genfromtxt(f"{output_path}numclust_{d}d.csv", delimiter=",")

	fig, axes = plt.subplots(1,1, figsize=(14,5))
	axes.plot(n_clust)
	axes.set_title(f"Trace plot of the number of clusters in {d} dimensions")
	plt.savefig(f"{output_path}n_clust_trace_plot_{d}d.png", dpi=500)
	plt.close()


	print(f"{d}d ESS: {az.ess(n_clust)}")
	print(f"{d}d number of clusters in the best clustering: {len(np.unique(best_clust))}")
