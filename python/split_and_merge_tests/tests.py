import subprocess
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import arviz as az

resources_path = "./resources/split_and_merge_tests/"

for dataset in ["galaxy", "faithful"]:
	cmd = ["./bash/split_and_merge_tests.sh", dataset]
	result = subprocess.run(cmd, capture_output=True)
	print(result.stdout.decode("utf-8"))
	print(result.stderr.decode("utf-8"))

	output_path = resources_path+dataset+"_tests/out/"

	density = np.genfromtxt(output_path+"density_file.csv", delimiter=",")
	best_clust = np.genfromtxt(output_path+"best_clustering.csv", delimiter=",")

	delimiter=" "
	if(dataset=="faithful"):
		delimiter=","
	data = np.genfromtxt(f"resources/datasets/{dataset}.csv",\
		delimiter=delimiter)
	grid = np.genfromtxt(f"resources/datasets/{dataset}_grid.csv",\
		delimiter=delimiter)

	n_clust = np.genfromtxt(output_path+"numclust.csv", delimiter=",")

	print(f"{dataset} ESS: {az.ess(n_clust)}")

	if dataset=="galaxy":
		fig, axes = plt.subplots(1,1, figsize=(14,5))
		dens = np.exp(np.mean(density[0::2], axis=0))
		axes.plot(grid, dens)
		axes.hist(data, density=True, color='lightgray')
		axes.set_title(f"{dataset} densities with DP")
		plt.savefig(output_path+f"{dataset}_density.png", dpi=500)
		plt.close()

		fig, axes = plt.subplots(1,1, figsize=(14,5))
		axes.scatter(data, np.repeat("SplitAndMerge", len(best_clust)),\
			c=best_clust, cmap='hsv')
		axes.set_ylim(-1, 3)
		axes.set_title(f"{dataset} clustering with DP")
		plt.savefig(output_path+f"{dataset}_cluster.png", dpi=500)
		plt.close()
	elif dataset=="faithful":
		fig, axes = plt.subplots(1,1, figsize=(14,5))
		dens = np.mean(density[0::2], axis=0).reshape(-1, 1)

		plot_data = pd.DataFrame(np.hstack([grid, dens]), columns=["x", "y", "z"])
		Z = plot_data.pivot_table(index="x", columns="y", values="z").T.values
		X_unique = np.sort(plot_data.x.unique())
		Y_unique = np.sort(plot_data.y.unique())
		X, Y = np.meshgrid(X_unique, Y_unique)
		axes.contour(X,Y,Z)
		axes.set_title(f"{dataset} log-densities with DP")
		plt.savefig(output_path+f"{dataset}_density.png", dpi=500)
		plt.close()

		fig, axes = plt.subplots(1,1, figsize=(14,5))
		axes.scatter(data[:,0], data[:,1], c=best_clust, cmap='hsv')
		axes.set_title(f"{dataset} clustering with DP")
		plt.savefig(output_path+f"{dataset}_cluster.png", dpi=500)
		plt.close()