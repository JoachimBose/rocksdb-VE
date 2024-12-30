#!/usr/bin/python3
# import necessary libraries and recreate the data since the execution environment was reset
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# Data for throughput in operations per second
throughput_data = {
   "enginetype": ["io_uring", "libaio", "posix", "io_uring_sqp"],
   "throughput": [44013, 62590, 65467, 30748],
}

# Convert data to DataFrame
throughput_df = pd.DataFrame(throughput_data)

# Create a bar chart for throughput
plt.figure(figsize=(4, 3))
sns.barplot(data=throughput_df, x="enginetype", y="throughput")

# Add titles and labels
plt.title("Throughput of Different Engines")
plt.xlabel("Engine Type")
plt.ylabel("Throughput (ops/sec)")

# Show the plot
plt.tight_layout()
plt.savefig("plot-throughput.pdf")
plt.show()
