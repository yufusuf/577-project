import matplotlib.pyplot as plt

# File path to the text file
file_path = "analytics.txt"

# Read the file and parse the data
results = []
with open(file_path, "r") as f:
    for line in f:
        # Split the line by commas and convert to float
        values = [float(x) for x in line.strip().split(",")]
        results.append(values)

# Sort results by the 'size' field (last column)
results.sort(key=lambda x: x[8])

# Separate the results into individual lists
dgtsv_time = [r[0] for r in results]
dgtsv_err = [r[1] for r in results]
dgesv_time = [r[2] for r in results]
dgesv_err = [r[3] for r in results]
cyclic_seq_time = [r[4] for r in results]
cyclic_seq_err = [r[5] for r in results]
cyclic_par_time = [r[6] for r in results]
cyclic_par_err = [r[7] for r in results]
sizes = [r[8] for r in results]

with open("./results/execution_times.txt", "w") as out_file:
    # Write the header
    out_file.write("Size,DGTSV Time,DGESV Time,Cyclic Sequential Time,Cyclic Parallel Time\n")
    # Write the data
    for size, dgtsv, dgesv, cyclic_seq, cyclic_par in zip(sizes, dgtsv_time, dgesv_time, cyclic_seq_time, cyclic_par_time):
        out_file.write(f"{size},{dgtsv},{dgesv},{cyclic_seq},{cyclic_par}\n")
# Plot the results
plt.figure(figsize=(12, 6))

# Plot execution times
# plt.subplot(1, 2, 1)
plt.yscale("log")
plt.plot(sizes, dgtsv_time, label="DGTSV Time", marker="o")
plt.plot(sizes, dgesv_time, label="DGESV Time", marker="o")
plt.plot(sizes, cyclic_seq_time, label="Cyclic Sequential Time", marker="o")
plt.plot(sizes, cyclic_par_time, label="Cyclic Parallel Time", marker="o")
plt.xlabel("Matrix Size")
plt.ylabel("Execution Time (ms)")
plt.title("Execution Times")
plt.legend()

# Plot errors
# plt.subplot(1, 2, 2)
# plt.plot(sizes, dgtsv_err, label="DGTSV Error", marker="o")
# plt.plot(sizes, dgesv_err, label="DGESV Error", marker="o")
# plt.plot(sizes, cyclic_seq_err, label="Cyclic Sequential Error", marker="o")
# plt.plot(sizes, cyclic_par_err, label="Cyclic Parallel Error", marker="o")
# plt.xlabel("Matrix Size")
# plt.ylabel("Error")
# plt.title("Errors")
# plt.legend()
plt.savefig("./results/execution_times.png", dpi=300, bbox_inches="tight")
plt.tight_layout()

