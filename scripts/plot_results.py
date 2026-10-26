import csv
import matplotlib.pyplot as plt

with open("results/matmul_results.csv") as file:
    reader = csv.DictReader(file)
    rows = list(reader)

n_values = [int(row["n"]) for row in rows]
ikj_times = [float(row["ikj_ms"]) for row in rows]
ijk_times = [float(row["ijk_ms"]) for row in rows]
speedups = [float(row["speedup"]) for row in rows]

plt.plot(n_values,ijk_times, marker = "o", label="IJK")
plt.plot(n_values, ikj_times, marker = "o", label="IKJ")

plt.xlabel("Matrix size (N)")
plt.ylabel("Execution time (ms)")
plt.legend()
plt.grid(True)
plt.title("Matrix Multiplication: IJK vs IKJ")
plt.xticks([64, 128, 256])
plt.tight_layout()
plt.savefig("results/execution_time.png",dpi=150)
plt.close()


plt.plot(n_values,speedups, marker = "o", label="speedup")
plt.xlabel("Matrix size (N)")
plt.ylabel("Speed up (x) ")
plt.legend()
plt.grid(True)
plt.title("IKJ Speedup over IJK")
plt.xticks([64, 128, 256])
plt.tight_layout()
plt.savefig("results/speedup.png",dpi=150)
plt.close()